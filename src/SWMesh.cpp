#include "SWMesh.hpp"

swSim::SWMesh::SWMesh(swSim::Model &lMod, swSim::Segmentation &seg, MPI_Comm l_mpi_comm) {

    std::vector<uint8_t> matlId;
    matlId.reserve(int(seg.processVoxels));

    MakeMesh(lMod, seg, l_mpi_comm, matlId);

    MakeMaterialIDField(matlId);
    MakeRotationsField(lMod, seg, l_mpi_comm);
    DefineCommunicationDatatypes(seg); 
    return; 
}

swSim::SWMesh::~SWMesh() {
}

void swSim::SWMesh::MakeMesh(swSim::Model &lMod, swSim::Segmentation &seg, MPI_Comm l_mpi_comm, std::vector<uint8_t> &matlId) {
    MPI_Status status;

    double dx=lMod.modGeometry.geomS.voxelSize[0];
    double dy=lMod.modGeometry.geomS.voxelSize[1];
    double dz=lMod.modGeometry.geomS.voxelSize[2];
    int Nx=int(lMod.modGeometry.geomS.dimensionsGlobalVoxel[0]); 
    int Ny=int(lMod.modGeometry.geomS.dimensionsGlobalVoxel[1]); 
    int Nz=int(lMod.modGeometry.geomS.dimensionsGlobalVoxel[2]); 

    int64_t Nxn=Nx+1;
    int64_t Nyn=Ny+1;
    int64_t Nzn=Nz+1;

    std::array<double,8> xboxrel({0.0,dx,dx,0.0,0.0,dx,dx,0.0});
    std::array<double,8> yboxrel({0.0,0.0,dy,dy,0.0,0.0,dy,dy});
    std::array<double,8> zboxrel({0.0,0.0,0.0,0.0,dz,dz,dz,dz});

    std::vector<uint8_t> slice;
    slice.resize(Ny*Nz);

    std::vector<std::array<double,3>> nodes;
    nodes.reserve(int(seg.processVoxels));
    std::vector<std::array<int32_t,8>> cells;
    cells.reserve(int(seg.processVoxels));
    std::map<int64_t,int32_t> node_map;

    swSim::MPIFileReader geometryFile(l_mpi_comm, lMod.modGeometry.GetGeometryFile(std::string(lMod.modS.inputPath)));
    for (int64_t kx = seg.xIndex0; kx<seg.xIndex0+seg.xDelta; kx++) 
    {
        MPI_File_read_at(geometryFile.fh, (kx)*uint64_t(Nz)*uint64_t(Ny), slice.data(), Ny*Nz, MPI_UINT8_T, &status);
        for(int64_t ky=0;ky<Ny;ky++){
            for(int64_t kz=0;kz<Nz;kz++){
                if(0<slice[kz+Nz*ky]){
                //compose cell
                    int64_t nidx=kz+Nzn*(ky+Nyn*kx);
                    std::array<int64_t,8> box ({
                    nidx,nidx+Nyn*Nzn,nidx+Nyn*Nzn+Nzn,nidx+Nzn,
                    nidx+1,nidx+Nyn*Nzn+1,nidx+Nyn*Nzn+Nzn+1,nidx+Nzn+1
                    });

                    matlId.push_back((double)slice[kz+Nz*ky]);
                    cells.push_back(std::array<int32_t,8>({0,0,0,0,0,0,0,0}));

                    for(int k=0;k<8;k++){
                        auto inserted_element=node_map.emplace(box.at(k),nodes.size());
                        if(true==inserted_element.second){
                            nodes.push_back(std::array<double,3>({kx*dx,ky*dy,kz*dz}));
                            nodes.back().at(0)+=xboxrel.at(k);
                            nodes.back().at(1)+=yboxrel.at(k);
                            nodes.back().at(2)+=zboxrel.at(k);
                        }
                        cells.back().at(k)=inserted_element.first->second;
                    }
                }
            }
        }
    }
    std::shared_ptr<HostMesh::HostMeshFactory> meshMfg=std::make_shared<HostMesh::HostMeshFactory>(HostMesh::HostMeshFactory());
    target->mesh=meshMfg->makeHexMesh(nodes,cells);
    // printf("Cell count: %i\nNode count: %i\nVerify node count: %i\n", cells.size(),   node_map.size(),nodes.size());
    return; 
}

