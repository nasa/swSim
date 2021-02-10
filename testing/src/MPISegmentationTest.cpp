//

#include <string>
#include <vector>
#include <cstdio>

#include "Config.hpp"
#include "Segmentation.hpp"

#include "gtest/gtest.h"

class SegmentationMPITest: public ::testing::Test{
    protected:
     void SetUp() override{
        dimensions.clear();
        dimensions.push_back(122);
        dimensions.push_back(122);
        dimensions.push_back(26);
        voxSz.clear();
        voxSz.push_back(0.00025);
        voxSz.push_back(0.00025);
        voxSz.push_back(0.00004);
        path = std::string(testInputFileDirectory) + std::string("30x30ElongatedQausiIsoCFRPTestPlate/");
        geomfile = "Geometry";
        rotfile = "Rotations.bin";
        geom1.Init(&dimensions[0], &voxSz[0], geomfile.c_str(), rotfile.c_str());
        memcpy(&(mod1.modS.inputPath), path.c_str(), sizeof(char)*150); 
        mod1.modGeometry.Copy(&geom1); 

        MPI_Comm_dup(MPI_COMM_WORLD,&mycomm);
        MPI_Comm_size(mycomm,&size);
        MPI_Comm_rank(mycomm,&rank);

    };
    //void TearDown() override{};
    std::vector<double> voxSz;
    std::vector<uint64_t> dimensions;
    std::string geomfile; 
    std::string rotfile;
    std::string path; 
    swSim::Geometry geom1;
    swSim::Model mod1;

    MPI_Comm mycomm;
    int size,rank; 

};

TEST_F(SegmentationMPITest,InitializerTest1){

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
        swSim::Segmentation seg(mod1, mycomm3); 

        if(rank==0) {
            EXPECT_EQ(seg.xIndex0, 0);
            EXPECT_EQ(seg.xDelta, 41);
            EXPECT_EQ(seg.neighborRight, 1);
            EXPECT_EQ(seg.neighborLeft, -1);
            EXPECT_EQ(seg.processVoxels, 40*(dimensions[1]-2)*(dimensions[2]-2));
            EXPECT_EQ(seg.globalIndex0, 0);
            EXPECT_EQ(seg.lHaloVoxels, 0);
            EXPECT_EQ(seg.rHaloVoxels, (dimensions[1]-2)*(dimensions[2]-2));
        }
        else if(rank==1) {
            EXPECT_EQ(seg.xIndex0, 40);
            EXPECT_EQ(seg.xDelta, 41);
            EXPECT_EQ(seg.neighborRight, 2);
            EXPECT_EQ(seg.neighborLeft, 0);
            EXPECT_EQ(seg.processVoxels, 41*(dimensions[1]-2)*(dimensions[2]-2));
            EXPECT_EQ(seg.globalIndex0, 39*(dimensions[1]-2)*(dimensions[2]-2));
            EXPECT_EQ(seg.lHaloVoxels, (dimensions[1]-2)*(dimensions[2]-2));
            EXPECT_EQ(seg.rHaloVoxels, (dimensions[1]-2)*(dimensions[2]-2));
        }
        else if(rank==2) {
            EXPECT_EQ(seg.xIndex0, 80);
            EXPECT_EQ(seg.xDelta, 42);
            EXPECT_EQ(seg.neighborRight, -1);
            EXPECT_EQ(seg.neighborLeft, 1);
            EXPECT_EQ(seg.processVoxels, 41*(dimensions[1]-2)*(dimensions[2]-2));
            EXPECT_EQ(seg.globalIndex0, 79*(dimensions[1]-2)*(dimensions[2]-2));
            EXPECT_EQ(seg.lHaloVoxels, (dimensions[1]-2)*(dimensions[2]-2));
            EXPECT_EQ(seg.rHaloVoxels, 0);
        }
    }
};
