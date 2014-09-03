#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// TODO: In the “Project Properties” under “Configuration Properties”,
// select the “Debugging” node and switch the “Debugger to launch:”
// combobox value to “MPI Cluster Debugger”.
int main(int argc, char* argv[])
{
	int p	=	0;
	int r	=	0;
	int i	=	0;
	int j	=	0;
    int k   =   0;
    int n   =   0;
	int nR1 =   0;
	int nC1 =   0;
	int nR2 =   0;
	int nC2 =   0;
    int nN1 =   0;
    int nN2 =   0;
    
    
    
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&p);
	MPI_Comm_rank(MPI_COMM_WORLD,&r);

//    BC
//    int msg = r;
//    MPI_Bcast(&msg, 1, MPI_INT, 0, MPI_COMM_WORLD);
//    if(r!=0)
//        printf("%d: %d\n",r,msg);
    
//    GT
//    int *rsg = (int*) malloc(p*sizeof(int));
//    int msg = r;
//    MPI_Gather(&msg, 1, MPI_INT, rsg, 1, MPI_INT, 0, MPI_COMM_WORLD);
//    if(r==0) {
//        for (int i=0; i<p; i++) {
//            printf("%d: %d\n",r,rsg[i]);
//        }
//    }

    
    
	MPI_Finalize();
	return 0;
}
