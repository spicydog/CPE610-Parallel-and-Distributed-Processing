#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>



void printMatrix(float *matrix, int nR, int nC);

int main(int argc, char* argv[])
{

    FILE *pFile;
    pFile = fopen ("heat.txt","r");
    int nLoop = 10000;


    int p,r;
    int i,j,k;
    int nRow,end,first;
    int n1;
    int n2;
    int nN;
    int nc;
    int nr;

    float *matrixA,*matrixB,*swap,*pA,*pB,*pC,*pD,*pM;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    MPI_Comm_rank(MPI_COMM_WORLD,&r);


    MPI_Request *requestList;
    requestList =(MPI_Request*)malloc((p*2)*sizeof(MPI_Request));

    fscanf(pFile, "%d %d", &n1, &n2);
    fclose(pFile);



    nr = (n1+1)/2;
    nc = (n2+1)/2;

    nN = nr*nc;
    matrixA = (float*) malloc(nN*sizeof(float));
    matrixB = (float*) malloc(nN*sizeof(float));

    // Generate matrix A
    swap = &matrixA[0];
    for (i=0; i<nr; i++) {
        for (j=0; j<nc; j++) {
            if (i==0 || j==0) {
                *swap = 255;
            } else {
                *swap = 0;
            }
            ++swap;
        }
    }

    // Generate matrix B
    for (i=0; i<nN; i++) {
        matrixB[i] = matrixA[i];
    }


    // Calculate worker size


    first = 0;
    for(i=0;i<p;i++) {
        nRow = (nr-1+i)/p;
        first += nRow;
        if(i==r) {
            first-=nRow;
            first++;
            end = first+nRow-1;
            break;
        }
    }


    // Collect data to rank 0
    int *rcounts;
    int *displs;
    int row;
    int sum = nc;
    rcounts = (int*) malloc(p*sizeof(int));
    displs = (int*) malloc(p*sizeof(int));
    for(i=0;i<p;i++) {
        row = (nr-1+i)/p;
        rcounts[i] = row*nc;
        displs[i] = sum;
        sum+=row*nc;
    }

    // Calculate
    double startTime;
    if(r==0) {
        startTime = MPI_Wtime();
    }



    for (k=0; k<nLoop; k++) {


        for (i=first; i<end; i++) {

            pM = &matrixB[(i)*nc+1];

            pA = &matrixA[(i)*nc];      // left
            pB = pA+2;                  // right
            pC = &matrixA[(i-1)*nc+1];  // top
            pD = &matrixA[(i+1)*nc+1];  // bottom


            for (j=1; j<nc-1; j++) {
                *(pM++) = ( *(pA++) + *(pB++) + *(pC++) + *(pD++) )/4;
            }

            pB=pA;
            if(n2%2==0)
                pB++;

            *(pM) = ( *(pA) + *(pB) + *(pC) + *(pD) )/4;
        }


        pM = &matrixB[(i)*nc+1];

        pA = &matrixA[(i)*nc];      // left
        pB = pA+2;                  // right
        pC = &matrixA[(i-1)*nc+1];  // top

        if(i==nr-1) {               // last row
            pD = pC;                // bottom
            if(n1%2==0)
                pD = pA+1;
        } else {
            pD = &matrixA[(i+1)*nc+1];  // bottom
        }

        for (j=1; j<nc-1; j++) {
            *(pM++) = ( *(pA++) + *(pB++) + *(pC++) + *(pD++) )/4;
        }

        pB=pA;
        if(n2%2==0)
            pB++;
        *(pM) = ( *(pA) + *(pB) + *(pC) + *(pD) )/4;



        swap = matrixA;
        matrixA = matrixB;
        matrixB = swap;

        if(r!=0)
            MPI_Isend(&matrixA[first*nc], nc, MPI_FLOAT, r-1, 1, MPI_COMM_WORLD, &requestList[r*2]); // Send first row to r-1
        if(r!=p-1)
            MPI_Isend(&matrixA[end*nc], nc, MPI_FLOAT, r+1, 2, MPI_COMM_WORLD, &requestList[r*2+1]); // Send last row to r+1

        if(r!=p-1)
            MPI_Recv(&matrixA[(end+1)*nc], nc, MPI_FLOAT, r+1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Get last+1 row from r+1
        if(r!=0)
            MPI_Recv(&matrixA[(first-1)*nc], nc, MPI_FLOAT, r-1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Get first-1 row from r-1


        if(r==0)
            if(k%100==0)
                printf("Loop: %d %fs\n",k,MPI_Wtime()-startTime);

    }

    free(matrixB);


    if(r==0) {
        for(i=1;i<p;i++) {
            MPI_Recv(&matrixA[displs[i]], rcounts[i], MPI_FLOAT, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        MPI_Send(&matrixA[displs[r]], rcounts[r], MPI_FLOAT, 0, r ,MPI_COMM_WORLD);
    }


    if(r==0) {
        printf("Done: %fs \n",MPI_Wtime()-startTime);
    }

    free(requestList);

    free(displs);
    free(rcounts);




    if(r==0) {
        printMatrix(matrixA,n1,n2);
    }

    free(matrixA);


    MPI_Finalize();
    return 0;
}



void printMatrix(float *matrix, int nR, int nC) {


    int nr = (nR+1)/2;
    int nc = (nC+1)/2;

    int i,j;


    FILE *rFile = fopen ("result.txt","w");
    fprintf(rFile, "%d %d\n", nR, nC);

    for (i=0; i<nr; i++) {
        for (j=0; j<nc; j++) {
            fprintf(rFile, "%.2f " ,matrix[i*nc+j]);
        }

        for (j=nC%2==0?nc-1:nc-2; j>=0; j--) {
            fprintf(rFile, "%.2f " ,matrix[i*nc+j]);
        }
        fprintf(rFile, "\n");
    }


    for (i=nR%2==0?nr-1:nr-2; i>=0; i--) {
        for (j=0; j<nc; j++) {
            fprintf(rFile, "%.2f " ,matrix[i*nc+j]);
        }

        for (j=nC%2==0?nc-1:nc-2; j>=0; j--) {
            fprintf(rFile, "%.2f " ,matrix[i*nc+j]);
        }
        fprintf(rFile, "\n");
    }
}
