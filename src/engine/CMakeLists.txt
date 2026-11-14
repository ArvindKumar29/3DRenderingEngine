# 1. SOURCE FILE DISCOVERY
# GLOB_RECURSE automatically finds every .cpp and .h file in this folder and all subfolders.
# CONFIGURE_DEPENDS tells CMake to automatically re-run if you create a new C++ file.
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# 2. TARGET CREATION
# Bundles all the discovered C++ files into a single Static Library named "engine".
add_library(engine STATIC ${ENGINE_SOURCES} "Renderer/Camera.h" "Time.cpp" "Renderer/UI.h" "Vulkan/vma.cpp" "Vulkan/vma.h" "Renderer/StbImage.cpp" "Renderer/Texture.h" "Renderer/Texture.cpp")

# Automatically mirrors your actual folder structure (e.g., core/, renderer/) inside 
# Visual Studio's Solution Explorer so everything stays organized.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_SOURCES})

# 3. INCLUDE DIRECTORIES
# Tells the compiler where to look when you type #include "engine/Version.h" or #include <stb_image.h>.
# PUBLIC means any future app that links to "engine" will also automatically know these include paths.
target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/src ${STB_INCLUDE_DIRS})

# Enforces C++20 standard strictly for this engine library.
target_compile_features(engine PUBLIC cxx_std_20)

# 4. DEPENDENCY LINKING
# Glues the vcpkg libraries you downloaded to your engine.
target_link_libraries(engine PUBLIC
  glfw                             # Windowing and input
  glm::glm                         # 3D Math
  spdlog::spdlog                   # Logging
  GPUOpen::VulkanMemoryAllocator   # Memory management
  imgui::imgui                     # GUI
  Tracy::TracyClient               # Profiler
)

# 5. C++ PREPROCESSOR MACROS (#defines)
# These are automatically injected into your C++ code before it compiles.
target_compile_definitions(engine PUBLIC
  GLFW_INCLUDE_NONE               # Stops GLFW from accidentally loading OpenGL (we are using Vulkan!).
  VK_NO_PROTOTYPES                # Crucial for Volk: Forces Vulkan functions to be loaded dynamically at runtime.
  VK_USE_PLATFORM_WIN32_KHR       # Tells Vulkan we are building for Windows (allows native HWND window surfaces).
  
  GLM_FORCE_RADIANS               # Forces GLM to use radians instead of degrees for math (standard for graphics).
  GLM_FORCE_DEPTH_ZERO_TO_ONE     # Fixes a massive bug: OpenGL depth is -1 to 1, but Vulkan depth is 0 to 1.
  
  NOMINMAX                        # Stops Windows.h from breaking std::min and std::max.
  WIN32_LEAN_AND_MEAN             # Speeds up compilation by stripping out useless, heavy Windows APIs.
  
  # Bakes the absolute folder paths of your shaders and assets directly into the C++ binary.
  # This allows your C++ code to find assets no matter where the .exe is launched from.
  "VKR_SHADER_DIR=\"${CMAKE_BINARY_DIR}/shaders\""
  "VKR_ASSET_DIR=\"${CMAKE_SOURCE_DIR}/assets\""
)

# 6. COMPILER WARNINGS (Visual Studio Specific)
if(MSVC)  
  # /W4: Turns on high-level warnings to catch bugs early.
  # /permissive-: Enforces strict, standard-compliant C++ (disables old Microsoft-specific quirks).
  # /utf-8: Ensures your source files can read special characters without encoding errors.
  target_compile_options(engine PUBLIC /W4 /permissive- /utf-8)
endif()