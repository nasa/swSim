//

#include "StubMesh.hpp"
#include "ConstantField.hpp"
#include "Field.hpp"

#include "gtest/gtest.h"

class ConstantFieldTests : public ::testing::Test {
protected:
  void SetUp() override {
    mesh=std::make_shared<Stubs::StubMesh>(Stubs::StubMesh(ds,Nx,Ny,Nz));
    field=new Fields::ConstantField();
  };
  void TearDown() override {
    delete field;
  };
  std::shared_ptr<Stubs::StubMesh> mesh;
  int Nx=16,Ny=8,Nz=4;
  double ds=0.25;

  Fields::ConstantField* field;
};

TEST_F(ConstantFieldTests,Nothing){};

TEST_F(ConstantFieldTests,checkMesh){
  field->build(mesh,10.5);
  EXPECT_EQ(mesh,field->mesh());
};

TEST_F(ConstantFieldTests,checkType){
  EXPECT_EQ(ModelUT::Field::CONSTANT,field->type());
  EXPECT_EQ(nullptr,field->spIndices());
  EXPECT_EQ(1,field->size());
  EXPECT_FALSE(field->isSparse());
};

TEST_F(ConstantFieldTests,checkData1){
  field->build(mesh,10.5);
  EXPECT_DOUBLE_EQ(10.5,(*(field->data())));
};
TEST_F(ConstantFieldTests,checkData2){
  field->build(mesh,3.14);
  EXPECT_DOUBLE_EQ(3.14,(*(field->data())));
};
TEST_F(ConstantFieldTests,checkData3){
  field->build(mesh,2.727);
  EXPECT_DOUBLE_EQ(2.727,field->at(100));
};
TEST_F(ConstantFieldTests,checkData4){
  field->build(mesh,1.45);
  EXPECT_DOUBLE_EQ(1.45,field->at(20));
};