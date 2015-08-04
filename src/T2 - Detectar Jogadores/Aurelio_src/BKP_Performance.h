#ifndef BKP_PERFORMANCE
#define BKP_PERFORMANCE

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "BKPNeuralNet.h"
#include "NewBKPSensAnalysis.h"
#include "Util.h"
#include "ImageSamples.h"



/**
* Classe que tem como finalidade principal treinar e avaliar redes neurais, avaliando o conjunto de 
* dados de entrada, bem como possiveis topologias de rede.
*/
class BKP_Performance
{
public:

  BKPNeuralNet *_prfNet;                ///< Instancia de rede neural sendo analizada.
  NewBKPSensAnalysis *_prfSenAnalysis;  ///< Instancia da classe de analise se sensibilidade que analisa a rede.
  ImageSamples *_prfSamples;            ///< Instancia da classe que ira conter todas as amostras.

  // Demais dados referentes as instancias das redes que serao testadas:
  TrainType _prfType;                   ///< Tipo de treinamento a ser executado.
  int _prfMinHiddenNeurons;             ///< Numero minimo de neuronios testados na camada escondida.
  int _prfMaxHiddenNeurons;             ///< Numero maximo de neuronios testados na camada escondida.
  int _prfMinEntries;                   ///< Numero de entradas na camada de entrada utilizado como criterio de parada.
  int _prfRepeat;                       ///< Numero de repeticoes de treino de uma mesma topologia (escolhe-se a melhor).
  MatrixType _prfMatrixtype;            ///< Tipo de matriz a ser utilizada durante a análise de sensibilidade.
  bool *_prfInPrepFlags;                ///< Vetor de flags indica os índices do vetor de entradas original atualmente sendo utilizados.

  // Dados necessários para a análise de sensibilidade (variam no tempo).
  // TODOS OS DADOS AQUI CONTIDOS JÁ ESTÃO PRONTOS PARA TREINAMENTO, ESTANDO ESCALONADOS, DE DIMENSÃO APROPRIADA, ETC:
  int _nSamples;                ///< Número de amostras que irão treinar a rede em cada passo.
  int _dimIn_i;                 ///< Dimensão das amostras de entrada da rede em cada passo.
  int _dimOut_k;                ///< Dimensão das amostras de saída da rede em cada passo.
  float** _inSamples;           ///< Matriz contendo todas as amostras de entrada.
  float** _outSamples;          ///< Matriz contendo todas as amostras de saída.
  dataType* _inSamplesTypes;    ///< Vetor dos tipos das amostras de entrada (mesma dimensão das amostras de entrada).
  dataGroup* _inSamplesGroups;  ///< Vetor dos grupos das amostras de entrada (mesma dimensão das amostras de entrada). 

  // NÃO SERÃO UTILIZADOS POR ENQUANTO:
  dataType* _outSamplesTypes;   ///< Vetor dos tipos das amostras de saída (mesma dimensão das amostras de saída).
  dataGroup* _outSamplesGroups; ///< Vetor dos grupos das amostras de saída (mesma dimensão das amostras de saída). 

  // VARIÁVEIS AUXILIARES (APENAS PARA EVITAR REALOCAÇÃO) :
  bool *_resultBoolVector;       ///< Vetor temporário para conter os resultados da análise de sensibilidade a cada passo.



  /**
  * Função auxiliar. Imprime uma determinada amostra retornada por uma instância de ImageSamples (DOS DADOS QUE VARIAM C/ O TEMPO).
  * @param inSample Vetor contendo amostra de entrada.
  * @param inType  Vetor contendo o tipo de cada medição da amostra de entrada.
  * @param inGroup Vetor contendo o grupo de cada medição da amostra de entrada.
  * @param inSize Dimensão do vetor de entrada, bem como dos vetores de tipos e grupos.
  * @param outSample Vetor contendo uma das amostras de saída.
  * @param outSize Dimensão da amostra de saída.
  * @return Retorna false em caso de erro, ou true caso ok.
  */
  void prfSmpPrint( float *inSample, dataType *inType, dataGroup *inGroup, int inSize, 
    float *outSample, int outSize )
  {
    printf("****************************************************************\n");
    printf("InSample:\n");
    for( int j=0 ; j<inSize ; j++ )
    {
      printf("\t%s", GetDataTypeChar( inType[j] ));
      printf("\t%s", GetDataGroupChar( inGroup[j] ));
      printf("\t%lf\n", inSample[j]);
    }
    printf("\n\n");
    printf("OutSample:\n");
    for( int j=0 ; j<outSize ; j++ )
    {
      printf("\t%lf", outSample[j]);
    }
    printf("\n\n\n");
  }


