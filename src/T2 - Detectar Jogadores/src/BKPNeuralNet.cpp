#include "BKPNeuralNet.h"
#include "Util.h"
#include <ctime>


BKPNeuralNet::BKPNeuralNet( int Layers, int *nLayers)
{
	CreateNet( Layers, nLayers);
}

void BKPNeuralNet::CreateNet( int Layers, int *nLayers)
{
	int i, j, l;                 /* Auxiliary counters. */

	/* Verifying possible errors: */
	if ( nLayers == NULL )
		return;

	j=0; /* After the "for" loop below, j will have the number of neurons of the biggest Net Layer: */
	for( i=0 ; i<Layers ; i++ )
	{
		if( nLayers[i] > j )
			j = nLayers[i];
	}

	/* Adding +1 to j (including BIAS): */
	j++;
	/* Allocating space to store neurons's output values of each layer, and to deltas: */

	_neurons = new float *[Layers];
	_deltas  = new float *[Layers];
  //_neurons = (float**)malloc( Layers*sizeof(float*) );
  //_deltas = (float**)malloc( Layers*sizeof(float*) ); 

	for( i=0 ; i<Layers ; i++)
	{
		//_neurons[i]=(float*)malloc(j*sizeof(float));
		//_deltas[i]=(float*)malloc(j*sizeof(float));
    _neurons[i] = new float[j];
    _deltas[i]  = new float[j];
	}

	/* Allocating space to store the neuron's binding Weights from Layer n to Layer n+1: */
	_weights = new float**[(Layers-1)];
  //_weights=(float***)malloc((Layers-1)*sizeof(float**));
	for( i=0 ; i<(Layers-1) ; i++ )
	{
		//_weights[i]=(float**)malloc(j*sizeof(float*));
		_weights[i] = new float*[j];
    for( l=0 ; l<j ; l++ )
			_weights[i][l] = new float[j];
    //_weights[i][l]=(float*)malloc(j*sizeof(float));
	}
	/* Allocating space to store the Gradients related to the Weights of the bindings between neurons: */
	_gradients = new float**[(Layers-1)];
  _gradientsMt = new float**[(Layers-1)];
  //_gradients=(float***)malloc((Layers-1)*sizeof(float**));
  //_gradientsMt=(float***)malloc((Layers-1)*sizeof(float**));


	for( i=0 ; i<(Layers-1) ; i++ )
	{
		_gradients[i] = new float*[j];
    _gradientsMt[i] = new float*[j];
    //_gradients[i]=(float**)malloc(j*sizeof(float*));
    //_gradientsMt[i]=(float**)malloc(j*sizeof(float*));
		for( l=0 ; l<j ; l++ )
    {
			_gradients[i][l] = new float[j];
			_gradientsMt[i][l] = new float[j];
      //_gradients[i][l]=(float*)malloc(j*sizeof(float));
      //_gradientsMt[i][l]=(float*)malloc(j*sizeof(float));

    }
	}

	/* Updating the Net fields: */
	_layers = Layers;
	_nLayers = nLayers;
  _flagMomentum = 0;
}


/**
* Destrutor.
*/
BKPNeuralNet::~BKPNeuralNet()
{
  delete _neurons;
  delete _weights;
  delete _gradientsMt;
  delete _gradients;
  delete _deltas;
  delete _nLayers;
}


/**
* Multiplies 2 vectors, element by element, and adds all results. 
* Ideally, the first vector would be the input vector, and the second would be the Weight vector with the Weights to be
* applied to each one of the neuron's inputs.
*/
float BKPNeuralNet::DotProduct( int n, float *vec1, float *vec2 )
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
* Inserts random numbers between 0.0 and 0.1 as Weight values.
*/
int BKPNeuralNet::NetInit()
{
	int i, j, k;

	/* Gerando uma semente nova, para os n�meros aleat�rios: */
	srand( (unsigned)time( NULL ) );

	/* Initializing Weights and Gradients: */
	for( i=_layers-2 ; i>=0 ; i-- )
		for( j=0 ; j<=_nLayers[i+1] ; j++ )
			for( k=0 ; k<=_nLayers[i] ; k++ )
			{
				/* Inserting random values for the Weights: */
				_weights[i][j][k] = ((float)rand()/(float)RAND_MAX)/_nLayers[i];
		
				/* Inserting 0.0 in Gradients: */
				_gradients[i][j][k] = 0.0;
        _gradientsMt[i][j][k] = 0.0;
			}
			/* Inserting 0.0 in the neurons's output values: */
			for( i=0 ; i<_layers ; i++ )
				for ( j=0 ; j<_nLayers[i] ; j++ )
					_neurons[i][j] = ((float)rand()/(float)RAND_MAX);
			return 0;
}

