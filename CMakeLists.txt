
cmake_minimum_required (VERSION 3.13)
project(rsg_sim C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin) #for static libs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin) # for shared libs

option(GO_TEST "Enable Tests" ON)

option(MAKE_KERNEL "Makes the Kernal on CUDE machines" ON)
option(MAKE_PREPROCESSOR_ONLY "Makes Preprocessor alone and test" ON)
option(MAKE_RSG "Makes RSG_Main" ON)

if(GO_TEST_COVERAGE)
    if(NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        message(FATAL ERROR "GNU compiler is required to run gcov tests. Compiler is ${CMAKE_CXX_COMPILER_ID}")
    endif()
    
    set(GO_TEST ON)
    set(MAKE_KERNEL ON)
    message(STATUS "-- AUTO-ENABLING TEST AND KERNEL BUILDS FOR TEST COVERAGE ASSESSMENT --")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON) # keeps gcov file endings from being appended onto source code extension
endif()

if(GO_TEST)
    enable_testing()
    message(STATUS "-- TESTING ENABLED --")
    set(GTEST_SERVICE_DIR "${PROJECT_SOURCE_DIR}/modules/services/gtest")
    add_subdirectory("${GTEST_SERVICE_DIR}/googletest")
    include_directories("${GTEST_SERVICE_DIR}/googletest/include" 
                        "${GTEST_SERVICE_DIR}/googlemock/include"
                        )
    include_directories("${PROJECT_SOURCE_DIR}/testing/include")

    set(PROJECT_TEST_SRC "${PROJECT_SOURCE_DIR}/testing/src")
    set(GTEST_DRIVERS_DIR "${CMAKE_SOURCE_DIR}/modules/testDrivers")

endif()

set(testInputFileDirectory "\"${PROJECT_SOURCE_DIR}/testing/\"")
configure_file("${PROJECT_SOURCE_DIR}/include/Config.hpp.in" 
			   "${PROJECT_SOURCE_DIR}/bin/Config.hpp"
)

