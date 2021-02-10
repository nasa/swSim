// Manager Implementation

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>

#define CUDA_AWARE_MPI TRUE
//
#define _GOCUSPARSE

#ifdef _GOCUSPARSE
    #include <cusparse.h>
#endif
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

/*contains: self class definition*/
#include "KernelComposerManager.hpp"

# ifndef _CUBLAS
    __global__
    void DAXPY(double* X, double* Y,size_t N,double a){
        int tidx=threadIdx.x+blockDim.x*blockIdx.x;
        if(tidx<N){
            Y[tidx]+=a*X[tidx];
        };
    };
#endif
#ifndef _GOCUSPARSE
    __global__
    void KCSpMV(double alpha,
                swSim::KernelComposerManager::MappedSparseMatrix A,
                swSim::KernelComposerManager::MappedDenseVector X,
                double beta,
                swSim::KernelComposerManager::MappedDenseVector Y){
        int tidx=threadIdx.x+blockDim.x*blockIdx.x;
        int aidx;
        if(tidx<(Y.vector_length-1)){
            Y.vector_d[tidx]=beta*Y.vector_d[tidx];
            for(int midx=A.rowpointers_d[tidx];
                    midx<A.rowpointers_d[tidx+1];midx++){
                aidx=A.columns_d[midx];
                Y.vector_d[tidx]+=alpha*A.values_d[midx]*X.vector_d[aidx];
            };
        }else if(tidx==(Y.vector_length-1)){
            Y.vector_d[tidx]=beta*Y.vector_d[tidx];
            for(int midx=A.rowpointers_d[tidx];
                    midx<A.num_matrix_entries;midx++){
                aidx=A.columns_d[midx];
                Y.vector_d[tidx]+=alpha*A.values_d[midx]*X.vector_d[aidx];
            };    
        };


    };
#endif

// wrapper to print Cuda errors
inline void CHECK_CUDA(cudaError_t err){if(cudaSuccess!=err){printf("%s\n",cudaGetErrorName(err));throw std::runtime_error("Cuda runtime error.");};};

swSim::KernelComposerManager::~KernelComposerManager(){
    if(deviceIsCommitted){
        release();
    };
};

void swSim::KernelComposerManager::release(){
    CHECK_CUDA(cudaSetDevice(device));
    if(deviceIsCommitted){
        for(auto it=dense_vectors.begin();
                 it!=dense_vectors.end();it++){
            //release cusparse_vector
            cusparseDestroyDnVec(it->second.cusparse_vector);
            if(it->second.vector_d){
                CHECK_CUDA(cudaFree(it->second.vector_d));
                it->second.vector_d=NULL;
            };
            it->second.vector_length=0;
        };
        for(auto it=sparse_vectors.begin();
                 it!=sparse_vectors.end();it++){
            //release cusparse vector
            cusparseDestroySpVec(it->second.cusparse_vector);
            if(it->second.vector_d){
                CHECK_CUDA(cudaFree(it->second.vector_d));
                it->second.vector_d=NULL;
            };
            if(it->second.idx_d){
                CHECK_CUDA(cudaFree(it->second.idx_d));
                it->second.idx_d=NULL;
            };
            it->second.compressed_length=0;
        };
        for(auto it=sparse_matrices.begin();
                 it!=sparse_matrices.end();it++){
            //release cusparse matrix
            cusparseDestroySpMat(it->second.cusparse_matrix);
            if(it->second.values_d){
                CHECK_CUDA(cudaFree(it->second.values_d));
                it->second.values_d=NULL;
            };
            if(it->second.columns_d){
                CHECK_CUDA(cudaFree(it->second.columns_d));
                it->second.columns_d=NULL;
            };
            if(it->second.rowpointers_d){
                CHECK_CUDA(cudaFree(it->second.rowpointers_d));
                it->second.rowpointers_d=NULL;
            };
            it->second.num_matrix_entries=0;
            it->second.rowpointer_length=0;
        };
        for(auto it=operations.begin();
                 it!=operations.end();it++){
            if(it->buffer_d){
                CHECK_CUDA(cudaFree(it->buffer_d));
                it->buffer_d=NULL;
            };
            it->buffer_size=0;
        };
        cusparseDestroy(sphandle);
        
        #ifdef _CUBLAS
            cublasDestroy(blashandle);
        #endif
        
        deviceIsCommitted=false;
    };
};
void swSim::KernelComposerManager::commit(int device_id){
    int ndev;
    CHECK_CUDA(cudaGetDeviceCount(&ndev));
    if(device_id>=ndev){throw std::runtime_error("device_id exceeds number of GPUs on system");};
    device=device_id;
    commit();
};

