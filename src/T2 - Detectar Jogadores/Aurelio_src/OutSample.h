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

  // Vetores de entradas e saídas:
  float     *_outVector;          ///< Vetor contendo os valores de entrada da amostra.
  dataType  *_outDataType;        ///< Vetor de tipos: qual o tipo de dado em cada posicao do vetor.
  dataGroup *_outDataGroup;       ///< Vetor de tipos: qual o grupo de escalonamento ao qual cada dado pertence.
  int _outNumberOfDays;           ///< Número de dias de medições contidos na amostra.
  int _outSize;                   ///< Dimensão da entrada original.
  float _outReady;                ///< Saída calculada.             
  float _outNaturalValue;         ///< Valor considerado como a saída (utilizado na comparação durante a geração de _outReady).
  dataType  _outNaturalDataType;  ///< tipo: qual o tipo de dado do valor a ser comparado pra gerar a saída.
  dataGroup _outNaturalDataGroup; ///< grupo: qual o grupo de escalonamento do valor a ser comparado pra gerar a saída.


  // Informações a respeito da amostra de entrada relacionada a essa amostra de saída:
  float _inNaturalValue;        ///< Valor da respectiva amostra de entrada utilizada como comparação pela função que gera das saídas.
  dataType _inNaturalType;      ///< Tipo da amostra de entrada utilizada como comparação pela função que gera das saídas.
  dataGroup _inNaturalGroup;    ///< Grupo da amostra de entrada utilizada como comparação pela função que gera das saídas.


  // Variáveis obtidas indiretamente com algum processamento:
  int _dataTypeSize;          ///< Numero de tipos de dados diferentes nas amostras.
  int _dataGroupSize;         ///< Numero de tipos de dados diferentes nas amostras.




  /**
  * Auxiliar. Encontra o número de grupos de escalonamento diferentes na amostra.
  * @return Retorna o número de grupos de escalonamento diferentes na amostra.
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
  * Auxiliar. Encontra o número de tipos de dados diferentes na amostra.
  * @return Retorna o número de tipos de dados diferentes na amostra.
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
  * Insere o valor do dado da amostra de entrada de referência.
  * @param inNaturalValue Valor da amostra de entrada de referência.
  * @param inNaturalType Tipo da amostra de entrada de referência.
  * @param inNaturalGroup Grupo da amostra de entrada de referência.
  */
  void OutSmpSetInNaturalValue( float inNaturalValue, dataType inNaturalType, dataGroup inNaturalGroup )
  {
    _inNaturalValue = inNaturalValue;
    _inNaturalType  = inNaturalType;
    _inNaturalGroup = inNaturalGroup;
  }


  /**
  * Configura o valor do dado a ser utilizado para gerar a saída junto com a amostra de entrada de referência.
  * @param outDay Dia escolhido entre os dias disponíveis na amostra de saída. O primeiro dia da saída é D0 (0).
  * @param outType Tipo da amostra do dia outDay a ser utilizada.
  * @param outGroup Grupo da amostra do dia outDay a ser utilizada.
  * @return Retorna o valor da medição de tipo outType e grupo outGroup contida no dia outDay.
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
  * Imprime o conteúdo das amostras.
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
  * @param outVector Vetor de entrada. Contém uma amostra de saída já formada, com os dados originais.
  * @param outDataType Vetor de tipos indicando qual o tipo de dado em cada posicao do vetor.
  * @param outDataGroup Vetor de tipos indicando qual o grupo de escalonamento ao qual cada dado pertence.
  * @param outSize Dimensão dos vetores de saída recebidos (outVector, outDataFlag, outDataGroup).
  * @param outNumberOfDays Número de dias de medição contidos na amostra.
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

    // Dimensões:
    _outNumberOfDays = outNumberOfDays;
    _outSize  = outSize; 

    // Saída a ser gerada:
    _outReady = 0;

    // Valor inicial da base para comparação utilizada na geração da saída:
    _inNaturalValue = -1;

    // Número de tipos de dados e de grupos de dados existentes:
    _dataTypeSize   = outSmpFindDataTypeSize();
    _dataGroupSize  = outSmpFindNumDataGroupSize();
  }


  /**
  * Construtor.
  * @param outVector Vetor de entrada. Contém uma amostra de saída já formada, com os dados originais.
  * @param outDataType Vetor de tipos indicando qual o tipo de dado em cada posicao do vetor.
  * @param outDataGroup Vetor de tipos indicando qual o grupo de escalonamento ao qual cada dado pertence.
  * @param outSize Dimensão dos vetores de saída recebidos (outVector, outDataFlag, outDataGroup).
  * @param outNumberOfDays Número de dias de medição contidos na amostra.
  * @param inNaturalValue Valor da amostra de entrada de referência.
  * @param inNaturalType Tipo da amostra de entrada de referência.
  * @param inNaturalGroup Grupo da amostra de entrada de referência.
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

    // Dimensões:
    _outNumberOfDays = outNumberOfDays;
    _outSize  = outSize; 

    // Saída a ser gerada:
    _outReady = 0;

    // Informações a respeito da amostra de entrada de referência:
    _inNaturalValue = inNaturalValue;
    _inNaturalType  = inNaturalType;
    _inNaturalGroup = inNaturalGroup;

    // Número de tipos de dados e de grupos de dados existentes:
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
  * Obtém (caso exista) um determinado valor de medição dentro de uma instância da classe.
  * Para isso recebe o índice do dia dessa medição dentro da amostra, além do seu tipo.
  * Os dias possuem índices ordenados da maneira convencional: o primeiro da saída é o D1,
  * o segundo é D2, terceiro D3, ...., Dn.
  * @param mType Tipo da medição desejada.
  * @param mGroup Grupo de normalização ao qual esse dado pertence.
  * @param mDay Dia que contém a medição desejada.
  * @param retValue Valor da medição desejada retornado (caso exista).
  * @return Retorna false em caso de erro (medição inexistente, dia inexistente, etc), ou 
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
  * Calcula a saída dessa amostra segundo o modelo que prevê a saída entre 0.1 e 0.9.
  * @param maintainInterval Intervalo de diferença (percentual) para o qual será considerada a saída 
  * como sendo 0.5.
  * @param percentToMax Diferença percentual máxima a ser utilizada para a escala (em casos de diferenças 
  * maiores que essas, o valor obtido é limitado à essa diferença).
  */
  void OutSmpGenerate( float maintainInterval, float percentToMax )
  {
    // Diferença percentual do dia anterior para o próximo: 
    float porcent = ( (_outNaturalValue - _inNaturalValue) / _inNaturalValue );

    // Limitando a diferença percentual obtida para que ela não ultrapasse maxScale:
    if( porcent > percentToMax  )      porcent = percentToMax;
    if( porcent < -percentToMax )      porcent = -percentToMax;

    /* Caso em que a saída é Interv% ou mais maior que a entrada: compra: */
    if (porcent > maintainInterval)
      _outReady = 0.5f + (porcent*0.4f)/percentToMax;
    /* Caso em que a saída é entre -Interv% e +Interv% em relação à entrada: manutenção: */
    if ((porcent >= -maintainInterval) && (porcent <= maintainInterval))
      _outReady = 0.5f;
    /* Caso em que a saída é Interv% ou mais maior que a entrada: venda: */
    if (porcent < -maintainInterval)
    {
      porcent = -porcent;
      _outReady = 1-(0.5f + (porcent*0.4f)/percentToMax);
    }
  }


  /**
  * Obtém o valor da saída pronta para uso.
  * @return Retorna o valor da saída para utilização.
  */
  float OutSmpGetOutReady()
  {
    return _outReady;
  }

};

#endif // OUTSAMPLE_H