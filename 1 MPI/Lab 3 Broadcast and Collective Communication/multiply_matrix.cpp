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
    
    
    FILE *pFile1 = fopen ("matA.txt","r");
    FILE *pFile2 = fopen ("matB.txt","r");
    
    
    fscanf(pFile1, "%d %d", &nR1, &nC1);
    nN1 = nR1*nC1;
    
    fscanf(pFile2, "%d %d", &nR2, &nC2);
    nN2 = nR2*nC2;
    
    float *A;
    float *B;
    float *C;
    
    int nA = nR1*nC1;
    int nB = nR2*nC2;
    int nC = nR1*nC2;
    
    A = (float*) malloc(nA*sizeof(float));
    B = (float*) malloc(nB*sizeof(float));
    C = (float*) malloc(nC*sizeof(float));
    
    
    for (i=0; i<nA; i++) {
        fscanf(pFile1, "%f", &A[i]);
    }
    for (i=0; i<nB; i++) {
        fscanf(pFile2, "%f", &B[i]);
    }
    
    
    int d = nC/p+1; // Amount of data for this rank
    int first = r*d;     // First index for this rank
    int last = (r+1)*d;   // Last index+1 for this rank
    if(last>nC) {      // n must not be more than the lastest value
        last = nC;
        d = last-first;
    }
    printf("%d: %d %d %d %d\n",r,nC,first,last,d);
    
    int ij;
    
    for (i=0; i<nR1; i++) {
        for (j=0; j<nC2; j++) {
            ij = i*nC2+j;
            if(ij<first || ij>=last)
                continue;
            
            C[ij] = 0;
            for (k=0; k<nC1; k++) {
                C[ij] += A[i*nC1+k] * B[k*nC2+j];
            }
        }
        
    }
    
    if (r==2) {
        
        
        printf("write file\n");
        
        FILE *rFile = fopen ("result.txt","w");
        fprintf(rFile, "%d %d\n", nR1, nC2);
        for (i=0; i<nR1; i++) {
            for (j=0; j<nC2; j++) {
                fprintf(rFile, "%.2f ",C[i*nC2+j]);
            }
            fprintf(rFile, "\n");
        }
        
        fclose(rFile);
        
        printf("done\n");
        
    }
    
    
    free(A);
    free(B);
    free(C);
	MPI_Finalize();
	return 0;
}
