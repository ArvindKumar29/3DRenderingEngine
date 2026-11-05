# Benchmarks

## Test system

| Component | Details |
| --- | --- |
| Laptop | ASUS TUF Dash F15 (TODO: exact model number) |
| CPU | Intel Core i5-11300H (11th Gen), 4 cores / 8 threads, 3.1 GHz base, up to 4.4 GHz turbo (about 4.0 GHz observed at idle) |
| CPU cache | L1 320 KB, L2 5.0 MB, L3 8.0 MB |
| RAM | 16 GB DDR4-3200 (SODIMM, both slots used) |
| GPU (used for all tests) | NVIDIA GeForce RTX 3050 Laptop GPU, 4 GB dedicated VRAM, driver 621.01 |
| GPU (not used) | Intel Iris Xe Graphics (integrated) |
| Storage | System NVMe SSD, SK hynix HFM512GD3JX013N, 512 GB (project on D:); second NVMe, Crucial P3 1 TB |
| OS | Windows 11 (TODO: version and build from `winver`) |
| Vulkan | SDK 1.4.363, NVIDIA driver API version 1.4.357, validation layer 1.4.363 |
| Display | TODO: resolution and refresh rate |

## Test conditions

- Laptop plugged in, power mode: Turbo.
- Rendering on the NVIDIA GPU (the app logs the selected device at startup).
- Each result is the median and 1% low over TODO frames after a warm-up.

## Results

| Date | Milestone | Scene | Median frame time (ms) | 1% low (ms) | Notes |

| 5/11/2026 | 2 | A rectangle rendered with Camer and UI | 287(FPS) | - | NIL |
