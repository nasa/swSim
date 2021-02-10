//

#include <cstdio>

#include <cstdint>

#include "StubMesh.hpp"
#include "DenseNodeField.hpp"
#include "Field.hpp"

#include "gtest/gtest.h"

class DenseNodeFieldTests : public ::testing::Test {
protected:
  void SetUp() override {
    mesh=std::make_shared<Stubs::StubMesh>(Stubs::StubMesh(ds,Nx,Ny,Nz));
    field=new Fields::DenseNodeField();
  };
  void TearDown() override {
    delete field;
  };
  std::shared_ptr<Stubs::StubMesh> mesh;
  int Nx=16,Ny=8,Nz=4;
  double ds=0.25;
  Fields::DenseNodeField* field;
};

TEST_F(DenseNodeFieldTests,Nothing){};

TEST_F(DenseNodeFieldTests,checkType){
  EXPECT_EQ(ModelUT::Field::NODE,field->type());
  EXPECT_FALSE(field->isSparse());
  EXPECT_EQ(nullptr,field->spIndices());
};

TEST_F(DenseNodeFieldTests,built){
  field->build(mesh);
  EXPECT_EQ(mesh,field->mesh());
  for(int k=0;k<field->size();k++){
    EXPECT_DOUBLE_EQ(0.0,field->data()[k]);
    EXPECT_DOUBLE_EQ(0.0,field->at(k));
  };
};
