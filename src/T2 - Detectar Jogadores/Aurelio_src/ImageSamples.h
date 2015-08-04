#ifndef IMAGE_SAMPLES_H
#define IMAGE_SAMPLES_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "Natual_Types.h"
#include "InSample.h"


// Enumerador que define o tipo de escalonamento a ser aplicado a um grupo de dados:
typedef enum
{
  MAX_MIN_SCALE=0,  ///< Tipo de escalonamento em que os dados são escalonados linearmente entre min e max.
  REFERENCE_VAL     ///< Os dados são escalonados linearmente a partir de um valor de referência fornecido e uma tolerância máxima.
} scaleType;



/**
* Estrutura de dados para conter amostras obtidas a partir de uma instância de dados.
*/
class ImageSamples
{
public:

  dataType  *_inDataType;         ///< Vetor de tipos: qual o tipo de dado em cada posicao dos vetores (sempre os de entrada).
  dataGroup *_inDataGroup;        ///< Vetor de tipos: qual o grupo de escalonamento ao qual cada dado pertence (sempre os de entrada).
  bool      *_inPrepFlags;        ///< Vetor de flags indica, os índices do vetor original atualmente sendo utilizados.
  InSample  **_inSamples;         ///< Vetor de instâncias da classe InSample contendo as amostras de entrada.
  int _inEachSampleSize;          ///< Dimensão do vetor de entrada da amostra já montado.
  int _inEachSamplePrepSize;      ///< Dimensão do vetor de entrada (de dados preparados da amostra) já montado. 
  int _inDataTypeSize;            ///< Numero de tipos de dados diferentes nas amostras.
  int _inDataGroupSize;           ///< Numero de tipos de dados diferentes nas amostras.
  dataType  *_inDataTypeNoRep;    ///< Armazena todos Tipos diferentes, sem repetição (dimensão _dataTypeSize).
  dataGroup *_inDataGroupNoRep;   ///< Armazena todos Grupos diferentes sem repetição (dimensão _dataGroupSize).
  int _inNumberOfDays;            ///< Número de dias(repetições) contidos nas amostras de entrada.
  float **_outSamples;            ///< Vetor de instâncias da classe OutSample contendo as amostras de saída.
  int _outEachSampleSize;         ///< Dimensão de cada um dos vetores de outSamples.
  int _numberOfSamples;           ///< Número de amostras completas (in/out) disponíveis.


  /**
  * Função auxiliar para encontrar o número de tipos ou grupos existentes, sem repetição.
  * Percorre um vetor até a posição apropriada, verificando se o valor recebido já existe nesse vetor até 
  * aquela posição.
  * @param inVector Vetor de entrada, contendo os valores.
  * @param maxPos Posição onde deve ser finalizada a procura, ou seja, a procura será executada até essa posição.
  * @param value Vator que será procurado.
  * @return Retorna -1 em caso de erro (vetor nulo, etc.), 0 em caso de não ter encontrado, ou 1 caso encontre.
  */
  int smpLookForValue( int* inVector, int maxPos, int value )
  {
    // Caso de retorno (vetor nulo):
    if( !inVector )
      return -1;

    // Caso encontre o valor, retorna 1:
    for( int i=0 ; i<maxPos ; i++ )
    {
      if( inVector[i] == value )
        return 1;
    }

    // Caso não tenha sido encontrado, retorna 0:
    return 0;
  }



  /**
  * Auxiliar. Encontra o número de grupos de escalonamento diferentes na amostra de entrada.
  * @return Retorna o número de grupos de escalonamento diferentes na amostra de entrada.
  */
  int smpFindNumDataGroupSize()
  {
    // Casos de retorno:
    if( _inEachSampleSize <= 1 )
      return -1;
    int dataGroupSize = 0;
    bool flag = false;
    dataGroup tmpData;
    for( int i=0 ; i<_inEachSampleSize ; i++ )
    {
      tmpData = _inDataGroup[i];
      for( int j=0 ; j<i; j++ )
      {
        if( tmpData == _inDataGroup[j] )
          flag = true;
      }
      if( flag == false )
        dataGroupSize++;
      flag = false;
    }
    return dataGroupSize;
  }


