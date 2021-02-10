/*! \headerfile SparseMatrix.hpp "include/SparseMatrix.hpp"
* "SparseMatrix.hpp" contains the class definition encapsulating the 
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

#ifndef SWSIM_SPARSE_MATRIX_BASE_H
#define SWSIM_SPARSE_MATRIX_BASE_H

#include <string>
//#include <Eigen/Sparse>

namespace swSim{
    /*! \class SparseMatrix SparseMatrix.hpp "include/SparseMatrix.hpp"
    *
    * Defines data structures for providing sparse matrices to KernelComposer
    */
    class SparseMatrix_base{
        public:
        /*!
        * Get Key-Name for this matrix
        */
        virtual std::string getName() const =0;
        /*!
        * Get length of value array
        */
        virtual size_t getNonZeroLength() const =0;
        /*!
        * Get pointer to head of value array
        */
        virtual double* getValueArray() =0;
        /*!
        * Get pointer to head of column index array
        */
        virtual int* getColumnIndexArray() =0;
        /*!
        * Get pointer to head of rowpointer array
        */
        virtual int* getRowpointerArray() =0;
        /*!
        * Get length of rowpointer array
        */
        virtual size_t getRowpointerLength() const =0;
        /*!
        * Get row dimension of the matrix
        */
        virtual int getRowCount() const =0;
        /*!
        * Get column dimension of the matrix
        */
        virtual int getColumnCount() const =0;
    };
};

#endif /*SWSIM_SPARSE_MATRIX_BASE_H*/