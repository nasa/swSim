#include "Stencil.hpp"
#include <iostream>
#include <stdexcept>


swSim::Stencil::Stencil(swSim::Model &model, swSim::SWMesh &localMesh, MPI_Comm l_mpi_comm, swSim::Segmentation &lSeg ) {
    neighbors.push_back({6,7,5,4,2,3,1,0});
    neighbors.push_back({6,5,7,4,2,1,3,0});
    neighbors.push_back({6,2,7,3,5,1,4,0});
    cOrder = {0, 5, 4, 5, 1, 3, 4, 3, 2}; 

    std::string matName; 
    for(int v =0; v<3; v++) 
    {
        for(int s =0; s<3; s++)
        {
            matName = "AS"+std::to_string(v)+std::to_string(s)+ "toV" + std::to_string(v); 
            AStoV.push_back(swSim::SparseMatrix(matName, localMesh.target->mesh->nodeCount(), localMesh.target->mesh->cellCount()));
            AStoV[v*3+s].mat.reserve(8*localMesh.target->mesh->cellCount());
        }
    }
    for(int s =0; s<6; s++) 
    {
        for(int v=0; v<3; v++)
        {
            matName = "AV"+std::to_string(v)+ "toS" + std::to_string(s); 
            AVtoS.push_back(swSim::SparseMatrix(matName, localMesh.target->mesh->cellCount(), localMesh.target->mesh->nodeCount()));
            AVtoS[s*3+v].mat.reserve(8*localMesh.target->mesh->nodeCount());
        }
    }
    AinvRhoBar.setName("inv_rho_bar");
    AinvRhoBar.setSize(localMesh.target->mesh->nodeCount(), localMesh.target->mesh->nodeCount()); 
    AinvRhoBar.mat.reserve(localMesh.target->mesh->nodeCount());
    AForceDispersion.setName("ForceDispersion");
    AForceDispersion.setSize(localMesh.target->mesh->nodeCount(), localMesh.target->mesh->cellCount()); 
    AForceDispersion.mat.reserve(localMesh.target->mesh->cellCount()*8);

    for (int exc = 0; exc < model.modS.numExcitations; exc++)
    {
        U.push_back(swSim::DriveVector(localMesh.target->mesh->cellCount()));
    }

    std::string vecName; 
    for(int v =0; v<3; v++) 
    {
        vecName = "V"+std::to_string(v); 
        V.push_back(swSim::DenseVector(vecName, localMesh.target->mesh->nodeCount()));
        V.back().mpiComm(l_mpi_comm);
        V.back().configureSegmentedComms(lSeg);
    }

    for(int s =0; s<6; s++) 
    {
        vecName = "S"+std::to_string(s); 
        S.push_back(swSim::DenseVector(vecName, localMesh.target->mesh->cellCount()));
    } 

    PopulateRhoInvMatrix(model, AinvRhoBar, localMesh);
    PopulateForceDispersionMatrix(AForceDispersion, localMesh);
    PopulateStressToVelocityMatrices(model, AStoV, localMesh);
    PopulateVelocityToStressMatrices(model, AVtoS, localMesh); 
    PlaceTransducers(model, U, localMesh);
    for (int exc = 0; exc < model.modS.numExcitations; exc++)
    {
        U[exc].CommitCoefficents(&AinvRhoBar, &AForceDispersion);
    }  
}

swSim::Stencil::~Stencil() {


}


void swSim::Stencil::PopulateRhoInvMatrix(swSim::Model &model, swSim::SparseMatrix &A, swSim::SWMesh &localMesh) {
    std::vector<int32_t> stencil(8,-1);
    double averageRho = 0.0;
    for (int32_t node_id = 0; node_id<localMesh.target->mesh->nodeCount(); node_id++) {
        localMesh.target->mesh->node(node_id, stencil.data());
        averageRho = 0.0;
        for(int i = 0; i<stencil.size(); i++) {
            if(stencil.at(i) > -1) {
                averageRho = averageRho + model.modMaterialsList.at(localMesh.materialID->at(stencil.at(i))-1).matS.density*0.125; 
            }                   
        } 
        A.SetValue(node_id, node_id, 1.0/averageRho);
    }
    return; 
}

