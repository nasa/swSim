//

#include <string>
#include <vector>
#include <cstdio>

#include "Config.hpp"
#include "Stencil.hpp"

#include "gtest/gtest.h"

class StencilMPITest: public ::testing::Test{
    protected:
     void SetUp() override{

        MPI_Comm_dup(MPI_COMM_WORLD,&mycomm);
        MPI_Comm_size(mycomm,&size);
        MPI_Comm_rank(mycomm,&rank);

        AStoVsz.push_back(921600);
        AVtoSsz.push_back(921600);

        AStoVsz.push_back(944640);
        AVtoSsz.push_back(944640);

        AStoVsz.push_back(944640);
        AVtoSsz.push_back(944640);

    };
    //void TearDown() override{};

    std::vector<int> AStoVsz;
    std::vector<int> AVtoSsz;
    MPI_Comm mycomm;
    int size,rank; 

};

TEST_F(StencilMPITest,InitializerTest1){

    MPI_Comm mycomm3;
    MPI_Group mycomm_group;
    MPI_Group mycomm3_group;

    MPI_Comm_dup(MPI_COMM_WORLD,&mycomm);
    MPI_Comm_group(mycomm, &mycomm_group);

    MPI_Comm_size(mycomm,&size);
    MPI_Comm_rank(mycomm,&rank);

    ASSERT_TRUE(size >= 3) << "3 or More processes must be used";

    int n = 3;
    const int ranks[3] = {0, 1, 2};
    MPI_Group_incl(mycomm_group, n, ranks, &mycomm3_group);
    MPI_Comm_create_group(mycomm, mycomm3_group, 0, &mycomm3);

    if(rank <3) {

        MPI_Comm_size(mycomm3,&size);
        MPI_Comm_rank(mycomm3,&rank);
    
        swSim::Model mMod;
        std::string inputFile = std::string(testInputFileDirectory) 
            + std::string("30x30ElongatedQausiIsoCFRPTestPlate/") 
            + std::string("QuasiIsoPlateSimulationInputs.xml");
        mMod.Input((char*)inputFile.c_str(), mycomm3);
        swSim::Segmentation mSeg(mMod, mycomm3);
        swSim::SWMesh localMesh(mMod, mSeg, mycomm3);
        swSim::Stencil mStencil(mMod, localMesh, mycomm3, mSeg);


        EXPECT_EQ(mStencil.V.size(), 3); 
        for(int v=0; v<mStencil.V.size(); v++) {
            EXPECT_EQ(mStencil.V.at(v).getArrayLength(), localMesh.target->mesh->nodeCount());
        }

        EXPECT_EQ(mStencil.S.size(), 6); 
        for(int s=0; s<mStencil.S.size(); s++) {
            EXPECT_EQ(mStencil.S.at(s).getArrayLength(), localMesh.target->mesh->cellCount());
        }

        EXPECT_EQ(mStencil.AStoV.size(), 9);
        for(int a=0; a<mStencil.AStoV.size(); a++) {
            EXPECT_EQ(mStencil.AStoV.at(a).getRowCount(), localMesh.target->mesh->nodeCount());
            EXPECT_EQ(mStencil.AStoV.at(a).getColumnCount(), localMesh.target->mesh->cellCount());
            EXPECT_EQ(mStencil.AStoV.at(a).getNonZeroLength(), AStoVsz[rank]);
        }
        EXPECT_EQ(mStencil.AVtoS.size(), 18);
        for(int a=0; a<mStencil.AVtoS.size(); a++) {
            EXPECT_EQ(mStencil.AVtoS.at(a).getRowCount(), localMesh.target->mesh->cellCount());
            EXPECT_EQ(mStencil.AVtoS.at(a).getColumnCount(), localMesh.target->mesh->nodeCount());
            EXPECT_EQ(mStencil.AVtoS.at(a).getNonZeroLength(), AVtoSsz[rank]);
        }


        EXPECT_EQ(mStencil.AinvRhoBar.getRowCount(), localMesh.target->mesh->nodeCount());
        EXPECT_EQ(mStencil.AinvRhoBar.getColumnCount(), localMesh.target->mesh->nodeCount());

        EXPECT_EQ(mStencil.AForceDispersion.getRowCount(), localMesh.target->mesh->nodeCount());
        EXPECT_EQ(mStencil.AForceDispersion.getColumnCount(), localMesh.target->mesh->cellCount());

        EXPECT_EQ(mStencil.U.size(), 1);

    }
};
