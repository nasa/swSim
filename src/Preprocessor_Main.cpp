#include <mpi.h>  //MPI inclusion for parallelization
#include <iostream>
#include <cstdio>
#include <ctime>
#include "MPIData.hpp"
#include "Model.hpp"
//#include "SWMesh.hpp"
//#include "Segmentation.hpp"
//#include "SparseMatrix.hpp"
//#include "DenseVector.hpp"
//#include "Stencil.hpp"
//#include "DriveVector.hpp"

//#include "VTKPDataPresenter.hpp"
//#include "Field.hpp"
//#include "HostFieldFactory.hpp"

int main(int argc, char *argv[]){
    bool printout = true; 
    time_t tstart, tend; 

    int rank, size;
    MPI_Comm mComm_world;
    MPI_Status status;
    MPI_Init(&argc, &argv); //initialize MPI
    
    swSim::MPIData mMPIData(MPI_COMM_WORLD);
    mComm_world = mMPIData.mComm;
    size = mMPIData.size;
    rank = mMPIData.rank; 

    swSim::Model mMod;

    char *infilename = argv[1];

    mMod.Input(infilename, mComm_world);

//     tstart = time(0); 
//     swSim::Segmentation mSeg(mMod, mComm_world);
//     tend = time(0); 
//     std::printf("Rank %d, Segmentation took %f second.\n", rank, difftime(tend, tstart));
//     tstart = time(0); 
//     swSim::SWMesh localMesh(mMod, mSeg, mComm_world);
//     tend = time(0); 
//     std::printf("Rank %d, Building Mesh took %f second.\n", rank, difftime(tend, tstart));

//     tstart = time(0); 
//     std::vector<swSim::SparseMatrix> A;
//     std::string matName; 
//     for(int v =0; v<3; v++) 
//     {
//         for(int s =0; s<3; s++)
//         {
//             matName = "AS"+std::to_string(v)+std::to_string(s)+ "toV" + std::to_string(v); 
//             A.push_back(swSim::SparseMatrix(matName, localMesh.target->mesh->nodeCount(), localMesh.target->mesh->cellCount()));
//             A[v*3+s].mat.reserve(8*localMesh.target->mesh->cellCount());
//         }
//     }
//     for(int s =0; s<6; s++) 
//     {
//         for(int v=0; v<3; v++)
//         {
//             matName = "AV"+std::to_string(v)+ "toS" + std::to_string(s); 
//             A.push_back(swSim::SparseMatrix(matName, localMesh.target->mesh->cellCount(), localMesh.target->mesh->nodeCount()));
//             A[9+s*3+v].mat.reserve(8*localMesh.target->mesh->nodeCount());
//         }
//     }
//     A.push_back(swSim::SparseMatrix("inv_rho_bar", localMesh.target->mesh->nodeCount(), localMesh.target->mesh->nodeCount()));
//     A[27].mat.reserve(localMesh.target->mesh->nodeCount());
//     A.push_back(swSim::SparseMatrix("ForceDispersion", localMesh.target->mesh->nodeCount(), localMesh.target->mesh->cellCount()));
//     A[28].mat.reserve(localMesh.target->mesh->cellCount()*8);

//     tend = time(0); 
//     std::printf("Rank %d, Creating Matrices took %f second.\n", rank, difftime(tend, tstart));

//     tstart = time(0); 
//     PopulateStressToVelocityOpperation(mMod, A, localMesh);
//     tend = time(0); 
//     std::printf("Rank %d, StoV stencil took %f second.\n", rank, difftime(tend, tstart));
//     tstart = time(0);
//     PopulateToVelocityToStressOpperation(mMod, A, localMesh);
//     tend = time(0); 
//     std::printf("Rank %d, VtoS stencil took %f second.\n", rank, difftime(tend, tstart));

//     MPI_Barrier(mComm_world);

//     tstart = time(0);
//     std::vector<swSim::DriveVector> U;
//     for (int exc = 0; exc < mMod.modS.numExcitations; exc++)
//     {
//         U.push_back(swSim::DriveVector(localMesh.target->mesh->cellCount()));
//     }
//     tend = time(0); 
//     std::printf("Rank %d, Creating DriveVector(s) took %f second.\n", rank, difftime(tend, tstart));


//     tstart = time(0); 
//     PlaceTransducers(mMod, U, localMesh);
//     for (int exc = 0; exc < mMod.modS.numExcitations; exc++)
//     {
//         U[exc].CommitCoefficents(&A[27], &A[28]);
//     }
//     tend = time(0); 
//     std::printf("Rank %d, Placing transducer took %f second.\n", rank, difftime(tend, tstart));

//     tstart = time(0);
//     std::vector<swSim::DenseVector> V;
//     std::string vecName; 
//     for(int v =0; v<3; v++) 
//     {
//         vecName = "V"+std::to_string(v); 
//         V.push_back(swSim::DenseVector(vecName, localMesh.target->mesh->nodeCount()));
//         V.back().mpiComm(mComm_world);
//         V.back().configureSegmentedComms(mSeg);
//     }
//     std::vector<swSim::DenseVector> S;
//     for(int s =0; s<6; s++) 
//     {
//         vecName = "S"+std::to_string(s); 
//         S.push_back(swSim::DenseVector(vecName, localMesh.target->mesh->cellCount()));
//     }
//     tend = time(0); 
//     std::printf("Rank %d, Creating State vectors took %f second.\n", rank, difftime(tend, tstart));


//     std::vector<std::vector<double>> waveForm;
//     for (int tr = 0; tr < mMod.modS.numExcitations; tr ++)
//     {
//         std::vector<double> waveFormRead(mMod.modS.numTimeSteps, 0.0f);
//         MPI_File drivefunc_fh;
//         char fileName[50];
//         sprintf(fileName, "%s%s", mMod.modS.inputPath, mMod.modExcitationsList[tr].excS.waveformFile); 
//         int openError = MPI_File_open(MPI_COMM_WORLD, fileName,  MPI_MODE_RDONLY, MPI_INFO_NULL, &drivefunc_fh);
//         if(openError) {
//             std::printf("Rank %d, Error opening Waveform file.\n", rank);
//         }
//         MPI_File_read_at(drivefunc_fh, mMod.modExcitationsList[tr].excS.initTimeStep, waveFormRead.data(), mMod.modExcitationsList[tr].excS.excitationNumTimeSteps, MPI_DOUBLE, &status);
//         MPI_File_close(&drivefunc_fh);
//         waveForm.push_back( std::vector<double> (waveFormRead.begin(), waveFormRead.end()));
//     }



    if(printout) {
        for (int i=0; i<size; i++) {
            if( rank == i) {
                std::printf("Rank %d, Time Steps %d.\n", rank, mMod.modS.numTimeSteps);
                std::printf("Rank %d, time step size %.10f.\n", rank, mMod.modS.dt);
                std::printf("Rank %d, Number of Materials %d.\n", rank, mMod.modS.numMaterials);
                std::printf("Rank %d, Number of Excitations %d.\n", rank, mMod.modS.numExcitations);
                
                for (int i = 0; i < 3; i++) {
                        std::printf("Rank %d, Voxel Dimension %d = %llu.\n", rank, i, mMod.modGeometry.geomS.dimensionsGlobalVoxel[i]); 
                }
                std::printf("Rank %d, Geometry is %llu x %llu x %llu voxels.\n", rank, mMod.modGeometry.geomS.dimensionsGlobalVoxel[0], mMod.modGeometry.geomS.dimensionsGlobalVoxel[1], mMod.modGeometry.geomS.dimensionsGlobalVoxel[2]);
                std::printf("Rank %d, Voxel Size %f x %f x %f mm.\n", rank, mMod.modGeometry.geomS.voxelSize[0]*1000, mMod.modGeometry.geomS.voxelSize[1]*1000, mMod.modGeometry.geomS.voxelSize[2]*1000 );
                std::printf("Rank %d, Input File Name %s.\n", rank, mMod.modS.inputFile);
                std::printf("Rank %d, Input File Path %s.\n", rank, mMod.modS.inputPath);
                std::printf("Rank %d, Geometry file %s.\n", rank, mMod.modGeometry.geomS.geometryFile);
                std::printf("Rank %d, Rotation file %s.\n", rank, mMod.modGeometry.geomS.rotationsFile);

                for (int mat_i = 0; mat_i<mMod.modS.numMaterials; mat_i++) {
                    std::printf("Rank %d, Material %d, Material ID %d.\n", rank, mat_i, mMod.modMaterialsList[mat_i].matS.materialID); 
                    std::printf("Rank %d, Material %d, Material density %f.\n", rank, mat_i, mMod.modMaterialsList[mat_i].matS.density); 
                    for (int i = 0; i < 21; i++) {
                        std::printf("Rank %d, Material %d, Material Stiffness Value %d = %f.\n", rank, mat_i, i, mMod.modMaterialsList[mat_i].matS.stiffnessValues[i]); 
                    }
                }

                for (int exc_i = 0; exc_i < mMod.modS.numExcitations; exc_i ++) {
                    std::printf("Rank %d, Excitation %d, Excitation ID %d.\n", rank, exc_i, mMod.modExcitationsList[exc_i].excS.excitationID); //
                    std::printf("Rank %d, Excitation %d, Excitation radius %f.\n", rank, exc_i, mMod.modExcitationsList[exc_i].excS.radius); //
                    std::printf("Rank %d, Excitation %d, Located at (%f, %f, %f).\n", rank, exc_i, mMod.modExcitationsList[exc_i].excS.location[0], mMod.modExcitationsList[exc_i].excS.location[1], mMod.modExcitationsList[exc_i].excS.location[2]);  
                }
                
                // for(int v =0; v<3; v++) 
                // {
                //     for(int s =0; s<3; s++)
                //     {
                //         std::printf("Rank %d Transition Matrix %s, is of size %d \n", rank, A[v*3+s].getName().c_str(), A[v*3+s].getColumnCount()); 
                //     }
                // }
                // for(int s =0; s<6; s++) 
                // {
                //     for(int v=0; v<3; v++)
                //     {
                //         std::printf("Rank %d Transition Matrix %s, is of size %d \n", rank, A[9+s*3+v].getName().c_str(), A[9+s*3+v].getColumnCount()); 
                //     }
                // }

                // for (int a = 0; a < A.size(); a++) {
                //     std::printf("Rank %d Transition Matrix %s, has %d non-zero elements. \n", rank, A[a].getName().c_str(), A[a].getNonZeroLength()); 
                // }

                // std::printf("Rank %d Total number of Drive Vectors is %lu. \n", rank, U.size()); 
                // for(int v =0; v<3; v++) 
                // {
                //     std::printf("Rank %d State Vector %d, is of size %lu \n", rank, v, V[v].getArrayLength()); 
                // }

                // for(int s =0; s<6; s++) 
                // {
                //     std::printf("Rank %d State Vector %d, is of size %lu \n", rank, s, S[s].getArrayLength()); 
                // }
            }
            MPI_Barrier(mComm_world); 
        }
    }

//     std::printf("Rank %d: Write Geometry Inputs \n", rank); 
//     std::shared_ptr<Controller::DataPresenter> geomwriter=std::make_shared<VTKIO::VTKPDataPresenter>(VTKIO::VTKPDataPresenter(mComm_world));
//     std::shared_ptr<Fields::HostFieldFactory> geomfieldMfg=std::make_shared<Fields::HostFieldFactory>(Fields::HostFieldFactory());
//     std::shared_ptr<ModelUT::Field> geomnodeOutput;
//     geomnodeOutput = geomfieldMfg->makeDenseNodeField(localMesh.target->mesh);
//     geomwriter->setInspectedTarget(localMesh.target);
//     geomwriter->addOverlayedDataFieldToTarget(localMesh.materialID,"materialID");
//     geomwriter->addOverlayedDataFieldToTarget(localMesh.rotations.at(0),"rot0");
//     geomwriter->addOverlayedDataFieldToTarget(localMesh.rotations.at(1),"rot1");
//     geomwriter->addOverlayedDataFieldToTarget(localMesh.rotations.at(2),"rot2");
//     for(int kn=0;kn<localMesh.target->mesh->nodeCount();kn++){
//         geomnodeOutput->at(kn)=A.at(27).GetValue(kn,kn);
//     };
//     geomwriter->addOverlayedDataFieldToTarget(geomnodeOutput, A.at(27).getName());
//     for(int kn=0;kn<localMesh.target->mesh->nodeCount();kn++){
//         geomnodeOutput->at(kn)=U.at(0).uCoeffs.at(2).vec.coeffRef(kn);
//     };
//     geomwriter->addOverlayedDataFieldToTarget(geomnodeOutput, U.at(0).uCoeffs.at(2).getName());
//     geomwriter->commitToFile("GeomInputs.pvtu");

    std::printf("Rank %d done!\n", rank); 
    MPI_Barrier(mComm_world);
    MPI_Finalize();
    return 0;
}
