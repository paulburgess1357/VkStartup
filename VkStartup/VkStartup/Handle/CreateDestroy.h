#pragma once
#include "VkStartup/Misc/Exceptions.h"
#include "VkShared/Macros.h"
#include "VkShared/MemAlloc.h"
#include <vulkan/vulkan.h>
// ReSharper disable CppClangTidyClangDiagnosticShadow

namespace VkStartup {

struct CreateDestroyInstance {
  void create() {
    handle = VK_NULL_HANDLE;
  }
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
  void create() {
    handle = VK_NULL_HANDLE;
  }
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
  void create() {
    handle = VK_NULL_HANDLE;
  }
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

class CreateDestroyImageView {
 public:
  void create() {
    handle = VK_NULL_HANDLE;
  }
  void create(const VkImageViewCreateInfo& info, VkDevice vk_device) {
    VkCheck(vkCreateImageView(vk_device, &info, nullptr, &handle), Exceptions::VkStartupException());
    m_vk_device = vk_device;
  }
  void destroy() const {
    if (handle && m_vk_device) {
      vkDestroyImageView(m_vk_device, handle, nullptr);
    }
  }
  VkImageView handle{VK_NULL_HANDLE};

 private:
  VkDevice m_vk_device{VK_NULL_HANDLE};
};

struct CreateDestroyVMA {
  void create() {
    handle = VK_NULL_HANDLE;
  }
  void create(const VmaAllocatorCreateInfo& info) {
    VkCheck(vmaCreateAllocator(&info, &handle), Exceptions::VkStartupException());
  }
  void destroy() const {
    if (handle) {
      vmaDestroyAllocator(handle);
    }
  }
  VmaAllocator handle{VK_NULL_HANDLE};
};

class CreateDestroyFramebuffer {
 public:
  void create() {
    handle = VK_NULL_HANDLE;
  }
  void create(const VkFramebufferCreateInfo& info, VkDevice vk_device) {
    VkCheck(vkCreateFramebuffer(vk_device, &info, nullptr, &handle), Exceptions::VkRenderPassCreationException());
    m_device = vk_device;
  }
  void destroy() const {
    if (handle && m_device) {
      vkDestroyFramebuffer(m_device, handle, nullptr);
    }
  }
  VkFramebuffer handle{VK_NULL_HANDLE};

 private:
  VkDevice m_device{VK_NULL_HANDLE};
};

class CreateDestroyRenderPass {
 public:
  void create() {
    handle = VK_NULL_HANDLE;
  }
  void create(const VkRenderPassCreateInfo& info, VkDevice vk_device) {
    VkCheck(vkCreateRenderPass(vk_device, &info, nullptr, &handle), Exceptions::VkRenderPassCreationException());
    m_device = vk_device;
  }
  void destroy() const {
    if (handle && m_device) {
      vkDestroyRenderPass(m_device, handle, nullptr);
    }
  }
  VkRenderPass handle{VK_NULL_HANDLE};

 private:
  VkDevice m_device{VK_NULL_HANDLE};
};

}  // namespace VkStartup
