#ifndef OUTSAMPLE_H
#define OUTSAMPLE_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "Natual_Types.h"

#define MAX_VALUE  1e12
#define MIN_VALUE -1e12


// Malandragem para imprimir o enum como string:
#define GET_ENUM_NAME(bla) #bla



class OutSample
{
public:

  // Vetores de entradas e sa�das:
  float     *_outVector;          ///< Vetor contendo os valores de entrada da amostra.
  dataType  *_outDataType;        ///< Vetor de tipos: qual o tipo de dado em cada posicao do vetor.
  dataGroup *_outDataGroup;       ///< Vetor de tipos: qual o grupo de escalonamento ao qual cada dado pertence.
  int _outNumberOfDays;           ///< N�mero de dias de medi��es contidos na amostra.
  int _outSize;                   ///< Dimens�o da entrada original.
  float _outReady;                ///< Sa�da calculada.             
  float _outNaturalValue;         ///< Valor considerado como a sa�da (utilizado na compara��o durante a gera��o de _outReady).
  dataType  _outNaturalDataType;  ///< tipo: qual o tipo de dado do valor a ser comparado pra gerar a sa�da.
  dataGroup _outNaturalDataGroup; ///< grupo: qual o grupo de escalonamento do valor a ser comparado pra gerar a sa�da.


  // Informa��es a respeito da amostra de entrada relacionada a essa amostra de sa�da:
  float _inNaturalValue;        ///< Valor da respectiva amostra de entrada utilizada como compara��o pela fun��o que gera das sa�das.
  dataType _inNaturalType;      ///< Tipo da amostra de entrada utilizada como compara��o pela fun��o que gera das sa�das.
  dataGroup _inNaturalGroup;    ///< Grupo da amostra de entrada utilizada como compara��o pela fun��o que gera das sa�das.


  // Vari�veis obtidas indiretamente com algum processamento:
  int _dataTypeSize;          ///< Numero de tipos de dados diferentes nas amostras.
  int _dataGroupSize;         ///< Numero de tipos de dados diferentes nas amostras.




  /**
  * Auxiliar. Encontra o n�mero de grupos de escalonamento diferentes na amostra.
  * @return Retorna o n�mero de grupos de escalonamento diferentes na amostra.
  */
  int outSmpFindNumDataGroupSize()
  {
    // Casos de retorno:
    if( _outSize <= 1 )
      return -1;
    int dataGroupSize = 0;
    bool flag = false;
    dataGroup tmpData;
    for( int i=0 ; i<_outSize ; i++ )
    {
      tmpData = _outDataGroup[i];
      for( int j=0 ; j<i; j++ )
      {
        if( tmpData == _outDataGroup[j] )
          flag = true;
      }
      if( flag == false )
        dataGroupSize++;
      flag = false;
    }
    return dataGroupSize;
  }


  /**
  * Auxiliar. Encontra o n�mero de tipos de dados diferentes na amostra.
  * @return Retorna o n�mero de tipos de dados diferentes na amostra.
  */
  int outSmpFindDataTypeSize()
  {
    // Casos de retorno:
    if( _outSize <= 1 )
      return -1;
    int dataTypeSize = 0;
    bool flag = false;
    dataType tmpData;
    for( int i=0 ; i<_outSize ; i++ )
    {
      tmpData = _outDataType[i];
      for( int j=0 ; j<i; j++ )
      {
        if( tmpData == _outDataType[j] )
          flag = true;
      }
      if( flag == false )
        dataTypeSize++;
      flag = false;
    }
    return dataTypeSize;
  }


  /**
  * Insere o valor do dado da amostra de entrada de refer�ncia.
  * @param inNaturalValue Valor da amostra de entrada de refer�ncia.
  * @param inNaturalType Tipo da amostra de entrada de refer�ncia.
  * @param inNaturalGroup Grupo da amostra de entrada de refer�ncia.
  */
  void OutSmpSetInNaturalValue( float inNaturalValue, dataType inNaturalType, dataGroup inNaturalGroup )
  {
    _inNaturalValue = inNaturalValue;
    _inNaturalType  = inNaturalType;
    _inNaturalGroup = inNaturalGroup;
  }


