/*! \headerfile DataPresenter.hpp "modules/Controller/include/DataPresenter.hpp"
* "DataPresenter.hpp" contains the presenter interface for writing a mesh and it's 
* mesh-assigned data to file.
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

#ifndef DATAPRESENTER_HPP
#define DATAPRESENTER_HPP

#include <cstdint>
#include <string>
#include <memory>

#include "InspectionTarget.hpp"
#include "Field.hpp"

namespace Controller {
  /*! \class DataPresenter DataPresenter.hpp "imodules/Controller/include/DataPresenter.hpp"
  *
  * Defines the interactions through which the main software can write mesh and mesh assigned
  * data to file. This interfaces provides for setting the mesh, the data on mesh, and committing 
  * that mesh and data to file.
  *
  */
  class DataPresenter {
    public:
      /*!
      * register a mesh with the Presenter for eventual file write.
      * \param target is a ModelUT structure containing the mesh and the material. The material 
      * shared pointer can be a nullptr, as it is unused. 
      */
      virtual void setInspectedTarget(std::shared_ptr<ModelUT::InspectionTarget>& target) =0;
      /*!
      * register a data field with the Presenter for eventual file write.
      * \param field is a ModelUT structure containing the data-on-mesh to be written.
      * \param name is the std::string name of the field by which it will be called in the 
      * eventually written file.
      */
      virtual void addOverlayedDataFieldToTarget(std::shared_ptr<ModelUT::Field> field,std::string name) =0;
      /*!
      * Instruct the presenter to invoke the file writer.
      * \param filename is the name of the file to be written.
      */
      virtual void commitToFile(std::string filename) =0;
  };
};

#endif /* DATAPRESENTER_HPP*/