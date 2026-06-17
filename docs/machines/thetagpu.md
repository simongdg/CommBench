# ThetaGPU

**GPU:** NVIDIA A100 (8 per node)  
**Architecture:** sm_80  
**Scheduler:** Cobalt  
**MPI:** MPICH (system)  
**Account flag:** `-A GRACE` (replace with your allocation)

## Modules

The default environment on ThetaGPU provides CUDA and MPICH. No explicit
`module load` is required for a basic CUDA build.

## Configure and build

```bash
cmake -S . -B build \
  -DUSE_CUDA=ON \
  -DCUDA_ARCHITECTURE=80

cmake --build build -j
```

To build with NCCL support, load the module and pass the path:

```bash
module load nccl    # if available on your ThetaGPU environment
cmake -S . -B build \
  -DUSE_CUDA=ON \
  -DCUDA_ARCHITECTURE=80 \
  -DUSE_XCCL=ON \
  -DXCCL_PATH=<path-to-nccl>
```

## Interactive allocation

```bash
qsub -A GRACE -n 2 -t 60 -I --attrs="filesystems=home,theta-fs0"
```

`filesystems=home,theta-fs0` is required to mount the home filesystem inside
the job — omitting it means the binary won't be found at job startup.

## Batch submission

```bash
qsub scripts/run_thetagpu.sh
```

Key Cobalt directives in `run_thetagpu.sh`:

```bash
#COBALT -A GRACE -n 2 -t 00:20:00 -q full-node \
  --attrs="filesystems=home,theta-fs0" --mode script
```

`--mode script` runs the script directly rather than wrapping it; required
when the script calls `mpirun` itself.

## Running

```bash
mpirun --display-map --display-allocation \
  -hostfile ${COBALT_NODEFILE} \
  -n 16 -N 8 \
  ./build/CommBench --use mpi --pattern rail --nbytes 16777216
```

`-n 16 -N 8` launches 16 total ranks with 8 per node (one per A100).
`${COBALT_NODEFILE}` is set automatically by Cobalt.
