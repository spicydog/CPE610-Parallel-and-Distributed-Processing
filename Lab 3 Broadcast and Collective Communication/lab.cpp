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
    
    double startTime1;
    double endTime1;
    double startTime2;
    double endTime2;
    double startTime3;
    double endTime3;
    
    
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&p);
	MPI_Comm_rank(MPI_COMM_WORLD,&r);
    
    FILE *pFile1;
    FILE *pFile2;
    
    int *indexs = (int*) malloc(4*sizeof(int));
    
    
    if (r==0) {
        pFile1 = fopen ("matA.txt","r");
        pFile2 = fopen ("matB.txt","r");
        
        
        fscanf(pFile1, "%d %d", &indexs[0], &indexs[1]);
        fscanf(pFile2, "%d %d", &indexs[2], &indexs[3]);
        
        startTime1 = MPI_Wtime();
    }
    
    MPI_Bcast(indexs, 4, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (r==0) {
        endTime1 = MPI_Wtime();
    }
    
    nR1 = indexs[0];
    nC1 = indexs[1];
    nR2 = indexs[2];
    nC2 = indexs[3];
    
    printf("%d: indexes received\n",r);
    
    float *A;
    float *B;
    float *C;
    
    int nA = nR1*nC1;
    int nB = nR2*nC2;
    int nC = nR1*nC2;
    
    A = (float*) malloc(nA*sizeof(float));
    B = (float*) malloc(nB*sizeof(float));
    
    
    if (r==0) {
        for (i=0; i<nA; i++) {
            fscanf(pFile1, "%f", &A[i]);
        }
        for (i=0; i<nB; i++) {
            fscanf(pFile2, "%f", &B[i]);
        }
        
        startTime2 = MPI_Wtime();
    }
    
    
    MPI_Bcast(A, nA, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, nB, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    if (r==0) {
        endTime2 = MPI_Wtime();
        startTime3 = MPI_Wtime();
    }
    
    
    printf("%d: data received\n",r);
    
    int d = nC/p+1; // Amount of data for this rank
    int first = r*d;     // First index for this rank
    int last = (r+1)*d;   // Last index+1 for this rank
    if(last>nC) {      // n must not be more than the lastest value
        last = nC;
        d = last-first;
    }
    
    if(r==0)
        C = (float*) malloc(nC*sizeof(float));
    else
        C = (float*) malloc(d*sizeof(float));
    

    int ij;
    
    for (i=0; i<nR1; i++) {
        for (j=0; j<nC2; j++) {
            ij = i*nC2+j;
            if(ij<first || ij>=last)
                continue;
            
            C[ij-first] = 0;
            for (k=0; k<nC1; k++) {
                C[ij-first] += A[i*nC1+k] * B[k*nC2+j];
            }
        }
        
    }
    
    
    int *rcounts;
    int *displs;
    if (r==0) {
        rcounts = (int*) malloc(p*sizeof(int));
        displs = (int*) malloc(p*sizeof(int));
        for (i=p-1; i>=0; i--) {
            d = nC/p+1;
            first = i*d;
            last = (i+1)*d;
            if(last>nC) {
                d = last-first;
            }
            
            rcounts[i] = d;
            displs[i] = first;
        }
    }
    
    printf("%d: completed\n",r);
    
    MPI_Gatherv(C, d, MPI_FLOAT, C, rcounts, displs, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    if (r==0) {
        
        endTime3 = MPI_Wtime();
        
        printf("%d: write file\n",r);
        FILE *rFile = fopen ("result.txt","w");
        fprintf(rFile, "%d %d\n", nR1, nC2);
        for (i=0; i<nR1; i++) {
            for (j=0; j<nC2; j++) {
                fprintf(rFile, "%.2f ",C[i*nC2+j]);
            }
            fprintf(rFile, "\n");
        }
        fclose(rFile);
        printf("%d: done\n",r);
        
        
        
        
        printf("%d: Index broadcast: %f\n",r,endTime1-startTime1);
        printf("%d: Data broadcast: %f\n",r,endTime2-startTime2);
        printf("%d: Result collected: %f\n",r,endTime3-startTime3);
        printf("%d: Total time: %f\n",r,endTime1-startTime1+endTime2-startTime2+endTime3-startTime3);
        
        free(rcounts);
        free(displs);
        
        fclose(pFile1);
        fclose(pFile2);
    }
    
    free(A);
    free(B);
    free(C);
    
	MPI_Finalize();
	return 0;
}
