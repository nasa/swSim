//

#include <string>
#include <vector>
#include <cstdio>

#include "Segmentation.hpp"

#include "gtest/gtest.h"

class SegmentationTest: public ::testing::Test{
    protected:
    void SetUp() override{
        for(int i = 0; i<40; i++) {
            plnCounts40_820.push_back(i+1);
        }

        plnCounts2019.push_back(0);
        for(int i = 0; i<2017; i++) {
            plnCounts2019.push_back(32272);
        }
        plnCounts2019.push_back(0);
    };
    //void TearDown() override{};
    std::vector<uint64_t> plnCounts40_820;
    std::vector<uint64_t> plnCounts2019;

};

TEST_F(SegmentationTest,InitializerTest1){
    swSim::Segmentation seg; 
    EXPECT_EQ(seg.xIndex0, 0);
    EXPECT_EQ(seg.xDelta, 0);
    EXPECT_EQ(seg.neighborRight, -1);
    EXPECT_EQ(seg.neighborLeft, -1);
    EXPECT_EQ(seg.processVoxels, 0);
    EXPECT_EQ(seg.globalIndex0, 0);
    EXPECT_EQ(seg.lHaloVoxels, 0);
    EXPECT_EQ(seg.rHaloVoxels, 0);
};

TEST_F(SegmentationTest,SumVectorTester){
    swSim::Segmentation seg; 
    EXPECT_EQ(plnCounts40_820.size(), 40);
    uint64_t sumval = seg.SumVector(plnCounts40_820);
    EXPECT_EQ(sumval, 820);
};

TEST_F(SegmentationTest,MeetTargetCellsTester1){
    swSim::Segmentation seg; 
    uint64_t voxIdx = 0;
    int out_xpos = seg.MeetTargetCells(plnCounts40_820, 0, voxIdx, 38); 

    EXPECT_EQ(out_xpos, 8);
    EXPECT_EQ(voxIdx, 36);
};

TEST_F(SegmentationTest,SetRightValues1){
    swSim::Segmentation seg; 
    uint64_t voxIdx = 0;
    int out_xpos = seg.MeetTargetCells(plnCounts40_820, 0, voxIdx, 273); 
    EXPECT_EQ(out_xpos, 22);
    EXPECT_EQ(voxIdx, 253);
    seg.SetRightValues(plnCounts40_820, out_xpos);
    EXPECT_EQ(seg.xIndex0, 0);
    EXPECT_EQ(seg.xDelta, 23);
    EXPECT_EQ(seg.processVoxels, 276);
    EXPECT_EQ(seg.globalIndex0, 0);
    EXPECT_EQ(seg.rHaloVoxels, 23);
};

TEST_F(SegmentationTest,SetLValues1){
    swSim::Segmentation seg; 
    uint64_t voxIdx = 0;
    int out_xpos = seg.MeetTargetCells(plnCounts40_820, 0, voxIdx, 273); 
    EXPECT_EQ(out_xpos, 22);
    EXPECT_EQ(voxIdx, 253);
    seg.SetLeftValues(plnCounts40_820, out_xpos, voxIdx);
    EXPECT_EQ(seg.globalIndex0, 253);
    EXPECT_EQ(seg.lHaloVoxels, 23);
};

TEST_F(SegmentationTest,LoadBalance0Tester){
    swSim::Segmentation seg; 
    int rank = 0;
    int size = 3; 
    seg.LoadBalance(plnCounts40_820, rank, size);
    EXPECT_EQ(seg.xIndex0, 0);
    EXPECT_EQ(seg.xDelta, 23);
    EXPECT_EQ(seg.neighborRight, 1);
    EXPECT_EQ(seg.neighborLeft, -1);
    EXPECT_EQ(seg.processVoxels, 276);
    EXPECT_EQ(seg.globalIndex0, 0);
    EXPECT_EQ(seg.lHaloVoxels, 0);
    EXPECT_EQ(seg.rHaloVoxels, 23);
};

TEST_F(SegmentationTest,LoadBalance1Tester){
    swSim::Segmentation seg; 
    int rank = 1;
    int size = 3; 
    seg.LoadBalance(plnCounts40_820, rank, size);
    EXPECT_EQ(seg.xIndex0, 22);
    EXPECT_EQ(seg.xDelta, 11);
    EXPECT_EQ(seg.neighborRight, 2);
    EXPECT_EQ(seg.neighborLeft, 0);
    EXPECT_EQ(seg.processVoxels, 308);
    EXPECT_EQ(seg.globalIndex0, 253);
    EXPECT_EQ(seg.lHaloVoxels, 23);
    EXPECT_EQ(seg.rHaloVoxels, 33);
};

TEST_F(SegmentationTest,LoadBalance2Tester){
    swSim::Segmentation seg; 
    int rank = 2;
    int size = 3; 
    seg.LoadBalance(plnCounts40_820, rank, size);
    EXPECT_EQ(seg.xIndex0, 32);
    EXPECT_EQ(seg.xDelta, 8);
    EXPECT_EQ(seg.neighborRight, -1);
    EXPECT_EQ(seg.neighborLeft, 1);
    EXPECT_EQ(seg.processVoxels, 292);
    EXPECT_EQ(seg.globalIndex0, 528);
    EXPECT_EQ(seg.lHaloVoxels, 33);
    EXPECT_EQ(seg.rHaloVoxels, 0);
};

TEST_F(SegmentationTest,LoadBalance0ATester){
    swSim::Segmentation seg; 
    int rank = 0;
    int size = 3; 
    seg.LoadBalance(plnCounts2019, rank, size);
    EXPECT_EQ(seg.xIndex0, 0);
    EXPECT_EQ(seg.xDelta, 674);
    EXPECT_EQ(seg.neighborRight, 1);
    EXPECT_EQ(seg.neighborLeft, -1);
    EXPECT_EQ(seg.processVoxels, 21719056);
    EXPECT_EQ(seg.globalIndex0, 0);
    EXPECT_EQ(seg.lHaloVoxels, 0);
    EXPECT_EQ(seg.rHaloVoxels, 32272);
};

TEST_F(SegmentationTest,LoadBalance1ATester){
    swSim::Segmentation seg; 
    int rank = 1;
    int size = 3; 
    seg.LoadBalance(plnCounts2019, rank, size);
    EXPECT_EQ(seg.xIndex0, 673);
    EXPECT_EQ(seg.xDelta, 673);
    EXPECT_EQ(seg.neighborRight, 2);
    EXPECT_EQ(seg.neighborLeft, 0);
    EXPECT_EQ(seg.processVoxels, 21719056);
    EXPECT_EQ(seg.globalIndex0, 21686784);
    EXPECT_EQ(seg.lHaloVoxels, 32272);
    EXPECT_EQ(seg.rHaloVoxels, 32272);
};

TEST_F(SegmentationTest,LoadBalance2ATester){
    swSim::Segmentation seg; 
    int rank = 2;
    int size = 3; 
    seg.LoadBalance(plnCounts2019, rank, size);
    EXPECT_EQ(seg.xIndex0, 1345);
    EXPECT_EQ(seg.xDelta, 674);
    EXPECT_EQ(seg.neighborRight, -1);
    EXPECT_EQ(seg.neighborLeft, 1);
    EXPECT_EQ(seg.processVoxels, 21719056);
    EXPECT_EQ(seg.globalIndex0, 43373568);
    EXPECT_EQ(seg.lHaloVoxels, 32272);
    EXPECT_EQ(seg.rHaloVoxels, 0);
};
