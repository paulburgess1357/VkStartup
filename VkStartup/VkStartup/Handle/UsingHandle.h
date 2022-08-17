#pragma once
#include "VkStartup/Handle/CreateDestroy.h"
#include "VkShared/THandle.h"

namespace VulkanUtilities::VkStartup {
using VkInstanceHandle = VkShared::THandle<CreateDestroyInstance>;
using VkDeviceHandle = VkShared::THandle<CreateDestroyDevice>;
}  // namespace VulkanUtilities::VkStartup
