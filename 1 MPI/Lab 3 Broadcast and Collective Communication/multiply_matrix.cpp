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
	int nR1 = 0;
	int nC1 = 0;
	int nR2 = 0;
	int nC2 = 0;
    int nN1 = 0;
    int nN2 = 0;
    
    
    
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&p);
	MPI_Comm_rank(MPI_COMM_WORLD,&r);
    
    
    FILE *pFile1 = fopen ("matA.txt","r");
    FILE *pFile2 = fopen ("matB.txt","r");
    
    
    fscanf(pFile1, "%d %d", &nR1, &nC1);
    nN1 = nR1*nC1;
    
    fscanf(pFile2, "%d %d", &nR2, &nC2);
    nN2 = nR2*nC2;
    
    float **A;
    float **B;
    float **C;
    A = (float**) malloc(nR1*sizeof(float*));
    B = (float**) malloc(nR2*sizeof(float*));
    C = (float**) malloc(nR1*sizeof(float*));
    
    for (i=0; i<nR1; i++) {
        A[i] = (float*) malloc(nC1*sizeof(float));
        B[i] = (float*) malloc(nC2*sizeof(float));
        C[i] = (float*) malloc(nC2*sizeof(float));
        for (j=0; j<nC1; j++) {
            fscanf(pFile1, "%f", &A[i][j]);
        }
        for (j=0; j<nC2; j++) {
            fscanf(pFile2, "%f", &B[i][j]);
        }
    }
    
    for (i=0; i<nR1; i++) {
        for (j=0; j<nC1; j++) {
            fscanf(pFile1, "%f", &A[i][j]);
        }
        for (j=0; j<nC2; j++) {
            fscanf(pFile2, "%f", &B[i][j]);
        }
    }
    
    for (i=0; i<nR1; i++) {
        for (j=0; j<nC2; j++) {
            C[i][j] = 0;
            for (k=0; k<nC1; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
        printf("row: %d\n",i);

    }

    
    printf("write file\n");
    
    FILE *rFile = fopen ("result.txt","w");
    fprintf(rFile, "%d %d", nR1, nC2);
    for (i=0; i<nR1; i++) {
        for (j=0; j<nC2; j++) {
            fprintf(rFile, "%.2f ",C[i][j]);
        }
        fprintf(rFile, "\n");
    }
    
    fclose(rFile);
    
    printf("done\n");
    
    
    free(*A);
    free(*B);
    free(*C);
	MPI_Finalize();
	return 0;
}
