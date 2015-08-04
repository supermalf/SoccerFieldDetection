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

  // As três matrizes abaixo armazenam o valor resultante da análise para um dado conjunto de amostras:
  BKPNeuralNet *_nBKPNet; ///< Rede a ser analisada.
  float **_avgS_Matrix;   ///< Valores (mean square average sensibility) de cada neurônio de saída em relação a cada neurônio de entrada.
  float **_absS_Matrix;   ///< Valores (absolute value average sensibility) de cada neurônio de saída em relação a cada neurônio de entrada.
  float **_maxS_Matrix;   ///< Valores (maximum sensibility) de cada neurônio de saída em relação a cada neurônio de entrada.
  float **_inSamples;     ///< Matriz contendo todas as amostras de entrada a serem testadas.
  float **_outSamples;    ///< Matriz contendo todas as amostras de saída a serem testadas.
  int _nSamples;          ///< Número de amostras contido nas matrizes.
  int _dimIn_i;           ///< Dimensão de cada uma das amostras de entrada.
  int _dimOut_k;          ///< Dimensão de cada uma das amostras de saída.

  // Matriz temporária utilizada como auxiliar nos cálculos:
  float **_sampleMatrix;


  /**
  * Função auxiliar utilizada na implementação da funcionalidade de Sensitivity Analysis.
  * @param k Índice do neurônio da camada de saída para o qual se deseja calcular o valor da derivada.
  * @return Retorna o valor obtido para a amostra atual.
  */
  float sensitivityOutDeriv( int k );


  /**
  * Função auxiliar utilizada na implementação da funcionalidade de Sensitivity Analysis.
  * @param layer Camada escondida do neurônio referido em j.
  * @param j Índice do neurônio da camada escondida para o qual se deseja calcular o valor da derivada.
  * @return Retorna -1 em caso de erro (índice inexistente de camada escondida, etc.), ou 0 caso ok.
  */
  float sensitivityHiddenDeriv( int layer, int j );


  /**
  * O método de Sensitivity Analysis gera uma matriz M(k,i), onde k representa a dimensão
  * de cada vetor de saída e i representa a dimensão do vetor de saída para uma instância 
  * de rede BKP anteriormente criada..
  * @param k Neurônio de saída considerado na análise.
  * @param i Neurônio de entrada considerado na análise.
  * @param sample Índice da amostra escolhida dentro do conjunto de amostras fornecido no construtor.
  * @param valRet Valor obtido pelo procedimento para essa amostra apresentada.
  * @return Retorna -1 em caso de erro (parâmetros, amostras inválidas) ou 0 caso ok.
  */
  int sensitivityCalculus( int i, int k, int sample, float *valRet );


  /**
  * Calcula o valor (Sensibility Analysis) para uma dada amostra de treino para todos os neurônios
  * de entrada relacionando-os aos de saída, preenchendo a matriz de valores temporários.
  * @param sample Amostra para a qual o procedimento será calculado.
  */
  void sensitivitySetSampleMatrix( int sample );


