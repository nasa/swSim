//

#include <cstdio>

#include <cstdint>

#include "StubMesh.hpp"
#include "SparseNodeField.hpp"
#include "Field.hpp"

#include "gtest/gtest.h"

class SparseNodeFieldTests : public ::testing::Test {
protected:
  void SetUp() override {
    mesh=std::make_shared<Stubs::StubMesh>(Stubs::StubMesh(ds,Nx,Ny,Nz));
    field=new Fields::SparseNodeField();
  };
  void TearDown() override {
    delete field;
  };
  std::shared_ptr<Stubs::StubMesh> mesh;
  int Nx=16,Ny=8,Nz=4;
  double ds=0.25;
  Fields::SparseNodeField* field;
};

TEST_F(SparseNodeFieldTests,Nothing){};

TEST_F(SparseNodeFieldTests,checkType){
  EXPECT_EQ(ModelUT::Field::NODE,field->type());
  EXPECT_TRUE(field->isSparse());
};

TEST_F(SparseNodeFieldTests,built){
  std::vector<int32_t> spidx;
  int32_t kz=0;
  for(int32_t kx=7;kx<9;kx++){
    for(int32_t ky=3;ky<5;ky++){
      spidx.push_back(kz+Nz*(ky+Ny*kx));
    };
  };
  field->build(mesh,spidx);
  EXPECT_EQ(mesh,field->mesh());
  EXPECT_EQ(spidx.size(),field->size());
  int32_t* idx=field->spIndices();
  for(int k=0;k<field->size();k++){
    EXPECT_EQ(spidx.at(k),idx[k]);
    EXPECT_DOUBLE_EQ(0.0,field->data()[k]);
    EXPECT_DOUBLE_EQ(0.0,field->at(field->spIndices()[k]));
  };
};
