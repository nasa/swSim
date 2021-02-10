/*! \headerfile DriveVector.hpp "include/DriveVector.hpp"
* "DriveVector.hpp" contains the class definition encapsulating the 
* data structure interface for a DriveVector in a swSim model.
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

#ifndef DRIVEVECTOR_HPP
#define DRIVEVECTOR_HPP

#include "SparseVector.hpp"
#include "SparseMatrix.hpp"
#include <vector>


namespace swSim{
    /*! \class DriveVector DriveVector.hpp "include/DriveVector.hpp"
    *
    * Defines data structures for DriveVector in swSim
    */
    class DriveVector
    {
    public:
        std::vector<swSim::SparseVector> uCoeffs; /*!< Vector of sparse vectors that define the input coefficients for each each cell */

         /*!
        * Create the DriveVector
        */
        DriveVector(int sz);
        /*!
        * Destroy the DriveVector
        */ 
        ~DriveVector();
        /*!
        * Creates the coefficients for a pair of sparse matrices to be multipled by, A1*A2*Drivevector[i] 
        */
        bool CommitCoefficents(swSim::SparseMatrix *matrix_1, swSim::SparseMatrix *matrix_2);


    private:
        bool committed = false; /*!< Used to keep track of if the coefficients have been committed already. */

    };
};

#endif