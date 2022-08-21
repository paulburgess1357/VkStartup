#pragma once
#include "VkStartup/Handle/UsingHandle.h"
#include <VkShared/Enums.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>

namespace VulkanUtilities::VkStartup {

struct PhysicalDeviceInfo {
  VkPhysicalDevice vk_physical_device{VK_NULL_HANDLE};
  std::unordered_map<VkShared::Enums::QueueFamily, uint32_t> vk_queue_family_indices{};
  VkPhysicalDeviceFeatures features_to_activate = {};
  std::vector<const char*> device_extensions = {};
};

class PhysicalDevice {
 public:
  explicit PhysicalDevice(VkInstance instance, std::vector<const char*> desired_device_extension,
                          std::vector<const char*> required_device_extensions);
  virtual ~PhysicalDevice() = default;

  PhysicalDevice(const PhysicalDevice& source) = default;
  PhysicalDevice& operator=(const PhysicalDevice& rhs) = default;
  PhysicalDevice(PhysicalDevice&& source) noexcept = default;
  PhysicalDevice& operator=(PhysicalDevice&& rhs) noexcept = default;

  [[nodiscard]] PhysicalDeviceInfo physical_device_info();

 protected:
  [[nodiscard]] static bool extension_supported(const std::vector<VkExtensionProperties>& supported,
                                                const char* value_to_check);
  [[nodiscard]] std::vector<const char*> device_extensions_to_use(VkPhysicalDevice device) const;

  VkPhysicalDevice m_vk_physical_device{VK_NULL_HANDLE};
  VkPhysicalDeviceFeatures m_device_features_to_activate = {};

  std::vector<const char*> m_desired_device_extensions{};
  std::vector<const char*> m_required_device_extensions{};

 private:
  virtual void select_best_physical_device(const std::vector<VkPhysicalDevice>& devices) = 0;
  virtual void set_features_to_activate() = 0;

  void select_physical_device();
  void set_queue_indices();

  VkInstance m_vk_instance{VK_NULL_HANDLE};

  // Selected device
  VkPhysicalDeviceProperties m_device_properties = {};
  std::unordered_map<VkShared::Enums::QueueFamily, uint32_t> m_queue_indices;
};

// Default implementation of selecting physical device.  This can
// be implemented by the user in their own derived class
class PhysicalDeviceDefault final : public PhysicalDevice {
 public:
  explicit PhysicalDeviceDefault(VkInstance instance, std::vector<const char*> desired_device_extension,
                                 std::vector<const char*> required_device_extensions)
      : PhysicalDevice{instance, std::move(desired_device_extension), std::move(required_device_extensions)} {
  }

 private:
  void select_best_physical_device(const std::vector<VkPhysicalDevice>& devices) override;
  void set_features_to_activate() override;
  [[nodiscard]] static bool device_meets_requirements(VkPhysicalDevice device,
                                                      const VkPhysicalDeviceFeatures& device_features);
};

}  // namespace VulkanUtilities::VkStartup
