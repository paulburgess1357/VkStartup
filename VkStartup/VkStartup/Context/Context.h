#pragma once
#include "VkStartup/Context/Debugger.h"
#include "VkStartup/Handle/UsingHandle.h"
#include "VkStartup/Context/PhysicalDevice.h"
#include "VkStartup/Context/SurfaceLoader.h"
#include "VkStartup/Context/Renderpass.h"
#include "VkShared/Enums.h"
#include <memory>

namespace VkStartup {

struct QueueIndexHandle {
  uint32_t family_index{999};
  VkQueue handle{VK_NULL_HANDLE};
};

struct VkSwapchainContext {
  std::unique_ptr<SurfaceLoader> surface_loader{};
  Swapchain::SwapchainFormatDetails swap_format_details{};
  VkSwapchainHandle swapchain{};
  RenderpassBuffers rp_buffers{};
  QueueIndexHandle present_queue;
};

struct VkContext {
  VkInstanceHandle instance{};
  std::unique_ptr<VkDebugger> debugger{};
  PhysicalDeviceInfo phy_device_info{};
  VkDeviceHandle device{};
  std::unordered_map<VkShared::Enums::QueueFamily, QueueIndexHandle> queues{};

  // Multiple surfaces to be drawn to
  std::unordered_map<std::string, VkSwapchainContext> swap_ctx{};
  VmaAllocatorHandle mem_alloc{};

  [[nodiscard]] VkExtent2D swap_extent(const std::string& id) const {
    return swap_ctx.at(id).swap_format_details.extent;
  }
};

}  // namespace VkStartup