  /**
  * Auxiliar. Encontra o número de tipos de dados diferentes na amostra de entrada.
  * @return Retorna o número de tipos de dados diferentes na amostra de entrada.
  */
  int smpFindDataTypeSize()
  {
    // Casos de retorno:
    if( _inEachSampleSize <= 1 )
      return -1;
    int dataTypeSize = 0;
    bool flag = false;
    dataType tmpData;
    for( int i=0 ; i<_inEachSampleSize ; i++ )
    {
      tmpData = _inDataType[i];
      for( int j=0 ; j<i; j++ )
      {
        if( tmpData == _inDataType[j] )
          flag = true;
      }
      if( flag == false )
        dataTypeSize++;
      flag = false;
    }
    return dataTypeSize;
  }



  /**
  * Construtor. Cria a instância e inicializa todas as amostras de entrada e saída.
  * @param inMatrixSamples Matriz contendo todos os vetores de amostras de entrada "in-natura".
  * @param inMatrixTypes  Vetor contendo o tipo de cada medição dos vetores de entrada.
  * @param inMatrixGroups Vetor contendo o grupo de cada medição dos vetores de entrada.
  * @param inSize Dimensão de cada um dos vetores de entrada, bem como dos vetores de tipos e grupos.
  * @param outMatrixSamples Matriz contendo todos os vetores de cada uma das amostras de saída.
  * @param outSize Dimensão de cada uma das amostras de saída.
  * @param nSamples Número de amostras de entrada/saída existentes.
  * @param
  */
  ImageSamples( float **inMatrixSamples, dataType *inMatrixTypes, dataGroup *inMatrixGroups, int inSize, 
    float **outMatrixSamples, int outSize, int nSamples )
  {

    // Retorna no caso de não haverem dados disponíveis:
    if( (!inMatrixSamples) || (!inMatrixTypes) || (!inMatrixGroups) || (!outMatrixSamples) )
      return;

    // Dimensão dos vetores existentes:
    _inEachSampleSize     = inSize;
    _inEachSamplePrepSize = inSize;
    _outEachSampleSize    = outSize;

    // Alocando espaço para os tipos e grupos e inserindo os valores correspondentes:
    _inDataType   = new dataType[inSize];
    _inDataGroup  = new dataGroup[inSize];
    _inPrepFlags  = new bool[inSize];

    // Inserindo os valores de DataTypes e DataGroups das entradas:
    for( int j=0 ; j<inSize ; j++ )
    {
      _inDataType[j]  = inMatrixTypes[j];
      _inDataGroup[j] = inMatrixGroups[j];
      _inPrepFlags[j] = true;   //< No início todos os dados estão disponiveis.
    }

    // Número total de amostras de entrada:
    _numberOfSamples = nSamples;

    // Alocando o vetor de ponteiros de classe InSample:
    _inSamples = new InSample*[_numberOfSamples];

    // Criando cada uma das amostras de entrada:
    for( int i=0 ; i<_numberOfSamples ; i++ )
    {
      // Criando a instância de InSample atual:
      _inSamples[i] = new InSample( inMatrixSamples[i], _inDataType, _inDataGroup, _inEachSampleSize, 1 );
    }

    // Número de tipos de dados e de grupos de escalonamento de dados:
    _inDataTypeSize   = smpFindDataTypeSize();
    _inDataGroupSize  = smpFindNumDataGroupSize();

    int ret;
    // Número de tipos e de grupos de escalonamento de dados sem repetição:
    _inDataGroupNoRep = _inSamples[0]->inSmpGetDataGroupNoRepVector( &ret );
    _inDataTypeNoRep  = _inSamples[0]->inSmpGetDataTypeNoRepVector( &ret );

    // Criando cada uma das amostras de saída:
    _outSamples = new float*[_numberOfSamples];
    for( int i=0 ; i<_numberOfSamples ; i++ )
    {
      _outSamples[i] = new float[_outEachSampleSize];
      for( int j=0 ; j<_outEachSampleSize ; j++ )
        _outSamples[i][j] = outMatrixSamples[i][j];
    }
  }


  /**
  * Destrutor.
  */
  ~ImageSamples()
  {
    delete _inDataType;
    delete _inDataGroup;
    delete _inPrepFlags;
    delete _inSamples;
    delete _inDataTypeNoRep;
    delete _inDataGroupNoRep;
    delete _outSamples;
  }


