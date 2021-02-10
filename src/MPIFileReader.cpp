#include "MPIFileReader.hpp"

swSim::MPIFileReader::MPIFileReader(MPI_Comm aComm, std::string aFname)
{
    //memcpy(&fileName, afname, sizeof(char)*50); 
    fileName = aFname; 
    comm = aComm;

    int rank; 
    MPI_Comm_rank(comm, &rank);

    openError = MPI_File_open(comm, fileName.c_str(),  MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    if (openError)
    {
        printf("Rank %i: Error openning %s.\n",rank, fileName.c_str());
    }
    
}

swSim::MPIFileReader::~MPIFileReader() {
    MPI_File_close(&fh); 
}