void swSim::Stencil::PopulateForceDispersionMatrix(swSim::SparseMatrix &A, swSim::SWMesh &localMesh) {
    std::vector<int32_t> stencil(8,-1);

    for (int32_t node_id = 0; node_id<localMesh.target->mesh->nodeCount(); node_id++) {
        localMesh.target->mesh->node(node_id, stencil.data());
        for(int i = 0; i<stencil.size(); i++) {
            if(stencil.at(i) > -1) {
                A.SetValue(node_id, stencil.at(i), 0.125);
            }                   
        } 
    }
    return; 
}

void swSim::Stencil::PopulateStressToVelocityMatrices(swSim::Model &model, std::vector<swSim::SparseMatrix> &A, swSim::SWMesh &localMesh) {
    std::vector<int32_t> stencil(8,-1);
    std::vector<double> val(3,0.0); 
    for(int i=0; i<3; i++) {
        val[i] = 0.25*model.modS.dt/model.modGeometry.geomS.voxelSize[i];
    }

    for (int32_t node_id = 0; node_id<localMesh.target->mesh->nodeCount(); node_id++) {
        localMesh.target->mesh->node(node_id, stencil.data());
        double averageRho = 0.0;
        for(int i = 0; i<stencil.size(); i++) {
            if(stencil.at(i) > -1) {
                averageRho = averageRho + model.modMaterialsList.at(localMesh.materialID->at(stencil.at(i))-1).matS.density*0.125; 
            }                   
        } 
        for(int deltaAxis = 0; deltaAxis<3; deltaAxis++){
            for(int velocity = 0; velocity<3; velocity++) {
                Delta(stencil, node_id, A[velocity*3+deltaAxis], val[deltaAxis]/averageRho, deltaAxis);
            }
        }
    }
    return;
}

void swSim::Stencil::PopulateVelocityToStressMatrices(swSim::Model &model, std::vector<swSim::SparseMatrix> &A, swSim::SWMesh &localMesh) {
    std::vector<int32_t> stencil(8,-1);
    std::vector<std::vector<int>> neighbors; 
    float rotVec[3] = {0.0, 0.0, 0.0}; 
    float C[36];
    std::vector<double> val(3,0.0); 
    for(int i=0; i<3; i++) {
        val[i] = 0.25*model.modS.dt/model.modGeometry.geomS.voxelSize[i];
    }

    for (int32_t cell_id = 0; cell_id<localMesh.target->mesh->cellCount(); cell_id++) {
        localMesh.target->mesh->cell(cell_id, stencil.data());
        for (int rot = 0; rot < 3; rot++){
            rotVec[rot] = localMesh.rotations.at(rot)->at(cell_id); 
        }
        model.modMaterialsList[localMesh.materialID->at(cell_id)-1].RotateStiffnessMatrix(rotVec, &C[0]);
        for(int stress= 0; stress<6; stress++) {
            for(int velocity = 0; velocity<3; velocity++) {
                for(int deltaAxis = 0; deltaAxis<3; deltaAxis++) {
                    Delta(stencil, cell_id, A[stress*3+velocity], C[stress*6+cOrder[velocity*3+deltaAxis]]*val[deltaAxis], deltaAxis);
                }
            }
        }
    }
    return;
}


void swSim::Stencil::Delta(std::vector<int32_t> &stencil, uint64_t indx, swSim::SparseMatrix &A,  double val, int deltaAxis) {

    for(int i = 0; i<8; i++) {
        if(stencil.at(neighbors.at(deltaAxis).at(i)) > -1) {
             A.mat.coeffRef(indx, stencil.at(neighbors.at(deltaAxis).at(i))) += val;
        }
        val = -val;
    }
}


void swSim::Stencil::PlaceTransducers(swSim::Model &model, std::vector<swSim::DriveVector> &u, swSim::SWMesh &localMesh) {
    std::vector<double> pCoords(3,0.0);
    double zdist = model.modGeometry.geomS.voxelSize[2] / 2.0; 
    for(int tr = 0; tr < model.modS.numExcitations; tr++) {
        double radius = model.modExcitationsList[tr].excS.radius;
        std::vector<double> center(model.modExcitationsList[tr].excS.location, model.modExcitationsList[tr].excS.location + 3);
        
        for (int32_t cell_id = 0; cell_id<localMesh.target->mesh->cellCount(); cell_id++) {
            localMesh.target->mesh->cellCoordinate(cell_id, pCoords.data());
            if(LocationCheck(pCoords, center, radius, zdist))
            {   
                u[tr].uCoeffs.at(2).vec.coeffRef(cell_id) = model.modS.dt; 
            }
        }
    }
    return; 
}

