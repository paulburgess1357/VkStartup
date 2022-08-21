#pragma once
#include <vulkan/vulkan_core.h>

namespace VulkanUtilities::VkStartup::CreateInfo {
inline [[nodiscard]] VkApplicationInfo vk_application_info() {
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  return app_info;
}

inline [[nodiscard]] VkInstanceCreateInfo vk_instance_create_info() {
  VkInstanceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  return info;
}

inline [[nodiscard]] VkDebugUtilsMessengerCreateInfoEXT vk_debug_utils_messenger_create_info() {
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

inline [[nodiscard]] VkDeviceQueueCreateInfo vk_device_queue_create_info() {
  VkDeviceQueueCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  info.pNext = nullptr;
  return info;
}

inline [[nodiscard]] VkDeviceCreateInfo vk_device_create_info() {
  VkDeviceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info.pNext = nullptr;
  return info;
}

}  // namespace VulkanUtilities::VkStartup::CreateInfo
