#include "NewBKPSensAnalysis.h"
#include "Util.h"

/**
* Função auxiliar utilizada na implementação da funcionalidade de Sensitivity Analysis.
* @param k Índice do neurônio da camada de saída para o qual se deseja calcular o valor da derivada.
* @return Retorna o valor obtido para a amostra atual.
*/
float NewBKPSensAnalysis::sensitivityOutDeriv( int k )
{
  float retValue=0;
  /* Loop for each of the bindings between the selected neuron and the neurons of the next Layer: */
  for( int l=0 ; l<(_nBKPNet->_nLayers[_nBKPNet->_layers-2]) ; l++ )
    retValue += _nBKPNet->_weights[_nBKPNet->_layers-2][k][l] * _nBKPNet->_neurons[_nBKPNet->_layers-2][l];
  retValue = _nBKPNet->Actderiv( retValue );
  return retValue;
}


/**
* Função auxiliar utilizada na implementação da funcionalidade de Sensitivity Analysis.
* @param layer Camada escondida do neurônio referido em j.
* @param j Índice do neurônio da camada escondida para o qual se deseja calcular o valor da derivada.
* @return Retorna -1 em caso de erro (índice inexistente de camada escondida, etc.), ou 0 caso ok.
*/
float NewBKPSensAnalysis::sensitivityHiddenDeriv( int layer, int j )
{
  float retValue=0;
  /* Loop for each of the bindings between the selected neuron and the neurons of the next Layer: */
  for( int l=0 ; l<(_nBKPNet->_nLayers[layer-1]) ; l++ )
    retValue += _nBKPNet->_weights[layer-1][j][l] * _nBKPNet->_neurons[layer-1][l];
  retValue = _nBKPNet->Actderiv( retValue );
  return retValue;
}


/**
* O método de Sensitivity Analysis gera uma matriz M(k,i), onde k representa a dimensão
* de cada vetor de saída e i representa a dimensão do vetor de saída para uma instância 
* de rede BKP anteriormente criada..
* @param i Neurônio de entrada considerado na análise.
* @param k Neurônio de saída considerado na análise.
* @param sample Índice da amostra escolhida dentro do conjunto de amostras fornecido no construtor.
* @param valRet Valor obtido pelo procedimento para essa amostra apresentada.
* @return Retorna -1 em caso de erro (parâmetros, amostras inválidas) ou 0 caso ok.
*/
int NewBKPSensAnalysis::sensitivityCalculus( int i, int k, int sample, float *valRet )
{
  // Obtendo um ponteiro para a amostra a ser utilizada na análise:
  float *inSample = _inSamples[sample];
  float *outSample = _outSamples[sample];

  float dOk = 0; 
  float auxS = 0;
  // Inserindo a amostra de entrada na rede e obtendo a saída correspondente:
  _nBKPNet->Use(_dimIn_i, inSample);

  // Para cada um dos neurônios da camada escondida, encontra Wkj * Y'j * Vij:
  for( int j=0 ; j<_nBKPNet->_nLayers[_nBKPNet->_layers-2] ; j++ )
    auxS += _nBKPNet->_weights[_nBKPNet->_layers-2][k][j] * 
              sensitivityHiddenDeriv(_nBKPNet->_layers-2, j) * _nBKPNet->_weights[0][j][i];

  // Derivada obtida na camada de saída:
  dOk = sensitivityOutDeriv(k);

  float tmpvalue = dOk * auxS;
  *valRet = dOk * auxS;
  return 0;
}


/**
* Calcula o valor (Sensibility Analysis) para uma dada amostra de treino para todos os neurônios
* de entrada relacionando-os aos de saída, preenchendo a matriz de valores temporários.
* @param sample Amostra para a qual o procedimento será calculado.
*/
void NewBKPSensAnalysis::sensitivitySetSampleMatrix( int sample )
{
  // Para cada um dos neurônios da camada de saída:
  for( int k=0 ; k<_nBKPNet->_nLayers[_nBKPNet->_layers-1] ; k++ )
  {
    for( int i=0 ; i<_nBKPNet->_nLayers[0] ; i++ )
    {
      sensitivityCalculus(i, k, sample, &(_sampleMatrix[k][i]) );
      //printf("%lf\n", _sampleMatrix[k][i]);
    }
    //printf("\n");
  }
}