  /**
  * Recebe um vetor de valores, além da variância e média dos valores do vetor, e 
  * retorna um vetor booleano contendo true em todas as posições do vetor dentro 
  * que estão dentro do desvio-padrão ou além do desvio-padrão para mais, e false 
  * em todas as que estiverem fora do desvio-padrão para menos (ou seja, retorna as
  * que possuem maior valor).
  * @param inVector Vetor contendo todos os valores de entrada.
  * @param mean Média desses valores.
  * @param var Variância dos valores desse vetor.
  * @param retVector Vetor retornado com somente as entradas dentro do desvio-padrão.
  * @return Retorna false em caso de erro, ou true caso ok.
  */
  bool prfGetImportanceVector( float *inVector, int size, float mean, float var, bool *retVector )
  {
    // Casos de retorno:
    if( (!inVector) || (!retVector) )
      return false;

    float stdDev = (float)sqrt(var);

    for( int i=0 ; i<size ; i++ )
    {
      if( (ABS(inVector[i]-mean) > stdDev) && (inVector[i]<mean) )
        retVector[i] = false;
      else
        retVector[i] = true;
    }
    return true;
  }


  /**
  * Recebe um vetor de valores, e retorna um vetor booleano em que foram desligadas as
  * n menores componentes do vetor de entrada. 
  * @param inVector Vetor contendo todos os valores de entrada.
  * @param numOff Número de entradas que terão seu valor false (serão desligadas).
  * @param retVector Vetor retornado com somente as entradas dentro do desvio-padrão.
  * @return Retorna false em caso de erro, ou true caso ok.
  */
  bool prfGetImportanceVector( float *inVector, int size, float numOff, bool *retVector )
  {
    // Casos de retorno:
    if( (!inVector) || (!retVector) )
      return false;

    // Liga todas as componentes do vetor booleano:
    for( int i=0 ; i<size ; i++ )
      retVector[i] = true;

    float min = (float)1e12;
    int pos = 0;
    for( int n=0 ; n<numOff ; n++ )
    {
      float min = (float)1e12;
      int pos = 0;
      for( int i=0 ; i<size ; i++ )
      {
        if( (inVector[i] < min) && (retVector[i] == true) )
        {
          min = inVector[i];
          pos = i;
        }
      }
      retVector[pos] = false;
    }
    return true;
  }

