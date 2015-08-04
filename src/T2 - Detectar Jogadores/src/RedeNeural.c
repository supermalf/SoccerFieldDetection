#ifdef _WIN32
#include <windows.h>
#endif


#ifndef NEURAL_H
#include "RedeNeural.h"
#endif

#define printf myprintf
void myprintf (){}

/**
  * Function used to calculate the activation function value for any input, based on the table.
  * @param x Input to calculate the corresponding output value of the function.
  * @return Returns the function output value for the corresponding x input.
  */
float Act_func (float x);


/**
  * To train the neural Net, we'll need the activation function derivative's value of any
  * point. This function returns this derivative.
  * @param f Input value. The activation function derivative will be calculated for this value.
  * @return Returns the value of the activation function derivative.
  */
float Actderiv (float f);


/**
  * Example: for a 3-Layer Net, if I want to train the third Layer
  * I just need to call ActivCalculus( Net, 2 ); -> the indice begins with zero!!!
  * The first Layer (Layer zero) doesn't have Weights before it, right??? 
  */
int ActivCalculus( PtNet Net, int Layer );


/** 
  * Function used to find the activation outputs for all the neurons of the Net, calling ActivCalculus for each of the Layers.
  * @param Net The Neural Network.
  * @return Returns -2 if the value pointed by Net is NULL, or 0 if ok.
  */
int ActivAll( PtNet Net );


/**
  * Function used to calculate the gradients of the last Layer of neurons.
  * @param PtNet Net Pointer to the Net in use.
  * @param targets Target vector (desired outputs).
  * @param dimTargets Dimension of the target vector.
  * @return Returns -2 if Net points to NULL, -1 if dimTargets is less than zero, or 0 if ok.
  */
int OutGradient( PtNet Net, float *targets, int dimTargets );


/**
  * Function used to train the hidden Layers's neurons.
  * @param Net Net being used.
  * @param Layer Layer to receive the Gradients based on the already calculated Weights.
  * @return 
  */
int HiddenGradient( PtNet Net, int Layer );


/**
  * Function used to train all the layers.
  * @param Net Net being used.
  * @param targets Target vector (desired outputs).
  * @param dimTargets Dimension of the target vector.
  * @return Returns 1 if ok.
  */
int TrainOneEpoch( PtNet Net, float *targets, int dimTargets );

/**
  * Function that update the Weights on the Net.
  * @param Net Net being used.
  * @param rate The Learning rate.
  * @return Returns -2 if Net points to NULL, -1 if the value of rate is not between 0.0 and 1.0, or 0.
  */
int UpdateWeights( PtNet Net, float rate );






















/**
  * Generate aleatory numbers from zero to one
  */
float generateRand (void)
 {
   return (float)rand()/(float)RAND_MAX;
 }


/**
  * Function used to create a new neural Net.
  */
int CreateNet( PtNet *RetNet, int Layers, int *nLayers)
{
  int i, j, l;                 /* Auxiliary counters. */
  float **Neurons;
  float ***Weights;
  float ***grads;
  float **deltas;
  
  PtNet NewNet;
  /* Verifying possible errors: */
  if ( nLayers == NULL )
    return -2;
  /* Allocating space to create the new Net: */
  NewNet=(PtNet)malloc(sizeof(Net));
  j=0; /* After the "for" loop below, j will have the number of neurons of the biggest Net Layer: */
  for( i=0 ; i<Layers ; i++ )
  {
    if( nLayers[i] > j )
      j = nLayers[i];
  }
  /* Adding +1 to j (including BIAS): */
  j++;
  /* Allocating space to store neurons's output values of each layer, and to deltas: */
  Neurons=(float**)malloc(Layers*sizeof(float*));
  deltas=(float**)malloc(Layers*sizeof(float*)); 
  for( i=0 ; i<Layers ; i++)
  {
    Neurons[i]=(float*)malloc(j*sizeof(float));
    deltas[i]=(float*)malloc(j*sizeof(float));
  }

  /* Allocating space to store the neuron's binding Weights from Layer n to Layer n+1: */
  Weights=(float***)malloc((Layers-1)*sizeof(float**));
  for( i=0 ; i<(Layers-1) ; i++ )
  {
    Weights[i]=(float**)malloc(j*sizeof(float*));
    for( l=0 ; l<j ; l++ )
      Weights[i][l]=(float*)malloc(j*sizeof(float));
  }
  /* Allocating space to store the Gradients related to the Weights of the bindings between neurons: */
  grads=(float***)malloc((Layers-1)*sizeof(float**));
  for( i=0 ; i<(Layers-1) ; i++ )
  {
    grads[i]=(float**)malloc(j*sizeof(float*));
    for( l=0 ; l<j ; l++ )
      grads[i][l]=(float*)malloc(j*sizeof(float));
  }
  /* Updating the Net fields: */
  NewNet->Layers = Layers;
  NewNet->Gradients = grads;
  NewNet->NLayers = nLayers;
  NewNet->Weights = Weights;
  NewNet->Neurons = Neurons;
  NewNet->Deltas = deltas;
  *RetNet = NewNet;
  return 0;
}