/**
* A partir do conjunto de amostras contido na instância, monta as três matrizes correspondentes 
* às três unidades de medida utilizando para isso todo o conjunto de amostras
* disponível. A cada amostra, cria a matriz temporária com os valores de análise para a nova amostra, 
* e atualiza os valores das três matrizes de dados.
*/
void NewBKPSensAnalysis::sensitivityBuildMatrices()
{
  // Reinicia os valores das três matrizes:
  for( int k=0 ; k<_dimOut_k ; k++ )
  {
    for( int i=0 ; i<_dimIn_i ; i++)
    {
      _avgS_Matrix[k][i] = 0;
      _absS_Matrix[k][i] = 0;
      _maxS_Matrix[k][i] = 0;
      _sampleMatrix[k][i] = 0;
    }
  }

  // Para cada amostra e para cada matriz, atualiza cada um dos seus valores:
  // Atualiza os valores de todas as posições:
  for( int actSample=0 ; actSample<_nSamples ; actSample++ )
  {
    // Calcula os valores de sensibilidade para cada posição da matriz para a amostra atual:
    sensitivitySetSampleMatrix( actSample );
    // Uma vez que a matriz temporária contém os valores obtidos para a nova amostra, atualiza as 3 matrizes:
    for( int k=0 ; k<_dimOut_k ; k++ )
    {
      for( int i=0 ; i<_dimIn_i ; i++)
      {
        // Incrementa o valor da "mean square average sensibility matrix":
        _avgS_Matrix[k][i] += _sampleMatrix[k][i] * _sampleMatrix[k][i];
        // Incrementa o valor da "absolute value average sensibility matrix" (valores absolutos):
        _absS_Matrix[k][i] += (float)fabs(_sampleMatrix[k][i]);
        // Caso o valor atual seja o maior até agora, incrementa o valor da "maximum sensibility matrix": 
        if( _sampleMatrix[k][i] > _maxS_Matrix[k][i] )
          _maxS_Matrix[k][i] = _sampleMatrix[k][i];
      }
    }  
  }
  // _avgS_Matrix e _absS_Matrix são matrizes de valores médios, e portanto precisam ter cada posição
  // dividida pelo número de amostras:
  for( int k=0 ; k<_dimOut_k ; k++ )
  {
    for( int i=0 ; i<_dimIn_i ; i++)
    {
      _avgS_Matrix[k][i] = (float)sqrt(_avgS_Matrix[k][i] / _nSamples);
      _absS_Matrix[k][i] = _absS_Matrix[k][i] / _nSamples;
    }
  }
}


