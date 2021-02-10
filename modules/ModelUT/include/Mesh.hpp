/*! \headerfile Mesh.hpp "modules/ModelUT/include/Mesh.hpp"
* "Mesh.hpp" contains an interface for a general mesh definition 
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

#ifndef MESH_HPP
#define MESH_HPP

#include <cstdint>

namespace ModelUT{
  /*! \class Mesh Mesh.hpp "modules/ModelUT/include/Mesh.hpp"
  *
  * Is an interface definition of a general mesh.
  *
  */
  class Mesh{
    public:
      /*!
      * Query the number of nodes in the mesh
      */
      virtual int32_t nodeCount() =0;
      /*!
      * Query the number of cells in the mesh
      */
      virtual int32_t cellCount() =0;
      /*!
      * Query the partition Id in the mesh
      */
      virtual int32_t partitionId() =0;
      /*!
      * Query for the node coordinate by node index
      * \param node_id the node index being requested
      * \param coord the 3 element double c-array to which the point coordinates will be written
      */
      virtual void nodeCoordinate(int32_t node_id,double* coord) =0;
      /*!
      * Query for the cell coordinate by cell index
      * \param cell_id the cell index being requested
      * \param coord the 3 element double c-array to which the cell coordinates will be written
      */
      virtual void cellCoordinate(int32_t cell_id,double* coord) =0;

      /*!
      * Query for the cell sizes by cell index
      * \param cell_id the cell index being requested
      * \param coord the 3 element double c-array to which the cell dimensions will be written
      */
      virtual void cellExtents(int32_t cell_id,double ds[3]) =0;

      /*!
      * Query for the nodes connected to the cell by cell index
      * \param cell_id the cell index being requested
      * \param cell_nodes the 8 element int c-array to which the cell's connected nodal indices will be written
      */
      virtual void cell(int32_t cell_id,int32_t* cell_nodes) =0;

      /*!
      * Query for the cells connected to the node by node index
      * \param node_id the node index being requested
      * \param node_cells the 8 element int c-array to which the nodes's connected cellular indices will be written
      */
      virtual void node(int32_t node_id,int32_t* node_cells) =0;

      /*!
      * Query for the global nodal index by local nodal index
      * \param node_id the local node index being requested
      */
      virtual int64_t globalNodeId(int32_t node_id) =0;
      /*!
      * Query for the owning partition by local nodal index
      * \param node_id the local node index being requested
      */
      virtual int32_t nodeHomePartition(int32_t node_id) =0;

      /*!
      * Query for the global cell index by local cell index
      * \param cell_id the local cell index being requested
      */
      virtual int64_t globalCellId(int32_t cell_id) =0;
      /*!
      * Query for the owning partition by local cell index
      * \param cell_id the local cell index being requested
      */
      virtual int32_t cellHomePartition(int32_t cell_id) =0;
  };
};

#endif /* MESH_HPP */