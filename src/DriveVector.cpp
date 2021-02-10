#include "DriveVector.hpp"

swSim::DriveVector::DriveVector(int sz){
    uCoeffs.push_back(swSim::SparseVector("driveVector_coeff_X", sz)); 
    uCoeffs.push_back(swSim::SparseVector("driveVector_coeff_Y", sz)); 
    uCoeffs.push_back(swSim::SparseVector("driveVector_coeff_Z", sz)); 

    committed = false; 

}
swSim::DriveVector::~DriveVector(){}


bool swSim::DriveVector::CommitCoefficents(swSim::SparseMatrix *matrix_1, swSim::SparseMatrix *matrix_2){
    if(!committed)
    {
        for(int i=0; i<3; i++)
        {
            uCoeffs[i].vec = matrix_1->mat * matrix_2->mat *uCoeffs[i].vec; 
        }
    }
    committed = true; 
    return committed; 
}

