#ifndef INSAMPLE_H
#define INSAMPLE_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "Natual_Types.h"

#define MAX_VALUE  1e12
#define MIN_VALUE -1e12


// Malandragem para imprimir o enum como string:
#define GET_ENUM_NAME(bla) #bla


class InSample
{
public:

  // Vetores de entradas e sa�das:
  float    *_inVector;          ///< Vetor contendo os valores de entrada da amostra.
  float    *_inPrepVector;      ///< Vetor contendo os valores de entrada da amostra j� preparados.
  dataType  *_inDataType;       ///< Vetor de tipos: qual o tipo de dado em cada posicao do vetor.
  dataGroup *_inDataGroup;      ///< Vetor de tipos: qual o grupo de escalonamento ao qual cada dado pertence.
  bool      *_inPrepFlags;      ///< Vetor de flags indica, os �ndices do vetor original atualmente sendo utilizados.
  int _inNumberOfDays;          ///< N�mero de dias de medi��es contidos na amostra.
  int _inSize;                  ///< Dimens�o da entrada original.
  int _inPrepSize;              ///< Dimens�o da entrada dos dados preparados.

  // Vari�veis obtidas indiretamente com algum processamento:
  int _dataTypeSize;            ///< Numero de tipos de dados diferentes nas amostras.
  int _dataGroupSize;           ///< Numero de tipos de dados diferentes nas amostras.
  dataType  *_inDataTypeNoRep;  ///< Armazena todos Tipos diferentes, sem repeti��o (dimens�o _dataTypeSize).
  dataGroup *_inDataGroupNoRep; ///< Armazena todos Grupos diferentes sem repeti��o (dimens�o _dataGroupSize).


  // Vari�veis e vetores auxiliares (utilizados temporariamente durante o processamento):
  float *_auxPrepInVector;      //< Armazena valores tempor�rios durante o escalonamento.
  int _PrepInVectorActualSize;  //< Por��o do vetor sendo utilizada durante o escalonamento.
  int _auxAlreadyUsedPos;       //< Posi��o m�xima sendo utilizada no vetor tempor�rio.

  // Vari�veis criadas para manter a sa�da j� pronta pra uso:
  float *_tmpSampleVector;      //< Vetor que cont�m a �ltima amostra preparada gerada como sa�da.
  int _tmpSampleVectorSize;     //< Dimens�o atual do vetor que cont�m a �ltima amostra preparada gerada como sa�da.



  /**
  * Fun��o auxiliar para encontrar o n�mero de tipos ou grupos existentes, sem repeti��o.
  * Percorre um vetor at� a posi��o apropriada, verificando se o valor recebido j� existe nesse vetor at� 
  * aquela posi��o.
  * @param inVector Vetor de entrada, contendo os valores.
  * @param maxPos Posi��o onde deve ser finalizada a procura, ou seja, a procura ser� executada at� essa posi��o.
  * @param value Vator que ser� procurado.
  * @return Retorna -1 em caso de erro (vetor nulo, etc.), 0 em caso de n�o ter encontrado, ou 1 caso encontre.
  */
  int inSmpLookForValue( int* inVector, int maxPos, int value )
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
  * Auxiliar. Inicializa todas as componentes de um vetor com o valor recebido.
  * @param inVector Vetor a ter seus valores inicializados.
  * @param size Posi��o at� a qual os valores ser�o inicializados (no m�ximo a dimens�o do vetor).
  * @param value Valor de inicializa��o a ser aplicado.
  */
  void inSmpInitVector( int *inVector, int size, int value )
  {
    if( !inVector )
      return;
    for( int i=0 ; i<size ; i++ )
      inVector[i] = value;
  }


  /**
  * Imprime o conte�do das amostras.
  */
  void inSmpPrint()
  {
    printf("InSample:\n");
    for( int j=0 ; j<_inSize ; j++ )
    {
      printf("\t%s", GetDataTypeChar( _inDataType[j] ));
      printf("\t%s", GetDataGroupChar( _inDataGroup[j] ));
      //printf("\t%.2lf", _inVector[j] );
      printf("\t%.2lf, %.2lf", _inVector[j], _inPrepVector[j]);
      if( _inPrepFlags[j] == true )
        printf("\t\tT\n");
      else
        printf("\t\t\tF\n");
    }
    printf("\n\n\n");
  }