void swSim::KernelComposerManager::commit(){
    CHECK_CUDA(cudaSetDevice(device));
    if(!deviceIsCommitted){
        cusparseCreate(&sphandle);

        #ifdef _CUBLAS
            cublasCreate(&blashandle);
        #endif

        for(auto it=dense_vectors.begin();
                 it!=dense_vectors.end();it++){
            if(it->second.vector_d){
                CHECK_CUDA(cudaFree(it->second.vector_d));
                it->second.vector_d=NULL;
            };
            it->second.vector_length=it->second.vector->getArrayLength();
            //cudaMalloc(&it->second.vector_d,it->second.vector_length*sizeof(double));
            //cudaMemcpy(it->second.vector_d,it->second.vector->getValueArray(),it->second.vector_length*sizeof(double),cudaMemcpyHostToDevice);
            CHECK_CUDA(cudaMallocManaged(&it->second.vector_d,
                       it->second.vector_length*sizeof(double)));
            memcpy(it->second.vector_d,it->second.vector->getValueArray(),it->second.vector_length*sizeof(double));
            cusparseCreateDnVec(&(it->second.cusparse_vector),
                                it->second.vector_length,
                                it->second.vector_d,CUDA_R_64F);
        };
        for(auto it=sparse_vectors.begin();
                 it!=sparse_vectors.end();it++){
            if(it->second.vector_d){
                CHECK_CUDA(cudaFree(it->second.vector_d));
                it->second.vector_d=NULL;
            };
            it->second.compressed_length=it->second.vector->getNonZeroLength();
            //cudaMalloc(&it->second.vector_d,it->second.compressed_length*sizeof(double));
            //cudaMemcpy(it->second.vector_d,it->second.vector->getValueArray(),it->second.compressed_length*sizeof(double),cudaMemcpyHostToDevice);
            CHECK_CUDA(cudaMallocManaged(&it->second.vector_d,it->second.compressed_length*sizeof(double)));
            memcpy(it->second.vector_d,it->second.vector->getValueArray(),it->second.compressed_length*sizeof(double));
            if(it->second.idx_d){
                CHECK_CUDA(cudaFree(it->second.idx_d));
                it->second.idx_d=NULL;
            };
            //cudaMalloc(&it->second.idx_d,it->second.compressed_length*sizeof(int));
            //cudaMemcpy(it->second.idx_d,it->second.vector->getIndexArray(),it->second.compressed_length*sizeof(int),cudaMemcpyHostToDevice);
            CHECK_CUDA(cudaMallocManaged(&it->second.idx_d,it->second.compressed_length*sizeof(int)));
            memcpy(it->second.idx_d,it->second.vector->getIndexArray(),it->second.compressed_length*sizeof(int));
            //build cusparse vector
            cusparseCreateSpVec(&(it->second.cusparse_vector),
                                it->second.vector->getFullLength(),
                                it->second.compressed_length,
                                it->second.idx_d,it->second.vector_d,
                                CUSPARSE_INDEX_32I,
                                CUSPARSE_INDEX_BASE_ZERO,
                                CUDA_R_64F);
        };
        for(auto it=sparse_matrices.begin();
                 it!=sparse_matrices.end();it++){
            if(it->second.values_d){
                CHECK_CUDA(cudaFree(it->second.values_d));
                it->second.values_d=NULL;
            };
            it->second.num_matrix_entries=
                        it->second.matrix->getNonZeroLength();
            //cudaMalloc(&it->second.values_d,it->second.num_matrix_entries*sizeof(double));
            //cudaMemcpy(it->second.values_d,it->second.matrix->getValueArray(),it->second.num_matrix_entries*sizeof(double),cudaMemcpyHostToDevice);
            CHECK_CUDA(cudaMallocManaged(&it->second.values_d,it->second.num_matrix_entries*sizeof(double)));
            memcpy(it->second.values_d,it->second.matrix->getValueArray(),it->second.num_matrix_entries*sizeof(double));
            if(it->second.columns_d){
                CHECK_CUDA(cudaFree(it->second.columns_d));
                it->second.columns_d=NULL;
            };
            //cudaMalloc(&it->second.columns_d,it->second.num_matrix_entries*sizeof(int));
            //cudaMemcpy(it->second.columns_d,it->second.matrix->getColumnIndexArray(),it->second.num_matrix_entries*sizeof(int),cudaMemcpyHostToDevice);
            CHECK_CUDA(cudaMallocManaged(&it->second.columns_d,it->second.num_matrix_entries*sizeof(int)));
            memcpy(it->second.columns_d,it->second.matrix->getColumnIndexArray(),it->second.num_matrix_entries*sizeof(int));
            if(it->second.rowpointers_d){
                CHECK_CUDA(cudaFree(it->second.rowpointers_d));
                it->second.rowpointers_d=NULL;
            };
            it->second.rowpointer_length=
                    it->second.matrix->getRowpointerLength();
            //cudaMalloc(&it->second.rowpointers_d,it->second.rowpointer_length*sizeof(int));
            //cudaMemcpy(it->second.rowpointers_d,it->second.matrix->getRowpointerArray(),it->second.rowpointer_length*sizeof(int),cudaMemcpyHostToDevice);
            CHECK_CUDA(cudaMallocManaged(&it->second.rowpointers_d,it->second.rowpointer_length*sizeof(int)));
            memcpy(it->second.rowpointers_d,it->second.matrix->getRowpointerArray(),it->second.rowpointer_length*sizeof(int));
            
            //eigen doesn't include this value at tail-end charlie
            it->second.rowpointers_d[it->second.rowpointer_length-1]=it->second.matrix->getColumnCount();
            
            //build cusparse matrix
            cusparseCreateCsr(&(it->second.cusparse_matrix),
                        it->second.matrix->getRowCount(),
                        it->second.matrix->getColumnCount(),
                        it->second.num_matrix_entries,
                        it->second.rowpointers_d,
                        it->second.columns_d,it->second.values_d,
                        CUSPARSE_INDEX_32I, 
                        CUSPARSE_INDEX_32I,
                        CUSPARSE_INDEX_BASE_ZERO, 
                        CUDA_R_64F);
        };
        for(auto it=operations.begin();
                 it!=operations.end();it++){
            if(it->buffer_d){
                CHECK_CUDA(cudaFree(it->buffer_d));
                it->buffer_d=NULL;
                it->buffer_size=0;
            };
            if(it->T==SPMV){
                cusparseSpMV_bufferSize(sphandle,
                    CUSPARSE_OPERATION_NON_TRANSPOSE,
                    &(it->alpha),
                    sparse_matrices.
                        at(it->matrix_name.c_str()).
                        cusparse_matrix,
                    dense_vectors.
                        at(it->argument_name.c_str()).
                        cusparse_vector,
                    &(it->beta),
                    dense_vectors.
                        at(it->result_name.c_str()).
                        cusparse_vector,
                    CUDA_R_64F,CUSPARSE_MV_ALG_DEFAULT,
                    &(it->buffer_size));
                CHECK_CUDA(cudaMallocManaged(&(it->buffer_d),(it->buffer_size>0)?(it->buffer_size):1));
            };
        };
        deviceIsCommitted=true;
    };
};
                             
