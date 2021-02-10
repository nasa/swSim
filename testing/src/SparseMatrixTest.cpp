//

#include <string>
#include <vector>
//#include <tuple>
#include <cstdio>

#include "SparseMatrix.hpp"

#include "gtest/gtest.h"

class SparseMatrixTest: public ::testing::Test{
    protected:
    //SparseVectorTest(){};
    //~SparseVectorTest(){};
    
    void SetUp() override{
        matname.assign("name_it");
        mat1.resize(Rows,Cols);
        for(int kr=0;kr<Rows;kr++){
            for(int kc=kr;kc<Cols;kc+=7){
                mat1.coeffRef(kr,kc)=0.0333*((double)(kc+kr*Cols));
            };
        };
        mat1.makeCompressed();
    };

    
    std::string matname;
    Eigen::SparseMatrix<double,1> mat1;
    int Rows=50;
    int Cols=100;
};

TEST_F(SparseMatrixTest,Initializer){
    swSim::SparseMatrix SpM(matname,Rows,Cols);
    EXPECT_EQ(SpM.getName(),matname);
    EXPECT_EQ(SpM.getRowCount(),Rows);
    EXPECT_EQ(SpM.getColumnCount(),Cols);
};

TEST_F(SparseMatrixTest,GetName){
    swSim::SparseMatrix SpM;
    SpM.setName(matname);
    EXPECT_EQ(SpM.getName(),matname);
};

TEST_F(SparseMatrixTest,GetMatrix){
    swSim::SparseMatrix SpM;
    SpM.mat=mat1;
    ASSERT_NE(&SpM.mat,&mat1);
    EXPECT_EQ(SpM.getRowCount(),Rows);
    EXPECT_EQ(SpM.getColumnCount(),Cols);
    ASSERT_NE(SpM.getValueArray(),nullptr);
    ASSERT_NE(SpM.getValueArray(),mat1.valuePtr());
    ASSERT_NE(SpM.getColumnIndexArray(),nullptr);
    ASSERT_NE(SpM.getColumnIndexArray(),mat1.innerIndexPtr());
    ASSERT_NE(SpM.getRowpointerArray(),nullptr);
    ASSERT_NE(SpM.getRowpointerArray(),mat1.outerIndexPtr());
    EXPECT_EQ(SpM.getNonZeroLength(),mat1.nonZeros());
    EXPECT_EQ(SpM.getRowpointerLength(),mat1.outerSize()+1);
    for(int k=0;k<mat1.nonZeros();k++){
        EXPECT_EQ(SpM.getColumnIndexArray()[k],mat1.innerIndexPtr()[k]);
        EXPECT_EQ(SpM.getValueArray()[k],mat1.valuePtr()[k]);
    };
    for(int k=0;k<mat1.outerSize();k++){
        EXPECT_EQ(SpM.getRowpointerArray()[k],mat1.outerIndexPtr()[k]);
    };
};

