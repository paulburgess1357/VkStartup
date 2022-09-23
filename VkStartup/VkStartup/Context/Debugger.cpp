#include "VkStartup/Context/Debugger.h"
#include "VkStartup/Misc/Exceptions.h"
#include "VkShared/Macros.h"

namespace VkStartup {

VkDebugger::VkDebugger(VkInstance instance) : m_vk_instance{instance} {
  init();
}

VkDebugger::~VkDebugger() {
  destroy(m_vk_instance, m_debug_messenger, nullptr);
}

VkDebugger::VkDebugger(VkDebugger&& source) noexcept
    : m_vk_instance(source.m_vk_instance), m_debug_messenger{source.m_debug_messenger} {
  reset();
}

VkDebugger& VkDebugger::operator=(VkDebugger&& rhs) noexcept {
  if (this != &rhs) {
    m_vk_instance = rhs.m_vk_instance;
    m_debug_messenger = rhs.m_debug_messenger;
    rhs.reset();
  }
  return *this;
}

VkDebugUtilsMessengerCreateInfoEXT VkDebugger::instance_debug_create_info() {
  // Create and destroy debug info (normal debugging requires a valid instance)
  auto create_info = CreateInfo::vk_debug_utils_messenger_create_info();
  create_info.pfnUserCallback = debug_callback;
  return create_info;
}

void VkDebugger::reset() {
  m_vk_instance = VK_NULL_HANDLE;
  m_debug_messenger = VK_NULL_HANDLE;
}

#pragma warning(disable : 4100)
VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugger::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                          VkDebugUtilsMessageTypeFlagsEXT type,
                                                          const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                          void* user_data) {
  VkError(callback_data->pMessage);
  return VK_FALSE;
}
#pragma warning(default : 4100)

void VkDebugger::init() {
  auto create_info = CreateInfo::vk_debug_utils_messenger_create_info();
  create_info.pfnUserCallback = debug_callback;

  VkCheck(create_debug_messenger_ext(m_vk_instance, &create_info, nullptr, &m_debug_messenger),
          Exceptions::VkStartupException());
}
VkResult VkDebugger::create_debug_messenger_ext(VkInstance instance,
                                                const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                                                const VkAllocationCallbacks* allocator,
                                                VkDebugUtilsMessengerEXT* messenger) {
  if (const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
      func != nullptr) {
    return func(instance, create_info, allocator, messenger);
  }
  return VK_ERROR_EXTENSION_NOT_PRESENT;
}
void VkDebugger::destroy(VkInstance instance, VkDebugUtilsMessengerEXT messenger,
                         const VkAllocationCallbacks* allocator) const {
  if (const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
      func != nullptr) {
    if (m_vk_instance && m_debug_messenger) {
      func(instance, messenger, allocator);
    }
  }
}

VkInstance m_vk_instance = VK_NULL_HANDLE;
VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;

}  // namespace VkStartup
