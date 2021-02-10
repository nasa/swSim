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

#ifndef SWSIM_DENSE_VECTOR_BASE_H
#define SWSIM_DENSE_VECTOR_BASE_H

#include <string>

namespace swSim{
    /*!
    * enum type returning error/warning states.
    */
    enum DenseVectorError_t{
        DNV_OK=0,/*!< no warnings or errors */
        ERROR_GENERIC/*!< some error encountered. */
    };
    
    /*! \class DenseVector DenseVector.hpp "include/DenseVector.hpp"
    *
    * Defines data structures for providing dense vectors to KernelComposer
    */
    class DenseVector_base{
        public:
        /*!
        * Get Key-Name for this vector
        */
        virtual std::string getName() const =0;
        /*!
        * Get pointer to head of value array
        */
        virtual double* getValueArray() =0;
        /*!
        * Get length of value array
        */
        virtual size_t getArrayLength() const =0;

        //probably should rename these to fit the style guide, but for now, the names work
        /*!
        * execute halo exchange for this data
        */
        virtual DenseVectorError_t sendrecv_MPI() =0;
        /*!
        * execute halo exchange for this data using cuda-aware MPI
        */
        virtual DenseVectorError_t sendrecv_MPI(double* DnV_d) =0;
    };
};

#endif /*SWSIM_DENSE_VECTOR_BASE_H*/