  /**
  * Função que permite escalonar cada um dos grupos em separado.
  * @param inGroups Vetor contendo cada um dos grupos de escalonamento contidos nas amostras.
  * @param inScales Vetor contendo os respectivos tipos de escalonamento a serem aplicados.
  * @param val_1 Vetor contendo o primeiro parâmetro de escalonamento para o respectivo tipo.
  * @param val_2 Vetor contendo o segundo parâmetro de escalonamento para o respectivo tipo de escalonamento.
  * @param val_3 Vetor contendo o último parâmetro de escalonamento (em caso de MAX_MIN, esse valor é nulo).
  * @param vectorsSize Número de elementos dos vetores acima (todos devem ter a mesma dimensão).
                [ dataGroup    ScaleType      val_1    val_2    val_3  ]
                [ dataGroup    MAX_MIN_SCALE  V_MAX    V_MIN    00000  ]
                [ dataGroup    REFERENCE_VAL  N_REF    P_MAX    M_INT  ]
                [   ...             ...        ...      ...      ...   ]
  * @return Retorna -1 em caso de erro dos parâmetros, vetores nulos, etc, ou 0 caso ok.
  */
  int smpInSampleAllDataScale( dataGroup* inGroups, scaleType *inScales, 
                                float* val_1, float* val_2, float* val_3, int vectorsSize )
  {
    // Casos de retorno (matriz ou vetor de grupos sem repetição nulos):
    if( (!inGroups) || (!_inDataGroupNoRep) || (!inScales) || (!val_1) || (!val_3) || (!val_3) )
      return -1;
    // Caso a matriz possua um número diferente de grupos que o existente, retorna erro:
    if( vectorsSize != _inDataGroupSize )
      return -1;

    // Caso contrário, Verifica se os grupos existentes na matriz são os mesmos do vetor de
    // grupos sem repetição:
    for( int i=0 ; i<vectorsSize ; i++ )
    {
      // Obtém o i-ésimo elemento do vetor de grupos da matriz e verifica se esse grupo está 
      // contido no vetor sem repetição de grupos:
      if( 0 == smpLookForValue( (int*)_inDataGroupNoRep, _inDataGroupSize, (int)inGroups[i]) )
        return -1;
    }

    // Caso a matriz tenha dados consistentes, escalona cada um dos grupos de dados da forma desejada:
    for( int i=0 ; i<vectorsSize ; i++ )
    {
      if( inScales[i] == (int) MAX_MIN_SCALE )
      {
        for( int j=0 ; j<_numberOfSamples ; j++ )
        {
          _inSamples[j]->inSmpScaleSimilarData( inGroups[i], val_1[i], val_2[i] );
          //if( j > 3999 && j < 4050)
            //_inSamples[j]->inSmpPrint();
        }
      }
      if( inScales[i] == (int) REFERENCE_VAL )
      {
        for( int j=0 ; j<_numberOfSamples ; j++ )
        {
          _inSamples[j]->inSmpScaleSimilarData( val_1[i], inGroups[i], val_2[i], val_3[i] );
        }
      }
    }
    //_inSamples[4]->inSmpPrint();


    return 0;
  }


  /**
  * Retorna o número total de amostras de entrada disponiveis.
  * @return Retorna o número total de amostras de entrada.
  */
  int smpSamplesGetNumber()
  {
    return _numberOfSamples;
  }

  /**
  * Retorna a amostra de entrada de índice recebido.
  * @param inSampleNumber Índice da amostra de entrada desejada.
  * @return Retorna a amostra de entrada de índice espedificado, ou NULL em caso de erro.
  */
  InSample* smpInSampleGet( int inSampleNumber )
  {
    if( inSampleNumber >= _numberOfSamples )
      return NULL;
    if( !_inSamples )
      return NULL;
    return _inSamples[inSampleNumber];
  }

  /**
  * Retorna a amostra de saída de índice recebido.
  * @param outSampleNumber Índice da amostra de entrada desejada.
  * @return Retorna a amostra de saída de índice espedificado, ou NULL em caso de erro.
  */
  float* smpOutSampleGet( int outSampleNumber )
  {
    if( outSampleNumber > _numberOfSamples )
      return NULL;
    if( !_outSamples )
      return NULL;
    return _outSamples[outSampleNumber];
  }


