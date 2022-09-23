#pragma once
#include "VkStartup/Handle/CreateDestroy.h"
#include "VkShared/THandle.h"

namespace VkStartup {
using VkInstanceHandle = VkShared::THandle<CreateDestroyInstance>;
using VkDeviceHandle = VkShared::THandle<CreateDestroyDevice>;
using VkSwapchainHandle = VkShared::THandle<CreateDestroySwapchain>;
using VkImageViewHandle = VkShared::THandle<CreateDestroyImageView>;
using VmaAllocatorHandle = VkShared::THandle<CreateDestroyVMA>;
}  // namespace VkStartup
