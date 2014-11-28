#include <stdio.h>
#include "cuda.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

int threshold = 30;

__global__ void kernel(int *img, int *result, int rows, int cols, int threshold)
{
    //int i = (blockIdx.x*blockDim.x)+threadIdx.x;

    int i = blockIdx.x+1;
    int j = threadIdx.x+1;
    int k,l;


    // int i,j,k,l;

    int t = threshold;
    int px,gx,gy,ox,oy;

    px = 0;
    gx = 0; gy = 0;
    ox = 0; oy = 0;

    for (k=i-1; k<=i+1; k++) {
        for (l=j-1; l<=j+1; l++) {
            ox = (l-j) * (k==i ? 2:1);
            oy = (i-k) * (l==j ? 2:1);

            gx += img[k*cols+l]*ox;
            gy += img[k*cols+l]*oy;
        }
    }

    px = gx<0 ? -gx:gx + gy<0 ? -gy:gy;
    px = px>t ? 255 : 0;
    result[(i-1)*(cols-2)+(j-1)] = px;


}




void cuda(int *img, int *result, int rows, int cols) {

    int i,j,k,l;

    int t = threshold;
    int px,gx,gy,ox,oy;

    for(i = 1; i < rows-1; i++)
    {
        for(j = 1; j < cols-1; j++)
        {
            px = 0;
            gx = 0; gy = 0;
            ox = 0; oy = 0;

            for (k=i-1; k<=i+1; k++) {
                for (l=j-1; l<=j+1; l++) {
                    ox = (l-j) * (k==i ? 2:1);
                    oy = (i-k) * (l==j ? 2:1);

                    gx += img[k*cols+l]*ox;
                    gy += img[k*cols+l]*oy;
                }
            }

            px = gx<0?-gx:gx + gy<0?-gy:gy;
            px = px>t ? 255 : 0;
            result[(i-1)*(cols-2)+(j-1)] = px;
        }
    }


}

int main() {

    FILE *pFile = fopen ("input.txt","r");

    int imgRow,imgCol;
    int i,j;

    fscanf(pFile, "%d %d", &imgRow, &imgCol);
    int *img = (int*) malloc(imgRow*imgCol*sizeof(int));
    for(i = 0; i < imgRow; i++)
    {
        for(j = 0; j < imgCol; j++)
        {
            fscanf(pFile, "%d", &img[i*imgCol+j]);
        }
        printf("read file:%d/%d\n",i+1,imgRow);
    }
    fclose(pFile);



    int *result = (int*) malloc(imgRow*imgCol*sizeof(int));

    int k,l;
    int *temp;


    int maxDataPerRow = 256;
    int maxDataPerCol = 256;


    // Splite work by rows
    int secRow = ( (imgRow+1) / (maxDataPerRow-2) ) ;
    int *secSizeRow  = (int*) malloc(secRow*sizeof(int));
    int *secDisplRow = (int*) malloc((secRow+1)*sizeof(int));
    l = 0; i = 0;
    while( l<imgRow ) {
        secDisplRow[i] = l;
        l += maxDataPerRow;
        if(l>=imgRow)
            break;
        secSizeRow[i] = l - secDisplRow[i];
        l -= 2; i++;
    }

    l = imgRow;
    secSizeRow[i] = l - secDisplRow[i];
    secDisplRow[i+1] = secDisplRow[i] + secSizeRow[i]-4;



    // Splite work by cols
    int secCol = ( (imgCol+1) / (maxDataPerCol-2) ) ;
    int *secSizeCol  = (int*) malloc(secCol*sizeof(int));
    int *secDisplCol = (int*) malloc((secCol+1)*sizeof(int));
    l = 0; i = 0;
    while( l<imgCol ) {
        secDisplCol[i] = l;
        l += maxDataPerCol;
        if(l>=imgCol)
            break;
        secSizeCol[i] = l - secDisplCol[i];
        l -= 2; i++;
    }

    l = imgCol;
    secSizeCol[i] = l - secDisplCol[i];
    secDisplCol[i+1] = secDisplCol[i] + secSizeCol[i]-4;


    for (i=0; i<=secRow; i++) {
        for (j=0; j<=secCol; j++) {

            int *array = (int*) malloc((secSizeRow[i]*secSizeCol[j])*sizeof(int));
            temp = &array[0];
            for(k=secDisplRow[i]; k<secDisplRow[i]+secSizeRow[i]; k++) {
                for(l=secDisplCol[j]; l<secDisplCol[j]+secSizeCol[j]; l++) {
                    *temp = img[k*imgCol+l];
                    temp++;
                }
            }


            int *array_dev;
            cudaMalloc((void**) &array_dev, (secSizeRow[i]*secSizeCol[j])*sizeof(int));

            cudaMemcpy(array_dev,array,(secSizeRow[i]*secSizeCol[j])*sizeof(int),cudaMemcpyHostToDevice);


            // Result

            int *cudaResult = (int*) malloc( (secSizeRow[i]-2) * (secSizeCol[j]-2)*sizeof(int) );

            int *cudaResult_dev;
            cudaMalloc((void**) &cudaResult_dev, (secSizeRow[i]-2) * (secSizeCol[j]-2)*sizeof(int));


            int threadNum = secSizeRow[i]-2;
            int blockNum  = secSizeCol[j]-2;


            kernel <<<blockNum,threadNum>>>(array_dev, cudaResult_dev, secSizeRow[i], secSizeCol[j], threshold);


            cudaMemcpy(cudaResult,cudaResult_dev,(secSizeRow[i]-2) * (secSizeCol[j]-2)*sizeof(int),cudaMemcpyDeviceToHost);



            temp = &cudaResult[0];
            for(k=secDisplRow[i]+1; k<secDisplRow[i]+secSizeRow[i]-1; k++) {
                for(l=secDisplCol[j]+1; l<secDisplCol[j]+secSizeCol[j]-1; l++) {
                    result[k*imgCol+l] = *temp;
                    temp++;
                }
            }


            free(cudaResult);
            free(array);

            printf("piece:%d,%d\n",i,j);

        }
    }

    free(img);

    pFile = fopen ("output.txt","w");
    fprintf(pFile, "%d %d\n", imgRow, imgCol);

    for(i = 0; i < imgRow; i++)
    {
        for(j = 0; j < imgCol; j++)
        {
            if(i==0 || i==imgRow-1 || j==0 || j==imgRow-1)
                result[i*imgCol+j] = 0;

            fprintf(pFile, "%d ", result[i*imgCol+j]);
            // if(result[i*imgCol+j]>0)
            //     printf("!");

        }
        fprintf(pFile, "\n");
        printf("write file:%d/%d\n",i+1,imgRow);
    }

    fclose(pFile);

    free(result);



    return 0;

}



