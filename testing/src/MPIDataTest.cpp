//

#include <mpi.h>

#include "MPIData.hpp"

#include "gtest/gtest.h"

class MPITest : public ::testing::Test{
    protected:
    //myTest(){};
    //~myTest(){};
    
    void SetUp() override{
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


TEST_F(MPITest,MPIdataTest){
    swSim::MPIData mMPIData(MPI_COMM_WORLD);
    EXPECT_EQ(mMPIData.rank,myrank);
    EXPECT_EQ(mMPIData.size,mycomm_size);
};

TEST_F(MPITest,MPIdataSendRecieveTest){
    swSim::MPIData mMPIData(MPI_COMM_WORLD);
    mycomm = mMPIData.mComm; 
    
    MPI_Send(&values.at(myrank),1,MPI_INT,lneighbor,101,mycomm);
    MPI_Send(&values.at(myrank),1,MPI_INT,rneighbor,102,mycomm);
    MPI_Recv(&recieved[0],1,MPI_INT,lneighbor,102,mycomm,&status[1]);
    MPI_Recv(&recieved[1],1,MPI_INT,rneighbor,101,mycomm,&status[0]);
    if(lneighbor!=MPI_PROC_NULL){
        EXPECT_EQ(values.at(lneighbor),recieved[0]);
    };
    if(rneighbor!=MPI_PROC_NULL){
        EXPECT_EQ(values.at(rneighbor),recieved[1]);
    };
};
