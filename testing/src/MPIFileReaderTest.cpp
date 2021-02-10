//

#include <string>
#include <vector>
#include <cstdio>
#include <iostream>

#include "Config.hpp"
#include "MPIFileReader.hpp"

#include "gtest/gtest.h"

class MPIFileReaderTest: public ::testing::Test{
    protected:
    void SetUp() override{
        MPI_Comm_dup(MPI_COMM_WORLD,&mycomm);
        MPI_Comm_size(MPI_COMM_WORLD,&mycomm_size);
        MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
        for(int k=0;k<mycomm_size;k++){
            values.push_back(k);
        };
        lneighbor=((myrank-1)>=0)?(myrank-1):MPI_PROC_NULL;
        rneighbor=((myrank+1)<mycomm_size)?(myrank+1):MPI_PROC_NULL;
    };
    //void TearDown() override{};
    
    MPI_Comm mycomm;
    int mycomm_size,myrank;
    std::vector<int> values;
    int recieved[2]={-1,-1};
    int lneighbor,rneighbor;
    MPI_Status status[2];
};

TEST_F(MPIFileReaderTest,Initializer){
    std::string filename = std::string(testInputFileDirectory) 
    + std::string("30x30ElongatedQausiIsoCFRPTestPlate/") 
    + std::string("Geometry.raw");

    swSim::MPIFileReader mpiFile(mycomm, filename.c_str());
    EXPECT_EQ(0, mpiFile.openError);
};

TEST_F(MPIFileReaderTest,Initializer2){
    std::string filename = std::string(testInputFileDirectory) 
    + std::string("30x30ElongatedQausiIsoCFRPTestPlate/") 
    + std::string("Geometry.bin");
    
    std::cout << "Note: this test expects  MPI File reader to return null result for .bin file" << std::endl;
    swSim::MPIFileReader mpiFile(mycomm, filename.c_str());
    EXPECT_GT(mpiFile.openError, 0);
};



