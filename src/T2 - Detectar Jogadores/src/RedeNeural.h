#ifndef NEURAL_H
#define NEURAL_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


typedef struct neural_net
{
	int Layers;			    /*< Number of Net Layers. */ 
	int *NLayers;       /*< Number of neurons in each Layer. */
	float **Neurons;    /*< Output value of each neuron. */
	float ***Weights;   /*< Connection Weights of each neuron from Layer n to Layer n+1. */
	float ***Gradients; /*< Gradients of each Weight. */
  float **Deltas;     /*< Auxiliary vector to save temporary deltas while training the net. */  
} Net, *PtNet;


/**
  * Function used to create a new neural Net.
  * @param RetNet Pointer to a neural Net to be created and returned.
  * @param Layers Number of Layers in the new Net (max two hidden Layers).
  * @param nLayers Number of neurons in each Net Layer to be created. 
  * @return Returns -1 if there is an identified error, -2 if value of nLayers is NULL. Returns 0 if ok.
  */
int CreateNet( PtNet *RetNet, int Layers, int *nLayers );


/**
  * Inserts random numbers between 0.0 and 0.1 as Weight values.
  * @param Net Neural network to be created.
  * @return Returns -2 if Net is pointing to NULL, or 0.  
  */
int NetInit( PtNet Net );


/**
  * Function that calculates the Net's output error.
  * @param Net Pointer to the Net. 
  * @param targets Neuron's targeted output.
  * @param dimTargets Dimension of target vector.
  * @param mode Medium error, Quadratic medium error.
  * @return Returns the resulting error.
  */
float OutputError( PtNet Net, float *targets, int dimTargets );


/**
  * Function used to train the net.
  * @param Net Net being used.
  * @param size The size of entry vector.
  * @param entry Vector that contains values of entries.
  * @param size2 The size of out vector.
  * @param out Vector that contains values of outs.
  * @param l_rate The learning rate.
  * @return returns 1 if ok.
  */
int Train( PtNet Net, int size, float *entry, int size2, float *out, float l_rate );


/**
  * Loads a neural net.
  * @param filename The name of the file.
  * @param myNet Neural net that is going to be loaded.
  * @return Returns -3 if fails to open file, -2 if Net don't point to NULL, -1 if filename is NULL, or 0 if ok.
  */
int LoadNet( char* filename, PtNet *myNet );


/**
  * Saves a neural net.
  * @param filename The name of the file.
  * @param Net Neural net that is going to be saved.
  * @return Returns -3 if fails to open file, -2 if Net points to NULL, -1 if filename is NULL, or 0 if ok.
  */
int SaveNet( char* filename, PtNet Net );


/**
  * Function that inserts entry values on the Net.
  * @param Net Neural network being used.
  * @param size Length of the entry vector.
  * @param entry Vector that contains the entries.
  * @return It returns 0 if ok.
  */
 int SetEntry( PtNet Net, int size, float *entry );


/**
  * Function that gets out values from the Net. 
  * @param Net Neural network being used.
  * @param out Out vetcor.
  * @return It returns 0 if ok.
  */
 int GetOut( PtNet Net, float *out );

 
 /**
   * It gets the index of output neuron that haves greater value.
   * @param Net Neural network being used.
   * @return It returns 0 if ok.
   */
 int GetOutIndex( PtNet Net );


 /**
   * Once the net is ready, we need a function to use it.
   * @param Net Neural network being used.
   * @param size Length of the entry vector.
   * @param entry The entries vector.
   * @return It returns 0 if ok.
   */
  int Use( PtNet Net, int size, float *entry);


#endif
