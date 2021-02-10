//

#pragma once

#include <vector>
#include <cstdint>

#include "Mesh.hpp"

namespace Stubs{
  class StubMesh : public ModelUT::Mesh {
    public:
      //mesh interface overrides

      //necessary pieces copied from infinity::MeshInterface
      int32_t nodeCount() override{return nodeCoords.size();};
      int32_t cellCount() override{return cells.size();};
      int32_t partitionId() override{return 0;};

      void nodeCoordinate(int32_t node_id,double coord[3]) override{
        coord[0]=nodeCoords.at(node_id).X;
        coord[1]=nodeCoords.at(node_id).Y;
        coord[2]=nodeCoords.at(node_id).Z;
      };
      void cellCoordinate(int32_t cell_id,double coord[3]) override{
        coord[0]=cellCoords.at(cell_id).X;
        coord[1]=cellCoords.at(cell_id).Y;
        coord[2]=cellCoords.at(cell_id).Z;
      };

      void cellExtents(int32_t cell_id,double ds[3]) override {
        Point aabb[2];
        Point pt;
        for(int32_t k=0;k<8;k++){
          pt=nodeCoords.at(cells.at(cell_id).at(k));
          if(k==0){aabb[0]=pt;aabb[1]=pt;};
          aabb[0].X=(aabb[0].X<=pt.X)?aabb[0].X:pt.X;
          aabb[0].Y=(aabb[0].Y<=pt.Y)?aabb[0].Y:pt.Y;
          aabb[0].Z=(aabb[0].Z<=pt.Z)?aabb[0].Z:pt.Z;
          aabb[1].X=(aabb[1].X>=pt.X)?aabb[1].X:pt.X;
          aabb[1].Y=(aabb[1].Y>=pt.Y)?aabb[1].Y:pt.Y;
          aabb[1].Z=(aabb[1].Z>=pt.Z)?aabb[1].Z:pt.Z;
        };
        ds[0]=aabb[1].X-aabb[0].X;
        ds[1]=aabb[1].Y-aabb[0].Y;
        ds[2]=aabb[1].Z-aabb[0].Z;
      };

      //cell_nodes is the list of nodes at the corners of the cells
      //  needs better name, but unable to come up with one right now
      void cell(int32_t cell_id,int32_t cell_nodes[8]) override{
        for(int k=0;k<8;k++){cell_nodes[k]=cells.at(cell_id).at(k);};
      };

      //node_cells is the list of cells for which the node is a corner - -1 indicates no cell on that corner
      void node(int32_t node_id,int32_t node_cells[8]) override{
        for(int k=0;k<8;k++){node_cells[k]=-1;};
        for(int k=0;k<nodes.at(node_id).size();k++){
          node_cells[k]=nodes.at(node_id).at(k);
        };
      };

      int64_t globalNodeId(int32_t node_id) override{return (int64_t)node_id;};
      int32_t nodeHomePartition(int32_t node_id) override{return 0;};

      int64_t globalCellId(int32_t cell_id) override{return (int64_t)cell_id;};
      int32_t cellHomePartition(int32_t cell_id) override{return 0;};



      StubMesh(double ds=0.0625,int Nx=32,int Ny=32,int Nz=16):Nx(Nx),Ny(Ny),Nz(Nz){
        nodeCoords.resize(Nx*Ny*Nz);
        setupCellTable();
        setupNodeTable();
        buildNodeCoords(ds);
        buildLinks();
      };
    private:
      struct Point{
        double X;
        double Y;
        double Z;
      };
      void setupCellTable(){
        cells.resize((Nx-1)*(Ny-1)*(Nz-1));
        cellCoords.resize(cells.size());
        for(auto it=cells.begin();it!=cells.end();it++){it->resize(8);};
      };
      void setupNodeTable(){
        nodes.resize(Nx*Ny*Nz);
        for(auto it=nodes.begin();it!=nodes.end();it++){
          it->resize(8);
          for(int k=0;k<8;k++){it->at(k)=-1;};
        };
      };
      void buildNodeCoords(double ds){
        for(int kx=0;kx<Nx;kx++){
          for(int ky=0;ky<Ny;ky++){
            for(int kz=0;kz<Nz;kz++){
              int idx=kz+Nz*(ky+Ny*kx);
              nodeCoords.at(idx).X=kx*ds;
              nodeCoords.at(idx).Y=ky*ds;
              nodeCoords.at(idx).Z=kz*ds;
            };
          };
        };
      };
      void buildLinks(){
        for(int kx=0;kx<(Nx-1);kx++){
          for(int ky=0;ky<(Ny-1);ky++){
            for(int kz=0;kz<(Nz-1);kz++){
              int idx=kz+(Nz-1)*(ky+(Ny-1)*kx);
              int nidx=kz+Nz*(ky+Ny*kx);
              cells.at(idx).at(0)=nidx;
              nodes.at(nidx).at(6)=idx;

              cells.at(idx).at(1)=nidx+Nz*Ny;
              nodes.at(nidx+Nz*Ny).at(7)=idx;

              cells.at(idx).at(2)=nidx+Nz*Ny+Nz;
              nodes.at(nidx+Nz*Ny+Nz).at(4)=idx;

              cells.at(idx).at(3)=nidx+Nz;
              nodes.at(nidx+Nz).at(5)=idx;

              cells.at(idx).at(4)=nidx+1;
              nodes.at(nidx+1).at(2)=idx;

              cells.at(idx).at(5)=nidx+1+Nz*Ny;
              nodes.at(nidx+1+Nz*Ny).at(3)=idx;

              cells.at(idx).at(6)=nidx+1+Nz*Ny+Nz;
              nodes.at(nidx+1+Nz*Ny+Nz).at(0)=idx;

              cells.at(idx).at(7)=nidx+1+Nz;
              nodes.at(nidx+1+Nz).at(1)=idx;

              cellCoords.at(idx).X=0.0;
              cellCoords.at(idx).Y=0.0;
              cellCoords.at(idx).Z=0.0;
              for(int k=0;k<8;k++){
                cellCoords.at(idx).X+=0.125*(nodeCoords.at(cells.at(idx).at(k)).X);
                cellCoords.at(idx).Y+=0.125*(nodeCoords.at(cells.at(idx).at(k)).Y);
                cellCoords.at(idx).Z+=0.125*(nodeCoords.at(cells.at(idx).at(k)).Z);
              };
            };
          };
        };
      };

      std::vector<Point> nodeCoords;
      std::vector<Point> cellCoords;
      std::vector<std::vector<int32_t>> cells;
      std::vector<std::vector<int32_t>> nodes;
      int Nx,Ny,Nz;
  };
};