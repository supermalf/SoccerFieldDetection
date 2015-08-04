#ifndef _GROWING_NEURAL_GAS_H_
#define _GROWING_NEURAL_GAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Vector.h"

#ifdef __cplusplus
extern "C" {
#endif
//#include "Vector.h"
#ifdef __cplusplus
}
#endif

/**
* Valor para RAND_MAX
*/
#ifndef RAND_MAX
#define RAND_MAX 0x7fff
#endif

/**
* Módulo da diferença entre dois números.
*/
#define Modulo(a,b) ((a>=b)?(a-b):(b-a))

/**
* Maior entre dois números.
*/
#define Maior(a, b) ((a>=b)?(a):(b))

/**
* Menor entre dois números.
*/
#define Menor(a, b) ((a<b)?(a):(b))

/**
* Estrutura que representa um neurônio. 
*/
typedef struct neuron
{
  int Id;               //< Índice de identificação do neurônio.
  ptMVector Weights;    //< Pesos neurônio.
  int NumWinners;       //< Número de vezes em que foi vencedor.
  float AccQuadDist;    //< Distância quadrática acumulada.
  struct neuron* *Nbrs; //< Lista de vizinhos.
  int *Edges;           //< Lista de arestas.
  int nNbrs;            //< Número máximo de vizinhos e de arestas.

} Neuron, *ptNeuron;

