#ifndef BKP_NEURAL_NET
#define BKP_NEURAL_NET

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


typedef enum
{
  RANDOM_TRAIN,
  ORDERED_TRAIN
} TrainType;


class BKPNeuralNet{

protected:
	 
	int _layers;            ///< Number of Net Layers.
	int *_nLayers;          ///< Number of neurons in each Layer.
	float **_neurons;       ///< Output value of each neuron.
	float ***_weights;      ///< Connection Weights of each neuron from Layer n to Layer n+1.
  float ***_gradients;    ///< Gradients of each Weight.
  float ***_gradientsMt;  ///< Temporary Gradients of each Weight (Momentum).
	float **_deltas;        ///< Auxiliary vector to save temporary deltas while training the net.
  int _flagMomentum;      ///< Auxiliary flag to indicate this is not the first train. 


  /**
  * Retorna a distância Euclideana.
  */
  float EuclideanQuadDist( float *vec1, float* vec2, int size )
  {
    int i;
    float temp = 0;
    for( i=0 ; i<size ; i++ )
      temp+= (float)( (vec1[i]-vec2[i])*(vec1[i]-vec2[i]) );
    return temp;
  }


  /**
  * Variância de um vetor qualquer.
  */
  float Variance( float *vec, int size, float *m_ret )
  {
    // Média:
    float media=0;
    for( int i=0 ; i<size ; i++ )
      media+= vec[i];
    media = media/size;
    *m_ret = media;

    // Obtendo a variância:
    float var=0;
    for( int i=0 ; i<size ; i++ )
      var += ( (vec[i]-media)*(vec[i]-media) );
    return var/size;
  }


	/**
	* Function used to calculate the activation function value for any input, based on the table.
	* @param x Input to calculate the corresponding output value of the function.
	* @return Returns the function output value for the corresponding x input.
	*/
	inline float Act_func (float x) {return 1.0f / (1.0f + (float)exp(-x));}

	/**
	* To train the neural Net, we'll need the activation function derivative's value of any
	* point. This function returns this derivative.
	* @param f Input value. The activation function derivative will be calculated for this value.
	* @return Returns the value of the activation function derivative.
	*/
	inline float Actderiv (float f) {return f * (1 - f);}

	/**
	* Example: for a 3-Layer Net, if I want to train the third Layer
	* I just need to call ActivCalculus( Net, 2 ); -> the indice begins with zero!!!
	* The first Layer (Layer zero) doesn't have Weights before it, right??? 
	*/
	int ActivCalculus( int Layer );

	/** 
	* Function used to find the activation outputs for all the neurons of the Net, calling ActivCalculus for each of the Layers.
	* @return Returns -2 if the value pointed by Net is NULL, or 0 if ok.
	*/
	void ActivAll();

	/**
	* Multiplies 2 vectors, element by element, and adds all results. 
	* Ideally, the first vector would be the input vector, and the second would be the Weight vector with the Weights to be
	* applied to each one of the neuron's inputs.
	*/
	float DotProduct( int n, float *vec1, float *vec2 );

	/**
	* Function that inserts entry values on the Net.
	* @param size Length of the entry vector.
	* @param entry Vector that contains the entries.
	* @return It returns 0 if ok.
	*/
	int SetEntry( int size, float *entry );

  /**
  * Function used to calculate the gradients of the last Layer of neurons.
  * @param PtNet Net Pointer to the Net in use.
  * @param targets Target vector (desired outputs).
  * @param dimTargets Dimension of the target vector.
  * @return Returns -2 if Net points to NULL, -1 if dimTargets is less than zero, or 0 if ok.
  */
	void OutGradient( float *targets, int dimTargets );

  /**
  * Function used to train the hidden Layers's neurons.
  * @param Net Net being used.
  * @param Layer Layer to receive the Gradients based on the already calculated Weights.
  * @return Returns -2 if Net points to NULL, -1 if dimTargets is less than zero, or 0 if ok.
  */
	void HiddenGradient( int Layer );

  /**
  * Function used to calculate the gradients of the last Layer of neurons.
  * @param targets Target vector (desired outputs).
  * @param dimTargets Dimension of the target vector.
  * @param MFactor Momentum factor.
  */
  void OutGradientMtm( float *targets, int dimTargets, float MFactor );

  /**
  * Function used to train the hidden Layers's neurons.
  * @param Layer Layer to receive the Gradients based on the already calculated Weights.
  * @param MFactor Momentum factor.
  * @return Returns -2 if Net points to NULL, -1 if dimTargets is less than zero, or 0 if ok.
  */
  int HiddenGradientMtm( int Layer, float MFactor );

  /**
  * Function that update the Weights on the Net.
  * @param Net Net being used.
  * @param rate The Learning rate.
  * @return Returns -2 if Net points to NULL, -1 if the value of rate is not between 0.0 and 1.0, or 0.
  */
	int UpdateWeights( float rate );

  /**
  * Recebe um vetor de tamanho n e desloca os valores 1 posição à esquerda.
  * O primeiro valor (o valor contido na posição de índice 0 do vetor) será deletado.
  */
  bool ShiftLeft( float *Vector, int size, float newValue, int newValuePosition );


public:

  /**
  * Constructor. It receives a filename of a previous trained BKP Neural Network.
  * @param filename File to be loaded.
  */
	BKPNeuralNet( char* filename );

  /**
  * Constructor. 
  * @param Layers Number of layers desired.
  * @param nLayers Vector containing the number of neurons on each layer.
  */
	BKPNeuralNet( int Layers, int *nLayers);

	/**
	* Function used to create a new neural Net.
	*/
	void CreateNet( int Layers, int *nLayers);

	/**
	* Inserts random numbers between 0.0 and 0.1 as Weight values.
	* @return Returns -2 if Net is pointing to NULL, 0.  
	*/
	int NetInit();
	
	/**
	* Function that calculates the Net's output error
	* @param targets Neuron's targeted output.
	* @param dimTargets Dimension of target vector.
	* @param mode Medium error, Quadratic medium error.
	* @return Returns the resulting error.
	*/
	float OutputError( float *targets, int dimTargets );


	/**
	* Function used to train the net.
	* @param size The size of entry vector.
	* @param entry Vector that contains values of entries.
	* @param size2 The size of out vector.
	* @param out Vector that contains values of outs.
	* @param l_rate The learning rate.
  * @param momentum Momentum factor.
	* @return returns 1 if ok.
	*/
  int Train( int size, float *entry, int size2, float *out, float l_rate, float momentum = 0 );


	/**
	* Once the net is ready, we need a function to use it.
	* @param size Length of the entry vector.
	* @param entry The entries vector.
	* @return It returns 0 if ok.
	*/
	int Use(int size, float *entry);

	/**
	* Function that gets out values from the Net. 
	* @param out Out vetcor.
	* @return It returns 0 if ok.
	*/
	int GetOut( float *out );

	/**
	* Saves a neural net.
  * @param filename Name of the new file. 
	* @return It returns 0 if ok.
	*/
	int SaveNet( char* filename );


  /**
  * Função que obtém o erro médio quadrático de um conjunto de amostras fornecido.
  * @param inMatrix Matriz M[nSamples][inSize] contendo todas as amostras de entrada.
  * @param outMatrix Matriz M[nSamples][inSize] contendo todas as amostras de saída respectivas.
  * @param inSize Dimensão das amostras de entrada (mesma dimensão da camada de entrada da rede).
  * @param outSize Dimensão das amostras de saída (mesma dimensão da camada de saída da rede).
  * @param nSamples Número de amostras fornecido.
  * @param retRMS_Error Valor do erro médio quadrático retornado.
  * @return It returns 0 if ok.
  */
  int RMS_error( float**inMatrix, float **outMatrix, int inSize, int outSize, int nSamples, float* retRMS_Error );
  

  /**
  *   Função contruída com o objetivo de facilitar o treinamento de uma rede. Utiliza critérios 
  * de parada  pré-definidos. O objetivo é paralizar o treinamento a partir do momento em que o 
  * erro médio quadrático da rede em relação às amostras para de  diminuir. Recebe um  parâmetro 
  * indicando um número mínimo de treinos, a a partir do qual se inicia a verificação da variaçao
  * do erro médio quadrático. Recebe também o número de treinamentos a ser executado até que uma
  * nova medição  do erro seja feita. Caso a variância (porcentual) das últimas n  medições seja 
  * menor ou igual a um determinado valor (entre 0 e 1), paraliza o treinamento.
  *   A função recebe ainda um conjunto de amostras (matriz de entradas/matriz de saídas), número 
  * de amostras contidas nas matrizes, a dimensão de cada amostra de entrada e de cada amostra de 
  * saída e um flag indicando se as amostras devem ser treinadas aleatoriamente ou em ordem.
  * @param inMatrix Matriz M[nSamples][inSize] contendo todas as amostras de entrada.
  * @param outMatrix Matriz M[nSamples][inSize] contendo todas as amostras de saída respectivas.
  * @param inSize Dimensão das amostras de entrada (mesma dimensão da camada de entrada da rede).
  * @param outSize Dimensão das amostras de saída (mesma dimensão da camada de saída da rede).
  * @param nSamples Número de amostras fornecido.
  * @param minTrains Número mínimo de treinamentos a ser executado.
  * @param varVectorSize Número de medições erro médio quadrático utilizadas na obtenção da variância.
  * @param minStdDev Desvio-padrão mínimo (porcentuam em relação a média 0~1) das medições de erro 
  * utilizadas como o critério de parada.
  * @param numTrains Uma vez que o número mínimo (minTrains) já foi executado, numTrains é o número de 
  * treinos executado até a próxima medição do erro médio quadrático.
  * @param type Forma como as amostras serão apresentadas à rede (RANDOM_TRAIN/ORDERED_TRAIN).
  * @param l_rate The learning rate.
  * @param momentum Momentum factor.
  * @param retExecutedTrains Número de treinamentos executados até o critério de parada ser atingido.
  * @return It returns 0 if ok.
  */
  int AutoTrain( float**inMatrix, float **outMatrix, int inSize, int outSize, int nSamples, 
                  int minTrains, int varVectorSize, float minStdDev, int numTrains, TrainType type, 
                  float l_rate, float momentum, int* retExecutedTrains );
  
	

};

#endif	// BKP_NEURAL_NET