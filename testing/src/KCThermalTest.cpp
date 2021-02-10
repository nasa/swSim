//

#include <string>
#include <vector>
//#include <tuple>
#include <cstdio>

#include <Eigen/Sparse>

#include "SparseMatrix.hpp"
#include "SparseVector.hpp"
#include "DenseVector.hpp"
#include "KernelComposer.hpp"
#include "KernelComposerHelpers.hpp"

#include "gtest/gtest.h"

TEST(KCEndToEnd,Thermal){
    swSim::KernelComposer ThermalKernel;
    swSim::SparseMatrix DiffusionKernel;
    swSim::SparseVector SurfaceFlux;
    swSim::DenseVector TikTemp;
    swSim::DenseVector TocTemp;

    std::vector<double> HeatPulse;
    
    std::vector<double> TikTempStdVec;
    std::vector<double> TocTempStdVec;
    Eigen::SparseVector<double> SurfaceFluxEigVec;
    Eigen::SparseMatrix<double,1> DiffusionKernelEigMat;
    
    double Fo=0.125;
    int Nx,Ny,Nz,Nt,N,idx;
    Nx=100;Ny=100;Nz=100;Nt=10000;N=Nx*Ny*Nz;
    
    DiffusionKernelEigMat.resize(N,N);
    SurfaceFluxEigVec.resize(N);
    for(int kz=0;kz<Nz;kz++){
        for(int ky=0;ky<Ny;ky++){
            for(int kx=0;kx<Nx;kx++){
                idx=kx+Nx*(ky+Ny*kz);
                DiffusionKernelEigMat.coeffRef(idx,idx)=1.0;
                if(kx>0 && kx<(Nx-1) &&
                   ky>0 && ky<(Ny-1) &&
                   kz>0 && kz<(Nz-1)){
                    DiffusionKernelEigMat.coeffRef(idx,idx)-=6.0*Fo;
                    DiffusionKernelEigMat.coeffRef(idx,idx-1)=Fo;
                    DiffusionKernelEigMat.coeffRef(idx,idx+1)=Fo;
                    DiffusionKernelEigMat.coeffRef(idx,idx-Nx)=Fo;
                    DiffusionKernelEigMat.coeffRef(idx,idx+Nx)=Fo;
                    DiffusionKernelEigMat.coeffRef(idx,idx-(Nx*Ny))=Fo;
                    DiffusionKernelEigMat.coeffRef(idx,idx+(Nx*Ny))=Fo;
                };
                TikTempStdVec.push_back(0.0);
                TocTempStdVec.push_back(0.0);
            };
        };
    };
    DiffusionKernelEigMat.makeCompressed();
    for(int ky=(Ny*7/16);ky<(Ny*9/16);ky++){
        for(int kx=(Nx*7/16);kx<(Nx*9/16);kx++){
            SurfaceFluxEigVec.coeffRef(kx+Nx*ky)=1.0;
        };
    };
    for(int kt=0;kt<Nt/2;kt++){
        HeatPulse.push_back(1.0);
    };
    
    DiffusionKernel.mat=DiffusionKernelEigMat;
    TikTemp.setVector(TikTempStdVec);
    TocTemp.setVector(TocTempStdVec);
    SurfaceFlux.vec=SurfaceFluxEigVec;
    
    ThermalKernel.setSparseMatrix(&DiffusionKernel);
    ThermalKernel.setDenseVector(&TikTemp);
    ThermalKernel.setDenseVector(&TocTemp);
    ThermalKernel.setSparseVector(&SurfaceFlux);
    
    //Configure test, ensure configuration is correct
    ASSERT_FALSE(ThermalKernel.isCommitted());
    ASSERT_EQ(ThermalKernel.setSPMV(DiffusionKernel.getName(),
                TikTemp.getName(),TocTemp.getName(),1.0,0.0),swSim::KC_OK);
    ASSERT_EQ(ThermalKernel.setSPaXPY(SurfaceFlux.getName(),
                TocTemp.getName(),&HeatPulse),swSim::KC_OK);
    ASSERT_EQ(ThermalKernel.setSPMV(DiffusionKernel.getName(),
                TocTemp.getName(),TikTemp.getName(),1.0,0.0),swSim::KC_OK);
    ASSERT_EQ(ThermalKernel.setSPaXPY(SurfaceFlux.getName(),
                TikTemp.getName(),&HeatPulse),swSim::KC_OK);
    ASSERT_EQ(ThermalKernel.kernelCommit(),swSim::KC_OK);
    ASSERT_TRUE(ThermalKernel.isCommitted());
    ASSERT_EQ(ThermalKernel.run(Nt),swSim::KC_OK);
    ASSERT_EQ(ThermalKernel.getDeviceData(TikTemp.getName()),swSim::KC_OK);
    ASSERT_EQ(ThermalKernel.kernelDecommit(),swSim::KC_OK);
    ASSERT_FALSE(ThermalKernel.isCommitted());
    
    FILE* pfile;
    pfile=fopen("ThermalSim.dat","wb");
    ASSERT_NE(pfile,nullptr);
    fwrite(TikTemp.getValueArray(),sizeof(double),N,pfile);
    fclose(pfile);
};