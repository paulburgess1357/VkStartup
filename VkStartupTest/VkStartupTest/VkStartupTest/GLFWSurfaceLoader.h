#pragma once
#include "VkStartup/Context/SurfaceLoader.h"

// GLFW Surface Loader Example
class GLFWSurfaceLoader final : public VulkanUtilities::VKStartup::SurfaceLoader {
 public:
  void init_surface() const override {
  }
};