  /**
  * Função auxiliar. Apenas imprime (um por linha) os valores de um vetor de float de 
  * dimensão conhecida.
  * @param floatVector Vetor contendo a amostra de saída a ser impressa
  * @param floatVectorSize Dimensão desse vetor.
  */
  bool OutSmpPrint( float *floatVector, int floatVectorSize )
  {
    // Casos retorno:
    if( !floatVector )
      return false;

    printf("OutSample:\n");
    for( int i=0 ; i<floatVectorSize ; i++ )
      printf("Out Value %d: %lf\n", i, floatVector[i]);
    printf("\n\n\n");
    return true;
  }


  /**
  * Imprime os valores de entrada e os respectivos valores de saída de uma determinada amostra.
  * @param SampleNumber Índice da amostra desejada.
  */
  void smpPrint( int SampleNumber )
  {
    InSample  *tmpInSample  = smpInSampleGet( SampleNumber );
    float *tmpOutSample     = smpOutSampleGet( SampleNumber );

    printf("****************************************************************\n");
    printf("Sample number: %d\n", SampleNumber);
    tmpInSample->inSmpPrint();
    OutSmpPrint(tmpOutSample, _outEachSampleSize);
  }


  /**
  * IMPORTANTE VERIFICAR QUE SEMPRE QUE EU DESLIGO/LIGO UMA MEDIÇÃO, SEU VETOR DE TIPOS E GRUPOS TAMBÉM É DESLIGADO.
  * Modifica o valor do flag de uma determinada medição de todas as amostras. Para isso, 
  * recebe o novo valor do flag (true ou false), além do tipo, grupo de normalização e 
  * o dia da medição a ter seu estado modificado. 
  * Os dias possuem índices ordenados da seguinte maneira: o último dia é o dia zero (D0).
  * o dia anterior é D-1, e o anterior a esse é o dia D-2, ... etc, até termos Dn-1.
  * @param inReferenceType Tipo da medição desejada.
  * @param inReferenceGroup Grupo de normalização ao qual esse dado pertence.
  * @param inReferenceDay Dia(repetição) que contém a medição desejada.
  * @param newState Novo estado (ligado=true/desligado=false) da medição desejada.
  * @return Retorna false em caso de erro (medição inexistente, dia inexistente, etc), ou 
  * true caso ok.
  */
  bool smpAllInSmpSetOneMeasurementNewState( dataType inReferenceType, dataGroup inReferenceGroup, int inReferenceDay, bool newState )
  {
    // Para cada uma das amostras existentes:
    for( int i=0 ; i<_numberOfSamples ; i++ )
      if(_inSamples[i]->inSmpSetOneMeasurementNewState( inReferenceType, inReferenceGroup, inReferenceDay, newState ) == false )
        return false;

    // Conta o número de medições ligadas em cada amostra de entrada:
    int numValidValues = smpGetNumInValidValues();
    if( numValidValues == -1 )
      return false;

    // Atualiza a variável que mantém o tamanho das amostras de entrada preparadas:
    _inEachSamplePrepSize = numValidValues;

    // Atualiza também o vetor de DataVectors, DataGroups e flags:
    dataGroup *tmpDataGroup = _inSamples[0]->inSmpGetDataGroupVector();
    dataType  *tmpDataType  = _inSamples[0]->inSmpGetDataTypeVector();
    bool *tmpInPrepFlags    = _inSamples[0]->inSmpGetInPrepFlagsVector();
    for( int i=0 ; i<_inEachSampleSize ; i++ )
    {
      _inDataGroup[i] = tmpDataGroup[i];
      _inDataType[i]  = tmpDataType[i];
      _inPrepFlags[i] = tmpInPrepFlags[i];
    }

    return true;
  }