/**
* Construtor. Recebe um conjunto de amostras de entrada e suas respectivas amostras
* de saída, as dimensões das amostras de entrada e saída, além de um nome de arquivo 
* contendo a rede (previamente criada e já treinada) a passar pelo processo.
* @param nSamples Número de amostras contidas nas matrizes de amostras de entrada e de saída.
* @param dimIn_i Dimensão de cada uma das amostras de entrada (mesma dimensão de entrada da rede BKP).
* @param dimOut_k Dimensão de cada uma das amostras de saída (mesma dimenstão de saída da rede BKP).
* @param inSamples Matriz (nSamples x dimIn_k) contendo todas as amostras de saída.
* @param outSamples Matriz (nSamples x dimOut_i) contendo todas as amostras de entrada.
* @param filename Nome do arquivo que contém a instância de rede neural BKP já treinada.
*/
NewBKPSensAnalysis::NewBKPSensAnalysis( int nSamples, int dimIn_i, int dimOut_k, float** inSamples, 
                                 float** outSamples, char* filename ) 
{
  // Cria as três matrizes de dimensões i x k:
  _avgS_Matrix = new float* [dimOut_k];
  _absS_Matrix = new float* [dimOut_k];
  _maxS_Matrix = new float* [dimOut_k];
  _sampleMatrix   = new float* [dimOut_k];

  for( int i=0 ; i<dimOut_k ; i++ )
  {
    _avgS_Matrix[i]  = new float[dimIn_i];
    _absS_Matrix[i]  = new float[dimIn_i];
    _maxS_Matrix[i]  = new float[dimIn_i];
    _sampleMatrix[i] = new float[dimIn_i];
    for( int j=0 ; j<dimIn_i ; j++ )
    {
      _avgS_Matrix[i][j]  = 0;
      _absS_Matrix[i][j]  = 0;
      _maxS_Matrix[i][j]  = 0;
      _sampleMatrix[i][j] = 0;
    }
  }
  // Armazenando demais valores recebidos:
  _dimIn_i = dimIn_i;
  _dimOut_k = dimOut_k;
  _nSamples = nSamples;
  _inSamples = inSamples;
  _outSamples = outSamples;

  // Abrindo o arquivo que contém a rede neural a ser analisada:
  _nBKPNet = new BKPNeuralNet( filename );
}


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
* @param Layers Número de camadas desejado para a rede neural a ser criada.
* @param nLayers Vetor contendo o número de neurônios em cada uma das camadas da rede.
*/
NewBKPSensAnalysis::NewBKPSensAnalysis( int nSamples, int dimIn_i, int dimOut_k, float** inSamples, 
                                 float** outSamples, BKPNeuralNet *nBKPNet ) 
{
  // Cria as três matrizes de dimensões i x k:
  _avgS_Matrix = new float* [dimOut_k];
  _absS_Matrix = new float* [dimOut_k];
  _maxS_Matrix = new float* [dimOut_k];
  _sampleMatrix   = new float* [dimOut_k];

  for( int i=0 ; i<dimOut_k ; i++ )
  {
    _avgS_Matrix[i]  = new float[dimIn_i];
    _absS_Matrix[i]  = new float[dimIn_i];
    _maxS_Matrix[i]  = new float[dimIn_i];
    _sampleMatrix[i] = new float[dimIn_i];
    for( int j=0 ; j<dimIn_i ; j++ )
    {
      _avgS_Matrix[i][j]  = 0;
      _absS_Matrix[i][j]  = 0;
      _maxS_Matrix[i][j]  = 0;
      _sampleMatrix[i][j] = 0;
    }
  }
  // Armazenando demais valores recebidos:
  _dimIn_i = dimIn_i;
  _dimOut_k = dimOut_k;
  _nSamples = nSamples;
  _inSamples = inSamples;
  _outSamples = outSamples;
  _nBKPNet = nBKPNet;
}


/**
* Destrutor.
*/
NewBKPSensAnalysis::~NewBKPSensAnalysis()
{
  delete _avgS_Matrix;
  delete _sampleMatrix;
  delete _absS_Matrix;
  delete _maxS_Matrix;
  delete _inSamples;
  delete _outSamples;
}


/**
* Normaliza as saídas das matrizes de análise de sensibilidade para que possam ser impressas.
*/
void NewBKPSensAnalysis::sensitivityNormalizeMatrices()
{
  // Normalizando os três vetores:
  for( int k=0 ; k<_dimOut_k ; k++ )
  {
    NormalizeFloatVector( _avgS_Matrix[k], _dimIn_i );
    NormalizeFloatVector( _absS_Matrix[k], _dimIn_i );
    NormalizeFloatVector( _maxS_Matrix[k], _dimIn_i );
  }
}


