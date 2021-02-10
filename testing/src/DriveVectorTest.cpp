//

#include <string>
#include <vector>
#include <cstdio>

#include "DriveVector.hpp"

#include "gtest/gtest.h"

class DriveVectorTest: public ::testing::Test{
    protected:
    //DenseVectorTest(){};
    //~DenseVectorTest(){};
    
    void SetUp() override{
        szi = 12; 
        szo = 10;
        As.push_back(swSim::SparseMatrix("mat1", szo, szi)); 
        As.push_back(swSim::SparseMatrix("mat2", szo, szo));
        for(int i=0; i<szo; i++) {
            As[0].mat.coeffRef(i,i) = i*0.1;
            As[0].mat.coeffRef(i,szi-i-1) = i;  
            As[1].mat.coeffRef(i,i) = i*i*0.01;
        }
        vFinal = {0.0, 0.0, 0.0, 0.027, 0.064, 0.125, 2.16, 3.43, 5.12, 0.0};
    };
    //void TearDown() override{};
    int szi;
    int szo;
    std::vector<swSim::SparseMatrix> As;
    std::vector<double> vFinal; 
};

TEST_F(DriveVectorTest,Initializer){
    swSim::DriveVector dVec(szi); 
    EXPECT_EQ(dVec.uCoeffs.size(), 3);
    for(int i = 0; i<3; i++) 
    {
        EXPECT_EQ(dVec.uCoeffs.at(i).getFullLength(), szi);
        for(int j = 0; j<dVec.uCoeffs.at(i).getFullLength(); j++) 
        {
            EXPECT_NEAR(dVec.uCoeffs.at(i).vec.coeffRef(j), 0.0,  0.0001);
        }
    }  
};

TEST_F(DriveVectorTest,CommitCoefficents){
    swSim::DriveVector dVec(szi); 

    for(int i = 3; i<6; i++) {
        dVec.uCoeffs[0].vec.coeffRef(i) = 1; 
    }

    dVec.CommitCoefficents(&As[1],&As[0]); 

    for(int j = 0; j<dVec.uCoeffs.at(0).getFullLength(); j++) 
        {
            EXPECT_NEAR(dVec.uCoeffs.at(0).vec.coeffRef(j), vFinal[j],  0.0001);
        }

    
    for(int i = 1; i<3; i++) 
    {
        EXPECT_EQ(dVec.uCoeffs.at(i).getFullLength(), szo);
        for(int j = 0; j<dVec.uCoeffs.at(i).getFullLength(); j++) 
        {
            EXPECT_NEAR(dVec.uCoeffs.at(i).vec.coeffRef(j), 0.0,  0.000000000000000001);
        }
    }  
};


