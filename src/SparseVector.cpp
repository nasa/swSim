//

#include <string>
#include <Eigen/Sparse>

#include "SparseVector.hpp"

swSim::SparseVector::SparseVector(){};

swSim::SparseVector::SparseVector(std::string nm, int length){
    name = nm;
    vec.resize(length);
};

swSim::SparseVector::~SparseVector(){};

void swSim::SparseVector::setName(std::string nm){name=nm;return;};

std::string swSim::SparseVector::getName() const {return name;};

double* swSim::SparseVector::getValueArray() {return vec.valuePtr();};

int* swSim::SparseVector::getIndexArray() {return vec.innerIndexPtr();};

size_t swSim::SparseVector::getNonZeroLength() const {return vec.nonZeros();};

size_t swSim::SparseVector::getFullLength() const {return vec.size();};

void swSim::SparseVector::update(int kt){
    //example
    //for(int k=0;k<getNonZeroLength();k++){
    //    getValueArray()[k]+=(((double)(kt%2))-0.5);
    //};
    return;
};


