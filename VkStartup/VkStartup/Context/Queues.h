#pragma once
#include <vulkan/vulkan_core.h>

namespace VkStartup::Queues {

struct QueueIndexHandle {
  uint32_t family_index{999};
  VkQueue handle{VK_NULL_HANDLE};
};

}  // namespace VkStartup::Queues
