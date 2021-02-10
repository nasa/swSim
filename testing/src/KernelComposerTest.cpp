//

#include <string>
#include <vector>
//#include <tuple>
#include <cstdio>

#include <Eigen/Sparse>

#include "SparseMatrix.hpp"
#include "SparseVector.hpp"
#include "DenseVector.hpp"
#include "KernelComposer.hpp"
#include "KernelComposerHelpers.hpp"

#include "gtest/gtest.h"

class KernelComposerTest: public ::testing::Test{
    protected:
    //KernelComposerTest(){};
    //~KernelComposerTest(){};
    
    void SetUp() override{
        std::string dnv1name("dnv_name_1");
        std::string dnv2name("dnv_name_2");
        std::string spvname("spv_name");
        std::string spmname("spm_name");
        std::vector<double> dnv1;
        std::vector<double> dnv2;
        Eigen::SparseVector<double> spv;
        Eigen::SparseMatrix<double,1> spm;
        
        for(int k=0;k<Nv;k++){
            dnv1.push_back(10.0*((double)(k)));
        };
        DnV1.setName(dnv1name);
        DnV1.setVector(dnv1);
        
        for(int k=0;k<Nv;k++){
            dnv2.push_back(10.0);
        };
        DnV2.setName(dnv2name);
        DnV2.setVector(dnv2);
        
        spv.resize(Nv);
        for(int k=Nv/4;k<(3*Nv/4);k++){
            spv.coeffRef(k)=10.0;
        };
        SpV.setName(spvname);
        SpV.vec=spv;
        
        spm.resize(Nv,Nv);
        for(int k=0;k<Nv;k++){
            spm.coeffRef(k,k)=1.0;
        };
        spm.makeCompressed();
        SpM.setName(spmname);
        SpM.mat=spm;

        for(int k=0;k<NtSig;k++){
            ft.push_back(1.0);
        };
    };
    //void TearDown() override{};
    swSim::DenseVector DnV1;
    swSim::DenseVector DnV2;
    swSim::SparseVector SpV;
    swSim::SparseMatrix SpM;
    std::vector<double> ft;
    
    int Nv=20;
    int NtSig=10;
};

TEST_F(KernelComposerTest,committed){
    swSim::KernelComposer KC;
    EXPECT_FALSE(KC.isCommitted());
};

TEST_F(KernelComposerTest,setDnV){
    swSim::KernelComposer KC;
    EXPECT_EQ(KC.setDenseVector(&DnV1),swSim::KC_OK);
};

TEST_F(KernelComposerTest,setSpV){
    swSim::KernelComposer KC;
    EXPECT_EQ(KC.setSparseVector(&SpV),swSim::KC_OK);
};

TEST_F(KernelComposerTest,setSpM){
    swSim::KernelComposer KC;
    EXPECT_EQ(KC.setSparseMatrix(&SpM),swSim::KC_OK);
};

TEST_F(KernelComposerTest,setSpM2){
    swSim::KernelComposer KC;
    EXPECT_EQ(KC.setSparseMatrix(&SpM),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
};

TEST_F(KernelComposerTest,Commit){
    swSim::KernelComposer KC;
    ASSERT_FALSE(KC.isCommitted());
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
};

TEST_F(KernelComposerTest,SpMVstaging){
    swSim::KernelComposer KC;
    KC.setSparseMatrix(&SpM);
    KC.setDenseVector(&DnV1);
    KC.setDenseVector(&DnV2);
   
    //Configure test, ensure configuration is correct
    ASSERT_FALSE(KC.isCommitted());
    ASSERT_EQ(KC.setSPMV(SpM.getName(),DnV1.getName(),DnV2.getName(),1.0,1.0),
              swSim::KC_OK);
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.run(1),swSim::KC_OK);
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
};