  /**
  * Configura o valor do dado a ser utilizado para gerar a sa�da junto com a amostra de entrada de refer�ncia.
  * @param outDay Dia escolhido entre os dias dispon�veis na amostra de sa�da. O primeiro dia da sa�da � D0 (0).
  * @param outType Tipo da amostra do dia outDay a ser utilizada.
  * @param outGroup Grupo da amostra do dia outDay a ser utilizada.
  * @return Retorna o valor da medi��o de tipo outType e grupo outGroup contida no dia outDay.
  */
  float OutSmpSetOutNaturalValue( int outDay, dataType outType, dataGroup outGroup )
  {
    if( _outNumberOfDays <= outDay )
      return -1;

    for( int i=0, j=0 ; i<_outSize ; i++ )
    {
      if( (_outDataGroup[i] == outGroup) && (_outDataType[i] == outType) && (j != outDay) )
        j++;
      if( (_outDataGroup[i] == outGroup) && (_outDataType[i] == outType) && (j == outDay) )
      {
        _outNaturalValue = _outVector[i];
        _outNaturalDataType = outType;
        _outNaturalDataGroup = outGroup;
        return _outNaturalValue;
      }
    }
    return -1;
  }


  /**
  * Imprime o conte�do das amostras.
  */
  void OutSmpPrint()
  {
    printf("OutSample:\n");
    printf("\t%s", GetDataTypeChar( _outNaturalDataType ));
    printf("\t%s", GetDataGroupChar( _outNaturalDataGroup ));
    printf("\t%.2lf, %.2lf", _outNaturalValue, _outReady );
    printf("\n\n\n");
  }


  /**
  * Construtor.
  * @param outVector Vetor de entrada. Cont�m uma amostra de sa�da j� formada, com os dados originais.
  * @param outDataType Vetor de tipos indicando qual o tipo de dado em cada posicao do vetor.
  * @param outDataGroup Vetor de tipos indicando qual o grupo de escalonamento ao qual cada dado pertence.
  * @param outSize Dimens�o dos vetores de sa�da recebidos (outVector, outDataFlag, outDataGroup).
  * @param outNumberOfDays N�mero de dias de medi��o contidos na amostra.
  */
  OutSample(float *outVector, dataType *outDataType, dataGroup *outDataGroup, int outSize, int outNumberOfDays )
  {
    _outVector         = new float    [outSize];
    _outDataType       = new dataType  [outSize];
    _outDataGroup      = new dataGroup [outSize];

    // Inserindo os valores nos vetores:
    for( int i=0 ; i<outSize ; i++ )
    {
      _outVector[i]    = outVector[i];
      _outDataType[i]  = outDataType[i];
      _outDataGroup[i] = outDataGroup[i];
    }

    // Dimens�es:
    _outNumberOfDays = outNumberOfDays;
    _outSize  = outSize; 

    // Sa�da a ser gerada:
    _outReady = 0;

    // Valor inicial da base para compara��o utilizada na gera��o da sa�da:
    _inNaturalValue = -1;

    // N�mero de tipos de dados e de grupos de dados existentes:
    _dataTypeSize   = outSmpFindDataTypeSize();
    _dataGroupSize  = outSmpFindNumDataGroupSize();
  }


  /**
  * Construtor.
  * @param outVector Vetor de entrada. Cont�m uma amostra de sa�da j� formada, com os dados originais.
  * @param outDataType Vetor de tipos indicando qual o tipo de dado em cada posicao do vetor.
  * @param outDataGroup Vetor de tipos indicando qual o grupo de escalonamento ao qual cada dado pertence.
  * @param outSize Dimens�o dos vetores de sa�da recebidos (outVector, outDataFlag, outDataGroup).
  * @param outNumberOfDays N�mero de dias de medi��o contidos na amostra.
  * @param inNaturalValue Valor da amostra de entrada de refer�ncia.
  * @param inNaturalType Tipo da amostra de entrada de refer�ncia.
  * @param inNaturalGroup Grupo da amostra de entrada de refer�ncia.
  */
  OutSample(float *outVector, dataType *outDataType, dataGroup *outDataGroup, int outSize, int outNumberOfDays,
    float inNaturalValue, dataType inNaturalType, dataGroup inNaturalGroup )
  {
    _outVector         = new float    [outSize];
    _outDataType       = new dataType  [outSize];
    _outDataGroup      = new dataGroup [outSize];

    // Inserindo os valores nos vetores:
    for( int i=0 ; i<outSize ; i++ )
    {
      _outVector[i]    = outVector[i];
      _outDataType[i]  = outDataType[i];
      _outDataGroup[i] = outDataGroup[i];
    }

    // Dimens�es:
    _outNumberOfDays = outNumberOfDays;
    _outSize  = outSize; 

    // Sa�da a ser gerada:
    _outReady = 0;

    // Informa��es a respeito da amostra de entrada de refer�ncia:
    _inNaturalValue = inNaturalValue;
    _inNaturalType  = inNaturalType;
    _inNaturalGroup = inNaturalGroup;

    // N�mero de tipos de dados e de grupos de dados existentes:
    _dataTypeSize   = outSmpFindDataTypeSize();
    _dataGroupSize  = outSmpFindNumDataGroupSize();
  }


