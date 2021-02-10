//

#include <vector>
#include <array>
#include <cstdint>

#include "HexMesh.hpp"
#include "StubMesh.hpp"

#include "gtest/gtest.h"

void boundingBoxCheck(double currentBox[6],double newPoint[3]);

class HexMeshTests : public ::testing::Test {
protected:
  void SetUp() override {
    stub=std::make_shared<Stubs::StubMesh>(Stubs::StubMesh(ds,Nx,Ny,Nz));
  };
  void TearDown() override {
  };
  std::shared_ptr<HostMesh::HexMesh> mesh;
  std::shared_ptr<Stubs::StubMesh> stub;
  int Nx=2,Ny=2,Nz=2;
  double ds=0.25;
};

void boundingBoxCheck(double currentBox[6],double newPoint[3]){
  for(int k=0;k<3;k++){
    currentBox[k]=(currentBox[k]<=newPoint[k])?currentBox[k]:newPoint[k];
    currentBox[k+3]=(currentBox[k+3]>=newPoint[k])?currentBox[k+3]:newPoint[k];
  };
};

TEST_F(HexMeshTests,Nothing){};

TEST_F(HexMeshTests,CopyStubAndCheck){
  std::array<double,3> pt;
  std::array<int32_t,8> box;
  std::vector<std::array<double,3>> pts;
  pts.resize(stub->nodeCount());
  std::vector<std::array<int32_t,8>> cells;
  cells.resize(stub->cellCount());
  for(int kp=0;kp<stub->nodeCount();kp++){
    stub->nodeCoordinate(kp,pt.data());
    for(int k=0;k<3;k++){pts.at(kp).at(k)=pt.at(k);};
  };
  for(int kc=0;kc<stub->cellCount();kc++){
    stub->cell(kc,box.data());
    for(int k=0;k<8;k++){cells.at(kc).at(k)=box.at(k);};
  };
  mesh=std::make_shared<HostMesh::HexMesh>(HostMesh::HexMesh(pts,cells));

  double ptstub[3];
  double pthex[3];
  int32_t boxstub[8];
  int32_t boxhex[8];
  for(int kc=0;kc<stub->cellCount();kc++){
    stub->cellCoordinate(kc,ptstub);
    mesh->cellCoordinate(kc,pthex);
    for(int k=0;k<3;k++){EXPECT_DOUBLE_EQ(ptstub[k],pthex[k]);};
    stub->cellExtents(kc,ptstub);
    mesh->cellExtents(kc,pthex);
    for(int k=0;k<3;k++){EXPECT_DOUBLE_EQ(ptstub[k],pthex[k]);};
    stub->cell(kc,boxstub);
    mesh->cell(kc,boxhex);
    for(int k=0;k<8;k++){EXPECT_EQ(boxstub[k],boxhex[k]);};
  };
  for(int kn=0;kn<stub->nodeCount();kn++){
    stub->nodeCoordinate(kn,ptstub);
    mesh->nodeCoordinate(kn,pthex);
    for(int k=0;k<3;k++){EXPECT_DOUBLE_EQ(ptstub[k],pthex[k]);};
    stub->node(kn,boxstub);
    mesh->node(kn,boxhex);
    for(int k=0;k<8;k++){EXPECT_EQ(boxstub[k],boxhex[k]);};
  };
};
