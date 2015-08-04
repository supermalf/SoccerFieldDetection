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
  MAX_MIN_SCALE=0,  ///< Tipo de escalonamento em que os dados s�o escalonados linearmente entre min e max.
  REFERENCE_VAL     ///< Os dados s�o escalonados linearmente a partir de um valor de refer�ncia fornecido e uma toler�ncia m�xima.
} scaleType;



/**
* Estrutura de dados para conter amostras obtidas a partir de uma inst�ncia de dados.
*/
class ImageSamples
{
public:

  dataType  *_inDataType;         ///< Vetor de tipos: qual o tipo de dado em cada posicao dos vetores (sempre os de entrada).
  dataGroup *_inDataGroup;        ///< Vetor de tipos: qual o grupo de escalonamento ao qual cada dado pertence (sempre os de entrada).
  bool      *_inPrepFlags;        ///< Vetor de flags indica, os �ndices do vetor original atualmente sendo utilizados.
  InSample  **_inSamples;         ///< Vetor de inst�ncias da classe InSample contendo as amostras de entrada.
  int _inEachSampleSize;          ///< Dimens�o do vetor de entrada da amostra j� montado.
  int _inEachSamplePrepSize;      ///< Dimens�o do vetor de entrada (de dados preparados da amostra) j� montado. 
  int _inDataTypeSize;            ///< Numero de tipos de dados diferentes nas amostras.
  int _inDataGroupSize;           ///< Numero de tipos de dados diferentes nas amostras.
  dataType  *_inDataTypeNoRep;    ///< Armazena todos Tipos diferentes, sem repeti��o (dimens�o _dataTypeSize).
  dataGroup *_inDataGroupNoRep;   ///< Armazena todos Grupos diferentes sem repeti��o (dimens�o _dataGroupSize).
  int _inNumberOfDays;            ///< N�mero de dias(repeti��es) contidos nas amostras de entrada.
  float **_outSamples;            ///< Vetor de inst�ncias da classe OutSample contendo as amostras de sa�da.
  int _outEachSampleSize;         ///< Dimens�o de cada um dos vetores de outSamples.
  int _numberOfSamples;           ///< N�mero de amostras completas (in/out) dispon�veis.


