/*! \headerfile Segmentation.hpp "include/Segmentation.hpp"
* "Segmentation.hpp" contains the class definition encapsulating the 
* data structure interface for providing sparse matrices to KernelComposer
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

#ifndef SEGMENTATION_HPP
#define SEGMENTATION_HPP

#include <mpi.h>
#include <stdint.h>
#include <fstream>  //file input/output
#include <stdio.h>
#include <vector>

#include "Geometry.hpp"
#include "Model.hpp"
#include "MPIFileReader.hpp"

namespace swSim{
    /*! \class Segmentation Segmentation.hpp "include/Segmentation.hpp"
    *
    * Defines data structures for providing Segmentation object to swSim
    */
    class Segmentation {
    public:
        int xIndex0; /*!< the global X index of the first slice for this process*/
        int xDelta; /*!< the number of YZ slices iin this process */
        int neighborLeft; /*!< the rank of this processes left neightbor */
        int neighborRight; /*!< the rank of this processes right neightbor */
        uint64_t globalIndex0; /*!< the absolute, global index of the first voxel in this process */
        uint64_t processVoxels; /*!< the number of voxels in this process */
        uint64_t lHaloVoxels; /*!< the number of voxels that are in the left communication halo in this process */
        uint64_t rHaloVoxels; /*!< the number of voxels that are in the right communication halo in this process */
        MPI_Datatype recieveLeftType; /*!< the MPI data type for recieving data from the left */
        MPI_Datatype sendLeftType; /*!< the MPI data type for sending data to the left */
        MPI_Datatype recieveRightType; /*!< the MPI data type for recieving data from the right */
        MPI_Datatype sendRightType; /*!< the MPI data type for sending data to the right */

        /*!
        * Create the Segmentation
        */
        Segmentation();
        /*!
        * Create the Segmentation with a model and a communicator
        */
        Segmentation(swSim::Model &mod, MPI_Comm l_mpi_comm);
        /*!
        * Create the Segmentation with another Segmentation
        */
        Segmentation(swSim::Segmentation &otherSeg);
        /*!
        * Destroy the Segmentation
        */
        ~Segmentation();
        /*!
        * Sums a vector.
        */
        uint64_t SumVector(std::vector<uint64_t> &vec);
        /*!
        * Balances the number of slices for each process
        */ 
        void LoadBalance(std::vector<uint64_t> &planeCounts, int rank, int size);
        /*!
        * Determines the number of slices for this process
        */  
        int MeetTargetCells(std::vector<uint64_t> &planeCounts, int xpos, uint64_t &globalIndexVox, uint64_t target_number_of_cells);
        /*!
        * Sets the Communication values on the left
        */ 
        void SetLeftValues(std::vector<uint64_t> &planeCounts, int xpos, uint64_t globalIndexVox);
        /*!
        * Sets the Communication values on the right
        */ 
        void SetRightValues(std::vector<uint64_t> &planeCounts, int xpos);
        /*!
        * Sets the Communication values on the right
        */ 
        void SetRightValues(std::vector<uint64_t>::size_type planeCountsSize, uint64_t total_cells_remaining);
        /*!
        * Creates the MPI datatype for recieving from the left
        */ 
        void MPICreateLeftReceive(std::vector<int32_t> &lrNodes);
        /*!
        * Creates the MPI datatype for sending to the left
        */ 
        void MPICreateLeftSend(std::vector<int32_t> &lsNodes);
        /*!
        * Creates the MPI datatype for sending to the right
        */ 
        void MPICreateRightSend(std::vector<int32_t> &rsNodes);
        /*!
        * Creates the MPI datatype for recieving from the right
        */ 
        void MPICreateRightReceive(std::vector<int32_t> &rrNodes);
    };
};

#endif