void swSim::SWMesh::MakeMaterialIDField(std::vector<uint8_t> &matlId) {
    std::shared_ptr<Fields::HostFieldFactory> fieldMfg=std::make_shared<Fields::HostFieldFactory>(Fields::HostFieldFactory());
    materialID=fieldMfg->makeDenseCellField(target->mesh);
    for(int kc=0;kc<target->mesh->cellCount();kc++){
        materialID->at(kc)=matlId.at(kc);
    }
    return; 
}

void swSim::SWMesh::MakeRotationsField(swSim::Model &lMod, swSim::Segmentation &seg, MPI_Comm l_mpi_comm) {
    MPI_Status status;
    std::shared_ptr<Fields::HostFieldFactory> fieldMfg=std::make_shared<Fields::HostFieldFactory>(Fields::HostFieldFactory());
    for(int r=0; r<3; r++) {
        rotations.push_back(fieldMfg->makeDenseCellField(target->mesh));
    }
    std::vector<float> rotVec;
    rotVec.resize(target->mesh->cellCount()*3);
    swSim::MPIFileReader rotationsFile(l_mpi_comm, lMod.modGeometry.GetRotationsFile(std::string(lMod.modS.inputPath)));
    MPI_File_read_at(rotationsFile.fh, (seg.globalIndex0)*3*sizeof(float), rotVec.data(), target->mesh->cellCount()*3, MPI_FLOAT, &status);  

    for(int kc=0;kc<target->mesh->cellCount();kc++){
        for(int r=0; r<3; r++) {
            rotations.at(r)->at(kc) = rotVec.at(kc*3+r); //Right but the file is wrong
            //rotations.at(r)->at(kc) = 0; 
        }
    }
    return; 
}


void swSim::SWMesh::DefineCommunicationDatatypes(swSim::Segmentation &seg){
    std::pair <std::set<int32_t>::iterator,bool> ret; 
    std::set<int32_t> leftReceiveNodes; 
    std::set<int32_t> leftSendNodes;
    std::set<int32_t> rightSendNodes;
    std::set<int32_t> rightReceiveNodes;

    std::vector<int32_t> lrNodes;
    std::vector<int32_t> lsNodes;
    std::vector<int32_t> rsNodes;
    std::vector<int32_t> rrNodes;

    std::vector<int32_t> stencil(8,-1);

    int lrn = 0;
    int lsn = 0;
    int rrn = 0;
    int rsn = 0;

    std::vector<int> p1 = {0, 3, 4, 7};
    std::vector<int> p2 = {1, 2, 5, 6};


    for(int32_t Lcell_id = 0; Lcell_id < seg.lHaloVoxels; Lcell_id++) {
        target->mesh->cell(Lcell_id, stencil.data());
        for(std::vector<int>::iterator it = p1.begin(); it!=p1.end(); it++) {
            ret = leftReceiveNodes.emplace(stencil[*it]); 
            if(ret.second) {
                lrNodes.push_back(stencil[*it]);
                lrn++;
            }
        }
        for(std::vector<int>::iterator it = p2.begin(); it!=p2.end(); it++) {
            ret = leftSendNodes.emplace(stencil[*it]); 
            if(ret.second) {
                lsNodes.push_back(stencil[*it]);
                lsn++;
            }
        }
    }
    for(int32_t Rcell_id = target->mesh->cellCount()-seg.rHaloVoxels; Rcell_id < target->mesh->cellCount(); Rcell_id++) {
        target->mesh->cell(Rcell_id, stencil.data());
        for(std::vector<int>::iterator it = p1.begin(); it!=p1.end(); it++) {
            ret = rightSendNodes.emplace(stencil[*it]); 
            if(ret.second) {
                rsNodes.push_back(stencil[*it]);
                rsn++;
            }
        }
        for(std::vector<int>::iterator it = p2.begin(); it!=p2.end(); it++) {
            ret = rightReceiveNodes.emplace(stencil[*it]); 
            if(ret.second) {
                rrNodes.push_back(stencil[*it]);
                rrn++;
            }
        }
    }

    seg.MPICreateLeftReceive(lrNodes);
    seg.MPICreateLeftSend(lsNodes);
    seg.MPICreateRightSend(rsNodes);
    seg.MPICreateRightReceive(rrNodes);
    return; 
}
