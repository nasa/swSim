// mpirun -np <n> -outfile-pattern=log%r.dat ./<binary name> 

#include <mpi.h>
#include <cstdio>

#include "gtest/gtest.h"

int main(int argc, char** argv){
    MPI_Init(&argc,&argv);
    ::testing::InitGoogleTest(&argc, argv);
    int result=RUN_ALL_TESTS();
    if(result!=0){printf("RUN_ALL_TESTS() nonzero exit code: %i\n",result);};
    MPI_Finalize();
    return 0;//result;
};
