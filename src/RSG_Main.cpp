#include <mpi.h>  //MPI inclusion for parallelization
#include <iostream>
#include <cstdio>
#include <ctime>
#include "MPIData.hpp"
#include "Model.hpp"
#include "SWMesh.hpp"
#include "Segmentation.hpp"
#include "Stencil.hpp"
#include "KernelComposer.hpp"

#include "VTKPDataPresenter.hpp"
#include "Field.hpp"
#include "HostFieldFactory.hpp"

int main(int argc, char *argv[]){
    bool printout = true; 
    time_t tstart, tend, tstart0, tend0; 

    int rank, size;
    MPI_Comm mComm_world;
    MPI_Status status;
    MPI_Init(&argc, &argv); 

    swSim::MPIData mMPIData(MPI_COMM_WORLD);
    mComm_world = mMPIData.mComm;
    size = mMPIData.size;
    rank = mMPIData.rank; 

    swSim::Model mMod;
    char *infilename = argv[1];
    mMod.Input(infilename, mComm_world);

    tstart = time(0); 
    swSim::Segmentation mSeg(mMod, mComm_world);
    tend = time(0); 
    std::printf("Rank %d, Segmentation took %f second.\n", rank, difftime(tend, tstart));
    tstart = time(0); 
    swSim::SWMesh localMesh(mMod, mSeg, mComm_world);
    tend = time(0); 
    std::printf("Rank %d, Building Mesh took %f second.\n", rank, difftime(tend, tstart));
    swSim::Stencil mStencil(mMod, localMesh, mComm_world, mSeg); 

    MPI_Barrier(mComm_world);


    std::vector<std::vector<double>> waveForm;
    for (int tr = 0; tr < mMod.modS.numExcitations; tr ++)
    {
        std::vector<double> waveFormRead(mMod.modS.numTimeSteps, 0.0f);
        MPI_File drivefunc_fh;
        char fileName[50];
        sprintf(fileName, "%s%s", mMod.modS.inputPath, mMod.modExcitationsList[tr].excS.waveformFile); 
        int openError = MPI_File_open(MPI_COMM_WORLD, fileName,  MPI_MODE_RDONLY, MPI_INFO_NULL, &drivefunc_fh);
        if(openError) {
            std::printf("Rank %d, Error opening Waveform file.\n", rank);
        }
        MPI_File_read_at(drivefunc_fh, mMod.modExcitationsList[tr].excS.initTimeStep, waveFormRead.data(), mMod.modExcitationsList[tr].excS.excitationNumTimeSteps, MPI_DOUBLE, &status);
        MPI_File_close(&drivefunc_fh);
        waveForm.push_back( std::vector<double> (waveFormRead.begin(), waveFormRead.end()));
    }

    if(printout) {
        for (int r = 0; r<size; r++)
        {
            if(rank == r) {
                //mMod.Print(rank); 
                mStencil.Print(rank);
            }
            MPI_Barrier(mComm_world);
        }
    }

    std::printf("Rank %d: Write Geometry Inputs \n", rank); 
    std::shared_ptr<Controller::DataPresenter> geomwriter=std::make_shared<VTKIO::VTKPDataPresenter>(VTKIO::VTKPDataPresenter(mComm_world));
    std::shared_ptr<Fields::HostFieldFactory> geomfieldMfg=std::make_shared<Fields::HostFieldFactory>(Fields::HostFieldFactory());
    std::shared_ptr<ModelUT::Field> geomnodeOutput;
    geomnodeOutput = geomfieldMfg->makeDenseNodeField(localMesh.target->mesh);
    geomwriter->setInspectedTarget(localMesh.target);
    geomwriter->addOverlayedDataFieldToTarget(localMesh.materialID,"materialID");
    geomwriter->addOverlayedDataFieldToTarget(localMesh.rotations.at(0),"rot0");
    geomwriter->addOverlayedDataFieldToTarget(localMesh.rotations.at(1),"rot1");
    geomwriter->addOverlayedDataFieldToTarget(localMesh.rotations.at(2),"rot2");
    for(int kn=0;kn<localMesh.target->mesh->nodeCount();kn++){
        geomnodeOutput->at(kn)=mStencil.AinvRhoBar.GetValue(kn,kn);
    };
    geomwriter->addOverlayedDataFieldToTarget(geomnodeOutput, mStencil.AinvRhoBar.getName());
    for(int kn=0;kn<localMesh.target->mesh->nodeCount();kn++){
        geomnodeOutput->at(kn)=mStencil.U.at(0).uCoeffs.at(2).vec.coeffRef(kn);
    };
    geomwriter->addOverlayedDataFieldToTarget(geomnodeOutput, mStencil.U.at(0).uCoeffs.at(2).getName());
    geomwriter->commitToFile("GeomInputs.pvtu");


    std::printf("Rank %d: Start KC Construction \n", rank); 
    swSim::KernelComposer RSGKernel;
    for(int k=0;k<mStencil.V.size();k++){RSGKernel.setDenseVector(&mStencil.V.at(k));};
    for(int k=0;k<mStencil.S.size();k++){RSGKernel.setDenseVector(&mStencil.S.at(k));};
    
    for(int k=0;k<mStencil.AStoV.size();k++){RSGKernel.setSparseMatrix(&mStencil.AStoV.at(k));};
    for(int k=0;k<mStencil.AVtoS.size();k++){RSGKernel.setSparseMatrix(&mStencil.AVtoS.at(k));};
    for(int k=0;k<mStencil.U.size();k++){
        for(int k2=0;k2<3;k2++){
            RSGKernel.setSparseVector(&(mStencil.U.at(k).uCoeffs.at(k2)));
        };
    };

    std::printf("Rank %d: Start KC Construction 2 \n", rank); 

    RSGKernel.setSPMV(mStencil.AStoV.at(0).getName(),mStencil.S.at(0).getName(),mStencil.V.at(0).getName(),1.0,1.0); // S11 > V1
    RSGKernel.setSPMV(mStencil.AStoV.at(1).getName(),mStencil.S.at(5).getName(),mStencil.V.at(0).getName(),1.0,1.0); // S12 > V1
    RSGKernel.setSPMV(mStencil.AStoV.at(2).getName(),mStencil.S.at(4).getName(),mStencil.V.at(0).getName(),1.0,1.0); // S13 > V1
    
    RSGKernel.setSPMV(mStencil.AStoV.at(3).getName(),mStencil.S.at(5).getName(),mStencil.V.at(1).getName(),1.0,1.0); // S12 > V2
    RSGKernel.setSPMV(mStencil.AStoV.at(4).getName(),mStencil.S.at(1).getName(),mStencil.V.at(1).getName(),1.0,1.0); // S22 > V2
    RSGKernel.setSPMV(mStencil.AStoV.at(5).getName(),mStencil.S.at(3).getName(),mStencil.V.at(1).getName(),1.0,1.0); // S23 > V2
    
    RSGKernel.setSPMV(mStencil.AStoV.at(6).getName(),mStencil.S.at(4).getName(),mStencil.V.at(2).getName(),1.0,1.0); // S13 > V3
    RSGKernel.setSPMV(mStencil.AStoV.at(7).getName(),mStencil.S.at(3).getName(),mStencil.V.at(2).getName(),1.0,1.0); // S23 > V3
    RSGKernel.setSPMV(mStencil.AStoV.at(8).getName(),mStencil.S.at(2).getName(),mStencil.V.at(2).getName(),1.0,1.0); // S33 > V3

    std::printf("Rank %d: Start KC Construction 3 \n", rank); 
 
    RSGKernel.setSPaXPY(mStencil.U.at(0).uCoeffs.at(2).getName(),mStencil.V.at(2).getName(),&waveForm.at(0)); //

    std::printf("Rank %d: Start KC Construction 4 \n", rank); 
///*    
    RSGKernel.setMPIcomms(mStencil.V.at(0).getName());
    RSGKernel.setMPIcomms(mStencil.V.at(1).getName());
    RSGKernel.setMPIcomms(mStencil.V.at(2).getName());

    std::printf("Rank %d: Start KC Construction 5 \n", rank); 
//*/    
    RSGKernel.setSPMV(mStencil.AVtoS.at(0).getName(),mStencil.V.at(0).getName(),mStencil.S.at(0).getName(),1.0,1.0);  // V1 > S11
    RSGKernel.setSPMV(mStencil.AVtoS.at(1).getName(),mStencil.V.at(1).getName(),mStencil.S.at(0).getName(),1.0,1.0); // V2 > S11
    RSGKernel.setSPMV(mStencil.AVtoS.at(2).getName(),mStencil.V.at(2).getName(),mStencil.S.at(0).getName(),1.0,1.0); // V3 > S11
    RSGKernel.setSPMV(mStencil.AVtoS.at(3).getName(),mStencil.V.at(0).getName(),mStencil.S.at(1).getName(),1.0,1.0); // V1 > S22
    RSGKernel.setSPMV(mStencil.AVtoS.at(4).getName(),mStencil.V.at(1).getName(),mStencil.S.at(1).getName(),1.0,1.0); // V2 > S22
    RSGKernel.setSPMV(mStencil.AVtoS.at(5).getName(),mStencil.V.at(2).getName(),mStencil.S.at(1).getName(),1.0,1.0); // V3 > S22
    RSGKernel.setSPMV(mStencil.AVtoS.at(6).getName(),mStencil.V.at(0).getName(),mStencil.S.at(2).getName(),1.0,1.0); // V1 > S33
    RSGKernel.setSPMV(mStencil.AVtoS.at(7).getName(),mStencil.V.at(1).getName(),mStencil.S.at(2).getName(),1.0,1.0); // V2 > S33
    RSGKernel.setSPMV(mStencil.AVtoS.at(8).getName(),mStencil.V.at(2).getName(),mStencil.S.at(2).getName(),1.0,1.0); // V3 > S33
    RSGKernel.setSPMV(mStencil.AVtoS.at(9).getName(),mStencil.V.at(0).getName(),mStencil.S.at(3).getName(),1.0,1.0); // V1 > S23
    RSGKernel.setSPMV(mStencil.AVtoS.at(10).getName(),mStencil.V.at(1).getName(),mStencil.S.at(3).getName(),1.0,1.0); // V2 > S23
    RSGKernel.setSPMV(mStencil.AVtoS.at(11).getName(),mStencil.V.at(2).getName(),mStencil.S.at(3).getName(),1.0,1.0); // V3 > S23
    RSGKernel.setSPMV(mStencil.AVtoS.at(12).getName(),mStencil.V.at(0).getName(),mStencil.S.at(4).getName(),1.0,1.0); // V1 > S13
    RSGKernel.setSPMV(mStencil.AVtoS.at(13).getName(),mStencil.V.at(1).getName(),mStencil.S.at(4).getName(),1.0,1.0); // V2 > S13
    RSGKernel.setSPMV(mStencil.AVtoS.at(14).getName(),mStencil.V.at(2).getName(),mStencil.S.at(4).getName(),1.0,1.0); // V3 > S13
    RSGKernel.setSPMV(mStencil.AVtoS.at(15).getName(),mStencil.V.at(0).getName(),mStencil.S.at(5).getName(),1.0,1.0); // V1 > S12
    RSGKernel.setSPMV(mStencil.AVtoS.at(16).getName(),mStencil.V.at(1).getName(),mStencil.S.at(5).getName(),1.0,1.0); // V2 > S12
    RSGKernel.setSPMV(mStencil.AVtoS.at(17).getName(),mStencil.V.at(2).getName(),mStencil.S.at(5).getName(),1.0,1.0); // V3 > S12
//*/
    std::printf("Rank %d: Start KC Committ \n", rank); 
    RSGKernel.kernelCommit(rank);
    MPI_Barrier(mComm_world);

    char ofilename[50];
    std::shared_ptr<Controller::DataPresenter> writer=std::make_shared<VTKIO::VTKPDataPresenter>(VTKIO::VTKPDataPresenter(mComm_world));
    writer->setInspectedTarget(localMesh.target);
    std::shared_ptr<Fields::HostFieldFactory> fieldMfg=std::make_shared<Fields::HostFieldFactory>(Fields::HostFieldFactory());
    std::shared_ptr<ModelUT::Field> nodeOutput;
    std::shared_ptr<ModelUT::Field> cellOutput;
    nodeOutput = fieldMfg->makeDenseNodeField(localMesh.target->mesh);
    cellOutput = fieldMfg->makeDenseCellField(localMesh.target->mesh);


    for(int v=0; v<3; v++)
    {
        RSGKernel.getDeviceData(mStencil.V.at(v).getName());
        for(int kn=0;kn<localMesh.target->mesh->nodeCount();kn++){
            nodeOutput->at(kn)=mStencil.V.at(v).vec.at(kn);
            };
        writer->addOverlayedDataFieldToTarget(nodeOutput,mStencil.V.at(v).getName());
    }

    RSGKernel.getDeviceData(mStencil.U.at(0).uCoeffs.at(2).getName());
    for(int kn=0;kn<localMesh.target->mesh->nodeCount();kn++){
        nodeOutput->at(kn)=mStencil.U.at(0).uCoeffs.at(2).vec.coeffRef(kn);
        };
    writer->addOverlayedDataFieldToTarget(nodeOutput,mStencil.U.at(0).uCoeffs.at(2).getName());

    int time_step = 0;
    int NT = mMod.modS.numTimeSteps;
    int cycle_size = 50;


    std::printf("Rank %d: Writeout at t= %d \n", rank, time_step); 
    sprintf(ofilename, "Output_at_%d.pvtu",time_step);
    writer->commitToFile(ofilename);

    MPI_Barrier(mComm_world);
    std::printf("Rank %d  has %d degrees of freedom.\n", rank, localMesh.target->mesh->nodeCount()*3+localMesh.target->mesh->cellCount()*6);


    std::printf("Rank %d: Run \n", rank);  
    
    tstart0 = time(0);
    while(time_step < NT)
    {
        tstart = time(0);
        RSGKernel.run(time_step, time_step+cycle_size);
        time_step = time_step+cycle_size;
        MPI_Barrier(mComm_world);
        tend = time(0); 
        if(rank == 0)
        {
            std::printf("Ran %d time steps in %f seconds.\n", cycle_size, difftime(tend, tstart));
        }

        std::printf("Rank %d: Writeout at t= %d \n", rank, time_step); 
        for(int v=0; v<3; v++)
        {
            RSGKernel.getDeviceData(mStencil.V.at(v).getName());
            for(int kn=0;kn<localMesh.target->mesh->nodeCount();kn++){
                nodeOutput->at(kn)=mStencil.V.at(v).vec.at(kn);
                };
            writer->addOverlayedDataFieldToTarget(nodeOutput,mStencil.V.at(v).getName());
        }
        
        // RSGKernel.getDeviceData(mStencil.U.at(0).uCoeffs.at(2).getName());
        // for(int kn=0;kn<localMesh.target->mesh->nodeCount();kn++){
        //     nodeOutput->at(kn)=mStencil.U.at(0).uCoeffs.at(2).vec.coeffRef(kn);
        // };
        // writer->addOverlayedDataFieldToTarget(nodeOutput,mStencil.U.at(0).uCoeffs.at(2).getName());

        sprintf(ofilename, "Output_at_%d.pvtu",time_step);
        writer->commitToFile(ofilename);
    }
    tend0 = time(0);
    RSGKernel.kernelDecommit();
    std::printf("Rank %d done!\n", rank); 
    std::printf("Rank %d ran complete simulation in %f seconds.\n", rank, difftime(tend0, tstart0));
    MPI_Barrier(mComm_world);
    MPI_Finalize();
    return 0;
}
