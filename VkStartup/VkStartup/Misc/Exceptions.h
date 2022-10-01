#pragma once
#include <exception>

namespace VkStartup::Exceptions {

class VkStartupException final : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "VkStartup failed";
  }
};

class VkRenderPassCreationException final : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Failed to create renderpass";
  }
};

class VkGraphicsPipelineCreationException final : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Failed to create graphics pipeline";
  }
};

}  // namespace VkStartup::Exceptions
