//

#include <cstdio>

#include <cstdint>

#include "StubMesh.hpp"
#include "SparseCellField.hpp"
#include "Field.hpp"

#include "gtest/gtest.h"

class SparseCellFieldTests : public ::testing::Test {
protected:
  void SetUp() override {
    mesh=std::make_shared<Stubs::StubMesh>(Stubs::StubMesh(ds,Nx,Ny,Nz));
    field=new Fields::SparseCellField();
  };
  void TearDown() override {
    delete field;
  };
  std::shared_ptr<Stubs::StubMesh> mesh;
  int Nx=16,Ny=8,Nz=4;
  double ds=0.25;
  Fields::SparseCellField* field;
};

TEST_F(SparseCellFieldTests,Nothing){};

TEST_F(SparseCellFieldTests,checkType){
  EXPECT_EQ(ModelUT::Field::CELL,field->type());
  EXPECT_TRUE(field->isSparse());
};

TEST_F(SparseCellFieldTests,built){
  std::vector<int32_t> spidx;
  int32_t kz=0;
  for(int32_t kx=7;kx<9;kx++){
    for(int32_t ky=3;ky<5;ky++){
      spidx.push_back(kz+(Nz-1)*(ky+(Ny-1)*kx));
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
