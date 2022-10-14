#include "VkStartup/Context/InitContext.h"
#include "VkStartup/Handle/UsingHandle.h"
#include "VkStartup/Misc/Exceptions.h"
#include "VkStartup/Misc/CreateInfo.h"
#include "VkShared/Macros.h"
#include <memory>
#include <unordered_set>
#include <vector>
#include <cstring>
#include <algorithm>
#include <type_traits>

namespace VkStartup {

InitContext::InitContext(InitContextOptions options) : m_opt{std::move(options)} {
  init();
}

void InitContext::init() {
  VkTrace("Running VkStartup");
  init_instance();
  init_physical_device();
  init_logical_device();
  init_queue_handles();
  init_surfaces();
  init_presentation();
  init_swapchain();
  init_vma();
}

void InitContext::init_instance() {
  VkInstanceCreateFlags instance_flags{0};

  // MacOS Portability
#ifdef __APPLE__
  m_opt.required_instance_ext.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  instance_flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  // Extensions
  const auto supported_ext = ext_properties();
  auto ext = ext_to_load(supported_ext);

  // Layers
  const auto supported_layers = layer_properties();
  auto layers = layers_to_load(supported_layers);

  // Check and add validation
  add_validation_requirements(ext, supported_ext, layers, supported_layers);

  // Update layers in context options as it needs to be
  // known when the logical device is created.  Extensions
  // for instance / physical device can be different, so we
  // keep don't overwrite instance extensions here.
  m_opt.required_layers = layers;

  // Create instance
  const VkApplicationInfo app_info = CreateInfo::vk_application_info(VK_MAKE_VERSION(1, 0, 0), VK_MAKE_VERSION(1, 0, 0),
                                                                     m_opt.api_version);
  VkInstanceCreateInfo create_info = CreateInfo::vk_instance_create_info(ext, layers, instance_flags, app_info);

  // Debug instance creation
  const auto instance_debug = VkDebugger::instance_debug_create_info();
  if (m_opt.enable_validation) {
    create_info.pNext = &instance_debug;
  }

  // Create instance
  m_ctx.instance = VkInstanceHandle{create_info};

  // Enable full debugging if its included in layers
  if (m_opt.enable_validation) {
    m_ctx.debugger = std::make_unique<VkDebugger>(m_ctx.instance());
  }
}

void InitContext::init_physical_device() {
  // Swapchain support is only required if a surface loader exists:
  if (!m_opt.surface_loaders.empty()) {
    const std::string swap_ext{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    if (std::ranges::find(m_opt.desired_device_ext, swap_ext) == m_opt.desired_device_ext.end() &&
        std::ranges::find(m_opt.required_device_ext, swap_ext) == m_opt.required_device_ext.end()) {
      m_opt.required_device_ext.emplace_back(swap_ext.c_str());
    }
  }

  // User defined physical device selection or default:
  if (m_opt.phy_device_criteria) {
    m_ctx.phy_device_info = m_opt.phy_device_criteria->info();
  } else {
    PhysicalDeviceDefault phy_device{m_ctx.instance(), m_opt.desired_device_ext, m_opt.required_device_ext};
    m_ctx.phy_device_info = phy_device.info();
  }
}

void InitContext::init_logical_device() {
  auto& [vk_physical_device, vk_queue_family_indices, features_to_activate, device_extensions] = m_ctx.phy_device_info;

  // Populate queue family create info for each unique queue family
  std::unordered_set<uint32_t> unique_family_indices;
  for (const auto& [family, family_index] : vk_queue_family_indices) {
    unique_family_indices.insert(family_index);
  }

  // Create device queue info for each unique queue
  std::vector<VkDeviceQueueCreateInfo> all_queue_info;
  all_queue_info.reserve(unique_family_indices.size());

  constexpr float queue_priority{1.0f};
  for (const auto& index : unique_family_indices) {
    auto queue_info = CreateInfo::vk_device_queue_create_info(index);
    queue_info.pQueuePriorities = &queue_priority;
    all_queue_info.push_back(queue_info);
  }

  // Create logical device
  auto logical_info = CreateInfo::vk_device_create_info(all_queue_info, features_to_activate, device_extensions,
                                                        m_opt.required_layers);
  m_ctx.device = VkDeviceHandle{logical_info, vk_physical_device};
}

void InitContext::init_queue_handles() {
  for (const auto& [family, family_index] : m_ctx.phy_device_info.vk_queue_family_indices) {
    if (!m_ctx.queues.contains(family)) {
      QueueIndexHandle queue{family_index, VK_NULL_HANDLE};
      // Only one queue per family is being used (hence the 0).
      vkGetDeviceQueue(m_ctx.device(), family_index, 0, &queue.handle);
      if (!queue.handle) {
        VkError("Unable to create queue handle");
        throw Exceptions::VkStartupException();
      }
      m_ctx.queues[family] = queue;
    }
  }
}

void InitContext::init_surfaces() {
  if (!m_opt.surface_loaders.empty()) {
    for (auto& surface_loader : m_opt.surface_loaders) {
      surface_loader->init(m_ctx.instance());
      m_ctx.swap_ctx[surface_loader->id()].surface_loader = std::move(surface_loader);
    }
  } else {
    VkWarning("No Surface loader supplied.  Vulkan will be initialized without a surface for drawing");
  }
}

void InitContext::init_presentation() {
  if (!m_ctx.swap_ctx.empty()) {
    for (auto& [id, swap_ctx] : m_ctx.swap_ctx) {
      const auto vk_physical_device = m_ctx.phy_device_info.vk_phy_device;

      uint32_t queue_family_count = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &queue_family_count, nullptr);

      VkBool32 present_support = false;
      for (uint32_t i = 0; i < queue_family_count; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(vk_physical_device, i, swap_ctx.surface_loader->surface(),
                                             &present_support);
        if (present_support) {
          swap_ctx.present_queue.family_index = i;
          vkGetDeviceQueue(m_ctx.device(), i, 0, &swap_ctx.present_queue.handle);
          break;
        }
      }
      if (!present_support) {
        VkWarning("No presentation queue found for the swapchain surface id: " + id);
      }
    }
  }
}

void InitContext::init_swapchain() {
  // Initialize swapchain
  if (!m_ctx.swap_ctx.empty()) {
    for (auto& [id, swap_ctx] : m_ctx.swap_ctx) {
      // Supported swapchain details based on the user defined physical device selection
      const auto supported_swap_details = Swapchain::query_swap_support(m_ctx.phy_device_info.vk_phy_device,
                                                                        swap_ctx.surface_loader->surface());

      // Swapchain creation details (likely the same for all windows but not required)
      swap_ctx.swap_format_details = swap_ctx.surface_loader->select_swapchain_format(supported_swap_details);
      const auto& [format, present_mode, extent, image_count, pretransform, usage_flags] = swap_ctx.swap_format_details;

      // Initialize the swapchain using 'selected_swapchain_details'
      const auto unique_queues_vec = unique_queues();  // Sharing mode
      auto info = CreateInfo::vk_swapchain_create_info(unique_queues_vec);
      info.minImageCount = image_count;
      info.imageFormat = format.format;
      info.imageColorSpace = format.colorSpace;
      info.imageExtent = extent;
      info.preTransform = pretransform;
      info.presentMode = present_mode;
      info.imageUsage = usage_flags;
      info.surface = swap_ctx.surface_loader->surface();
      info.oldSwapchain = swap_ctx.swapchain();
      info.clipped = VK_TRUE;
      info.imageArrayLayers = 1;
      info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
      swap_ctx.swapchain = VkSwapchainHandle{info, m_ctx.device()};

      // Set swapchain images
      // Count is required because 'min image count' above is a request that isn't guarenteed
      uint32_t img_count{0};
      vkGetSwapchainImagesKHR(m_ctx.device(), swap_ctx.swapchain(), &img_count, nullptr);
      swap_ctx.rp_buffers.vk_images.resize(img_count);

      auto& [width, height, renderpass, vk_imgs, img_views, framebuffers] = swap_ctx.rp_buffers;
      width = info.imageExtent.width;
      height = info.imageExtent.height;
      vkGetSwapchainImagesKHR(m_ctx.device(), swap_ctx.swapchain(), &img_count, vk_imgs.data());

      // Set image views
      img_views.clear();  // Handle remakes
      for (size_t i = 0; i < img_count; i++) {
        auto image_view_info = CreateInfo::vk_image_view_create_info(vk_imgs[i]);
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = format.format;
        image_view_info.components = VkComponentMapping{VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                                                        VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
        image_view_info.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        img_views.emplace_back(image_view_info, m_ctx.device());
      }
    }
  }
}

void InitContext::remake_swapchain() {
  init_swapchain();
}

void InitContext::init_vma() {
  auto info = CreateInfo::vma_allocator_info(m_ctx.instance(), m_ctx.device(), m_ctx.phy_device_info.vk_phy_device,
                                             m_opt.api_version);
  m_ctx.mem_alloc = VmaAllocatorHandle{info};
}

std::vector<const char*> InitContext::ext_to_load(const std::vector<VkExtensionProperties>& supported_ext) const {
  // Check required extensions
  std::vector<const char*> extensions;
  for (const auto& value : m_opt.required_instance_ext) {
    if (ext_supported(supported_ext, value)) {
      extensions.push_back(value);
    } else {
      VkError("Extension: " + std::string{value} + " is not supported");
      throw Exceptions::VkStartupException();
    }
  }

  // Check desired extensions
  for (const auto& value : m_opt.desired_instance_ext) {
    if (ext_supported(supported_ext, value)) {
      extensions.push_back(value);
    } else {
      VkWarning("Extension: " + std::string{value} + " is not supported");
    }
  }

  return extensions;
}

std::vector<VkExtensionProperties> InitContext::ext_properties() {
  uint32_t ext_count{0};
  vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, nullptr);
  std::vector<VkExtensionProperties> ext{ext_count};
  vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, ext.data());
  return ext;
}