public:


  /**
  * A partir do conjunto de amostras contido na instância, monta as três matrizes correspondentes 
  * às três unidades de medida utilizando para isso todo o conjunto de amostras
  * disponível. A cada amostra, cria a matriz temporária com os valores de análise para a nova amostra, 
  * e atualiza os valores das três matrizes de dados.
  */
  void sensitivityBuildMatrices();


  /**
  * Construtor. Recebe um conjunto de amostras de entrada e suas respectivas amostras
  * de saída, as dimensões das amostras de entrada e saída, além de um nome de arquivo 
  * contendo a rede (previamente criada e já treinada) a passar pelo processo.
  * @param nSamples Número de amostras contidas nas matrizes de amostras de entrada e de saída.
  * @param dimIn_k Dimensão de cada uma das amostras de entrada (mesma dimensão de entrada da rede BKP).
  * @param dimOut_i Dimensão de cada uma das amostras de saída (mesma dimenstão de saída da rede BKP).
  * @param inSamples Matriz (nSamples x dimIn_k) contendo todas as amostras de saída.
  * @param outSamples Matriz (nSamples x dimOut_i) contendo todas as amostras de entrada.
  * @param filename Nome do arquivo que contém a instância de rede neural BKP já treinada.
  */
  NewBKPSensAnalysis( int nSamples, int dimIn_i, int dimOut_k, float** inSamples, 
    float** outSamples, char* filename );

  /**
  * Construtor. Recebe um conjunto de amostras de entrada e suas respectivas amostras
  * de saída, as dimensões das amostras de entrada e saída, além de um inteiro contendo o número 
  * de camadas da rede BKP a ser criada, bem como um vetor de inteiros contendo o número de neurônios em
  * cada camada da nova rede.
  * @param nSamples Número de amostras contidas nas matrizes de amostras de entrada e de saída.
  * @param dimIn_i Dimensão de cada uma das amostras de entrada (mesma dimensão de entrada da rede BKP).
  * @param dimOut_k Dimensão de cada uma das amostras de saída (mesma dimenstão de saída da rede BKP).
  * @param inSamples Matriz (nSamples x dimIn_k) contendo todas as amostras de saída.
  * @param outSamples Matriz (nSamples x dimOut_i) contendo todas as amostras de entrada.
  * @param nBKPNet Ponteiro para a rede neural BKP que irá passar pela análise de sensibilidade.
  * @param nLayers Vetor contendo o número de neurônios em cada uma das camadas da rede.
  */
  NewBKPSensAnalysis( int nSamples, int dimIn_i, int dimOut_k, float** inSamples, 
    float** outSamples, BKPNeuralNet *nBKPNet );


  /**
  * Destrutor.
  */
  ~NewBKPSensAnalysis();


  /**
  * Normaliza as saídas das matrizes de análise de sensibilidade para que possam ser impressas.
  */
  void sensitivityNormalizeMatrices();


  /**
  * Retorna os resultados da análise de sensibilidade para uma saída específica da rede.
  * @param desiredOut Saída para a qual será retornada o resultado da análise das entradas.
  * @param resultVector Vetor (alocado internamente) contendo os resultados da análise de sensibilidade.
  * @param matrixtype Tipo da matriz de entradas utilizada no cálculo.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  void sensitivityGetResultVector( int desiredOut, float** resultVector, MatrixType matrixtype );


  /**
  * Escala as saídas das matrizes de análise de sensibilidade para que possam ser impressas.
  * @param maxScale Valor máximo a ser aplicado na escala.
  * @param minScale Valor mínimo a ser aplicado na escala.
  */
  void sensitivityScaleMatrices( float maxScale, float minScale );


  /**
  * Encontra a média e variância dos valores obtidos da analise de sensibilidade das entradas em relação
  * à uma saída determinada.
  * @param desiredOut Saída para a qual será calculada a média e variância do resultado da análise das entradas.
  * @param retMean Média das sensibilidades das entradas em relação a essa saída (retornada).
  * @param retVar Variância das sensibilidades das entradas em relação a essa saída (retornada).
  * @param matrixtype Tipo da matriz de entradas utilizada no cálculo.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int sensitivityStatistics( int desiredOut, float* retMean, float* retVar, MatrixType matrixtype );
 

  /**
  * Uma vez que as matrizes foram calculadas para as amostras recebidas pela classe,
  * essa função recebe um nome de arquivo, além de uma opção de gravação (acrescentar
  * os dados ao arquivo ou reescrevê-lo do início), e grava no arquivo os dados referentes
  * aos valores da análise obtidos para as amostras.
  * @param filename Nome do arquivo onde os dados serão inseridos.
  * @param mode Modo de impressão dos dados no arquivo. 
  * mode=1 : Caso o arquivo já exista, os dados são adicionados no fim do arquivo.
  * Caso o arquivo de nome filename ainda não exista, ele é criado e os dados são adicionados.
  * mode=2 : Caso o arquivo já possua algum tipo de dados, seu valor é deletado. Os dados são adicionados
  * a partir do início do arquivo.
  * @param dataInformation Matriz contendo os nomes de cada uma das entradas.
  * @param dataInformationSize Tamanho (dimensão) da matriz.
  * @return Retorna -1 em caso de erro (nos parâmetros), ou 0 caso ok.
  */
  int sensitivityPrintData( char *filename, int mode, char**dataInformation, int dataInformationSize );


  /**
  * Uma vez que as matrizes foram calculadas para as amostras recebidas pela classe,
  * essa função recebe um nome de arquivo, além de uma opção de gravação (acrescentar
  * os dados ao arquivo ou reescrevê-lo do início), e grava no arquivo os dados referentes
  * aos valores da análise obtidos para as amostras.
  * @param filename Nome do arquivo onde os dados serão inseridos.
  * @param dataInformation Matriz contendo os nomes de cada uma das entradas.
  * @param dataInformationSize Tamanho (dimensão) da matriz.
  * @return Retorna -1 em caso de erro (nos parâmetros), ou 0 caso ok.
  */
  int sensitivityPrintData( char *filename, char**dataInformation, int dataInformationSize );


  /**
  * Uma vez que as matrizes foram calculadas para as amostras recebidas pela classe,
  * essa função recebe um nome de arquivo, além de uma opção de gravação (acrescentar
  * os dados ao arquivo ou reescrevê-lo do início), e grava no arquivo os dados referentes
  * aos valores da análise obtidos para as amostras.
  * @param filename Nome do arquivo onde os dados serão inseridos.
  * @return Retorna -1 em caso de erro (nos parâmetros), ou 0 caso ok.
  */
  int sensitivityPrintData( char *filename );


};

#endif	// NEW_BKP_SENS_ANALYSIS