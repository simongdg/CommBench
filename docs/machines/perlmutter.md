# Perlmutter

**GPU:** NVIDIA A100 (4 per node)  
**Architecture:** sm_80  
**Scheduler:** Slurm  
**MPI:** Cray MPICH (via `nvhpc`)  
**Account flag:** `-A m4301` (replace with your allocation)

## Modules

```bash
module reset
module load nccl
module load nvhpc
```

`nvhpc` provides the NVIDIA HPC SDK compilers and sets up CUDA. `nccl` sets
`NCCL_DIR` so CMake can find the NCCL headers and library.

## Configure and build

```bash
cmake -S . -B build \
  -DUSE_CUDA=ON \
  -DCUDA_ARCHITECTURE=80 \
  -DUSE_XCCL=ON \
  -DXCCL_PATH=${NCCL_DIR}

cmake --build build -j
```

## Interactive allocation

```bash
salloc --nodes 2 --qos interactive --time 01:00:00 \
  --constraint gpu --gpus 4 --account=m4301
```

## Batch submission

```bash
sbatch scripts/run_perlmutter.sh
```

Key SBATCH directives in `run_perlmutter.sh`:

```bash
#SBATCH -A m4301
#SBATCH -C gpu
#SBATCH -q regular
#SBATCH -t 00:30:00
#SBATCH -N 2
#SBATCH --ntasks-per-node=4
#SBATCH -c 32
#SBATCH --gpus-per-task=1
#SBATCH --gpu-bind=none
```

`--gpus-per-task=1` with `--gpu-bind=none` makes all 4 GPUs visible to every
rank, which CommBench requires for its `myid % numdevice` device selection.
See the [Perlmutter running-jobs docs](https://docs.nersc.gov/systems/perlmutter/running-jobs/#4-nodes-16-tasks-16-gpus-all-gpus-visible-to-all-tasks)
for full explanation.

## Running

```bash
srun -N 2 --ntasks-per-node=4 -C gpu -c 32 \
  --gpus-per-task=1 --gpu-bind=none \
  ./build/CommBench --use xccl --pattern rail --nbytes 16777216
```
