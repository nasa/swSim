//

#include <string>
#include <cstdio>
#include <mpi.h>

#include "DenseVector.hpp"

#include "Segmentation.hpp"

swSim::DenseVector::DenseVector(){};
swSim::DenseVector::DenseVector(std::string nm, int length) 
{
    name = nm;
    vec.clear();
    vec.resize(length);
}

swSim::DenseVector::~DenseVector() {}

std::string swSim::DenseVector::getName() const { return name;}

double* swSim::DenseVector::getValueArray() { return vec.data();}

size_t swSim::DenseVector::getArrayLength() const { return vec.size();}

swSim::DenseVectorError_t swSim::DenseVector::sendrecv_MPI(){
    MPI_Status sStatus[2];
    MPI_Status rStatus[2];
    MPI_Request sRequest[2];
    MPI_Request rRequest[2];

    MPI_Isend(vec.data(), 1, seg->sendRightType, seg->neighborRight, 101, comm, &sRequest[1]);
    MPI_Irecv(vec.data(), 1, seg->recieveLeftType, seg->neighborLeft, 101, comm, &rRequest[0]);

    MPI_Isend(vec.data(), 1, seg->sendLeftType, seg->neighborLeft, 102, comm, &sRequest[0]);
    MPI_Irecv(vec.data(), 1, seg->recieveRightType, seg->neighborRight, 102, comm, &rRequest[1]);

    MPI_Wait(&rRequest[0],&rStatus[0]);
    MPI_Wait(&rRequest[1],&rStatus[0]);
    return DNV_OK;
};

swSim::DenseVectorError_t swSim::DenseVector::sendrecv_MPI(double* DnV_d){
    MPI_Status sStatus[2];
    MPI_Status rStatus[2];
    MPI_Request sRequest[2];
    MPI_Request rRequest[2];

    MPI_Isend(DnV_d, 1, seg->sendRightType, seg->neighborRight, 101, comm, &sRequest[1]);
    MPI_Irecv(DnV_d, 1, seg->recieveLeftType, seg->neighborLeft, 101, comm, &rRequest[0]);

    MPI_Isend(DnV_d, 1, seg->sendLeftType, seg->neighborLeft, 102, comm, &sRequest[0]);
    MPI_Irecv(DnV_d, 1, seg->recieveRightType, seg->neighborRight, 102, comm, &rRequest[1]);


    MPI_Wait(&rRequest[0],&rStatus[0]);
    MPI_Wait(&rRequest[1],&rStatus[0]);
    return DNV_OK;
};


void swSim::DenseVector::setName(std::string nm){
    name.assign(nm.c_str());
    return;
};
void swSim::DenseVector::setVector(std::vector<double> v){
    vec.clear();
    for(auto it=v.begin();it!=v.end();it++){
        vec.push_back(*it);
    };
    return;
};
void swSim::DenseVector::setVector(double* v,int length){
    vec.clear();
    for(int k=0;k<length;k++){
        vec.push_back(v[k]);
    };
    return;
};
void swSim::DenseVector::configureSegmentedComms(swSim::Segmentation & seg){
    this->seg = new swSim::Segmentation(seg); 
}
void swSim::DenseVector::mpiComm(MPI_Comm comm){
    this->comm = comm; 
}
