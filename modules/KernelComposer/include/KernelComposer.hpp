/*! \headerfile KernelComposer.hpp "include/KernelComposer.hpp"
* "KernelComposer.hpp" contains the class definition encapsulating 
* the construction of a sequence of operations. These 
* operations are performed in the order in which they are
* set. The sequence as constructed is then performed for 
* each time step.
*
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

#ifndef SWSIM_KERNEL_COMPOSER_H
#define SWSIM_KERNEL_COMPOSER_H

#include <string>
//#include <unordered_map>
//#include <vector>
//#include <cusparse>

/*contains: name, Nrows, Ncols, Value array, 
            column index array, row pointer array*/
#include "SparseMatrix_base.hpp"

/*contains: name, value array, "comms info"*/
#include "DenseVector_base.hpp"

/*contains: name, value array, index array*/
#include "SparseVector_base.hpp"

/*contains: all the private data structures and 
            functionality that does not need exposure
            outside the inner workings of this class*/
#include "KernelComposerManager.hpp"

/*contains: helpers regarding error flags, etc*/
#include "KernelComposerHelpers.hpp"

namespace swSim{
    /*! \class KernelComposer KernelComposer.hpp "include/KernelComposer.hpp"
    *
    * Defines the construction of a sequence of comms, 
    * Y=a*SpM*X+b*Y, and Y=a*X+Y operations to be
    * performed each time step. This class includes 
    * GPU device allocation, execution, and cleanup.
    *
    */
    class KernelComposer : private KernelComposerManager{
        public:
        /*! 
        * Destructor to ensure GPU device is freed upon
        * destruction of KernelComposer object.
        */
        ~KernelComposer();
        
        /*! 
        * register dense vector data with KernelComposer.
        * \param dense_vector points to an object representing the dense vector
        */
        KernelComposerError_t setDenseVector(DenseVector_base* dense_vector);
        /*! 
        * register sparse vector data with KernelComposer.
        * \param sparse_vector points to an object representing the sparse vector
        */
        KernelComposerError_t setSparseVector(SparseVector_base* sparse_vector);
        /*! 
        * register sparse matrix data with KernelComposer.
        * \param sparse_matrix points to an object representing the sparse matrix
        */
        KernelComposerError_t setSparseMatrix(SparseMatrix_base* sparse_matrix);
        
        /*!
        * [DEPRECATED] register a sparse vector update (from host) operation with KernelComposer.
        * \param sparse_vector_name key-string that indicates which sparse vector is to be updated
        */
        KernelComposerError_t setUpdateSparseVectorFromHost(std::string sparse_vector_name);
        /*!
        * register a BLAS 2 Sparse Matrix/Vector product (SPMV) operation with KernelComposer. 
        * Operation performs: $Y=a*A*X+b*Y$ where $X$ and $Y$ are dense vectors, 
        *                     $a$ and $b$ are coefficients, and $A$ is a sparse matrix.
        * NOTE: $X$ and $Y$ CANNOT BE THE SAME VECTOR. DO NOT DO THIS.
        * \param matrix_name Key-name for which sparse matrix to use in the operation
        * \param vector_x_name Key-name for which dense vector it to be multiplied against the sparse matrix
        * \param vector_y_name Key-name for which dense vector is to be accumulated to.
        * \param a scalar coefficient on the matrix-vector product
        * \param b scalar coefficient on the result vector prior to accumulation of the matrix-vector product
        */
        KernelComposerError_t setSPMV(std::string matrix_name,std::string vector_x_name,
                                std::string vector_y_name,double a,double b);                        
        /*!
        * register MPI communication operation with KernelComposer
        * \param vector_name dense vector undergoing MPI comms (such as halo exchange)
        */
        KernelComposerError_t setMPIcomms(std::string vector_name);
        /*!
        * register a BLAS 1 Sparse Vector/Vector element-wise sum (SPaXPY) operation with KernelComposer
        * Operation performs: $Y=a*X+Y$ where $Y$ is a dense vector, $X$ is a sparse vector, and $a$ is
        *                     a scalar coefficient on the sparse vector.
        * \param sparse_vector_x_name Key-name for sparse vector to be added to $Y$
        * \param vector_y_name Key-name for dense vector being augmented by $X$
        * \param a scalar coefficient on the sparse vector.
        */
        KernelComposerError_t setSPaXPY(std::string sparse_vector_x_name,
                                        std::string vector_y_name,std::vector<double>* scale);
        /*!
        * register a BLAS 1 Vector/Vector element-wise sum (AXPY) operation with KernelComposer
        * Operation performs: $Y=a*X+Y$ where $Y$ and is a dense vector, and $a$ is
        *                     a scalar coefficient on the sparse $X$ vector.
        * \param vector_x_name Key-name for sparse vector to be added to $Y$
        * \param vector_y_name Key-name for dense vector being augmented by $X$
        * \param scale is a std::vector of time-domain scalar coefficient on the sparse vector.
        */
        KernelComposerError_t setAXPY(std::string vector_x_name,
                                      std::string vector_y_name,double a);
        
        
        /*!
        * commit data to GPU and build cuSparse objects and working buffers (if necessary)
        */
        KernelComposerError_t kernelCommit();
        /*!
        * free all device resources allocated in kernelCommit()
        */
        KernelComposerError_t kernelCommit(int device_id);
        /*!
        * free all device resources allocated in kernelCommit()
        */
        KernelComposerError_t kernelDecommit();
        /*!
        * execute operations sequence $num_steps$ times. deviceCommit() must be called prior.
        * \param num_steps number of executions of operations sequence to be executed
        */
        KernelComposerError_t run(int num_steps);
        /*!
        * [DEPRECATED] execute operations sequence from timestep 0 to $num_steps$ times. 
        * deviceCommit() must be called prior. The state variables are not reset,
        * thus this "re-runs" the model for $num_steps$ with an initial condition dictated 
        * by the end state of the prior run. Really was intended for when intermediate results
        * were unecessary to save.
        * \param num_steps number of executions of operations sequence to be executed
        */
        KernelComposerError_t run(int start_time_idx,int end_time_idx);
        /*!
        * copy data associated with key-name vector_name from GPU to host.
        */
        KernelComposerError_t getDeviceData(std::string key_name);
        /*!
        * copy data associated with key-name vector_name from host to GPU.
        */
        KernelComposerError_t setDeviceData(std::string key_name);
        
        /*!
        * utility to check if data has been committed to device.
        */
        bool isCommitted();
    };
};
#endif /*SWSIM_KERNEL_COMPOSER_H*/