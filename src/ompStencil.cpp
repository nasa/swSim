#include "Stencil.hpp"
#include <iostream>
#include <stdexcept>
#include <omp.h>


void PopulateStressToVelocityOpperation(swSim::Model &model, std::vector<swSim::SparseMatrix> &A, swSim::SWMesh &localMesh)
{
    std::vector<int32_t> stencil(8,-1);
    std::vector<double> val(3,0.0); 
    for(int i=0; i<3; i++) {
        val[i] = 0.25*model.modS.dt/model.modGeometry.geomS.voxelSize[i];
    }
     // voxle size
    double averageRho;
    double A27_val; 

#pragma omp parallel for  shared(A, localMesh, model) private(averageRho, A27_val, stencil, val) schedule(static)
    for (int32_t node_id = 0; node_id<localMesh.target->mesh->nodeCount(); node_id++) {
        localMesh.target->mesh->node(node_id, stencil.data());
        averageRho = 0.0;
        A27_val = 0.0; 
        for(int i = 0; i<stencil.size(); i++) {
            if(stencil.at(i) > -1) {
                averageRho = averageRho + model.modMaterialsList.at(localMesh.materialID->at(stencil.at(i))-1).matS.density*0.125; 
                A27_val = A27_val + 0.125; 
            }                   
        }
        if (A27_val  > 0.9) {
            A27_val = 0.0;
        }
        if (A27_val > 0.0) {
            A27_val = A27_val/A27_val * 1.0/averageRho; 
            A[27].SetValue(node_id, node_id, A27_val);
        }
        for(int velocity = 0; velocity<3; velocity++) {
            PopulateStressToVelocityMatrices(A, val, averageRho, stencil, node_id, velocity);
        }
    }
    return;
}

void PopulateStressToVelocityMatrices(std::vector<swSim::SparseMatrix> &A, std::vector<double> &val, double rho, std::vector<int32_t> &lStencil, uint64_t vIndx, int vNum) {
    for(int deltaAxis = 0; deltaAxis<3; deltaAxis++){
        Delta(lStencil, vIndx, A[vNum*3+deltaAxis], val[deltaAxis]/rho, axis(deltaAxis));
    }
    return; 
}

void PopulateToVelocityToStressOpperation(swSim::Model &model, std::vector<swSim::SparseMatrix> &A, swSim::SWMesh &localMesh) {
    std::vector<int32_t> stencil(8,-1);
    float rotVec[3] = {0.0, 0.0, 0.0}; 
    float C[36];
    std::vector<double> val(3,0.0); 
    for(int i=0; i<3; i++) {
        val[i] = 0.25*model.modS.dt/model.modGeometry.geomS.voxelSize[i];
    }
#pragma omp parallel for private(rotVec, Stencil, C, val)
    for (int32_t cell_id = 0; cell_id<localMesh.target->mesh->cellCount(); cell_id++) {
        localMesh.target->mesh->cell(cell_id, stencil.data());
        for (int rot = 0; rot < 3; rot++){
            rotVec[rot] = localMesh.rotations.at(rot)->at(cell_id); 
        }
        model.modMaterialsList[localMesh.materialID->at(cell_id)-1].RotateStiffnessMatrix(rotVec, &C[0]);
        for(int stress= 0; stress<6; stress++) {
            PopulateVelocityToStressMatrices(A, val, &C[6*stress], stencil, cell_id, stress);
        }
    }
    return;
}

void PopulateVelocityToStressMatrices(std::vector<swSim::SparseMatrix> &A, std::vector<double> &val, float *C, std::vector<int32_t> &lVStencil, uint64_t sIndx, int sNum) {
    // V_1 to S_*
    Delta(lVStencil, sIndx, A[9+sNum*3], C[0]*val[0], axis(0));
    Delta(lVStencil, sIndx, A[9+sNum*3], C[5]*val[1], axis(1));
    Delta(lVStencil, sIndx, A[9+sNum*3], C[4]*val[2], axis(2));

    // V_2 to S_*
    Delta(lVStencil, sIndx, A[9+sNum*3+1], C[5]*val[0], axis(0));
    Delta(lVStencil, sIndx, A[9+sNum*3+1], C[1]*val[1], axis(1));
    Delta(lVStencil, sIndx, A[9+sNum*3+1], C[3]*val[2], axis(2));

    // V_3 to S_*
    Delta(lVStencil, sIndx, A[9+sNum*3+2], C[4]*val[0], axis(0));
    Delta(lVStencil, sIndx, A[9+sNum*3+2], C[3]*val[1], axis(1));
    Delta(lVStencil, sIndx, A[9+sNum*3+2], C[2]*val[2], axis(2));
    return; 
}

void Delta(std::vector<int32_t> &stencil, uint64_t indx, swSim::SparseMatrix &A,  double val, axis deltaAxis) {
    std::vector<int> neighbors; 
    if(deltaAxis == axis(0)) {
         neighbors.insert(neighbors.end(), {6,7,5,4,2,3,1,0}); 
    }
    if(deltaAxis == axis(1)) {
        neighbors.insert(neighbors.end(), {6,5,7,4,2,1,3,0}); 
    }
    if(deltaAxis == axis(2)) {
        neighbors.insert(neighbors.end(), {6,2,7,3,5,1,4,0}); 
    }
    for(int i = 0; i<neighbors.size(); i++) {
        if(stencil.at(neighbors.at(i)) > -1) {
            A.AddValue(indx, stencil.at(neighbors.at(i)), val);
        }
        val = -val; 
    }
}


