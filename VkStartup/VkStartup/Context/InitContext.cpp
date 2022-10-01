#include "VkStartup/Context/InitContext.h"
#include "VkStartup/Handle/UsingHandle.h"
#include "VkStartup/Misc/Exceptions.h"
#include "VkStartup/Misc/CreateInfo.h"
#include "VkStartup/Context/Queues.h"
#include "VkShared/Macros.h"
#include <memory>
#include <unordered_set>
#include <cstring>
#include <algorithm>

namespace VkStartup {
void InitContext::init() {
  VkTrace("Running VkStartup");
  init_instance();
  init_physical_device();
  init_logical_device();
  init_queue_handles();
  init_surfaces();
  init_swapchain();
  init_vma();
}

void InitContext::init_instance() {
  // Extensions
  uint32_t extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> supported_extensions{extension_count};
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, supported_extensions.data());
  VkInstanceCreateFlags instance_flags{0};

  // MacOS Portability
#ifdef __APPLE__
  m_options.required_instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  instance_flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  // Check required extensions
  std::vector<const char*> extensions;
  for (const auto& value : m_options.required_instance_extensions) {
    if (extension_supported(supported_extensions, value)) {
      extensions.push_back(value);
    } else {
      VkError("Extension: " + std::string{value} + " is not supported");
      throw Exceptions::VkStartupException();
    }
  }

  // Check desired extensions
  for (const auto& value : m_options.desired_instance_extensions) {
    if (extension_supported(supported_extensions, value)) {
      extensions.push_back(value);
    } else {
      VkWarning("Extension: " + std::string{value} + " is not supported");
    }
  }

  // Layers
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> supported_layers{layer_count};
  vkEnumerateInstanceLayerProperties(&layer_count, supported_layers.data());

  // Check required layers
  std::vector<const char*> layers;
  for (const auto& value : m_options.required_layers) {
    if (layer_supported(supported_layers, value)) {
      layers.push_back(value);
    } else {
      VkError("Layer: " + std::string{value} + " is not supported");
      throw Exceptions::VkStartupException();
    }
  }

  // Check desired layers
  for (const auto& value : m_options.desired_layers) {
    if (layer_supported(supported_layers, value)) {
      layers.push_back(value);
    } else {
      VkWarning("Layer: " + std::string{value} + " is not supported");
    }
  }

  if (m_options.enable_validation) {
    const auto debug_ext_name = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    if (const auto val_layer_name = "VK_LAYER_KHRONOS_validation";
        layer_supported(supported_layers, val_layer_name) &&
        extension_supported(supported_extensions, debug_ext_name)) {
      layers.push_back(val_layer_name);
      extensions.push_back(debug_ext_name);
    } else {
      m_options.enable_validation = false;
      VkWarning("Validation or debug extension not supported");
    }
  }

  // Update layers in context options as it needs to be
  // known when the logical device is created.  Extensions
  // for instance / physical device can be different, so we
  // keep don't overwrite instance extensions here.
  m_options.required_layers = layers;

  // Create instance
  VkApplicationInfo app_info = CreateInfo::vk_application_info();
  app_info.pApplicationName = "Vulkan";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "Vulkan Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = m_options.api_version;

  VkInstanceCreateInfo create_info = CreateInfo::vk_instance_create_info();
  create_info.pApplicationInfo = &app_info;
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();
  create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
  create_info.ppEnabledLayerNames = layers.data();
  create_info.flags = instance_flags;

  // Debug instance creation
  const auto instance_debug = VkDebugger::instance_debug_create_info();
  if (m_options.enable_validation) {
    create_info.pNext = &instance_debug;
  }

  // Create instance
  m_context.instance = VkInstanceHandle{create_info};

  // Enable full debugging if its included in layers
  if (m_options.enable_validation) {
    m_context.debugger = std::make_unique<VkDebugger>(m_context.instance());
  }
}

