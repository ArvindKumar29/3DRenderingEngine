# ==============================================================================
# 1. BASELINE & PROJECT SETUP
# ==============================================================================
# Sets the absolute minimum version of CMake required to build this project.
cmake_minimum_required(VERSION 3.21)

# Names the overall workspace and specifies that we are using C++ (CXX).
project(RenderingEngine VERSION 0.1.0 LANGUAGES CXX)

# ==============================================================================
# 2. MODERN C++ COMPILER RULES
# ==============================================================================
# Mandates C++20. Modern engines rely heavily on data-oriented design patterns, 
# so C++20 ensures the compiler has the advanced features needed to optimize the codebase.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disables compiler-specific quirks (like Microsoft or GNU extensions). 
# This ensures your engine's code is strictly standard-compliant and easily portable.
set(CMAKE_CXX_EXTENSIONS OFF)

# Generates a 'compile_commands.json' file. This is incredibly useful for IDEs 
# (like VS Code or CLion) to provide accurate autocomplete (IntelliSense) and error checking.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Forces all compiled executables (.exe) to be placed neatly into the "build/bin" folder, 
# rather than scattering them randomly deep inside the build directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ==============================================================================
# 3. DEPENDENCY RESOLUTION (The vcpkg Integration)
# ==============================================================================
# Because we linked the vcpkg toolchain in CMakePresets.json, these find_package() 
# commands are completely intercepted by vcpkg[cite: 3, 4]. 
# Instead of searching your computer blindly, vcpkg checks your vcpkg.json manifest, 
# downloads the source code, compiles the libraries, and links them automatically[cite: 3, 4].
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Tracy CONFIG REQUIRED)
find_package(Stb REQUIRED)

# STB is a header-only library (no compilation needed), so we just locate the folder containing the header.
find_path(STB_INCLUDE_DIRS "stb_image.h")

# The Vulkan SDK must be installed on your host OS. 
# This specifically hunts down "glslc", which is the official program needed to compile raw GLSL into SPIR-V.
find_package(Vulkan REQUIRED COMPONENTS glslc)

# ==============================================================================
# 4. SHADER AUTOMATION
# ==============================================================================
# Loads the custom shader compilation script we reviewed earlier.
include(cmake/CompileShaders.cmake)

# Executes the function: Tells it to look in your raw "shaders/" folder, compile everything, 
# output the .spv binaries into "shaders/", and groups them into a master target named "shaders".
add_shader_target(shaders ${CMAKE_SOURCE_DIR}/src/engine/Shaders ${CMAKE_SOURCE_DIR}/shaders)

# ==============================================================================
# 5. MODULAR PROJECT DELEGATION
# ==============================================================================
# Instead of cramming all your C++ files into this one script, we modularize the project.
# CMake will now dive into these subfolders and look for smaller, dedicated CMakeLists.txt files.
add_subdirectory(src/engine) # Handles compiling your engine logic into a library (.lib).
add_subdirectory(src/app)    # Handles compiling your actual game loop into an executable (.exe).
target_compile_definitions(engine PUBLIC ENGINE_ROOT_DIR="${CMAKE_SOURCE_DIR}/")
target_compile_definitions(engine PUBLIC
  IMGUI_IMPL_VULKAN_USE_VOLK
  IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING)

# ==============================================================================
# 6. IDE QUALITY OF LIFE
# ==============================================================================
# Tells Visual Studio that when you press the "Play/Run" button, it should launch 
# the "app" executable by default. Without this, it might try to "run" your engine .lib file, which would throw an error.
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT app)