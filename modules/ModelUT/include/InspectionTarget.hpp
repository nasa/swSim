/*! \headerfile InspectionTarget.hpp "modules/ModelUT/include/InspectionTarget.hpp"
* "InspectionTarget.hpp" defines a struct that bundles mesh and material data for UT simulations
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

#ifndef INSPECTIONTARGET_HPP
#define INSPECTIONTARGET_HPP

#include <memory>

#include "Material.hpp"
#include "Mesh.hpp"

namespace ModelUT{
  /*!
  * \struct InspectionTarget InspectionTarget.hpp "modules/ModelUT/include/InspectionTarget.hpp"
  * 
  * provides a mesh/material bundle for convenience of passing around this commonly paired data
  *
  */
  struct InspectionTarget{
    std::shared_ptr<ModelUT::Mesh> mesh;
    std::shared_ptr<ModelUT::Material> material;
  };
};

#endif /* INSPECTIONTARGET_HPP */