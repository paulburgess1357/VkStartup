#pragma once
#include "VkStartup/Context/MemAlloc.h"
#include <vulkan/vulkan_core.h>
#include <vector>

namespace VkStartup::CreateInfo {

[[nodiscard]] inline VkApplicationInfo vk_application_info() {
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  return app_info;
}

[[nodiscard]] inline VkInstanceCreateInfo vk_instance_create_info() {
  VkInstanceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
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

[[nodiscard]] inline VkDeviceQueueCreateInfo vk_device_queue_create_info() {
  VkDeviceQueueCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  info.pNext = nullptr;
  return info;
}

[[nodiscard]] inline VkDeviceCreateInfo vk_device_create_info() {
  VkDeviceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
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

}  // namespace VkStartup::CreateInfo
