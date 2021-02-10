//

#include <cstdio>

#include <cstdint>

#include "StubMesh.hpp"
#include "HostMeshFactory.hpp"
#include "Mesh.hpp"

#include "gtest/gtest.h"

class HostMeshFactoryTests : public ::testing::Test {
protected:
  void SetUp() override {
    stub=std::make_shared<Stubs::StubMesh>(Stubs::StubMesh(ds,Nx,Ny,Nz));
    factory=new HostMesh::HostMeshFactory();
  };
  void TearDown() override {
    delete factory;
  };
  std::shared_ptr<Stubs::StubMesh> stub;
  int Nx=16,Ny=8,Nz=4;
  double ds=0.25;
  std::shared_ptr<ModelUT::Mesh> host;
  HostMesh::HostMeshFactory* factory;
};

TEST_F(HostMeshFactoryTests,Nothing){};

TEST_F(HostMeshFactoryTests,CopyStubAndCheck){
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
  host=factory->makeHexMesh(pts,cells);

  double ptstub[3];
  double pthex[3];
  int32_t boxstub[8];
  int32_t boxhex[8];
  for(int kc=0;kc<stub->cellCount();kc++){
    stub->cellCoordinate(kc,ptstub);
    host->cellCoordinate(kc,pthex);
    for(int k=0;k<3;k++){EXPECT_DOUBLE_EQ(ptstub[k],pthex[k]);};
    stub->cellExtents(kc,ptstub);
    host->cellExtents(kc,pthex);
    for(int k=0;k<3;k++){EXPECT_DOUBLE_EQ(ptstub[k],pthex[k]);};
    stub->cell(kc,boxstub);
    host->cell(kc,boxhex);
    for(int k=0;k<8;k++){EXPECT_EQ(boxstub[k],boxhex[k]);};
  };
  for(int kn=0;kn<stub->nodeCount();kn++){
    stub->nodeCoordinate(kn,ptstub);
    host->nodeCoordinate(kn,pthex);
    for(int k=0;k<3;k++){EXPECT_DOUBLE_EQ(ptstub[k],pthex[k]);};
    stub->node(kn,boxstub);
    host->node(kn,boxhex);
    for(int k=0;k<8;k++){EXPECT_EQ(boxstub[k],boxhex[k]);};
  };
};
