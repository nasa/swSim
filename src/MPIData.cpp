
#include "MPIData.hpp"

swSim::MPIData::MPIData() {
    mComm = Dup_Comm_World(&rank, &size);
}

swSim::MPIData::MPIData(MPI_Comm l_mpi_comm) {
    MPI_Comm_rank(l_mpi_comm, &rank);
    MPI_Comm_size(l_mpi_comm, &size);
    MPI_Comm_dup(l_mpi_comm, &mComm);
}


swSim::MPIData::~MPIData() {
}

MPI_Comm swSim::MPIData::Dup_Comm_World(int *mRank, int *mSize) {
    MPI_Comm mComm_world;
    MPI_Comm_rank(MPI_COMM_WORLD, mRank);
    MPI_Comm_size(MPI_COMM_WORLD, mSize);
    MPI_Comm_dup(MPI_COMM_WORLD, &mComm_world);
    return mComm_world;
}