/**
  * Inserts random numbers between 0.0 and 0.1 as Weight values.
  */
int NetInit( PtNet Net )
{
  int i, j, k;

  /* Verifying possible errors: */
  if (Net==NULL)
    return -2;
  /* Initializing Weights and Gradients: */
  for( i=Net->Layers-2 ; i>=0 ; i-- )
    for( j=0 ; j<=Net->NLayers[i+1] ; j++ )
      for( k=0 ; k<=Net->NLayers[i] ; k++ )
      {
        /* Inserting random values for the Weights: */
		  Net->Weights[i][j][k] = generateRand()/Net->NLayers[i];
        /*printf("Net->Weights: %lf\n", Net->Weights[i][j][k]);*/
        /* Inserting 0.0 in Gradients: */
        Net->Gradients[i][j][k] = 0.0;
      }
  /* Inserting 0.0 in the neurons's output values: */
  for( i=0 ; i<Net->Layers ; i++ )
    for ( j=0 ; j<Net->NLayers[i] ; j++ )
      Net->Neurons[i][j] = generateRand();
  return 0;
}



/**
  * Multiplies 2 vectors, element by element, and adds all results. 
  * Ideally, the first vector would be the input vector, and the second would be the Weight vector with the Weights to be
  * applied to each one of the neuron's inputs.
  */
float DotProduct( int n, float *vec1, float *vec2 )
{

  int i; /* Auxiliary counter. */
  float acum; /* Acumulator, keeps the added values. */
  acum = 0.0;
  for ( i = 0 ; i < n ; i++ )
  {
	  acum += vec1[i] * vec2[i];
  }

  return acum;
}



/**
  * Function used to calculate the activation function value for any input, based on the table.
  */
float Act_func (float x)
{
	return 1.0f / (1.0f + (float)exp(-x));
}



/**
  * To train the neural Net, we'll need the activation function derivative's value of any 
  * point. This function returns this derivative.
  */
float Actderiv (float f)
{
  return f * (1 - f);
}



/**
  * Example: for a 3-Layer Net, if I want to train the third Layer
  * I just need to call ActivCalculus( Net, 2 ); -> the indice begins with zero!!!
  * The first Layer (Layer zero) doesn't have Weights before it, right??? 
  */
int ActivCalculus( PtNet Net, int Layer )
{
  int i;
  float net, *Weights;
  
  /* If Layer doesn't exist, returns -1: */
  if( (Layer == 0)  )
    return -1; 
  /* If Net is NULL, returns -2: */
  if( Net == NULL )
    return -2;
  /* For i from zero to the Layer's number of neurons to be calculated: */ 
  for( i=0 ; i<(Net->NLayers[Layer]) ; i++ )
  {
    /* To train the activations of Layer n, I need to know the Weights that bind n to n+1: */
    Weights = Net->Weights[Layer-1][i];
    net = DotProduct( (Net->NLayers[Layer-1]), 
            Net->Neurons[Layer-1], Weights );
    /* Including the bias: */
    net += Net->Weights[Layer-1][i][Net->NLayers[Layer-1]];
    /* Inserts the neuron's output value in the Net: */
    Net->Neurons[Layer][i] = Act_func( net );
//	printf ("Camada:%d[%d]=\t %f\n",Layer, i, Net->Neurons[Layer][i]);
	

  }
  return 0;
}



