#ifndef NEW_BKP_SENS_ANALYSIS
#define NEW_BKP_SENS_ANALYSIS

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "BKPNeuralNet.h"


typedef enum
{
  MEAN_SQUARE_AVERAGE,
  ABSOLUTE_VALUE_AVERAGE,
  MAXIMUM_VALUE
} MatrixType;



class NewBKPSensAnalysis{

private:

  // As tr�s matrizes abaixo armazenam o valor resultante da an�lise para um dado conjunto de amostras:
  BKPNeuralNet *_nBKPNet; ///< Rede a ser analisada.
  float **_avgS_Matrix;   ///< Valores (mean square average sensibility) de cada neur�nio de sa�da em rela��o a cada neur�nio de entrada.
  float **_absS_Matrix;   ///< Valores (absolute value average sensibility) de cada neur�nio de sa�da em rela��o a cada neur�nio de entrada.
  float **_maxS_Matrix;   ///< Valores (maximum sensibility) de cada neur�nio de sa�da em rela��o a cada neur�nio de entrada.
  float **_inSamples;     ///< Matriz contendo todas as amostras de entrada a serem testadas.
  float **_outSamples;    ///< Matriz contendo todas as amostras de sa�da a serem testadas.
  int _nSamples;          ///< N�mero de amostras contido nas matrizes.
  int _dimIn_i;           ///< Dimens�o de cada uma das amostras de entrada.
  int _dimOut_k;          ///< Dimens�o de cada uma das amostras de sa�da.

  // Matriz tempor�ria utilizada como auxiliar nos c�lculos:
  float **_sampleMatrix;


  /**
  * Fun��o auxiliar utilizada na implementa��o da funcionalidade de Sensitivity Analysis.
  * @param k �ndice do neur�nio da camada de sa�da para o qual se deseja calcular o valor da derivada.
  * @return Retorna o valor obtido para a amostra atual.
  */
  float sensitivityOutDeriv( int k );


  /**
  * Fun��o auxiliar utilizada na implementa��o da funcionalidade de Sensitivity Analysis.
  * @param layer Camada escondida do neur�nio referido em j.
  * @param j �ndice do neur�nio da camada escondida para o qual se deseja calcular o valor da derivada.
  * @return Retorna -1 em caso de erro (�ndice inexistente de camada escondida, etc.), ou 0 caso ok.
  */
  float sensitivityHiddenDeriv( int layer, int j );


  /**
  * O m�todo de Sensitivity Analysis gera uma matriz M(k,i), onde k representa a dimens�o
  * de cada vetor de sa�da e i representa a dimens�o do vetor de sa�da para uma inst�ncia 
  * de rede BKP anteriormente criada..
  * @param k Neur�nio de sa�da considerado na an�lise.
  * @param i Neur�nio de entrada considerado na an�lise.
  * @param sample �ndice da amostra escolhida dentro do conjunto de amostras fornecido no construtor.
  * @param valRet Valor obtido pelo procedimento para essa amostra apresentada.
  * @return Retorna -1 em caso de erro (par�metros, amostras inv�lidas) ou 0 caso ok.
  */
  int sensitivityCalculus( int i, int k, int sample, float *valRet );


  /**
  * Calcula o valor (Sensibility Analysis) para uma dada amostra de treino para todos os neur�nios
  * de entrada relacionando-os aos de sa�da, preenchendo a matriz de valores tempor�rios.
  * @param sample Amostra para a qual o procedimento ser� calculado.
  */
  void sensitivitySetSampleMatrix( int sample );


public:


  /**
  * A partir do conjunto de amostras contido na inst�ncia, monta as tr�s matrizes correspondentes 
  * �s tr�s unidades de medida utilizando para isso todo o conjunto de amostras
  * dispon�vel. A cada amostra, cria a matriz tempor�ria com os valores de an�lise para a nova amostra, 
  * e atualiza os valores das tr�s matrizes de dados.
  */
  void sensitivityBuildMatrices();


  /**
  * Construtor. Recebe um conjunto de amostras de entrada e suas respectivas amostras
  * de sa�da, as dimens�es das amostras de entrada e sa�da, al�m de um nome de arquivo 
  * contendo a rede (previamente criada e j� treinada) a passar pelo processo.
  * @param nSamples N�mero de amostras contidas nas matrizes de amostras de entrada e de sa�da.
  * @param dimIn_k Dimens�o de cada uma das amostras de entrada (mesma dimens�o de entrada da rede BKP).
  * @param dimOut_i Dimens�o de cada uma das amostras de sa�da (mesma dimenst�o de sa�da da rede BKP).
  * @param inSamples Matriz (nSamples x dimIn_k) contendo todas as amostras de sa�da.
  * @param outSamples Matriz (nSamples x dimOut_i) contendo todas as amostras de entrada.
  * @param filename Nome do arquivo que cont�m a inst�ncia de rede neural BKP j� treinada.
  */
  NewBKPSensAnalysis( int nSamples, int dimIn_i, int dimOut_k, float** inSamples, 
    float** outSamples, char* filename );