/**
* Example: for a 3-Layer Net, if I want to train the third Layer
* I just need to call ActivCalculus( Net, 2 ); -> the indice begins with zero!!!
* The first Layer (Layer zero) doesn't have Weights before it, right??? 
*/
int BKPNeuralNet::ActivCalculus( int Layer )
{
	float net, *Weights;

	/* If Layer doesn't exist, returns -1: */
	if( Layer == 0 )
		return -1; 
	
	/* For i from zero to the Layer's number of neurons to be calculated: */ 
	for( int neuronId = 0; neuronId < (_nLayers[Layer]); neuronId++ )
	{
		/* To train the activations of Layer n, I need to know the Weights that bind n to n+1: */
		Weights = _weights[Layer-1][neuronId];
		net = DotProduct( (_nLayers[Layer-1]), _neurons[Layer-1], Weights );

		/* Including the bias: */
		net += _weights[Layer-1][neuronId][_nLayers[Layer-1]];

		/* Inserts the neuron's output value in the Net: */
		_neurons[Layer][neuronId] = Act_func( net );
	}
	return 0;
}



/** 
* Function used to find the activation outputs for all the neurons of the Net, calling ActivCalculus for each of the Layers.
*/
void BKPNeuralNet::ActivAll()
{
	
	/* Calculates the neuron's activations in each Layer: */
	for( int layer = 0; layer < (_layers); layer++ )
		ActivCalculus( layer );
}


/**
* Function that calculates the Net's output error: 
*/
float BKPNeuralNet::OutputError( float *targets, int dimTargets )
{
	float d, sum;

	// Initializing counter:
	sum = 0.0;

	// outLayerId will receive the Net's number of Layers: 
	int outLayerId = _layers - 1;

	// Temp points to the last Layer's neurons:
	float* outNeurons = _neurons[outLayerId];

	for( int neuronId = 0; neuronId < _nLayers[outLayerId]; neuronId++ )
	{
		d = targets[neuronId] - outNeurons[neuronId];
		sum += d * d; // Quadractic error
	}

	// Medium error:
	return sum / (float) _nLayers[outLayerId]; 
}


/**
* Function that inserts entry values on the input Layer of the Net.
*/
int BKPNeuralNet::SetEntry( int size, float *entry )
{
	int count;

	if ( (size != _nLayers[0]) || (entry == NULL) )
		return -1;

	for( count = 0; count < size; count++ )
	{
		/* Inserting correct values: */
		_neurons[0][count] = entry[count];  
	}

	return 0;
}

/**
* Function used to calculate the gradients of the last Layer of neurons.
*/
void BKPNeuralNet::OutGradient( float *targets, int dimTargets )
{
	int i, j;
	float delta, *PtGrad;

	/* j gets the last Layer's index: */
	j = _layers - 1;

	/* Loop for each of the Net's output neurons: */
	for( i = 0; i < _nLayers[j]; i++ )
	{
		/* Calculates the delta value for the index i neuron: */
		delta = (targets[i] -(_neurons[j][i])) * Actderiv(_neurons[j][i]);

		/* Stores the delta value in the delta vector: */
		_deltas[_layers-1][i] = delta;

		/* PtGrad points to the vector that holds the bindings from index i neuron to the neurons of the previous Layer: */
		PtGrad = _gradients[_layers-2][i];

		/* Loop for each of the bindings to the previous Layer: */
		for( int k = 0; k < _nLayers[_layers-2]; k++ )
			PtGrad[k] = _neurons[_layers-2][k] * delta; 

		PtGrad[_nLayers[_layers-2]] = delta; /* The BIAS activation is always one. */
	}
}

