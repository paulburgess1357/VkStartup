#include "VkStartup/Context//Renderpass.h"
#include "VkStartup/Misc/CreateInfo.h"
#include "VkShared/Macros.h"

namespace VkStartup {

VkRenderPassHandle RenderpassBuilder::create_renderpass(const RenderpassData& data, VkDevice device) {
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
  info.pDependencies = data.subpass_dependencies.data();
  info.dependencyCount = static_cast<uint32_t>(data.subpass_dependencies.size());

#ifndef NDEBUG
  if (data.color_attachments.empty()) {
    VkWarning("Renderpass does not contain any color attachments");
  }
  if (data.subpass_descs.empty()) {
    VkWarning("Renderpass does not contain any subpass descriptions");
  }

  for (auto& subpass_desc : data.subpass_descs) {
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

}  // namespace VkStartup
