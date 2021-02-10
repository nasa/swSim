//

#include <string>
#include <vector>
#include <cstdio>

#include "SparseVector.hpp"

#include "gtest/gtest.h"

class SparseVectorTest: public ::testing::Test{
    protected:
    //SparseVectorTest(){};
    //~SparseVectorTest(){};
    
    void SetUp() override{
        vecname.assign("name_it");
        vec1.resize(Length);
        for(int k=0;k<Length;k+=7){
            vec1.coeffRef(k)=1.333*((double)(k));
            idx.push_back(k);
        };
    };
    //void TearDown() override{};
    
    std::string vecname;
    Eigen::SparseVector<double> vec1;
    std::vector<int> idx;
    int Length=100;
};

TEST_F(SparseVectorTest,Initializer){
    swSim::SparseVector SpV(vecname,Length);
    EXPECT_EQ(SpV.getName(),vecname);
    EXPECT_EQ(SpV.getFullLength(),Length);
    EXPECT_EQ(SpV.getNonZeroLength(),0);
};

TEST_F(SparseVectorTest,GetName){
    swSim::SparseVector SpV;
    SpV.setName(vecname);
    EXPECT_EQ(SpV.getName(),vecname);
};

TEST_F(SparseVectorTest,GetVector){
    swSim::SparseVector SpV;
    SpV.vec=vec1;
    ASSERT_NE(&SpV.vec,&vec1);
    ASSERT_NE(SpV.getValueArray(),nullptr);
    ASSERT_NE(SpV.getValueArray(),vec1.valuePtr());
    ASSERT_NE(SpV.getIndexArray(),nullptr);
    ASSERT_NE(SpV.getIndexArray(),vec1.innerIndexPtr());
    EXPECT_EQ(SpV.getFullLength(),Length);
    EXPECT_EQ(SpV.getNonZeroLength(),idx.size());
    for(int k=0;k<idx.size();k++){
        EXPECT_EQ(SpV.getIndexArray()[k],idx.at(k));
        EXPECT_EQ(SpV.getValueArray()[k],vec1.coeffRef(idx.at(k)));
    };
};

