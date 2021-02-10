
cmake_minimum_required (VERSION 3.13)

project(Stubs C CXX)

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

add_library(Stubs SHARED #STATIC #
    "${PROJECT_SOURCE_DIR}/source/StubMesh.cpp"
	)

target_link_libraries(Stubs ModelUT)# Fields)
target_include_directories(Stubs  PUBLIC "${PROJECT_SOURCE_DIR}/include")