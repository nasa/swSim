/*! \headerfile SparseNodeField.hpp "modules/Fields/include/SparseNodeField.hpp"
* "SparseNodeField.hpp" contains a concrete implementation of a sparse node field using the
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

#ifndef SPARSENODEFIELD_HPP
#define SPARSENODEFIELD_HPP

#include <vector>
#include <algorithm>
#include <cstdint>
#include <memory>

#include "Field.hpp"
#include "Mesh.hpp"

namespace Fields {
  /*! \class SparseNodeField SparseNodeField.hpp "imodules/Fields/include/SparseNodeField.hpp"
  *
  * Is a concrete implementation of a sparse node field. Provides the relevant functionality 
  * to appear as a general field as used in the rest of the code-base.
  *
  */
  class SparseNodeField : public ModelUT::Field {
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
      * Query the sparsity of the data. Definitionally sparse, thus returns true.
      */
      bool isSparse() override {
        return true;
      };
      /*!
      * Query the size of the underlying array. 
      */
      int32_t size() override {
        return field_data.size();
      };
      /*!
      * Query the populated indices of sparse data. 
      */
      int32_t* spIndices() override {
        return indices.data();
      };
      /*!
      * Query the data by reference at a given mesh index. 
      */
      double& at(int32_t index) override {
        //std::vector will throw std::out_of_range if index is improperly provided
        auto p=std::find(indices.begin(),indices.end(),index);
        int32_t idx=std::distance(indices.begin(),p);
        return field_data.at(idx);
      };

      /*!
      * Utility to construct this specific implementation from the underlying mesh and the 
      * list of nonzero indices
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      * \param nonzeroIndices is the list of populated node indices
      */
      void build(std::shared_ptr<ModelUT::Mesh> theMesh,std::vector<int32_t> nonzeroIndices){
        mymesh=theMesh;
        vectorResize<double>(field_data,nonzeroIndices.size());
        vectorCopy<int32_t>(indices,nonzeroIndices);
      };

    private:
      template<typename T>
      void vectorResize(std::vector<T>& target,int newsize){
        target.clear();target.resize(newsize);
      };
      template<typename T>
      void vectorCopy(std::vector<T>& target,std::vector<T>& source){
        vectorResize<T>(target,source.size());
        for(int k=0;k<source.size();k++){
          target.at(k)=source.at(k);
        };
      };

      std::shared_ptr<ModelUT::Mesh> mymesh;
      std::vector<double> field_data;
      std::vector<int32_t> indices;
  };
};

#endif /* SPARSENODEFIELD_HPP */