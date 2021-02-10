/*! \headerfile Material.hpp "include/Material.hpp"
* "Material.hpp" contains the class definition encapsulating the 
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

#ifndef MATERIAL_HPP
#define MATERIAL_HPP


#include <mpi.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <stddef.h>
#include <math.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <Eigen/Dense>

#include "ParsableObject.hpp"

typedef Eigen::Matrix<float, 6, 6> Matrix6f;
namespace swSim{
    /*! \class Material Material.hpp "include/Material.hpp"
    *
    * Defines data structures for Material in swSim
    */
    class Material: public ParsableObject {   
    public:
        struct material_s {
            uint32_t materialID; /*!< Unique identification number for this material */
            float density; /*!< Material density in kg/cubic meter */
            float stiffnessValues[21]; /*!< 21 Unique stiffness values (in Pascals) in the 6x6, symmetric stiffness matrix */

        };
        material_s matS; /*!< Material struct */
        MPI_Datatype mpi_material; /*!< MPI datatype for communication */
        /*!
        * Create the Material
        */
        Material();
        /*!
        * Destroy the Material
        */
        ~Material();
        /*!
        * Initilize data values
        */
        void Init(uint32_t id, float rho, float *c);
        /*!
        * Copy values of one Material to another.
        */
        void Copy(Material *other);
        /*!
        * Create the MPI datatype for the Material.
        */
        MPI_Datatype MPICreate();
        /*!
        * Get a local stiffness Matrix for that voxel base on the material stiffness matrix and a rotation vector.
        */
        void RotateStiffnessMatrix(float *rotVec, float *C);
        /*!
        * Provide parsing instructions for reading inputs associated with the Material
        */
        void ParseSwitch(xmlDocPtr doc, xmlNodePtr cur, int caseNumber) override;
        /*!
        * Convert the array of stiffness Values to an Eigen Matrix
        */
        Matrix6f StiffnessValuesArrayToMatrix(); 
     private:
        /*!
        * Get a 3D Rotation matrix based on a 3 element rotation vector.
        */
        Eigen::Matrix3f get3DRotation(float *rotVec);
        /*!
        * Get a Rotation Matrix from a rotation abotu the Z axis
        */
        Eigen::Matrix3f getRotationAboutZ(float angle);
        /*!
        * Get a Rotation Matrix from a rotation abotu the Y axis
        */
        Eigen::Matrix3f getRotationAboutY(float angle);
        /*!
        * Get a Rotation Matrix from a rotation abotu the X axis
        */
        Eigen::Matrix3f getRotationAboutX(float angle);
        /*!
        * Get a 3D Rotation Transformation Matrix
        */
        Matrix6f get3DTransformationMatrix(Eigen::Matrix3f R); 
        /*!
        * Convert MAtrix Values back to an array
        */ 
        void MatrixToStiffnessValuesArray(float *valueArray, Matrix6f C);
    };
};



#endif