/** 
  * Function used to find the activation outputs for all the neurons of the Net, calling ActivCalculus for each of the Layers.
  */
int ActivAll( PtNet Net )
{
  int i;

  /* If Net is NULL, returns -1: */
  if( Net == NULL )
    return -2;
  /* Calculates the neuron's activations in each Layer: */
  for( i=0; i<(Net->Layers) ; i++ )
    ActivCalculus( Net, i );

  return 0;
}



/**
  * Function that calculates the Net's output error: 
  */
float OutputError( PtNet Net, float *targets, int dimTargets )
{
  int i, j;
  float d, sum, *temp;

  /* Initializing counter: */
  sum = 0.0;
  /* j will receive the Net's number of Layers: */
  j = Net->Layers;
  /* Temp points to the last Layer's neurons: */
  temp = Net->Neurons[j-1];
  for( i=0 ; i< Net->NLayers[j-1] ; i++ )
  {
    d = targets[i] - temp[i];
    sum += d*d; /* Quadractic error. */
  }
  return sum / (float) j; /* Medium error. */
}



/**
  * Function used to calculate the gradients of the last Layer of neurons.
  */
int OutGradient( PtNet Net, float *targets, int dimTargets )
{
  int i, j, k;
  float delta, *PtGrad;

  /* Verifying possible errors: */
  if ( (Net==NULL) || (targets==NULL) )
    return -2;
  if ( dimTargets <= 0 )
    return -1;
  /* j gets the last Layer's index: */
  j = (Net->Layers)-1;
  /* If the targets vector is NULL, or have an incompatible dimension, returns -1: */
  if (dimTargets!=Net->NLayers[j])
    return -1;
  /* Loop for each of the Net's output neurons: */
  for( i=0 ; i<(Net->NLayers[j]) ; i++ )
  {
    /* Calculates the delta value for the index i neuron: */
    delta = (targets[i] -(Net->Neurons[j][i])) * Actderiv(Net->Neurons[j][i]);
    /* Stores the delta value in the delta vector: */
    Net->Deltas[Net->Layers-1][i] = delta;
    /* PtGrad points to the vector that holds the bindings from index i neuron to the neurons of the previous Layer: */
    PtGrad = Net->Gradients[Net->Layers-2][i];
    /* Loop for each of the bindings to the previous Layer: */

    for( k=0 ; k<(Net->NLayers[Net->Layers-2]) ; k++ )
      PtGrad[k] = Net->Neurons[Net->Layers-2][k] * delta; 
    PtGrad[(Net->NLayers[Net->Layers-2])] = delta; /* The BIAS activation is always one. */
  }
  return 0;
}

  


/**
  * Function used to train the hidden Layers's neurons.
  */
int HiddenGradient( PtNet Net, int Layer )
{
  int j, k, l;
  float delta, soma, *PtGrad;
  
  /* j gets the last Layer's index: */
  j = (Net->Layers)-1;
  /* If Net is NULL, returns -2: */
  if ( Net == NULL )
    return -2;
  /* Loop for each of the neurons in the Net's selected Layer: */
  for( k=0 ; k<(Net->NLayers[Layer]) ; k++ )
  {
    soma = 0.0;
    /* PtGrad points to the vector that holds the bindings of the k index to the previous Layer's neurons: */
    PtGrad = Net->Gradients[Layer-1][k];
    /* Loop for each of the bindings between the selected neuron and the neurons of the next Layer: */
    for( l=0 ; l<(Net->NLayers[Layer+1]) ; l++ )
       soma += Net->Weights[Layer][l][k] * Net->Deltas[Layer+1][l];
    delta = soma * Actderiv( Net->Neurons[Layer][k] );
    /* Saving this Layer's deltas to train the previous Layer, if there is one: */
    Net->Deltas[Layer][k] = delta;
    /* Updating Gradients: */
    for( l=0 ; l<(Net->NLayers[Layer-1]) ; l++ )
      PtGrad[l] = delta * Net->Neurons[Layer-1][l];
    PtGrad[Net->NLayers[Layer-1]] = delta; 
  }
  return 0;
}