  /**
  * Recebe um vetor de flags de mesma dimensão das entradas, e reconfigura quais das entradas estão ligadas/desligadas
  * durante a geração das amostras prontas para uso.
  * @param 


  /**
  * Retorna uma matriz contendo somente as strings contendo os nomes de cada uma das medições EM USO.
  * @param retTypeStringMatrix Matrix contendo todas as strings dos tipos de medição das medições ligadas.
  * @param retGroupStringMatrix Matrix contendo todas as strings dos grupos de escalonamento das medições ligadas.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int smpGetInSamplesStrings( char*** retTypeStringMatrix, char*** retGroupStringMatrix )
  {
    char **retTypeStringMatrixtmp = new char*[_inEachSamplePrepSize];
    char **retGroupStringMatrixtmp = new char*[_inEachSamplePrepSize];

    // Cada string será copiada para a matriz:
    for( int i=0, pos=0 ; i<_inEachSampleSize ; i++ )
    {
      if( _inPrepFlags[i] == true )
      {
        retTypeStringMatrixtmp[pos] = GetDataTypeChar( _inDataType[i]);
        retGroupStringMatrixtmp[pos] = GetDataGroupChar( _inDataGroup[i]);
        pos++;
      }
    }
    *retTypeStringMatrix  = retTypeStringMatrixtmp;
    *retGroupStringMatrix = retGroupStringMatrixtmp;

    return 0;
  }


  /**
  * Obtém a dimensão de cada uma das amostras de entrada.
  * @return Retorna a dimensão das amostras de entrada.
  */
  int smpGetInSamplesDimension()
  {
    return _inEachSampleSize;
  }


  /**
  * Obtém a dimensão de cada uma das amostras de entrada preparadas.
  * @return Retorna a dimensão das amostras de entrada preparadas..
  */
  int smpGetInSamplesPrepDimension()
  {
    return _inEachSamplePrepSize;
  }


  /**
  * Obtém a dimensão de cada uma das amostras de saida.
  * @return Retorna a dimensão das amostras de saida.
  */
  int smpGetOutSamplesDimension()
  {
    return _outEachSampleSize;
  }


  /**
  * Obtém o número de valores válidos de uma amostra.
  * @param validMeasurementsSize Número de valores válidos da amostra.
  * @return Retorna o número de valores válidos da amostra.
  */
  int smpGetNumInValidValues()
  {
    // Para cada uma das amostras existentes:
    int validMeasurementsSize = 0;
    InSample  *tmpInSample  = smpInSampleGet( 0 );
    validMeasurementsSize = tmpInSample->inSmpGetNumValidValues();
    for( int i=1 ; i<_numberOfSamples ; i++ )
    {
      tmpInSample  = smpInSampleGet( i );
      int auxNumValidValues = tmpInSample->inSmpGetNumValidValues();
      if( validMeasurementsSize != auxNumValidValues )
        return -1;
    }
    return validMeasurementsSize;
  }


  /**
  * Recebe dois ponteiros para matrizes e cria duas matrizes, uma contendo todas as amostras de 
  * entrada e outra contendo todas as amostras de saída. O número de amostras em cada uma das matrizes,
  * bem como a dimensão tanto da de entrada quanto da de saída é retornado.
  * @param inMatrixSamples Matriz contendo todos os vetores de amostras de entrada já prontos para uso.
  * @param inMatrixTypes  Vetor contendo o tipo de cada medição dos vetores de entrada.
  * @param inMatrixGroups Vetor contendo o grupo de cada medição dos vetores de entrada.
  * @param inSize Dimensão de cada um dos vetores de entrada, bem como dos vetores de tipos e grupos.
  * @param outMatrixSamples Matriz contendo todos os vetores de cada uma das amostras de saída.
  * @param outSize Dimensão de cada uma das amostras de saída.
  * @param nSamples Número de amostras de entrada/saída existentes.
  * @return Retorna false em caso de erro, ou true caso ok.
  */
  bool smpGetMatrices( float ***inMatrixSamplesRet, dataType **inMatrixTypesRet, dataGroup **inMatrixGroupsRet, int *inSizeRet, 
                        float ***outMatrixSamplesRet, int* outSizeRet, int* nSamplesRet )
  {
    float **inMatrixSamplesRet_tmp;
    float **outMatrixSamplesRet_tmp;

    // Número total de amostras:
    int nSamplesRet_tmp = _numberOfSamples;
    // Dimensão das amostras de entrada:
    int inSizeRet_tmp = _inEachSamplePrepSize;
    // Dimensão das amostras de saída:
    int outSizeRet_tmp = _outEachSampleSize;

    // Alocando as matrizes a serem retornadas:
    inMatrixSamplesRet_tmp  = new float*[_numberOfSamples];
    outMatrixSamplesRet_tmp = new float*[_numberOfSamples];

    for( int i=0 ; i<_numberOfSamples ; i++ )
    {
      inMatrixSamplesRet_tmp[i]   = new float[_inEachSamplePrepSize];
      outMatrixSamplesRet_tmp[i]  = new float[_outEachSampleSize];

      InSample  *tmpInSample  = smpInSampleGet( i );
      //tmpInSample->inSmpPrint();
      int retSize;
      tmpInSample->inSmpGetValidValuesVector( inMatrixSamplesRet_tmp[i], &retSize );
      for( int j=0 ; j<_outEachSampleSize ; j++ )
        outMatrixSamplesRet_tmp[i][j] = _outSamples[i][j];

      if( retSize != _inEachSamplePrepSize )
        return false;
    }

    // Obtendo os vetores de tipos e de grupos:
    dataType *inMatrixTypesRet_tmp = new dataType[_inEachSamplePrepSize];
    dataGroup *inMatrixGroupsRet_tmp = new dataGroup[_inEachSamplePrepSize];
    // Preenchendo esses vetores com os tipos e grupos requeridos:
    for( int i=0, pos=0 ; i<_inEachSampleSize ; i++ )
    {
      if( _inPrepFlags[i] == true )
      {
        inMatrixTypesRet_tmp[pos]   = _inDataType[i];
        inMatrixGroupsRet_tmp[pos]  = _inDataGroup[i];
        pos++;
      }
    }

    *inMatrixSamplesRet = inMatrixSamplesRet_tmp;
    *inMatrixTypesRet   = inMatrixTypesRet_tmp;
    *inMatrixGroupsRet  = inMatrixGroupsRet_tmp;
    *inSizeRet    = inSizeRet_tmp;
    *outMatrixSamplesRet = outMatrixSamplesRet_tmp;
    *outSizeRet   = outSizeRet_tmp;
    *nSamplesRet  = nSamplesRet_tmp;

    return true;
  }


