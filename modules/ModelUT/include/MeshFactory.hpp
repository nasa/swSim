/*! \headerfile MeshFactory.hpp "modules/ModelUT/include/MeshFactory.hpp"
* "MeshFactory.hpp" contains an interface definition for a mesh factory
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

#ifndef MESHFACTORY_HPP
#define MESHFACTORY_HPP

#include <vector>
#include <array>
#include <cstdint>
#include <memory>

#include "Mesh.hpp"
#include "MeshElementsWithId.hpp"

namespace ModelUT {
/*! \class MeshFactory MeshFactory.hpp "modules/ModelUT/include/MeshFactory.hpp"
  *
  * Is an interface definition of a mesh factory. Defines the relevant functionality 
  * for the code base to be able to construct hex meshes.
  *
  */
  class MeshFactory {
    public:
      /*!
      * Construct a hex mesh on the basic node and cell data
      * \param nodePositions vector of node coordinates
      * \param hexes vector of cell definitions by cgns windings
      */
      virtual std::unique_ptr<Mesh> makeHexMesh(std::vector<std::array<double,3>> nodePositions,
                                         std::vector<std::array<int32_t,8>>hexes) =0;
      /*!
      * Construct a hex mesh on the node/hex structs
      * \param nodes vector of node structs containing ids and coordinates
      * \param hexes vector of cell structs containing ids and hex definitions
      */
      virtual std::unique_ptr<Mesh> makeHexMeshPartition(std::vector<MeshNodeWithId> nodes,
                                         std::vector<MeshHexWithId> hexes) =0;
      /*!
      * Construct a hex mesh partition on the node/hex structs
      * \param nodes vector of node structs containing ids and coordinates
      * \param hexes vector of cell structs containing ids and hex definitions
      * \param parititionId partition identifier
      */
      virtual std::unique_ptr<Mesh> makeHexMeshPartition(std::vector<MeshNodeWithId> nodes,
                                         std::vector<MeshHexWithId> hexes,
                                         int32_t partitionId) =0;
      //virtual void unmakeMesh(Mesh* mesh) =0;
  };
};

#endif /* MESHFACTORY_HPP */