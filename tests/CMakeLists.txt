file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/unit_tests)

# Set the output directory for the test executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/unit_tests)

add_library(tests INTERFACE)
target_link_libraries(tests INTERFACE CommFunctions OpenMP::OpenMP_CXX)

add_subdirectory(mpi)

if (USE_CUDA OR USE_HIP)
    add_subdirectory(ipc)
endif()

if (USE_XCCL)
    add_subdirectory(xccl)
endif()