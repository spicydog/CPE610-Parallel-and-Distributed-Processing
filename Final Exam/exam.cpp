#include "mpi.h"
#include <stdio.h>
#include "math.h"
#include <stdlib.h>
#include <time.h>

// Instruction: count if the sulution in line #42 is more than 0 from random x and y; 0 <= x <= b and 0 <= y <= k. 
// Examination time is 1.5 hours without internet access. 

int main(int argc, char* argv[])
{
        MPI_Init(&argc, &argv);
   
        int i;
        int p,r;
        float N,b,k;
 
 
        MPI_Comm_size(MPI_COMM_WORLD,&p);
        MPI_Comm_rank(MPI_COMM_WORLD,&r);
 
        if(r==0) {
                printf("Please Enter (N,b,k) : ");
                scanf("%f,%f,%f",&N,&b,&k);
        }
 
        MPI_Bcast(&N, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&b, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&k, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
 
        float jobSize = (int)N/p;
        jobSize += N%p>r?1:0;
       
        float result;
        float x,y;
        float M = 0;
       
        srand(time(0)*(r+1));
        for(i=0;i<jobSize;i++) {
                x = ((float)rand()/(float)RAND_MAX)*b;
                y = ((float)rand()/(float)RAND_MAX)*k;
                result = abs(k*sin((x*3.14/180)))-abs(y);
                if(result>0) {
                        M++;
                }
        }
 
        if(r==0) {
                printf("ID: %d || TASKSIZE %.0f\n",0,jobSize);
                printf("ID: %d || FOUND %.0f\n",0,M);
                printf("\n");
 
                float totalM = M;
                float totalJob = jobSize;
                for(i=1;i<p;i++) {
 
                        MPI_Recv(&jobSize,1,MPI_FLOAT,i,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                        MPI_Recv(&M,1,MPI_FLOAT,i,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
 
                        printf("ID: %d || TASKSIZE %.0f\n",i,jobSize);
                        printf("ID: %d || FOUND %.0f\n",i,M);
                        printf("\n");
 
                        totalJob += jobSize;
                        totalM += M;
                }
 
               
                printf("\n");
                printf("Total M points: %.0f\n", totalM);
                printf("Total N points: %.0f\n", totalJob);
                printf("TRatio of M/N: %.2f\n", totalM/totalJob);
 
        } else {
                MPI_Send(&jobSize,1,MPI_FLOAT,0,0,MPI_COMM_WORLD);
                MPI_Send(&M,1,MPI_FLOAT,0,0,MPI_COMM_WORLD);
        }
 
 
        MPI_Finalize();
        return 0;
}