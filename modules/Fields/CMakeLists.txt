
cmake_minimum_required (VERSION 3.13)

project(Fields C CXX)

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

add_library(Fields SHARED #STATIC #
    "${PROJECT_SOURCE_DIR}/source/ConstantField.cpp"
    "${PROJECT_SOURCE_DIR}/source/SparseCellField.cpp"
    "${PROJECT_SOURCE_DIR}/source/SparseNodeField.cpp"
    "${PROJECT_SOURCE_DIR}/source/DenseCellField.cpp"
    "${PROJECT_SOURCE_DIR}/source/DenseNodeField.cpp"
    "${PROJECT_SOURCE_DIR}/source/HostFieldFactory.cpp"
    )

if(GO_TEST_COVERAGE)
    target_link_libraries(Fields gcov)
endif()

#target_link_libraries(Stubs cusparse cublas)