/**
* Function used to train the hidden Layers's neurons.
*/
void BKPNeuralNet::HiddenGradient( int Layer )
{
	int j, l;
	float delta, soma, *PtGrad;

	/* j gets the last Layer's index: */
	j = _layers - 1;

	/* Loop for each of the neurons in the Net's selected Layer: */
	for( int k = 0 ; k < _nLayers[Layer]; k++ )
	{
		soma = 0.0;
		
		/* PtGrad points to the vector that holds the bindings of the k index to the previous Layer's neurons: */
		PtGrad = _gradients[Layer-1][k];

		/* Loop for each of the bindings between the selected neuron and the neurons of the next Layer: */
		for( l = 0; l < _nLayers[Layer+1] ; l++ )
			soma += _weights[Layer][l][k] * _deltas[Layer+1][l];

		delta = soma * Actderiv( _neurons[Layer][k] );

		/* Saving this Layer's deltas to train the previous Layer, if there is one: */
		_deltas[Layer][k] = delta;

		/* Updating Gradients: */
		for( l = 0 ; l < _nLayers[Layer-1]; l++ )
			PtGrad[l] = delta * _neurons[Layer-1][l];

		PtGrad[_nLayers[Layer-1]] = delta; /* The BIAS activation is always one. */ 
	}
}


/**
* Function used to calculate the gradients of the last Layer of neurons.
*/
void BKPNeuralNet::OutGradientMtm( float *targets, int dimTargets, float MFactor )
{
  int i, k;
  float delta, *PtGrad, *PtGradMt, auxGrad;

  /* Loop for each of the Net's output neurons: */
  for( i=0 ; i<(_nLayers[_layers-1]) ; i++ )
  {
    if( _flagMomentum == 0)
    {
      /* Calculates the delta value for the index i neuron: */
      delta = (targets[i] -(_neurons[_layers-1][i])) * Actderiv(_neurons[_layers-1][i]);
      /* Stores the delta value in the delta vector: */
      _deltas[_layers-1][i] = delta;
      /* PtGrad points to the vector that holds the bindings from index i neuron to the neurons of the previous Layer: */
      PtGrad   = _gradients[_layers-2][i];
      PtGradMt = _gradientsMt[_layers-2][i];
      /* Loop for each of the bindings to the previous Layer: */
      for( k=0 ; k<(_nLayers[_layers-2]) ; k++ )
      {
        PtGrad[k] = _neurons[_layers-2][k] * delta; 
        PtGradMt[k] = _neurons[_layers-2][k] * delta; 
      }
      PtGrad[(_nLayers[_layers-2])] = delta; /* The BIAS activation is always one. */
      PtGradMt[(_nLayers[_layers-2])] = delta; /* The BIAS activation is always one. */
      _flagMomentum = 1;
    }
    else  /* In this case, it's not the first train. */
    {
      /* Calculates the delta value for the index i neuron: */
      delta = (targets[i] -(_neurons[(_layers)-1][i])) * Actderiv(_neurons[(_layers)-1][i]);
      /* Stores the delta value in the delta vector: */
      _deltas[_layers-1][i] = delta;
      /* PtGrad points to the vector that holds the bindings from index i neuron to the neurons of the previous Layer: */
      PtGrad   = _gradients[_layers-2][i];
      PtGradMt = _gradientsMt[_layers-2][i];
      /* Loop for each of the bindings to the previous Layer: */
      for( k=0 ; k<(_nLayers[_layers-2]) ; k++ )
      {
        auxGrad = _neurons[_layers-2][k] * delta; 
        PtGrad[k] = (MFactor * auxGrad) + (PtGradMt[k] * (1 - MFactor));
        PtGradMt[k] = auxGrad; 
      }
      auxGrad = delta; /* The BIAS activation is always one. */
      PtGrad[(_nLayers[_layers-2])] = (MFactor * auxGrad) + (PtGradMt[(_nLayers[_layers-2])] * (1 - MFactor));
      PtGradMt[(_nLayers[_layers-2])] = delta; /* The BIAS activation is always one. */
    }

  }
}


