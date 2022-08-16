#pragma once
#include <vulkan/vulkan_core.h>
#include "VkStartup/Misc/Exceptions.h"
#include "VkShared/Macros.h"

namespace VulkanUtilities::VkStartup {

struct CreateDestroyInstance {
  void create(const VkInstanceCreateInfo& info) {
    VkCheck(vkCreateInstance(&info, nullptr, &handle), Exceptions::VkStartupException());
  }
  void destroy() const {
    if (handle) {
      vkDestroyInstance(handle, nullptr);
    }
  }
  VkInstance handle;
};

struct CreateDestroyDevice {
  void create(const VkDeviceCreateInfo& info, VkPhysicalDevice vk_physical_device) {
    VkCheck(vkCreateDevice(vk_physical_device, &info, nullptr, &handle), Exceptions::VkStartupException());
  }
  void destroy() const {
    if (handle) {
      vkDestroyDevice(handle, nullptr);
    }
  }
  VkDevice handle{VK_NULL_HANDLE};
};

}  // namespace VulkanUtilities::VkStartup
