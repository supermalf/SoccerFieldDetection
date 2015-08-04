#include "NewBKPSensAnalysis.h"
#include "Util.h"

/**
* Fun��o auxiliar utilizada na implementa��o da funcionalidade de Sensitivity Analysis.
* @param k �ndice do neur�nio da camada de sa�da para o qual se deseja calcular o valor da derivada.
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
* Fun��o auxiliar utilizada na implementa��o da funcionalidade de Sensitivity Analysis.
* @param layer Camada escondida do neur�nio referido em j.
* @param j �ndice do neur�nio da camada escondida para o qual se deseja calcular o valor da derivada.
* @return Retorna -1 em caso de erro (�ndice inexistente de camada escondida, etc.), ou 0 caso ok.
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
* O m�todo de Sensitivity Analysis gera uma matriz M(k,i), onde k representa a dimens�o
* de cada vetor de sa�da e i representa a dimens�o do vetor de sa�da para uma inst�ncia 
* de rede BKP anteriormente criada..
* @param i Neur�nio de entrada considerado na an�lise.
* @param k Neur�nio de sa�da considerado na an�lise.
* @param sample �ndice da amostra escolhida dentro do conjunto de amostras fornecido no construtor.
* @param valRet Valor obtido pelo procedimento para essa amostra apresentada.
* @return Retorna -1 em caso de erro (par�metros, amostras inv�lidas) ou 0 caso ok.
*/
int NewBKPSensAnalysis::sensitivityCalculus( int i, int k, int sample, float *valRet )
{
  // Obtendo um ponteiro para a amostra a ser utilizada na an�lise:
  float *inSample = _inSamples[sample];
  float *outSample = _outSamples[sample];

  float dOk = 0; 
  float auxS = 0;
  // Inserindo a amostra de entrada na rede e obtendo a sa�da correspondente:
  _nBKPNet->Use(_dimIn_i, inSample);

  // Para cada um dos neur�nios da camada escondida, encontra Wkj * Y'j * Vij:
  for( int j=0 ; j<_nBKPNet->_nLayers[_nBKPNet->_layers-2] ; j++ )
    auxS += _nBKPNet->_weights[_nBKPNet->_layers-2][k][j] * 
              sensitivityHiddenDeriv(_nBKPNet->_layers-2, j) * _nBKPNet->_weights[0][j][i];

  // Derivada obtida na camada de sa�da:
  dOk = sensitivityOutDeriv(k);

  float tmpvalue = dOk * auxS;
  *valRet = dOk * auxS;
  return 0;
}


