/*! \headerfile MPIData.hpp "include/MPIData.hpp"
* "MPIData.hpp" contains the class definition encapsulating the 
* data structure interface for providing MPIData object for to swSim
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

#ifndef MPIDATA_HPP
#define MPIDATA_HPP

#include <mpi.h>


namespace swSim{
    /*! \class MPIData MPIData.hpp "include/MPIData.hpp"
    *
    * Defines data structures for MPIData in swSim
    */
    class MPIData {
    public:
        MPI_Comm mComm; /*!< An MPI Communicator */
        int rank; /*!< This processes rank int he communicator */
        int size; /*!< The total number of  processes in the communicator*/
         /*!
        * Create the MPIData object with mpi_comm_world
        */
        MPIData();
        /*!
        * Create the MPIData object with a specific communicator
        */
        MPIData(MPI_Comm l_mpi_comm);
        /*!
        * Destroy the MPIData object
        */
        ~MPIData(); 

    private:
    /*!
    * Dubplicates the MPI World Communicator
    */
    MPI_Comm Dup_Comm_World(int *mRank, int *mSize);
    }; 
}; 

#endif