  /**
  * Destrutor.
  */
  ~OutSample()
  {
    delete _outVector;
    delete _outDataType;
    delete _outDataGroup;
  }


  /**
  * Obt�m (caso exista) um determinado valor de medi��o dentro de uma inst�ncia da classe.
  * Para isso recebe o �ndice do dia dessa medi��o dentro da amostra, al�m do seu tipo.
  * Os dias possuem �ndices ordenados da maneira convencional: o primeiro da sa�da � o D1,
  * o segundo � D2, terceiro D3, ...., Dn.
  * @param mType Tipo da medi��o desejada.
  * @param mGroup Grupo de normaliza��o ao qual esse dado pertence.
  * @param mDay Dia que cont�m a medi��o desejada.
  * @param retValue Valor da medi��o desejada retornado (caso exista).
  * @return Retorna false em caso de erro (medi��o inexistente, dia inexistente, etc), ou 
  * true caso ok.
  */
  bool OutSmpGetOneMeasurement( dataType mType, dataGroup mGroup, int mDay, float *retValue )
  {
    // Casos de retorno:
    if( mDay > _outNumberOfDays )
      return false;
    int convertedDay = mDay-1;

    int i=0; int j=0;
    while( i < _outSize )
    {
      if( (_outDataType[i] == mType) && (_outDataGroup[i] == mGroup) && (j == convertedDay) )
      {
        *retValue = _outVector[i];
        return true;
      }
      if( (_outDataType[i] == mType) && (_outDataGroup[i] == mGroup) && (j != convertedDay) )
        j++;
      i++;
    }

    *retValue = NULL;
    return false;
  }


  /**
  * Calcula a sa�da dessa amostra segundo o modelo que prev� a sa�da entre 0.1 e 0.9.
  * @param maintainInterval Intervalo de diferen�a (percentual) para o qual ser� considerada a sa�da 
  * como sendo 0.5.
  * @param percentToMax Diferen�a percentual m�xima a ser utilizada para a escala (em casos de diferen�as 
  * maiores que essas, o valor obtido � limitado � essa diferen�a).
  */
  void OutSmpGenerate( float maintainInterval, float percentToMax )
  {
    // Diferen�a percentual do dia anterior para o pr�ximo: 
    float porcent = ( (_outNaturalValue - _inNaturalValue) / _inNaturalValue );

    // Limitando a diferen�a percentual obtida para que ela n�o ultrapasse maxScale:
    if( porcent > percentToMax  )      porcent = percentToMax;
    if( porcent < -percentToMax )      porcent = -percentToMax;

    /* Caso em que a sa�da � Interv% ou mais maior que a entrada: compra: */
    if (porcent > maintainInterval)
      _outReady = 0.5f + (porcent*0.4f)/percentToMax;
    /* Caso em que a sa�da � entre -Interv% e +Interv% em rela��o � entrada: manuten��o: */
    if ((porcent >= -maintainInterval) && (porcent <= maintainInterval))
      _outReady = 0.5f;
    /* Caso em que a sa�da � Interv% ou mais maior que a entrada: venda: */
    if (porcent < -maintainInterval)
    {
      porcent = -porcent;
      _outReady = 1-(0.5f + (porcent*0.4f)/percentToMax);
    }
  }


  /**
  * Obt�m o valor da sa�da pronta para uso.
  * @return Retorna o valor da sa�da para utiliza��o.
  */
  float OutSmpGetOutReady()
  {
    return _outReady;
  }

};

#endif // OUTSAMPLE_H