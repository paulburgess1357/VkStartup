#include "VkStartup/Context/InitContext.h"
#include "VkStartup/Handle/UsingHandle.h"
#include "VkStartup/Misc/Exceptions.h"
#include "VkStartup/Misc/CreateInfo.h"
#include "VkShared/Macros.h"
#include <memory>
#include <unordered_set>
#include <cstring>

namespace VulkanUtilities::VkStartup {
void InitContext::init() {
  VkTrace("Running VkStartup");
  init_instance();
  init_physical_device();
  init_logical_device();
  init_queue_handles();
  init_surface();
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
    const auto val_layer_name = "VK_LAYER_KHRONOS_validation";
    const auto debug_ext_name = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    if (layer_supported(supported_layers, val_layer_name) &&
        extension_supported(supported_extensions, debug_ext_name)) {
      layers.push_back(val_layer_name);
      extensions.push_back(debug_ext_name);
    } else {
      m_options.enable_validation = false;
      VkWarning("Validation or debug dextension not supported");
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
  m_context.instance = std::make_unique<VkInstanceHandle>(create_info);
  m_context.vk_instance = m_context.instance->handle();

  // Enable full debugging if its included in layers
  if (m_options.enable_validation) {
    m_context.debugger = std::make_unique<VkDebugger>(m_context.vk_instance);
  }
}

void InitContext::init_physical_device() {
  if (m_options.custom_physical_device_criteria) {
    // User defined physical device selection
    m_context.physical_device_info = m_options.custom_physical_device_criteria->physical_device_info();
  } else {
    // Default physical device selection
    PhysicalDeviceDefault default_physical_device{m_context.vk_instance, m_options.desired_device_extensions,
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
    // ReSharper disable once CppUseStructuredBinding
    auto queue_create_info = CreateInfo::vk_device_queue_create_info();
    queue_create_info.queueFamilyIndex = index;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  // Create logical device
  // ReSharper disable once CppUseStructuredBinding
  auto logical_create_info = CreateInfo::vk_device_create_info();
  logical_create_info.pQueueCreateInfos = queue_create_infos.data();
  logical_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
  logical_create_info.pEnabledFeatures = &m_context.physical_device_info.features_to_activate;
  logical_create_info.enabledLayerCount = static_cast<uint32_t>(m_options.required_layers.size());
  logical_create_info.ppEnabledLayerNames = m_options.required_layers.data();

  logical_create_info.enabledExtensionCount = static_cast<uint32_t>(
      m_context.physical_device_info.device_extensions.size());
  logical_create_info.ppEnabledExtensionNames = m_context.physical_device_info.device_extensions.data();

  m_context.device = std::make_unique<VkDeviceHandle>(logical_create_info,
                                                      m_context.physical_device_info.vk_physical_device);
  m_context.vk_device = m_context.device->handle();
}

void InitContext::init_queue_handles() {
  for (const auto& [family, family_index] : m_context.physical_device_info.vk_queue_family_indices) {
    if (!m_context.vk_queues.contains(family)) {
      VkQueue queue{VK_NULL_HANDLE};
      // Only one queue per family is being used (hence the 0).
      vkGetDeviceQueue(m_context.vk_device, family_index, 0, &queue);
      if (!queue) {
        VkError("Unable to create queue handle");
        throw Exceptions::VkStartupException();
      }
      m_context.vk_queues[family] = QueueIndexHandle{family_index, queue};
    }
  }
}

void InitContext::init_surface() {
  if (m_options.custom_surface_loader) {
    m_options.custom_surface_loader->init(m_context.vk_instance);
    m_context.surface_loader = std::move(m_options.custom_surface_loader);
  } else {
    VkWarning("No Surface loader supplied.  Vulkan will be initialized without a surface for drawing");
  }
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

}  // namespace VulkanUtilities::VkStartup