  /**
  * Construtor. Recebe um conjunto de amostras de entrada e suas respectivas amostras
  * de sa�da, as dimens�es das amostras de entrada e sa�da, al�m de um inteiro contendo o n�mero 
  * de camadas da rede BKP a ser criada, bem como um vetor de inteiros contendo o n�mero de neur�nios em
  * cada camada da nova rede.
  * @param nSamples N�mero de amostras contidas nas matrizes de amostras de entrada e de sa�da.
  * @param dimIn_i Dimens�o de cada uma das amostras de entrada (mesma dimens�o de entrada da rede BKP).
  * @param dimOut_k Dimens�o de cada uma das amostras de sa�da (mesma dimenst�o de sa�da da rede BKP).
  * @param inSamples Matriz (nSamples x dimIn_k) contendo todas as amostras de sa�da.
  * @param outSamples Matriz (nSamples x dimOut_i) contendo todas as amostras de entrada.
  * @param nBKPNet Ponteiro para a rede neural BKP que ir� passar pela an�lise de sensibilidade.
  * @param nLayers Vetor contendo o n�mero de neur�nios em cada uma das camadas da rede.
  */
  NewBKPSensAnalysis( int nSamples, int dimIn_i, int dimOut_k, float** inSamples, 
    float** outSamples, BKPNeuralNet *nBKPNet );


  /**
  * Destrutor.
  */
  ~NewBKPSensAnalysis();


  /**
  * Normaliza as sa�das das matrizes de an�lise de sensibilidade para que possam ser impressas.
  */
  void sensitivityNormalizeMatrices();


  /**
  * Retorna os resultados da an�lise de sensibilidade para uma sa�da espec�fica da rede.
  * @param desiredOut Sa�da para a qual ser� retornada o resultado da an�lise das entradas.
  * @param resultVector Vetor (alocado internamente) contendo os resultados da an�lise de sensibilidade.
  * @param matrixtype Tipo da matriz de entradas utilizada no c�lculo.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  void sensitivityGetResultVector( int desiredOut, float** resultVector, MatrixType matrixtype );


  /**
  * Escala as sa�das das matrizes de an�lise de sensibilidade para que possam ser impressas.
  * @param maxScale Valor m�ximo a ser aplicado na escala.
  * @param minScale Valor m�nimo a ser aplicado na escala.
  */
  void sensitivityScaleMatrices( float maxScale, float minScale );


  /**
  * Encontra a m�dia e vari�ncia dos valores obtidos da analise de sensibilidade das entradas em rela��o
  * � uma sa�da determinada.
  * @param desiredOut Sa�da para a qual ser� calculada a m�dia e vari�ncia do resultado da an�lise das entradas.
  * @param retMean M�dia das sensibilidades das entradas em rela��o a essa sa�da (retornada).
  * @param retVar Vari�ncia das sensibilidades das entradas em rela��o a essa sa�da (retornada).
  * @param matrixtype Tipo da matriz de entradas utilizada no c�lculo.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int sensitivityStatistics( int desiredOut, float* retMean, float* retVar, MatrixType matrixtype );
 

  /**
  * Uma vez que as matrizes foram calculadas para as amostras recebidas pela classe,
  * essa fun��o recebe um nome de arquivo, al�m de uma op��o de grava��o (acrescentar
  * os dados ao arquivo ou reescrev�-lo do in�cio), e grava no arquivo os dados referentes
  * aos valores da an�lise obtidos para as amostras.
  * @param filename Nome do arquivo onde os dados ser�o inseridos.
  * @param mode Modo de impress�o dos dados no arquivo. 
  * mode=1 : Caso o arquivo j� exista, os dados s�o adicionados no fim do arquivo.
  * Caso o arquivo de nome filename ainda n�o exista, ele � criado e os dados s�o adicionados.
  * mode=2 : Caso o arquivo j� possua algum tipo de dados, seu valor � deletado. Os dados s�o adicionados
  * a partir do in�cio do arquivo.
  * @param dataInformation Matriz contendo os nomes de cada uma das entradas.
  * @param dataInformationSize Tamanho (dimens�o) da matriz.
  * @return Retorna -1 em caso de erro (nos par�metros), ou 0 caso ok.
  */
  int sensitivityPrintData( char *filename, int mode, char**dataInformation, int dataInformationSize );


  /**
  * Uma vez que as matrizes foram calculadas para as amostras recebidas pela classe,
  * essa fun��o recebe um nome de arquivo, al�m de uma op��o de grava��o (acrescentar
  * os dados ao arquivo ou reescrev�-lo do in�cio), e grava no arquivo os dados referentes
  * aos valores da an�lise obtidos para as amostras.
  * @param filename Nome do arquivo onde os dados ser�o inseridos.
  * @param dataInformation Matriz contendo os nomes de cada uma das entradas.
  * @param dataInformationSize Tamanho (dimens�o) da matriz.
  * @return Retorna -1 em caso de erro (nos par�metros), ou 0 caso ok.
  */
  int sensitivityPrintData( char *filename, char**dataInformation, int dataInformationSize );


  /**
  * Uma vez que as matrizes foram calculadas para as amostras recebidas pela classe,
  * essa fun��o recebe um nome de arquivo, al�m de uma op��o de grava��o (acrescentar
  * os dados ao arquivo ou reescrev�-lo do in�cio), e grava no arquivo os dados referentes
  * aos valores da an�lise obtidos para as amostras.
  * @param filename Nome do arquivo onde os dados ser�o inseridos.
  * @return Retorna -1 em caso de erro (nos par�metros), ou 0 caso ok.
  */
  int sensitivityPrintData( char *filename );


};

#endif	// NEW_BKP_SENS_ANALYSIS