# Summit

**GPU:** NVIDIA V100 (6 per node)  
**Architecture:** sm_70  
**Scheduler:** LSF + jsrun  
**MPI:** Spectrum MPI  
**Account flag:** `-P CHM137` (replace with your allocation)

## Modules

```bash
module load gcc
module load cuda
module load job-step-viewer
```

`job-step-viewer` provides the `js_task_info` helper used in the run script
to print per-rank GPU/CPU affinity.

## Configure and build

NCCL is available system-wide on Summit; no explicit path is needed.

```bash
cmake -S . -B build \
  -DUSE_CUDA=ON \
  -DCUDA_ARCHITECTURE=70 \
  -DUSE_XCCL=ON

cmake --build build -j
```

## Interactive allocation

```bash
bsub -q debug -alloc_flags gpudefault -W 01:00 -nnodes 2 \
  -P CHM137 -env "all,LSF_CPU_ISOLATION=on" -Is /bin/bash
```

`LSF_CPU_ISOLATION=on` prevents CPU core sharing between resource sets, which
is required for accurate timing.

## Batch submission

```bash
bsub scripts/run_summit.sh
```

Key LSF directives in `run_summit.sh`:

```bash
#BSUB -P CHM137
#BSUB -W 00:30
#BSUB -nnodes 2
#BSUB -alloc_flags gpudefault
```

## Running

Summit uses `jsrun` instead of `srun`/`mpirun`. Each resource set maps to
one GPU + one MPI rank + 7 CPU cores:

```bash
export OMP_NUM_THREADS=7

# Network affinity for dual-rail InfiniBand
export PAMI_ENABLE_STRIPING=1
export PAMI_IBV_ADAPTER_AFFINITY=1
export PAMI_IBV_DEVICE_NAME="mlx5_0:1,mlx5_3:1"
export PAMI_IBV_DEVICE_NAME_1="mlx5_3:1,mlx5_0:1"

jsrun --smpiargs="-gpu"       \
  -n 12                       \  # total resource sets (6 per node × 2 nodes)
  -r 6                        \  # resource sets per node
  -c 7                        \  # CPU cores per resource set
  -g 1                        \  # GPUs per resource set
  -a 1                        \  # MPI ranks per resource set
  -bpacked:7                  \
  js_task_info \
  ./build/CommBench --use xccl --pattern rail --nbytes 16777216
```

The `PAMI_IBV_DEVICE_NAME` / `PAMI_IBV_DEVICE_NAME_1` variables enable
dual-rail striping across Summit's two InfiniBand ports per node. Omitting
them halves inter-node bandwidth.
