#pragma once
#include "VkStartup/Handle/UsingHandle.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace VkStartup {

struct RenderpassBuffers {
  uint32_t width{0};
  uint32_t height{0};
  VkRenderPassHandle renderpass{};
  std::vector<VkImage> vk_images{VK_NULL_HANDLE};
  std::vector<VkImageViewHandle> image_views{};
  std::vector<VkFramebufferHandle> framebuffers{};
};

struct RenderpassData {
  // Color
  std::vector<VkAttachmentDescription> color_attachments{};
  // std::vector<VkAttachmentReference> color_attachment_refs{};

  // Resolve
  std::vector<VkAttachmentDescription> resolve_attachments{};
  std::vector<VkAttachmentReference> resolve_attachment_refs{};

  // Preserve
  std::vector<VkAttachmentDescription> preserve_attachments{};
  std::vector<VkAttachmentReference> preserve_attachment_refs{};

  // Depth
  VkAttachmentDescription depth_attachment = {};
  VkAttachmentReference depth_attachment_ref = {};

  // Subpass Dependencies
  std::vector<VkSubpassDescription> subpass_descs{};
  std::vector<VkSubpassDependency> subpass_dependencies{};
};

class RenderpassBuilder {
 public:
  [[nodiscard]] static VkRenderPassHandle create_renderpass(RenderpassData& data, VkDevice device,
                                                            const bool implicit_transition = true);

 private:
  static void add_implicit_transition_dependency(RenderpassData& rp_data);
};

}  // namespace VkStartup
