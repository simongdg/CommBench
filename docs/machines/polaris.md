# Polaris

**GPU:** NVIDIA A100 (4 per node)  
**Architecture:** sm_80  
**Scheduler:** PBS Pro  
**MPI:** Cray MPICH  
**Account flag:** `-A GRACE` (replace with your allocation)

## Modules

Polaris module environment is set up automatically via the default
`PrgEnv-nvhpc` environment. NCCL is bundled with the NVIDIA HPC SDK at a
fixed path:

```
/opt/nvidia/hpc_sdk/Linux_x86_64/21.9/comm_libs/nccl
```

No explicit `module load` is required for a basic CUDA build.

## Configure and build

```bash
cmake -S . -B build \
  -DUSE_CUDA=ON \
  -DCUDA_ARCHITECTURE=80 \
  -DUSE_XCCL=ON \
  -DXCCL_PATH=/opt/nvidia/hpc_sdk/Linux_x86_64/21.9/comm_libs/nccl

cmake --build build -j
```

## Interactive allocation

```bash
qsub -A GRACE -I -l select=2 -l filesystems=home -l walltime=1:00:00 -q debug
```

## Batch submission

```bash
qsub scripts/run_polaris.sh
```

Key PBS directives in `run_polaris.sh`:

```bash
#PBS -N AFFINITY
#PBS -l select=2:ncpus=256
#PBS -l walltime=00:30:00
#PBS -q debug-scaling
#PBS -l filesystems=home
#PBS -A GRACE
```

## Running

Polaris requires a GPU affinity wrapper because the A100s are wired to the
host in reverse NUMA order. The wrapper `scripts/set_affinity_gpu_polaris.sh`
assigns GPUs as `num_gpus - 1 - PMI_LOCAL_RANK % num_gpus` to correct for
this.

```bash
NNODES=$(wc -l < $PBS_NODEFILE)
NRANKS=4          # MPI ranks per node
NDEPTH=16         # hardware threads per rank
NTHREADS=16       # OMP_NUM_THREADS
NTOTRANKS=$(( NNODES * NRANKS ))

mpiexec --np ${NTOTRANKS} -ppn ${NRANKS} -d ${NDEPTH} \
  --cpu-bind depth \
  -env OMP_NUM_THREADS=${NTHREADS} \
  ./scripts/set_affinity_gpu_polaris.sh \
  ./build/CommBench --use xccl --pattern rail --nbytes 16777216
```

Always pass `set_affinity_gpu_polaris.sh` as the launcher wrapper — skipping
it causes wrong GPU-to-rank assignments and incorrect bandwidth readings.
