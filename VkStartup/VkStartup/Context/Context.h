#pragma once
#include "VkStartup/Context/Debugger.h"
#include "VkStartup/Handle/UsingHandle.h"
#include "VkStartup/Context/PhysicalDevice.h"
#include "VkStartup/Context/SurfaceLoader.h"
#include "VkShared/Enums.h"
#include <memory>

namespace VulkanUtilities::VkStartup {

struct QueueIndexHandle {
  uint32_t family_index{999};
  VkQueue handle{VK_NULL_HANDLE};
};

struct VkContext {
  std::unique_ptr<VkInstanceHandle> instance{};
  VkInstance vk_instance{VK_NULL_HANDLE};

  std::unique_ptr<VkDebugger> debugger{};
  PhysicalDeviceInfo physical_device_info{};

  std::unique_ptr<VkDeviceHandle> device{};
  VkDevice vk_device{VK_NULL_HANDLE};

  std::unordered_map<VkShared::Enums::QueueFamily, QueueIndexHandle> vk_queues{};

  std::unique_ptr<SurfaceLoader> surface_loader{};
};

}  // namespace VulkanUtilities::VkStartup