  /**
  * Reconfigura o vetor de flags de medições das entradas de todas as amostras de entrada.
  * @param flagsVector Novo vetor de flags das amostras de entrada.
  * @param flagsVectorSize Dimensão do vetor de flags de entrada.
  * @return Retorna -1 em caso de erro (dimensões incompatíveis, vetor nulo, etc), ou 0 caso ok.
  */
  int smpChangeFlags( bool *flagsVector, int flagsVectorSize )
  {
    // Casos de retorno:
    if( (!flagsVector) || (flagsVectorSize!=_inEachSampleSize) )
      return -1;
    // Caso contrário, para cada uma das amostras de entrada, reconfigura cada uma das medições:
    for( int i=0 ; i<_numberOfSamples ; i++ )
    {
      for( int j=0 ; j<_inEachSampleSize ; j++ )
        _inSamples[i]->inSmpChangeOneFlag( flagsVector[j], j );
    }

    // Conta o número de medições ligadas em cada amostra de entrada:
    int numValidValues = smpGetNumInValidValues();
    if( numValidValues == -1 )
      return false;

    // Atualiza a variável que mantém o tamanho das amostras de entrada preparadas:
    _inEachSamplePrepSize = numValidValues;

    // Atualiza também o vetor de DataVectors, DataGroups e flags (é necessário?? somente o vetor de flags muda!!):
    dataGroup *tmpDataGroup = _inSamples[0]->inSmpGetDataGroupVector();
    dataType  *tmpDataType  = _inSamples[0]->inSmpGetDataTypeVector();
    bool *tmpInPrepFlags    = _inSamples[0]->inSmpGetInPrepFlagsVector();
    for( int i=0 ; i<_inEachSampleSize ; i++ )
    {
      _inDataGroup[i] = tmpDataGroup[i];
      _inDataType[i]  = tmpDataType[i];
      _inPrepFlags[i] = tmpInPrepFlags[i];
    }

    return 0;
  }