  /**
  * Essa é a função que executa a análise de sensibilidade do estado atual das amostras e retorna o resultado.
  * SOMENTE PARA A CONFIGURAÇÃO ATUAL!!!!   INSERIR AS VARIÁVEIS DE AUTOTRAIN DEPOIS!!!
  * @param minTrains Número mínimo de treinamentos a ser executado.
  * @param varVectorSize Número de medições erro médio quadrático utilizadas na obtenção da variância.
  * @param minStdDev Desvio-padrão mínimo (porcentuam em relação a média 0~1) das medições de erro 
  * utilizadas como o critério de parada.
  * @param numTrains Uma vez que o número mínimo (minTrains) já foi executado, numTrains é o número de 
  * treinos executado até a próxima medição do erro médio quadrático.
  */
  int prfMakeSensAnalysis( char* filename )
  {
    int *nLayers = NULL;    //< Irá conter a topologia da rede.

    // Para imprimir os dados, é necessário que tenhamos seus tipos e grupos de escalonamento:
    dataGroup *tmpdataGroup = _prfSamples->_inSamples[0]->inSmpGetDataGroupVector();
    dataType  *tmpdataType  = _prfSamples->_inSamples[0]->inSmpGetDataTypeVector();
    int inSize = _prfSamples->smpGetInSamplesDimension();

    int gen=0;      //< Geração atual da análise de sensibilidade.
    int layers = 3; //< Número de camadas.
    nLayers = new int[layers];   //< Alocando espaço para o vetor que mantém a topologia da rede.

    // Enquanto o número mínimo de entradas não for atingido:
    while( _prfSamples->smpGetInSamplesPrepDimension() >= _prfMinEntries )
    {
      // Criando as matrizes e vetores necessários para a análise de acordo com a configuração atual:
      prfConfigureData();

      BKPNeuralNet *bestNet = NULL;     //< Ponteiro para armazenar a melhor das redes neurais.
      float bestRMS_Error = (float)1e12;
      // Feito isso, c/ as informações é criada a instância da rede neural e da classe de análise de sensibilidade:
      for( int i=_prfMinHiddenNeurons ; i<_prfMaxHiddenNeurons ; i++ )
      {
        nLayers[0] = _dimIn_i;     //< Número de neurônios em cada camada.
        nLayers[1] = i;
        nLayers[2] = _dimOut_k;

        // Treinando a mesma topologia _prfRepeat vezes e escolhendo a melhor:
        for( int j=0 ; j<_prfRepeat ; j++ )
        {
          int nTrainsRet = 0; //< Número de treinamentos realizados em cada uma das redes.
          // Criando a instância da rede neural de topologia apropriada:
          bestNet = new BKPNeuralNet( layers, nLayers );
          bestNet->NetInit();

          float retRMS_Error=0;
          float retBestPerc=0;
          // Treinando a rede com as amostras até que o critério de parada seja atingido:
          int ret = bestNet->AutoTrain( _inSamples, _outSamples, _dimIn_i, _dimOut_k, _nSamples-1, 3000, 5, 0.08f, 1000, _prfType,
            0.2f, 0.6f, &nTrainsRet,  &retRMS_Error, &retBestPerc );

          if( ret == -1 )
            j--;

          // Caso essa seja a melhor rede obtida até agora, ela é armazenada:
          if( (retRMS_Error < bestRMS_Error) && (retRMS_Error > 0.0000001) )
          {
            _prfNet = bestNet;
            bestRMS_Error = retRMS_Error;
          }
          else    //< Caso contrário ela é deletada: 
            delete bestNet;
          bestNet = NULL;
        }
        // Incrementa o número de neurônios na camada escondida para a nova rede que será criada:
        nLayers[1] = i;
      }

      if( _prfNet!=NULL )
      {
        // Uma vez obtida a melhor rede, é criada a instância da classe de análise de sensibilidade apropriada:
        _prfSenAnalysis = new NewBKPSensAnalysis( _nSamples, _dimIn_i, _dimOut_k, _inSamples, _outSamples, _prfNet );
        _prfSenAnalysis->sensitivityBuildMatrices();    //< De posse da rede treinada, constrói as matrizes de sensibilidade.
        _prfSenAnalysis->sensitivityScaleMatrices( 0.9f, 0.1f );  //< Escalona as saídas.
        // Obtém o vetor de sensibilidades da matriz desejada, e obtém também os valores da média e variância dos resultados:
        float* resultVector = NULL;
        float retMean = NULL;
        float retVar = NULL;
        _prfSenAnalysis->sensitivityStatistics( 0, &retMean, &retVar, _prfMatrixtype );
        _prfSenAnalysis->sensitivityGetResultVector( 0, &resultVector, _prfMatrixtype );

        // De posse do novo vetor booleano, posso reconfigurar as amostras e treinar redes com menos entradas:
        _prfSamples->smpGetInSamplesFlagsVector( _prfInPrepFlags );

        // Imprimindo os resultados desse geração:
        SensAnalysisPrint( tmpdataType, tmpdataGroup, _prfInPrepFlags, inSize, resultVector, _dimIn_i, "teste.txt", gen );

        // Obtendo o vetor booleano da nova configuração:
        //prfGetImportanceVector( resultVector, _dimIn_i, retMean, retVar, _resultBoolVector );
        prfGetImportanceVector( resultVector, _dimIn_i, 1, _resultBoolVector );

        int pos = 0;
        for( int i=0 ; i<_prfSamples->smpGetInSamplesDimension() ; i++ )
        {
          int j = _prfSamples->smpGetInSamplesDimension();
          
          if( _prfInPrepFlags[i] == true )
          {
            _prfSamples->smpChangeOneFlag( _resultBoolVector[pos], i );
            pos++;
          }
        }

        // Incrementa a geração:
        gen++;
      }
    }
    return 0;
  }