/**
  * Function used to train all the layers.
  */
int TrainOneEpoch( PtNet Net, float *targets, int dimTargets )
{
   int i, Ret, aux;

   /* Training the layer before the out layer: */
   Ret = OutGradient( Net, targets, dimTargets );
   aux = Net->Layers -2;
   /* Training the net for each hidden layer: */
   for ( i = aux ; i > 0 ; i-- )
      Ret = HiddenGradient( Net, i );
   return Ret;
}




/**
  * Function that update the weights on the net.
  */
int UpdateWeights( PtNet Net, float rate )
{
  int i, j, k;

  if (Net==NULL)
    return -2;
  if (( rate<=0.0) || (rate>1.0))
    return -1;
  for( i=Net->Layers-2 ; i>=0 ; i-- )
    for( j=0 ; j<=Net->NLayers[i+1] ; j++ )
      for( k=0 ; k<=Net->NLayers[i] ; k++ )
        /* Inserts random values for the Weights: */
        Net->Weights[i][j][k] += rate * Net->Gradients[i][j][k];
  return 0;
}



/**
  * Function that inserts entry values on the Net.
  */
 int SetEntry( PtNet Net, int size, float *entry )
{
  int count;
  if ( Net == NULL )
    return -2;
  if ( (size != Net->NLayers[0]) || (entry == NULL) )
    return -1;
  for (count = 0 ; count < size ; count++ )
  {
    /* Inserting correct values: */
    Net->Neurons[0][count] = entry[count];  
	//printf ("Camada:0[%d]=\t %f\n", count, Net->Neurons[0][count]);
  }
  return 0;
}



/**
  * Function that gets out values from the Net. 
  */
 int GetOut( PtNet Net, float *out )
 {
  int count;
   
  if ( Net == NULL )
    return -2;
  
  if ( out == NULL )
    return -1;
   
  for (count = 0 ; count < Net->NLayers[Net->Layers-1] ; count++ )
  {
    /* Inserting correct values: */
    out[count] = Net->Neurons[Net->Layers-1][count] ;
  }  
   return 0;
}


/**
  * Function that gets out values from the Net. 
  */
 int GetOutIndex( PtNet Net )
{
  int count;
  float maior;
  int maiorIndex;
 
  maior = Net->Neurons[Net->Layers-1][0];
  maiorIndex = 0;
  for (count = 0 ; count < Net->NLayers[Net->Layers-1] ; count++ )
  {
	//printf (" %d:%.3d ",count, (int)(Net->Neurons[Net->Layers-1][count]*1000));
    if (maior < Net->Neurons[Net->Layers-1][count])
	{
		maior = Net->Neurons[Net->Layers-1][count];
		maiorIndex = count;
	}
  }
   //printf ("\n");
  return maiorIndex;
}



/* Função UseNet, para utilizar a rede depois de treinada: */
 int Use( PtNet Net, int size, float *entry)
 {
    int Ret;
	/* Inserting the entries: */
    Ret = SetEntry( Net, size, entry);
    /* Getting the wrong out values: */
    ActivAll(Net);
    return 0;
 }



/**
  * Function that get two vectors, one for the entries and another to the outs, and train one epoch: 
  */
 int Train( PtNet Net, int size, float *entry, int size2, float *out, float l_rate )
 {
    int Ret;

    /* Inserting the entries: */
    Ret = SetEntry( Net, size, entry);
    /* Getting the wrong out values: */
    ActivAll(Net);
    /* Training one epoch: */
    Ret = TrainOneEpoch( Net, out, size2 );
    /* Updating the weights: */
    UpdateWeights( Net, l_rate );
    return Ret;
 }



/**
  * Saves a neural net.
  */