void swSim::KernelComposerManager::execute(int tidx){
    CHECK_CUDA(cudaSetDevice(device));
    if(deviceIsCommitted){
        for(auto it=operations.begin();
                 it!=operations.end();it++){
            switch(it->T){
                case(NOOP):
                break;
                case(SPMV):
                #ifdef _GOCUSPARSE
                //call cusparseSpMV();
                cusparseSpMV(sphandle,
                    CUSPARSE_OPERATION_NON_TRANSPOSE,
                    &(it->alpha),
                    sparse_matrices.
                        at(it->matrix_name).
                        cusparse_matrix,
                    dense_vectors.
                        at(it->argument_name).
                        cusparse_vector,
                    &(it->beta),
                    dense_vectors.
                        at(it->result_name).
                        cusparse_vector,
                    CUDA_R_64F,CUSPARSE_MV_ALG_DEFAULT,
                    &(it->buffer_d));
                #else
                int device;
                cudaGetDevice(&device);
                printf("%i: %s=%f*%s*%s+%f*%s\n",device,
                        it->result_name.c_str(),it->alpha,
                        it->matrix_name.c_str(),
                        it->argument_name.c_str(),it->beta,
                        it->result_name.c_str());
KCSpMV<<<dense_vectors.at(it->argument_name).vector_length/32+1,32>>>(it->alpha,sparse_matrices.at(it->matrix_name),
                       dense_vectors.at(it->argument_name),
                       it->beta,dense_vectors.at(it->result_name));
                #endif
                CHECK_CUDA(cudaDeviceSynchronize());
                break;
                case(SXPY):
                //call aSpXPY
                if(it->scale->size()>tidx){
                cusparseDaxpyi(sphandle,
                        sparse_vectors.
                            at(it->argument_name).
                            compressed_length,
                        &(it->scale->at(tidx)),
                        sparse_vectors.
                            at(it->argument_name).
                            vector_d,
                        sparse_vectors.
                            at(it->argument_name).
                            idx_d,
                        dense_vectors.
                            at(it->result_name).
                            vector_d,
                        CUSPARSE_INDEX_BASE_ZERO);
                };
                CHECK_CUDA(cudaDeviceSynchronize());
                break;
                case(DXPY):
                //call AXPY
                #ifdef _CUBLAS
                    cublasDaxpy(blashandle,
                            dense_vectors.
                                at(it->result_name).
                                vector_length,
                            &(it->alpha),
                            dense_vectors.
                                at(it->argument_name).
                                vector_d,1,
                            dense_vectors.
                                at(it->result_name).
                                vector_d,1);
                #else
DAXPY<<<dense_vectors.at(it->argument_name).vector_length/32+1,32>>>
                    (dense_vectors.at(it->argument_name).vector_d,
                     dense_vectors.at(it->result_name).vector_d,
                     dense_vectors.at(it->result_name).vector_length,
                     (it->alpha));
                #endif
                    CHECK_CUDA(cudaDeviceSynchronize());
                break;
                case(UPDT):
                //call from host update device data
                if(sparse_vectors.find(it->argument_name)!=
                    sparse_vectors.end()){
                        //check host with tidx for update readiness
                        sparse_vectors.at(it->argument_name).
                                vector->update(tidx);
                        setSparseVectorData(sparse_vectors.
                                        at(it->argument_name));
                };
                break;
                case(COMM):
                if(dense_vectors.find(it->argument_name)!=
                    dense_vectors.end()){
                        #ifdef CUDA_AWARE_MPI
dense_vectors.at(it->argument_name.c_str()).vector->sendrecv_MPI(
    dense_vectors.at(it->argument_name.c_str()).vector_d);
                        #else
                            dense_vectors.
                                at(it->argument_name.c_str()).
                                    vector->sendrecv_MPI();
                        #endif
                };
                break;
                default:
                //pop error?
                break;
            };
        };
    };
};

