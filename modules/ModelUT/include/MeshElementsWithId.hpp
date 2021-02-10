/*! \headerfile MeshElementsWithId.hpp "modules/ModelUT/include/MeshElementsWithId.hpp"
* "MeshElementsWithId.hpp" defines two structs that bundle mesh node and mesh cell data with 
* necessary information for decomposition into sub-meshes for parallelization
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

#ifndef MESHELEMENTSWITHID_HPP
#define MESHELEMENTSWITHID_HPP

#include <array>
#include <cstdint>

namespace ModelUT {
  /*!
  * \struct MeshNodeWithId MeshElementsWithId.hpp "modules/ModelUT/include/MeshElementsWithId.hpp"
  * 
  * provides a mesh node bundle with the nodal coordinate, the owning mesh id, and the node global ID
  *
  */
  struct MeshNodeWithId {
    int64_t global_id;
    int32_t owner;
    std::array<double,3> point;
  };
  /*!
  * \struct MeshHexWithId MeshElementsWithId.hpp "modules/ModelUT/include/MeshElementsWithId.hpp"
  * 
  * provides a mesh hex bundle with the hex cgns winding definition, the owning mesh id, and the hex global ID
  *
  */
  struct MeshHexWithId {
    int64_t global_id;
    int32_t owner;
    std::array<int32_t,8> cell;
  };
};

#endif /* MESHELEMENTSWITHID_HPP */
