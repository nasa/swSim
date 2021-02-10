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

#ifndef SWSIM_SPARSE_MATRIX_H
#define SWSIM_SPARSE_MATRIX_H

#include <string>
#include <Eigen/Sparse>

#include "SparseMatrix_base.hpp"

namespace swSim{
    /*! \class SparseMatrix SparseMatrix.hpp "include/SparseMatrix.hpp"
    *
    * Defines data structures for providing sparse matrices to KernelComposer
    */
    class SparseMatrix: public SparseMatrix_base{
        public:
        Eigen::SparseMatrix<double,1> mat; /*!< Underlying Matrix */
        /*!
        * Create the SparseMatrix
        */
        SparseMatrix();
        /*!
        * Create the SparseMatrix with name, rows and columns
        */
        SparseMatrix(std::string nm, int rw, int col); 
        /*!
        * Destroy the SparseMatrix
        */ 
        ~SparseMatrix(); 
        /*!
        * Set the name of the SparseMatrix
        */ 
        void setName(std::string nm);
        /*!
        * Resize this matrix
        */
        void setSize(int rw, int col);
        /*!
        * Get Key-Name for this matrix
        */
        std::string getName() const;
        /*!
        * Get length of value array
        */
        size_t getNonZeroLength() const;
        /*!
        * Get pointer to head of value array
        */
        double* getValueArray();
        /*!
        * Get pointer to head of column index array
        */
        int* getColumnIndexArray();
        /*!
        * Get pointer to head of rowpointer array
        */
        int* getRowpointerArray();
        /*!
        * Get length of rowpointer array
        */
        size_t getRowpointerLength() const;
        /*!
        * Get row dimension of the matrix
        */
        int getRowCount() const;
        /*!
        * Get column dimension of the matrix
        */
        int getColumnCount() const;
        /*!
        * Set the value at row i and column j 
        */ 
        void SetValue(int i, int j, double val);
        /*!
        * Get the value at row i and column j 
        */ 
        double GetValue(int i, int j);
        /*!
        * Add to the value at row i and column j 
        */ 
        void AddValue(int i, int j, double val);
        /*!
        * Prune Zero values
        */ 
        void Prune();


        private:
        std::string name;  /*!< Matrix Name*/
    };
};

#endif /*SWSIM_SPARSE_MATRIX_H*/