  /**
  * Auxiliar. Encontra o n�mero de grupos de escalonamento diferentes na amostra.
  * @return Retorna o n�mero de grupos de escalonamento diferentes na amostra..
  */
  int inSmpFindNumDataGroupSize()
  {
    // Casos de retorno:
    if( _inSize <= 1 )
      return -1;
    int dataGroupSize = 0;
    bool flag = false;
    dataGroup tmpData;
    for( int i=0 ; i<_inSize ; i++ )
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
  * Auxiliar. Encontra o n�mero de tipos de dados diferentes na amostra.
  * @return Retorna o n�mero de tipos de dados diferentes na amostra.
  */
  int inSmpFindDataTypeSize()
  {
    // Casos de retorno:
    if( _inSize <= 1 )
      return -1;
    int dataTypeSize = 0;
    bool flag = false;
    dataType tmpData;
    for( int i=0 ; i<_inSize ; i++ )
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
  * Construtor.
  * @param inVector Vetor de entrada. Cont�m uma amostra de entrada j� formada, com os dados originais.
  * @param inDataType Vetor de tipos indicando qual o tipo de dado em cada posicao do vetor.
  * @param inDataGroup Vetor de tipos indicando qual o grupo de escalonamento ao qual cada dado pertence.
  * @param inSize Dimens�o dos vetores de entrada recebidos (inVector, inDataFlag, inDataGroup).
  * @param inNumberOfDays N�mero de dias de medi��o contidos na amostra.
  */
  InSample(float *inVector, dataType *inDataType, dataGroup *inDataGroup, int inSize, int inNumberOfDays )
  {
    _inVector         = new float    [inSize];
    _inPrepVector     = new float    [inSize];
    _inDataType       = new dataType  [inSize];
    _inDataGroup      = new dataGroup [inSize];
    _inPrepFlags      = new bool      [inSize];
    // Valores iniciais das vari�veis tempor�rias:
    _auxPrepInVector  = new float    [inSize];
    _tmpSampleVector  = new float    [inSize];
    _auxAlreadyUsedPos      = 0;
    _PrepInVectorActualSize = 0;
    _tmpSampleVectorSize    = 0;

    // Inserindo os valores nos vetores:
    for( int i=0 ; i<inSize ; i++ )
    {
      _inVector[i]    = inVector[i];
      _inDataType[i]  = inDataType[i];
      _inDataGroup[i] = inDataGroup[i];
      _inPrepFlags[i] = true;           //< Todos os valores come�am sendo utilizados.
      _tmpSampleVector[i] = -1;         //< Valor dummy.
    }

    // Dimens�es:
    _inNumberOfDays = inNumberOfDays;
    _inSize  = inSize;  ///< Dimens�o dos dados de entrada originais.
    _inPrepSize = 0;    ///< Dimens�o da entrada dos dados preparados inicialmente � 0.

    _dataTypeSize   = inSmpFindDataTypeSize();
    _dataGroupSize  = inSmpFindNumDataGroupSize();

    // Alocando os vetores que ir�o conter todos os tipos e grupos, s� que sem repeti��o.
    // Como n�o se sabe a priori quantos grupos e tipos diferentes existir�o, o tamanho 
    // alocado para cada um desses vetores ser� _inSize. No entanto, a posi��o m�xima 
    // utilizada dos dois ser� _dataTypeSize e _dataGroupSize:
    _inDataTypeNoRep  = new dataType[_inSize];
    _inDataGroupNoRep = new dataGroup[_inSize];

    // Zerando os vetores cujos valores n�o foram preenchidos:
    inSmpInitVector( (int*) _inDataTypeNoRep, _inSize, -1 );
    inSmpInitVector( (int*) _inDataGroupNoRep, _inSize, -1 );

    // Insere os tipos n�o repetidos e os grupos n�o repetidos nos vetores:
    int posType=0, posGroup=0;
    for( int i=0 ; i<_inSize ; i++ )
    {
      // Obtendo o tipo e o grupo atual do vetor:
      int thisDataType = (int)_inDataType[i];
      int thisDataGroup = (int)_inDataGroup[i];

      // Verificando se eles j� existem nos vetores de valores sem repeti��o. Caso ainda n�o existam, 
      // s�o acrescentados:
      if( inSmpLookForValue( (int*)_inDataTypeNoRep, _inSize, thisDataType ) == 0 )
      {
        _inDataTypeNoRep[posType] = (dataType)thisDataType;
        posType++;
      }
      if( inSmpLookForValue( (int*)_inDataGroupNoRep, _inSize, thisDataGroup ) == 0 )
      {
        _inDataGroupNoRep[posGroup] = (dataGroup)thisDataGroup;
        posGroup++;
      }
    }
    // O n�mero de grupos e de tipos diferentes encontrados nos vetores tem que ser os mesmos das vari�veis
    //_dataTypeSize e _dataGroupSize.
    if( posType != _dataTypeSize )
      printf( "Erro, tamanhos de n�mero de tipos diferente!!!\n");
    if( posGroup != _dataGroupSize )
      printf( "Erro, tamanhos de n�mero de grupos diferente!!!\n");


  }


  /**
  * Destrutor.
  */
  ~InSample()
  {
    delete _inVector;
    delete _inPrepVector;
    delete _inDataType;
    delete _inDataGroup;
    delete _inPrepFlags;
    delete _inDataTypeNoRep;
    delete _inDataGroupNoRep;
    delete _auxPrepInVector;
    delete _tmpSampleVector;
  }



  /**
  * Escalona o vetor segundo o modelo que prev� a sa�da entre 0.1 e 0.9 fornecendo um vator de refer�ncia
  * e uma porcentagem de toler�ncia em rela��o a esse valor de refer�ncia. Caso o valor de refer�ncia seja
  * omitido, utiliza o valor da m�dia das componentes do vetor como sendo a refer�ncia.
  * @param maintainInterval Intervalo de diferen�a (percentual) para o qual ser� considerada a sa�da 
  * como sendo 0.5.
  * @param percentToMax Diferen�a percentual m�xima a ser utilizada para a escala (em casos de diferen�as 
  * maiores que essas, o valor obtido � limitado a essa diferen�a).
  * @param n_reference Valor da refer�ncia, caso exista.
  */
  void inSmpAutoScaleIn( float n_reference, float maintainInterval, float percentToMax )
  {
    float nReference = n_reference;

    // Caso n�o exista valor de refer�ncia, obt�m a refer�ncia como m�dia das componentes do vetor:
    if( n_reference == 0 )
    {
      float mean = 0;   //< Vari�vel que ir� conter a m�dia.
      // Obt�m a m�dia dos componentes do vetor:
      for( int i=0 ; i<_auxAlreadyUsedPos ; i++ )
        mean += _auxPrepInVector[i];
      mean = mean/_auxAlreadyUsedPos;
      nReference = mean;
    }

    for( int i=0 ; i<_auxAlreadyUsedPos ; i++ )
    {
      // Diferen�a percentual da amostra para a m�dia: 
      float porcent = ( (_auxPrepInVector[i] - nReference) / nReference );

      // Limitando a diferen�a percentual obtida para que ela n�o ultrapasse maxScale:
      if( porcent > percentToMax  )      porcent = percentToMax;
      if( porcent < -percentToMax )      porcent = -percentToMax;

      /* Caso em que a sa�da � Interv% ou mais maior que a entrada: compra: */
      if (porcent > maintainInterval)
        _auxPrepInVector[i] = 0.5f + (porcent*0.4f)/percentToMax;
      /* Caso em que a sa�da � entre -Interv% e +Interv% em rela��o � entrada: manuten��o: */
      if ((porcent >= -maintainInterval) && (porcent <= maintainInterval))
        _auxPrepInVector[i] = 0.5f;
      /* Caso em que a sa�da � Interv% ou mais maior que a entrada: venda: */
      if (porcent < -maintainInterval)
      {
        porcent = -porcent;
        _auxPrepInVector[i] = 1-(0.5f + (porcent*0.4f)/percentToMax);
      }
    }
  }


  /**
  * Obt�m uma vers�o do vetor com todas as componentes dentro de um intervalo pr�-definido.
  * Utiliza as informa��es dispon�veis no _inDataType e no _inPrepFlags para re-gerar os vetores 
  * de entrada da rede.
  * @param maxScale Valor m�ximo da escala a ser aplicada.
  * @param minScale Valor m�nimo da escala a ser aplicada.
  */
  void inSmpAutoScaleIn( float maxScale, float minScale )
  {
    float scale; 
    float minValue = (float)MAX_VALUE;
    float maxValue = (float)MIN_VALUE;

    // Obt�m o maior e o menor valores dentre das componentes do vetor: 
    for( int i=0 ; i<_auxAlreadyUsedPos ; i++ )
    {
      if( _auxPrepInVector[i] < minValue )
        minValue = _auxPrepInVector[i];
      if( _auxPrepInVector[i] > maxValue )
        maxValue = _auxPrepInVector[i];
    }
    // Escala a ser aplicada: 
    scale   = (float)((maxScale - minScale)/(maxValue - minValue));
    // Aplicando a escala ao vetor: 
    for( int i=0 ; i<_auxAlreadyUsedPos ; i++ )
      _auxPrepInVector[i] = (_auxPrepInVector[i]-minValue)*scale + minScale;
  }


  /**
  * Obt�m uma vers�o do vetor com todas as componentes dentro de um intervalo pr�-definido.
  * Utiliza as informa��es dispon�veis no _inDataType e no _inPrepFlags para re-gerar os vetores 
  * de entrada da rede.
  */
  void inSmpAutoScaleIn()
  {
    // Aplicando a escala ao vetor: 
    for( int i=0 ; i<_auxAlreadyUsedPos ; i++ )
      _auxPrepInVector[i] = _auxPrepInVector[i]*0.9f;
  }

  /**
  * Recebe como par�metro um tipo de dado dentre todos os tipos contidos nas amostras,
  * e escalona todos os valores (atualmente sendo utilizados) do vetor que cont�m dados dessa natureza,
  * inserindo os valores j� escalonados no vetor de sa�das.
  * @param actualGroup Grupo de dados que ser� escalonado.
  * @param maxScale Valor m�ximo da escala a ser aplicada.
  * @param minScale Valor m�nimo da escala a ser aplicada.
  */
  void inSmpScaleSimilarData( dataGroup actualGroup, float maxScale, float minScale )
  {
    _auxAlreadyUsedPos = 0;
    for( int i=0 ; i<_inSize ; i++ )  //< Percorre cada uma das posi��es do vetor de entrada.
    {
      // Caso o flag esteja ligado, verifica o tipo de dado armazenado e, caso o tipo seja 
      // o correto (actualGroup), o indice do dado 'e armazenado no vetor:
      if( _inPrepFlags[i] == true )
      {
        if( _inDataGroup[i] == actualGroup )
        {
          _auxPrepInVector[_auxAlreadyUsedPos] = _inVector[i];
          _auxAlreadyUsedPos++;
        }
      }
    }

    // Caso n�o existam pelo menos dois dados no mesmo grupo, apenas copia as entradas no vetor de dados preparados:
    if( _auxAlreadyUsedPos < 3 )
    {
      inSmpAutoScaleIn();
    }
    else 
    {
      // Uma vez que todos os valores ja foram copiados, e preciso escalona-los:
      inSmpAutoScaleIn( maxScale, minScale );
    }

    // Caso o flag esteja ligado, o valor da vari�vel � setado como -1:
    //if( _inPrepFlags[i] == false )
    //  _inPrepVector[i] = -1;

    // Copia os valores ja escalonados para o vetor de entradas:
    int pt=0;
    for( int i=0 ; i<_inSize ; i++)
    {
      // Caso o flag esteja ligado, verifica o tipo de dado armazenado e, caso o tipo seja 
      // o correto (actualGroup), o dado � inserido no �ndice dovetor:
      if( _inPrepFlags[i] == true )
      {
        if( _inDataGroup[i] == actualGroup )
        {
          _inPrepVector[i] = _auxPrepInVector[pt];
          pt++;
        }
      }
    }
  }


  /**
  * Recebe como par�metro um tipo de dado dentre todos os tipos contidos nas amostras,
  * e escalona todos os valores (atualmente sendo utilizados) do vetor que cont�m dados dessa natureza,
  * inserindo os valores j� escalonados no vetor de sa�das.
  * @param n_reference Valor da refer�ncia, caso exista.
  * @param actualGroup Grupo de dados que ser� escalonado.
  * @param maintainInterval Intervalo de diferen�a (percentual) para o qual ser� considerada a sa�da 
  * como sendo 0.5.
  * @param percentToMax Diferen�a percentual m�xima a ser utilizada para a escala (em casos de diferen�as 
  * maiores que essas, o valor obtido � limitado a essa diferen�a).
  */
  void inSmpScaleSimilarData( float n_reference, dataGroup actualGroup, float maintainInterval, float percentToMax )
  {
    _auxAlreadyUsedPos = 0;
    for( int i=0 ; i<_inSize ; i++ )  //< Percorre cada uma das posi��es do vetor de entrada.
    {
      // Caso o flag esteja ligado, verifica o tipo de dado armazenado e, caso o tipo seja 
      // o correto (actualGroup), o indice do dado 'e armazenado no vetor:
      if( _inPrepFlags[i] == true )
      {
        if( _inDataGroup[i] == actualGroup )
        {
          _auxPrepInVector[_auxAlreadyUsedPos] = _inVector[i];
          _auxAlreadyUsedPos++;
        }
      }
    }

    // Caso n�o existam pelo menos dois dados no mesmo grupo, apenas copia as entradas no vetor de dados preparados:
    if( _auxAlreadyUsedPos < 3 )
    {
      inSmpAutoScaleIn();
    }
    else
    {
      // Uma vez que todos os valores ja foram copiados, e preciso escalona-los:
      inSmpAutoScaleIn( n_reference, maintainInterval, percentToMax );
    }

    // Copia os valores ja escalonados para o vetor de entradas:
    int pt=0;
    for( int i=0 ; i<_inSize ; i++)
    {
      // Caso o flag esteja ligado, verifica o tipo de dado armazenado e, caso o tipo seja 
      // o correto (actualGroup), o dado � inserido no �ndice dovetor:
      if( _inPrepFlags[i] == true )
      {
        if( _inDataGroup[i] == actualGroup )
        {
          _inPrepVector[i] = _auxPrepInVector[pt];
          pt++;
        }
      }
    }
  }


  /** 
  * Inicializa os valores de todas as medi��es do vetor de dados preparados com -1.
  */
  void inSmpPrepVectorInit()
  {
    // Zera todas as entradas do vetor de entradas escalonadas (ja preparadas, porem em suas
    // posicoes originais:
    for( int i=0 ; i<_inSize ; i++ )
      _inPrepVector[i] = -1;
  }


  /**
  * Recebe um ponteiro e insere todos os valores v�lidos da amostra no vetor representado por 
  * esse ponteiro.
  * @param retVector Ponteiro para o vetor a ser preenchido e retornado.
  * @param vectorSize Dimens�o desse vetor. Nessa mesma vari�vel � retornada
  * a dimens�o da por��o preenchida do vetor pela fun��o.
  * @return Retorna false em caso de erro, ou true caso ok.
  */
  bool inSmpGetValidValuesVector( float *retVector, int *vectorSize )
  {
    // Casos de retorno:
    if( (!retVector) )
      return false;

    int position = 0;
    // Caso contrario, insere os valores v�lidos da amostra no vetor:
    for( int i=0 ; i<_inSize ; i++)
    {
      if( _inPrepFlags[i] == true ) 
      {
        retVector[position] = (float)_inPrepVector[i];
        position++;
      }
    }
    // Atualiza o tamanho (n�mero de amostras contidas no vetor retornado):
    *vectorSize = position;
    return true;
  }


  /**
  * Liga/desliga a utiliza��o de um determinado �ndice (valor) da amostra.
  * @param newState Novo estado desse valor (true=ligado, false=desligado).
  * @param pos �ndice desse valor dentro da amostra.
  * @return Retorna false em caso de erro (vetor nulo ou de dimens�es diferentes), ou true caso ok.
  */
  bool inSmpChangeOneFlag( bool newState, int pos )
  {
    // Casos de retorno:
    if( (pos < 0) || (pos >= _inSize) )
      return false;
    // Caso contr�rio, reconfigura a amostra:
    _inPrepFlags[pos] = newState;
    return true;
  }


  /**
  * Obt�m (caso exista) um determinado valor de medi��o dentro de uma inst�ncia da classe.
  * Para isso recebe o �ndice do dia dessa medi��o dentro da amostra, al�m do seu tipo.
  * Os dias possuem �ndices ordenados da seguinte maneira: o �ltimo dia � o dia zero (D0).
  * o dia anterior � D-1, e o anterior a esse � o dia D-2, ... etc, at� termos Dn-1.
  * @param mType Tipo da medi��o desejada.
  * @param mGroup Grupo de normaliza��o ao qual esse dado pertence.
  * @param mDay Dia que cont�m a medi��o desejada.
  * @param retValue Valor da medi��o desejada retornado (caso exista).
  * @return Retorna false em caso de erro (medi��o inexistente, dia inexistente, etc), ou 
  * true caso ok.
  */
  bool inSmpGetOneMeasurement( dataType mType, dataGroup mGroup, int mDay, float *retValue )
  {
    // Casos de retorno:
    if( (abs(mDay)+1) > _inNumberOfDays )
      return false;

    int convertedDay = _inNumberOfDays - (abs(mDay)+1);

    int i=0;
    int j=0;
    while( i < _inSize )
    {
      if( (_inDataType[i] == mType) && (_inDataGroup[i] == mGroup) && (j == convertedDay) )
      {
        *retValue = _inVector[i];
        return true;
      }
      if( (_inDataType[i] == mType) && (_inDataGroup[i] == mGroup) && (j != convertedDay) )
        j++;
      i++;
    }

    *retValue = NULL;
    return false;
  }


  /**
  * Modifica (caso exista) um determinado valor de medi��o dentro de uma inst�ncia da classe.
  * Para isso recebe o �ndice do dia dessa medi��o dentro da amostra, al�m do seu tipo.
  * Os dias possuem �ndices ordenados da seguinte maneira: o �ltimo dia � o dia zero (D0).
  * o dia anterior � D-1, e o anterior a esse � o dia D-2, ... etc, at� termos Dn-1.
  * @param mType Tipo da medi��o desejada.
  * @param mGroup Grupo de normaliza��o ao qual esse dado pertence.
  * @param mDay Dia que cont�m a medi��o desejada.
  * @param newValue Novo valor da medi��o desejada (caso exista).
  * @return Retorna false em caso de erro (medi��o inexistente, dia inexistente, etc), ou 
  * true caso ok.
  */
  bool inSmpSetOneMeasurement( dataType mType, dataGroup mGroup, int mDay, float newValue )
  {
    // Casos de retorno:
    if( (abs(mDay)+1) > _inNumberOfDays )
      return false;

    int convertedDay = _inNumberOfDays - (abs(mDay)+1);

    int i=0;
    int j=0;
    while( i < _inSize )
    {
      if( (_inDataType[i] == mType) && (_inDataGroup[i] == mGroup) && (j == convertedDay) )
      {
        _inVector[i] = newValue;
        return true;
      }
      if( (_inDataType[i] == mType) && (_inDataGroup[i] == mGroup) && (j != convertedDay) )
        j++;
      i++;
    }
    return false;
  }


  /**
  * Modifica o valor do flag de uma determinada medi��o dentro da amostra. Para isso, 
  * recebe o novo valor do flag (true ou false), al�m do tipo, grupo de normaliza��o e 
  * o dia da medi��o a ter seu estado modificado. 
  * Os dias possuem �ndices ordenados da seguinte maneira: o �ltimo dia � o dia zero (D0).
  * o dia anterior � D-1, e o anterior a esse � o dia D-2, ... etc, at� termos Dn-1.
  * @param mType Tipo da medi��o desejada.
  * @param mGroup Grupo de normaliza��o ao qual esse dado pertence.
  * @param mDay Dia que cont�m a medi��o desejada.
  * @param newState Novo estado (ligado=true/desligado=false) da medi��o desejada.
  * @return Retorna false em caso de erro (medi��o inexistente, dia inexistente, etc), ou 
  * true caso ok.
  */
  bool inSmpSetOneMeasurementNewState( dataType mType, dataGroup mGroup, int mDay, bool newState )
  {
    // Casos de retorno:
    if( (abs(mDay)+1) > _inNumberOfDays )
      return false;

    int convertedDay = _inNumberOfDays - (abs(mDay)+1);

    int i=0;
    int j=0;
    while( i < _inSize )
    {
      if( (_inDataType[i] == mType) && (_inDataGroup[i] == mGroup) && (j == convertedDay) )
      {
        inSmpChangeOneFlag( newState, i );
        return true;
      }
      if( (_inDataType[i] == mType) && (_inDataGroup[i] == mGroup) && (j != convertedDay) )
        j++;
      i++;
    }
    return false;
  }


  /**
  * Obt�m o valor do flag de uma determinada medi��o dentro da amostra. Para isso, 
  * recebe o tipo, grupo de normaliza��o e o dia da medi��o a ter seu valor de flag retornado. 
  * Os dias possuem �ndices ordenados da seguinte maneira: o �ltimo dia � o dia zero (D0).
  * o dia anterior � D-1, e o anterior a esse � o dia D-2, ... etc, at� termos Dn-1.
  * @param mType Tipo da medi��o desejada.
  * @param mGroup Grupo de normaliza��o ao qual esse dado pertence.
  * @param mDay Dia que cont�m a medi��o desejada.
  * @param State Estado (ligado=true/desligado=false) da medi��o desejada retornado.
  * @return Retorna false em caso de erro (medi��o inexistente, dia inexistente, etc), ou 
  * true caso ok.
  */
  bool inSmpGetOneMeasurementState( dataType mType, dataGroup mGroup, int mDay, bool *State )
  {
    // Casos de retorno:
    if( (abs(mDay)+1) > _inNumberOfDays )
      return false;

    int convertedDay = _inNumberOfDays - (abs(mDay)+1);

    int i=0;
    int j=0;
    while( i < _inSize )
    {
      if( (_inDataType[i] == mType) && (_inDataGroup[i] == mGroup) && (j == convertedDay) )
      {
        *State = _inPrepFlags[i];
        return true;
      }
      if( (_inDataType[i] == mType) && (_inDataGroup[i] == mGroup) && (j != convertedDay) )
        j++;
      i++;
    }
    return false;
  }


  /**
  * Obt�m o n�mero de valores v�lidos de uma amostra.
  * @return Retorna o n�mero de valores v�lidos da amostra.
  */
  int inSmpGetNumValidValues()
  {
    int validMeasurementsSize = 0;
    // Caso contrario, insere os valores v�lidos da amostra no vetor:
    for( int i=0 ; i<_inSize ; i++)
    {
      if( _inPrepFlags[i] == true ) 
      {
        validMeasurementsSize++;
      }
    }
    return validMeasurementsSize;
  }


  /**
  * Obt�m o Vetor (completo, inclusive dos desligados) de tipos de dados.
  * @return Retorna o vetor de tipos de dados contidos na amostra.
  */
  dataType* inSmpGetDataTypeVector()
  {
    return _inDataType;
  }


  /**
  * Obt�m o Vetor (completo, inclusive dos desligados) de grupos de escalonamento de dados.
  * @return Retorna vetor de grupos de escalonamento contidos na amostra.
  */
  dataGroup* inSmpGetDataGroupVector()
  {
    return _inDataGroup;
  }



  /**
  * Obt�m o Vetor de flags de dados.
  * @return Retorna o vetor de flags de dados contidos na amostra.
  */
  bool* inSmpGetInPrepFlagsVector()
  {
    return _inPrepFlags;
  }


  /**
  * Obt�m o Vetor de tipos que n�o possui repeti��es, al�m do n�mero de tipos nele contidos.
  * @param dimRet Dimens�o do vetor de dataTypes, retornada.
  * @return Retorna o vetor de tipos de dados contidos na amostra.
  */
  dataType* inSmpGetDataTypeNoRepVector( int *dimRet=NULL )
  {
    *dimRet = _dataTypeSize;
    return _inDataTypeNoRep;
  }


  /**
  * Obt�m o Vetor de tipos que n�o possui repeti��es, al�m do n�mero de tipos nele contidos.
  * @param dimRet Dimens�o do vetor de dataGroups, retornada.
  * @return Retorna o vetor de tipos de dados contidos na amostra.
  */
  dataGroup* inSmpGetDataGroupNoRepVector( int *dimRet )
  {
    *dimRet = _dataGroupSize;
    return _inDataGroupNoRep;
  }


};


#endif // INSAMPLE_H