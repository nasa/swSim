
cmake_minimum_required (VERSION 3.13)

project(HostMesh C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) #for static lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) # for shared lib

if(GO_TEST)
    enable_testing()
    add_subdirectory("${PROJECT_SOURCE_DIR}/testing")
endif()

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${MODELUT_SOURCE_DIR}/include")

add_library(HostMesh SHARED #STATIC #
    "${PROJECT_SOURCE_DIR}/source/HexMesh.cpp"
    "${PROJECT_SOURCE_DIR}/source/HostMeshFactory.cpp"
    )

if(GO_TEST_COVERAGE)
    target_link_libraries(HostMesh gcov)
endif()

#target_link_libraries(Stubs cusparse cublas)
