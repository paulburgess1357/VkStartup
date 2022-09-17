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
[![MIT License][license-shield]][license-url]



<!-- PROJECT LOGO -->
<br />
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
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
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
<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage
### Custom Physical Device Selection Class
The user can rely on the default physical device selection or create their own physical device selection class.  This class also determines enabled features used for the logical device creation.  See the `PhysicalDeviceDefault` for examples on creating your own physical device selection & feature enabling class:
  * [PhysicalDeviceDefault.h](https://github.com/paulburgess1357/VkStartup/blob/master/VkStartup/VkStartup/Context/PhysicalDevice.h)
  * [PhysicalDeviceDefault.cpp](https://github.com/paulburgess1357/VkStartup/blob/master/VkStartup/VkStartup/Context/PhysicalDevice.cpp)
  
### Custom Surface Loading Class
Surface creation is optional.  Surfaces can be created from any windowing system and are user defined.  The examples here use GLFW, but SDL or other windowing systems can be used.  Multiple surface loaders can be used and must be given a unique ID.  All handles related to the surface, swapchain, images, etc. will be accessed using the unique ID.  


### InitContextOptions
Creating a context uses the `InitContextOptions` [struct](https://github.com/paulburgess1357/VkStartup/blob/master/VkStartup/VkStartup/Context/InitContext.h).  This struct provides the following:
 * Required & desired instance & device extensions
 * Required & desired layers
 * boolean option for enabling validation layers
 * User defined physical device selection criteria.  If no criteria is provided, the default physical device selection criteria will be used.
 * Custom surface loaders.  This is optional.  A user may create a zero, a single, or multiple surfaces.  Swapchain images will be created.  If no surface loader is provided, no swapchain images will be created.

### InitContextOptions Usage
```
VulkanUtilities::VkStartup::InitContextOptions options;

// Extensions and layers (see struct for other options)
options.required_instance_extensions = VulkanUtilities::VkStartupTest::GLFWSurfaceLoader::extensions();
options.desired_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
options.enable_validation = true;

// Custom surface loader
options.custom_surface_loaders.emplace_back(std::make_unique<VulkanUtilities::VkStartupTest::GLFWSurfaceLoader>(*window, "main_window"));
options.custom_surface_loaders.emplace_back(std::make_unique<VulkanUtilities::VkStartupTest::GLFWSurfaceLoader>(*window, "main_window2"));

```




_For more examples, please refer to the [Documentation](https://example.com)_

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [x] Add Changelog
- [x] Add back to top links
- [ ] Add Additional Templates w/ Examples
- [ ] Add "components" document to easily copy & paste sections of the readme
- [ ] Multi-language Support
    - [ ] Chinese
    - [ ] Spanish

See the [open issues](https://github.com/othneildrew/Best-README-Template/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Your Name - [@your_twitter](https://twitter.com/your_username) - email@example.com

Project Link: [https://github.com/your_username/repo_name](https://github.com/your_username/repo_name)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

Use this space to list resources you find helpful and would like to give credit to. I've included a few of my favorites to kick things off!

* [Choose an Open Source License](https://choosealicense.com)
* [GitHub Emoji Cheat Sheet](https://www.webpagefx.com/tools/emoji-cheat-sheet)
* [Malven's Flexbox Cheatsheet](https://flexbox.malven.co/)
* [Malven's Grid Cheatsheet](https://grid.malven.co/)
* [Img Shields](https://shields.io)
* [GitHub Pages](https://pages.github.com)
* [Font Awesome](https://fontawesome.com)
* [React Icons](https://react-icons.github.io/react-icons/search)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/othneildrew/Best-README-Template.svg?style=for-the-badge
[contributors-url]: https://github.com/othneildrew/Best-README-Template/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/othneildrew/Best-README-Template.svg?style=for-the-badge
[forks-url]: https://github.com/othneildrew/Best-README-Template/network/members
[stars-shield]: https://img.shields.io/github/stars/othneildrew/Best-README-Template.svg?style=for-the-badge
[stars-url]: https://github.com/othneildrew/Best-README-Template/stargazers
[issues-shield]: https://img.shields.io/github/issues/othneildrew/Best-README-Template.svg?style=for-the-badge
[issues-url]: https://github.com/othneildrew/Best-README-Template/issues
[license-shield]: https://img.shields.io/github/license/othneildrew/Best-README-Template.svg?style=for-the-badge
[license-url]: https://github.com/othneildrew/Best-README-Template/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/othneildrew
[product-screenshot]: images/screenshot.png
[Next.js]: https://img.shields.io/badge/next.js-000000?style=for-the-badge&logo=nextdotjs&logoColor=white
[Next-url]: https://nextjs.org/
[React.js]: https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB
[React-url]: https://reactjs.org/
[Vue.js]: https://img.shields.io/badge/Vue.js-35495E?style=for-the-badge&logo=vuedotjs&logoColor=4FC08D
[Vue-url]: https://vuejs.org/
[Angular.io]: https://img.shields.io/badge/Angular-DD0031?style=for-the-badge&logo=angular&logoColor=white
[Angular-url]: https://angular.io/
[Svelte.dev]: https://img.shields.io/badge/Svelte-4A4A55?style=for-the-badge&logo=svelte&logoColor=FF3E00
[Svelte-url]: https://svelte.dev/
[Laravel.com]: https://img.shields.io/badge/Laravel-FF2D20?style=for-the-badge&logo=laravel&logoColor=white
[Laravel-url]: https://laravel.com
[Bootstrap.com]: https://img.shields.io/badge/Bootstrap-563D7C?style=for-the-badge&logo=bootstrap&logoColor=white
[Bootstrap-url]: https://getbootstrap.com
[JQuery.com]: https://img.shields.io/badge/jQuery-0769AD?style=for-the-badge&logo=jquery&logoColor=white
[JQuery-url]: https://jquery.com 
