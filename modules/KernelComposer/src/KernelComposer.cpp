//

#include <vector>
#include <string>
#include <unordered_map>

//#include <cusparse.h>
//#include <cuda.h>
//#include <cublas_api.h>

#include "KernelComposer.hpp"
#include "KernelComposerManager.hpp"
#include "KernelComposerHelpers.hpp"

#include "SparseVector_base.hpp"
#include "SparseMatrix_base.hpp"
#include "DenseVector_base.hpp"

swSim::KernelComposer::~KernelComposer(){
    if(deviceIsCommitted){
        kernelDecommit();
    };
};

swSim::KernelComposerError_t swSim::KernelComposer::
                             setDenseVector(DenseVector_base* dense_vector){
    if(!deviceIsCommitted){
        MappedDenseVector mdnv;
        mdnv.vector=dense_vector;
        dense_vectors.emplace(dense_vector->getName(),mdnv);
        return KC_OK;
    };
    return WARN_COMMITTED;
};
swSim::KernelComposerError_t swSim::KernelComposer::
                             setSparseVector(SparseVector_base* sparse_vector){
    if(!deviceIsCommitted){
        if(0==sparse_vector->getNonZeroLength()){
            return WARN_INVALID_KEY;
        };
        MappedSparseVector mspv;
        mspv.vector=sparse_vector;
        sparse_vectors.emplace(sparse_vector->getName(),mspv);
        return KC_OK;
    };
    return WARN_COMMITTED;
};
swSim::KernelComposerError_t swSim::KernelComposer::
                             setSparseMatrix(SparseMatrix_base* sparse_matrix){
    if(!deviceIsCommitted){
        MappedSparseMatrix mspm;
        mspm.matrix=sparse_matrix;
        sparse_matrices.emplace(sparse_matrix->getName(),mspm);
        return KC_OK;
    };
    return WARN_COMMITTED;
};

swSim::KernelComposerError_t swSim::KernelComposer::
                             setSPMV(std::string matrix_name,
                                     std::string vector_x_name,
                                     std::string vector_y_name,
                                     double a,double b){
    if(!deviceIsCommitted){
        if(sparse_matrices.find(matrix_name.c_str())==sparse_matrices.end() ||
           dense_vectors.find(vector_x_name.c_str())==dense_vectors.end() ||
           dense_vectors.find(vector_y_name.c_str())==dense_vectors.end()){
            return WARN_INVALID_KEY;
        };
        Operation Op;
        Op.T=SPMV;
        Op.matrix_name.assign(matrix_name.c_str());
        Op.argument_name.assign(vector_x_name.c_str());
        Op.result_name.assign(vector_y_name.c_str());
        Op.alpha=a;
        Op.beta=b;
        operations.push_back(Op);
        return KC_OK;
    };
    return WARN_COMMITTED;
};
swSim::KernelComposerError_t swSim::KernelComposer::
                             setSPaXPY(std::string sparse_vector_x_name,
                                       std::string vector_y_name,std::vector<double>* scale){
    if(!deviceIsCommitted){
        if(dense_vectors.find(vector_y_name.c_str())==dense_vectors.end() ||
           sparse_vectors.find(
                sparse_vector_x_name.c_str())==sparse_vectors.end()){
            return WARN_INVALID_KEY;
        };
        Operation Op;
        Op.T=SXPY;
        Op.argument_name.assign(sparse_vector_x_name.c_str());
        Op.result_name.assign(vector_y_name.c_str());
        Op.scale=scale;
        operations.push_back(Op);
        return KC_OK;
    };
    return WARN_COMMITTED;
};
swSim::KernelComposerError_t swSim::KernelComposer::
                             setAXPY(std::string vector_x_name,
                                     std::string vector_y_name,double a){
    if(!deviceIsCommitted){
        if(dense_vectors.find(vector_x_name.c_str())==dense_vectors.end() ||
           dense_vectors.find(vector_y_name.c_str())==dense_vectors.end()){
            return WARN_INVALID_KEY;
        };
        Operation Op;
        Op.T=DXPY;
        Op.argument_name.assign(vector_x_name.c_str());
        Op.result_name.assign(vector_y_name.c_str());
        Op.alpha=a;
        operations.push_back(Op);
        return KC_OK;
    };
    return WARN_COMMITTED;
};
swSim::KernelComposerError_t swSim::KernelComposer::
                             setUpdateSparseVectorFromHost(
                                    std::string sparse_vector_name){
    if(!deviceIsCommitted){
        if(sparse_vectors.find(
                sparse_vector_name.c_str())==sparse_vectors.end()){
            return WARN_INVALID_KEY;
        };
        Operation Op;
        Op.T=UPDT;
        Op.argument_name.assign(sparse_vector_name.c_str());
        operations.push_back(Op);
        return KC_OK;
    };
    return WARN_COMMITTED;
};
swSim::KernelComposerError_t swSim::KernelComposer::
                             setMPIcomms(std::string vector_name){
    if(!deviceIsCommitted){
        if(dense_vectors.find(vector_name.c_str())==dense_vectors.end()){
            return WARN_INVALID_KEY;
        };
        Operation Op;
        Op.T=COMM;
        Op.argument_name.assign(vector_name.c_str());
        operations.push_back(Op);
        return KC_OK;
    };
    return WARN_COMMITTED;
};