bool InitContext::ext_supported(const std::vector<VkExtensionProperties>& supported, const char* value_to_check) {
  for (const auto& [extensionName, specVersion] : supported) {
    if (strcmp(extensionName, value_to_check) == 0) {
      return true;
    }
  }
  return false;
}

std::vector<VkLayerProperties> InitContext::layer_properties() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> layers{layer_count};
  vkEnumerateInstanceLayerProperties(&layer_count, layers.data());
  return layers;
}

bool InitContext::layer_supported(const std::vector<VkLayerProperties>& supported, const char* value_to_check) {
  for (const auto& [layerName, specVersion, implementationVersion, description] : supported) {
    if (strcmp(layerName, value_to_check) == 0) {
      return true;
    }
  }
  return false;
}

std::vector<const char*> InitContext::layers_to_load(const std::vector<VkLayerProperties>& supported_layers) const {
  // Check required layers
  std::vector<const char*> layers;
  for (const auto& value : m_opt.required_layers) {
    if (layer_supported(supported_layers, value)) {
      layers.push_back(value);
    } else {
      VkError("Layer: " + std::string{value} + " is not supported");
      throw Exceptions::VkStartupException();
    }
  }

  // Check desired layers
  for (const auto& value : m_opt.desired_layers) {
    if (layer_supported(supported_layers, value)) {
      layers.push_back(value);
    } else {
      VkWarning("Layer: " + std::string{value} + " is not supported");
    }
  }

  return layers;
}