/**
* Function used to train the hidden Layers's neurons.
*/
int BKPNeuralNet::HiddenGradientMtm( int Layer, float MFactor )
{
  int j, k, l;
  float delta, soma, *PtGrad, *PtGradMt, auxGrad;

  /* j gets the last Layer's index: */
  j = (_layers)-1;

  /* Loop for each of the neurons in the Net's selected Layer: */
  for( k=0 ; k<(_nLayers[Layer]) ; k++ )
  {
    soma = 0.0;
    /* PtGrad points to the vector that holds the bindings of the k index to the previous Layer's neurons: */
    PtGrad = _gradients[Layer-1][k];
    PtGradMt = _gradientsMt[Layer-1][k];
    /* Loop for each of the bindings between the selected neuron and the neurons of the next Layer: */
    for( l=0 ; l<(_nLayers[Layer+1]) ; l++ )
      soma += _weights[Layer][l][k] * _deltas[Layer+1][l];
    delta = soma * Actderiv( _neurons[Layer][k] );
    /* Saving this Layer's deltas to train the previous Layer, if there is one: */
    _deltas[Layer][k] = delta;
    /* Updating Gradients: */
    if( _flagMomentum == 0 )
    {
      for( l=0 ; l<(_nLayers[Layer-1]) ; l++ )
        PtGrad[l] = delta * _neurons[Layer-1][l];
      PtGrad[_nLayers[Layer-1]] = delta; 
    }
    else
    {
      for( l=0 ; l<(_nLayers[Layer-1]) ; l++ )
      {
        auxGrad =  delta * _neurons[Layer-1][l];
        PtGrad[l] = (MFactor * auxGrad) + (PtGradMt[l] * (1 - MFactor));
        PtGradMt[l] = auxGrad;
      }
      auxGrad = delta;
      PtGrad[_nLayers[Layer-1]] = (MFactor * auxGrad) + (PtGradMt[_nLayers[Layer-1]] * (1 - MFactor));
      PtGradMt[_nLayers[Layer-1]] = delta;  /* The BIAS activation is always one. */
    }
  }
  return 0;
}


/**
* Function that update the weights on the net.
*/
int BKPNeuralNet::UpdateWeights( float rate )
{
	if ( rate <= 0.0 || rate > 1.0 )
		return -1;

	for( int layerId =_layers-2 ; layerId >= 0 ; layerId-- )
		for( int neuronId = 0; neuronId <= _nLayers[layerId+1] ; neuronId++ )
			for( int weightId = 0 ; weightId <= _nLayers[layerId] ; weightId++ )
				_weights[layerId][neuronId][weightId] += rate * _gradients[layerId][neuronId][weightId];
	return 0;
}


/**
* Recebe um vetor de tamanho n e desloca os valores 1 posi��o � esquerda.
* O primeiro valor (o valor contido na posi��o de �ndice 0 do vetor) ser� deletado.
*/
bool BKPNeuralNet::ShiftLeft( float *Vector, int size, float newValue, int newValuePosition )
{
  // Caso a posi��o seja maior que a dimens�o do vetor, "shifta" todos pra esquerda e insere o novo valor:
  if( newValuePosition >= size )
  {
    int i=1;
    for( ; i<size ; i++ )
      Vector[i-1] = Vector[i];
    Vector[size-1] = newValue;
    return true;
  }
  // Caso contr�rio, apenas insere o valor na posi��o adequada:
  Vector[newValuePosition] = newValue;
  return false;
}


/**
* Function that get two vectors, one for the entries and another to the outs, and train one epoch: 
*/
int BKPNeuralNet::Train( int size, float *entry, int size2, float *out, float l_rate, float momentum )
{
	/* Inserting the entries: */
	if( SetEntry( size, entry ) == -1 )
		return -1;

	/* Getting the wrong out values: */
	ActivAll();

	/* Training the layer before the out layer: */
  if( momentum != 0)
    OutGradientMtm( out, size2, momentum );
  else
    OutGradient( out, size2 );

  /* Training the net for each hidden layer: */
	for( int i = _layers - 2; i > 0 ; i-- )
  {
    if( momentum != 0)
      HiddenGradientMtm( i, momentum );
    else
      HiddenGradient( i );
  }
			
	/* Updating the weights: */
	UpdateWeights( l_rate );

	return 0;
}




/* Fun��o UseNet, para utilizar a rede depois de treinada: */
int BKPNeuralNet::Use( int size, float *entry)
{
	/* Inserting the entries: */
	if( SetEntry( size, entry) != 0 )
		return -1;

	/* Getting the wrong out values: */
	ActivAll();

	return 0;
}

