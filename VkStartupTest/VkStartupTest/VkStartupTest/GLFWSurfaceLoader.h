#pragma once
#include "VkStartup/Context/SurfaceLoader.h"

// required before GLFW if not using 'GLFW_INCLUDE_VULKAN'
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>

namespace VkStartupTest {

// GLFW Surface Loader Example
class GLFWSurfaceLoader final : public VkStartup::SurfaceLoader {
 public:
  explicit GLFWSurfaceLoader(GLFWwindow& window, std::string surface_id)
      : SurfaceLoader{std::move(surface_id)}, m_window{window} {
  }

  // User defined surface initialization override
  [[nodiscard]] VkResult init_surface() override {
    return glfwCreateWindowSurface(m_vk_instance, &m_window, nullptr, &khr_surface);
  }

  // User defined swapchain format override
  [[nodiscard]] VkStartup::Swapchain::SwapchainFormatDetails select_swapchain_format(
      const VkStartup::Swapchain::SwapchainFormatSupport supported_details) const override {
    VkStartup::Swapchain::SwapchainFormatDetails details{};

    // Desired format and color space
    bool desired_format_found{false};
    for (const auto& available_format : supported_details.formats) {
      if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
          available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        details.format = available_format;
        desired_format_found = true;
      }
    }

    // Default value
    if (!desired_format_found) {
      VkWarning("Desired swapchain not found.  Defaulting to first supported format and colorspace");
      details.format = supported_details.formats.at(0);
    }

    // Desired presentation mode
    bool desired_presentation_mode_found{false};
    for (const auto& available_present_mode : supported_details.present_modes) {
      if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
        details.present_mode = available_present_mode;
        desired_presentation_mode_found = true;
      }
    }

    // Default value
    if (!desired_presentation_mode_found) {
      details.present_mode = VK_PRESENT_MODE_FIFO_KHR;
    }

    // Swap Extent
    if (supported_details.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
      details.extent = supported_details.capabilities.currentExtent;
    } else {
      int width{};
      int height{};
      glfwGetFramebufferSize(&m_window, &width, &height);
      details.extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

      details.extent.width = std::clamp(details.extent.width, supported_details.capabilities.minImageExtent.width,
                                        supported_details.capabilities.maxImageExtent.width);
      details.extent.height = std::clamp(details.extent.height, supported_details.capabilities.minImageExtent.height,
                                         supported_details.capabilities.maxImageExtent.height);
    }

    // Image count
    details.image_count = supported_details.capabilities.minImageCount + 1;
    // Check that we don't exeed the maximum supported image count
    if (supported_details.capabilities.maxImageCount > 0 &&
        details.image_count > supported_details.capabilities.maxImageCount) {
      details.image_count = supported_details.capabilities.maxImageCount;
    }

    // Pre-Transform
    details.pretransform = supported_details.capabilities.currentTransform;

    // Usage
    details.usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    return details;
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

}  // namespace VkStartupTest
