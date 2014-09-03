#include "mpi.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
// TODO: In the “Project Properties” under “Configuration Properties”,
// select the “Debugging” node and switch the “Debugger to launch:”
// combobox value to “MPI Cluster Debugger”.
int main(int argc, char* argv[])
{
    int i	=	0;
	int n	=	1000;
	int p	=	0;
	int id	=	0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&p);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);


	srand(time(NULL)+id*n);
	int r = rand()%n;

	printf("This is process ID:%d\nMy number is %d\n",id,r);
	

	if(id==0) {
		int rr;
		int winner = 0;
		int max = r;
		for(int i=1;i<p;i++) {
			MPI_Recv(&rr, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(rr>max) {
				max = rr;
				winner = i;
			}
		}
		printf("\nThe winner is %d with value %d\n",winner,max);
	} else {
		MPI_Send(&r,1,(MPI_INT),0, 0 ,MPI_COMM_WORLD);
	}
	MPI_Finalize();

	getch();

	return 0;
}