  /** Para a impressão:
  * @param inDataType Vetor de dataTypes, contendo os tipos de TODAS AS AMOSTRAS DE ENTRADA ORIGINAIS.
  * @param inDataGroup Vetor de dataGroups, contendo os grupos de TODAS AS AMOSTRAS DE ENTRADA ORIGINAIS.
  * @param inDataFlags Vetor de flags, contendo o estágio atual dos flags de TODAS AS AMOSTRAS DE ENTRADA ORIGINAIS.
  * @param inSize Dimensão dos três vetores acima.
  * @param sensResult Vetor contendo o resultado da análise de sensibilidade da geração atual das amostras.
  * @param sensResultSize Dimensão do vetor acima.
  * @param filename Nome do arquivo onde serão armazenadas as informações.
  * @param gen Geração atual da impressão.
  */
  void SensAnalysisPrint( dataType *inDataType, dataGroup *inDataGroup, bool *inDataFlags, int inSize,
                          float *sensResult, int sensResultSize, char* filename, int gen )
  {
    //  Abrindo o arquivo dos relatórios:
    FILE *f = fopen( filename, "a" );

    fprintf(f, "\n\n");
    fprintf(f, "********** Geracao: %d: **********\n\n", gen);

    int pos = 0;
    for( int j=0 ; j<inSize ; j++ )
    {
      if( inDataFlags[j] == true )
      {
        fprintf(f, "T");
        fprintf(f, "\t%.5lf", sensResult[pos]);
        pos++;
      }
      else
      {
        fprintf(f, "F");
        fprintf(f, "\tNotWorking");
      }
      fprintf(f, "\t%s", GetDataTypeChar( inDataType[j] ));
      fprintf(f, "\t%s", GetDataGroupChar( inDataGroup[j] ));
      fprintf(f, "\n");
    }
    // Fechando o arquivo de relatórios:
    fclose(f);

  }



  /**
  * Construtor. 
  * @param prfSamples Recebe uma instância da classe ImageSamples contendo todas as amostras.
  */
  BKP_Performance( ImageSamples *prfSamples, TrainType prfType, int prfMinHiddenNeurons, int prfMaxHiddenNeurons,
                    int prfMinEntries, int prfRepeat, MatrixType prfMatrixtype )
  {
    // Insere os dados recebidos:
    _prfSamples           = prfSamples;  
    _prfType              = prfType;
    _prfMinHiddenNeurons  = prfMinHiddenNeurons;
    _prfMaxHiddenNeurons  = prfMaxHiddenNeurons;
    _prfMinEntries        = prfMinEntries;
    _prfRepeat            = prfRepeat;
    _prfMatrixtype        = prfMatrixtype;

    // Os Demais dados terão que ser inicializados:
    _prfNet           = NULL;    _prfSenAnalysis   = NULL;
    _prfInPrepFlags   = NULL;    _nSamples         = -1;
    _dimIn_i          = -1;      _dimOut_k         = -1;
    _inSamples        = NULL;    _outSamples       = NULL;
    _inSamplesTypes   = NULL;    _inSamplesGroups  = NULL;
    _outSamplesTypes  = NULL;    _outSamplesGroups = NULL;

    // Obtendo os flags atuais das amostras:
    _prfSamples->smpGetInSamplesFlagsVector( &_prfInPrepFlags );

    // Alocando o vetor auxiliar que irá conter os resultados da análise a cada passo:
    _resultBoolVector = new bool[prfSamples->smpGetInSamplesDimension()];
  }


  /**
  * Funcao que obtem todos os valores da configuracao atual das amostras da instancia da classe ImageSamples
  * contida na classe, e a partir dessas informaçoes configura toda a parte dos dados necessarios para executar
  * uma analise de sensibilidade da rede que ira trabalhar com essa configuracao.
  */
  int prfConfigureData()
  {
    // TODA ALOCAÇÃO E PREENCHIMENTO DE QUALQUER MATRIZ OU VETOR É FEITO DENTRO DA FUNÇÃO smpGetMatrices:

    // Caso as matrizes ou vetores já existam, são deletados:
    if( _inSamples )
      delete _inSamples;
    if( _outSamples )
      delete _outSamples;
    if( _inSamplesTypes )     //< Tipos dos dados das amostras de entrada.
      delete _inSamplesTypes;
    if( _inSamplesGroups )    //< Grupos dos dados das amostras de entrada.
      delete _inSamplesGroups;
    if( _outSamplesTypes )    //< Tipos dos dados das amostras de saída.
      delete _outSamplesTypes;
    if( _outSamplesGroups )   //< Grupos dos dados das amostras de entrada.
      delete _outSamplesGroups;

    // Faz uma chamada à função que realoca a memória necessária, além de inserir todos os valores das amostras 
    // já na configuração atual:
    _prfSamples->smpGetMatrices( &_inSamples, &_inSamplesTypes, &_inSamplesGroups, &_dimIn_i, &_outSamples, &_dimOut_k, &_nSamples );

    return 0;
  }




