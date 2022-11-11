#include "VkStartup/Context//Renderpass.h"
#include "VkStartup/Misc/CreateInfo.h"
#include "VkShared/Macros.h"

namespace VkStartup {

VkRenderPassHandle RenderpassBuilder::create_renderpass(RenderpassData& data, VkDevice device,
                                                        const bool implicit_transition) {
  auto info = CreateInfo::vk_renderpass_create_info();

  // Combine attachments
  std::vector<VkAttachmentDescription> attachments{};
  attachments.insert(attachments.end(), data.color_attachments.begin(), data.color_attachments.end());
  attachments.insert(attachments.end(), data.resolve_attachments.begin(), data.resolve_attachments.end());
  attachments.insert(attachments.end(), data.preserve_attachments.begin(), data.preserve_attachments.end());

  if (data.depth_attachment.format != VK_FORMAT_UNDEFINED) {
    attachments.push_back(data.depth_attachment);
  }

  // Load attachments
  info.pAttachments = attachments.data();
  info.attachmentCount = static_cast<uint32_t>(attachments.size());

  // Load subpass descriptions
  info.pSubpasses = data.subpass_descs.data();
  info.subpassCount = static_cast<uint32_t>(data.subpass_descs.size());

  // Load subpass dependencies
  if (implicit_transition) {
    add_implicit_transition_dependency(data);
  }
  // Depth subpass dependency
  if (data.depth_attachment.format != VK_FORMAT_UNDEFINED) {
    add_depth_transition_dependency(data);
  }

  info.pDependencies = data.subpass_dependencies.data();
  info.dependencyCount = static_cast<uint32_t>(data.subpass_dependencies.size());

#ifndef NDEBUG
  if (data.color_attachments.empty()) {
    VkWarning("Renderpass does not contain any color attachments");
  }
  if (data.subpass_descs.empty()) {
    VkWarning("Renderpass does not contain any subpass descriptions");
  }

  for (const auto& subpass_desc : data.subpass_descs) {
    if (!subpass_desc.pColorAttachments) {
      VkWarning("Subpass descriprition does not contain a color attachment");
    }
  }
  // if (data.color_attachment_refs.empty()) {
  //   VkWarning("Renderpass does not contain any color attachment references");
  // }
  // if (data.color_attachments.size() != data.color_attachment_refs.size()) {
  //   VkWarning("Renderpass color attachment size does not match color attachment ref size");
  // }
#endif

  return VkRenderPassHandle{info, device};
}

void RenderpassBuilder::add_implicit_transition_dependency(RenderpassData& rp_data) {
  // This is not needed if the renderpass being created waits to run
  // by using the stage: VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT.  This ensures
  // the image is available.  If a different stage is used, this implicit
  // transition is necessary.  Its safest to add this to every renderpass.

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;  // implicit subpass before our first subpass
  dependency.dstSubpass = 0;                    // refers to first subpass

  // Source operations
  // operation to wait on prior to img access (e.g. src must complete the following operation):
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;

  // Destination operations
  // The destination stages that must wait on the above.  This means other destination
  // operations and stages can run up until that point
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  rp_data.subpass_dependencies.push_back(dependency);
}

void RenderpassBuilder::add_depth_transition_dependency(RenderpassData& rp_data) {
  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  rp_data.subpass_dependencies.push_back(dependency);
}

}  // namespace VkStartup
