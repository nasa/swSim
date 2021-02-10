//

#include <cstdio>

#include <cstdint>

#include "StubMesh.hpp"
#include "DenseCellField.hpp"
#include "Field.hpp"

#include "gtest/gtest.h"

class DenseCellFieldTests : public ::testing::Test {
protected:
  void SetUp() override {
    mesh=std::make_shared<Stubs::StubMesh>(Stubs::StubMesh(ds,Nx,Ny,Nz));
    field=new Fields::DenseCellField();
  };
  void TearDown() override {
    delete field;
  };
  std::shared_ptr<Stubs::StubMesh> mesh;
  int Nx=16,Ny=8,Nz=4;
  double ds=0.25;
  Fields::DenseCellField* field;
};

TEST_F(DenseCellFieldTests,Nothing){};

TEST_F(DenseCellFieldTests,checkType){
  EXPECT_EQ(ModelUT::Field::CELL,field->type());
  EXPECT_FALSE(field->isSparse());
  EXPECT_EQ(nullptr,field->spIndices());
};

TEST_F(DenseCellFieldTests,built){
  field->build(mesh);
  EXPECT_EQ(mesh,field->mesh());
  for(int k=0;k<field->size();k++){
    EXPECT_DOUBLE_EQ(0.0,field->data()[k]);
    EXPECT_DOUBLE_EQ(0.0,field->at(k));
  };
};