void InitContext::init_physical_device() {
  // Swapchain support is only required if a surface loader exists:
  if (!m_options.custom_surface_loaders.empty()) {
    // Check required and optional.  Technically its required
    // here, however its possible the user forgot:
    const std::string swap_extension{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    if (std::ranges::find(m_options.desired_device_extensions, swap_extension) ==
            m_options.desired_device_extensions.end() &&
        std::ranges::find(m_options.required_device_extensions, swap_extension) ==
            m_options.required_device_extensions.end()) {
      m_options.required_device_extensions.emplace_back(swap_extension.c_str());
    }
  }

  if (m_options.custom_physical_device_criteria) {
    // User defined physical device selection
    m_context.physical_device_info = m_options.custom_physical_device_criteria->physical_device_info();
  } else {
    // Default physical device selection
    PhysicalDeviceDefault default_physical_device{m_context.instance(), m_options.desired_device_extensions,
                                                  m_options.required_device_extensions};
    m_context.physical_device_info = default_physical_device.physical_device_info();
  }
}

void InitContext::init_logical_device() {
  // Populate queue family create info for each unique queue family
  std::unordered_set<uint32_t> unique_family_indices;
  for (const auto& [family, family_index] : m_context.physical_device_info.vk_queue_family_indices) {
    unique_family_indices.insert(family_index);
  }

  // Create device queue create info's for each unique queue
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  queue_create_infos.reserve(unique_family_indices.size());

  constexpr float queue_priority{1.0f};
  for (const auto& index : unique_family_indices) {
    auto queue_create_info = CreateInfo::vk_device_queue_create_info();
    queue_create_info.queueFamilyIndex = index;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  // Create logical device
  auto logical_create_info = CreateInfo::vk_device_create_info();
  logical_create_info.pQueueCreateInfos = queue_create_infos.data();
  logical_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
  logical_create_info.pEnabledFeatures = &m_context.physical_device_info.features_to_activate;
  logical_create_info.enabledLayerCount = static_cast<uint32_t>(m_options.required_layers.size());
  logical_create_info.ppEnabledLayerNames = m_options.required_layers.data();

  logical_create_info.enabledExtensionCount = static_cast<uint32_t>(
      m_context.physical_device_info.device_extensions.size());
  logical_create_info.ppEnabledExtensionNames = m_context.physical_device_info.device_extensions.data();

  m_context.device = VkDeviceHandle{logical_create_info, m_context.physical_device_info.vk_physical_device};
}

void InitContext::init_queue_handles() {
  for (const auto& [family, family_index] : m_context.physical_device_info.vk_queue_family_indices) {
    if (!m_context.vk_queues.contains(family)) {
      VkQueue queue{VK_NULL_HANDLE};
      // Only one queue per family is being used (hence the 0).
      vkGetDeviceQueue(m_context.device(), family_index, 0, &queue);
      if (!queue) {
        VkError("Unable to create queue handle");
        throw Exceptions::VkStartupException();
      }
      m_context.vk_queues[family] = Queues::QueueIndexHandle{family_index, queue};
    }
  }
}

void InitContext::init_surfaces() {
  if (!m_options.custom_surface_loaders.empty()) {
    for (auto& surface_loader : m_options.custom_surface_loaders) {
      surface_loader->init(m_context.instance());
      m_context.swapchain_context[surface_loader->id()].surface_loader = std::move(surface_loader);
    }
  } else {
    VkWarning("No Surface loader supplied.  Vulkan will be initialized without a surface for drawing");
  }
}

void InitContext::init_swapchain() {
  // Initialize swapchain
  if (!m_context.swapchain_context.empty()) {
    for (auto& [window_id, swapchain_data] : m_context.swapchain_context) {
      // Supported swapchain details.  This is dependent on the physical device.  The user
      // takes these details and uses them to determine what format they want for their swapchain
      const auto supported_swapchain_details = Swapchain::query_swapchain_support(
          m_context.physical_device_info.vk_physical_device, swapchain_data.surface_loader->surface());

      // Swapchain creation details (likely the same for all windows but not required)
      const auto selected_swapchain_details = swapchain_data.surface_loader->select_swapchain_format(
          supported_swapchain_details);
      swapchain_data.swapchain_format_details = selected_swapchain_details;

      // Initialize the swapchain using 'selected_swapchain_details'
      const auto unique_queues_vec = Queues::unique_queues(m_context.vk_queues);  // Sharing mode
      auto info = CreateInfo::vk_swapchain_create_info(unique_queues_vec);
      info.surface = swapchain_data.surface_loader->surface();
      info.minImageCount = selected_swapchain_details.image_count;
      info.imageFormat = selected_swapchain_details.format.format;
      info.imageColorSpace = selected_swapchain_details.format.colorSpace;
      info.imageExtent = selected_swapchain_details.extent;
      info.preTransform = selected_swapchain_details.pretransform;
      info.presentMode = selected_swapchain_details.present_mode;
      info.clipped = VK_TRUE;
      info.imageArrayLayers = 1;
      info.imageUsage = selected_swapchain_details.usage_flags;
      info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
      info.oldSwapchain = swapchain_data.swapchain();

      swapchain_data.swapchain = VkSwapchainHandle{info, m_context.device()};

      // Set swapchain images
      // Count is required because 'min image count' above is a request that isn't guarenteed
      uint32_t swap_image_count{};
      vkGetSwapchainImagesKHR(m_context.device(), swapchain_data.swapchain(), &swap_image_count, nullptr);
      swapchain_data.rp_buffers.vk_images.resize(swap_image_count);

      auto& [width, height, vk_images, image_views, renderpass, framebuffer] = swapchain_data.rp_buffers;
      width = info.imageExtent.width;
      height = info.imageExtent.height;
      vkGetSwapchainImagesKHR(m_context.device(), swapchain_data.swapchain(), &swap_image_count, vk_images.data());

      // Set swapchain image views
      for (size_t i = 0; i < swap_image_count; i++) {
        auto image_view_info = CreateInfo::vk_image_view_create_info(vk_images[i]);
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = selected_swapchain_details.format.format;
        image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.levelCount = 1;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.layerCount = 1;
        image_views.emplace_back(image_view_info, m_context.device());
      }
    }
  }
}

void InitContext::init_vma() {
  auto info = CreateInfo::vma_allocator_info(m_context.instance(), m_context.device(),
                                             m_context.physical_device_info.vk_physical_device, m_options.api_version);
  m_context.mem_alloc = VmaAllocatorHandle{info};
}

bool InitContext::extension_supported(const std::vector<VkExtensionProperties>& supported, const char* value_to_check) {
  for (const auto& val : supported) {
    if (strcmp(val.extensionName, value_to_check) == 0) {
      return true;
    }
  }
  return false;
}

bool InitContext::layer_supported(const std::vector<VkLayerProperties>& supported, const char* value_to_check) {
  for (const auto& val : supported) {
    if (strcmp(val.layerName, value_to_check) == 0) {
      return true;
    }
  }
  return false;
}

const VkContext& InitContext::context() const {
  return m_context;
}

}  // namespace VkStartup
