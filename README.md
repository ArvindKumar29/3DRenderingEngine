# 3D Rendering Engine

A real-time 3D renderer written from scratch in C++20 and Vulkan 1.3/1.4 (`vulkan\_raii.hpp`). I am building it to learn graphics and engine programming properly, and to document the process in public.

<p align="center">
  <img src="Results/First%20Triangle.png" alt="First Triangle Rendered in Vulkan 1.3" width="45%" />
  \&nbsp;\&nbsp;
  <img src="Results/Rectangle%20rendered%20using%20vertex%20input%20data.png" alt="Rectangle Rendered Using Vertex \& Index Buffers" width="45%" />
\&nbsp;\&nbsp;
<img src="Results/Camera%20and%20UI.png" alt=Camera and UI" width="45%" />
</p>

## Status

**Milestone 1 Complete \& Milestone 2 In Progress:** The engine initializes Vulkan 1.3 Dynamic Rendering and Synchronization2 on the discrete GPU, encapsulates all low-level Vulkan handles behind a clean `Vulkan/` backend facade, and renders indexed geometry using custom C++ Vertex and Index Buffers.

|Area|State|
|-|-|
|Build system (CMake and vcpkg)|Done|
|Window creation \& Vulkan Surface|Done|
|Instance, Physical \& Logical Device, Swapchain|Done|
|First triangle with Dynamic Rendering \& Sync2|Done|
|Shutdown cleanup \& `Vulkan/` facade refactor|Done|
|Custom Vertex \& Index Buffers (Indexed Quad)|Done|
|Input, Camera, VMA, Depth Buffering \& ImGui|Next|

Progress notes and debugging logs are in the [devlog](docs/devlog/) and [benchmarks](docs/Benchmarks.md). The full plan is in the [roadmap](docs/RoadMap.md).

## Architecture

All low-level Vulkan objects (`Instance`, `ValidationLayers`, `Window`, `Device`, `Pipeline`, and `CommandPool`) are encapsulated inside the `Vulkan/` backend module (`vlk.h` / `vlk.cpp`), keeping `Engine` clean for high-level rendering features.

<p align="center">
  <img src="https://github.com/user-attachments/assets/44cdb0b9-90d5-4729-9620-7d6e79f978ab" alt="Architecture Diagram" width="700" />
</p>

## Goals

* Learn modern Vulkan (dynamic rendering, synchronization2, descriptor indexing) by writing my own thin abstraction layer instead of using a ready-made one.
* Measure performance and report it with hardware, scene and frame times.
* Explain design decisions in the devlog, including the mistakes.

## Planned features

**Foundation:** instance, device and swapchain; frames in flight; shader compilation; GPU memory and staging uploads; vertex and index buffers; depth buffering; free camera \& input; textures; glTF loading; bindless textures; physically based shading with image-based lighting; shadow maps; ImGui debug UI; CPU and GPU profiling.

**Core:** HDR, tone mapping and bloom; MSAA and SSAO; cascaded shadow maps; indirect rendering with CPU culling and LODs; skeletal animation; asynchronous asset loading and a job system; texture compression.

**Stretch:** GPU-driven culling with Hi-Z, clustered forward lighting, a render graph, temporal anti-aliasing, a custom job scheduler, animation blending and morph targets, ray queries.

## Dependencies

Installed automatically by vcpkg from `vcpkg.json`:

|Library|Used for|
|-|-|
|GLFW|Window, input and Vulkan surface|
|VulkanMemoryAllocator|GPU memory allocation|
|GLM|Math|
|spdlog|Logging|
|stb|Image decoding|

Shaders are compiled with `glslc`, which ships with the Vulkan SDK.

**Planned additions:** Dear ImGui, Tracy, fastgltf, meshoptimizer, Taskflow, KTX-Software. A move from GLSL to Slang is planned for the PBR stage.

## Building

Windows 10 or 11 (x64) only for now.

**Requirements**

* Visual Studio 2022 / 2026 with the "Desktop development with C++" workload
* CMake 3.21 or newer
* Vulkan SDK 1.3 or newer (sets the `VULKAN\_SDK` environment variable)
* Git, and vcpkg with the `VCPKG\_ROOT` environment variable set
* A GPU and driver with Vulkan 1.3+ support

**One-time vcpkg setup**

```powershell

git clone [https://github.com/microsoft/vcpkg.git](https://github.com/microsoft/vcpkg.git) D:\dev\vcpkg
D:\dev\vcpkg\bootstrap-vcpkg.bat -disableMetrics
setx VCPKG_ROOT "D:\dev\vcpkg"
```
Reopen the terminal after `setx`.

**Build and run**

```powershell
git clone https://github.com/ArvindKumar29/3DRenderingEngine.git
cd RenderingEngine
cmake --preset default
cmake --build --preset debug
.\\build\\bin\\Debug\\app.exe
```

The first configure takes several minutes while vcpkg builds the dependencies.

## Project structure

```txt
RenderingEngine/
  cmake/        Build helper scripts (automatic SPIR-V shader compilation)
  src/engine/   Core engine orchestrator (Vulkan/ backend and Renderer/ systems)
  src/app/      Executable entry point (main.cpp)
  shaders/      GLSL shaders (.vert, .frag), compiled to SPIR-V at build time
  assets/       Models and textures
  Results/      Rendered output screenshots
  docs/         Devlogs, architecture diagrams (docs/media/), benchmarks and roadmap
```
## Benchmarks

Test machine: NVIDIA GeForce RTX 3050 Laptop GPU, Windows 11. Results are published in [docs/Benchmarks.md](docs/Benchmarks.md) as they are measured.

## References and credits

I use these as references while writing my own code. Where an approach follows a reference closely, the devlog says so.

* *3D Graphics Rendering Cookbook* by Sergey Kosarevsky and Viktor Latypov
* *Game Engine Architecture* by Jason Gregory
* [vkguide.dev](https://vkguide.dev)
* Test scenes from the [Khronos glTF Sample Assets](https://github.com/KhronosGroup/glTF-Sample-Assets); each asset keeps its own license

## License

MIT. See `LICENSE`.

## Author

Arvind Kumar: [Portfolio](https://arvind-kumar-portfolio.bolt.host), [LinkedIn](https://linkedin.com/in/arvind-kumar-59b609178), [GitHub](https://github.com/ArvindKumar29)
