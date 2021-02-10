/*! \headerfile KernelComposerManager.hpp "include/KernelComposerManager.hpp"
* "KernelComposerManager.hpp" contains the base class definition encapsulating 
* the data structures needed to register and track the user-defined composition 
* of the sequence of operations defining the user's desired kernel.
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

#ifndef SWSIM_KERNEL_COMPOSER_MANAGER_H
#define SWSIM_KERNEL_COMPOSER_MANAGER_H

#define _CUBLAS

#include <vector>
#include <string>
#include <unordered_map>

#include <cusparse.h>
#include <cuda.h>

#ifdef _CUBLAS
    #include <cublas_v2.h>
#endif

/*contains: name, Nrows, Ncols, Value array, 
            column index array, row pointer array*/
#include "SparseMatrix_base.hpp"

/*contains: name, value array, "comms info"*/
#include "DenseVector_base.hpp"

/*contains: name, value array, index array*/
#include "SparseVector_base.hpp"

/*contains: helpers regarding error flags, etc*/
#include "KernelComposerHelpers.hpp"


namespace swSim{
    /*! \class KernelComposerManager KernelComposerManager.hpp "include/KernelComposerManager.hpp"
    * 
    * Defines the data structures required to implement the user-defined composition
    * of the compound compute kernel. Defines packaged structures for mapped data between
    * host and device, as well as unordered maps for access to the lists of vectors and
    * matrices. Further structure is provided for the mapping and ordering of the operations
    * to be performed on the device.
    */
    class KernelComposerManager{
        public:
        /*!
        * Destructor that ensures any device reservations have been released.
        */
        ~KernelComposerManager();
        /*!
        * Commits current data to device memory.
        */
        void commit();
        /*!
        * Commits current data to device memory.
        */
        void commit(int device_id);
        /*!
        * actually execute composed kernel. pass in current time index for any host activity sync
        */
        void execute(int tidx);
        /*!
        * releases device memory. 
        * if commit is called, release should be called (like malloc/free)
        */
        void release();
        
        /*!
        * \struct MappedDenseVector KernelComposerManager.hpp "include/KernelComposerManager.hpp"
        *
        * contains pointer to the host dense vector data object and device data memory.
        */        
        struct MappedDenseVector{
            DenseVector_base* vector=NULL;/*!< pointer to host data object */
            cusparseDnVecDescr_t cusparse_vector;/*!< vector descriptor for the cusparse library */
            double* vector_d=NULL;/*!< pointer to device value array */
            int vector_length=0;/*< vector length as placed on the device*/
        };
        /*!
        * \struct MappedSparseVector KernelComposerManager.hpp "include/KernelComposerManager.hpp"
        *
        * contains pointer to the host sparse vector data object and device data memory.
        */
        struct MappedSparseVector{
            SparseVector_base* vector=NULL;/*!< pointer to host data object */
            cusparseSpVecDescr_t cusparse_vector;/*!< vector descriptor for the cusparse library */
            double* vector_d=NULL;/*!< pointer to device value array */
            int* idx_d=NULL;/*!< pointer to device index array */
            int compressed_length=0;/*!< compressed length of the vector*/
        };
        /*!
        * \struct MappedSparseMatrix KernelComposerManager.hpp "include/KernelComposerManager.hpp"
        *
        * contains pointer to the host sparse matrix data object and device data memory.
        */
        struct MappedSparseMatrix{
            SparseMatrix_base* matrix=NULL;/*!< pointer to host data object */
            cusparseSpMatDescr_t cusparse_matrix;/*!< matrix descriptor for the cusparse library */
            double* values_d=NULL;/*!< pointer to device value array */
            int* columns_d=NULL;/*!< pointer to device column index array */
            int* rowpointers_d=NULL;/*!< pointer to device rowpointer array */
            int rowpointer_length=0;/*!< length of rowpointer array*/
            int num_matrix_entries=0;/*!< number of defined entries in matrix*/
        };
        
        //these calls are only good for unchanged array sizes
        void getDenseVectorData(MappedDenseVector& vector);
        void setDenseVectorData(MappedDenseVector& vector);

        void getSparseVectorData(MappedSparseVector& vector);
        void setSparseVectorData(MappedSparseVector& vector);
        
        void getSparseMatrixData(MappedSparseMatrix& matrix);
        void setSparseMatrixData(MappedSparseMatrix& matrix);
        
        
        std::unordered_map<std::string,MappedDenseVector> dense_vectors;/*!< 
            unordered map of dense vectors, keyed by the user-provided vector name */
        
        std::unordered_map<std::string,MappedSparseVector> sparse_vectors;/*!< 
            unordered map of sparse vectors, keyed by the user-provided vector name */
        
        std::unordered_map<std::string,MappedSparseMatrix> sparse_matrices;/*!< 
            unordered map of sparse matrices, keyed by the user-provided matrix name */
        
        /*!
        * \enum OpType KernelComposerManager.hpp "include/KernelComposerManager.hpp"
        *
        * enum definition that contains all available operations to register with the 
        * KernelComposer
        */
        enum OpType
        {
            NOOP=0,/*!< Default No-Operation setting. Should never be selected at commit */
            SPMV=1,/*!< SPMV registration: scalar a * sparse M * dense X + scalar b * dense Y*/
            SXPY=2,/*!< Sparse AXPY registration: scalar a * sparse X + dense Y*/
            DXPY=3,/*!< Dense AXPY registration: scalar a * dense X + dense Y*/
            UPDT=4,/*!< Device update from host registration */
            COMM=5 /*!< MPI communication registration*/
        };
        
        /*!
        * \struct Operation KernelComposerManager.hpp "include/KernelComposerManager.hpp"
        *
        * container for metadata required to define all available operations
        */
        struct Operation
        {
            OpType T=NOOP;/*!< What operation is being registered */
            std::string matrix_name;/*!< matrix key-name, if applicable */
            std::string argument_name;/*!< argument vector key-name, if applicable */
            std::string result_name;/*!< resultant vector key-name, if applicable */
            double alpha=0.0;/*!< scalar coefficient on argument vector, if applicable */
            double beta=0.0;/*!< scalar coefficient preapplied on resultant vector, if applicable */
            std::vector<double>* scale=NULL;/*!< array of scalars for time-dependent coefficients */
            void* buffer_d=NULL;/*!< device working buffer */
            size_t buffer_size=0;/*!< device working buffer size */
        };
        
        cusparseHandle_t sphandle;/*!< cuSparse context handle */
        
        #ifdef _CUBLAS
            cublasHandle_t blashandle;/*!< cuBlas context handle */
        #endif

        std::vector<Operation> operations;/*!< vector containing the operations for a single cycle, in execution order */
        
        bool deviceIsCommitted=false;/*!< flag indicating device commit state */

        int device=0;
    };
 };
 
 #endif /*SWSIM_KERNEL_COMPOSER_MANAGER_H*/