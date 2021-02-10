/*! \headerfile HostMeshFactory.hpp "modules/HostMesh/include/HostMeshFactory.hpp"
* "HostMeshFactory.hpp" contains a concrete implementation of the mesh constructor class
* defined with the ModelUT::MeshFactory interface (modules/ModelUT/include/MeshFactory.hpp)
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

#ifndef HOSTMESHFACTORY_HPP
#define HOSTMESHFACTORY_HPP

#include <vector>
#include <array>
#include <cstdint>
#include <memory>

#include "MeshFactory.hpp"
#include "Mesh.hpp"
#include "MeshElementsWithId.hpp"

#include "HexMesh.hpp"

namespace HostMesh {
  /*! \class HostMeshFactory HostMeshFactory.hpp "imodules/HostMesh/include/HostMeshFactory.hpp"
  *
  * Is a concrete implementation of the mesh factory. Provides the relevant functionality 
  * for the code base to be able to construct hex meshes.
  *
  */
  class HostMeshFactory : public ModelUT::MeshFactory {
    public:
      /*!
      * Construct a hex mesh on the basic node and cell data
      * \param nodePositions vector of node coordinates
      * \param hexes vector of cell definitions by cgns windings
      */
      std::unique_ptr<ModelUT::Mesh> makeHexMesh(std::vector<std::array<double,3>> nodePositions,
              std::vector<std::array<int32_t,8>>hexes) override {
        return std::unique_ptr<HexMesh> (new HexMesh(nodePositions,hexes));
      };
      /*!
      * Construct a hex mesh on the node/hex structs
      * \param nodes vector of node structs containing ids and coordinates
      * \param hexes vector of cell structs containing ids and hex definitions
      */
      std::unique_ptr<ModelUT::Mesh> makeHexMeshPartition(std::vector<ModelUT::MeshNodeWithId> nodes,
                                 std::vector<ModelUT::MeshHexWithId> hexes) override {
        return std::unique_ptr<HexMesh> (new HexMesh(nodes,hexes));

      };
      /*!
      * Construct a hex mesh partition on the node/hex structs
      * \param nodes vector of node structs containing ids and coordinates
      * \param hexes vector of cell structs containing ids and hex definitions
      * \param parititionId partition identifier
      */
      std::unique_ptr<ModelUT::Mesh> makeHexMeshPartition(std::vector<ModelUT::MeshNodeWithId> nodes,
                                 std::vector<ModelUT::MeshHexWithId> hexes,
                                 int32_t partitionId) override {
        return std::unique_ptr<HexMesh> (new HexMesh(nodes,hexes,partitionId));

      };
  };
};

#endif /* HOSTMESHFACTORY_HPP */