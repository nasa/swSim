/*! \headerfile FieldFactory.hpp "modules/ModelUT/include/FieldFactory.hpp"
* "FieldFactory.hpp" contains an interface definition of the field constructor class
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

#ifndef FIELDFACTORY_HPP
#define FIELDFACTORY_HPP

#include <vector>
#include <memory>
#include <cstdint>

#include "Field.hpp"
#include "Mesh.hpp"

namespace ModelUT {
  /*! \class FieldFactory FieldFactory.hpp "imodules/ModelUT/include/FieldFactory.hpp"
  *
  * Is an interface definition of the field factory. Describes the relevant functionality 
  * for the code base to be able to construct fields.
  *
  */
  class FieldFactory {
    public:
      /*!
      * Construct a dense node field on the underlying mesh.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      */
      virtual std::unique_ptr<Field> makeDenseNodeField(std::shared_ptr<Mesh> mesh) =0;
      /*!
      * Construct a dense cell field on the underlying mesh.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      */
      virtual std::unique_ptr<Field> makeDenseCellField(std::shared_ptr<Mesh> mesh) =0;
      /*!
      * Construct a sparse node field on the underlying mesh.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      */
      virtual std::unique_ptr<Field> makeSparseNodeField(std::shared_ptr<Mesh> mesh,std::vector<int32_t>nzidx) =0;
      /*!
      * Construct a sparse cell field on the underlying mesh.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      */
      virtual std::unique_ptr<Field> makeSparseCellField(std::shared_ptr<Mesh> mesh,std::vector<int32_t>nzidx) =0;
      /*!
      * Construct a constant field on the underlying mesh.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      */
      virtual std::unique_ptr<Field> makeConstantField(std::shared_ptr<Mesh> mesh,double value=0.0) =0;

      virtual ~FieldFactory(){};
  };
};

#endif /* FIELDFACTORY_HPP */