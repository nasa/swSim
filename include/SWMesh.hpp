/*! \headerfile SWMesh.hpp "include/SWMesh.hpp"
* "SWMesh.hpp" contains the class definition encapsulating the 
* data structure interface for a SWMesh in swSim.
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

#ifndef SWMESH_HPP
#define SWMESH_HPP


#include <stddef.h>
#include <stdexcept>
#include <string>
#include <cstring>
#include <cstdio>
#include <map>
#include <set>
#include <utility>  
#include <vector>
#include <array>
#include <cstdint>

#include "Model.hpp"
#include "MPIFileReader.hpp"
#include "Segmentation.hpp"

#include "InspectionTarget.hpp"
#include "Field.hpp"
#include "Mesh.hpp"
#include "MeshFactory.hpp"

#include "HostMeshFactory.hpp"
#include "HostFieldFactory.hpp"

namespace swSim{
    /*! \class SWMesh SWMesh.hpp "include/SWMesh.hpp"
    *
    * Defines data structures for SWMesh in swSim
    */
    class SWMesh
    {    
    public:
        std::shared_ptr<ModelUT::InspectionTarget> target=std::make_shared<ModelUT::InspectionTarget>(ModelUT::InspectionTarget());
        std::shared_ptr<ModelUT::Field> materialID;
        std::vector<std::shared_ptr<ModelUT::Field>> rotations;

        /*!
        * Create the SWMesh
        */
        SWMesh(swSim::Model &lMod, swSim::Segmentation &seg, MPI_Comm l_mpi_comm);
        /*!
        * Destroy the SWMesh
        */
        ~SWMesh();
   void ReturnCommunicationNodes(swSim::Segmentation &seg, std::vector<int32_t> &lrNodes, std::vector<int32_t> &lsNodes, std::vector<int32_t> &rsNodes, std::vector<int32_t> &rrNodes);
    private:
        /*!
        * Makes the bare mesh
        */
        void MakeMesh(swSim::Model &lMod, swSim::Segmentation &seg, MPI_Comm l_mpi_comm, std::vector<uint8_t> &matlId);
        /*!
        * Makes the material ID field
        */
        void MakeMaterialIDField(std::vector<uint8_t> &matlId);
        /*!
        * Makes the rotation vectorfield
        */
        void MakeRotationsField(swSim::Model &lMod, swSim::Segmentation &seg, MPI_Comm l_mpi_comm);
        /*!
        * defines the commucation DataTypes
        */
        void DefineCommunicationDatatypes(swSim::Segmentation &seg);




    };
};
#endif