#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// TODO: In the ?Project Properties? under ?Configuration Properties?,
// select the ?Debugging? node and switch the ?Debugger to launch:?
// combobox value to ?MPI Cluster Debugger?.
int main(int argc, char* argv[])
{
	int p	=	0;
	int r	=	0;
	int i	=	0;
	int j	=	0;
    int n   =   0;
	int nR = 0;
	int nC = 0;
    int nN = 0;



	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&p);
	MPI_Comm_rank(MPI_COMM_WORLD,&r);

    MPI_Request *requestList,requestNull;


    float avg = 0;
    int loopI;
    int loopN = 1;

    for (loopI=0; loopI<loopN; loopI++) {

        FILE *pFile1 = fopen ("Matrix_A_707x353F.txt","r");
        FILE *pFile2 = fopen ("Matrix_B_707x353F.txt","r");

        fscanf(pFile1, "%d %d", &nR, &nC);
        nN = nR*nC;

        fscanf(pFile2, "%d %d", &nR, &nC);
        nN = nR*nC;


        //    nN = 10
        //    p = 3
        //
        //    d = nN/p+1 = 4;
        //    r0 = r*d = 0, (r+1)*d-1 = 0124;
        //    r1 = r*d = 4, (r+1)*d-1 = 4567;
        //    r2 = r*d = 4, (r+1)*d-1>nN-1 ?  89;


        int d = nN/p+1; // Amount of data for this rank
        j = r*d;        // First index for this rank
        n = (r+1)*d;    // Last index+1 for this rank
        if(n>nN) {      // n must not be more than the lastest value
            n = nN;
            d = n-j;
        }
        double startTime1;
        double endTime1;
        double startTime2;
        double endTime2;


        float *sum;

        if (r>0) {
            sum = (float*) malloc(d*sizeof(float));
        } else {
            sum = (float*) malloc(nR*nC*sizeof(float));
            requestList =(MPI_Request*)malloc((p-1)*sizeof(MPI_Request));


            startTime1 = MPI_Wtime();

            for(i=1;i<p;i++) {

                int d = nN/p+1;
                j = i*d;        // First index for this rank
                n = (i+1)*d;    // Last index+1 for this rank
                if(n>nN) {
                    n = nN;     // n must not be more than the lastest value
                    d = n-j;
                }

                MPI_Request request;
                MPI_Status status;
                MPI_Irecv(&sum[j], d, MPI_FLOAT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &requestList[i-1]);

                printf("%d start recv buffer from %d\n",r,i);

            }

            endTime1 = MPI_Wtime();

        }

        float a,b;
        for(i=0;i<nN;i++) {
            fscanf(pFile1, "%f", &a);
            fscanf(pFile2, "%f", &b);
            if( i>=j && i<n ) {
                sum[i-j] = a+b;
            }
        }

        fclose(pFile1);
        fclose(pFile2);



        if (r>0) {
            printf("%d send result\n",r);
            MPI_Send(sum,d,(MPI_FLOAT),0, 0 ,MPI_COMM_WORLD);


        } else {
            MPI_Status  status;


            int flag;

            startTime2 = MPI_Wtime();
            for(i=1;i<p;i++) {
                MPI_Test(&requestList[i-1], &flag, &status);
                if(flag==0) {
                    printf("waiting for %d status %d\n",i,flag);
                    usleep(50);
                    MPI_Test(&requestList[i-1], &flag, &status);
                } else {
                    continue;
                }

                while(flag==0) {
                    usleep(50);
                    MPI_Test(&requestList[i-1], &flag, &status);
                }
            }
            endTime2 = MPI_Wtime();

            float time = endTime1-startTime1+endTime2-startTime2;
            avg += time;
            printf("%d communication finish in %f\n",r,time);


            FILE *rFile = fopen ("result.txt","w");
            fprintf(rFile, "%d %d", nR, nC);
            for (i=0; i<nR*nC; i++) {
                if(i%nC==0)
                    fprintf(rFile, "\n");
                fprintf(rFile, "%.2f ", sum[i]);
            }
            fclose(rFile);
        }

        free(*sum);

    }
    if (r==0) {
        avg/=loopN;
        printf("%d RUNs: %d\nAVERAGE TIME: %f\n",r,loopN,avg);
    }

	MPI_Finalize();
	return 0;
}
