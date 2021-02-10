/*! \headerfile ConstantField.hpp "modules/Fields/include/ConstantField.hpp"
* "ConstantField.hpp" contains a concrete implementation of a constant field using the
* ModelUT::Field interface (modules/ModelUT/include/Field.hpp)
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

#ifndef CONSTANTFIELD_HPP
#define CONSTANTFIELD_HPP

#include <cstdint>
#include <memory>

#include "Field.hpp"
#include "Mesh.hpp"

namespace Fields {
  /*! \class ConstantField ConstantField.hpp "imodules/Fields/include/ConstantField.hpp"
  *
  * Is a concrete implementation of a simple constant field. Provides the relevant functionality 
  * to appear as a general field as used in the rest of the code-base.
  *
  */
  class ConstantField : public ModelUT::Field {
    public:
      /*!
      * Query the field for its type. Returns ModelUT::Field::CONSTANT
      */
      ModelUT::Field::FieldType type() override {
        return ModelUT::Field::CONSTANT;
      };
      /*!
      * Query the field for its mesh.
      */
      std::shared_ptr<ModelUT::Mesh> mesh() override {
        return mymesh;
      };
      /*!
      * Get the pointer to the raw data.
      */
      double* data() override {
        return &field_data;
      };
      /*!
      * Query the sparsity of the data. Definitionally dense, thus returns false.
      */
      bool isSparse() override {
        return false;
      };
      /*!
      * Query the size of the underlying array. Field is constant, therefore returns unity
      */
      int32_t size() override {
        return 1;
      };
      /*!
      * Query the populated indices of sparse data. Since the field is definitionally dense, 
      * it returns a nullptr
      */
      int32_t* spIndices() override {
        return nullptr;
      };
      /*!
      * Query the data by reference at a given mesh index. Since the field is constant, it returns
      * a reference to the scalar value.
      */
      double& at(int32_t index) override {
        return field_data;
      };
      /*!
      * Utility to construct this specific implementation from the underlying mesh and a value.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      * \param value is the initial value of the field on the mesh.
      */
      void build(std::shared_ptr<ModelUT::Mesh> theMesh,double value=0.0){
        mymesh=theMesh;
        field_data=value;
      };

    private:
      std::shared_ptr<ModelUT::Mesh> mymesh;
      double field_data=0.0;
  };
};

#endif /* CONSTANTFIELD_HPP */