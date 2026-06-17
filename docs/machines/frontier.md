# Frontier

**GPU:** AMD MI250X (2 GCDs per physical GPU, 8 GCDs per node)  
**Architecture:** gfx90A  
**Scheduler:** Slurm  
**MPI:** Cray MPICH  
**Account flag:** `-A CMB103` (replace with your allocation)

## Modules

```bash
module reset
module load craype-accel-amd-gfx90a
module load PrgEnv-cray
module load amd-mixed/5.4.3
module unload darshan-runtime
```

`darshan-runtime` must be unloaded — it intercepts file I/O and can cause
hangs during benchmark runs.

## Configure and build

```bash
export MPICH_GPU_SUPPORT_ENABLED=1

cmake -S . -B build \
  -DUSE_HIP=ON \
  -DHIP_ARCHITECTURE=gfx90A \
  -DUSE_XCCL=ON \
  -DXCCL_PATH=${ROCM_PATH}

cmake --build build -j
```

RCCL ships inside ROCm, so `XCCL_PATH=${ROCM_PATH}` is correct.
`MPICH_GPU_SUPPORT_ENABLED=1` is required for GPU-aware MPI; set it before
both `cmake` and `srun`.

### Building aws-ofi-rccl (required for RCCL over Slingshot)

RCCL needs the AWS OFI plugin to run over Frontier's Slingshot network.
Build it once per ROCm version:

```bash
bash scripts/build_aws_ofi_rccl.sh
```

The script clones and builds the plugin in the current directory and prints
the `lib/` path to add to `LD_LIBRARY_PATH`.

## Interactive allocation

```bash
salloc -A CMB103 -t 01:00:00 -N 4 \
  --ntasks-per-node=8 --gpus-per-node=8 \
  --gpu-bind=closest --switch=1
```

`--switch=1` requests that all nodes land on the same network switch, which
is important for accurate intra-cabinet bandwidth measurements.

## Batch submission

```bash
sbatch scripts/run_frontier.sh
```

Key SBATCH directives in `run_frontier.sh`:

```bash
#SBATCH -A CMB137
#SBATCH -t 00:30:00
#SBATCH -N 2
#SBATCH --ntasks-per-node=8
#SBATCH --gpus-per-node=8
#SBATCH --gpu-bind=closest
```

## Running

```bash
export MPICH_GPU_SUPPORT_ENABLED=1
export LD_LIBRARY_PATH=/path/to/aws-ofi-rccl/lib:$LD_LIBRARY_PATH
export NCCL_NET_GDR_LEVEL=3
export OMP_NUM_THREADS=7

srun -c7 ./build/CommBench --use xccl --pattern rail --nbytes 16777216
```

`-c7` gives 7 CPU cores per GCD (56 cores / 8 GCDs). `OMP_NUM_THREADS=7`
matches. `NCCL_NET_GDR_LEVEL=3` enables GPU Direct RDMA over Slingshot.

## Known issues

- **RCCL without aws-ofi-rccl:** falls back to socket transport, ~10x slower
  and can time out on large messages. Always set `LD_LIBRARY_PATH` to the
  OFI plugin before running with `--use xccl`.
- **darshan-runtime loaded:** causes stalls during MPI-IO init. Always
  `module unload darshan-runtime` before building or running.