/**
* Retorna os resultados da análise de sensibilidade para uma saída específica da rede.
* @param desiredOut Saída para a qual será retornada o resultado da análise das entradas.
* @param resultVector Vetor (alocado internamente) contendo os resultados da análise de sensibilidade.
* @param matrixtype Tipo da matriz de entradas utilizada no cálculo.
* @return Retorna -1 em caso de erro, ou 0 caso ok.
*/
void NewBKPSensAnalysis::sensitivityGetResultVector( int desiredOut, float** resultVector, MatrixType matrixtype )
{
  // Aloca o vetor a ser retornado:
  float *tmpResultVector = new float[_dimIn_i];

  // Deleta o vetor de entrada, caso já tenha sido alocado:
  float *retVect = *resultVector;
  if( retVect )
    delete retVect;

  // Encontra a matriz desejada:
  float **Matrix;
  if( matrixtype == MEAN_SQUARE_AVERAGE )     Matrix = _avgS_Matrix;
  if( matrixtype == ABSOLUTE_VALUE_AVERAGE )  Matrix = _absS_Matrix;
  if( matrixtype == MAXIMUM_VALUE )           Matrix = _maxS_Matrix;

  // Copiando os valores do resultado da análise de sensibilidade para o vetor a ser retornado:
  for( int i=0 ; i<_dimIn_i ; i++ )
    tmpResultVector[i] = Matrix[desiredOut][i];

  // Retornando o vetor:
  *resultVector = tmpResultVector;
}


/**
* Escala as saídas das matrizes de análise de sensibilidade para que possam ser impressas.
* @param maxScale Valor máximo a ser aplicado na escala.
* @param minScale Valor mínimo a ser aplicado na escala.
*/
void NewBKPSensAnalysis::sensitivityScaleMatrices( float maxScale, float minScale )
{
  // Normalizando os três vetores:
  for( int k=0 ; k<_dimOut_k ; k++ )
  {
    IntervalScaleInFloatVector( _avgS_Matrix[k], _dimIn_i, maxScale, minScale );
    IntervalScaleInFloatVector( _absS_Matrix[k], _dimIn_i, maxScale, minScale );
    IntervalScaleInFloatVector( _maxS_Matrix[k], _dimIn_i, maxScale, minScale );
  }
}


/**
* Encontra a média e variância dos valores obtidos da analise de sensibilidade das entradas em relação
* à uma saída determinada.
* @param desiredOut Saída para a qual será calculada a média e variância do resultado da análise das entradas.
* @param retMean Média das sensibilidades das entradas em relação a essa saída (retornada).
* @param retVar Variância das sensibilidades das entradas em relação a essa saída (retornada).
* @param matrixtype Tipo da matriz de entradas utilizada no cálculo.
* @return Retorna -1 em caso de erro, ou 0 caso ok.
*/
int NewBKPSensAnalysis::sensitivityStatistics( int desiredOut, float* retMean, float* retVar, MatrixType matrixtype )
{
  float mean=0;
  float var=0;

  // Caso o índice referente à saída desejada não exista, retorna erro:
  if( desiredOut >= _dimOut_k )
    return -1;

  // Constroi as matrizes:
  //sensitivityBuildMatrices();
  //sensitivityScaleMatrices( 0.9f, 0.1f );

  // Encontra a matriz desejada:
  float **Matrix;
  if( matrixtype == MEAN_SQUARE_AVERAGE )     Matrix = _avgS_Matrix;
  if( matrixtype == ABSOLUTE_VALUE_AVERAGE )  Matrix = _absS_Matrix;
  if( matrixtype == MAXIMUM_VALUE )           Matrix = _maxS_Matrix;

  // Média:
  for( int i=0 ; i<_dimIn_i ; i++ )
    mean+= Matrix[desiredOut][i];
  mean = mean/_dimIn_i;
  // Variância:
  for( int i=0 ; i<_dimIn_i ; i++ )
    var+= (Matrix[desiredOut][i]-mean)*(Matrix[desiredOut][i]-mean);
  var = var/_dimIn_i;

  // Retorna os valores da média e variância:
  *retMean = mean;
  *retVar  = var;

  return 0;
}


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
* @return Retorna -1 em caso de erro (nos parâmetros), ou 0 caso ok.
*/
int NewBKPSensAnalysis::sensitivityPrintData( char *filename, int mode, 
                                          char**dataInformation, int dataInformationSize )
{
  // Casos de retorno: 
  //if( (!filename) || (mode < 1) || (mode > 2) || (!dataInformation) || (dataInformationSize != _dimIn_i) )
  if( (!filename) || (mode < 1) || (mode > 2) )

    return -1;

  FILE *f;
  // Abrindo o arquivo:
  if( mode == 1)
    f = fopen(filename, "a");
  if( mode == 2)
    f = fopen(filename, "w");

  fprintf(f, "\n\n");
  // Imprime o valor da sensibilidade de cada uma das saídas da rede em relação a cada uma das entradas:
  for( int k=0 ; k<_dimOut_k ; k++ )
  {
    fprintf(f, "********** Saida %d: **********\n", k);
    fprintf(f, "Mean Square Average sensibility:\n");
    for( int i=0 ; i<_dimIn_i ; i++ )
      fprintf(f, "%s\t%lf\n", dataInformation[i], _avgS_Matrix[k][i]);
    
    fprintf(f, "\n");

    fprintf(f, "Absolute Value Average sensibility:\n");
    for( int i=0 ; i<_dimIn_i ; i++ )
    fprintf(f, "%s\t%lf\n", dataInformation[i], _absS_Matrix[k][i]);
    fprintf(f, "\n");

    fprintf(f, "Maximum sensibility:\n");
    for( int i=0 ; i<_dimIn_i ; i++ )
    fprintf(f, "%s\t%lf\n", dataInformation[i], _maxS_Matrix[k][i]);
    
    fprintf(f, "\n\n");
  }

  fclose(f);
  return 0;
}


