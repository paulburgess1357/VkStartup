from conans import ConanFile, CMake

class HelloConan(ConanFile):
    name = "VkStartup"
    version = "0.1"

    # Optional metadata
    license = "MIT"
    author = "<Paul Burgess> <paulburgess1357@gmail.com>"
    url = "https://github.com/paulburgess1357/VkStartup"
    description = "Vulkan Startup Package"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake"
    exports_sources = "CMakeLists.txt", "VkStartup/*"
    requires = [
                "VkShared/0.1",
                "vulkan-loader/1.3.216.0", 
                "vulkan-memory-allocator/3.0.0"
                ]

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="VkStartup")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["VkStartup"]