/**
* Function that gets out values from the Net. 
*/
int BKPNeuralNet::GetOut(float *out )
{
	int count;

	if ( out == NULL )
		return -1;

	for (count = 0 ; count < _nLayers[_layers-1] ; count++ )
	{
		/* Inserting correct values: */
		out[count] = _neurons[_layers-1][count] ;
	}  
	return 0;
}

/**
* Saves a neural net.
*/
int BKPNeuralNet::SaveNet( char* filename )
{
	int i, j; /* Temporary counters. */
	FILE *f;
	char name[128];
	char namebin[128];
	int numFloats = 0;

	if ( filename == NULL )
		return -1;

	/* Inserting the correct extensions: */
	sprintf( name, "%s.nnt", filename );
	sprintf( namebin, "%s.bin", filename );

	f = fopen(name, "w");
	if ( f == NULL )
		return -3;

	/* Number of layers: */
	fprintf(f, "Number of layers: %d\n\n", _layers);

	/* Number of neurons on each layer: */
	for( i = 0; i < _layers; i++ )
		fprintf( f, "Neurons on layer %d: %d\n", i, _nLayers[i]);
	fprintf(f, "Binary file that contains the weights: %s\n", namebin);

	/* Closing the file: */
	fclose(f);

	/* Creating the binary file: */
	f = fopen( namebin, "wb" );
	if ( f == NULL )
		return -3;

	/* Values of the neurons: */
	for( i = 0; i < _layers; i++ )
		numFloats += (int)fwrite( _neurons[i], sizeof(float), _nLayers[i], f );

	/* Weight values */
	for( i = _layers - 2; i >= 0; i-- )			  
	{
		for( j = 0; j <= _nLayers[i+1]; j++ )	
			numFloats += (int)fwrite( _weights[i][j], sizeof(float), _nLayers[i]+1, f );
	}

	fclose( f );
	return 0;
}


/**
* Fun��o que obt�m o erro m�dio quadr�tico de um conjunto de amostras fornecido.
*/
int BKPNeuralNet::RMS_error( float**inMatrix, float **outMatrix, int inSize, int outSize, int nSamples, float* retRMS_Error )
{
  // Casos de retorno:
  if( (!inMatrix) || (!outMatrix) || (inSize!=_nLayers[0]) || (_nLayers[_layers-1]!=outSize) )
    return -1;

  float thisOutput = 0;  
  float RMS_Error_Acum = 0;
  // Executando os treinamentos obrigat�rios:
  for( int thisSample=0 ; thisSample<nSamples ; thisSample++ )
  {
    Use( inSize, inMatrix[thisSample] );
    thisOutput += OutputError(outMatrix[thisSample], outSize);
  }
  *retRMS_Error = (thisOutput/nSamples);
  return 0;
}


