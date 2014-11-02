#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int main(int argc, char* argv[])
{

    double G  = 6.67428E-11;
    double T  = 20;
    double halfTT = T*T/2;

    int i,j,k,index;
    double r;
    double vectorX;
    double vectorY;
    double aX;
    double aY;


    int nProcess,rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nProcess);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);




    int nL = 10000;
    int nN;
    double *swap;

    FILE *inputFile;
    FILE *outputFile;

    if( rank == 0 ) {
        inputFile = fopen ("input.txt","r");
        outputFile = fopen("output.txt","w");
        fscanf(inputFile, "%d", &nN);
    }
    MPI_Bcast(&nN, 1, MPI_INT, 0, MPI_COMM_WORLD);


    // Calculate work load
    int *rankNn = (int*) malloc(nProcess*sizeof(int));
    int *displs = (int*) malloc(nProcess*sizeof(int));
    int begin,end,sum,work;
    sum = 0;
    for (i = 0; i < nProcess; ++i) {
        int nWork = nN/nProcess + (nN%nProcess > i );
        rankNn[i] = nWork;
        if(i==rank) {
            work = nWork;
            begin = sum;
            end = sum+nWork-1;
        }
        displs[i] = sum;
        sum += nWork;
    }




    // Calculate
    double startTime;
    if(r==0) {
        startTime = MPI_Wtime();
    }

    // Initial variables
    double *posX    = (double*) malloc(nN*sizeof(double));
    double *posY    = (double*) malloc(nN*sizeof(double));
    int   *mass    = (int*)   malloc(nN*sizeof(int));

    double *newPosX = (double*) malloc(work*sizeof(double));
    double *newPosY = (double*) malloc(work*sizeof(double));
    double *speedX  = (double*) malloc(work*sizeof(double));
    double *speedY  = (double*) malloc(work*sizeof(double));
    double *accX    = (double*) malloc(work*sizeof(double));
    double *accY    = (double*) malloc(work*sizeof(double));

    for (i = 0; i < work; ++i) {
        speedX[i] = 0;
        speedY[i] = 0;
    }


    // Read file

    if( rank == 0 ) {
        fprintf(outputFile, "%d %d\n", nL, nN);
        for (i=0; i<nN; i++) {
            fscanf(inputFile, "%lf %lf %d", &posX[i], &posY[i], &mass[i]);
            if(rank==0)
                fprintf(outputFile, "%d ", mass[i]);
        }
        fclose(inputFile);
    }


    MPI_Bcast(posX, nN, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(posY, nN, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (i=0; i<nL; i++) {
        for (j=begin; j<=end; j++) {
            aX = 0;
            aY = 0;
            for (k=0; k<nN; k++) {
                if(j!=k) {
                    vectorX = posX[k]-posX[j];
                    vectorY = posY[k]-posY[j];

                    r = sqrt(vectorX*vectorX + vectorY*vectorY);
                    if(r!=0) {
                        aX += (mass[k]/r) * vectorX;
                        aY += (mass[k]/r) * vectorY;
                    }
                }
            }

            index = j-begin;

            accX[index] = aX * G;
            accY[index] = aY * G;

            speedX[index] = speedX[index] + accX[index] * T;
            speedY[index] = speedY[index] + accY[index] * T;

            newPosX[index] = posX[index] + speedX[index] * T + accX[index] * halfTT;
            newPosY[index] = posY[index] + speedY[index] * T + accY[index] * halfTT;

        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgatherv( newPosX, work, MPI_DOUBLE, posX, rankNn, displs, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Allgatherv( newPosY, work, MPI_DOUBLE, posY, rankNn, displs, MPI_DOUBLE, MPI_COMM_WORLD);


        if(rank==0) {
            fprintf(outputFile, "\n");
            for(j=0;j<nN;j++) {
                fprintf(outputFile, "%.2lf,%.2lf ",posX[j], posY[j]);
            }
            printf("time: %d\n",i);
        }



    }


    free(posX);
    free(posY);
    free(mass);

    free(newPosX);
    free(newPosY);
    free(speedX);
    free(speedY);
    free(accX);
    free(accY);

    if (rank==0) {
        fclose(outputFile);
    }

    MPI_Finalize();

    return 0;


}



