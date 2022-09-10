#pragma once
#include <vulkan/vulkan.h>
#include "VkStartup/Misc/Exceptions.h"
#include "VkShared/Macros.h"
// ReSharper disable CppClangTidyClangDiagnosticShadow

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
  VkInstance handle{VK_NULL_HANDLE};
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

class CreateDestroySwapchain {
 public:
  void create(const VkSwapchainCreateInfoKHR& info, VkDevice vk_device) {
    VkCheck(vkCreateSwapchainKHR(vk_device, &info, nullptr, &handle), Exceptions::VkStartupException());
    m_vk_device = vk_device;
  }
  void destroy() const {
    if (handle && m_vk_device) {
      vkDestroySwapchainKHR(m_vk_device, handle, nullptr);
    }
  }
  VkSwapchainKHR handle{VK_NULL_HANDLE};

 private:
  VkDevice m_vk_device{VK_NULL_HANDLE};
};

}  // namespace VulkanUtilities::VkStartup
