//
//  main.cpp
//  N-Body
//
//  Created by Spicydog Proxy on 10/31/14.
//  Copyright (c) 2014 spicydog. All rights reserved.
//

#include <iostream>
#include <math.h>


int main(int argc, const char * argv[]) {


    double G  = 6.67428E-11;
    double T  = 20;

    double halfTT = T*T/2;

    int i,j,k;
    float r;
    float vectorX;
    float vectorY;
    float aX;
    float aY;



    FILE *inputFile = fopen ("input.txt","r");





    int nL = 10000;
    int nN;
    float *swap;



    fscanf(inputFile, "%d", &nN);
    FILE *outputFile = fopen("output.txt","w");
    fprintf(outputFile, "%d %d\n", nL, nN);

    float *posX    = (float*) malloc(nN*sizeof(float));
    float *posY    = (float*) malloc(nN*sizeof(float));
    float *newPosX = (float*) malloc(nN*sizeof(float));
    float *newPosY = (float*) malloc(nN*sizeof(float));
    int   *mass    = (int*) malloc(nN*sizeof(int));
    float *speedX  = (float*) malloc(nN*sizeof(float));
    float *speedY  = (float*) malloc(nN*sizeof(float));
    float *accX    = (float*) malloc(nN*sizeof(float));
    float *accY    = (float*) malloc(nN*sizeof(float));

    for (i=0; i<nN; i++) {
        fscanf(inputFile, "%f %f %d", &posX[i], &posY[i], &mass[i]);
        newPosX[i] = posX[i];
        newPosY[i] = posY[i];
        speedX[i] = 0;
        speedY[i] = 0;
        fprintf(outputFile, "%d ", mass[i]);
    }
    fclose(inputFile);


    fprintf(outputFile, "%d ", mass[i]);

    for (i=0; i<nL; i++) {

        fprintf(outputFile, "\n");
        for (j=0; j<nN; j++) {
            aX = 0;
            aY = 0;
            for (k=0; k<nN; k++) {
                if(j!=k) {
                    vectorX = posX[k]-posX[j];
                    vectorY = posY[k]-posY[j];
                    r = sqrt(vectorX*vectorX + vectorY*vectorY);
                    aX += (mass[k]/r) * vectorX;
                    aY += (mass[k]/r) * vectorY;
                }
            }
            accX[j] = aX * G;
            accY[j] = aY * G;

            speedX[j] = speedX[j] + accX[j] * T;
            speedY[j] = speedY[j] + accY[j] * T;

            newPosX[j] = posX[j] + speedX[j] * T + accX[j] * halfTT;
            newPosY[j] = posY[j] + speedY[j] * T + accY[j] * halfTT;

            fprintf(outputFile, "%.2f,%.2f ",newPosX[j], newPosY[j]);
        }

        swap = newPosX;
        newPosX = posX;
        posX = swap;

        swap = newPosY;
        newPosY = posY;
        posY = swap;

        printf("%d\n",i);
    }

    fclose(outputFile);

    return 0;
}