  /**************************** Funções meio sem motivo: *****************************/
  /**************************** Funções meio sem motivo: *****************************/
  /**************************** Funções meio sem motivo: *****************************/

#if 0
  /**
  * FUNÇÃO UTILIZADA SOMENTE PARA O CASO DE INSERÇÃO DE AMOSTRAS ATRAVÉS DE UMA CLASSE EXTERNA.
  * Cria (ou recria) as matrizes de amostras de entrada e saída. Apenas aloca a memória para as matrizes,
  * deixando que os valores sejam inseridos posteriormente.
  * @param nSamples Número de amostras existentes nas matrizes.
  * @param dimIn_i Dimensão das amostras de entrada.
  * @param dimOut_k Dimensão das amostras de saída.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int prfInsertSamples( int nSamples, int dimIn_i, int dimOut_k )
  {
    // Casos de retorno: matriz nula:
    if( (!inSamples) || (!outSamples) )
      return -1;

    // Inserindo os dados:
    _nSamples     = nSamples;
    _dimIn_i      = dimIn_i;
    _dimOut_k     = dimOut_k;

    // Caso as matrizes já existam, são deletadas e realocadas:
    if( _inSamples != NULL )
      delete _inSamples;
    if( _outSamples != NULL )
      delete _outSamples;

    // Realocando as matrizes:
    _inSamples  = new float*[_nSamples];
    _outSamples = new float*[_nSamples];
    for( int i=0 ; i<_nSamples ; i++ )
    {
      _inSamples[i]  = new float[_dimIn_i];
      _outSamples[i] = new float[_dimOut_k];
    }

    // Realocando os vetores datatype e datagroup:
    if( _inSamplesTypes )     //< Tipos dos dados das amostras de entrada.
      delete _inSamplesTypes;
    _inSamplesTypes = new dataType[_dimIn_i];
    if( _inSamplesGroups )    //< Grupos dos dados das amostras de entrada.
      delete _inSamplesGroups;
    _inSamplesGroups = new dataGroup[_dimIn_i];
    if( _outSamplesTypes )    //< Tipos dos dados das amostras de saída.
      delete _outSamplesTypes;
    _outSamplesTypes = new dataType[_dimOut_k];
    if( _outSamplesGroups )   //< Grupos dos dados das amostras de entrada.
      delete _outSamplesGroups;
    _outSamplesGroups = new dataGroup[_dimOut_k];

    return 0;
  }


  /**
  * FUNÇÃO UTILIZADA SOMENTE PARA O CASO DE INSERÇÃO DE AMOSTRAS ATRAVÉS DE UMA CLASSE EXTERNA.
  * Cria (ou recria) as matrizes de amostras de entrada e saída.
  * @param nSamples Número de amostras existentes nas matrizes.
  * @param dimIn_i Dimensão das amostras de entrada.
  * @param dimOut_k Dimensão das amostras de saída.
  * @param inSamples Nova matriz contendo o conjunto de amostras de entrada.
  * @param outSamples Nova matriz contendo o conjunto de amostras de saída.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int prfInsertSamples( int nSamples, int dimIn_i, int dimOut_k, float** inSamples, float** outSamples,
              dataType* inSamplesTypes, dataGroup* inSamplesGroups, dataType* outSamplesTypes, dataGroup* outSamplesGroups )
  {
    // Aloca as matrizes e insere o número de amostras, a dimensão de entrada e de saída das amostras:
    int ret = prfInsertSamples( nSamples, dimIn_i, dimOut_k );
    if( ret == -1 )
      return -1;

    // Copiando as matrizes:
    if( (inSamples) && (outSamples) )
    {
      for( int i=0 ; i<_nSamples ; i++ )
      {
        for( int j=0 ; j<_dimIn_i ; j++ )
          _inSamples[i][j] = inSamples[i][j];
        for( int j=0 ; j<_dimOut_k ; j++ )
          _outSamples[i][j] = _outSamples[i][j];
      }
    }

    // Copiando tipos e grupos de entrada:
    if( (inSamplesTypes) && (inSamplesGroups) )
    {
      delete _inSamplesTypes;
      delete _inSamplesGroups;
      _inSamplesTypes = new dataType[_dimIn_i];
      _inSamplesGroups = new dataGroup[_dimIn_i];
      for( int j=0 ; j<_dimIn_i ; j++ )
      {
        _inSamplesTypes[j] = inSamplesTypes[j];
        _inSamplesGroups[j] = inSamplesGroups[j];
      }
    }

    // Copiando tipos e grupos de entrada:
    if( (outSamplesTypes) && (outSamplesGroups) )
    {
      delete _outSamplesTypes;
      delete _outSamplesGroups;
      _outSamplesTypes = new dataType[_dimOut_k];
      _outSamplesGroups = new dataGroup[_dimOut_k];
      for( int j=0 ; j<_dimOut_k ; j++ )
      {
        _outSamplesTypes[j] = outSamplesTypes[j];
        _outSamplesGroups[j] = outSamplesGroups[j];
      }
    }
    return 0;
  }
#endif


};


#endif  // BKP_PERFORMANCE