# Roadmap

This is a plan, not a promise. The order of the tiers matters more than the dates, and anything that slips is noted in the [devlog](devlog/).

## Setup (October 2026)

* \[x] CMake and vcpkg toolchain
* \[x] Empty window

## Milestone 1: first triangle (November)

* \[x] Instance, device and swapchain
* \[x] Frames in flight and command buffers
* \[x] Shader pipeline with runtime compilation
* \[x] Triangle on screen with zero validation errors

## Milestone 2: geometry and tools (late November)

* \[x] Vertex and index buffers with staging uploads
* \[x] Depth buffering
* \[x] Free camera
* \[x] ImGui debug UI
* \[x] GPU memory with VMA

## Milestone 3: assets (December)

* \[ ] Textures with stb\_image
* \[ ] glTF static meshes with fastgltf
* \[ ] Descriptor indexing (bindless textures)
* \[ ] CPU and GPU profiling zones with Tracy

## Foundation (January to February)

* \[ ] Physically based shading (metallic-roughness)
* \[ ] Image-based lighting
* \[ ] Data-oriented scene graph and transform hierarchy
* \[ ] Material system
* \[ ] Thin Vulkan wrapper extracted from repeated patterns
* \[ ] Shadow maps for directional lights
* \[ ] HDR, tone mapping and bloom
* \[ ] MSAA

## Core (March to June)

* \[ ] Indirect rendering, CPU frustum culling and LODs
* \[ ] Skeletal animation baseline
* \[ ] SSAO
* \[ ] Cascaded shadow maps
* \[ ] Asynchronous asset loading and a job system
* \[ ] Texture compression (BC7 or KTX2) with mipmaps
* \[ ] Profiling pass with Nsight or Radeon GPU Profiler and RenderDoc
* \[ ] Benchmarks and architecture notes

## Stretch (July onward)

* \[ ] Animation blending and morph targets
* \[ ] GPU-driven culling with Hi-Z and multi-draw indirect
* \[ ] Clustered forward lighting
* \[ ] Render graph with automatic barriers
* \[ ] Temporal anti-aliasing
* \[ ] Custom work-stealing job scheduler
* \[ ] Ray queries for shadows or ambient occlusion