/**
* Uma vez que as matrizes foram calculadas para as amostras recebidas pela classe,
* essa função recebe um nome de arquivo, além de uma opção de gravação (acrescentar
* os dados ao arquivo ou reescrevê-lo do início), e grava no arquivo os dados referentes
* aos valores da análise obtidos para as amostras.
* @return Retorna -1 em caso de erro (nos parâmetros), ou 0 caso ok.
*/
int NewBKPSensAnalysis::sensitivityPrintData( char *filename, char**dataInformation, int dataInformationSize )
{
  // Casos de retorno: 
  if( (!filename) || (!dataInformation) || (dataInformationSize != _dimIn_i) )
    return -1;

  // Abré o arquivo:
  FILE *f;

  // Abre o arquivo:
  f = fopen(filename, "a");

  // Imprime o valor da sensibilidade de cada uma das saídas da rede em relação a cada uma das entradas:
  for( int k=0 ; k<_dimOut_k ; k++ )
  {
    for( int i=0 ; i<_dimIn_i ; i++ )
      fprintf(f, "%.15lf\t", _avgS_Matrix[k][i]);
    fprintf(f, "\n");

  }

  fclose(f);
  return 0;
}


/**
* Uma vez que as matrizes foram calculadas para as amostras recebidas pela classe,
* essa função recebe um nome de arquivo, além de uma opção de gravação (acrescentar
* os dados ao arquivo ou reescrevê-lo do início), e grava no arquivo os dados referentes
* aos valores da análise obtidos para as amostras.
* @return Retorna -1 em caso de erro (nos parâmetros), ou 0 caso ok.
*/
int NewBKPSensAnalysis::sensitivityPrintData( char *filename )
{
  // Casos de retorno: 
  if( (!filename)  )
    return -1;

  // Abré o arquivo:
  FILE *f;

  // Abre o arquivo:
  f = fopen(filename, "a");

  // Imprime o valor da sensibilidade de cada uma das saídas da rede em relação a cada uma das entradas:
  for( int k=0 ; k<_dimOut_k ; k++ )
  {
    for( int i=0 ; i<_dimIn_i ; i++ )
      fprintf(f, "%.15lf\n", _avgS_Matrix[k][i]);
    fprintf(f, "\n");

  }

  fclose(f);
  return 0;
}