bool swSim::Stencil::LocationCheck(std::vector<double> &pCoords, std::vector<double> center, double radius, double zdist)  {
    double sq_dist = pow(pCoords[0] - center[0], 2)+pow(pCoords[1] - center[1],2);
    if(sq_dist <= pow(radius,2)) {
        return((pow(pCoords[2]-center[2],2)<=pow(zdist,2))); 
    }
    return false;
}

void swSim::Stencil::Print(int rank) {

    for(int v =0; v<3; v++) 
        {
        for(int s =0; s<3; s++)
        {
            std::printf("Rank %d Transition Matrix %s, is of size %d \n", rank, AStoV[v*3+s].getName().c_str(), AStoV[v*3+s].getColumnCount()); 
            std::printf("Rank %d Transition Matrix %s, has %d non-zero elements. \n", rank, AStoV[v*3+s].getName().c_str(), AStoV[v*3+s].getNonZeroLength());
        }
    }
    for(int s =0; s<6; s++) 
    {
        for(int v=0; v<3; v++)
        {
            std::printf("Rank %d Transition Matrix %s, is of size %d \n", rank, AVtoS[s*3+v].getName().c_str(), AVtoS[s*3+v].getColumnCount());
            std::printf("Rank %d Transition Matrix %s, has %d non-zero elements. \n", rank, AVtoS[s*3+v].getName().c_str(), AVtoS[s*3+v].getNonZeroLength()); 
        }
    }

    std::printf("Rank %d Total number of Drive Vectors is %lu. \n", rank, U.size()); 
    for(int v =0; v<3; v++) 
    {
        std::printf("Rank %d State Vector %d, is of size %lu \n", rank, v, V[v].getArrayLength()); 
    }

    for(int s =0; s<6; s++) 
    {
        std::printf("Rank %d State Vector %d, is of size %lu \n", rank, s, S[s].getArrayLength()); 
    }

    return; 
}





// void Stencil(swSim::Model *mod, uint8_t *material1, uint8_t *material0, float *rotVec, uint64_t *block[1], uint64_t *block[0], int ky, int kz, std::vector<swSim::SparseMatrix> &A) 
// {

//     // AStoV[0] = S11>V1, S[0]>V[0]
//     // AStoV[1] = S12>V1, S[5]>V[0]
//     // AStoV[2] = S13>V1, S[4]>V[0]

//     // AStoV[3] = S12>V2, S[5]>V[1]
//     // AStoV[4] = S22>V2, S[1]>V[1]
//     // AStoV[5] = S23>V2, S[3]>V[1]

//     // AStoV[6] = S13>V3, S[3]>V[2]
//     // AStoV[7] = S23>V3, S[5]>V[2]
//     // AStoV[8] = S33>V3, S[2]>V[2]

//     // AVtoS[0]  = V1>S11, V[0]>S[0]
//     // AVtoS[1] = V2>S11, V[1]>S[0]
//     // AVtoS[2] = V3>S11, V[2]>S[0]

//     // AVtoS[3] = V1>S22, V[0]>S[1]
//     // AVtoS[4] = V2>S22, V[1]>S[1]
//     // AVtoS[5] = V3>S22, V[2]>S[1]

//     // AVtoS[6] = V1>S33, V[0]>S[2]
//     // AVtoS[7] = V2>S33, V[1]>S[2]
//     // AVtoS[8] = V3>S33, V[2]>S[2]

//     // AVtoS[9] = V1>S23, V[0]>S[3]
//     // AVtoS[10] = V2>S23, V[1]>S[3]
//     // AVtoS[11] = V3>S23, V[2]>S[3]

//     // AVtoS[12] = V1>S13, V[0]>S[4]
//     // AVtoS[13] = V2>S13, V[1]>S[4]
//     // AVtoS[14] = V3>S13, V[2]>S[4]

//     // AVtoS[15] = V1>S12, V[0]>S[5]
//     // AVtoS[16] = V2>S12, V[1]>S[5]
//     // AVtoS[17] = V3>S12, V[2]>S[5]


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