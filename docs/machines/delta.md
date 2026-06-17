# Delta

**GPU:** NVIDIA A100 (4 per node, `gpuA100x4` partition)  
**Architecture:** sm_80  
**Scheduler:** Slurm  
**MPI:** OpenMPI  
**Account flag:** `--account=bbkf-delta-gpu` (replace with your allocation)

## Modules

```bash
module reset
module load nccl
module load openmpi/4.1.5+cuda
```

## Configure and build

`module load nccl` sets up NCCL headers and library on the default search
path, so no explicit `XCCL_PATH` is needed.

```bash
cmake -S . -B build \
  -DUSE_CUDA=ON \
  -DCUDA_ARCHITECTURE=80 \
  -DUSE_XCCL=ON

cmake --build build -j
```

## Interactive allocation

```bash
salloc --account=bbkf-delta-gpu --partition=gpuA100x4-interactive \
  --nodes=4 --gpus-per-node=4 --ntasks-per-node=4 \
  --cpus-per-task=16 --exclusive --mem=0 --time=01:00:00
```

`--exclusive --mem=0` allocates the full node. `--cpus-per-task=16` gives 16
CPU cores per rank (64 cores / 4 GPUs).

## Batch submission

```bash
sbatch scripts/run_delta.sh
```

Key SBATCH directives in `run_delta.sh`:

```bash
#SBATCH --exclusive
#SBATCH --mem=0
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=16
#SBATCH --partition=gpuA100x4
#SBATCH --account=bbkf-delta-gpu
#SBATCH --time=00:30:00
#SBATCH --gpus-per-node=4
```

## Running

```bash
srun ./build/CommBench --use xccl --pattern rail --nbytes 16777216
```

`srun` inherits the allocation's GPU binding from the SBATCH/salloc
directives, so no additional GPU-mapping wrapper is needed.
