#pragma once
#include "VkStartup/Context/Context.h"
#include "VkStartup/Context/PhysicalDevice.h"
#include "VkStartup/Context/SurfaceLoader.h"
#include <vector>
#include <unordered_set>
#include <type_traits>
#include <memory>

namespace VkStartup {

struct InitContextOptions {
  // Instance
  uint32_t api_version{VK_API_VERSION_1_0};
  std::vector<const char*> required_instance_ext{};
  std::vector<const char*> desired_instance_ext{};
  std::vector<const char*> required_layers{};
  std::vector<const char*> desired_layers{};
  bool enable_validation{false};

  // Device
  std::vector<const char*> required_device_ext{};
  std::vector<const char*> desired_device_ext{};

  // User defined physical device criteria.
  std::unique_ptr<PhysicalDevice> phy_device_criteria{};

  // User defined surfaces creation (SDL, GLFW, etc.); Multiple surfaces can be drawn to:
  std::vector<std::unique_ptr<SurfaceLoader>> surface_loaders;
};

class InitContext {
 public:
  explicit InitContext(InitContextOptions options) : m_opt{std::move(options)} {
    init();
  }
  [[nodiscard]] VkContext& context();

 private:
  void init();
  void init_instance();
  void init_physical_device();
  void init_logical_device();
  void init_queue_handles();
  void init_surfaces();
  void init_swapchain();
  void init_presentation();
  void init_vma();

  // Extension
  [[nodiscard]] static std::vector<VkExtensionProperties> extension_properties();
  [[nodiscard]] std::vector<const char*> extensions_to_load(
      const std::vector<VkExtensionProperties>& supported_extensions) const;
  [[nodiscard]] static bool extension_supported(const std::vector<VkExtensionProperties>& supported,
                                                const char* value_to_check);

  // Layers
  [[nodiscard]] static std::vector<VkLayerProperties> layer_properties();
  [[nodiscard]] std::vector<const char*> layers_to_load(const std::vector<VkLayerProperties>& supported_layers) const;
  [[nodiscard]] static bool layer_supported(const std::vector<VkLayerProperties>& supported,
                                            const char* value_to_check);

  void add_validation_requirements(std::vector<const char*>& extensions,
                                   const std::vector<VkExtensionProperties>& supported_extensions,
                                   std::vector<const char*>& layers,
                                   const std::vector<VkLayerProperties>& supported_layers);

  [[nodiscard]] inline std::vector<uint32_t> unique_queues() const;

  InitContextOptions m_opt;
  VkContext m_ctx;
};

}  // namespace VkStartup
