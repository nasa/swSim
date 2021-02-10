//

#include <string>
#include <vector>
#include <cstdio>

#include "Config.hpp"
#include "SWMesh.hpp"

#include "gtest/gtest.h"

class SWMeshMPITest: public ::testing::Test{
    protected:
     void SetUp() override{

        MPI_Comm_dup(MPI_COMM_WORLD,&mycomm);
        MPI_Comm_size(mycomm,&size);
        MPI_Comm_rank(mycomm,&rank);

        std::string inputFile = std::string(testInputFileDirectory) +
            std ::string( "30x30ElongatedQausiIsoCFRPTestPlate/") +
            std::string("QuasiIsoPlateSimulationInputs.xml");
        mMod.Input((char*)inputFile.c_str(), mycomm);
        

    };
    //void TearDown() override{};

    MPI_Comm mycomm;
    int size,rank; 
    std::string infile; 
    swSim::Model mMod;

};

TEST_F(SWMeshMPITest,InitializerTest){

    std::vector<double> pCoords(3,0.0);
    double dz=mMod.modGeometry.geomS.voxelSize[2];
    float err = 0.001;

    swSim::Segmentation mSeg(mMod, mycomm);
    swSim::SWMesh lMesh(mMod, mSeg, mycomm); 

    int NX = mSeg.xDelta; 
    int NY = mMod.modGeometry.geomS.dimensionsGlobalVoxel[1]-2; 
    int NZ = mMod.modGeometry.geomS.dimensionsGlobalVoxel[2]-2;
    if(rank==0) {
        NX = NX-1; 
    }
    if(rank==size-1) {
        NX = NX-1; 
    }

    EXPECT_EQ(lMesh.rotations.size(), 3);
    EXPECT_EQ(lMesh.target->mesh->nodeCount(), (NX+1)*(NY+1)*(NZ+1));
    EXPECT_EQ(lMesh.target->mesh->cellCount(), mSeg.processVoxels); 

    std::vector<float> rots = {0.0, 0.785398, -0.785398, 1.5708, 1.5708, -0.785398, 0.785398,};

    for(int kc=0;kc<24;kc++){
        EXPECT_EQ(lMesh.materialID->at(kc), 1); 
    };
    int cnum = 0;
    for(int r=0; r<8; r++) {
        for(int p=0; p<3; p++) {
            EXPECT_NEAR(lMesh.rotations.at(0)->at(cnum), rots[r], err);
            cnum++; 
        }
    }

};


TEST_F(SWMeshMPITest,CommNodesTest){

    std::vector<double> pCoords(3,0.0);
    double dx=mMod.modGeometry.geomS.voxelSize[0];

    std::vector<int32_t> lrNodes;
    std::vector<int32_t> lsNodes;
    std::vector<int32_t> rsNodes;
    std::vector<int32_t> rrNodes;

    swSim::Segmentation mSeg(mMod, mycomm);
    swSim::SWMesh lMesh(mMod, mSeg, mycomm); 


    for(int kc=0;kc<lMesh.target->mesh->cellCount();kc++){
    
        EXPECT_EQ(lMesh.rotations.at(1)->at(kc), 0); 
        EXPECT_EQ(lMesh.rotations.at(2)->at(kc), 0);  
    };

};
