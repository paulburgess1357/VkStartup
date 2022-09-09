#pragma once
#include <exception>

namespace VulkanUtilities::VkStartupTest::Exceptions {

class VkStartupTestException final : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "VkStartupTest failed";
  }
};

}  // namespace Exceptions