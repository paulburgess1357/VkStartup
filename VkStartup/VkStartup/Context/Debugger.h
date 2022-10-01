#pragma once
#include <vulkan/vulkan_core.h>

namespace VkStartup {

class VkDebugger {
 public:
  explicit VkDebugger(VkInstance instance);
  ~VkDebugger();

  VkDebugger(VkDebugger&& source) noexcept;
  VkDebugger& operator=(VkDebugger&& rhs) noexcept;
  VkDebugger(const VkDebugger& source) = delete;
  VkDebugger& operator=(const VkDebugger& rhs) = delete;

  [[nodiscard]] static VkDebugUtilsMessengerCreateInfoEXT instance_debug_create_info();

 private:
  void reset();
  [[nodiscard]] static VKAPI_ATTR VkBool32 VKAPI_CALL
  debug_callback([[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                 [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT type,
                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data, [[maybe_unused]] void* user_data);
  void init();
  [[nodiscard]] static VkResult create_debug_messenger_ext(VkInstance instance,
                                                           const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                                                           const VkAllocationCallbacks* allocator,
                                                           VkDebugUtilsMessengerEXT* messenger);
  void destroy(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* allocator) const;

  VkInstance m_vk_instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;
};

}  // namespace VkStartup
