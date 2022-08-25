#pragma once
#include <vulkan/vulkan.h>

namespace VulkanUtilities::VKStartup {

class SurfaceLoader {
 public:
  SurfaceLoader() = default;
  virtual ~SurfaceLoader() {
    destroy_surface();
  }

  SurfaceLoader(const SurfaceLoader& source) = delete;
  SurfaceLoader& operator=(const SurfaceLoader& source) = delete;

  SurfaceLoader(SurfaceLoader&& source) noexcept : khr_surface{source.khr_surface} {
    reset(source);
  }

  SurfaceLoader& operator=(SurfaceLoader&& rhs) noexcept {
    if (this != &rhs) {
      khr_surface = rhs.khr_surface;
      reset(rhs);
    }
    return *this;
  }

  [[nodiscard]] VkSurfaceKHR surface() const {
    return khr_surface;
  }

  void init(VkInstance instance) {
    m_vk_instance = instance;
    init_surface();
  }

 protected:
  virtual void init_surface() const = 0;
  VkInstance m_vk_instance{VK_NULL_HANDLE};
  VkSurfaceKHR khr_surface{VK_NULL_HANDLE};

 private:
  static void reset(SurfaceLoader& surface) {
    surface.khr_surface = VK_NULL_HANDLE;
    surface.m_vk_instance = VK_NULL_HANDLE;
  }
  void destroy_surface() const {
    if (khr_surface != VK_NULL_HANDLE) {
      vkDestroySurfaceKHR(m_vk_instance, khr_surface, nullptr);
    }
  }
};

}  // namespace VulkanUtilities::VKStartup