  /**
  * Liga/desliga a utilização de um determinado índice (valor) de todas as amostras.
  * @param newState Novo estado desse valor (true=ligado, false=desligado).
  * @param pos Índice desse valor dentro da amostra.
  * @return Retorna false em caso de erro (vetor nulo ou de dimensões diferentes), ou true caso ok.
  */
  bool smpChangeOneFlag( bool newState, int pos )
  {
    // Casos de retorno:
    if( (pos < 0) || (pos >= _inEachSampleSize) )
      return false;

    // Caso contrário, reconfigura essa posição em todas as amostras:
    for( int i=0 ; i<_numberOfSamples ; i++ )
      _inSamples[i]->inSmpChangeOneFlag( newState, pos );

    // Conta o número de medições ligadas em cada amostra de entrada:
    int numValidValues = smpGetNumInValidValues();
    if( numValidValues == -1 )
      return false;

    // Atualiza a variável que mantém o tamanho das amostras de entrada preparadas:
    _inEachSamplePrepSize = numValidValues;

    // Atualiza também o vetor de DataVectors, DataGroups e flags (é necessário?? somente o vetor de flags muda!!):
    dataGroup *tmpDataGroup = _inSamples[0]->inSmpGetDataGroupVector();
    dataType  *tmpDataType  = _inSamples[0]->inSmpGetDataTypeVector();
    bool *tmpInPrepFlags    = _inSamples[0]->inSmpGetInPrepFlagsVector();
    for( int i=0 ; i<_inEachSampleSize ; i++ )
    {
      _inDataGroup[i] = tmpDataGroup[i];
      _inDataType[i]  = tmpDataType[i];
      _inPrepFlags[i] = tmpInPrepFlags[i];
    }

    return true;
  }



  /**
  * Recebe dois vetores (de entrada e de saída) e insere todos os valores válidos da amostra nesse vetor.
  * @param retVector Vetor a ser preenchido e retornado.
  * @param vectorSize Dimensão desse vetor. Nessa mesma variável é retornada
  * a dimensão da porção preenchida do vetor pela função.
  * @return Retorna false em caso de erro, ou true caso ok.
  */
  bool smpGetValidValuesVector( int SampleNumber, float *retInVector, int *inVectorSize, 
    float *retOutVector, int *outVectorSize )
  {
    // Casos de retorno:
    if( (!retInVector) || (*inVectorSize < _inEachSampleSize) )
      return false;
    if( (!retOutVector) || (*outVectorSize < 1) )
      return false;

    InSample  *tmpInSample  = smpInSampleGet( SampleNumber );

    tmpInSample->inSmpGetValidValuesVector( retInVector, inVectorSize );
    for( int j=0 ; j<_outEachSampleSize ; j++ )
      retOutVector[j] = _outSamples[SampleNumber][j];

    *inVectorSize   = _inEachSamplePrepSize; 
    *outVectorSize  = _outEachSampleSize;

    return true;
  }


  /**
  * Obtém o Vetor de tipos que não possui repetições, além do número de tipos nele contidos.
  * @param dimRet Dimensão do vetor de dataTypes, retornada.
  * @return Retorna o vetor de tipos de dados contidos na amostra.
  */
  dataType* smpGetDataTypeNoRepVector( int *dimRet )
  {
    *dimRet = _inDataTypeSize;
    return _inDataTypeNoRep;
  }


  /**
  * Obtém o Vetor de tipos que não possui repetições, além do número de tipos nele contidos.
  * @param dimRet Dimensão do vetor de dataGroups, retornada.
  * @return Retorna o vetor de tipos de dados contidos na amostra.
  */
  dataGroup* smpGetDataGroupNoRepVector( int *dimRet )
  {
    *dimRet = _inDataGroupSize;
    return _inDataGroupNoRep;
  }


  /**
  * Obtém uma cópia do vetor de flags (estado atual) das amostras de entrada.
  * @param inFlagsVector Cópia do vetor de flags retornada.
  */
  void smpGetInSamplesFlagsVector( bool** inFlagsVector )
  {
    bool* tmpVector = *inFlagsVector;
    if( tmpVector )
      delete tmpVector;
    tmpVector = new bool[_inEachSampleSize];
    for( int i=0 ; i<_inEachSampleSize ; i++ )
      tmpVector[i] = _inPrepFlags[i];

    // Retornando o vetor de flags das amostras em seu estado atual:
    *inFlagsVector = tmpVector;
  }


  /**
  * Obtém uma cópia do vetor de flags (estado atual) das amostras de entrada.
  * @param inFlagsVector Cópia do vetor de flags retornada.
  */
  void smpGetInSamplesFlagsVector( bool* inFlagsVector )
  {
    bool *tmpVector = inFlagsVector;
    for( int i=0 ; i<_inEachSampleSize ; i++ )
      tmpVector[i] = _inPrepFlags[i];
  }

};


#endif // IMAGE_SAMPLES_H