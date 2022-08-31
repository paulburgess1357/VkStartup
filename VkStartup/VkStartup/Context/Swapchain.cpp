#include "VkStartup/Context/Swapchain.h"
#include "VkShared/Macros.h"

namespace VulkanUtilities::VkStartup {

SwapchainFormatSupport SwapchainFormatSelect::query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapchainFormatSupport swapchain_support;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapchain_support.capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

  if (format_count != 0) {
    swapchain_support.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, swapchain_support.formats.data());
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

  if (present_mode_count != 0) {
    swapchain_support.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count,
                                              swapchain_support.present_modes.data());
  }

  return swapchain_support;
}

SwapchainFormatDetails DefaultSwapchainFormatSelect::select_swapchain_format(
    const SwapchainFormatSupport supported_details) const {
  SwapchainFormatDetails details{};

  // Desired format and color space
  bool desired_format_found{false};
  for (const auto& available_format : supported_details.formats) {
    if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      details.format = available_format;
      desired_format_found = true;
    }
  }

  if (!desired_format_found) {
    VkWarning("Desired swapchain not found.  Defaulting to first supported format and colorspace");
    details.format = supported_details.formats.at(0);
  }

  // Desired presentation mode
  bool desired_presentation_mode_found{false};
  for (const auto& available_present_mode : supported_details.present_modes) {
    if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      details.present_mode = available_present_mode;
      desired_presentation_mode_found = true;
    }
  }

  if (!desired_presentation_mode_found) {
    details.present_mode = VK_PRESENT_MODE_FIFO_KHR;
  }

  return details;

  // Desired swap extent
  // TODO glfw specific here... Should probably move this to the Test class...; GLFWSwapchainFormatSelector...

}

}  // namespace VulkanUtilities::VkStartup
