/* *************************************************************
*
* Class:  
* Created: 
* Author: malf
* $Id: $
*
* Description: 
* 
* $Log: $
*
* *************************************************************/

#ifndef _COOCCURRENCE_H_
#define _COOCCURRENCE_H_

int cooc_imgGrey(unsigned char* colorImage, unsigned char * greyImage,
            unsigned char* greyTable, int w, int h);

int cooc_boxImgGrey(unsigned char* colorImage, unsigned char * greyImage, unsigned char* greyTable, 
                    int w0, int w1, int h0, int h1, int imgW, int imgH);

void cooc_buildMatrix (int ** O, int ng, int d, int theta, int w, int h,
                  unsigned char * image, unsigned char * greyTable );

void cooc_boxBuildMatrix (int ** O, int ng, int d, int theta,
                          unsigned char * image, unsigned char * greyTable,
                          int w0, int w1, int h0, int h1, int imgW, int imgH);

void cooc_normalizeMatrix(int ** O, float ** P, int ng, int theta, int w, int h);

void cooc_boxOrderVertex(int* w0, int* w1, int* h0, int* h1);

float cooc_energy(float** P, int ng);

float cooc_entropy(float** P, int ng);

float cooc_maxProbability(float** P, int ng);

float cooc_contrast(float** P, int ng);

float cooc_IDM(float** P, int ng);

float cooc_mean(float** P, int ng);

float cooc_mean_x(float** P, int ng);

float cooc_mean_y(float** P, int ng);

float cooc_deviation_x(float** P, int ng);

float cooc_deviation_y(float** P, int ng);

float cooc_correlation(float** P, int ng);

float cooc_variance(float** P, int ng);

float cooc_dissimilarity(float** P, int ng);


#endif