int SaveNet( char* filename, PtNet Net )
{
	int i, j; /* Temporary counters. */
  FILE *f;
  char name[128];
  char namebin[128];
  int numFloats = 0;

	/* Checking arguments: */
	if ( Net == NULL )
		return -2;
	if ( filename == NULL )
		return -1;

  /* Inserting the correct extensions: */
  sprintf( name, "%s.nnt", filename );
  sprintf( namebin, "%s.bin", filename );

	f = fopen(name, "w");
	if ( f == NULL )
		return -3;

	/* Number of layers: */
  fprintf(f, "Number of layers: %d\n\n", Net->Layers);
	/* Number of neurons on each layer: */
	for ( i=0 ; i<Net->Layers ; i++ )
    fprintf(f, "Neurons on layer %d: %d\n", i, Net->NLayers[i]);
  fprintf(f, "Binary file that contains the weights: %s\n", namebin);

  /* Closing the file: */
  fclose(f);

  /* Creating the binary file: */
 	f = fopen(namebin, "wb");
	if ( f == NULL )
		return -3;

	/* Values of the neurons: */
	for ( i=0 ; i<Net->Layers ; i++ )
  {

	  numFloats += (int)fwrite(Net->Neurons[i] , sizeof(float), Net->NLayers[i], f);
  }

	/* Weight values */
	for( i=Net->Layers-2 ; i>=0 ; i-- )			  
  {
    printf ("\n%d", i);
    for( j=0 ; j<=Net->NLayers[i+1] ; j++ )	
		{
      numFloats += (int)fwrite(Net->Weights[i][j], sizeof(float), Net->NLayers[i]+1, f);

		}
  }

  printf ( " Total Gravados %d", numFloats);
	fclose(f);
	return 0;
}



/**
  * Loads a neural net.
  */
int LoadNet( char* filename, PtNet *myNet )
{
	int i, j; /* Temporary counters. */
  int lixo;
  int layers, *nLayers;
	FILE *f;
  char name[128];
  char namebin[128];
  PtNet Net;


  /* Checking arguments: */
	if ( filename == NULL )
		return -1;

  /* Inserting the correct extensions: */
  sprintf( name, "%s.nnt", filename );
  sprintf( namebin, "%s.bin", filename );
  
  f = fopen(name, "r");
	if ( f == NULL )
		return -3;

	/* Number of layers: */
  fscanf(f, "Number of layers: %d\n", &layers);
  /* Space to the number of neurons on each layer: */
  nLayers = (int*)malloc(layers*sizeof(int));
  if (nLayers == NULL)
	  return -1;
	/* Number of neurons on each layer: */
	for ( i=0 ; i<layers ; i++ )
  {
		fscanf(f, "Neurons on layer %d: %d\n", &lixo, &(nLayers[i]));
    printf( "Neurons on layer %d: %d\n", lixo, (nLayers[i]));
  }

  CreateNet( &Net, layers, nLayers);

  /* Closing the text file: */
  fclose(f);

  /* Opening the binary file: */
  f = fopen(namebin, "rb");
	if ( f == NULL )
		return -3;


	/* Values of the neurons: */
	for ( i=0 ; i<Net->Layers ; i++ )
	{
    fread(Net->Neurons[i] , sizeof(float), Net->NLayers[i], f);
		/*for ( j=0 ; j<Net->NLayers[i] ; j++ )
		fscanf(f, "%f", &(Net->Neurons[i][j]));*/
	}

	/* Weight values */
	for( i=Net->Layers-2 ; i>=0 ; i-- )			   
		for( j=0 ; j<=Net->NLayers[i+1] ; j++ )	
		{
      fread(Net->Weights[i][j], sizeof(float), Net->NLayers[i]+1, f);
      /*for( k=0 ; k<=Net->NLayers[i] ; k++ )	
			{
				fscanf(f, "%f", &(Net->Weights[i][j][k]));
			}	*/		
		}
   /* Closing file: */
	fclose(f);
   /* Updating pointers: */
  *myNet = Net;
	return 0;
}