find_package(MPI REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(${MPI_INCLUDE_PATH})
include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories("${PROJECT_SOURCE_DIR}/bin")
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/modules/KernelComposer/include")

if(MAKE_KERNEL)
    add_subdirectory("${PROJECT_SOURCE_DIR}/modules/KernelComposer")
endif()

set(MODELUT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/modules/ModelUT")
set(FIELDS_SOURCE_DIR "${PROJECT_SOURCE_DIR}/modules/Fields")
set(HOSTMESH_SOURCE_DIR "${PROJECT_SOURCE_DIR}/modules/HostMesh")
set(VTKIO_SOURCE_DIR "${CMAKE_SOURCE_DIR}/modules/VTKIO")
set(CONTROLLER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/modules/Controller")
set(STUBS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/modules/Stubs")

add_subdirectory("${MODELUT_SOURCE_DIR}")
add_subdirectory("${FIELDS_SOURCE_DIR}")
add_subdirectory("${HOSTMESH_SOURCE_DIR}")
add_subdirectory("${VTKIO_SOURCE_DIR}")
add_subdirectory("${STUBS_SOURCE_DIR}")

# include_directories("/usr/include/vtk")
include_directories("${MODELUT_SOURCE_DIR}/include")
include_directories("${FIELDS_SOURCE_DIR}/include")
include_directories("${HOSTMESH_SOURCE_DIR}/include")
include_directories("${VTKIO_SOURCE_DIR}/include")
include_directories("${CONTROLLER_SOURCE_DIR}/include")
include_directories("${STUBS_SOURCE_DIR}/include")

add_library(RsgPreProcessor SHARED
    "${PROJECT_SOURCE_DIR}/src/DenseVector.cpp"
    "${PROJECT_SOURCE_DIR}/src/SparseMatrix.cpp"
    "${PROJECT_SOURCE_DIR}/src/SparseVector.cpp"
    "${PROJECT_SOURCE_DIR}/src/SWMesh.cpp"
	"${PROJECT_SOURCE_DIR}/src/Excitation.cpp"
	"${PROJECT_SOURCE_DIR}/src/Geometry.cpp"
	"${PROJECT_SOURCE_DIR}/src/Material.cpp"
	"${PROJECT_SOURCE_DIR}/src/MPIData.cpp"
	"${PROJECT_SOURCE_DIR}/src/ParsableObject.cpp"
	"${PROJECT_SOURCE_DIR}/src/Segmentation.cpp"
	"${PROJECT_SOURCE_DIR}/src/Model.cpp"
	"${PROJECT_SOURCE_DIR}/src/DriveVector.cpp"
	"${PROJECT_SOURCE_DIR}/src/Stencil.cpp"
	"${PROJECT_SOURCE_DIR}/src/MPIFileReader.cpp"
    )

target_link_libraries(RsgPreProcessor ${LIBXML2_LIBRARIES})
target_link_libraries(RsgPreProcessor ${MPI_C_LIBRARIES})
target_link_libraries(RsgPreProcessor Fields ModelUT HostMesh VTKIO)
if(GO_TEST_COVERAGE)
    target_link_libraries(RsgPreProcessor gcov)
endif()
# if(MAKE_KERNEL)
#     target_link_libraries(RsgPreProcessor KernelComposer)
# endif()

if(MAKE_PREPROCESSOR_ONLY)
    add_executable(preprocessor_main
    "${PROJECT_SOURCE_DIR}/src/Preprocessor_Main.cpp"
    )
    target_link_libraries(preprocessor_main ${LIBXML2_LIBRARIES})
    target_link_libraries(preprocessor_main ${MPI_C_LIBRARIES})
    target_link_libraries(preprocessor_main RsgPreProcessor)
    target_link_libraries(preprocessor_main Fields ModelUT HostMesh VTKIO )
    if(GO_TEST_COVERAGE)
        target_link_libraries(preprocessor_main gcov)
    endif()

endif()

if(MAKE_RSG)
    add_executable(rsg_sim
        "${PROJECT_SOURCE_DIR}/src/RSG_Main.cpp"
        )

    target_link_libraries(rsg_sim ${LIBXML2_LIBRARIES})
    target_link_libraries(rsg_sim ${MPI_C_LIBRARIES})
    if(MAKE_KERNEL)
        target_link_libraries(rsg_sim KernelComposer)
    endif()
    target_link_libraries(rsg_sim RsgPreProcessor)
    target_link_libraries(rsg_sim Fields ModelUT HostMesh VTKIO )
    
    if(GO_TEST_COVERAGE)
        target_link_libraries(rsg_sim gcov)
    endif()
    
endif()



if(GO_TEST)

    add_executable(PreprocessorTests
    	"${PROJECT_TEST_SRC}/SegmentationTest.cpp"
    	"${PROJECT_TEST_SRC}/ModelTest.cpp"
    	"${PROJECT_TEST_SRC}/ExcitationTest.cpp"
    	"${PROJECT_TEST_SRC}/GeometryTest.cpp"
    	"${PROJECT_TEST_SRC}/MaterialTest.cpp"
        "${PROJECT_TEST_SRC}/DriveVectorTest.cpp"
    	"${PROJECT_TEST_SRC}/TestMain.cpp"
    	)

    target_link_libraries(PreprocessorTests ${LIBXML2_LIBRARIES})
    target_link_libraries(PreprocessorTests RsgPreProcessor)
    target_link_libraries(PreprocessorTests gtest gtest_main)
    if(GO_TEST_COVERAGE)
        target_link_libraries(PreprocessorTests gcov)
    endif()


    add_executable(MPITests
        "${PROJECT_TEST_SRC}/MPIDataTest.cpp"
        "${PROJECT_TEST_SRC}/MPIModelTest.cpp"
        "${PROJECT_TEST_SRC}/MPISegmentationTest.cpp"
        "${PROJECT_TEST_SRC}/MPIFileReaderTest.cpp"
        "${PROJECT_TEST_SRC}/MPISWMeshTest.cpp"
        "${PROJECT_TEST_SRC}/MPIStencilTest.cpp"
        "${PROJECT_TEST_SRC}/MPITestMain.cpp"
        )


    target_link_libraries(MPITests ${MPI_LIBRARIES})
    target_link_libraries(MPITests gtest gtest_main)
    target_link_libraries(MPITests RsgPreProcessor)
    if(GO_TEST_COVERAGE)
        target_link_libraries(MPITests gcov)
    endif()

    add_executable(KernelComposerTests
        "${PROJECT_TEST_SRC}/DenseVectorTest.cpp"
        "${PROJECT_TEST_SRC}/SparseVectorTest.cpp"
        "${PROJECT_TEST_SRC}/SparseMatrixTest.cpp"
        "${PROJECT_TEST_SRC}/KernelComposerTest.cpp"
        "${PROJECT_TEST_SRC}/KCThermalTest.cpp"
        "${PROJECT_TEST_SRC}/TestMain.cpp"
        )

    target_link_libraries(KernelComposerTests KernelComposer)
    target_link_libraries(KernelComposerTests gtest gtest_main)
    target_link_libraries(KernelComposerTests RsgPreProcessor)
    if(GO_TEST_COVERAGE)
        target_link_libraries(KernelComposerTests gcov)
    endif()

else()
    message(WARNING "-- TESTING DISABLED --")
endif()
