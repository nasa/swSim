
cmake_minimum_required (VERSION 3.13.5)

project(FieldsTests C CXX)#CUDA

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) #for static lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) # for shared lib

enable_testing()

include_directories("${GTEST_SERVICE_DIR}/googletest/include"
                    "${GTEST_SERVICE_DIR}/googlemock/include")
include_directories("${STUBS_SOURCE_DIR}/include")
include_directories("${MODELUT_SOURCE_DIR}/include")
include_directories("${FIELDS_SOURCE_DIR}/include")

include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable(FieldsTests
    "${PROJECT_SOURCE_DIR}/source/ConstantFieldTests.cpp"
    "${PROJECT_SOURCE_DIR}/source/SparseCellFieldTests.cpp"
    "${PROJECT_SOURCE_DIR}/source/SparseNodeFieldTests.cpp"
    "${PROJECT_SOURCE_DIR}/source/DenseNodeFieldTests.cpp"
    "${PROJECT_SOURCE_DIR}/source/DenseCellFieldTests.cpp"
    "${PROJECT_SOURCE_DIR}/source/HostFieldFactoryTests.cpp"
    "${GTEST_DRIVERS_DIR}/source/TestMain.cpp"
    )

target_link_libraries(FieldsTests Fields Stubs)
target_link_libraries(FieldsTests gtest gtest_main)
