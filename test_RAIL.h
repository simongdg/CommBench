{
  int numgroup = numproc / groupsize;

  Type *sendbuf_d;
  Type *recvbuf_d;

#ifdef PORT_CUDA
    cudaMalloc(&sendbuf_d, count * sizeof(Type));
    cudaMalloc(&recvbuf_d, count * sizeof(Type));
#elif defined PORT_HIP
    hipMalloc(&sendbuf_d, count * sizeof(Type));
    hipMalloc(&recvbuf_d, count * sizeof(Type));
#elif defined PORT_SYCL
    sycl::queue q(sycl::gpu_selector_v);
    sendbuf_d = sycl::malloc_device<Type>(count, q);
    recvbuf_d = sycl::malloc_device<Type>(count, q);
#else
    sendbuf_d = new Type[count];
    recvbuf_d = new Type[count];
#endif


  {
    CommBench::Comm<Type> bench(MPI_COMM_WORLD, (CommBench::capability) cap);

    double data = 0;

    switch(direction) {
      case 1: // UNI-DIRECTIONAL
        for(int send = 0; send < subgroupsize; send++)
          for(int recvgroup = 1; recvgroup < numgroup; recvgroup++) {
            int sender = send;
            int recver = recvgroup * groupsize + send;
            bench.add(sendbuf_d, 0, recvbuf_d, 0, count, sender, recver);
          }
        data = count * sizeof(Type) / 1.e9 * subgroupsize * (numgroup - 1);
	break;
      case 2: // BI-DIRECTIONAL
        for(int send = 0; send < subgroupsize; send++)
          for(int recvgroup = 1; recvgroup < numgroup; recvgroup++) {
            int sender = send;
            int recver = recvgroup * groupsize + send;
            bench.add(sendbuf_d, 0, recvbuf_d, 0, count, sender, recver);
            bench.add(sendbuf_d, 0, recvbuf_d, 0, count, recver, sender);
          }
        data = 2 * count * sizeof(Type) / 1.e9 * subgroupsize * (numgroup - 1);
	break;
      case 3: // OMNI-DIRECTIONAL
        for(int sendgroup = 0; sendgroup < numgroup; sendgroup++)
          for(int recvgroup = 0; recvgroup < numgroup; recvgroup++)
            if(sendgroup != recvgroup)
              for(int send = 0; send < subgroupsize; send++) {
                int sender = sendgroup * groupsize + send;
                int recver = recvgroup * groupsize + send;
                bench.add(sendbuf_d, 0, recvbuf_d, 0, count, sender, recver);
              }
        data = 2 * count * sizeof(Type) / 1.e9 * subgroupsize * (numgroup - 1);
	break;
    }

    bench.report();

    double minTime, medTime, maxTime, avgTime;
    bench.measure(warmup, numiter, minTime, medTime, maxTime, avgTime);
    if(myid == ROOT) {
      printf("TEST_RAIL (%d, %d)\n", groupsize, subgroupsize);
      printf("data: %.4e MB\n", data * 1e3);
      printf("minTime: %.4e us, %.4e s/GB, %.4e GB/s\n", minTime * 1e6, minTime / data, data / minTime);
      printf("medTime: %.4e us, %.4e s/GB, %.4e GB/s\n", medTime * 1e6, medTime / data, data / medTime);
      printf("maxTime: %.4e us, %.4e s/GB, %.4e GB/s\n", maxTime * 1e6, maxTime / data, data / maxTime);
      printf("avgTime: %.4e us, %.4e s/GB, %.4e GB/s\n", avgTime * 1e6, avgTime / data, data / avgTime);
    }
  }

#ifdef PORT_CUDA
   cudaFree(sendbuf_d);
   cudaFree(recvbuf_d);
#elif defined PORT_HIP
   hipFree(sendbuf_d);
   hipFree(recvbuf_d);
#elif defined PORT_SYCL
   sycl::free(sendbuf_d, q);
   sycl::free(recvbuf_d, q);
#else
   delete[] sendbuf_d;
   delete[] recvbuf_d;
#endif

}