/************************************************************************
Estrutura de dados que representa um GNG(Growing Neural Gas).
************************************************************************/
typedef struct gng
{
  char *Name;                     ///< Nome da instância.
  int NeuronsLength;              ///< Dimensão.
  int NearestIndex;               ///< Índice do último vencedor.
  ptNeuron Nearest;               ///< Ponteiro aponta para o último vencedor.
  ptMVector NearestWeights;       ///< Ponteiro para o vetor de pesos do último vencedor.
  int SecondNearestIndex;         ///< Índice do último vencedor.
  ptNeuron SecondNearest;         ///< Ponteiro aponta para o último vencedor.
  ptMVector SecondNearestWeights; ///< Ponteiro para o vetor de pesos do último vencedor.
  int nTraineds;

  // Dois vetores auxiliares, apenas utilizados pela função de treinamento:
  ptMVector aux1;

  // Parâmetros obrigatórios do GNG:
  int NNeurons;                 ///< Número total de neurônios.
  ptNeuron *lstNeurons;         ///< Lista de neurônios do mapa.
  int IndNbrs;                  ///< Número total de neurônios existentes no momento.
  int Lambda;                   ///< Determina quantos treinos até inclusão do próximo neurônio.
  float Eb;                     ///< Taxa de aprendizado utilizada no treinamento do neurônio vencedor.
  float En;                     ///< Taxa de aprendizado utilizada para os vizinhos do neurônio vencedor.
  float Alfa;                   ///< Taxa de decrescimento do erro do neurônio vencedor e seu vizinho.
  float D;                      ///< Taxa de decrescimento do erro geral dos neurônios.
  int Amax;                     ///< Peso máximo permitido a uma aresta s/ que seja removida.

  // Esse vetor serve para que se possa modificar a função de aprendizado hebbiado:
  float *weights;

} GNG, *ptGNG;


  /**
  * Cria um novo neurônio.
  * @param NewId Índice de identificação do neurônio. 
  * @param dim Número de pesos (sinapses) que o neurônio deve possuir.
  * @param numNbrs Número máximo de vizinhos que o neurônio pode possuir.
  * @param NewNeuron Novo neurônio retornado.
  * @return ...
  */
  int CreateNeuron( int NewId, int dim, int numNbrs, ptNeuron *NewNeuron  )
  {
    int tmpCont;
    // Aloca espaço para um novo neurônio:
    ptNeuron tmpneuron = (ptNeuron)malloc(sizeof(Neuron));
    // Criando o vetor de arestas:
    tmpneuron->Edges = (int*)malloc(numNbrs*sizeof(int));
    // Criando o vetor de ponteiros para vizinhos:
    tmpneuron->Nbrs = (ptNeuron*)malloc(numNbrs*sizeof(ptNeuron));

    // Incluíndo 0 nos pesos das arestas e NULL nos ponteiros:
    for( tmpCont = 0 ; tmpCont < numNbrs ; tmpCont++ )
    {
      tmpneuron->Edges[tmpCont] = -1; //< -1: não existe aresta.
      tmpneuron->Nbrs[tmpCont] = NULL;
    }

    // Alocando espaço para o vetor de pesos:
    NewEmptyVector(dim, &tmpneuron->Weights);
    // Inserindo valores aleatórios nos pesos do neurônio:
    for( tmpCont = 0 ; tmpCont < dim ; tmpCont++ )
      tmpneuron->Weights->values[tmpCont] = (float)((float)rand()/(float)RAND_MAX);
    // Número identificador:
    tmpneuron->Id = NewId;
    // Número de vezes em que foi vencedor:
    tmpneuron->AccQuadDist= 0;
    tmpneuron->nNbrs= numNbrs;
    //tmpneuron->IndNbrs= 0;
    *NewNeuron = tmpneuron;

    return 0;
  }


  /**
  * Cria um novo neurônio, sendo a estrutuda do neurônio alocada externamente.
  * @param NewId Índice de identificação do neurônio. 
  * @param dim Número de pesos (sinapses) que o neurônio deve possuir.
  * @param numNbrs Número máximo de vizinhos que o neurônio pode possuir.
  * @param NewNeuron Novo neurônio retornado.
  */
  int CreateNeuronAloc( int NewId, int dim, int numNbrs, ptNeuron NewNeuron  )
  {
    int tmpCont;
    // Aloca espaço para um novo neurônio:
    ptNeuron tmpneuron = NewNeuron;
    // Criando o vetor de arestas:
    tmpneuron->Edges = (int*)malloc(numNbrs*sizeof(int));
    // Criando o vetor de ponteiros para vizinhos:
    tmpneuron->Nbrs = (ptNeuron*)malloc(numNbrs*sizeof(ptNeuron));

    // Incluíndo 0 nos pesos das arestas e NULL nos ponteiros:
    for( tmpCont = 0 ; tmpCont < numNbrs ; tmpCont++ )
    {
      tmpneuron->Edges[tmpCont] = -1; //< -1: não existe aresta.
      tmpneuron->Nbrs[tmpCont] = NULL;
    }

    // Alocando espaço para o vetor de pesos:
    NewEmptyVector(dim, &tmpneuron->Weights);
    // Inserindo valores aleatórios nos pesos do neurônio:
    for( tmpCont = 0 ; tmpCont < dim ; tmpCont++ )
      tmpneuron->Weights->values[tmpCont] = (float)((float)rand()/(float)RAND_MAX);
    // Número identificador:
    tmpneuron->Id = NewId;
    // Número de vezes em que foi vencedor:
    tmpneuron->AccQuadDist= 0;
    tmpneuron->nNbrs= numNbrs;
    //tmpneuron->IndNbrs= 0;
    NewNeuron = tmpneuron;

    return 0;  
  }


  /**
  * Deleta um neurônio anteriormente criado.
  * @param dNeuron Neurônio a ser deletado.
  * @return Retorna -1 em caso de neurônio nulo, ou 0 caso ok.
  */
  int DeleteNeuron( ptNeuron dNeuron)
  {
    if( dNeuron == NULL )
      return -1;

    // Deleta vetores alocados:
    free(dNeuron->Weights->values);
    free(dNeuron->Edges);
    free(dNeuron->Nbrs);
    // Deleta a estrutura do neurônio:
    free(dNeuron);
    return 0;
  }


  /**
  * Obtém o vetor que corresponde às ligações do neurônio.
  * @return Retorna o vetor que corresponde às ligações do neurônio.
  */
  ptMVector GetWeights( ptNeuron dNeuron )
  {
    return dNeuron->Weights;
  }

  /**
  * Obtém o número de vezes em que o neurônio foi vencedor.
  * @return Retorna o número de vezes em que o neurônio foi vencedor.
  */
  int GetNumWinners( ptNeuron dNeuron )
  {
    return dNeuron->NumWinners;
  }                                       

  /**
  * Insere o número de vezes em que um neurônio foi vencedor.
  * @param nWin Número de vezes a ser inserido.
  */
  void SetNumWinners( ptNeuron dNeuron, int nWin )
  {
    dNeuron->NumWinners = nWin;
  }

  /**
  * Incrementa o número de vezes que o neurônio foi vencedor.
  */
  void IncNumWinners( ptNeuron dNeuron )
  {
    dNeuron->NumWinners++;
  }







  /**
  * Cria uma nova rede de neurônios.
  * @param newGNG Nova instância da GNG a ser criada.
  * @param name Nome da rede a ser criada.
  * @param nneurons Número de neurônios desejado.
  * @param nlength Dimensão dos neurônios (número de sinapses).
  * @param lambda Número de treinamentos executados até que um novo neurônio seja incluído.
  * @param eb Taxa de aprendizado para o neurônio vencedor.
  * @param en Taxa de aprendizado para os vizinhos do neurônio vencedor.
  * @param alfa Taxa de desconto dos erros aplicada somente aos dois vencedores.
  * @param d Taxa de desconto dos erros aplicada a todos.
  * @param amax Peso máximo de uma aresta antes que ela seja removida.
  * @return 
  */
  int CreateGNG( ptGNG *newGNG, char* name, int nneurons, int nlength, 
                int lambda, float eb, float en, float alfa, float d, int amax, float *inWeights)
  {
    int cont;
    int cont2;
    ptNeuron tmp, tmp2;
    // Aloca memória para uma nova rede:
    ptGNG tmpGNG = (ptGNG)malloc(sizeof(GNG));

    // Cria o número de neurônios desejado:
    tmpGNG->lstNeurons = (ptNeuron*)malloc(nneurons*sizeof(ptNeuron));

    // Cria cada um dos neurônios da rede:
    for( cont=0 ; cont<nneurons ; cont++ )
    {
      // Pega o neurônio atual:
      //tmp = (tmpGNG->lstNeurons[cont]);
      CreateNeuron(cont, nlength, nneurons, &(tmpGNG->lstNeurons[cont]));
    }

    // Criando os dois vetores auxiliares utilizados no treinamento:
    NewEmptyVector(nlength, &(tmpGNG->aux1));

    // Insere os ponteiros dos vizinhos dos neurônios:
    for( cont=0 ; cont<nneurons ; cont++ )
    {
      // Pega o neurônio atual:
      tmp = (tmpGNG->lstNeurons[cont]);
      // Inserindo os valores corretos dos nneurons ponteiros para os 
      // vizinhos em cada um dos neurônios:
      for( cont2=0 ; cont2<nneurons ; cont2++ )
      {
        tmp2 = (tmpGNG->lstNeurons[cont2]);
        (tmpGNG->lstNeurons[cont])->Nbrs[cont2] = (tmpGNG->lstNeurons[cont2]);
      }
    }

    // Parâmetros obrigatórios do GNG:
    tmpGNG->IndNbrs = 0;     
    tmpGNG->Lambda = lambda;      
    tmpGNG->Eb = eb;          
    tmpGNG->En = en;          
    tmpGNG->Alfa = alfa;        
    tmpGNG->D = d;           
    tmpGNG->Amax = amax;        

    // Alocando demais campos da estrutura:
    tmpGNG->nTraineds = 0;
    tmpGNG->Nearest = NULL;
    tmpGNG->NearestWeights = NULL;
    tmpGNG->NearestIndex = -1;
    tmpGNG->SecondNearest = NULL;
    tmpGNG->SecondNearestWeights = NULL;
    tmpGNG->SecondNearestIndex = -1;
    tmpGNG->Name = strdup(name);
    tmpGNG->NNeurons = nneurons;
    tmpGNG->NeuronsLength = nlength;


    // Cria o vetor e insere os pesos, caso necessário:
    tmpGNG->weights = (float*)malloc(nlength*sizeof(float));
    if( inWeights )
    {
    for( int i=0 ; i<nlength ; i++ )
      tmpGNG->weights[i] = inWeights[i];
    }
    else
      tmpGNG->weights = NULL;


    // Retorna a estrutura criada:
    *newGNG = tmpGNG;
    return 0;
  }


  /**
  * Obtém a lista de neurônios da rede.
  * @return Retorna um iterador para a lista de neurônios da rede.
  */
  ptNeuron* GetListNeurons( ptGNG tmpGNG )
  {
    return tmpGNG->lstNeurons;
  }


  /**
  * Recebe o índice de um determinado neurônio e retira esse neurônio da rede.
  * @param ptGNG tmpGNG Rede sendo utilizada. 
  * @param index Índice do neurônio a ser retirado da rede.
  * @return Retorna -1 em caso de parâmetros incompatíveis, ou 0 caso ok.
  */
  int EraseNeuron( ptGNG tmpGNG, int index )
  {
    int cont;
    ptNeuron tmpNeuron2;
    ptNeuron erasedNeuron;
    ptNeuron posModifiedNeuron;
    int lastValid;
    // Casos de retorno:
    if( (tmpGNG == NULL) || (index > tmpGNG->NNeurons) )
      return -1;

    // Caso contrário, encontra o neurônio, deleta esse neurônio e 
    // insere o último neurônio válido no lugar do neurônio deletado:

    // Ponteiro temporário para o neurônio a ser retirado:
    erasedNeuron = tmpGNG->lstNeurons[index];
    
    // Pega o último neurônio válido, a ser incluído no lugar do retirado:
    lastValid = tmpGNG->IndNbrs-1;
    posModifiedNeuron = tmpGNG->lstNeurons[lastValid];

    // Percorre a lista de neurônios atualizando todos os ponteiros para
    // o neurônio remanejado de forma que esses ponteiros passem a apontar 
    // para o seu novo local. As arestas ligadas a esse neurônio também 
    // devem ter seu índice remanejado:
    for( cont=0 ; cont<tmpGNG->IndNbrs ; cont++ )
    {
      // Pega o neurônio atual:
      tmpNeuron2 = tmpGNG->lstNeurons[cont];

      // As arestas do neurônio atual para o remanejado devem ser 
      // atualizadas:
      tmpNeuron2->Edges[index] = tmpNeuron2->Edges[lastValid];
      // O ponteiro para o neurônio deve ser atualizado:
      tmpNeuron2->Nbrs[index] = posModifiedNeuron;
      tmpNeuron2->Nbrs[lastValid] = erasedNeuron;
    }

    // Invertendo as posições dos neurônios:
    tmpGNG->lstNeurons[index] = posModifiedNeuron;
    tmpGNG->lstNeurons[lastValid] = erasedNeuron;

    // Decrementa o valor do índice do último válido:
    tmpGNG->IndNbrs = tmpGNG->IndNbrs-1;

    return 0;
  }


  /**
  * Encontra as Best Match Units(BMU's, primeira e segunda).
  * @param tmpGNG Rede sendo utilizada.
  * @param Vec_In Amostra escolhida no vetor de probabilidades.
  */
  int Get_BMU_GNG( ptGNG tmpGNG, ptMVector Vec_In )
  {
    int index;
    float dist, BMUdist, SecondBMUdist;
    ptNeuron KNeuronlist;
    float tmpDistance;
    ptNeuron tmpNeuron;
    ptMVector tmpVector;
    int tmpIndex;

    // Inicializado variáveis:
    BMUdist = dist = SecondBMUdist = 1e8;
    KNeuronlist = NULL;

    // Percorre todos os neurônios:
    for( index=0 ; index<tmpGNG->IndNbrs ; index++ )
    {
      KNeuronlist = (tmpGNG->lstNeurons[index]);
      //tmpDistance = EuclideanDist(GetWeights(KNeuronlist), Vec_In);
      if( !tmpGNG->weights )
      {
        tmpDistance = QuadDist(GetWeights(KNeuronlist), Vec_In);
      }
      else
      {
        tmpDistance = QuadDistWeight(GetWeights(KNeuronlist), Vec_In, tmpGNG->weights);
      }

      // Caso a distância encontrada seja menor que pelo menos uma
      // das atuais:
      if( (tmpDistance < BMUdist) || (tmpDistance < SecondBMUdist) )
      {
        // Atualiza a maior dentre as duas distâncias:
        if( Maior(BMUdist, SecondBMUdist) == BMUdist)
        {
          // Atualiza a BMUdist:
          BMUdist = tmpDistance;
          // Atualiza os dados:
          tmpGNG->NearestIndex = index;
          tmpGNG->Nearest = KNeuronlist;
          tmpGNG->NearestWeights = GetWeights(KNeuronlist);
        }
        else
        {
          // Atualiza a SecondBMUdist:
          SecondBMUdist = tmpDistance;
          // Atualiza os dados:
          tmpGNG->SecondNearestIndex = index;
          tmpGNG->SecondNearest = KNeuronlist;
          tmpGNG->SecondNearestWeights = GetWeights(KNeuronlist);
        }
      }
    }

    // Pega a maior e a segunda maior e organiza, se necessário:
    tmpDistance = Maior(BMUdist, SecondBMUdist);
    // Caso a maior seja a BMUdist, elas tem que ser trocadas:
    if( tmpDistance == BMUdist )
    {
      // Troca as distâncias:
      BMUdist = SecondBMUdist;
      SecondBMUdist = tmpDistance;
      // Troca os ponteiros:
      // Armazena o primeiro:
      tmpIndex = tmpGNG->NearestIndex;
      tmpNeuron = tmpGNG->Nearest;
      tmpVector = tmpGNG->NearestWeights;
      // Insere os valores do segundo no primeiro:
      tmpGNG->NearestIndex = tmpGNG->SecondNearestIndex;
      tmpGNG->Nearest = tmpGNG->SecondNearest;
      tmpGNG->NearestWeights = tmpGNG->SecondNearestWeights;
      // Insere os antigos valores do primeiro no segundo:
      tmpGNG->SecondNearestIndex = tmpIndex;
      tmpGNG->SecondNearest = tmpNeuron;
      tmpGNG->SecondNearestWeights = tmpVector;
     } 

    // Incrementa o contador de distâncias:
    tmpGNG->Nearest->AccQuadDist+=BMUdist;
    tmpGNG->SecondNearest->AccQuadDist+=SecondBMUdist;

    // Incrementa o contador de treinamentos já realizados:
    tmpGNG->nTraineds++;

    return 0;
  }


  /**
  * Função de inclusão de um novo neurônio.
  * @param tmpGNG Instância do GNG (já criada) a ser treinada.
  * @param Vec_In Amostra sendo utilizada no treinamento atual.
  * @param 
  * @return
  */
  int IncludeNewNeuron( ptGNG tmpGNG, ptMVector Vec_In )
  {
    int cont;
    ptNeuron tmpNeuron;
    ptNeuron sdistsNeuron;
    ptNeuron sSeconddistNeuron;
    float sdists, sSeconddists;
    int winInd, secwinInd;
    int fstEpt;
    ptMVector tmpVector;
    ptMVector WinVector, SecWinVector;

    // Verificando a integridade da estrutura recebida:
    if((Vec_In == NULL) || (tmpGNG == NULL) )
      return -1;
    // Caso o número máximo de neurônios já tenha sido atingido, retorna -2:
    if( tmpGNG->NNeurons == tmpGNG->IndNbrs )
      return -2;

    // Inserindo zero nos índices dos vencedores:
    winInd = secwinInd = 0;
    // Percorre todos os neurônio válidos verificando qual deles 
    // possui o maior somatório de distâncias:
    sdists = 0;
    for( cont=0 ; cont<tmpGNG->IndNbrs ; cont++ )
    {
      // Neurônio atual a ser testado:
      tmpNeuron = tmpGNG->lstNeurons[cont];
      // Caso sua distância acumulada seja a maior até agora, 
      // esse neurônio é armazenado:
      if( tmpNeuron->AccQuadDist > sdists )
      {
        sdists = tmpNeuron->AccQuadDist;
        sdistsNeuron = tmpNeuron;
        winInd = cont;
      }
    }

    // Uma vez que o neurônio de maior erro foi encontrado
    // é preciso incluir um novo neurônio na sua vizinhança:
    // Encontrando dentre os vizinhos o que possui maior erro:
    sSeconddists = 0;
    for( cont=0 ; cont<tmpGNG->IndNbrs ; cont++ )
    {
      // Caso exista uma aresta para esse neurônio, ele é vizinho:
      if( sdistsNeuron->Edges[cont] >= 0 )
      {
        // Neurônio atual a ser testado:
        tmpNeuron = tmpGNG->lstNeurons[cont];
        // Caso sua distância acumulada seja a maior até agora, 
        // esse neurônio é armazenado:
        if( tmpNeuron->AccQuadDist > sSeconddists )
        {
          sSeconddists = tmpNeuron->AccQuadDist;
          sSeconddistNeuron = tmpNeuron;
          secwinInd = cont;
        }
      }
    }

    // Pegando a primeira posição vazia:
    fstEpt = tmpGNG->IndNbrs;
    tmpVector = tmpGNG->lstNeurons[fstEpt]->Weights;
    // Pegando os vetores dos vencedores:
    WinVector = sdistsNeuron->Weights;
    SecWinVector = sSeconddistNeuron->Weights;

    // Zerando o vetor do novo neurônio:
    EscalarMult( tmpVector, 0.0f); //< Zerando o vetor;
    Add( tmpVector, WinVector);
    Add( tmpVector, SecWinVector);
    // Posição média entre os dois vencedores:
    EscalarMult( tmpVector, 0.5f);
    // Ligando o novo neurônio aos dois vencedores:
    tmpGNG->lstNeurons[fstEpt]->Edges[winInd] = 0;
    tmpGNG->lstNeurons[fstEpt]->Edges[secwinInd] = 0;
    sdistsNeuron->Edges[fstEpt] = 0;
    sSeconddistNeuron->Edges[fstEpt] = 0;
    // Removendo as arestas que ligavam os dois vencedores:
    sdistsNeuron->Edges[secwinInd] = -1;
    sSeconddistNeuron->Edges[winInd] = -1;

    // Decrementando o erro dos vencedores:
    sdistsNeuron->AccQuadDist = sdistsNeuron->AccQuadDist* tmpGNG->Alfa;
    sSeconddistNeuron->AccQuadDist = sSeconddistNeuron->AccQuadDist* tmpGNG->Alfa;

    // O novo neurônio incluído deve receber o erro do pai de maior erro:
    tmpGNG->lstNeurons[fstEpt]->AccQuadDist = sdistsNeuron->AccQuadDist;

    // Incrementando o número de neurônios válidos:
    tmpGNG->IndNbrs++;

    return 0;
  }


  /**
  * Função de treinamento do GNG(Growing Neural Gas).
  * @param tmpGNG Instância do GNG (já criada) a ser treinada.
  * @param Vec_In Vetor contendo todas as amostras.
  * @param Probs Probabilidade de ocorrência de cada uma das amostras.
  * @param dim Dimensão de Vec_In e, consequentemente, de Probs.
  * @param Size Critério de parada. número máximo de neurônios que a rede pode ter.
  * @param RMS_Error Critério de parada. Erro médio quadrático a ser atingido. 
  * @return Retorna
  */
  int TrainGNG( ptGNG tmpGNG, ptMVector Vec_In, int dim, int *nTrains )
  {
    int auxcont;
    float prob;
    int ind;
    int index;
    ptNeuron tmpNeuron;
    ptNeuron tmpNeuron2;
    ptMVector tmpVector;
    int flag;
    int NumTreinos;

    // Verificando a integridade da estrutura recebida:
    if((Vec_In == NULL) || (tmpGNG == NULL) )
      return -2;
    // Verificando se o vetor recebido é de tamanho compatível:
    if( Vec_In[0].dim != tmpGNG->NeuronsLength )
      return -1;

    // Atualiza o número de neurônios existentes até aqui:
    // (começa com 2 neurônios):
    tmpGNG->IndNbrs = 2;

    // Enquanto o critério de parada não for atingido:
    NumTreinos = 0;
    while( tmpGNG->IndNbrs != tmpGNG->NNeurons ) 
    {
      // Escolhe uma entrada de acordo com a distribuição de probabilidades:
      prob = ((float)((float)rand()/(float)RAND_MAX));
      ind = (int)(prob*dim);

      // Escolhendo o vencedor:
      Get_BMU_GNG( tmpGNG, &Vec_In[ind] );

      // Incrementa todas as arestas emanando de Nearest (s1):
      tmpNeuron = tmpGNG->Nearest;
      for( index=0 ; index<tmpGNG->IndNbrs ; index++ )
      {
        // Caso a aresta exista, ela tem seu peso incrementado:
        if( tmpNeuron->Edges[index] >= 0)
          tmpNeuron->Edges[index]++;
      }

      /****************************************************************
        Epata de treinamento dos neurônios, tanto do vencedor quanto 
        dos seus vizinhos: 
      ****************************************************************/

      // Primeira etapa: o treinamento do neurônio vencedor:
      // Obtendo um ponteiro para as componentes do neurônio vencedor:
      tmpNeuron = tmpGNG->Nearest ;
      tmpVector = GetWeights(tmpNeuron);
      // Executando o treinamento:
      
      // Incluíndo a amostra em um dos vetores temporários da estrutura:
      InsertValues( tmpGNG->aux1, Vec_In[ind].values, Vec_In[ind].dim );
      EscalarMult( tmpGNG->aux1, tmpGNG->Eb );
      EscalarMult( tmpVector, (1.0f - tmpGNG->Eb));
      Add( tmpVector, tmpGNG->aux1 );

      // Segunda etapa: para cada um dos seus vizinhos, realizar o treinamento:
      for( index=0 ; index<tmpGNG->IndNbrs ; index++ )
      {
        // Caso exista uma aresta ligando, é vizinho e é treinado:
        if( tmpNeuron->Edges[index] >= 0 )
        {
          // Pegando um ponteiro para esse vizinho:
          tmpNeuron2 = tmpNeuron->Nbrs[index];
          tmpVector = GetWeights(tmpNeuron2);
          // Executando o treinamento:
          InsertValues( tmpGNG->aux1, Vec_In[ind].values, Vec_In[ind].dim );
          EscalarMult( tmpGNG->aux1, tmpGNG->En );
          EscalarMult( tmpVector, (1.0f - tmpGNG->En));
          Add( tmpVector, tmpGNG->aux1 );
        }
      }

      // Caso exista uma aresta ligando os dois vencedores, insere 0 como seu peso,
      // caso essa aresta ainda não exista, ela é criada:
      tmpNeuron->Edges[tmpGNG->SecondNearestIndex] = 0;
      tmpNeuron2 = tmpGNG->SecondNearest;
      tmpNeuron2->Edges[tmpGNG->NearestIndex] = 0;

      // Removendo todas as arestas que possuem peso maior ou igual a Amax. Caso 
      // depois da remoção das arestas algum neurônio fique isolado, esse neurônio
      // também deverá ser removido da rede:

      // Percorrendo o vetor de neurônios da rede:
      // Verificanco cada um dos neurônios:
      for( index=0 ; index<tmpGNG->IndNbrs ; index++ )
      {
        // Verifica o peso de cada uma das arestas:
        for( auxcont=0 ; auxcont < tmpGNG->IndNbrs ; auxcont++)
        {
          if( tmpGNG->lstNeurons[index]->Edges[auxcont] >= tmpGNG->Amax )
            tmpGNG->lstNeurons[index]->Edges[auxcont] = -1; //< Retira a aresta.
        }

        // Caso as arestas desse neurônio tenham sido totalmente removidas, 
        // ele deve ser removido também:
        flag = 0;
        for( auxcont=0 ; auxcont < tmpGNG->IndNbrs ; auxcont++)
        {
          if( tmpGNG->lstNeurons[index]->Edges[auxcont] >= 0 )
          {
            flag++;
            break;
          }
        }
        // Caso o neurônio não possua nenhuma aresta, é removido:
        if( flag == 0 )
          EraseNeuron( tmpGNG, index );
      }

      // Verifica se o número de treinamentos já atingiu o número máximo para que
      // seja incluído um novo neurônio:
      if( tmpGNG->nTraineds == tmpGNG->Lambda)
      {
        // Chama a função de inclusão de um novo neurônio:
        IncludeNewNeuron( tmpGNG, &Vec_In[ind] );
        // Zera o contador de treinos:
        tmpGNG->nTraineds = 0;
      }

      // Decrementa o erro de todas as variáveis multiplicando-os pelo parâmetro d:
      for( auxcont=0 ; auxcont < tmpGNG->IndNbrs ; auxcont++)
      {
        tmpGNG->lstNeurons[auxcont]->AccQuadDist =
          tmpGNG->lstNeurons[auxcont]->AccQuadDist * tmpGNG->D;
      }

      // Incrementa o número de treinamentos:
      NumTreinos++;
    }

    *nTrains = NumTreinos;
    return 0;
  }


  /**
  * Função para ser utilizada depois de executado o treinamento.
  * Recebe uma entrada específica e obtém o representante dessa entrada.
  * @param tmpGNG Instância do GNG (já criada) a ser treinada.
  * @param Vec_In Vetor contendo a amostra.
  * @param Winner Índice do neurônio vencedor, a ser retornado pela função
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int GetNearest( ptGNG tmpGNG, ptMVector Vec_In, int *Winner)
  {
    // Casos de retorno:
    if( (tmpGNG == NULL) || (Vec_In == NULL) || (tmpGNG->NeuronsLength != Vec_In->dim) )
      return -1;
    // Caso contrário, encontra o neurônio vencedor (mais próximo):
    Get_BMU_GNG( tmpGNG, Vec_In );
    // Pega o valor do vencedor e retorna:
    *Winner = tmpGNG->NearestIndex;
    return 0;
  }


  /**
  * Função de deleção de um GNG.
  * @param tmpGNG Instância a ser deletada.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int DeleteGNG( ptGNG tmpGNG )
  {
    int cont;
    // Casos de retorno:
    if (tmpGNG == NULL)
      return -1;

    // Percorre a lista de neurônios deletando cada um deles:
    for( cont=0 ; cont<tmpGNG->NNeurons ; cont++ )
      DeleteNeuron( tmpGNG->lstNeurons[cont] );

    // Deleta as demais estruturas alocadas:
    free( tmpGNG->aux1->values );
    free( tmpGNG->aux1 );
    free( tmpGNG->Name );

    // Deleta a estrutura:
    free(tmpGNG);

    return 0;
  }

#endif