  /**
  * Fun��o auxiliar para encontrar o n�mero de tipos ou grupos existentes, sem repeti��o.
  * Percorre um vetor at� a posi��o apropriada, verificando se o valor recebido j� existe nesse vetor at� 
  * aquela posi��o.
  * @param inVector Vetor de entrada, contendo os valores.
  * @param maxPos Posi��o onde deve ser finalizada a procura, ou seja, a procura ser� executada at� essa posi��o.
  * @param value Vator que ser� procurado.
  * @return Retorna -1 em caso de erro (vetor nulo, etc.), 0 em caso de n�o ter encontrado, ou 1 caso encontre.
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

    // Caso n�o tenha sido encontrado, retorna 0:
    return 0;
  }



  /**
  * Auxiliar. Encontra o n�mero de grupos de escalonamento diferentes na amostra de entrada.
  * @return Retorna o n�mero de grupos de escalonamento diferentes na amostra de entrada.
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
  * Auxiliar. Encontra o n�mero de tipos de dados diferentes na amostra de entrada.
  * @return Retorna o n�mero de tipos de dados diferentes na amostra de entrada.
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
  * Construtor. Cria a inst�ncia e inicializa todas as amostras de entrada e sa�da.
  * @param inMatrixSamples Matriz contendo todos os vetores de amostras de entrada "in-natura".
  * @param inMatrixTypes  Vetor contendo o tipo de cada medi��o dos vetores de entrada.
  * @param inMatrixGroups Vetor contendo o grupo de cada medi��o dos vetores de entrada.
  * @param inSize Dimens�o de cada um dos vetores de entrada, bem como dos vetores de tipos e grupos.
  * @param outMatrixSamples Matriz contendo todos os vetores de cada uma das amostras de sa�da.
  * @param outSize Dimens�o de cada uma das amostras de sa�da.
  * @param nSamples N�mero de amostras de entrada/sa�da existentes.
  * @param
  */
  ImageSamples( float **inMatrixSamples, dataType *inMatrixTypes, dataGroup *inMatrixGroups, int inSize, 
    float **outMatrixSamples, int outSize, int nSamples )
  {

    // Retorna no caso de n�o haverem dados dispon�veis:
    if( (!inMatrixSamples) || (!inMatrixTypes) || (!inMatrixGroups) || (!outMatrixSamples) )
      return;

    // Dimens�o dos vetores existentes:
    _inEachSampleSize     = inSize;
    _inEachSamplePrepSize = inSize;
    _outEachSampleSize    = outSize;

    // Alocando espa�o para os tipos e grupos e inserindo os valores correspondentes:
    _inDataType   = new dataType[inSize];
    _inDataGroup  = new dataGroup[inSize];
    _inPrepFlags  = new bool[inSize];

    // Inserindo os valores de DataTypes e DataGroups das entradas:
    for( int j=0 ; j<inSize ; j++ )
    {
      _inDataType[j]  = inMatrixTypes[j];
      _inDataGroup[j] = inMatrixGroups[j];
      _inPrepFlags[j] = true;   //< No in�cio todos os dados est�o disponiveis.
    }

    // N�mero total de amostras de entrada:
    _numberOfSamples = nSamples;

    // Alocando o vetor de ponteiros de classe InSample:
    _inSamples = new InSample*[_numberOfSamples];

    // Criando cada uma das amostras de entrada:
    for( int i=0 ; i<_numberOfSamples ; i++ )
    {
      // Criando a inst�ncia de InSample atual:
      _inSamples[i] = new InSample( inMatrixSamples[i], _inDataType, _inDataGroup, _inEachSampleSize, 1 );
    }

    // N�mero de tipos de dados e de grupos de escalonamento de dados:
    _inDataTypeSize   = smpFindDataTypeSize();
    _inDataGroupSize  = smpFindNumDataGroupSize();

    int ret;
    // N�mero de tipos e de grupos de escalonamento de dados sem repeti��o:
    _inDataGroupNoRep = _inSamples[0]->inSmpGetDataGroupNoRepVector( &ret );
    _inDataTypeNoRep  = _inSamples[0]->inSmpGetDataTypeNoRepVector( &ret );

    // Criando cada uma das amostras de sa�da:
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
  * Fun��o que permite escalonar cada um dos grupos em separado.
  * @param inGroups Vetor contendo cada um dos grupos de escalonamento contidos nas amostras.
  * @param inScales Vetor contendo os respectivos tipos de escalonamento a serem aplicados.
  * @param val_1 Vetor contendo o primeiro par�metro de escalonamento para o respectivo tipo.
  * @param val_2 Vetor contendo o segundo par�metro de escalonamento para o respectivo tipo de escalonamento.
  * @param val_3 Vetor contendo o �ltimo par�metro de escalonamento (em caso de MAX_MIN, esse valor � nulo).
  * @param vectorsSize N�mero de elementos dos vetores acima (todos devem ter a mesma dimens�o).
                [ dataGroup    ScaleType      val_1    val_2    val_3  ]
                [ dataGroup    MAX_MIN_SCALE  V_MAX    V_MIN    00000  ]
                [ dataGroup    REFERENCE_VAL  N_REF    P_MAX    M_INT  ]
                [   ...             ...        ...      ...      ...   ]
  * @return Retorna -1 em caso de erro dos par�metros, vetores nulos, etc, ou 0 caso ok.
  */
  int smpInSampleAllDataScale( dataGroup* inGroups, scaleType *inScales, 
                                float* val_1, float* val_2, float* val_3, int vectorsSize )
  {
    // Casos de retorno (matriz ou vetor de grupos sem repeti��o nulos):
    if( (!inGroups) || (!_inDataGroupNoRep) || (!inScales) || (!val_1) || (!val_3) || (!val_3) )
      return -1;
    // Caso a matriz possua um n�mero diferente de grupos que o existente, retorna erro:
    if( vectorsSize != _inDataGroupSize )
      return -1;

    // Caso contr�rio, Verifica se os grupos existentes na matriz s�o os mesmos do vetor de
    // grupos sem repeti��o:
    for( int i=0 ; i<vectorsSize ; i++ )
    {
      // Obt�m o i-�simo elemento do vetor de grupos da matriz e verifica se esse grupo est� 
      // contido no vetor sem repeti��o de grupos:
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
  * Retorna o n�mero total de amostras de entrada disponiveis.
  * @return Retorna o n�mero total de amostras de entrada.
  */
  int smpSamplesGetNumber()
  {
    return _numberOfSamples;
  }

  /**
  * Retorna a amostra de entrada de �ndice recebido.
  * @param inSampleNumber �ndice da amostra de entrada desejada.
  * @return Retorna a amostra de entrada de �ndice espedificado, ou NULL em caso de erro.
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
  * Retorna a amostra de sa�da de �ndice recebido.
  * @param outSampleNumber �ndice da amostra de entrada desejada.
  * @return Retorna a amostra de sa�da de �ndice espedificado, ou NULL em caso de erro.
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
  * Fun��o auxiliar. Apenas imprime (um por linha) os valores de um vetor de float de 
  * dimens�o conhecida.
  * @param floatVector Vetor contendo a amostra de sa�da a ser impressa
  * @param floatVectorSize Dimens�o desse vetor.
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
  * Imprime os valores de entrada e os respectivos valores de sa�da de uma determinada amostra.
  * @param SampleNumber �ndice da amostra desejada.
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
  * IMPORTANTE VERIFICAR QUE SEMPRE QUE EU DESLIGO/LIGO UMA MEDI��O, SEU VETOR DE TIPOS E GRUPOS TAMB�M � DESLIGADO.
  * Modifica o valor do flag de uma determinada medi��o de todas as amostras. Para isso, 
  * recebe o novo valor do flag (true ou false), al�m do tipo, grupo de normaliza��o e 
  * o dia da medi��o a ter seu estado modificado. 
  * Os dias possuem �ndices ordenados da seguinte maneira: o �ltimo dia � o dia zero (D0).
  * o dia anterior � D-1, e o anterior a esse � o dia D-2, ... etc, at� termos Dn-1.
  * @param inReferenceType Tipo da medi��o desejada.
  * @param inReferenceGroup Grupo de normaliza��o ao qual esse dado pertence.
  * @param inReferenceDay Dia(repeti��o) que cont�m a medi��o desejada.
  * @param newState Novo estado (ligado=true/desligado=false) da medi��o desejada.
  * @return Retorna false em caso de erro (medi��o inexistente, dia inexistente, etc), ou 
  * true caso ok.
  */
  bool smpAllInSmpSetOneMeasurementNewState( dataType inReferenceType, dataGroup inReferenceGroup, int inReferenceDay, bool newState )
  {
    // Para cada uma das amostras existentes:
    for( int i=0 ; i<_numberOfSamples ; i++ )
      if(_inSamples[i]->inSmpSetOneMeasurementNewState( inReferenceType, inReferenceGroup, inReferenceDay, newState ) == false )
        return false;

    // Conta o n�mero de medi��es ligadas em cada amostra de entrada:
    int numValidValues = smpGetNumInValidValues();
    if( numValidValues == -1 )
      return false;

    // Atualiza a vari�vel que mant�m o tamanho das amostras de entrada preparadas:
    _inEachSamplePrepSize = numValidValues;

    // Atualiza tamb�m o vetor de DataVectors, DataGroups e flags:
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
  * Recebe um vetor de flags de mesma dimens�o das entradas, e reconfigura quais das entradas est�o ligadas/desligadas
  * durante a gera��o das amostras prontas para uso.
  * @param 


  /**
  * Retorna uma matriz contendo somente as strings contendo os nomes de cada uma das medi��es EM USO.
  * @param retTypeStringMatrix Matrix contendo todas as strings dos tipos de medi��o das medi��es ligadas.
  * @param retGroupStringMatrix Matrix contendo todas as strings dos grupos de escalonamento das medi��es ligadas.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int smpGetInSamplesStrings( char*** retTypeStringMatrix, char*** retGroupStringMatrix )
  {
    char **retTypeStringMatrixtmp = new char*[_inEachSamplePrepSize];
    char **retGroupStringMatrixtmp = new char*[_inEachSamplePrepSize];

    // Cada string ser� copiada para a matriz:
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
  * Obt�m a dimens�o de cada uma das amostras de entrada.
  * @return Retorna a dimens�o das amostras de entrada.
  */
  int smpGetInSamplesDimension()
  {
    return _inEachSampleSize;
  }


  /**
  * Obt�m a dimens�o de cada uma das amostras de entrada preparadas.
  * @return Retorna a dimens�o das amostras de entrada preparadas..
  */
  int smpGetInSamplesPrepDimension()
  {
    return _inEachSamplePrepSize;
  }


  /**
  * Obt�m a dimens�o de cada uma das amostras de saida.
  * @return Retorna a dimens�o das amostras de saida.
  */
  int smpGetOutSamplesDimension()
  {
    return _outEachSampleSize;
  }


  /**
  * Obt�m o n�mero de valores v�lidos de uma amostra.
  * @param validMeasurementsSize N�mero de valores v�lidos da amostra.
  * @return Retorna o n�mero de valores v�lidos da amostra.
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
  * entrada e outra contendo todas as amostras de sa�da. O n�mero de amostras em cada uma das matrizes,
  * bem como a dimens�o tanto da de entrada quanto da de sa�da � retornado.
  * @param inMatrixSamples Matriz contendo todos os vetores de amostras de entrada j� prontos para uso.
  * @param inMatrixTypes  Vetor contendo o tipo de cada medi��o dos vetores de entrada.
  * @param inMatrixGroups Vetor contendo o grupo de cada medi��o dos vetores de entrada.
  * @param inSize Dimens�o de cada um dos vetores de entrada, bem como dos vetores de tipos e grupos.
  * @param outMatrixSamples Matriz contendo todos os vetores de cada uma das amostras de sa�da.
  * @param outSize Dimens�o de cada uma das amostras de sa�da.
  * @param nSamples N�mero de amostras de entrada/sa�da existentes.
  * @return Retorna false em caso de erro, ou true caso ok.
  */
  bool smpGetMatrices( float ***inMatrixSamplesRet, dataType **inMatrixTypesRet, dataGroup **inMatrixGroupsRet, int *inSizeRet, 
                        float ***outMatrixSamplesRet, int* outSizeRet, int* nSamplesRet )
  {
    float **inMatrixSamplesRet_tmp;
    float **outMatrixSamplesRet_tmp;

    // N�mero total de amostras:
    int nSamplesRet_tmp = _numberOfSamples;
    // Dimens�o das amostras de entrada:
    int inSizeRet_tmp = _inEachSamplePrepSize;
    // Dimens�o das amostras de sa�da:
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
  * Reconfigura o vetor de flags de medi��es das entradas de todas as amostras de entrada.
  * @param flagsVector Novo vetor de flags das amostras de entrada.
  * @param flagsVectorSize Dimens�o do vetor de flags de entrada.
  * @return Retorna -1 em caso de erro (dimens�es incompat�veis, vetor nulo, etc), ou 0 caso ok.
  */
  int smpChangeFlags( bool *flagsVector, int flagsVectorSize )
  {
    // Casos de retorno:
    if( (!flagsVector) || (flagsVectorSize!=_inEachSampleSize) )
      return -1;
    // Caso contr�rio, para cada uma das amostras de entrada, reconfigura cada uma das medi��es:
    for( int i=0 ; i<_numberOfSamples ; i++ )
    {
      for( int j=0 ; j<_inEachSampleSize ; j++ )
        _inSamples[i]->inSmpChangeOneFlag( flagsVector[j], j );
    }

    // Conta o n�mero de medi��es ligadas em cada amostra de entrada:
    int numValidValues = smpGetNumInValidValues();
    if( numValidValues == -1 )
      return false;

    // Atualiza a vari�vel que mant�m o tamanho das amostras de entrada preparadas:
    _inEachSamplePrepSize = numValidValues;

    // Atualiza tamb�m o vetor de DataVectors, DataGroups e flags (� necess�rio?? somente o vetor de flags muda!!):
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
  * Liga/desliga a utiliza��o de um determinado �ndice (valor) de todas as amostras.
  * @param newState Novo estado desse valor (true=ligado, false=desligado).
  * @param pos �ndice desse valor dentro da amostra.
  * @return Retorna false em caso de erro (vetor nulo ou de dimens�es diferentes), ou true caso ok.
  */
  bool smpChangeOneFlag( bool newState, int pos )
  {
    // Casos de retorno:
    if( (pos < 0) || (pos >= _inEachSampleSize) )
      return false;

    // Caso contr�rio, reconfigura essa posi��o em todas as amostras:
    for( int i=0 ; i<_numberOfSamples ; i++ )
      _inSamples[i]->inSmpChangeOneFlag( newState, pos );

    // Conta o n�mero de medi��es ligadas em cada amostra de entrada:
    int numValidValues = smpGetNumInValidValues();
    if( numValidValues == -1 )
      return false;

    // Atualiza a vari�vel que mant�m o tamanho das amostras de entrada preparadas:
    _inEachSamplePrepSize = numValidValues;

    // Atualiza tamb�m o vetor de DataVectors, DataGroups e flags (� necess�rio?? somente o vetor de flags muda!!):
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
  * Recebe dois vetores (de entrada e de sa�da) e insere todos os valores v�lidos da amostra nesse vetor.
  * @param retVector Vetor a ser preenchido e retornado.
  * @param vectorSize Dimens�o desse vetor. Nessa mesma vari�vel � retornada
  * a dimens�o da por��o preenchida do vetor pela fun��o.
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
  * Obt�m o Vetor de tipos que n�o possui repeti��es, al�m do n�mero de tipos nele contidos.
  * @param dimRet Dimens�o do vetor de dataTypes, retornada.
  * @return Retorna o vetor de tipos de dados contidos na amostra.
  */
  dataType* smpGetDataTypeNoRepVector( int *dimRet )
  {
    *dimRet = _inDataTypeSize;
    return _inDataTypeNoRep;
  }


  /**
  * Obt�m o Vetor de tipos que n�o possui repeti��es, al�m do n�mero de tipos nele contidos.
  * @param dimRet Dimens�o do vetor de dataGroups, retornada.
  * @return Retorna o vetor de tipos de dados contidos na amostra.
  */
  dataGroup* smpGetDataGroupNoRepVector( int *dimRet )
  {
    *dimRet = _inDataGroupSize;
    return _inDataGroupNoRep;
  }


  /**
  * Obt�m uma c�pia do vetor de flags (estado atual) das amostras de entrada.
  * @param inFlagsVector C�pia do vetor de flags retornada.
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
  * Obt�m uma c�pia do vetor de flags (estado atual) das amostras de entrada.
  * @param inFlagsVector C�pia do vetor de flags retornada.
  */
  void smpGetInSamplesFlagsVector( bool* inFlagsVector )
  {
    bool *tmpVector = inFlagsVector;
    for( int i=0 ; i<_inEachSampleSize ; i++ )
      tmpVector[i] = _inPrepFlags[i];
  }

};


#endif // IMAGE_SAMPLES_H