void swSim::KernelComposerManager::
            getDenseVectorData(MappedDenseVector& vector){
    CHECK_CUDA(cudaSetDevice(device));
    if(deviceIsCommitted){
        if(vector.vector->getArrayLength()==
                    vector.vector_length){
            //cudaMemcpy(vector.vector->getValueArray(),vector.vector_d,vector.vector_length*sizeof(double),cudaMemcpyDeviceToHost);
            memcpy(vector.vector->getValueArray(),vector.vector_d,vector.vector_length*sizeof(double));
            //for(int k=0;k<vector.vector_length;k++){//printf("%f %f\n",vector.vector->getValueArray()[k],vector.vector_d[k]);};
        };
    };
};
void swSim::KernelComposerManager::
            getSparseVectorData(MappedSparseVector& vector){
    CHECK_CUDA(cudaSetDevice(device));
    if(deviceIsCommitted){
        if(vector.vector->getNonZeroLength()==
                    vector.compressed_length){
            //cudaMemcpy(vector.vector->getValueArray(),vector.vector_d,vector.compressed_length*sizeof(double),cudaMemcpyDeviceToHost);
            memcpy(vector.vector->getValueArray(),vector.vector_d,vector.compressed_length*sizeof(double));
        };
    };
};
void swSim::KernelComposerManager::
            getSparseMatrixData(MappedSparseMatrix& matrix){
    CHECK_CUDA(cudaSetDevice(device));
    if(deviceIsCommitted){
        if(matrix.matrix->getNonZeroLength()==
                    matrix.num_matrix_entries){
            //cudaMemcpy(matrix.matrix->getValueArray(),matrix.values_d,matrix.num_matrix_entries*sizeof(double));
            memcpy(matrix.matrix->getValueArray(),matrix.values_d,matrix.num_matrix_entries*sizeof(double));
        };
    };
};
void swSim::KernelComposerManager::
            setDenseVectorData(MappedDenseVector& vector){
    CHECK_CUDA(cudaSetDevice(device));
    if(deviceIsCommitted){
        if(vector.vector->getArrayLength()==
                    vector.vector_length){
            //cudaMemcpy(vector.vector_d,vector.vector->getValueArray(),vector.vector_length*sizeof(double),cudaMemcpyHostToDevice);
            memcpy(vector.vector_d,vector.vector->getValueArray(),vector.vector_length*sizeof(double));
        };
    };
};
void swSim::KernelComposerManager::
            setSparseVectorData(MappedSparseVector& vector){
    CHECK_CUDA(cudaSetDevice(device));
    if(deviceIsCommitted){
        if(vector.vector->getNonZeroLength()==
                    vector.compressed_length){
            //cudaMemcpy(vector.vector_d,vector.vector->getValueArray(),vector.compressed_length*sizeof(double),cudaMemcpyHostToDevice);
            memcpy(vector.vector_d,vector.vector->getValueArray(),vector.compressed_length*sizeof(double));
        };
    };
};
void swSim::KernelComposerManager::
            setSparseMatrixData(MappedSparseMatrix& matrix){
    CHECK_CUDA(cudaSetDevice(device));
    if(deviceIsCommitted){
        if(matrix.matrix->getNonZeroLength()==
                    matrix.num_matrix_entries){
            //cudaMemcpy(matrix.values_d,matrix.matrix->getValueArray(),matrix.num_matrix_entries*sizeof(double),cudaMemcpyHostToDevice);
            memcpy(matrix.values_d,matrix.matrix->getValueArray(),matrix.num_matrix_entries*sizeof(double));
        };
    };
};
