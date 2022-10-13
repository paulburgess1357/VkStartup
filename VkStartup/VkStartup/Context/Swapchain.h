#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

namespace VkStartup::Swapchain {

struct SwapchainFormatSupport {
  VkSurfaceCapabilitiesKHR capabilities = {};
  std::vector<VkSurfaceFormatKHR> formats{};
  std::vector<VkPresentModeKHR> present_modes{};
};

struct SwapchainFormatDetails {
  VkSurfaceFormatKHR format = {};
  VkPresentModeKHR present_mode = {};
  VkExtent2D extent = {};
  uint32_t image_count{};
  VkSurfaceTransformFlagBitsKHR pretransform = {};
  VkImageUsageFlags usage_flags{0};
};

[[nodiscard]] inline SwapchainFormatSupport query_swap_support(VkPhysicalDevice device, VkSurfaceKHR surface) {
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

}  // namespace VkStartup::Swapchain
