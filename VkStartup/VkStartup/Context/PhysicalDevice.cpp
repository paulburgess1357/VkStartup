#include "VkStartup/Context/PhysicalDevice.h"
#include "VkStartup/Misc/Exceptions.h"
#include "VkShared/Macros.h"
#include <vector>
#include <map>
#include <cstring>

namespace VkStartup {

PhysicalDevice::PhysicalDevice(VkInstance instance, std::vector<const char*> desired_device_ext,
                               std::vector<const char*> required_device_ext)
    : m_desired_device_ext{std::move(desired_device_ext)}, m_required_device_ext{std::move(required_device_ext)},
      m_vk_instance{instance} {
}

PhysicalDeviceInfo PhysicalDevice::info() {
  if (!m_vk_physical_device) {
    select_physical_device();
  }

  PhysicalDeviceInfo info;
  info.vk_phy_device = m_vk_physical_device;
  info.vk_queue_family_indices = m_queue_indices;
  info.features_to_activate = m_device_features_to_activate;
  info.device_ext = device_ext_to_use(m_vk_physical_device);
  info.depth_format = m_depth_format;
  info.depth_format_supports_stencil = m_depth_supports_stencil;
  return info;
}

void PhysicalDevice::select_physical_device() {
  // Use user defined physical device selection.  If not defined, the default
  // selection will be used
  const auto devices = physical_devices();
  select_best_physical_device(devices);
  if (!m_vk_physical_device) {
    VkError("Failed to select a physical device based on criteria");
    throw Exceptions::VkStartupException();
  }

  display_physical_device();

  // Store properties of best selected physical device
  vkGetPhysicalDeviceProperties(m_vk_physical_device, &m_device_properties);

  // Store features to activate later
  set_features_to_activate();

  // Store depth format
  set_depth_format();

  // Set queue indices
  set_queue_indices();
}

void PhysicalDevice::display_physical_device() const {
  // Display physical device info when debug
  VkPhysicalDeviceProperties properties = {};
  vkGetPhysicalDeviceProperties(m_vk_physical_device, &properties);
  VkInfo("Selected Physical Device: " + std::string{properties.deviceName});
}

std::vector<VkPhysicalDevice> PhysicalDevice::physical_devices() const {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(m_vk_instance, &device_count, nullptr);

  if (device_count == 0) {
    VkError("Unable to locate a physical device");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(m_vk_instance, &device_count, devices.data());

  return devices;
}

void PhysicalDevice::set_queue_indices() {
  using VkShared::Enums::QueueFamily;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_vk_physical_device, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(m_vk_physical_device, &queue_family_count, queue_families.data());

  int i = 0;
  for (const auto& family : queue_families) {
    if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      if (!m_queue_indices.contains(QueueFamily::Graphics)) {
        m_queue_indices[QueueFamily::Graphics] = i;
      }
    }
    if (family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      if (!m_queue_indices.contains(QueueFamily::Transfer)) {
        m_queue_indices[QueueFamily::Transfer] = i;
      }
    }
    if (family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      if (!m_queue_indices.contains(QueueFamily::Compute)) {
        m_queue_indices[QueueFamily::Compute] = i;
      }
    }
    i++;
  }

  // If no transfer queue exists, use graphics queue
  if (!m_queue_indices.contains(QueueFamily::Transfer)) {
    if (m_queue_indices.contains(QueueFamily::Graphics)) {
      m_queue_indices[QueueFamily::Transfer] = m_queue_indices[QueueFamily::Graphics];
    }
    m_queue_indices[QueueFamily::Transfer] = i;
  }
}

bool PhysicalDevice::ext_supported(const std::vector<VkExtensionProperties>& supported, const char* value_to_check) {
  for (const auto& [extensionName, specVersion] : supported) {
    if (strcmp(extensionName, value_to_check) == 0) {
      return true;
    }
  }
  return false;
}

std::vector<const char*> PhysicalDevice::device_ext_to_use(VkPhysicalDevice device) const {
  // Check extensions
  uint32_t extension_count{0};
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> supported_extensions{extension_count};
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, supported_extensions.data());

  // Check required extensions
  std::vector<const char*> extensions;
  for (const auto& value : m_required_device_ext) {
    if (ext_supported(supported_extensions, value)) {
      extensions.push_back(value);
    } else {
      VkError("Required Device Extension: " + std::string{value} + " is not supported");
      throw Exceptions::VkStartupException();
    }
  }

  // Check desired extensions
  for (const auto& value : m_desired_device_ext) {
    if (ext_supported(supported_extensions, value)) {
      extensions.push_back(value);
    } else {
      VkWarning("Device Extension: " + std::string{value} + " is not supported");
    }
  }

  return extensions;
}

PhysicalDeviceDefault::PhysicalDeviceDefault(VkInstance instance, std::vector<const char*> desired_device_ext,
                                             std::vector<const char*> required_device_ext)
    : PhysicalDevice{instance, std::move(desired_device_ext), std::move(required_device_ext)} {
}

void PhysicalDeviceDefault::select_best_physical_device(const std::vector<VkPhysicalDevice>& devices) {
  std::multimap<int, VkPhysicalDevice> candidates;

  for (const auto& device : devices) {
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(device, &properties);

    VkPhysicalDeviceFeatures features = {};
    vkGetPhysicalDeviceFeatures(device, &features);

    // Rank devices
    int score = 0;
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score += 1000;
    }

    // Drop llvmpipe
    if (strstr(properties.deviceName, "llvmpipe")) {
      score -= 100000;
    }

    score += static_cast<int>(properties.limits.maxImageDimension2D);
    if (device_meets_requirements(device, features)) {
      candidates.insert(std::make_pair(score, device));
    }
  }

  if (!candidates.empty()) {
    m_vk_physical_device = candidates.rbegin()->second;
  }
}

bool PhysicalDeviceDefault::device_meets_requirements(VkPhysicalDevice device,
                                                      const VkPhysicalDeviceFeatures& device_features) {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  bool meets_queue_critiera{false};
  for (const auto& queueFamily : queue_families) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      meets_queue_critiera = true;
      break;
    }
  }

  bool meets_feature_criteria{false};
  if (device_features.geometryShader) {
    meets_feature_criteria = true;
  }

  return meets_queue_critiera && meets_feature_criteria;
}

void PhysicalDeviceDefault::set_features_to_activate() {
  m_device_features_to_activate.geometryShader = VK_TRUE;
}

void PhysicalDeviceDefault::set_depth_format() {
  const std::vector formats{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
  constexpr VkImageTiling tiling{VK_IMAGE_TILING_OPTIMAL};
  constexpr VkFormatFeatureFlags feature_flags{VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT};

  for (const auto format : formats) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(m_vk_physical_device, format, &props);

    // ReSharper disable once CppRedundantBooleanExpressionArgument
    if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & feature_flags) == feature_flags) {
      m_depth_format = format;
      break;
    }

    // ReSharper disable once CppUnreachableCode
    if constexpr (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & feature_flags) == feature_flags) {
      m_depth_format = format;
      break;
    }
  }

  // Assert suitable depth format located
  VkAssert(m_depth_format != VK_FORMAT_UNDEFINED, "Unable to select a suitable depth format");

  // Indicate is selected depth format supports stencil
  m_depth_supports_stencil = m_depth_format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
                             m_depth_format == VK_FORMAT_D24_UNORM_S8_UINT;
}

}  // namespace VkStartup
