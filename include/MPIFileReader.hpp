/*! \headerfile MPIFileReader.hpp "include/MPIFileReader.hpp"
* "MPIFileReader.hpp" contains the class definition encapsulating the 
* data structure interface for a MPIFileReader in swSim.
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

#ifndef MPIFileReader_HPP
#define MPIFileReader_HPP


#include <mpi.h>
#include <stdint.h>
#include <string>
#include <stddef.h>

namespace swSim{
    /*! \class MPIFileReader MPIFileReader.hpp "include/MPIFileReader.hpp"
    *
    * Defines data structures for MPIFileReader in swSim
    */
    class MPIFileReader
    {
    public:
        std::string fileName; /*!< File to read From */
        int openError = 0; /*!< error flag from openning */
        int offset; /*!< Offset from where to begin reading */
        int length; /*!< Length from where to begin reading */
        MPI_Datatype thisType; /*!< Datatype to read */
        MPI_File fh; /*!< File handle */
        MPI_Comm comm; /*!< MPI Communicator for reading */

        /*!
        * Create the FileReader
        */
        MPIFileReader(MPI_Comm aComm, std::string aFname);
        /*!
        * Destroy the File Reader
        */
        ~MPIFileReader();

     private:

    };
};



#endif