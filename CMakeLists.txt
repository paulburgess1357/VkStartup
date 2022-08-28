set(TargetName "VkStartup")
message("\nBuilding " ${TargetName})

cmake_minimum_required(VERSION 3.9.0)
project(${TargetName})
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${TargetName}/*.cpp")
file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS "${TargetName}/*.h")

add_library(${TargetName} STATIC ${SourceFiles} ${HeaderFiles})
set_target_properties(${TargetName} PROPERTIES DEBUG_POSTFIX d)

set(DEP_VkShared VkShared)
message("\nCloning VkShared... (GitHub SSH key setup required)")
FetchContent_Declare(
 ${DEP_VkShared}
   GIT_REPOSITORY git@github.com:paulburgess1357/VkShared.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(${DEP_VkShared})

set(IncludeDirectories ${CMAKE_CURRENT_SOURCE_DIR}/${TargetName} ${VkShared_SOURCE_DIR}/${DEP_VkShared})
set(LinkDirectories ${DEP_VkShared})

set(ConanSetupHasRun False)
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	message("Using Conan for dependency management")
	message("Loading Conan macros")
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
	set(ConanSetupHasRun True)
	set(CONAN_INCLUDE_DIRS_FILTERED ${CONAN_INCLUDE_DIRS})

	# Filter out packages not necessary for main library
	list(REMOVE_ITEM CONAN_INCLUDE_DIRS_FILTERED 
		${CONAN_INCLUDE_DIRS_GLFW}
	)

	list(APPEND IncludeDirectories ${CONAN_INCLUDE_DIRS_FILTERED})
	list(APPEND LinkDirectories CONAN_PKG::vulkan-loader)
else()
	find_package(Vulkan REQUIRED)
	list(APPEND IncludeDirectories ${Vulkan_INCLUDE_DIR})
	list(APPEND LinkDirectories ${Vulkan_LIBRARIES})
endif()

target_include_directories(${TargetName} PRIVATE 
	${IncludeDirectories}
)
target_link_libraries(${TargetName} PRIVATE
	${LinkDirectories}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HeaderFiles})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})

# VkStartupTest
add_subdirectory(VkStartupTest)

install(TARGETS ${TargetName})
