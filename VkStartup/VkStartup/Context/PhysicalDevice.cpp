#include "VkStartup/Context/PhysicalDevice.h"
#include "VkStartup/Misc/Exceptions.h"
#include "VkShared/Macros.h"
#include <vector>
#include <map>
#include <cstring>

namespace VkStartup {

PhysicalDevice::PhysicalDevice(VkInstance instance, std::vector<const char*> desired_device_extension,
                               std::vector<const char*> required_device_extensions)
    : m_desired_device_extensions{std::move(desired_device_extension)},
      m_required_device_extensions{std::move(required_device_extensions)}, m_vk_instance{instance} {
}

PhysicalDeviceInfo PhysicalDevice::physical_device_info() {
  if (!m_vk_physical_device) {
    select_physical_device();
  }
  PhysicalDeviceInfo info;
  info.vk_physical_device = m_vk_physical_device;
  info.vk_queue_family_indices = m_queue_indices;
  info.features_to_activate = m_device_features_to_activate;
  info.device_extensions = device_extensions_to_use(m_vk_physical_device);
  return info;
}

void PhysicalDevice::select_physical_device() {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(m_vk_instance, &device_count, nullptr);

  if (device_count == 0) {
    VkError("Unable to locate a physical device");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(m_vk_instance, &device_count, devices.data());

  // Use user defined physical device selection.  If not defined, the default
  // selection will be used
  select_best_physical_device(devices);
  if (!m_vk_physical_device) {
    VkError("Failed to select a physical device based on criteria");
    throw Exceptions::VkStartupException();
  }

  // Display physical device info when debug
  VkPhysicalDeviceProperties properties = {};
  vkGetPhysicalDeviceProperties(m_vk_physical_device, &properties);
  VkInfo("Selected Physical Device: " + std::string{properties.deviceName});

  // Store properties of best selected physical device
  vkGetPhysicalDeviceProperties(m_vk_physical_device, &m_device_properties);

  // Store features to activate later
  set_features_to_activate();

  // Set queue indices
  set_queue_indices();
}

void PhysicalDevice::set_queue_indices() {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_vk_physical_device, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(m_vk_physical_device, &queue_family_count, queue_families.data());

  int i = 0;
  for (const auto& family : queue_families) {
    if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      constexpr auto graphics = VkShared::Enums::QueueFamily::Graphics;
      if (!m_queue_indices.contains(graphics)) {
        m_queue_indices[graphics] = i;
      }
    }
    if (family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      constexpr auto transfer = VkShared::Enums::QueueFamily::Transfer;
      if (!m_queue_indices.contains(transfer)) {
        m_queue_indices[transfer] = i;
      }
    }
    if (family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      constexpr auto compute = VkShared::Enums::QueueFamily::Compute;
      if (!m_queue_indices.contains(compute)) {
        m_queue_indices[compute] = i;
      }
    }
    i++;
  }

  // If no transfer queue exists, use graphics queue
  constexpr auto transfer = VkShared::Enums::QueueFamily::Transfer;
  constexpr auto graphics = VkShared::Enums::QueueFamily::Graphics;
  if (!m_queue_indices.contains(transfer)) {
    if (m_queue_indices.contains(graphics)) {
      m_queue_indices[transfer] = m_queue_indices[graphics];
    }
    m_queue_indices[transfer] = i;
  }
}

bool PhysicalDevice::extension_supported(const std::vector<VkExtensionProperties>& supported,
                                         const char* value_to_check) {
  for (const auto& val : supported) {
    if (strcmp(val.extensionName, value_to_check) == 0) {
      return true;
    }
  }
  return false;
}

std::vector<const char*> PhysicalDevice::device_extensions_to_use(VkPhysicalDevice device) const {
  // Check extensions
  uint32_t extension_count{0};
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> supported_extensions{extension_count};
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, supported_extensions.data());

  // Check required extensions
  std::vector<const char*> extensions;
  for (const auto& value : m_required_device_extensions) {
    if (extension_supported(supported_extensions, value)) {
      extensions.push_back(value);
    } else {
      VkError("Required Device Extension: " + std::string{value} + " is not supported");
      throw Exceptions::VkStartupException();
    }
  }

  // Check desired extensions
  for (const auto& value : m_desired_device_extensions) {
    if (extension_supported(supported_extensions, value)) {
      extensions.push_back(value);
    } else {
      VkWarning("Device Extension: " + std::string{value} + " is not supported");
    }
  }

  return extensions;
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

}  // namespace VkStartup
