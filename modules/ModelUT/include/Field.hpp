/*! \headerfile Field.hpp "modules/ModelUT/include/Field.hpp"
* "Field.hpp" defines a general field interface for accessing data on mesh
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

#ifndef FIELD_HPP
#define FIELD_HPP

#include <cstdint>
#include <memory>

#include "Mesh.hpp"

namespace ModelUT{
  /*! \class Field Field.hpp "modules/ModelUT/include/Field.hpp"
  *
  * provides a general field interface for accessing data on mesh
  *
  */
  class Field{
    public:
      /*!
      * FieldType provides an indicator of where the data exists on the mesh
      */
      enum FieldType{
        NODE=0,
        CELL=1,
        CONSTANT=2
      };
      /*!
      * Query the field for its type. 
      */
      virtual FieldType type() =0;
      /*!
      * Query the field for its mesh.
      */
      virtual std::shared_ptr<Mesh> mesh() =0;
      /*!
      * Get the pointer to the raw data. Not Recommended for "usual" practice
      */
      virtual double* data() =0;
      /*!
      * Query the data by reference at a given mesh index. 
      */
      virtual double& at(int index) =0;

      /*!
      * Query the sparsity of the data. Definitionally dense, thus returns false.
      */
      virtual bool isSparse() =0;
      
      /*!
      * Query the size of the underlying array. 
      */
      virtual int32_t size() =0;
      /*!
      * Query the populated indices of sparse data. If the field is dense, it returns a nullptr
      */
      virtual int32_t* spIndices() =0;

      virtual ~Field(){};
  };
};

#endif /* FIELD_HPP */
