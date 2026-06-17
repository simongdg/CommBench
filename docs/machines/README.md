# CommBench — Per-Machine Build & Run Guide

CMake-era onboarding for each supported HPC system. Each page covers module
loads, configure/build, interactive allocation, and job submission.

| System | Vendor | GPU | Arch | Scheduler |
|--------|--------|-----|------|-----------|
| [Frontier](frontier.md) | AMD | MI250X | gfx90A | Slurm |
| [Perlmutter](perlmutter.md) | NVIDIA | A100 | sm_80 | Slurm |
| [Polaris](polaris.md) | NVIDIA | A100 | sm_80 | PBS |
| [Delta](delta.md) | NVIDIA | A100 | sm_80 | Slurm |
| [Summit](summit.md) | NVIDIA | V100 | sm_70 | LSF |
| [ThetaGPU](thetagpu.md) | NVIDIA | A100 | sm_80 | Cobalt |

## Common CMake flags

| Flag | Default | Purpose |
|------|---------|---------|
| `-DUSE_CUDA=ON` | OFF | Enable NVIDIA GPU port |
| `-DUSE_HIP=ON` | OFF | Enable AMD GPU port |
| `-DCUDA_ARCHITECTURE=<N>` | 80 | CUDA SM target (e.g. 70, 80, 90) |
| `-DHIP_ARCHITECTURE=<arch>` | gfx942;gfx90A | HIP GFX target |
| `-DUSE_XCCL=ON` | OFF | Enable NCCL/RCCL |
| `-DXCCL_PATH=<path>` | — | Root of NCCL/RCCL installation |
| `-DUSE_GTL=ON` | OFF | Link Cray GPU Transfer Layer (Frontier) |
| `-DGTL_PATH=<path>` | — | Root of GTL installation (Frontier) |
| `-DUSE_XPMEM=ON` | OFF | Link XPMEM shared-memory library |