TEST_F(KernelComposerTest,SPMV){
    swSim::KernelComposer KC;
    KC.setSparseMatrix(&SpM);
    KC.setDenseVector(&DnV1);
    KC.setDenseVector(&DnV2);
    
    //Configure test, ensure configuration is correct
    ASSERT_FALSE(KC.isCommitted());
    ASSERT_EQ(KC.setSPMV(SpM.getName(),DnV1.getName(),DnV2.getName(),1.0,1.0),
              swSim::KC_OK);
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.run(1),swSim::KC_OK);
    ASSERT_EQ(KC.getDeviceData(DnV2.getName()),swSim::KC_OK);
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
    
    //test values
    for(int k=0;k<Nv;k++){
        EXPECT_FLOAT_EQ(DnV2.getValueArray()[k],10.0+((double)(k))*10.0);
    };
};
TEST_F(KernelComposerTest,SPMV2){
    swSim::KernelComposer KC;
    KC.setSparseMatrix(&SpM);
    KC.setDenseVector(&DnV1);
    KC.setDenseVector(&DnV2);
    
    //Configure test, ensure configuration is correct
    ASSERT_FALSE(KC.isCommitted());
    ASSERT_EQ(KC.setSPMV(SpM.getName(),DnV1.getName(),DnV2.getName(),1.0,1.0),
              swSim::KC_OK);
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.run(0,1),swSim::KC_OK);
    ASSERT_EQ(KC.getDeviceData(DnV2.getName()),swSim::KC_OK);
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
    
    //test values
    for(int k=0;k<Nv;k++){
        EXPECT_FLOAT_EQ(DnV2.getValueArray()[k],10.0+((double)(k))*10.0);
    };
};

TEST_F(KernelComposerTest,SPaXPY){
    swSim::KernelComposer KC;
    KC.setDenseVector(&DnV2);
    KC.setSparseVector(&SpV);
    
    //Configure test, ensure configuration is correct
    ASSERT_FALSE(KC.isCommitted());
    ASSERT_EQ(KC.setSPaXPY(SpV.getName(),DnV2.getName(),&ft),
              swSim::KC_OK);
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.run(1),swSim::KC_OK);
    ASSERT_EQ(KC.getDeviceData(DnV2.getName()),swSim::KC_OK);
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
    
    //test values
    for(int k=0;k<Nv;k++){
        EXPECT_FLOAT_EQ(DnV2.getValueArray()[k],10.0
                    +10.0*((double)((k>=(Nv/4)) && (k<(3*Nv/4)))));
    };
};
TEST_F(KernelComposerTest,SPaXPY2){
    swSim::KernelComposer KC;
    KC.setDenseVector(&DnV2);
    KC.setSparseVector(&SpV);
    
    //Configure test, ensure configuration is correct
    ASSERT_FALSE(KC.isCommitted());
    ASSERT_EQ(KC.setSPaXPY(SpV.getName(),DnV2.getName(),&ft),
              swSim::KC_OK);
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.run(0,1),swSim::KC_OK);
    ASSERT_EQ(KC.getDeviceData(DnV2.getName()),swSim::KC_OK);
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
    
    //test values
    for(int k=0;k<Nv;k++){
        EXPECT_FLOAT_EQ(DnV2.getValueArray()[k],10.0
                    +10.0*((double)((k>=(Nv/4)) && (k<(3*Nv/4)))));
    };
};

TEST_F(KernelComposerTest,AXPY){
    swSim::KernelComposer KC;
    KC.setDenseVector(&DnV1);
    KC.setDenseVector(&DnV2);
    
    //Configure test, ensure configuration is correct
    ASSERT_FALSE(KC.isCommitted());
    ASSERT_EQ(KC.setAXPY(DnV1.getName(),DnV2.getName(),1.0),
              swSim::KC_OK);
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.run(1),swSim::KC_OK);
    ASSERT_EQ(KC.getDeviceData(DnV2.getName()),swSim::KC_OK);
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
    
    //test values
    for(int k=0;k<Nv;k++){
        EXPECT_FLOAT_EQ(DnV2.getValueArray()[k],10.0+10.0*((double)(k)));
    };
};

