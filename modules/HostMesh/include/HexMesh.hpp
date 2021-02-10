/*! \headerfile HexMesh.hpp "modules/HostMesh/include/HexMesh.hpp"
* "HexMesh.hpp" contains a concrete implementation of a grid aligned voxel mesh
* ModelUT::Mesh interface (modules/ModelUT/include/Mesh.hpp)
*
*/

// Copyright 2020 United States Government as represented by the Administrator of the National 
// Aeronautics and Space Administration. No copyright is claimed in the United States under 
// Title 17, U.S. Code. All Other Rights Reserved. See Appendix A for 3rd party licenses.
//
// The Solid-Wave Sim (swSIM) platform is licensed under the Apache License, Version 2.0 (the 
// "License"); you may not use this file except in compliance with the License. You may obtain 
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0. 
// 
// Unless required by applicable law or agreed to in writing, software distributed under the 
// License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
// either express or implied. See the License for the specific language governing permissions 
// and limitations under the License.

#ifndef HEXMESH_HPP
#define HEXMESH_HPP

#include <vector>
#include <cstdint>
#include <array>
#include <stdexcept>

#include "Mesh.hpp"
#include "MeshElementsWithId.hpp"

namespace HostMesh{
  /*! \class HexMesh HexMesh.hpp "modules/HostMesh/include/HexMesh.hpp"
  *
  * Is a concrete implementation of a grid-aligned voxel mesh. Provides the relevant 
  * functionality to appear as a general mesh as used in the rest of the code-base.
  *
  */
  class HexMesh : public ModelUT::Mesh {
    public:
      /*!
      * Query the number of nodes in the mesh
      */
      int32_t nodeCount() override{return nodeCoords.size();};
      /*!
      * Query the number of cells in the mesh
      */
      int32_t cellCount() override{return cells.size();};
      /*!
      * Query the partition Id in the mesh
      */
      int32_t partitionId() override{return mypartition;};

      /*!
      * Query for the node coordinate by node index
      * \param node_id the node index being requested
      * \param coord the 3 element double c-array to which the point coordinates will be written
      */
      void nodeCoordinate(int32_t node_id,double coord[3]) override{
        for(int k=0;k<3;k++){
          coord[k]=nodeCoords.at(node_id).pt[k];
        };
      };
      /*!
      * Query for the cell coordinate by cell index
      * \param cell_id the cell index being requested
      * \param coord the 3 element double c-array to which the cell coordinates will be written
      */
      void cellCoordinate(int32_t cell_id,double coord[3]) override{
        for(int k=0;k<3;k++){
          coord[k]=cellCoords.at(cell_id).pt[k];
        };
      };

      /*!
      * Query for the cell sizes by cell index
      * \param cell_id the cell index being requested
      * \param coord the 3 element double c-array to which the cell dimensions will be written
      */
      void cellExtents(int32_t cell_id,double ds[3]) override {
        double aabb[2][3];
        double pt[3];
        for(int32_t kc=0;kc<8;kc++){
          for(int k=0;k<3;k++){pt[k]=nodeCoords.at(cells.at(cell_id).idx[kc]).pt[k];};
          if(kc==0){
            for(int k=0;k<3;k++){
              aabb[0][k]=pt[k];aabb[1][k]=pt[k];
            };
          };
          for(int k=0;k<3;k++){
            aabb[0][k]=(aabb[0][k]<=pt[k])?aabb[0][k]:pt[k];
            aabb[1][k]=(aabb[1][k]>=pt[k])?aabb[1][k]:pt[k];
          };
        };
        for(int k=0;k<3;k++){
          ds[k]=aabb[1][k]-aabb[0][k];
        };
      };

      /*!
      * Query for the nodes connected to the cell by cell index
      * \param cell_id the cell index being requested
      * \param cell_nodes the 8 element int c-array to which the cell's connected nodal indices will be written
      */
      void cell(int32_t cell_id,int32_t cell_nodes[8]) override{
        for(int k=0;k<8;k++){cell_nodes[k]=cells.at(cell_id).idx[k];};
      };

      /*!
      * Query for the cells connected to the node by node index
      * \param node_id the node index being requested
      * \param node_cells the 8 element int c-array to which the nodes's connected cellular indices will be written
      */
      void node(int32_t node_id,int32_t node_cells[8]) override{
        for(int k=0;k<8;k++){node_cells[k]=nodes.at(node_id).idx[k];};
      };