/**
* Calcula o valor (Sensibility Analysis) para uma dada amostra de treino para todos os neur�nios
* de entrada relacionando-os aos de sa�da, preenchendo a matriz de valores tempor�rios.
* @param sample Amostra para a qual o procedimento ser� calculado.
*/
void NewBKPSensAnalysis::sensitivitySetSampleMatrix( int sample )
{
  // Para cada um dos neur�nios da camada de sa�da:
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
* A partir do conjunto de amostras contido na inst�ncia, monta as tr�s matrizes correspondentes 
* �s tr�s unidades de medida utilizando para isso todo o conjunto de amostras
* dispon�vel. A cada amostra, cria a matriz tempor�ria com os valores de an�lise para a nova amostra, 
* e atualiza os valores das tr�s matrizes de dados.
*/
void NewBKPSensAnalysis::sensitivityBuildMatrices()
{
  // Reinicia os valores das tr�s matrizes:
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
  // Atualiza os valores de todas as posi��es:
  for( int actSample=0 ; actSample<_nSamples ; actSample++ )
  {
    // Calcula os valores de sensibilidade para cada posi��o da matriz para a amostra atual:
    sensitivitySetSampleMatrix( actSample );
    // Uma vez que a matriz tempor�ria cont�m os valores obtidos para a nova amostra, atualiza as 3 matrizes:
    for( int k=0 ; k<_dimOut_k ; k++ )
    {
      for( int i=0 ; i<_dimIn_i ; i++)
      {
        // Incrementa o valor da "mean square average sensibility matrix":
        _avgS_Matrix[k][i] += _sampleMatrix[k][i] * _sampleMatrix[k][i];
        // Incrementa o valor da "absolute value average sensibility matrix" (valores absolutos):
        _absS_Matrix[k][i] += (float)fabs(_sampleMatrix[k][i]);
        // Caso o valor atual seja o maior at� agora, incrementa o valor da "maximum sensibility matrix": 
        if( _sampleMatrix[k][i] > _maxS_Matrix[k][i] )
          _maxS_Matrix[k][i] = _sampleMatrix[k][i];
      }
    }  
  }
  // _avgS_Matrix e _absS_Matrix s�o matrizes de valores m�dios, e portanto precisam ter cada posi��o
  // dividida pelo n�mero de amostras:
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
* de sa�da, as dimens�es das amostras de entrada e sa�da, al�m de um nome de arquivo 
* contendo a rede (previamente criada e j� treinada) a passar pelo processo.
* @param nSamples N�mero de amostras contidas nas matrizes de amostras de entrada e de sa�da.
* @param dimIn_i Dimens�o de cada uma das amostras de entrada (mesma dimens�o de entrada da rede BKP).
* @param dimOut_k Dimens�o de cada uma das amostras de sa�da (mesma dimenst�o de sa�da da rede BKP).
* @param inSamples Matriz (nSamples x dimIn_k) contendo todas as amostras de sa�da.
* @param outSamples Matriz (nSamples x dimOut_i) contendo todas as amostras de entrada.
* @param filename Nome do arquivo que cont�m a inst�ncia de rede neural BKP j� treinada.
*/
NewBKPSensAnalysis::NewBKPSensAnalysis( int nSamples, int dimIn_i, int dimOut_k, float** inSamples, 
                                 float** outSamples, char* filename ) 
{
  // Cria as tr�s matrizes de dimens�es i x k:
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

  // Abrindo o arquivo que cont�m a rede neural a ser analisada:
  _nBKPNet = new BKPNeuralNet( filename );
}


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
* @param Layers N�mero de camadas desejado para a rede neural a ser criada.
* @param nLayers Vetor contendo o n�mero de neur�nios em cada uma das camadas da rede.
*/
NewBKPSensAnalysis::NewBKPSensAnalysis( int nSamples, int dimIn_i, int dimOut_k, float** inSamples, 
                                 float** outSamples, BKPNeuralNet *nBKPNet ) 
{
  // Cria as tr�s matrizes de dimens�es i x k:
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
* Normaliza as sa�das das matrizes de an�lise de sensibilidade para que possam ser impressas.
*/
void NewBKPSensAnalysis::sensitivityNormalizeMatrices()
{
  // Normalizando os tr�s vetores:
  for( int k=0 ; k<_dimOut_k ; k++ )
  {
    NormalizeFloatVector( _avgS_Matrix[k], _dimIn_i );
    NormalizeFloatVector( _absS_Matrix[k], _dimIn_i );
    NormalizeFloatVector( _maxS_Matrix[k], _dimIn_i );
  }
}


/**
* Retorna os resultados da an�lise de sensibilidade para uma sa�da espec�fica da rede.
* @param desiredOut Sa�da para a qual ser� retornada o resultado da an�lise das entradas.
* @param resultVector Vetor (alocado internamente) contendo os resultados da an�lise de sensibilidade.
* @param matrixtype Tipo da matriz de entradas utilizada no c�lculo.
* @return Retorna -1 em caso de erro, ou 0 caso ok.
*/
void NewBKPSensAnalysis::sensitivityGetResultVector( int desiredOut, float** resultVector, MatrixType matrixtype )
{
  // Aloca o vetor a ser retornado:
  float *tmpResultVector = new float[_dimIn_i];

  // Deleta o vetor de entrada, caso j� tenha sido alocado:
  float *retVect = *resultVector;
  if( retVect )
    delete retVect;

  // Encontra a matriz desejada:
  float **Matrix;
  if( matrixtype == MEAN_SQUARE_AVERAGE )     Matrix = _avgS_Matrix;
  if( matrixtype == ABSOLUTE_VALUE_AVERAGE )  Matrix = _absS_Matrix;
  if( matrixtype == MAXIMUM_VALUE )           Matrix = _maxS_Matrix;

  // Copiando os valores do resultado da an�lise de sensibilidade para o vetor a ser retornado:
  for( int i=0 ; i<_dimIn_i ; i++ )
    tmpResultVector[i] = Matrix[desiredOut][i];

  // Retornando o vetor:
  *resultVector = tmpResultVector;
}


/**
* Escala as sa�das das matrizes de an�lise de sensibilidade para que possam ser impressas.
* @param maxScale Valor m�ximo a ser aplicado na escala.
* @param minScale Valor m�nimo a ser aplicado na escala.
*/
void NewBKPSensAnalysis::sensitivityScaleMatrices( float maxScale, float minScale )
{
  // Normalizando os tr�s vetores:
  for( int k=0 ; k<_dimOut_k ; k++ )
  {
    IntervalScaleInFloatVector( _avgS_Matrix[k], _dimIn_i, maxScale, minScale );
    IntervalScaleInFloatVector( _absS_Matrix[k], _dimIn_i, maxScale, minScale );
    IntervalScaleInFloatVector( _maxS_Matrix[k], _dimIn_i, maxScale, minScale );
  }
}


/**
* Encontra a m�dia e vari�ncia dos valores obtidos da analise de sensibilidade das entradas em rela��o
* � uma sa�da determinada.
* @param desiredOut Sa�da para a qual ser� calculada a m�dia e vari�ncia do resultado da an�lise das entradas.
* @param retMean M�dia das sensibilidades das entradas em rela��o a essa sa�da (retornada).
* @param retVar Vari�ncia das sensibilidades das entradas em rela��o a essa sa�da (retornada).
* @param matrixtype Tipo da matriz de entradas utilizada no c�lculo.
* @return Retorna -1 em caso de erro, ou 0 caso ok.
*/
int NewBKPSensAnalysis::sensitivityStatistics( int desiredOut, float* retMean, float* retVar, MatrixType matrixtype )
{
  float mean=0;
  float var=0;

  // Caso o �ndice referente � sa�da desejada n�o exista, retorna erro:
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

  // M�dia:
  for( int i=0 ; i<_dimIn_i ; i++ )
    mean+= Matrix[desiredOut][i];
  mean = mean/_dimIn_i;
  // Vari�ncia:
  for( int i=0 ; i<_dimIn_i ; i++ )
    var+= (Matrix[desiredOut][i]-mean)*(Matrix[desiredOut][i]-mean);
  var = var/_dimIn_i;

  // Retorna os valores da m�dia e vari�ncia:
  *retMean = mean;
  *retVar  = var;

  return 0;
}


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
* @return Retorna -1 em caso de erro (nos par�metros), ou 0 caso ok.
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
  // Imprime o valor da sensibilidade de cada uma das sa�das da rede em rela��o a cada uma das entradas:
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
* essa fun��o recebe um nome de arquivo, al�m de uma op��o de grava��o (acrescentar
* os dados ao arquivo ou reescrev�-lo do in�cio), e grava no arquivo os dados referentes
* aos valores da an�lise obtidos para as amostras.
* @return Retorna -1 em caso de erro (nos par�metros), ou 0 caso ok.
*/
int NewBKPSensAnalysis::sensitivityPrintData( char *filename, char**dataInformation, int dataInformationSize )
{
  // Casos de retorno: 
  if( (!filename) || (!dataInformation) || (dataInformationSize != _dimIn_i) )
    return -1;

  // Abr� o arquivo:
  FILE *f;

  // Abre o arquivo:
  f = fopen(filename, "a");

  // Imprime o valor da sensibilidade de cada uma das sa�das da rede em rela��o a cada uma das entradas:
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
* essa fun��o recebe um nome de arquivo, al�m de uma op��o de grava��o (acrescentar
* os dados ao arquivo ou reescrev�-lo do in�cio), e grava no arquivo os dados referentes
* aos valores da an�lise obtidos para as amostras.
* @return Retorna -1 em caso de erro (nos par�metros), ou 0 caso ok.
*/
int NewBKPSensAnalysis::sensitivityPrintData( char *filename )
{
  // Casos de retorno: 
  if( (!filename)  )
    return -1;

  // Abr� o arquivo:
  FILE *f;

  // Abre o arquivo:
  f = fopen(filename, "a");

  // Imprime o valor da sensibilidade de cada uma das sa�das da rede em rela��o a cada uma das entradas:
  for( int k=0 ; k<_dimOut_k ; k++ )
  {
    for( int i=0 ; i<_dimIn_i ; i++ )
      fprintf(f, "%.15lf\n", _avgS_Matrix[k][i]);
    fprintf(f, "\n");

  }

  fclose(f);
  return 0;
}