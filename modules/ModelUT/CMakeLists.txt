
cmake_minimum_required (VERSION 3.13)

project(ModelUT C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) #for static lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) # for shared lib
if(GO_TEST_COVERAGE)
    add_compile_options(-lgcov)
endif()

add_library(ModelUT INTERFACE)


target_include_directories(ModelUT INTERFACE "${PROJECT_SOURCE_DIR}/include")
