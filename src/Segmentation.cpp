
#include "Segmentation.hpp"

swSim::Segmentation::Segmentation() 
{
    xIndex0 = 0;  // includes halo slices
    xDelta = 0; // includes halo slices
    neighborRight = MPI_PROC_NULL; 
    neighborLeft = MPI_PROC_NULL; 
    processVoxels = 0;  // includes halo voxels
    globalIndex0 = 0;  // includes halo voxels
    lHaloVoxels = 0;
    rHaloVoxels = 0; 
    return;   
}

swSim::Segmentation::Segmentation(swSim::Model &mod,  MPI_Comm l_mpi_comm) 
{
    xIndex0 = 0;  // includes halo slices
    xDelta = 0; // includes halo slices
    neighborRight = MPI_PROC_NULL; 
    neighborLeft = MPI_PROC_NULL; 
    processVoxels = 0;  // includes halo voxels
    globalIndex0 = 0;  // includes halo voxels
    lHaloVoxels = 0;
    rHaloVoxels = 0;

    int rank, size; 
    MPI_Comm_rank(l_mpi_comm, &rank);
    MPI_Comm_size(l_mpi_comm, &size);

    std::vector<uint64_t> planeCounts(mod.modGeometry.geomS.dimensionsGlobalVoxel[0], 0);
    swSim::MPIFileReader planeFile(l_mpi_comm, mod.modGeometry.GetPlaneCountFile(std::string(mod.modS.inputPath))); 
    MPI_Status status; 
    MPI_File_read_at(planeFile.fh, 0, &planeCounts[0], planeCounts.size(), MPI_UINT64_T, &status);
    LoadBalance(planeCounts, rank, size); 
    return;   
}

swSim::Segmentation::Segmentation(swSim::Segmentation &otherSeg) 
{
    xIndex0 = otherSeg.xIndex0;  // includes halo slices
    xDelta = otherSeg.xDelta; // includes halo slices
    neighborRight = otherSeg.neighborRight; 
    neighborLeft = otherSeg.neighborLeft; 
    processVoxels = otherSeg.processVoxels;  // includes halo voxels
    globalIndex0 = otherSeg.globalIndex0;  // includes halo voxels
    lHaloVoxels = otherSeg.lHaloVoxels;
    rHaloVoxels = otherSeg.rHaloVoxels;
    MPI_Type_dup(otherSeg.recieveLeftType, &recieveLeftType); 
    MPI_Type_dup(otherSeg.sendLeftType, &sendLeftType);
    MPI_Type_dup(otherSeg.recieveRightType, &recieveRightType); 
    MPI_Type_dup(otherSeg.sendRightType, &sendRightType);
    return;   
}

swSim::Segmentation::~Segmentation() {
}


void swSim::Segmentation::LoadBalance(std::vector<uint64_t> &planeCounts, int rank, int size) 
{
    uint64_t total_cells_remaining = SumVector(planeCounts); 
    uint64_t target_number_of_cells = 0 ; 
    int xpos = 0;
    uint64_t globalIndexVox=0; 

    for(int n = 0; n < size-1; n++) {
        target_number_of_cells = total_cells_remaining / (uint64_t)(size - n); 
        xpos = MeetTargetCells(planeCounts, xpos, globalIndexVox, target_number_of_cells);
        if(rank == n+1) {
            neighborLeft = rank - 1;
            SetLeftValues(planeCounts, xpos, globalIndexVox);
        }
        if(rank == n) {
            neighborRight = rank + 1;
            SetRightValues(planeCounts, xpos);
            return;  
        }
        total_cells_remaining = total_cells_remaining - processVoxels; 
    }
    SetRightValues(planeCounts.size(), total_cells_remaining); 
    return;
}


uint64_t swSim::Segmentation::SumVector(std::vector<uint64_t> &vec) 
{
    uint64_t total = 0;
    for(int i = 0; i < vec.size(); i++) {
        total = total + vec[i]; 
    }
    return total; 
}

int swSim::Segmentation::MeetTargetCells(std::vector<uint64_t> &planeCounts, int xpos, uint64_t &globalIndexVox, uint64_t target_number_of_cells) {
        processVoxels = 0;
        while(target_number_of_cells - processVoxels > planeCounts[xpos]) {
            processVoxels = processVoxels + planeCounts[xpos];
            globalIndexVox = globalIndexVox + planeCounts[xpos]; 
            xpos++;  
        }
        return xpos; 
}

void swSim::Segmentation::SetRightValues(std::vector<uint64_t> &planeCounts, int xpos) {
    xDelta = xpos - xIndex0 + 1; 
    rHaloVoxels = planeCounts[xpos];
    processVoxels = processVoxels + rHaloVoxels; 
    return; 
}

void swSim::Segmentation::SetRightValues(std::vector<uint64_t>::size_type planeCountsSize, uint64_t total_cells_remaining) {
    xDelta = planeCountsSize - xIndex0; 
    processVoxels = total_cells_remaining; 
    return; 
}

void swSim::Segmentation::SetLeftValues(std::vector<uint64_t> &planeCounts, int xpos, uint64_t globalIndexVox) {
    xIndex0 = xpos; 
    lHaloVoxels  = planeCounts[xpos];  
    globalIndex0 = globalIndexVox; 
    return;
}


void swSim::Segmentation::MPICreateLeftReceive(std::vector<int32_t> &lrNodes) {
    int lrn = lrNodes.size();
    std::vector<int> lrBlockLengths(lrn, 1);
    MPI_Type_indexed(lrn, lrBlockLengths.data(), lrNodes.data(), MPI_DOUBLE, &recieveLeftType); 
    MPI_Type_commit(&recieveLeftType); 
    return; 
}

void swSim::Segmentation::MPICreateLeftSend(std::vector<int32_t> &lsNodes) {
    int lsn = lsNodes.size();
    std::vector<int> lsBlockLengths(lsn, 1);
    MPI_Type_indexed(lsn, lsBlockLengths.data(), lsNodes.data(), MPI_DOUBLE, &sendLeftType); 
    MPI_Type_commit(&sendLeftType); 
    return; 
}

void swSim::Segmentation::MPICreateRightSend(std::vector<int32_t> &rsNodes) {
    int rsn = rsNodes.size();
    std::vector<int> rsBlockLengths(rsn, 1);
    MPI_Type_indexed(rsn, rsBlockLengths.data(), rsNodes.data(), MPI_DOUBLE, &sendRightType); 
    MPI_Type_commit(&sendRightType); 
    return; 
}

void swSim::Segmentation::MPICreateRightReceive(std::vector<int32_t> &rrNodes) {
    int rrn = rrNodes.size();
    std::vector<int> rrBlockLengths(rrn, 1);
    MPI_Type_indexed(rrn, rrBlockLengths.data(), rrNodes.data(), MPI_DOUBLE, &recieveRightType); 
    MPI_Type_commit(&recieveRightType); 
    return; 
}
