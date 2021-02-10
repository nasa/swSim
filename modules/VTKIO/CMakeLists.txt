
cmake_minimum_required (VERSION 3.13)

project(VTKIO C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) #for static lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) # for shared lib


# if(GO_TEST)
#     enable_testing()
#     add_subdirectory("${PROJECT_SOURCE_DIR}/testing")
# endif()

find_package(VTK 9 REQUIRED)
# if (NOT VTK_FOUND)
#   message(SEND_ERROR "Skipping VTKIO: ${VTK_NOT_FOUND_MESSAGE}")
#   return ()
# endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
# include(${VTK_USE_FILE})
# include_directories("${VTK_INCLUDE_DIRS}")


include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${MODELUT_SOURCE_DIR}/include")
include_directories("${CONTROLLER_SOURCE_DIR}/include")

add_library(VTKIO SHARED #STATIC #
    #"${PROJECT_SOURCE_DIR}/source/VTKDataPresenter.cpp"
    "${PROJECT_SOURCE_DIR}/source/VTKPDataPresenter.cpp"
    )

target_link_libraries(VTKIO ${VTK_LIBRARIES})

if(GO_TEST_COVERAGE)
    target_link_libraries(VTKIO gcov)
endif()

# target_include_directories(VTKIO  PUBLIC "${PROJECT_SOURCE_DIR}/include")