void InitContext::add_validation_requirements(std::vector<const char*>& ext,
                                              const std::vector<VkExtensionProperties>& supported_ext,
                                              std::vector<const char*>& layers,
                                              const std::vector<VkLayerProperties>& supported_layers) {
  if (m_opt.enable_validation) {
    const auto debug_ext_name = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    if (const auto val_layer_name = "VK_LAYER_KHRONOS_validation";
        layer_supported(supported_layers, val_layer_name) && ext_supported(supported_ext, debug_ext_name)) {
      layers.push_back(val_layer_name);
      ext.push_back(debug_ext_name);
    } else {
      m_opt.enable_validation = false;
      VkWarning("Validation or debug extension not supported");
    }
  }
}

std::vector<uint32_t> InitContext::unique_queues() const {
  // Check other family queues (based on the VkPhysicalDevice queues)
  std::unordered_set<uint32_t> unique_queues;

  // Standard queues
  for (const auto& [family, queue] : m_ctx.queues) {
    unique_queues.insert(queue.family_index);
  }

  // Swapchain queues (can vary if multiple surfaces)
  for (const auto& [id, swap_ctx] : m_ctx.swap_ctx) {
    unique_queues.insert(swap_ctx.present_queue.family_index);
  }

  return {unique_queues.begin(), unique_queues.end()};
}

VkContext& InitContext::context() {
  return m_ctx;
}

}  // namespace VkStartup
