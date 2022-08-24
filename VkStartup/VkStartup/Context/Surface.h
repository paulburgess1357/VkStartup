#pragma once
#include <vulkan/vulkan.h>

class Surface {
 public:
  explicit Surface(VkInstance instance) : m_vk_instance{instance} {
  }
  virtual ~Surface() {
    destroy_surface();
  }

  Surface(const Surface& source) = delete;
  Surface& operator=(const Surface& source) = delete;

  Surface(Surface&& source) noexcept : khr_surface{source.khr_surface} {
    reset(source);
  }

  Surface& operator=(Surface&& rhs) noexcept {
    if (this != &rhs) {
      khr_surface = rhs.khr_surface;
      reset(rhs);
    }
    return *this;
  }

  [[nodiscard]] VkSurfaceKHR surface() const {
    return khr_surface;
  }

  void destroy_surface() const {
    if (khr_surface != VK_NULL_HANDLE) {
      vkDestroySurfaceKHR(m_vk_instance, khr_surface, nullptr);
    }
  }

 protected:
  virtual void init_surface() const = 0;
  VkInstance m_vk_instance{VK_NULL_HANDLE};
  VkSurfaceKHR khr_surface{VK_NULL_HANDLE};

 private:
  static void reset(Surface& surface) {
    surface.khr_surface = VK_NULL_HANDLE;
  }
};