/**
*   Fun��o contru�da com o objetivo de facilitar o treinamento de uma rede. Utiliza crit�rios 
* de parada  pr�-definidos. O objetivo � paralizar o treinamento a partir do momento em que o 
* erro m�dio quadr�tico da rede em rela��o �s amostras para de  diminuir. Recebe um  par�metro 
* indicando um n�mero m�nimo de treinos, a a partir do qual se inicia a verifica��o da varia�ao
* do erro m�dio quadr�tico. Recebe tamb�m o n�mero de treinamentos a ser executado at� que uma
* nova medi��o  do erro seja feita. Caso a vari�ncia (porcentual) das �ltimas n  medi��es seja 
* menor ou igual a um determinado valor (entre 0 e 1), paraliza o treinamento.
*   A fun��o recebe ainda um conjunto de amostras (matriz de entradas/matriz de sa�das), n�mero 
* de amostras contidas nas matrizes, a dimens�o de cada amostra de entrada e de cada amostra de 
* sa�da e um flag indicando se as amostras devem ser treinadas aleatoriamente ou em ordem.
*/
int BKPNeuralNet::AutoTrain( float**inMatrix, float **outMatrix, int inSize, int outSize, int nSamples, 
              int minTrains, int varVectorSize, float minStdDev, int numTrains, TrainType type, 
              float l_rate, float momentum, int* retExecutedTrains )
{
  // Casos de retorno:
  if( (!inMatrix) || (!outMatrix) || (inSize!=_nLayers[0]) || (_nLayers[_layers-1]!=outSize) )
    return -1;

  // O n�mero de treinamentos inicial tem que ser pelo menos 0:
  if( *retExecutedTrains < 0 )
    *retExecutedTrains = 0;

  int thisSample = -1;    //< Vari�vel auxiliar, indica a amostra a ser treinada.
  // Executando os treinamentos obrigat�rios:
  for( int i=0 ; i<minTrains ; i++ )
  {
    if( type == ORDERED_TRAIN )
      thisSample = (++thisSample)%nSamples;
    if( type == RANDOM_TRAIN )
      thisSample = RandInt(0, (nSamples-1));
    Train( inSize, inMatrix[thisSample], outSize, outMatrix[thisSample], l_rate, momentum );
  }

  // Executando os demais treinamentos:
  float* varVector = new float[varVectorSize];  //< Vetor para conter as �ltimas medi��es de erro.
  int ptVarVector = 0;              //< Aponta para a primeira posi��o vazia de varVector.
  float lastVariance = (float)MAX_VALUE;   //< Vari�vel que mant�m o valor da varir�ncia.
  float StdDev = (float)MAX_VALUE;   //< Vari�vel que mant�m o valor do desvio-padr�o. 
  thisSample = -1;
  int nTrains=minTrains + *retExecutedTrains;  //< Mant�m o n�mero de treinamentos executados.
  bool varFlag = false;
  while( StdDev > minStdDev )
  {
    if( type == ORDERED_TRAIN )
      thisSample = (++thisSample)%nSamples;
    if( type == RANDOM_TRAIN )
      thisSample = RandInt(0, (nSamples-1));
    Train( inSize, inMatrix[thisSample], outSize, outMatrix[thisSample], l_rate, momentum );
    if( (nTrains%numTrains) == 0 ) //< A cada numTrains treinamentos, testa o erro:
    {
      float retRMS_Error = 0;
      float mean = 0;
      RMS_error( inMatrix, outMatrix, inSize, outSize, nSamples, &retRMS_Error );
      varFlag = ShiftLeft( varVector, varVectorSize, retRMS_Error, ptVarVector );
      if( varFlag == true )
      {
        lastVariance = Variance( varVector, varVectorSize, &mean );
        StdDev = ((float)sqrt(lastVariance))/mean;
      }
      ptVarVector++;
    }
    nTrains++;
    if( nTrains >= 90000 )   //< O n�mero m�ximo de treinamentos ser� 150000.
      StdDev = minStdDev;

  }
  *retExecutedTrains = nTrains;
  return 0;
}


/**
* Loads a neural net.
*/
BKPNeuralNet::BKPNeuralNet( char* filename )
{
	int i, j; /* Temporary counters. */
	int lixo;
	int layers, *nLayers;
	FILE *f;
	char name[128];
	char namebin[128];

	/* Checking arguments: */
	if ( filename == NULL )
		return;

	/* Inserting the correct extensions: */
	sprintf( name, "%s.nnt", filename );
	sprintf( namebin, "%s.bin", filename );

	f = fopen(name, "r");
	if ( f == NULL )
		return;

	/* Number of layers: */
	fscanf( f, "Number of layers: %d\n", &layers);

	/* Space to the number of neurons on each layer: */
	nLayers = (int*)malloc( layers * sizeof(int) );
	if( nLayers == NULL )
		return;

	/* Number of neurons on each layer: */
	for( i = 0; i < layers; i++ )
	{
		fscanf( f, "Neurons on layer %d: %d\n", &lixo, &(nLayers[i]) );
	}

	//BKPNeuralNet( layers, nLayers );
	CreateNet( layers, nLayers );

	/* Closing the text file: */
	fclose( f );

	/* Opening the binary file: */
	f = fopen( namebin, "rb" );
	if( f == NULL )
		return;

	/* Values of the neurons: */
	for( i = 0; i < _layers; i++ )
		fread( _neurons[i] , sizeof(float), _nLayers[i], f );

	/* Weight values */
	for( i = _layers - 2; i >= 0; i-- )			   
		for( j = 0; j <= _nLayers[i+1]; j++ )	
			fread( _weights[i][j], sizeof(float), _nLayers[i]+1, f );

	/* Closing file: */
	fclose( f );
}


