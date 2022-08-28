#pragma once
#include "VkStartup/Context/SurfaceLoader.h"

// required before GLFW if not using 'GLFW_INCLUDE_VULKAN'
#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>
#include <vector>

namespace VulkanUtilities::VkStartupTest {

// GLFW Surface Loader Example
class GLFWSurfaceLoader final : public VkStartup::SurfaceLoader {
 public:
  explicit GLFWSurfaceLoader(GLFWwindow& window, std::string surface_id)
      : SurfaceLoader{std::move(surface_id)}, m_window{window} {
  }

  [[nodiscard]] VkResult init_surface() override {
    return glfwCreateWindowSurface(m_vk_instance, &m_window, nullptr, &khr_surface);
  }

  [[nodiscard]] static std::vector<const char*> extensions() {
    uint32_t extension_count{0};
    const char** required_extensions = glfwGetRequiredInstanceExtensions(&extension_count);
    std::vector extensions(required_extensions, required_extensions + extension_count);
    return extensions;
  }

 private:
  GLFWwindow& m_window;
};

namespace Exceptions {

class VkStartupTestException final : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "VkStartupTest failed";
  }
};

}  // namespace Exceptions

}  // namespace VulkanUtilities::VkStartupTest
