#pragma once
#include "VkStartup/Handle/CreateDestroy.h"
#include "VkShared/THandle.h"

namespace VkStartup {
using VkInstanceHandle = VkShared::THandle<CreateDestroyInstance>;
using VkDeviceHandle = VkShared::THandle<CreateDestroyDevice>;
using VkSwapchainHandle = VkShared::THandle<CreateDestroySwapchain>;
using VkImageViewHandle = VkShared::THandle<CreateDestroyImageView>;
using VmaAllocatorHandle = VkShared::THandle<CreateDestroyVMA>;
using VkFramebufferHandle = VkShared::THandle<CreateDestroyFramebuffer>;
using VkRenderPassHandle = VkShared::THandle<CreateDestroyRenderPass>;
}  // namespace VkStartup
