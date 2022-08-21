#pragma once
#include "VkStartup/Context/Context.h"
#include "VkStartup/Context/PhysicalDevice.h"
#include <vector>
#include <unordered_set>

namespace VulkanUtilities::VkStartup {

struct InitContextOptions {
  // Instance
  uint32_t api_version{VK_API_VERSION_1_0};
  std::vector<const char*> required_extensions{};
  std::vector<const char*> desired_extensions{};
  std::vector<const char*> required_layers{};
  std::vector<const char*> desired_layers{};
  bool enable_validation{false};

  // Device
  std::vector<const char*> required_device_extensions{};
  std::vector<const char*> desired_device_extensions{};

  // User physical device criteria.
  std::shared_ptr<PhysicalDevice> custom_physical_device_criteria{};
};

class InitContext {
 public:
  explicit InitContext(InitContextOptions options) : m_options{std::move(options)} {
    init();
  }

 private:
  void init();
  void init_instance();
  void init_physical_device();
  void init_logical_device();
  void init_queue_handles();

  [[nodiscard]] static bool extension_supported(const std::vector<VkExtensionProperties>& supported,
                                                const char* value_to_check);
  [[nodiscard]] static bool layer_supported(const std::vector<VkLayerProperties>& supported,
                                            const char* value_to_check);
  template <typename T>
  [[nodiscard]] static std::vector<T> remove_duplicates(const std::vector<T>& input) {
    std::unordered_set<T> input_set(input.begin(), input.end());
    std::vector<T> unique_vector(input_set.begin(), input_set.end());
    return unique_vector;
  }

  InitContextOptions m_options;
  VkContext m_context;
};

}  // namespace VulkanUtilities::VkStartup
