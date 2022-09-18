Vulkan startup library to jumpstart your projects!
<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->

<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->

<!-- PROJECT LOGO -->
<div align="center">
<!--
  <a href="https://github.com/othneildrew/Best-README-Template">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>
-->
  <h1 align="center">VkStartup</h1>

  <p align="center">
    Vulkan startup library to jumpstart your projects!
    <br />
    <br />
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#custom-physical-device-and-surface-loader-usage">Custom Physical Device & Surface Loader Usage</a></li>
    <li><a href="#initcontext-usage">InitContext Usage</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## About The Project

Vulkan has a lot of boilerplate code necessary for creating a context.  This project handles the initial context creation while providing flexibility for individual use cases.  The context will be destroyed when it goes out of scope.  There is no need to manually destroy handles.  This project builds on both Windows and Linux (Mac untested).

Here's what's provided:
* VkInstance 
* VkPhysicalDevice (Default or user defined)
* VkDevice 
* QueueIndices & VkQueues
* VmaAllocator

Additionally, support for multiple surfaces exists but is not required.  If at least one surface loader is provided, the following will be created ***for each surface***:
* VkSwapchainKHR
* std::vector\<VkImage>
* std::vector\<VkImageView>

<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

This project relies on CMake and optionally Conan for dependency installation.  The following dependencies are required: 
* VulkanSDK
* VulkanMemoryAllocator
* GLFW (If building the VkStartupTest project); For production use, any windowing system (GLFW, SDL, etc.) is fine.

### Installation
Below are build steps for using Conan.  If you are not using Conan, you will need to have the dependencies already installed.

#### Conan Build
1. Clone the repo
   ```sh
   git clone git@github.com:paulburgess1357/VkStartup.git
   cd VkStartup
   ```
2. Build using Conan (Debug, Release, etc.).
   ```sh
   conan install . --build missing -s build_type=Debug -if build
   cd build
   ```
3. Build using CMake
   ```sh
   cmake ../
   cmake --build . --config Debug
   ```
4. Optionally build the VkStartupTest project
   ```sh
   cmake --build . --config Debug --target VkStartupTest
   ```
#### No Conan Build
You must have the dependencies listed above already installed
1. Clone the repo
   ```sh
   git clone git@github.com:paulburgess1357/VkStartup.git
   cd VkStartup
   ```
2. Build using CMake
   ```sh
   cmake ../
   cmake --build . --config Debug
   ```
3. Optionally build the VkStartupTest project
   ```sh
   cmake --build . --config Debug --target VkStartupTest
   ```


<!-- USAGE EXAMPLES -->
## Custom Physical Device and Surface Loader Usage
### Custom Physical Device Selection Class
The user can rely on the default physical device selection or create their own physical device selection class.  This class also determines enabled features used for the logical device creation.  See the `PhysicalDeviceDefault` for examples on creating your own physical device selection & feature enabling class:
  * [PhysicalDeviceDefault.h](https://github.com/paulburgess1357/VkStartup/blob/master/VkStartup/VkStartup/Context/PhysicalDevice.h)
  * [PhysicalDeviceDefault.cpp](https://github.com/paulburgess1357/VkStartup/blob/master/VkStartup/VkStartup/Context/PhysicalDevice.cpp)
  
```
class PhysicalDeviceDefault final : public PhysicalDevice {
 public:
  explicit PhysicalDeviceDefault(VkInstance instance, std::vector<const char*> desired_device_extension,
                                 std::vector<const char*> required_device_extensions)
      : PhysicalDevice{instance, std::move(desired_device_extension), std::move(required_device_extensions)} {
  }

 private:
  void select_best_physical_device(const std::vector<VkPhysicalDevice>& devices) override;
  void set_features_to_activate() override;
  [[nodiscard]] static bool device_meets_requirements(VkPhysicalDevice device,
                                                      const VkPhysicalDeviceFeatures& device_features);
};
```

### Custom Surface Loading Class
Surface creation is optional.  Surfaces can be created from any windowing system and are user defined.  The examples here use GLFW, but SDL or other windowing systems can be used.  Multiple surface loaders can be used and must be given a unique ID.  All handles related to the surface, swapchain, images, etc. will be accessed using the unique ID.  If a surface loader is used, the following handles will be created independently for each surface: 
  * VkSwapchainKHR
  * std::vector\<VkImage>
  * std::vector\<VkImageView>

Example Loader:
  * [GLFW Example Surface Loader](https://github.com/paulburgess1357/VkStartup/blob/master/VkStartupTest/VkStartupTest/VkStartupTest/GLFWSurfaceLoader.h)

## InitContext Usage

### InitContextOptions
Creating a context uses the `InitContextOptions` [struct](https://github.com/paulburgess1357/VkStartup/blob/master/VkStartup/VkStartup/Context/InitContext.h).  This struct provides the following:
 * Required & desired instance & device extensions
 * Required & desired layers
 * boolean option for enabling validation layers
 * User defined physical device selection criteria.  If no criteria is provided, the default physical device selection criteria will be used.
 * Custom surface loaders.  This is optional.  A user may create a zero, a single, or multiple surfaces.  Swapchain images will be created.  If no surface loader is provided, no swapchain images will be created.

```
VulkanUtilities::VkStartup::InitContextOptions options;

// Extensions and layers (see struct for other options)
options.required_instance_extensions = VulkanUtilities::VkStartupTest::GLFWSurfaceLoader::extensions();
options.desired_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
options.enable_validation = true;

// Custom surface loader
options.custom_surface_loaders.emplace_back(std::make_unique<VulkanUtilities::VkStartupTest::GLFWSurfaceLoader>(*window, "main_window"));
options.custom_surface_loaders.emplace_back(std::make_unique<VulkanUtilities::VkStartupTest::GLFWSurfaceLoader>(*window, "main_window2"));

VulkanUtilities::VkStartup::InitContext context{std::move(options)};

```
For a full example, build and run the test project `VkStartupTest`:

[VkStartupTest Example](https://github.com/paulburgess1357/VkStartup/blob/master/VkStartupTest/VkStartupTest/VkStartupTest/main.cpp)

<!-- LICENSE -->
## License
Distributed under the MIT License. See `LICENSE.txt` for more information.

## Contact
Paul Burgess - paulburgess1357@gmail.com

## Acknowledgments
* [Vulkan Tutorial](https://vulkan-tutorial.com/)
<p align="right">(<a href="#readme-top">back to top</a>)</p>
