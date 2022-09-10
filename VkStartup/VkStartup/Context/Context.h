#pragma once
#include "VkStartup/Context/Debugger.h"
#include "VkStartup/Handle/UsingHandle.h"
#include "VkStartup/Context/PhysicalDevice.h"
#include "VkStartup/Context/SurfaceLoader.h"
#include "VkStartup/Context/Queues.h"
#include "VkShared/Enums.h"
#include <memory>

namespace VulkanUtilities::VkStartup {

struct VkSwapchainContext {
  std::unique_ptr<SurfaceLoader> surface_loader{};
  std::unique_ptr<VkSwapchainHandle> swapchain{};
  VkSwapchainKHR vk_swapchain{VK_NULL_HANDLE};
  std::vector<VkImage> vk_swapchain_images{};
};

struct VkContext {
  std::unique_ptr<VkInstanceHandle> instance{};
  VkInstance vk_instance{VK_NULL_HANDLE};

  std::unique_ptr<VkDebugger> debugger{};
  PhysicalDeviceInfo physical_device_info{};

  std::unique_ptr<VkDeviceHandle> device{};
  VkDevice vk_device{VK_NULL_HANDLE};

  std::unordered_map<VkShared::Enums::QueueFamily, Queues::QueueIndexHandle> vk_queues{};

  // Multiple surfaces to be drawn to
  std::unordered_map<std::string, VkSwapchainContext> swapchain_data{};

  // std::unordered_map<std::string, std::unique_ptr<SurfaceLoader>> surface_loaders{};
  // std::unordered_map<std::string, std::unique_ptr<VkSwapchainHandle>> swapchains{};
  // std::unordered_map<std::string, VkSwapchainKHR> vk_swapchains{};
  // std::unordered_map<std::string, std::vector<VkImage>> vk_swapchain_images{};
};

}  // namespace VulkanUtilities::VkStartup