TEST_F(KernelComposerTest,SPMVandSPaXPY){
    swSim::KernelComposer KC;
    KC.setSparseMatrix(&SpM);
    KC.setDenseVector(&DnV1);
    KC.setDenseVector(&DnV2);
    KC.setSparseVector(&SpV);
    
    //Configure test, ensure configuration is correct
    ASSERT_FALSE(KC.isCommitted());
    ASSERT_EQ(KC.setSPMV(SpM.getName(),DnV1.getName(),DnV2.getName(),1.0,1.0),
              swSim::KC_OK);
    ASSERT_EQ(KC.setSPaXPY(SpV.getName(),DnV2.getName(),&ft),
              swSim::KC_OK);
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.run(1),swSim::KC_OK);
    ASSERT_EQ(KC.getDeviceData(DnV2.getName()),swSim::KC_OK);
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
    
    //test values
    for(int k=0;k<Nv;k++){
        EXPECT_FLOAT_EQ(DnV2.getValueArray()[k],10.0+((double)(k))*10.0
                        +10.0*((double)((k>=(Nv/4)) && (k<(3*Nv/4)))));
    };
};

TEST_F(KernelComposerTest,SPMVandSPaXPYandAXPY){
    swSim::KernelComposer KC;
    KC.setSparseMatrix(&SpM);
    KC.setDenseVector(&DnV1);
    KC.setDenseVector(&DnV2);
    KC.setSparseVector(&SpV);
    
    //Configure test, ensure configuration is correct
    ASSERT_FALSE(KC.isCommitted());
    ASSERT_EQ(KC.setSPMV(SpM.getName(),DnV1.getName(),DnV2.getName(),1.0,1.0),
              swSim::KC_OK);
    ASSERT_EQ(KC.setSPaXPY(SpV.getName(),DnV2.getName(),&ft),
              swSim::KC_OK);
    ASSERT_EQ(KC.setAXPY(DnV1.getName(),DnV2.getName(),1.0),
              swSim::KC_OK);
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.run(1),swSim::KC_OK);
    ASSERT_EQ(KC.getDeviceData(DnV2.getName()),swSim::KC_OK);
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
    
    //test values
    for(int k=0;k<Nv;k++){
        EXPECT_FLOAT_EQ(DnV2.getValueArray()[k],10.0+((double)(k))*10.0
                        +((double)(k))*10.0
                        +10.0*((double)((k>=(Nv/4)) && (k<(3*Nv/4)))));
    };
};

TEST_F(KernelComposerTest,updateSpVfromHost){
    swSim::KernelComposer KC;
    KC.setSparseMatrix(&SpM);
    KC.setDenseVector(&DnV1);
    KC.setDenseVector(&DnV2);
    KC.setSparseVector(&SpV);
    
    //Configure test, ensure configuration is correct
    ASSERT_FALSE(KC.isCommitted());
    //ASSERT_EQ(KC.setUpdateSparseVectorFromHost(SpV.getName()),swSim::KC_OK);
    ASSERT_EQ(KC.setSPaXPY(SpV.getName(),DnV2.getName(),&ft),
              swSim::KC_OK);
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.run(1),swSim::KC_OK);
    ASSERT_EQ(KC.getDeviceData(DnV2.getName()),swSim::KC_OK);
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
    
    //test values
    for(int k=0;k<Nv;k++){
        EXPECT_FLOAT_EQ(DnV2.getValueArray()[k],
                10.0+(10.0)*(k>=Nv/4 && k<3*Nv/4));
    };
};

TEST_F(KernelComposerTest,setDeviceData){
    swSim::KernelComposer KC;
    KC.setSparseMatrix(&SpM);
    KC.setDenseVector(&DnV1);
    KC.setDenseVector(&DnV2);
    ASSERT_EQ(KC.setSPMV(SpM.getName(),DnV1.getName(),DnV2.getName(),1.0,1.0),
              swSim::KC_OK);
    
    ASSERT_EQ(KC.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    for(int k=0;k<Nv;k++){
        DnV1.getValueArray()[k]=1.0;
    };
    ASSERT_EQ(KC.setDeviceData(DnV1.getName()),swSim::KC_OK);
    ASSERT_TRUE(KC.isCommitted());
    ASSERT_EQ(KC.run(1),swSim::KC_OK);
    ASSERT_EQ(KC.getDeviceData(DnV2.getName()),swSim::KC_OK);
    ASSERT_EQ(KC.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(KC.isCommitted());
    
    //test values
    for(int k=0;k<Nv;k++){
        EXPECT_FLOAT_EQ(DnV2.getValueArray()[k],11.0);
    };
};