      /*!
      * Query for the global nodal index by local nodal index
      * \param node_id the local node index being requested
      */
      int64_t globalNodeId(int32_t node_id) override{return nodeGlobalIds.at(node_id);};
      
      /*!
      * Query for the owning partition by local nodal index
      * \param node_id the local node index being requested
      */
      int32_t nodeHomePartition(int32_t node_id) override{return nodeOwner.at(node_id);};

      /*!
      * Query for the global cell index by local cell index
      * \param cell_id the local cell index being requested
      */
      int64_t globalCellId(int32_t cell_id) override{return cellGlobalIds.at(cell_id);};
      /*!
      * Query for the owning partition by local cell index
      * \param cell_id the local cell index being requested
      */
      int32_t cellHomePartition(int32_t cell_id) override{return cellOwner.at(cell_id);};

      /*!
      * Blank constructor
      */
      HexMesh(){};
      /*!
      * Constructor for purely local mesh.
      * \param meshnodes vector of ModelUT::MeshNodeWithId struct containing nodal definitions
      * \param meshhexes vector of ModelUT::MeshHexWithId struct containing cell definitions
      */
      HexMesh(std::vector<ModelUT::MeshNodeWithId> meshnodes,
              std::vector<ModelUT::MeshHexWithId> meshhexes){
        setupMesh(meshnodes,meshhexes);
      };
      /*!
      * Constructor for local partition of mesh.
      * \param meshnodes vector of ModelUT::MeshNodeWithId struct containing nodal definitions
      * \param meshhexes vector of ModelUT::MeshHexWithId struct containing cell definitions
      * \param partitionId partition index being constructed
      */
      HexMesh(std::vector<ModelUT::MeshNodeWithId> meshnodes,
              std::vector<ModelUT::MeshHexWithId> meshhexes,
              int32_t partitionId){
        mypartition=partitionId;
        setupMesh(meshnodes,meshhexes);
      };
      /*!
      * Constructor for purely local mesh.
      * \param nodePositions vector of node coordinates
      * \param hexes vector of hex definitions by 8 element c-arrays of nodal indices
      */
      HexMesh(std::vector<std::array<double,3>> nodePositions,
              std::vector<std::array<int32_t,8>> hexes){
        setupLocalMesh(nodePositions,hexes);
      };
      /*!
      * setup for local mesh.
      * \param meshnodes vector of ModelUT::MeshNodeWithId struct containing nodal definitions
      * \param meshhexes vector of ModelUT::MeshHexWithId struct containing cell definitions
      */
      void setupMesh(std::vector<ModelUT::MeshNodeWithId> meshnodes,
                     std::vector<ModelUT::MeshHexWithId> meshhexes){
        setupLocalMesh(configureLocalMeshNodes(meshnodes),
                       configureLocalMeshHexes(meshhexes));
        setupOwners(meshnodes,meshhexes);
      };

      /*!
      * setup for purely local mesh.
      * \param meshnodes vector of ModelUT::MeshNodeWithId struct containing nodal definitions
      * \param meshhexes vector of ModelUT::MeshHexWithId struct containing cell definitions
      */
      void setupLocalMesh(std::vector<std::array<double,3>> nodePositions,
                          std::vector<std::array<int32_t,8>> hexes){
        setupData(nodePositions.size(),hexes.size());
        buildNodeCoords(nodePositions);
        buildLinks(hexes);
      };
      /*!
      * assign cell and node owners
      * \param meshnodes vector of ModelUT::MeshNodeWithId struct containing nodal definitions
      * \param meshhexes vector of ModelUT::MeshHexWithId struct containing cell definitions
      */
      void setupOwners(std::vector<ModelUT::MeshNodeWithId> meshnodes,
                       std::vector<ModelUT::MeshHexWithId> meshhexes){
        nodeOwner.resize(meshnodes.size(),0);
        cellOwner.resize(meshhexes.size(),0);
        for(int kn=0;kn<meshnodes.size();kn++){
          nodeOwner.at(kn)=meshnodes.at(kn).owner;
        };
        for(int kc=0;kc<meshhexes.size();kc++){
          cellOwner.at(kc)=meshhexes.at(kc).owner;
        };
      };

