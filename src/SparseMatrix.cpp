
#include <string>
#include <Eigen/Sparse>

#include "SparseMatrix.hpp"


swSim::SparseMatrix::SparseMatrix(){};
swSim::SparseMatrix::SparseMatrix(std::string nm, int rw, int col){
    name = nm;
    mat.resize(rw,col);
};
swSim::SparseMatrix::~SparseMatrix(){}; 

void swSim::SparseMatrix::setName(std::string nm){name=nm;return;};
void swSim::SparseMatrix::setSize(int rw, int col){mat.resize(rw,col);return;};

std::string swSim::SparseMatrix::getName() const {return name;};
size_t swSim::SparseMatrix::getNonZeroLength() const {return mat.nonZeros();};
double* swSim::SparseMatrix::getValueArray() {return mat.valuePtr();};
int* swSim::SparseMatrix::getColumnIndexArray() {return mat.innerIndexPtr();};
int* swSim::SparseMatrix::getRowpointerArray() {return mat.outerIndexPtr();};
size_t swSim::SparseMatrix::getRowpointerLength() const {return mat.outerSize()+1;};
int swSim::SparseMatrix::getRowCount() const {return mat.rows();};
int swSim::SparseMatrix::getColumnCount() const {return mat.cols();};

void swSim::SparseMatrix::SetValue(int i, int j, double val)
{
    mat.coeffRef(i, j) = val;
    return; 
}

double swSim::SparseMatrix::GetValue(int i, int j){ return mat.coeffRef(i, j);};

void swSim::SparseMatrix::AddValue(int i, int j, double val)
{
    mat.coeffRef(i, j) += val;
    return; 
}
void swSim::SparseMatrix::Prune() 
{
    mat.prune(0.0);
    return;
}