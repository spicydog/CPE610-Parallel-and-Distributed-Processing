// Example showing how to read and write images
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.h>
#include <math.h>

int threshold = 30;

void generateInputFile() {
    
    cv::Mat img = cvLoadImage("/Users/spicydog/Desktop/opencv/original.jpg", 1);
    uint8_t* pixelPtr = (uint8_t*)img.data;
    int cn = img.channels();
    int avg;
    cv::Scalar_<uint8_t> bgrPixel;
    
    
    FILE *pFile = fopen ("/Users/spicydog/Desktop/opencv/input.txt","w");
    fprintf(pFile, "%d %d\n", img.rows, img.cols);
    
    for(int i = 0; i < img.rows; i++)
    {
        for(int j = 0; j < img.cols; j++)
        {
            bgrPixel.val[0] = pixelPtr[i*img.cols*cn + j*cn + 0]; // B
            bgrPixel.val[1] = pixelPtr[i*img.cols*cn + j*cn + 1]; // G
            bgrPixel.val[2] = pixelPtr[i*img.cols*cn + j*cn + 2]; // R
            
            avg = (int)(bgrPixel.val[0] + bgrPixel.val[1] + bgrPixel.val[2])/3;
            
            fprintf(pFile, "%d ", avg);
            
            
            pixelPtr[i*img.cols*cn + j*cn + 0] = avg;
            pixelPtr[i*img.cols*cn + j*cn + 1] = avg;
            pixelPtr[i*img.cols*cn + j*cn + 2] = avg;
            
        }
        fprintf(pFile, "\n");
        printf("%d/%d\n",i+1,img.rows);
    }
    
    fclose(pFile);
    
    IplImage copy = img;
    IplImage* new_image = &copy;
    cvSaveImage("/Users/spicydog/Desktop/opencv/input.jpg", new_image);
    
    
    img.release();
    cvReleaseImage(&new_image);
    
    
}

void detectEdge() {
    
    
    FILE *pFile = fopen ("/Users/spicydog/Desktop/opencv/input.txt","r");
    
    int rows,cols;
    fscanf(pFile, "%d %d", &rows, &cols);
    
    int *img = (int*) malloc(rows*cols*sizeof(int));
    int *result = (int*) malloc(rows*cols*sizeof(int));
    
    
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            fscanf(pFile, "%d", &img[i*cols+j]);
        }
        printf("1: %d/%d\n",i+1,rows);
    }
    fclose(pFile);
    
    int t = threshold;
    
    
    for(int i = 1; i < rows-1; i++)
    {
        for(int j = 1; j < cols-1; j++)
        {
            int px = 0;
            int gx = 0;
            int gy = 0;
            int ox = 0;
            int oy = 0;
            
            for (int k=i-1; k<=i+1; k++) {
                for (int l=j-1; l<=j+1; l++) {
                    ox = (l-j) * (k==i ? 2:1);
                    oy = (i-k) * (l==j ? 2:1);
                    
                    gx += img[k*cols+l]*ox;
                    gy += img[k*cols+l]*oy;
                }
            }
            
            
            px = gx<0?-gx:gx + gy<0?-gy:gy;
            
            px = px>t ? 255 : 0;
            
            
            result[i*cols+j] = px;
        }
        printf("2: %d/%d\n",i+1,rows);
    }
    
    
    pFile = fopen ("/Users/spicydog/Desktop/opencv/output.txt","w");
    fprintf(pFile, "%d %d\n", rows, cols);
    
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            if(i==0 || i==rows-1 || j==0 || j==rows-1)
                result[i*cols+j] = 0;
            
            fprintf(pFile, "%d ", result[i*cols+j]);
        }
        fprintf(pFile, "\n");
        printf("3: %d/%d\n",i+1,rows);
    }
    
    fclose(pFile);
    
}


void writeOutputImage() {
    
    
    FILE *pFile = fopen ("/Users/spicydog/Desktop/opencv/output.txt","r");
    
    int rows,cols,avg;
    fscanf(pFile, "%d %d", &rows, &cols);
    
    
    // cv::Mat img = cv::Mat::zeros(rows, cols, CV_8U);
    cv::Mat img = cvLoadImage("/Users/spicydog/Desktop/opencv/original.jpg", 1);
    
    
    uint8_t* pixelPtr = (uint8_t*)img.data;
    int cn = img.channels();
    cv::Scalar_<uint8_t> bgrPixel;
    
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            fscanf(pFile, "%d", &avg);
            
            pixelPtr[i*img.cols*cn + j*cn + 0] = avg; // B
            pixelPtr[i*img.cols*cn + j*cn + 1] = avg; // G
            pixelPtr[i*img.cols*cn + j*cn + 2] = avg; // R
            
        }
        printf("%d/%d\n",i+1,img.rows);
    }
    
    fclose(pFile);
    
    IplImage copy = img;
    IplImage* new_image = &copy;
    cvSaveImage("/Users/spicydog/Desktop/opencv/output.jpg", new_image);
    
    img.release();
    cvReleaseImage(&new_image);
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

void spliteWork(int *img, int imgRow, int imgCol) {

    int *result = (int*) malloc(imgRow*imgCol*sizeof(int));
    
    int i,j,k,l;
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
            
            
            int *array = (int*) malloc(( secSizeRow[i]*secSizeCol[j] )*sizeof(int));
            
            temp = &array[0];
            for(k=secDisplRow[i]; k<secDisplRow[i]+secSizeRow[i]; k++) {
                for(l=secDisplCol[j]; l<secDisplCol[j]+secSizeCol[j]; l++) {
                    *temp = img[k*imgCol+l];
                    temp++;
                }
            }
            
            
            int *cudaResult = (int*) malloc( (secSizeRow[i]-2) * (secSizeCol[j]-2) * sizeof(int) );
            
            cuda(array,cudaResult,secSizeRow[i],secSizeCol[j]);
            
            
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
    
    FILE *pFile = fopen ("/Users/spicydog/Desktop/opencv/output.txt","w");
    fprintf(pFile, "%d %d\n", imgRow, imgCol);
    
    for(i = 0; i < imgRow; i++)
    {
        for(j = 0; j < imgCol; j++)
        {
            if(i==0 || i==imgRow-1 || j==0 || j==imgRow-1)
                result[i*imgCol+j] = 0;
            
            fprintf(pFile, "%d ", result[i*imgCol+j]);
        }
        fprintf(pFile, "\n");
    }
    
    fclose(pFile);
    
    free(result);
    
}

int main(int argc, char** argv)
{
//    generateInputFile();
    
//    detectEdge();
    
    
//    
//    
//    FILE *pFile = fopen ("/Users/spicydog/Desktop/opencv/input.txt","r");
//    
//    int rows,cols;
//    fscanf(pFile, "%d %d", &rows, &cols);
//    int *img = (int*) malloc(rows*cols*sizeof(int));
//    for(int i = 0; i < rows; i++)
//    {
//        for(int j = 0; j < cols; j++)
//        {
//            fscanf(pFile, "%d", &img[i*cols+j]);
//        }
//    }
//    fclose(pFile);
//    
//    spliteWork(img,rows,cols);
    
    
    
    writeOutputImage();
    
    return 0;
}