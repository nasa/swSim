/*! \headerfile Stencil.hpp "include/Stencil.hpp"
* "Stencil.hpp" contains the class definition encapsulating the 
* data structure interface for a Stencil in swSim.
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

#ifndef STENCIL2_HPP
#define STENCIL2_HPP

#include <mpi.h>
#include <math.h>
#include "Model.hpp"
#include "Segmentation.hpp"
#include "SparseMatrix.hpp"
#include "DenseVector.hpp"
#include "DriveVector.hpp"
#include "Material.hpp"
#include "MPIFileReader.hpp"
#include "SWMesh.hpp"

namespace swSim{
    /*! \class Stencil Stencil.hpp "include/Stencil.hpp"
    *
    * Defines data structures for Stencil in swSim
    */
    class Stencil
    {
    public:
        std::vector<swSim::DenseVector> V; /*!< Velocity State vector */
        std::vector<swSim::DenseVector> S; /*!< Stress State vector */
        std::vector<swSim::SparseMatrix> AStoV; /*!< Transition matrix from Stress to Velocity */
        std::vector<swSim::SparseMatrix> AVtoS; /*!< Transition matrix from Velocity to Stress */
        swSim::SparseMatrix AinvRhoBar; /*!< Inverse of Average density for each node  */
        swSim::SparseMatrix AForceDispersion; /*!< 1/8 connectivity matrix for each cell to connected nodes  */
        std::vector<swSim::DriveVector> U; /*!< Drive Vector for the model  */

        /*!
        * Create the Stencil
        */
        Stencil(swSim::Model &model, swSim::SWMesh &localMesh, MPI_Comm l_mpi_comm, swSim::Segmentation &lSeg ); 
        /*!
        * Destroy the Stencil
        */
        ~Stencil();
        /*!
        * Print info about the Stencil
        */
        void Print(int rank); 

     private:

        std::vector<std::vector<int>> neighbors;  /*!< Order of the neighbors in the stencil  */
        std::vector<int> cOrder;  /*!< Order of the stiffness matrix coefficients for the stencil */

        /*!
        * Populates one row of the Transition matrix
        */
        void Delta(std::vector<int32_t> &stencil, uint64_t indx, swSim::SparseMatrix &A,  double val, int deltaAxis);
        /*!
        * Populates the Velocity to Stress Transition matrix
        */
        void PopulateVelocityToStressMatrices(swSim::Model &model, std::vector<swSim::SparseMatrix> &A, swSim::SWMesh &localMesh);
        /*!
        * Populates the Stress to Velocity Transition matrix
        */
        void PopulateStressToVelocityMatrices(swSim::Model &model, std::vector<swSim::SparseMatrix> &A, swSim::SWMesh &localMesh);
        /*!
        * Populates the Force Dispersion matrix
        */
        void PopulateForceDispersionMatrix(swSim::SparseMatrix &A, swSim::SWMesh &localMesh);
        /*!
        * Populates the inverse average density matrix
        */
        void PopulateRhoInvMatrix(swSim::Model &model, swSim::SparseMatrix &A, swSim::SWMesh &localMesh);
        /*!
        * Places the Excitations in the drive vector
        */
        void PlaceTransducers(swSim::Model &model, std::vector<swSim::DriveVector> &u, swSim::SWMesh &localMesh);
        /*!
        * Determines if a specific element is affected by an excitation
        */
        bool LocationCheck(std::vector<double> &pCoords, std::vector<double> center, double radius, double zdist); 

    };
};
#endif