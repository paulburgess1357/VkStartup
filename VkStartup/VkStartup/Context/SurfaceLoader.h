#pragma once
#include "VkStartup/Misc/Exceptions.h"
#include "VkShared/Macros.h"
#include <vulkan/vulkan.h>
#include <algorithm>

namespace VulkanUtilities::VkStartup {

class SurfaceLoader {
 public:
  explicit SurfaceLoader(std::string id) : m_id{std::move(id)} {
    if (std::ranges::find(unique_surface_ids, m_id) != unique_surface_ids.end()) {
      VkError("The id: " + m_id + " already exists for a surface.  Surface ids must be unique!");
    }
    unique_surface_ids.push_back(m_id);
  }
  virtual ~SurfaceLoader() {
    destroy_surface();
  }

  SurfaceLoader(const SurfaceLoader& source) = delete;
  SurfaceLoader& operator=(const SurfaceLoader& source) = delete;

  SurfaceLoader(SurfaceLoader&& source) noexcept : khr_surface{source.khr_surface} {
    reset(source);
  }

  SurfaceLoader& operator=(SurfaceLoader&& rhs) noexcept {
    if (this != &rhs) {
      khr_surface = rhs.khr_surface;
      reset(rhs);
    }
    return *this;
  }

  [[nodiscard]] VkSurfaceKHR surface() const {
    return khr_surface;
  }

  [[nodiscard]] std::string id() const {
    return m_id;
  }

  void init(VkInstance instance) {
    m_vk_instance = instance;
    VkCheck(init_surface(), Exceptions::VkStartupException());
  }

 protected:
  [[nodiscard]] virtual VkResult init_surface() = 0;
  VkInstance m_vk_instance{VK_NULL_HANDLE};
  VkSurfaceKHR khr_surface{VK_NULL_HANDLE};

 private:
  static void reset(SurfaceLoader& surface) {
    surface.khr_surface = VK_NULL_HANDLE;
    surface.m_vk_instance = VK_NULL_HANDLE;
  }
  void destroy_surface() const {
    if (khr_surface != VK_NULL_HANDLE) {
      vkDestroySurfaceKHR(m_vk_instance, khr_surface, nullptr);
    }
  }
  std::string m_id{};
  static inline std::vector<std::string> unique_surface_ids{};
};

}  // namespace VulkanUtilities::VkStartup
