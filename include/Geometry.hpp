/*! \headerfile Geometry.hpp "include/Geometry.hpp"
* "Geometry.hpp" contains the class definition encapsulating the 
* data structure interface for a Geemetry in a swSim Model.
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

#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <mpi.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include <stddef.h>

#include "ParsableObject.hpp"

namespace swSim{
    /*! \class Geometry Geometry.hpp "include/Geometry.hpp"
    *
    * Defines data structures for Geometry in swSim
    */
    class Geometry: public ParsableObject {   
    public:
        struct geometry_s {
            uint64_t dimensionsGlobalVoxel[3]; /*!< Bounding box size in x, y, z, in number of voxels of the gemetry */
            double voxelSize[3]; /*!< Dimensions in x, y, z (in meters) of the voxels */
            char geometryFile[50]; /*!< Geometry File name */
            char rotationsFile[50]; /*!< Rotations File name */
        };

        geometry_s geomS; /*!< Geometry struct */
        MPI_Datatype mpi_geometry; /*!< MPI datatype for communication */

        /*!
        * Create the Geometry
        */
        Geometry();
        /*!
        * Destroy the Geometry
        */
        ~Geometry();
        /*!
        * Initilize data values  with a 3 values for voxel size. 
        */
        void Init(uint64_t *nVox, double *ds, const char *gFname, const char *rFname);
        /*!
        * Initilize data values  with a 1 value for voxel size, so that voxels are cubes. 
        */
        void Init(uint64_t *nVox, double ds, const char *gFname, const char *rFname);
         /*!
        * Copy values of one Geometry to another.
        */
        void Copy(Geometry *other);
        /*!
        * Create the MPI datatype for the Geometry.
        */
        MPI_Datatype MPICreate();
        /*!
        * Provide parsing instructions for reading inputs associated with the Geometry
        */
        void ParseSwitch(xmlDocPtr doc, xmlNodePtr cur, int caseNumber) override;
        /*!
        * Get Geometry file and path as one string
        */
        std::string GetGeometryFile(std::string path);
        /*!
        * Get Plane Count file and path as one string
        */
        std::string GetPlaneCountFile(std::string path);
        /*!
        * Get Rotations file and path as one string
        */
        std::string GetRotationsFile(std::string path);


    private:
        
    };
};
#endif