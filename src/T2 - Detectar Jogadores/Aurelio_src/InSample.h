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

  // Vetores de entradas e saídas:
  float    *_inVector;          ///< Vetor contendo os valores de entrada da amostra.
  float    *_inPrepVector;      ///< Vetor contendo os valores de entrada da amostra já preparados.
  dataType  *_inDataType;       ///< Vetor de tipos: qual o tipo de dado em cada posicao do vetor.
  dataGroup *_inDataGroup;      ///< Vetor de tipos: qual o grupo de escalonamento ao qual cada dado pertence.
  bool      *_inPrepFlags;      ///< Vetor de flags indica, os índices do vetor original atualmente sendo utilizados.
  int _inNumberOfDays;          ///< Número de dias de medições contidos na amostra.
  int _inSize;                  ///< Dimensão da entrada original.
  int _inPrepSize;              ///< Dimensão da entrada dos dados preparados.

  // Variáveis obtidas indiretamente com algum processamento:
  int _dataTypeSize;            ///< Numero de tipos de dados diferentes nas amostras.
  int _dataGroupSize;           ///< Numero de tipos de dados diferentes nas amostras.
  dataType  *_inDataTypeNoRep;  ///< Armazena todos Tipos diferentes, sem repetição (dimensão _dataTypeSize).
  dataGroup *_inDataGroupNoRep; ///< Armazena todos Grupos diferentes sem repetição (dimensão _dataGroupSize).


  // Variáveis e vetores auxiliares (utilizados temporariamente durante o processamento):
  float *_auxPrepInVector;      //< Armazena valores temporários durante o escalonamento.
  int _PrepInVectorActualSize;  //< Porção do vetor sendo utilizada durante o escalonamento.
  int _auxAlreadyUsedPos;       //< Posição máxima sendo utilizada no vetor temporário.

  // Variáveis criadas para manter a saída já pronta pra uso:
  float *_tmpSampleVector;      //< Vetor que contém a última amostra preparada gerada como saída.
  int _tmpSampleVectorSize;     //< Dimensão atual do vetor que contém a última amostra preparada gerada como saída.



  /**
  * Função auxiliar para encontrar o número de tipos ou grupos existentes, sem repetição.
  * Percorre um vetor até a posição apropriada, verificando se o valor recebido já existe nesse vetor até 
  * aquela posição.
  * @param inVector Vetor de entrada, contendo os valores.
  * @param maxPos Posição onde deve ser finalizada a procura, ou seja, a procura será executada até essa posição.
  * @param value Vator que será procurado.
  * @return Retorna -1 em caso de erro (vetor nulo, etc.), 0 em caso de não ter encontrado, ou 1 caso encontre.
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

    // Caso não tenha sido encontrado, retorna 0:
    return 0;
  }


  /**
  * Auxiliar. Inicializa todas as componentes de um vetor com o valor recebido.
  * @param inVector Vetor a ter seus valores inicializados.
  * @param size Posição até a qual os valores serão inicializados (no máximo a dimensão do vetor).
  * @param value Valor de inicialização a ser aplicado.
  */
  void inSmpInitVector( int *inVector, int size, int value )
  {
    if( !inVector )
      return;
    for( int i=0 ; i<size ; i++ )
      inVector[i] = value;
  }


  /**
  * Imprime o conteúdo das amostras.
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
  * Auxiliar. Encontra o número de grupos de escalonamento diferentes na amostra.
  * @return Retorna o número de grupos de escalonamento diferentes na amostra..
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
  * Auxiliar. Encontra o número de tipos de dados diferentes na amostra.
  * @return Retorna o número de tipos de dados diferentes na amostra.
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
  * @param inVector Vetor de entrada. Contém uma amostra de entrada já formada, com os dados originais.
  * @param inDataType Vetor de tipos indicando qual o tipo de dado em cada posicao do vetor.
  * @param inDataGroup Vetor de tipos indicando qual o grupo de escalonamento ao qual cada dado pertence.
  * @param inSize Dimensão dos vetores de entrada recebidos (inVector, inDataFlag, inDataGroup).
  * @param inNumberOfDays Número de dias de medição contidos na amostra.
  */
  InSample(float *inVector, dataType *inDataType, dataGroup *inDataGroup, int inSize, int inNumberOfDays )
  {
    _inVector         = new float    [inSize];
    _inPrepVector     = new float    [inSize];
    _inDataType       = new dataType  [inSize];
    _inDataGroup      = new dataGroup [inSize];
    _inPrepFlags      = new bool      [inSize];
    // Valores iniciais das variáveis temporárias:
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
      _inPrepFlags[i] = true;           //< Todos os valores começam sendo utilizados.
      _tmpSampleVector[i] = -1;         //< Valor dummy.
    }

    // Dimensões:
    _inNumberOfDays = inNumberOfDays;
    _inSize  = inSize;  ///< Dimensão dos dados de entrada originais.
    _inPrepSize = 0;    ///< Dimensão da entrada dos dados preparados inicialmente é 0.

    _dataTypeSize   = inSmpFindDataTypeSize();
    _dataGroupSize  = inSmpFindNumDataGroupSize();

    // Alocando os vetores que irão conter todos os tipos e grupos, só que sem repetição.
    // Como não se sabe a priori quantos grupos e tipos diferentes existirão, o tamanho 
    // alocado para cada um desses vetores será _inSize. No entanto, a posição máxima 
    // utilizada dos dois será _dataTypeSize e _dataGroupSize:
    _inDataTypeNoRep  = new dataType[_inSize];
    _inDataGroupNoRep = new dataGroup[_inSize];

    // Zerando os vetores cujos valores não foram preenchidos:
    inSmpInitVector( (int*) _inDataTypeNoRep, _inSize, -1 );
    inSmpInitVector( (int*) _inDataGroupNoRep, _inSize, -1 );

    // Insere os tipos não repetidos e os grupos não repetidos nos vetores:
    int posType=0, posGroup=0;
    for( int i=0 ; i<_inSize ; i++ )
    {
      // Obtendo o tipo e o grupo atual do vetor:
      int thisDataType = (int)_inDataType[i];
      int thisDataGroup = (int)_inDataGroup[i];

      // Verificando se eles já existem nos vetores de valores sem repetição. Caso ainda não existam, 
      // são acrescentados:
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
    // O número de grupos e de tipos diferentes encontrados nos vetores tem que ser os mesmos das variáveis
    //_dataTypeSize e _dataGroupSize.
    if( posType != _dataTypeSize )
      printf( "Erro, tamanhos de número de tipos diferente!!!\n");
    if( posGroup != _dataGroupSize )
      printf( "Erro, tamanhos de número de grupos diferente!!!\n");


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
  * Escalona o vetor segundo o modelo que prevê a saída entre 0.1 e 0.9 fornecendo um vator de referência
  * e uma porcentagem de tolerância em relação a esse valor de referência. Caso o valor de referência seja
  * omitido, utiliza o valor da média das componentes do vetor como sendo a referência.
  * @param maintainInterval Intervalo de diferença (percentual) para o qual será considerada a saída 
  * como sendo 0.5.
  * @param percentToMax Diferença percentual máxima a ser utilizada para a escala (em casos de diferenças 
  * maiores que essas, o valor obtido é limitado a essa diferença).
  * @param n_reference Valor da referência, caso exista.
  */
  void inSmpAutoScaleIn( float n_reference, float maintainInterval, float percentToMax )
  {
    float nReference = n_reference;

    // Caso não exista valor de referência, obtém a referência como média das componentes do vetor:
    if( n_reference == 0 )
    {
      float mean = 0;   //< Variável que irá conter a média.
      // Obtém a média dos componentes do vetor:
      for( int i=0 ; i<_auxAlreadyUsedPos ; i++ )
        mean += _auxPrepInVector[i];
      mean = mean/_auxAlreadyUsedPos;
      nReference = mean;
    }

    for( int i=0 ; i<_auxAlreadyUsedPos ; i++ )
    {
      // Diferença percentual da amostra para a média: 
      float porcent = ( (_auxPrepInVector[i] - nReference) / nReference );

      // Limitando a diferença percentual obtida para que ela não ultrapasse maxScale:
      if( porcent > percentToMax  )      porcent = percentToMax;
      if( porcent < -percentToMax )      porcent = -percentToMax;

      /* Caso em que a saída é Interv% ou mais maior que a entrada: compra: */
      if (porcent > maintainInterval)
        _auxPrepInVector[i] = 0.5f + (porcent*0.4f)/percentToMax;
      /* Caso em que a saída é entre -Interv% e +Interv% em relação à entrada: manutenção: */
      if ((porcent >= -maintainInterval) && (porcent <= maintainInterval))
        _auxPrepInVector[i] = 0.5f;
      /* Caso em que a saída é Interv% ou mais maior que a entrada: venda: */
      if (porcent < -maintainInterval)
      {
        porcent = -porcent;
        _auxPrepInVector[i] = 1-(0.5f + (porcent*0.4f)/percentToMax);
      }
    }
  }


  /**
  * Obtém uma versão do vetor com todas as componentes dentro de um intervalo pré-definido.
  * Utiliza as informações disponíveis no _inDataType e no _inPrepFlags para re-gerar os vetores 
  * de entrada da rede.
  * @param maxScale Valor máximo da escala a ser aplicada.
  * @param minScale Valor mínimo da escala a ser aplicada.
  */
  void inSmpAutoScaleIn( float maxScale, float minScale )
  {
    float scale; 
    float minValue = (float)MAX_VALUE;
    float maxValue = (float)MIN_VALUE;

    // Obtém o maior e o menor valores dentre das componentes do vetor: 
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
  * Obtém uma versão do vetor com todas as componentes dentro de um intervalo pré-definido.
  * Utiliza as informações disponíveis no _inDataType e no _inPrepFlags para re-gerar os vetores 
  * de entrada da rede.
  */
  void inSmpAutoScaleIn()
  {
    // Aplicando a escala ao vetor: 
    for( int i=0 ; i<_auxAlreadyUsedPos ; i++ )
      _auxPrepInVector[i] = _auxPrepInVector[i]*0.9f;
  }

  /**
  * Recebe como parâmetro um tipo de dado dentre todos os tipos contidos nas amostras,
  * e escalona todos os valores (atualmente sendo utilizados) do vetor que contém dados dessa natureza,
  * inserindo os valores já escalonados no vetor de saídas.
  * @param actualGroup Grupo de dados que será escalonado.
  * @param maxScale Valor máximo da escala a ser aplicada.
  * @param minScale Valor mínimo da escala a ser aplicada.
  */
  void inSmpScaleSimilarData( dataGroup actualGroup, float maxScale, float minScale )
  {
    _auxAlreadyUsedPos = 0;
    for( int i=0 ; i<_inSize ; i++ )  //< Percorre cada uma das posições do vetor de entrada.
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

    // Caso não existam pelo menos dois dados no mesmo grupo, apenas copia as entradas no vetor de dados preparados:
    if( _auxAlreadyUsedPos < 3 )
    {
      inSmpAutoScaleIn();
    }
    else 
    {
      // Uma vez que todos os valores ja foram copiados, e preciso escalona-los:
      inSmpAutoScaleIn( maxScale, minScale );
    }

    // Caso o flag esteja ligado, o valor da variável é setado como -1:
    //if( _inPrepFlags[i] == false )
    //  _inPrepVector[i] = -1;

    // Copia os valores ja escalonados para o vetor de entradas:
    int pt=0;
    for( int i=0 ; i<_inSize ; i++)
    {
      // Caso o flag esteja ligado, verifica o tipo de dado armazenado e, caso o tipo seja 
      // o correto (actualGroup), o dado é inserido no índice dovetor:
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
  * Recebe como parâmetro um tipo de dado dentre todos os tipos contidos nas amostras,
  * e escalona todos os valores (atualmente sendo utilizados) do vetor que contém dados dessa natureza,
  * inserindo os valores já escalonados no vetor de saídas.
  * @param n_reference Valor da referência, caso exista.
  * @param actualGroup Grupo de dados que será escalonado.
  * @param maintainInterval Intervalo de diferença (percentual) para o qual será considerada a saída 
  * como sendo 0.5.
  * @param percentToMax Diferença percentual máxima a ser utilizada para a escala (em casos de diferenças 
  * maiores que essas, o valor obtido é limitado a essa diferença).
  */
  void inSmpScaleSimilarData( float n_reference, dataGroup actualGroup, float maintainInterval, float percentToMax )
  {
    _auxAlreadyUsedPos = 0;
    for( int i=0 ; i<_inSize ; i++ )  //< Percorre cada uma das posições do vetor de entrada.
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

    // Caso não existam pelo menos dois dados no mesmo grupo, apenas copia as entradas no vetor de dados preparados:
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
      // o correto (actualGroup), o dado é inserido no índice dovetor:
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
  * Inicializa os valores de todas as medições do vetor de dados preparados com -1.
  */
  void inSmpPrepVectorInit()
  {
    // Zera todas as entradas do vetor de entradas escalonadas (ja preparadas, porem em suas
    // posicoes originais:
    for( int i=0 ; i<_inSize ; i++ )
      _inPrepVector[i] = -1;
  }


  /**
  * Recebe um ponteiro e insere todos os valores válidos da amostra no vetor representado por 
  * esse ponteiro.
  * @param retVector Ponteiro para o vetor a ser preenchido e retornado.
  * @param vectorSize Dimensão desse vetor. Nessa mesma variável é retornada
  * a dimensão da porção preenchida do vetor pela função.
  * @return Retorna false em caso de erro, ou true caso ok.
  */
  bool inSmpGetValidValuesVector( float *retVector, int *vectorSize )
  {
    // Casos de retorno:
    if( (!retVector) )
      return false;

    int position = 0;
    // Caso contrario, insere os valores válidos da amostra no vetor:
    for( int i=0 ; i<_inSize ; i++)
    {
      if( _inPrepFlags[i] == true ) 
      {
        retVector[position] = (float)_inPrepVector[i];
        position++;
      }
    }
    // Atualiza o tamanho (número de amostras contidas no vetor retornado):
    *vectorSize = position;
    return true;
  }


  /**
  * Liga/desliga a utilização de um determinado índice (valor) da amostra.
  * @param newState Novo estado desse valor (true=ligado, false=desligado).
  * @param pos Índice desse valor dentro da amostra.
  * @return Retorna false em caso de erro (vetor nulo ou de dimensões diferentes), ou true caso ok.
  */
  bool inSmpChangeOneFlag( bool newState, int pos )
  {
    // Casos de retorno:
    if( (pos < 0) || (pos >= _inSize) )
      return false;
    // Caso contrário, reconfigura a amostra:
    _inPrepFlags[pos] = newState;
    return true;
  }


  /**
  * Obtém (caso exista) um determinado valor de medição dentro de uma instância da classe.
  * Para isso recebe o índice do dia dessa medição dentro da amostra, além do seu tipo.
  * Os dias possuem índices ordenados da seguinte maneira: o último dia é o dia zero (D0).
  * o dia anterior é D-1, e o anterior a esse é o dia D-2, ... etc, até termos Dn-1.
  * @param mType Tipo da medição desejada.
  * @param mGroup Grupo de normalização ao qual esse dado pertence.
  * @param mDay Dia que contém a medição desejada.
  * @param retValue Valor da medição desejada retornado (caso exista).
  * @return Retorna false em caso de erro (medição inexistente, dia inexistente, etc), ou 
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
  * Modifica (caso exista) um determinado valor de medição dentro de uma instância da classe.
  * Para isso recebe o índice do dia dessa medição dentro da amostra, além do seu tipo.
  * Os dias possuem índices ordenados da seguinte maneira: o último dia é o dia zero (D0).
  * o dia anterior é D-1, e o anterior a esse é o dia D-2, ... etc, até termos Dn-1.
  * @param mType Tipo da medição desejada.
  * @param mGroup Grupo de normalização ao qual esse dado pertence.
  * @param mDay Dia que contém a medição desejada.
  * @param newValue Novo valor da medição desejada (caso exista).
  * @return Retorna false em caso de erro (medição inexistente, dia inexistente, etc), ou 
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
  * Modifica o valor do flag de uma determinada medição dentro da amostra. Para isso, 
  * recebe o novo valor do flag (true ou false), além do tipo, grupo de normalização e 
  * o dia da medição a ter seu estado modificado. 
  * Os dias possuem índices ordenados da seguinte maneira: o último dia é o dia zero (D0).
  * o dia anterior é D-1, e o anterior a esse é o dia D-2, ... etc, até termos Dn-1.
  * @param mType Tipo da medição desejada.
  * @param mGroup Grupo de normalização ao qual esse dado pertence.
  * @param mDay Dia que contém a medição desejada.
  * @param newState Novo estado (ligado=true/desligado=false) da medição desejada.
  * @return Retorna false em caso de erro (medição inexistente, dia inexistente, etc), ou 
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
  * Obtém o valor do flag de uma determinada medição dentro da amostra. Para isso, 
  * recebe o tipo, grupo de normalização e o dia da medição a ter seu valor de flag retornado. 
  * Os dias possuem índices ordenados da seguinte maneira: o último dia é o dia zero (D0).
  * o dia anterior é D-1, e o anterior a esse é o dia D-2, ... etc, até termos Dn-1.
  * @param mType Tipo da medição desejada.
  * @param mGroup Grupo de normalização ao qual esse dado pertence.
  * @param mDay Dia que contém a medição desejada.
  * @param State Estado (ligado=true/desligado=false) da medição desejada retornado.
  * @return Retorna false em caso de erro (medição inexistente, dia inexistente, etc), ou 
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
  * Obtém o número de valores válidos de uma amostra.
  * @return Retorna o número de valores válidos da amostra.
  */
  int inSmpGetNumValidValues()
  {
    int validMeasurementsSize = 0;
    // Caso contrario, insere os valores válidos da amostra no vetor:
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
  * Obtém o Vetor (completo, inclusive dos desligados) de tipos de dados.
  * @return Retorna o vetor de tipos de dados contidos na amostra.
  */
  dataType* inSmpGetDataTypeVector()
  {
    return _inDataType;
  }


  /**
  * Obtém o Vetor (completo, inclusive dos desligados) de grupos de escalonamento de dados.
  * @return Retorna vetor de grupos de escalonamento contidos na amostra.
  */
  dataGroup* inSmpGetDataGroupVector()
  {
    return _inDataGroup;
  }



  /**
  * Obtém o Vetor de flags de dados.
  * @return Retorna o vetor de flags de dados contidos na amostra.
  */
  bool* inSmpGetInPrepFlagsVector()
  {
    return _inPrepFlags;
  }


  /**
  * Obtém o Vetor de tipos que não possui repetições, além do número de tipos nele contidos.
  * @param dimRet Dimensão do vetor de dataTypes, retornada.
  * @return Retorna o vetor de tipos de dados contidos na amostra.
  */
  dataType* inSmpGetDataTypeNoRepVector( int *dimRet=NULL )
  {
    *dimRet = _dataTypeSize;
    return _inDataTypeNoRep;
  }


  /**
  * Obtém o Vetor de tipos que não possui repetições, além do número de tipos nele contidos.
  * @param dimRet Dimensão do vetor de dataGroups, retornada.
  * @return Retorna o vetor de tipos de dados contidos na amostra.
  */
  dataGroup* inSmpGetDataGroupNoRepVector( int *dimRet )
  {
    *dimRet = _dataGroupSize;
    return _inDataGroupNoRep;
  }


};


#endif // INSAMPLE_H