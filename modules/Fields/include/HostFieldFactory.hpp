/*! \headerfile HostFieldFactory.hpp "modules/Fields/include/HostFieldFactory.hpp"
* "HostFieldFactory.hpp" contains a concrete implementation of the field constructor class
* defined with the ModelUT::FieldFactory interface (modules/ModelUT/include/FieldFactory.hpp)
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

#ifndef HOSTFIELDFACTORY_HPP
#define HOSTFIELDFACTORY_HPP

#include <vector>
#include <memory>

#include "FieldFactory.hpp"
#include "Field.hpp"
#include "Mesh.hpp"

#include "ConstantField.hpp"
#include "DenseNodeField.hpp"
#include "SparseNodeField.hpp"
#include "DenseCellField.hpp"
#include "SparseCellField.hpp"

namespace Fields {
  /*! \class HostFieldFactory HostFieldFactory.hpp "imodules/Fields/include/HostFieldFactory.hpp"
  *
  * Is a concrete implementation of the field factory. Provides the relevant functionality 
  * for the code base to be able to construct fields.
  *
  */
  class HostFieldFactory : public ModelUT::FieldFactory {
    public:
      /*!
      * Construct a dense node field on the underlying mesh.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      */
      std::unique_ptr<ModelUT::Field> makeDenseNodeField(std::shared_ptr<ModelUT::Mesh> mesh) override {
        std::unique_ptr<DenseNodeField> field=std::unique_ptr<DenseNodeField>(new DenseNodeField);
        field->build(mesh);
        return std::move(field);
      };
      /*!
      * Construct a dense cell field on the underlying mesh.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      */
      std::unique_ptr<ModelUT::Field> makeDenseCellField(std::shared_ptr<ModelUT::Mesh> mesh) override {
        std::unique_ptr<DenseCellField> field=std::unique_ptr<DenseCellField>(new DenseCellField);
        field->build(mesh);
        return std::move(field);
      };
      /*!
      * Construct a sparse node field on the underlying mesh.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      */
      std::unique_ptr<ModelUT::Field> makeSparseNodeField(std::shared_ptr<ModelUT::Mesh> mesh,std::vector<int32_t>nzidx) override {
        std::unique_ptr<SparseNodeField> field=std::unique_ptr<SparseNodeField>(new SparseNodeField);
        field->build(mesh,nzidx);
        return std::move(field);
      };
      /*!
      * Construct a sparse cell field on the underlying mesh.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      */
      std::unique_ptr<ModelUT::Field> makeSparseCellField(std::shared_ptr<ModelUT::Mesh> mesh,std::vector<int32_t>nzidx) override {
        std::unique_ptr<SparseCellField> field=std::unique_ptr<SparseCellField>(new SparseCellField);
        field->build(mesh,nzidx);
        return std::move(field);
      };
      /*!
      * Construct a constant field on the underlying mesh.
      * \param theMesh is the underlying ModelUT mesh on which the field is built.
      */
      std::unique_ptr<ModelUT::Field> makeConstantField(std::shared_ptr<ModelUT::Mesh> mesh,double value=0.0) override {
        std::unique_ptr<ConstantField> field=std::unique_ptr<ConstantField>(new ConstantField);
        field->build(mesh,value);
        return std::move(field);
      };
  };
};

#endif /* HOSTFIELDFACTORY_HPP */