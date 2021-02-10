/*! \headerfile DenseNodeField.hpp "modules/Fields/include/DenseNodeField.hpp"
* "DenseNodeField.hpp" contains a concrete implementation of a dense cell field using the
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

#ifndef DENSENODEFIELD_HPP
#define DENSENODEFIELD_HPP

#include <vector>
#include <cstdint>
#include <memory>

#include "Field.hpp"
#include "Mesh.hpp"

namespace Fields {
  /*! \class DenseNodeField DenseNodeField.hpp "imodules/Fields/include/DenseNodeField.hpp"
  *
  * Is a concrete implementation of a dense cell field. Provides the relevant functionality 
  * to appear as a general field as used in the rest of the code-base.
  *
  */
  class DenseNodeField : public ModelUT::Field {
    public:
      /*!
      * Query the field for its type. Returns ModelUT::Field::NODE
      */
      ModelUT::Field::FieldType type() override {
        return ModelUT::Field::NODE;
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
        return field_data.data();
      };
      /*!
      * Query the sparsity of the data. Definitionally dense, thus returns false.
      */
      bool isSparse() override {
        return false;
      };
      /*!
      * Query the size of the underlying array. 
      */
      int32_t size() override {
        return field_data.size();
      };
      /*!
      * Query the populated indices of sparse data. Since the field is definitionally dense, 
      * it returns a nullptr
      */
      int32_t* spIndices() override {
        return nullptr;
      };
      /*!
      * Query the data by reference at a given mesh index. 
      */
      double& at(int32_t index) override {
        return field_data.at(index);
      };

      /*!
      * Utility to construct this specific implementation from the underlying mesh.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      */
      void build(std::shared_ptr<ModelUT::Mesh> theMesh){
        field_data.clear();
        mymesh=theMesh;
        field_data.resize(mymesh->nodeCount(),0.0);
      };

    private:
      std::shared_ptr<ModelUT::Mesh> mymesh;
      std::vector<double> field_data;
  };
};

#endif /* DENSENODEFIELD_HPP */