void PlaceTransducers(swSim::Model &model, std::vector<swSim::DriveVector> &u, swSim::SWMesh &localMesh)
{
    std::vector<double> nodeCoords(3,0.0);
    for (int32_t node_id = 0; node_id<localMesh.target->mesh->nodeCount(); node_id++) {
        localMesh.target->mesh->nodeCoordinate(node_id, nodeCoords.data());
        for(int tr = 0; tr < model.modS.numExcitations; tr++) {
            if(model.modExcitationsList[tr].LocationCheck(int(nodeCoords.at(0)/model.modGeometry.geomS.voxelSize[0]), int(nodeCoords.at(1)/model.modGeometry.geomS.voxelSize[1]), int(nodeCoords.at(2)/model.modGeometry.geomS.voxelSize[2]) ))
            {   

                u[tr].uCoeffs.at(2).vec.coeffRef(node_id) = model.modS.dt; 
            }
        }
    }
    return; 
}






// void Stencil(swSim::Model *mod, uint8_t *material1, uint8_t *material0, float *rotVec, uint64_t *block[1], uint64_t *block[0], int ky, int kz, std::vector<swSim::SparseMatrix> &A) 
// {

//     //A[27] = TransducerF-V 

//     // A[0] = S11>V1, S[0]>V[0]
//     // A[1] = S12>V1, S[5]>V[0]
//     // A[2] = S13>V1, S[4]>V[0]

//     // A[3] = S12>V2, S[5]>V[1]
//     // A[4] = S22>V2, S[1]>V[1]
//     // A[5] = S23>V2, S[3]>V[1]

//     // A[6] = S13>V3, S[3]>V[2]
//     // A[7] = S23>V3, S[5]>V[2]
//     // A[8] = S33>V3, S[2]>V[2]

//     // A[9]  = V1>S11, V[0]>S[0]
//     // A[10] = V2>S11, V[1]>S[0]
//     // A[11] = V3>S11, V[2]>S[0]
//     // A[12] = V1>S22, V[0]>S[1]
//     // A[13] = V2>S22, V[1]>S[1]
//     // A[14] = V3>S22, V[2]>S[1]
//     // A[15] = V1>S33, V[0]>S[2]
//     // A[16] = V2>S33, V[1]>S[2]
//     // A[17] = V3>S33, V[2]>S[2]

//     // A[18] = V1>S23, V[0]>S[3]
//     // A[19] = V2>S23, V[1]>S[3]
//     // A[20] = V3>S23, V[2]>S[3]
//     // A[21] = V1>S13, V[0]>S[4]
//     // A[22] = V2>S13, V[1]>S[4]
//     // A[23] = V3>S13, V[2]>S[4]
//     // A[24] = V1>S12, V[0]>S[5]
//     // A[25] = V2>S12, V[1]>S[5]
//     // A[26] = V3>S12, V[2]>S[5]


//     return;
// }
        //15 (8) pt
        // Set past lStencil.el_lStencil.neighbors 
        // lStencil.el_stencil[idx][0] = block[0][ks-NY-1] // (x-1, y-1, z-1)
        // lStencil.el_stencil[idx][1] = block[0][ks-NY]   // (x-1, y-1, z)
        // lStencil.el_stencil[idx][2] = block[0][ks-1].   // (x-1, y,   z-1)
        // lStencil.el_stencil[idx][3] = block[0][ks].     // (x-1, y,   z)

        // lStencil.el_stencil[idx][4] = block[1][ks-NY-1] // (x,   y-1, z-1)
        // lStencil.el_stencil[idx][5] = block[1][ks-NY]   // (x,   y-1, z)
        // lStencil.el_stencil[idx][6] = block[1][ks-1]    // (x,   y,   z-1)
        // lStencil.el_stencil[idx][7] = block[1][ks].     // (x,   y,   z)

        //Set self as neighbor for past lStencil.el_lStencil.neighbors
        // lStencil.el_stencil[lStencil.el_stencil[idx][6]][8] = idx         // (x,   y,   z+1)
        // lStencil.el_stencil[lStencil.el_stencil[idx][5]][9] = idx         // (x,   y+1, z)
        // lStencil.el_stencil[lStencil.el_stencil[idx][4]][10] = idx        // (x,   y+1, z+1)
        // lStencil.el_stencil[lStencil.el_stencil[idx][3]][11] = idx        // (x+1, y,   z)
        // lStencil.el_stencil[lStencil.el_stencil[idx][2]][12] = idx        // (x+1, y,   z+1)
        // lStencil.el_stencil[lStencil.el_stencil[idx][1]][13] = idx        // (x+1, y+1, z) 
        // lStencil.el_stencil[lStencil.el_stencil[idx][0]][14] = idx        // (x+1, y+1, z+1)