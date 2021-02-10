//

#include <string>
#include <vector>
#include <cstdio>

#include "DenseVector.hpp"

#include "gtest/gtest.h"

class DenseVectorTest: public ::testing::Test{
    protected:
    //DenseVectorTest(){};
    //~DenseVectorTest(){};
    
    void SetUp() override{
        vecname.assign("name_it");
        vec1.clear();
        for(int k=0;k<Length;k++){
            vec1.push_back(1.333*((double)(k)));
        };
    };
    //void TearDown() override{};
    
    std::string vecname;
    std::vector<double> vec1;
    int Length=10;
};

TEST_F(DenseVectorTest,Initializer){
    swSim::DenseVector DnV(vecname,Length);
    EXPECT_EQ(DnV.getName(),vecname);
    ASSERT_NE(DnV.getValueArray(),nullptr);
    EXPECT_EQ(DnV.getArrayLength(),Length);
    for(int k=0;k<Length;k++){
        EXPECT_EQ(DnV.getValueArray()[k],0.0);
    };
};

TEST_F(DenseVectorTest,GetName){
    swSim::DenseVector DnV;
    DnV.setName(vecname);
    EXPECT_EQ(DnV.getName(),vecname);
};

TEST_F(DenseVectorTest,GetVector1){
    swSim::DenseVector DnV;
    DnV.setName(vecname);
    DnV.setVector(vec1);
    ASSERT_NE(DnV.getValueArray(),nullptr);
    EXPECT_EQ(DnV.getArrayLength(),Length);
    for(int k=0;k<Length;k++){
        EXPECT_EQ(DnV.getValueArray()[k],vec1.at(k));
    };
};

TEST_F(DenseVectorTest,GetVector2){
    swSim::DenseVector DnV;
    DnV.setName(vecname);
    DnV.setVector(vec1.data(),vec1.size());
    ASSERT_NE(DnV.getValueArray(),nullptr);
    EXPECT_EQ(DnV.getArrayLength(),Length);
    for(int k=0;k<Length;k++){
        EXPECT_EQ(DnV.getValueArray()[k],vec1.at(k));
    };
};


