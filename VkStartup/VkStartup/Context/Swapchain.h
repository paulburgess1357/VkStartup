#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

namespace VulkanUtilities::VkStartup {

struct SwapchainFormatSupport {
  VkSurfaceCapabilitiesKHR capabilities = {};
  std::vector<VkSurfaceFormatKHR> formats{};
  std::vector<VkPresentModeKHR> present_modes{};
};

struct SwapchainFormatDetails {
  VkSurfaceFormatKHR format = {};
  VkPresentModeKHR present_mode = {};
  VkExtent2D extent = {};
};

class SwapchainFormatSelect {
 public:
  virtual ~SwapchainFormatSelect() = default;

  SwapchainFormatSelect(const SwapchainFormatSelect& source) = default;
  SwapchainFormatSelect& operator=(const SwapchainFormatSelect& rhs) = default;
  SwapchainFormatSelect(SwapchainFormatSelect&& source) noexcept = default;
  SwapchainFormatSelect& operator=(SwapchainFormatSelect&& rhs) noexcept = default;

  [[nodiscard]] virtual SwapchainFormatDetails select_swapchain_format(
      const SwapchainFormatSupport supported_formats) const = 0;

 private:
  [[nodiscard]] static SwapchainFormatSupport query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface);
};

// User defined swapchain format details
class DefaultSwapchainFormatSelect final : public SwapchainFormatSelect {
public:
  DefaultSwapchainFormatSelect() = default;
  [[nodiscard]] SwapchainFormatDetails select_swapchain_format(const SwapchainFormatSupport supported_details) const override;
};

}  // namespace VulkanUtilities::VkStartup
