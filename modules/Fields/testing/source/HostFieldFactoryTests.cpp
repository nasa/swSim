//

#include <cstdio>

#include <cstdint>

#include "StubMesh.hpp"
#include "HostFieldFactory.hpp"
#include "Field.hpp"

#include "gtest/gtest.h"

class HostFieldFactoryTests : public ::testing::Test {
protected:
  void SetUp() override {
    mesh=std::make_shared<Stubs::StubMesh>(Stubs::StubMesh(ds,Nx,Ny,Nz));
    factory=new Fields::HostFieldFactory();
  };
  void TearDown() override {
    delete factory;
  };
  std::shared_ptr<Stubs::StubMesh> mesh;
  int Nx=16,Ny=8,Nz=4;
  double ds=0.25;
  std::shared_ptr<ModelUT::Field> field;
  Fields::HostFieldFactory* factory;
};

TEST_F(HostFieldFactoryTests,Nothing){};

TEST_F(HostFieldFactoryTests,MakeEach){
  std::vector<int32_t> spIdx;
  spIdx.push_back(Nx*Ny/2);
  spIdx.push_back(Nx*Ny/2+1);
  field=factory->makeDenseNodeField(mesh);
  field=factory->makeDenseCellField(mesh);
  field=factory->makeSparseNodeField(mesh,spIdx);
  field=factory->makeSparseCellField(mesh,spIdx);
  field=factory->makeConstantField(mesh,1.0);
};

TEST_F(HostFieldFactoryTests,checkDnNType){
  field=factory->makeDenseNodeField(mesh);
  EXPECT_EQ(ModelUT::Field::NODE,field->type());
  EXPECT_FALSE(field->isSparse());
  EXPECT_EQ(nullptr,field->spIndices());
  EXPECT_EQ(mesh,field->mesh());
  for(int k=0;k<field->size();k++){
    EXPECT_DOUBLE_EQ(0.0,field->data()[k]);
    EXPECT_DOUBLE_EQ(0.0,field->at(k));
  };
};

TEST_F(HostFieldFactoryTests,checkDnCType){
  field=factory->makeDenseCellField(mesh);
  EXPECT_EQ(ModelUT::Field::CELL,field->type());
  EXPECT_FALSE(field->isSparse());
  EXPECT_EQ(nullptr,field->spIndices());
  EXPECT_EQ(mesh,field->mesh());
  for(int k=0;k<field->size();k++){
    EXPECT_DOUBLE_EQ(0.0,field->data()[k]);
    EXPECT_DOUBLE_EQ(0.0,field->at(k));
  };
};

TEST_F(HostFieldFactoryTests,checkSpCType){
  std::vector<int32_t> spidx;
  int32_t kz=0;
  for(int32_t kx=7;kx<9;kx++){
    for(int32_t ky=3;ky<5;ky++){
      spidx.push_back(kz+(Nz-1)*(ky+(Ny-1)*kx));
    };
  };
  field=factory->makeSparseCellField(mesh,spidx);

  EXPECT_EQ(ModelUT::Field::CELL,field->type());
  EXPECT_TRUE(field->isSparse());
  EXPECT_EQ(mesh,field->mesh());
  EXPECT_EQ(spidx.size(),field->size());
  int32_t* idx=field->spIndices();
  for(int k=0;k<field->size();k++){
    EXPECT_EQ(spidx.at(k),idx[k]);
    EXPECT_DOUBLE_EQ(0.0,field->data()[k]);
    EXPECT_DOUBLE_EQ(0.0,field->at(field->spIndices()[k]));
  };
};

TEST_F(HostFieldFactoryTests,checkSpNType){
  std::vector<int32_t> spidx;
  int32_t kz=0;
  for(int32_t kx=7;kx<9;kx++){
    for(int32_t ky=3;ky<5;ky++){
      spidx.push_back(kz+Nz*(ky+Ny*kx));
    };
  };
  field=factory->makeSparseNodeField(mesh,spidx);

  EXPECT_EQ(ModelUT::Field::NODE,field->type());
  EXPECT_TRUE(field->isSparse());
  EXPECT_EQ(mesh,field->mesh());
  EXPECT_EQ(spidx.size(),field->size());
  int32_t* idx=field->spIndices();
  for(int k=0;k<field->size();k++){
    EXPECT_EQ(spidx.at(k),idx[k]);
    EXPECT_DOUBLE_EQ(0.0,field->data()[k]);
    EXPECT_DOUBLE_EQ(0.0,field->at(field->spIndices()[k]));
  };
};

TEST_F(HostFieldFactoryTests,checkConstantType){
  field=factory->makeConstantField(mesh,10.5);
  EXPECT_EQ(mesh,field->mesh());
  EXPECT_EQ(ModelUT::Field::CONSTANT,field->type());
  EXPECT_EQ(nullptr,field->spIndices());
  EXPECT_EQ(1,field->size());
  EXPECT_FALSE(field->isSparse());
};

