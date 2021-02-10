//

#include "StubMesh.hpp"

#include "gtest/gtest.h"

void boundingBoxCheck(double currentBox[6],double newPoint[3]);

class StubMeshTests : public ::testing::Test {
protected:
  void SetUp() override {
    mesh=std::make_shared<Stubs::StubMesh>(Stubs::StubMesh(ds,Nx,Ny,Nz));
  };
  void TearDown() override {
  };
  std::shared_ptr<Stubs::StubMesh> mesh;
  int Nx=16,Ny=8,Nz=4;
  double ds=0.25;
};

void boundingBoxCheck(double currentBox[6],double newPoint[3]){
  for(int k=0;k<3;k++){
    currentBox[k]=(currentBox[k]<=newPoint[k])?currentBox[k]:newPoint[k];
    currentBox[k+3]=(currentBox[k+3]>=newPoint[k])?currentBox[k+3]:newPoint[k];
  };
};

TEST_F(StubMeshTests,Nothing){};

TEST_F(StubMeshTests,Counts){
  EXPECT_EQ(Nx*Ny*Nz,mesh->nodeCount());
  EXPECT_EQ((Nx-1)*(Ny-1)*(Nz-1),mesh->cellCount());
};

TEST_F(StubMeshTests,boundingBox){
  double coordinate[3];
  double aabb[6];
  mesh->nodeCoordinate(0,coordinate);
  for(int k=0;k<3;k++){
    aabb[k]=coordinate[k];
    aabb[k+3]=coordinate[k];
  };
  for(int idx=1;idx<mesh->nodeCount();idx++){
    mesh->nodeCoordinate(idx,coordinate);
    boundingBoxCheck(aabb,coordinate);
  };
  for(int k=0;k<3;k++){EXPECT_DOUBLE_EQ(0.0,aabb[k]);};
  EXPECT_DOUBLE_EQ(ds*(double(Nx-1)),aabb[3]);
  EXPECT_DOUBLE_EQ(ds*(double(Ny-1)),aabb[4]);
  EXPECT_DOUBLE_EQ(ds*(double(Nz-1)),aabb[5]);
};

TEST_F(StubMeshTests,cellBBChecks){
  double aabb[6];
  double coordinate[3];
  int cell[8];
  for(int idx=0;idx<mesh->cellCount();idx++){
    mesh->cell(idx,cell);
    mesh->nodeCoordinate(cell[0],coordinate);
    for(int k=0;k<3;k++){
      aabb[k]=coordinate[k];
      aabb[k+3]=coordinate[k];
    };
    for(int k=1;k<8;k++){
      mesh->nodeCoordinate(cell[k],coordinate);
      boundingBoxCheck(aabb,coordinate);
    };
    for(int k=0;k<3;k++){
      EXPECT_DOUBLE_EQ(ds,aabb[k+3]-aabb[k]);
    };
  };
};

TEST_F(StubMeshTests,cellCoordCheck){
  double coord[3];
  for(int idx=0;idx<mesh->cellCount();idx++){
    mesh->cellCoordinate(idx,coord);
    for(int k=0;k<3;k++){
      EXPECT_LT(0.0,coord[k]);
    };
  };
};

TEST_F(StubMeshTests,nodeBBChecks){
  double aabb[6];
  double coordinate[3];
  int node[8];
  int cell[8];
  for(int idx=0;idx<mesh->nodeCount();idx++){
    mesh->nodeCoordinate(idx,coordinate);
    for(int k=0;k<3;k++){
      aabb[k]=coordinate[k];
      aabb[k+3]=coordinate[k];
    }
    mesh->node(idx,node);
    for(int k=0;k<8;k++){
      if(node[k]!=-1){
        mesh->cell(node[k],cell);
        for(int kk=0;kk<8;kk++){
          mesh->nodeCoordinate(cell[kk],coordinate);
          boundingBoxCheck(aabb,coordinate);
        };
      };
    };
    for(int k=0;k<3;k++){
      double val=((aabb[k+3]-aabb[k])/ds);
      double ival=(double)(int(val));
      val-=ival;
      EXPECT_DOUBLE_EQ(0.0,val);
      EXPECT_LE(1,ival);
      EXPECT_GE(2,ival);
    };
  };
};

TEST_F(StubMeshTests,checkCellSize){
  double extents[3];
  for(int idx=0;idx<mesh->cellCount();idx++){
    mesh->cellExtents(idx,extents);
    for(int k=0;k<3;k++){
      EXPECT_DOUBLE_EQ(ds,extents[k]);
    };
  };
};
