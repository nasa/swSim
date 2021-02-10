/*! \headerfile SparseVector.hpp "include/SparseVector.hpp"
* "SparseVector.hpp" contains the class definition encapsulating the 
* data structure interface for providing sparse vectors to KernelComposer
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

#ifndef SWSIM_SPARSE_VECTOR_H
#define SWSIM_SPARSE_VECTOR_H

#include <string>
#include <Eigen/Sparse>

#include "SparseVector_base.hpp"

namespace swSim{
    /*! \class SparseVector SparseVector.hpp "include/SparseVector.hpp"
    *
    * Defines data structures for providing sparse vectors to KernelComposer
    */
    class SparseVector: public SparseVector_base{
        public:
        Eigen::SparseVector<double> vec; /*!< Underlying Vector */
        
         /*!
        * Create the SparseVector
        */
        SparseVector();
        /*!
        * Create the SparseVector with a length and name
        */ 
        SparseVector(std::string nm, int length);
        /*!
        * Destroy the SparseVector
        */ 
        ~SparseVector();
        /*!
        * Set the name of the SparseVector
        */ 
        void setName(std::string nm);
        /*!
        * Get Key-Name for this vector
        */
        std::string getName() const;
        /*!
        * Get pointer to head of value array
        */
        double* getValueArray();
        /*!
        * Get pointer to head of index array
        */
        int* getIndexArray();
        /*!
        * Get length of value array
        */
        size_t getNonZeroLength() const;
        /*!
        * Get full length of vector (incl. zeros)
        */
        size_t getFullLength() const;
        /*!
        * [Deprecated] Update Sparse vector values based on time index
        */
        void update(int kt);

        private:

        std::string name; /*!< Vector Name*/
        //void (*UpdateSpV)(int);
    };
};

#endif /*SWSIM_SPARSE_VECTOR_H*/