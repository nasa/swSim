/*! \headerfile DenseVector.hpp "include/DenseVector.hpp"
* "DenseVector.hpp" contains the class definition encapsulating the 
* data structure interface for providing dense vectors to KernelComposer
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

#ifndef SWSIM_DENSE_VECTOR_H
#define SWSIM_DENSE_VECTOR_H

#include <mpi.h>
#include <string>
#include <vector>
#include "Segmentation.hpp"

#include "DenseVector_base.hpp"

namespace swSim{
    /*! \class DenseVector DenseVector.hpp "include/DenseVector.hpp"
    *
    * Defines data structures for providing dense vectors to KernelComposer
    */
    class DenseVector: public DenseVector_base{
        public:
        std::vector<double> vec; /*!< Underlying Vector */
        swSim::Segmentation* seg; /*!< Segmentation definition fo the vector */
        /*!
        * Create the DenseVector
        */
        DenseVector();
        /*!
        * Create the DenseVector with a length and name
        */ 
        DenseVector(std::string nm, int length);
        /*!
        * Destroy the DenseVector
        */ 
        ~DenseVector();
        /*!
        * Set the name of the DenseVector
        */ 
        void setName(std::string nm);
        /*!
        * Make a created vector the underlying vector
        */ 
        void setVector(std::vector<double> v);
        /*!
        * Make a created array the underlying vector
        */ 
        void setVector(double* v, int length);
        /*!
        * Get Key-Name for this vector
        */
        std::string getName() const;
        /*!
        * Get pointer to head of value array
        */
        double* getValueArray();
       /*!
        * Get length of value array
        */
        size_t getArrayLength() const;
        //probably should rename these to fit the style guide, but for now, the names work
        /*!
        * Execute halo exchange for this data
        */
        DenseVectorError_t sendrecv_MPI();
        /*!
        * Execute halo exchange for this data using cuda-aware MPI
        */
        DenseVectorError_t sendrecv_MPI(double* DnV_d);
        /*!
        * Set the Segmentation
        */ 
        void configureSegmentedComms(swSim::Segmentation &seg);
        /*!
        * Set the MPI Communicator
        */ 
        void mpiComm(MPI_Comm comm);


        private:
        std::string name; /*!< Vector Name */
        MPI_Comm comm; 
    };
};

#endif /*SWSIM_DENSE_VECTOR_H*/
