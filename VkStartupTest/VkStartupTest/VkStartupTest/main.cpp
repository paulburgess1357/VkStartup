#include "VkStartup/Context/InitContext.h"
#include "VkStartupTest/GLFWSurfaceLoader.h"
#include "VkStartupTest/Exceptions.h"

int main() {
  VkStartup::InitContextOptions options;
  options.enable_validation = true;
  options.desired_device_ext.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  // Any windowing system is fine.  This example shows GLFW:
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  auto window = glfwCreateWindow(1920, 1080, "VkStartupTest Window", nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    throw VkStartupTest::Exceptions::VkStartupTestException();
  }

  // Load custom surface loader & extensions into options
  options.surface_loaders.emplace_back(
      std::make_unique<VkStartupTest::GLFWSurfaceLoader>(*window, "main_window"));
  options.surface_loaders.emplace_back(
      std::make_unique<VkStartupTest::GLFWSurfaceLoader>(*window, "main_window2"));
  options.required_instance_ext = VkStartupTest::GLFWSurfaceLoader::extensions();

  // Create context
  VkStartup::InitContext context{std::move(options)};

  glfwDestroyWindow(window);
  return 0;
}
