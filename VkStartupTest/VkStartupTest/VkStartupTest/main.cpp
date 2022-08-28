#include "VkStartup/Context/InitContext.h"
#include "VkStartupTest/GLFWSurfaceLoader.h"

int main() {
  VulkanUtilities::VkStartup::InitContextOptions options;
  options.enable_validation = true;

  // Any windowing system is fine.  This example shows GLFW:
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  auto window = glfwCreateWindow(1920, 1080, "VkStartupTest Window", nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    throw VulkanUtilities::VkStartupTest::Exceptions::VkStartupTestException();
  }

  // Load custom surface loader & extensions into options
  options.custom_surface_loaders.emplace_back(std::make_unique<VulkanUtilities::VkStartupTest::GLFWSurfaceLoader>(*window,
                                                                                                         "main_window"));
  options.custom_surface_loaders.emplace_back(
      std::make_unique<VulkanUtilities::VkStartupTest::GLFWSurfaceLoader>(*window, "main_window2"));

  options.required_instance_extensions = VulkanUtilities::VkStartupTest::GLFWSurfaceLoader::extensions();

  // Create context
  VulkanUtilities::VkStartup::InitContext context{std::move(options)};

  glfwDestroyWindow(window);
  return 0;
}
