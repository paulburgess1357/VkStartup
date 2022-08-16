#pragma once
#include <exception>

namespace VulkanUtilities::VkStartup::Exceptions {
class VkStartupException final : public std::exception {
 public:
  [[nodiscard]] char const* what() const override {
    return "VkStartup failed";
  }
};
}  // namespace VulkanUtilities::VkStartup::Exceptions