      /*!
      * assign global ids.
      * \param nodeGlobalIds vector of global nodal indices
      * \param nodeGlobalIds vector of global cell indices
      */
      void setupGlobalIds(std::vector<int64_t> nodeGlobalIds,
                          std::vector<int64_t> cellGlobalIds){
        if(nodeGlobalIds.size()!=nodeCoords.size()){
          throw std::logic_error("incompatible node vector lengths");
        };
        if(cellGlobalIds.size()!=cellCoords.size()){
          throw std::logic_error("incompatible cell vector lengths");
        };
        for(int k=0;k<nodeGlobalIds.size();k++){
          (this->nodeGlobalIds).at(k)=nodeGlobalIds.at(k);
        };
        for(int k=0;k<cellGlobalIds.size();k++){
          (this->cellGlobalIds).at(k)=cellGlobalIds.at(k);
        };
      };

    private:
      struct Box{
        int32_t idx[8];
      };
      struct Point{
        double pt[3];
      };

      std::vector<std::array<double,3>> configureLocalMeshNodes(std::vector<ModelUT::MeshNodeWithId> meshnodes){
        std::vector<std::array<double,3>> nodePositions(meshnodes.size());
        nodeGlobalIds.resize(meshnodes.size());
        for(int k=0;k<meshnodes.size();k++){
          nodePositions.at(k)=meshnodes.at(k).point;
          nodeGlobalIds.at(k)=meshnodes.at(k).global_id;
        };
        return nodePositions;
      };
      std::vector<std::array<int32_t,8>> configureLocalMeshHexes(std::vector<ModelUT::MeshHexWithId> meshhexes){
        std::vector<std::array<int32_t,8>> hexes(meshhexes.size());
        cellGlobalIds.resize(meshhexes.size());
        for(int k=0;k<meshhexes.size();k++){
          hexes.at(k)=meshhexes.at(k).cell;
          cellGlobalIds.at(k)=meshhexes.at(k).global_id;
        };
        return hexes;
      };

      void setupData(int32_t Nnodes,int32_t Nhexes){
        setupNodes(Nnodes);
        setupCells(Nhexes);
      };
      void setupNodes(int32_t Nnodes){
        nodeCoords.resize(Nnodes);
        nodeGlobalIds.resize(Nnodes);
        nodeOwner.resize(Nnodes,0);
        Box init;for(int k=0;k<8;k++){init.idx[k]=-1;};
        nodes.resize(Nnodes,init);
      };
      void setupCells(int32_t Nhexes){
        cellCoords.resize(Nhexes);
        cells.resize(Nhexes);
        cellGlobalIds.resize(Nhexes);
        cellOwner.resize(Nhexes,0);
      };

      void buildNodeCoords(std::vector<std::array<double,3>> nodePositions){
        for(int k=0;k<nodeCoords.size();k++){
          nodeGlobalIds.at(k)=(int64_t)(k);
          for(int kp=0;kp<3;kp++){
            nodeCoords.at(k).pt[kp]=nodePositions.at(k).at(kp);
          };
        };
      };

      void buildLinks(std::vector<std::array<int32_t,8>>hexes){
        for(int32_t kh=0;kh<hexes.size();kh++){
          cellGlobalIds.at(kh)=(int64_t)(kh);
          for(int k=0;k<8;k++){
            cells.at(kh).idx[k]=hexes.at(kh).at(k);
          };
          nodes.at(hexes.at(kh).at(0)).idx[6]=kh;
          nodes.at(hexes.at(kh).at(1)).idx[7]=kh;
          nodes.at(hexes.at(kh).at(2)).idx[4]=kh;
          nodes.at(hexes.at(kh).at(3)).idx[5]=kh;
          nodes.at(hexes.at(kh).at(4)).idx[2]=kh;
          nodes.at(hexes.at(kh).at(5)).idx[3]=kh;
          nodes.at(hexes.at(kh).at(6)).idx[0]=kh;
          nodes.at(hexes.at(kh).at(7)).idx[1]=kh;

          for(int kr=0;kr<3;kr++){
            cellCoords.at(kh).pt[kr]=0.0;
            for(int kn=0;kn<8;kn++){
              cellCoords.at(kh).pt[kr]+=0.125*nodeCoords.at(cells.at(kh).idx[kn]).pt[kr];
            };
          };
        };
      };

      std::vector<Point> nodeCoords;
      std::vector<Point> cellCoords;
      std::vector<Box> cells;
      std::vector<Box> nodes;

      std::vector<int32_t> nodeOwner;
      std::vector<int32_t> cellOwner;

      std::vector<int64_t> nodeGlobalIds;
      std::vector<int64_t> cellGlobalIds;

      int mypartition=0;
  };
};

#endif /* HEXMESH_HPP */