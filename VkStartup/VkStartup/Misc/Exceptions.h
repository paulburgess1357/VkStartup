#pragma once
#include <exception>

namespace VkStartup::Exceptions {

class VkStartupException final : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "VkStartup failed";
  }
};

}  // namespace VkStartup::Exceptions
