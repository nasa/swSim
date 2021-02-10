
cmake_minimum_required (VERSION 3.13)

project(KernelComposer C CXX CUDA)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CUDA_EXTENSIONS -lcusparse -lcublas)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) #for static lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) # for shared lib

enable_testing()

#find_package(Eigen3 REQUIRED)
#include_directories(${EIGEN3_INCLUDE_DIR})
#find_package(MPI REQUIRED)

#include_directories(${MPI_INCLUDE_PATH})
#include_directories("${PROJECT_SOURCE_DIR}/include")

add_library(KernelComposer SHARED #STATIC #
    # "${PROJECT_SOURCE_DIR}/src/DenseVector.cpp"
    # "${PROJECT_SOURCE_DIR}/src/SparseVector.cpp"
    # "${PROJECT_SOURCE_DIR}/src/SparseMatrix.cpp"
    "${PROJECT_SOURCE_DIR}/src/KernelComposer.cpp"
    "${PROJECT_SOURCE_DIR}/src/KernelComposerManager.cu"
	)

#target_link_libraries(KernelComposer ${MPI_LIBRARIES})
target_link_libraries(KernelComposer cusparse cublas)
target_include_directories(KernelComposer PUBLIC "${PROJECT_SOURCE_DIR}/include")

if(GO_TEST_COVERAGE)
    target_link_libraries(KernelComposer gcov)
endif()