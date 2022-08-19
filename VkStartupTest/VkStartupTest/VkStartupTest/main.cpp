#include "VkStartup/Context/InitContext.h"

int main() {
  VulkanUtilities::VkStartup::InitContextOptions options;
  options.enable_validation = true;

  VulkanUtilities::VkStartup::InitContext context{options};
  return 0;
}