swSim::KernelComposerError_t swSim::KernelComposer::
                             getDeviceData(std::string key_name){
    if(deviceIsCommitted){
        int cnt=0;
        if(dense_vectors.find(key_name.c_str())!=dense_vectors.end()){
            getDenseVectorData(dense_vectors.at(key_name.c_str()));
            cnt++;
        };
        if(sparse_vectors.find(key_name.c_str())!=sparse_vectors.end()){
            getSparseVectorData(sparse_vectors.at(key_name.c_str()));
            cnt++;
        };
        if(sparse_matrices.find(key_name.c_str())!=sparse_matrices.end()){
            getSparseMatrixData(sparse_matrices.at(key_name.c_str()));
            cnt++;
        };
        if(cnt==0){
            return WARN_INVALID_KEY;
        };
        return KC_OK;
    };
    return WARN_UNCOMMITTED;
};

swSim::KernelComposerError_t swSim::KernelComposer::
                             setDeviceData(std::string key_name){
    if(deviceIsCommitted){
        int cnt=0;
        if(dense_vectors.find(key_name.c_str())!=dense_vectors.end()){
            setDenseVectorData(dense_vectors.at(key_name.c_str()));
            cnt++;
        };
        if(sparse_vectors.find(key_name.c_str())!=sparse_vectors.end()){
            setSparseVectorData(sparse_vectors.at(key_name.c_str()));
            cnt++;
        };
        if(sparse_matrices.find(key_name.c_str())!=sparse_matrices.end()){
            setSparseMatrixData(sparse_matrices.at(key_name.c_str()));
            cnt++;
        };
        if(cnt==0){
            return WARN_INVALID_KEY;
        };
        return KC_OK;
    };
    return WARN_UNCOMMITTED;
};


bool swSim::KernelComposer::isCommitted(){return deviceIsCommitted;};

swSim::KernelComposerError_t swSim::KernelComposer::kernelCommit(){
    if(!deviceIsCommitted){
        commit();
        return KC_OK;
    };
    return WARN_COMMITTED;
};

swSim::KernelComposerError_t swSim::KernelComposer::kernelCommit(int device_id){
    if(!deviceIsCommitted){
        commit(device_id);
        return KC_OK;
    };
    return WARN_COMMITTED;
};

swSim::KernelComposerError_t swSim::KernelComposer::run(int num_steps){
    if(deviceIsCommitted){
        for(int kt=0;kt<num_steps;kt++){
            execute(kt);
        };
        return KC_OK;
    };
    return WARN_UNCOMMITTED;
};
swSim::KernelComposerError_t swSim::KernelComposer::
                run(int start_time_idx,int end_time_idx){
    if(deviceIsCommitted){
        for(int kt=start_time_idx;kt<end_time_idx;kt++){
            execute(kt);
        };
        return KC_OK;
    };
    return WARN_UNCOMMITTED;
};

swSim::KernelComposerError_t swSim::KernelComposer::kernelDecommit(){
    if(deviceIsCommitted){
        release();
        return KC_OK;
    };
    return WARN_UNCOMMITTED;
};