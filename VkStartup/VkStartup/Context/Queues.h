#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

namespace VkStartup::Queues {

struct QueueIndexHandle {
  uint32_t family_index{999};
  VkQueue handle{VK_NULL_HANDLE};
};

[[nodiscard]] inline std::vector<uint32_t> unique_queues(
    const std::unordered_map<VkShared::Enums::QueueFamily, Queues::QueueIndexHandle>& vk_queues) {
  std::unordered_set<uint32_t> unique_queues;
  std::vector<uint32_t> unique_queues_vec;
  for (const auto& [family, queue] : vk_queues) {
    unique_queues.insert(queue.family_index);
  }
  unique_queues_vec.reserve(unique_queues.size());
  for (const auto idx : unique_queues) {
    unique_queues_vec.push_back(idx);
  }
  return unique_queues_vec;
}

}  // namespace VkStartup::Queues
