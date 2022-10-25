#pragma once
#include "VkShared/MemAlloc.h"
#include <vulkan/vulkan_core.h>
#include <vector>

namespace VkStartup::CreateInfo {

[[nodiscard]] inline VkApplicationInfo vk_application_info(const uint32_t app_version, const uint32_t engine_version,
                                                           const uint32_t api_version) {
  VkApplicationInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  info.pApplicationName = "Vulkan";
  info.applicationVersion = app_version;
  info.pEngineName = "Vulkan Engine";
  info.engineVersion = engine_version;
  info.apiVersion = api_version;
  return info;
}

[[nodiscard]] inline VkInstanceCreateInfo vk_instance_create_info(const std::vector<const char*>& extensions,
                                                                  const std::vector<const char*>& layers,
                                                                  const VkInstanceCreateFlags flags,
                                                                  const VkApplicationInfo& app_info) {
  VkInstanceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  info.ppEnabledExtensionNames = extensions.data();
  info.enabledLayerCount = static_cast<uint32_t>(layers.size());
  info.ppEnabledLayerNames = layers.data();
  info.pApplicationInfo = &app_info;
  info.flags = flags;
  return info;
}

[[nodiscard]] inline VkDebugUtilsMessengerCreateInfoEXT vk_debug_utils_messenger_create_info() {
  VkDebugUtilsMessengerCreateInfoEXT info = {};
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  info.pUserData = nullptr;
  info.pNext = nullptr;
  return info;
}

[[nodiscard]] inline VkDeviceQueueCreateInfo vk_device_queue_create_info(const uint32_t family_idx) {
  VkDeviceQueueCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  info.queueFamilyIndex = family_idx;
  info.queueCount = 1;
  info.pNext = nullptr;
  return info;
}

[[nodiscard]] inline VkDeviceCreateInfo vk_device_create_info(const std::vector<VkDeviceQueueCreateInfo>& queue_info,
                                                              const VkPhysicalDeviceFeatures& features_to_activate,
                                                              const std::vector<const char*>& extensions,
                                                              const std::vector<const char*>& layers) {
  VkDeviceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info.pQueueCreateInfos = queue_info.data();
  info.queueCreateInfoCount = static_cast<uint32_t>(queue_info.size());
  info.pEnabledFeatures = &features_to_activate;
  info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  info.ppEnabledExtensionNames = extensions.data();
  info.enabledLayerCount = static_cast<uint32_t>(layers.size());
  info.ppEnabledLayerNames = layers.data();
  info.pNext = nullptr;
  return info;
}

[[nodiscard]] inline VkSwapchainCreateInfoKHR vk_swapchain_create_info(
    const std::vector<uint32_t>& unique_queue_family_indices) {
  VkSwapchainCreateInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  if (unique_queue_family_indices.size() > 1) {
    // Concurrent sharing
    info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    info.queueFamilyIndexCount = static_cast<uint32_t>(unique_queue_family_indices.size());
    info.pQueueFamilyIndices = unique_queue_family_indices.data();
  } else {
    // Exclusive sharing
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
  }
  return info;
}

[[nodiscard]] inline VkImageViewCreateInfo vk_image_view_create_info(VkImage vk_image) {
  VkImageViewCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.image = vk_image;
  return info;
}

[[nodiscard]] inline VmaAllocatorCreateInfo vma_allocator_info(VkInstance vk_instance, VkDevice vk_device,
                                                               VkPhysicalDevice vk_physical_device,
                                                               const uint32_t vk_api_version) {
  VmaAllocatorCreateInfo info = {};
  info.instance = vk_instance;
  info.device = vk_device;
  info.physicalDevice = vk_physical_device;
  info.vulkanApiVersion = vk_api_version;
  return info;
}

[[nodiscard]] inline VkRenderPassCreateInfo vk_renderpass_create_info() {
  VkRenderPassCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  return info;
}

[[nodiscard]] inline VkFramebufferCreateInfo vk_framebuffer_create_info(const uint32_t width, const uint32_t height) {
  VkFramebufferCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  info.width = width;
  info.height = height;
  return info;
}

}  // namespace VkStartup::CreateInfo
