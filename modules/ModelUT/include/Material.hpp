/*! \headerfile Material.hpp "modules/ModelUT/include/Material.hpp"
* "Material.hpp" defines a general material interface for accessing material property fields
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

#ifndef MODELUT_MATERIAL_HPP
#define MODELUT_MATERIAL_HPP

#include <memory>
#include <cstdint>

#include "Field.hpp"

namespace ModelUT{
  /*! \class Material Material.hpp "modules/ModelUT/include/Material.hpp"
  *
  * provides a general material interface for accessing material property fields
  *
  */
  class Material{
    public:
      /*!
      * Query the material for stiffness coefficient from the 4th order stiffness tensor. 
      */
      virtual std::shared_ptr<Field> stiffness(int32_t i,int32_t j,int32_t m,int32_t n) =0;
      /*!
      * Query the material for its density. 
      */
      virtual std::shared_ptr<Field> density() =0;

      virtual ~Material(){};
  };
};

#endif /* MODELUT_MATERIAL_HPP */
