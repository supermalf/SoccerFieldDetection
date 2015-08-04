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
* M�dulo da diferen�a entre dois n�meros.
*/
#define Modulo(a,b) ((a>=b)?(a-b):(b-a))

/**
* Maior entre dois n�meros.
*/
#define Maior(a, b) ((a>=b)?(a):(b))

/**
* Menor entre dois n�meros.
*/
#define Menor(a, b) ((a<b)?(a):(b))

/**
* Estrutura que representa um neur�nio. 
*/
typedef struct neuron
{
  int Id;               //< �ndice de identifica��o do neur�nio.
  ptMVector Weights;    //< Pesos neur�nio.
  int NumWinners;       //< N�mero de vezes em que foi vencedor.
  float AccQuadDist;    //< Dist�ncia quadr�tica acumulada.
  struct neuron* *Nbrs; //< Lista de vizinhos.
  int *Edges;           //< Lista de arestas.
  int nNbrs;            //< N�mero m�ximo de vizinhos e de arestas.

} Neuron, *ptNeuron;

/************************************************************************
Estrutura de dados que representa um GNG(Growing Neural Gas).
************************************************************************/
typedef struct gng
{
  char *Name;                     ///< Nome da inst�ncia.
  int NeuronsLength;              ///< Dimens�o.
  int NearestIndex;               ///< �ndice do �ltimo vencedor.
  ptNeuron Nearest;               ///< Ponteiro aponta para o �ltimo vencedor.
  ptMVector NearestWeights;       ///< Ponteiro para o vetor de pesos do �ltimo vencedor.
  int SecondNearestIndex;         ///< �ndice do �ltimo vencedor.
  ptNeuron SecondNearest;         ///< Ponteiro aponta para o �ltimo vencedor.
  ptMVector SecondNearestWeights; ///< Ponteiro para o vetor de pesos do �ltimo vencedor.
  int nTraineds;

  // Dois vetores auxiliares, apenas utilizados pela fun��o de treinamento:
  ptMVector aux1;

  // Par�metros obrigat�rios do GNG:
  int NNeurons;                 ///< N�mero total de neur�nios.
  ptNeuron *lstNeurons;         ///< Lista de neur�nios do mapa.
  int IndNbrs;                  ///< N�mero total de neur�nios existentes no momento.
  int Lambda;                   ///< Determina quantos treinos at� inclus�o do pr�ximo neur�nio.
  float Eb;                     ///< Taxa de aprendizado utilizada no treinamento do neur�nio vencedor.
  float En;                     ///< Taxa de aprendizado utilizada para os vizinhos do neur�nio vencedor.
  float Alfa;                   ///< Taxa de decrescimento do erro do neur�nio vencedor e seu vizinho.
  float D;                      ///< Taxa de decrescimento do erro geral dos neur�nios.
  int Amax;                     ///< Peso m�ximo permitido a uma aresta s/ que seja removida.

  // Esse vetor serve para que se possa modificar a fun��o de aprendizado hebbiado:
  float *weights;

} GNG, *ptGNG;


  /**
  * Cria um novo neur�nio.
  * @param NewId �ndice de identifica��o do neur�nio. 
  * @param dim N�mero de pesos (sinapses) que o neur�nio deve possuir.
  * @param numNbrs N�mero m�ximo de vizinhos que o neur�nio pode possuir.
  * @param NewNeuron Novo neur�nio retornado.
  * @return ...
  */
  int CreateNeuron( int NewId, int dim, int numNbrs, ptNeuron *NewNeuron  )
  {
    int tmpCont;
    // Aloca espa�o para um novo neur�nio:
    ptNeuron tmpneuron = (ptNeuron)malloc(sizeof(Neuron));
    // Criando o vetor de arestas:
    tmpneuron->Edges = (int*)malloc(numNbrs*sizeof(int));
    // Criando o vetor de ponteiros para vizinhos:
    tmpneuron->Nbrs = (ptNeuron*)malloc(numNbrs*sizeof(ptNeuron));

    // Inclu�ndo 0 nos pesos das arestas e NULL nos ponteiros:
    for( tmpCont = 0 ; tmpCont < numNbrs ; tmpCont++ )
    {
      tmpneuron->Edges[tmpCont] = -1; //< -1: n�o existe aresta.
      tmpneuron->Nbrs[tmpCont] = NULL;
    }

    // Alocando espa�o para o vetor de pesos:
    NewEmptyVector(dim, &tmpneuron->Weights);
    // Inserindo valores aleat�rios nos pesos do neur�nio:
    for( tmpCont = 0 ; tmpCont < dim ; tmpCont++ )
      tmpneuron->Weights->values[tmpCont] = (float)((float)rand()/(float)RAND_MAX);
    // N�mero identificador:
    tmpneuron->Id = NewId;
    // N�mero de vezes em que foi vencedor:
    tmpneuron->AccQuadDist= 0;
    tmpneuron->nNbrs= numNbrs;
    //tmpneuron->IndNbrs= 0;
    *NewNeuron = tmpneuron;

    return 0;
  }


  /**
  * Cria um novo neur�nio, sendo a estrutuda do neur�nio alocada externamente.
  * @param NewId �ndice de identifica��o do neur�nio. 
  * @param dim N�mero de pesos (sinapses) que o neur�nio deve possuir.
  * @param numNbrs N�mero m�ximo de vizinhos que o neur�nio pode possuir.
  * @param NewNeuron Novo neur�nio retornado.
  */
  int CreateNeuronAloc( int NewId, int dim, int numNbrs, ptNeuron NewNeuron  )
  {
    int tmpCont;
    // Aloca espa�o para um novo neur�nio:
    ptNeuron tmpneuron = NewNeuron;
    // Criando o vetor de arestas:
    tmpneuron->Edges = (int*)malloc(numNbrs*sizeof(int));
    // Criando o vetor de ponteiros para vizinhos:
    tmpneuron->Nbrs = (ptNeuron*)malloc(numNbrs*sizeof(ptNeuron));

    // Inclu�ndo 0 nos pesos das arestas e NULL nos ponteiros:
    for( tmpCont = 0 ; tmpCont < numNbrs ; tmpCont++ )
    {
      tmpneuron->Edges[tmpCont] = -1; //< -1: n�o existe aresta.
      tmpneuron->Nbrs[tmpCont] = NULL;
    }

    // Alocando espa�o para o vetor de pesos:
    NewEmptyVector(dim, &tmpneuron->Weights);
    // Inserindo valores aleat�rios nos pesos do neur�nio:
    for( tmpCont = 0 ; tmpCont < dim ; tmpCont++ )
      tmpneuron->Weights->values[tmpCont] = (float)((float)rand()/(float)RAND_MAX);
    // N�mero identificador:
    tmpneuron->Id = NewId;
    // N�mero de vezes em que foi vencedor:
    tmpneuron->AccQuadDist= 0;
    tmpneuron->nNbrs= numNbrs;
    //tmpneuron->IndNbrs= 0;
    NewNeuron = tmpneuron;

    return 0;  
  }


  /**
  * Deleta um neur�nio anteriormente criado.
  * @param dNeuron Neur�nio a ser deletado.
  * @return Retorna -1 em caso de neur�nio nulo, ou 0 caso ok.
  */
  int DeleteNeuron( ptNeuron dNeuron)
  {
    if( dNeuron == NULL )
      return -1;

    // Deleta vetores alocados:
    free(dNeuron->Weights->values);
    free(dNeuron->Edges);
    free(dNeuron->Nbrs);
    // Deleta a estrutura do neur�nio:
    free(dNeuron);
    return 0;
  }


  /**
  * Obt�m o vetor que corresponde �s liga��es do neur�nio.
  * @return Retorna o vetor que corresponde �s liga��es do neur�nio.
  */
  ptMVector GetWeights( ptNeuron dNeuron )
  {
    return dNeuron->Weights;
  }

  /**
  * Obt�m o n�mero de vezes em que o neur�nio foi vencedor.
  * @return Retorna o n�mero de vezes em que o neur�nio foi vencedor.
  */
  int GetNumWinners( ptNeuron dNeuron )
  {
    return dNeuron->NumWinners;
  }                                       

  /**
  * Insere o n�mero de vezes em que um neur�nio foi vencedor.
  * @param nWin N�mero de vezes a ser inserido.
  */
  void SetNumWinners( ptNeuron dNeuron, int nWin )
  {
    dNeuron->NumWinners = nWin;
  }

  /**
  * Incrementa o n�mero de vezes que o neur�nio foi vencedor.
  */
  void IncNumWinners( ptNeuron dNeuron )
  {
    dNeuron->NumWinners++;
  }







  /**
  * Cria uma nova rede de neur�nios.
  * @param newGNG Nova inst�ncia da GNG a ser criada.
  * @param name Nome da rede a ser criada.
  * @param nneurons N�mero de neur�nios desejado.
  * @param nlength Dimens�o dos neur�nios (n�mero de sinapses).
  * @param lambda N�mero de treinamentos executados at� que um novo neur�nio seja inclu�do.
  * @param eb Taxa de aprendizado para o neur�nio vencedor.
  * @param en Taxa de aprendizado para os vizinhos do neur�nio vencedor.
  * @param alfa Taxa de desconto dos erros aplicada somente aos dois vencedores.
  * @param d Taxa de desconto dos erros aplicada a todos.
  * @param amax Peso m�ximo de uma aresta antes que ela seja removida.
  * @return 
  */
  int CreateGNG( ptGNG *newGNG, char* name, int nneurons, int nlength, 
                int lambda, float eb, float en, float alfa, float d, int amax, float *inWeights)
  {
    int cont;
    int cont2;
    ptNeuron tmp, tmp2;
    // Aloca mem�ria para uma nova rede:
    ptGNG tmpGNG = (ptGNG)malloc(sizeof(GNG));

    // Cria o n�mero de neur�nios desejado:
    tmpGNG->lstNeurons = (ptNeuron*)malloc(nneurons*sizeof(ptNeuron));

    // Cria cada um dos neur�nios da rede:
    for( cont=0 ; cont<nneurons ; cont++ )
    {
      // Pega o neur�nio atual:
      //tmp = (tmpGNG->lstNeurons[cont]);
      CreateNeuron(cont, nlength, nneurons, &(tmpGNG->lstNeurons[cont]));
    }

    // Criando os dois vetores auxiliares utilizados no treinamento:
    NewEmptyVector(nlength, &(tmpGNG->aux1));

    // Insere os ponteiros dos vizinhos dos neur�nios:
    for( cont=0 ; cont<nneurons ; cont++ )
    {
      // Pega o neur�nio atual:
      tmp = (tmpGNG->lstNeurons[cont]);
      // Inserindo os valores corretos dos nneurons ponteiros para os 
      // vizinhos em cada um dos neur�nios:
      for( cont2=0 ; cont2<nneurons ; cont2++ )
      {
        tmp2 = (tmpGNG->lstNeurons[cont2]);
        (tmpGNG->lstNeurons[cont])->Nbrs[cont2] = (tmpGNG->lstNeurons[cont2]);
      }
    }

    // Par�metros obrigat�rios do GNG:
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


    // Cria o vetor e insere os pesos, caso necess�rio:
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
  * Obt�m a lista de neur�nios da rede.
  * @return Retorna um iterador para a lista de neur�nios da rede.
  */
  ptNeuron* GetListNeurons( ptGNG tmpGNG )
  {
    return tmpGNG->lstNeurons;
  }


  /**
  * Recebe o �ndice de um determinado neur�nio e retira esse neur�nio da rede.
  * @param ptGNG tmpGNG Rede sendo utilizada. 
  * @param index �ndice do neur�nio a ser retirado da rede.
  * @return Retorna -1 em caso de par�metros incompat�veis, ou 0 caso ok.
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

    // Caso contr�rio, encontra o neur�nio, deleta esse neur�nio e 
    // insere o �ltimo neur�nio v�lido no lugar do neur�nio deletado:

    // Ponteiro tempor�rio para o neur�nio a ser retirado:
    erasedNeuron = tmpGNG->lstNeurons[index];
    
    // Pega o �ltimo neur�nio v�lido, a ser inclu�do no lugar do retirado:
    lastValid = tmpGNG->IndNbrs-1;
    posModifiedNeuron = tmpGNG->lstNeurons[lastValid];

    // Percorre a lista de neur�nios atualizando todos os ponteiros para
    // o neur�nio remanejado de forma que esses ponteiros passem a apontar 
    // para o seu novo local. As arestas ligadas a esse neur�nio tamb�m 
    // devem ter seu �ndice remanejado:
    for( cont=0 ; cont<tmpGNG->IndNbrs ; cont++ )
    {
      // Pega o neur�nio atual:
      tmpNeuron2 = tmpGNG->lstNeurons[cont];

      // As arestas do neur�nio atual para o remanejado devem ser 
      // atualizadas:
      tmpNeuron2->Edges[index] = tmpNeuron2->Edges[lastValid];
      // O ponteiro para o neur�nio deve ser atualizado:
      tmpNeuron2->Nbrs[index] = posModifiedNeuron;
      tmpNeuron2->Nbrs[lastValid] = erasedNeuron;
    }

    // Invertendo as posi��es dos neur�nios:
    tmpGNG->lstNeurons[index] = posModifiedNeuron;
    tmpGNG->lstNeurons[lastValid] = erasedNeuron;

    // Decrementa o valor do �ndice do �ltimo v�lido:
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

    // Inicializado vari�veis:
    BMUdist = dist = SecondBMUdist = 1e8;
    KNeuronlist = NULL;

    // Percorre todos os neur�nios:
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

      // Caso a dist�ncia encontrada seja menor que pelo menos uma
      // das atuais:
      if( (tmpDistance < BMUdist) || (tmpDistance < SecondBMUdist) )
      {
        // Atualiza a maior dentre as duas dist�ncias:
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

    // Pega a maior e a segunda maior e organiza, se necess�rio:
    tmpDistance = Maior(BMUdist, SecondBMUdist);
    // Caso a maior seja a BMUdist, elas tem que ser trocadas:
    if( tmpDistance == BMUdist )
    {
      // Troca as dist�ncias:
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

    // Incrementa o contador de dist�ncias:
    tmpGNG->Nearest->AccQuadDist+=BMUdist;
    tmpGNG->SecondNearest->AccQuadDist+=SecondBMUdist;

    // Incrementa o contador de treinamentos j� realizados:
    tmpGNG->nTraineds++;

    return 0;
  }


  /**
  * Fun��o de inclus�o de um novo neur�nio.
  * @param tmpGNG Inst�ncia do GNG (j� criada) a ser treinada.
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
    // Caso o n�mero m�ximo de neur�nios j� tenha sido atingido, retorna -2:
    if( tmpGNG->NNeurons == tmpGNG->IndNbrs )
      return -2;

    // Inserindo zero nos �ndices dos vencedores:
    winInd = secwinInd = 0;
    // Percorre todos os neur�nio v�lidos verificando qual deles 
    // possui o maior somat�rio de dist�ncias:
    sdists = 0;
    for( cont=0 ; cont<tmpGNG->IndNbrs ; cont++ )
    {
      // Neur�nio atual a ser testado:
      tmpNeuron = tmpGNG->lstNeurons[cont];
      // Caso sua dist�ncia acumulada seja a maior at� agora, 
      // esse neur�nio � armazenado:
      if( tmpNeuron->AccQuadDist > sdists )
      {
        sdists = tmpNeuron->AccQuadDist;
        sdistsNeuron = tmpNeuron;
        winInd = cont;
      }
    }

    // Uma vez que o neur�nio de maior erro foi encontrado
    // � preciso incluir um novo neur�nio na sua vizinhan�a:
    // Encontrando dentre os vizinhos o que possui maior erro:
    sSeconddists = 0;
    for( cont=0 ; cont<tmpGNG->IndNbrs ; cont++ )
    {
      // Caso exista uma aresta para esse neur�nio, ele � vizinho:
      if( sdistsNeuron->Edges[cont] >= 0 )
      {
        // Neur�nio atual a ser testado:
        tmpNeuron = tmpGNG->lstNeurons[cont];
        // Caso sua dist�ncia acumulada seja a maior at� agora, 
        // esse neur�nio � armazenado:
        if( tmpNeuron->AccQuadDist > sSeconddists )
        {
          sSeconddists = tmpNeuron->AccQuadDist;
          sSeconddistNeuron = tmpNeuron;
          secwinInd = cont;
        }
      }
    }

    // Pegando a primeira posi��o vazia:
    fstEpt = tmpGNG->IndNbrs;
    tmpVector = tmpGNG->lstNeurons[fstEpt]->Weights;
    // Pegando os vetores dos vencedores:
    WinVector = sdistsNeuron->Weights;
    SecWinVector = sSeconddistNeuron->Weights;

    // Zerando o vetor do novo neur�nio:
    EscalarMult( tmpVector, 0.0f); //< Zerando o vetor;
    Add( tmpVector, WinVector);
    Add( tmpVector, SecWinVector);
    // Posi��o m�dia entre os dois vencedores:
    EscalarMult( tmpVector, 0.5f);
    // Ligando o novo neur�nio aos dois vencedores:
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

    // O novo neur�nio inclu�do deve receber o erro do pai de maior erro:
    tmpGNG->lstNeurons[fstEpt]->AccQuadDist = sdistsNeuron->AccQuadDist;

    // Incrementando o n�mero de neur�nios v�lidos:
    tmpGNG->IndNbrs++;

    return 0;
  }


  /**
  * Fun��o de treinamento do GNG(Growing Neural Gas).
  * @param tmpGNG Inst�ncia do GNG (j� criada) a ser treinada.
  * @param Vec_In Vetor contendo todas as amostras.
  * @param Probs Probabilidade de ocorr�ncia de cada uma das amostras.
  * @param dim Dimens�o de Vec_In e, consequentemente, de Probs.
  * @param Size Crit�rio de parada. n�mero m�ximo de neur�nios que a rede pode ter.
  * @param RMS_Error Crit�rio de parada. Erro m�dio quadr�tico a ser atingido. 
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
    // Verificando se o vetor recebido � de tamanho compat�vel:
    if( Vec_In[0].dim != tmpGNG->NeuronsLength )
      return -1;

    // Atualiza o n�mero de neur�nios existentes at� aqui:
    // (come�a com 2 neur�nios):
    tmpGNG->IndNbrs = 2;

    // Enquanto o crit�rio de parada n�o for atingido:
    NumTreinos = 0;
    while( tmpGNG->IndNbrs != tmpGNG->NNeurons ) 
    {
      // Escolhe uma entrada de acordo com a distribui��o de probabilidades:
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
        Epata de treinamento dos neur�nios, tanto do vencedor quanto 
        dos seus vizinhos: 
      ****************************************************************/

      // Primeira etapa: o treinamento do neur�nio vencedor:
      // Obtendo um ponteiro para as componentes do neur�nio vencedor:
      tmpNeuron = tmpGNG->Nearest ;
      tmpVector = GetWeights(tmpNeuron);
      // Executando o treinamento:
      
      // Inclu�ndo a amostra em um dos vetores tempor�rios da estrutura:
      InsertValues( tmpGNG->aux1, Vec_In[ind].values, Vec_In[ind].dim );
      EscalarMult( tmpGNG->aux1, tmpGNG->Eb );
      EscalarMult( tmpVector, (1.0f - tmpGNG->Eb));
      Add( tmpVector, tmpGNG->aux1 );

      // Segunda etapa: para cada um dos seus vizinhos, realizar o treinamento:
      for( index=0 ; index<tmpGNG->IndNbrs ; index++ )
      {
        // Caso exista uma aresta ligando, � vizinho e � treinado:
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
      // caso essa aresta ainda n�o exista, ela � criada:
      tmpNeuron->Edges[tmpGNG->SecondNearestIndex] = 0;
      tmpNeuron2 = tmpGNG->SecondNearest;
      tmpNeuron2->Edges[tmpGNG->NearestIndex] = 0;

      // Removendo todas as arestas que possuem peso maior ou igual a Amax. Caso 
      // depois da remo��o das arestas algum neur�nio fique isolado, esse neur�nio
      // tamb�m dever� ser removido da rede:

      // Percorrendo o vetor de neur�nios da rede:
      // Verificanco cada um dos neur�nios:
      for( index=0 ; index<tmpGNG->IndNbrs ; index++ )
      {
        // Verifica o peso de cada uma das arestas:
        for( auxcont=0 ; auxcont < tmpGNG->IndNbrs ; auxcont++)
        {
          if( tmpGNG->lstNeurons[index]->Edges[auxcont] >= tmpGNG->Amax )
            tmpGNG->lstNeurons[index]->Edges[auxcont] = -1; //< Retira a aresta.
        }

        // Caso as arestas desse neur�nio tenham sido totalmente removidas, 
        // ele deve ser removido tamb�m:
        flag = 0;
        for( auxcont=0 ; auxcont < tmpGNG->IndNbrs ; auxcont++)
        {
          if( tmpGNG->lstNeurons[index]->Edges[auxcont] >= 0 )
          {
            flag++;
            break;
          }
        }
        // Caso o neur�nio n�o possua nenhuma aresta, � removido:
        if( flag == 0 )
          EraseNeuron( tmpGNG, index );
      }

      // Verifica se o n�mero de treinamentos j� atingiu o n�mero m�ximo para que
      // seja inclu�do um novo neur�nio:
      if( tmpGNG->nTraineds == tmpGNG->Lambda)
      {
        // Chama a fun��o de inclus�o de um novo neur�nio:
        IncludeNewNeuron( tmpGNG, &Vec_In[ind] );
        // Zera o contador de treinos:
        tmpGNG->nTraineds = 0;
      }

      // Decrementa o erro de todas as vari�veis multiplicando-os pelo par�metro d:
      for( auxcont=0 ; auxcont < tmpGNG->IndNbrs ; auxcont++)
      {
        tmpGNG->lstNeurons[auxcont]->AccQuadDist =
          tmpGNG->lstNeurons[auxcont]->AccQuadDist * tmpGNG->D;
      }

      // Incrementa o n�mero de treinamentos:
      NumTreinos++;
    }

    *nTrains = NumTreinos;
    return 0;
  }


  /**
  * Fun��o para ser utilizada depois de executado o treinamento.
  * Recebe uma entrada espec�fica e obt�m o representante dessa entrada.
  * @param tmpGNG Inst�ncia do GNG (j� criada) a ser treinada.
  * @param Vec_In Vetor contendo a amostra.
  * @param Winner �ndice do neur�nio vencedor, a ser retornado pela fun��o
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int GetNearest( ptGNG tmpGNG, ptMVector Vec_In, int *Winner)
  {
    // Casos de retorno:
    if( (tmpGNG == NULL) || (Vec_In == NULL) || (tmpGNG->NeuronsLength != Vec_In->dim) )
      return -1;
    // Caso contr�rio, encontra o neur�nio vencedor (mais pr�ximo):
    Get_BMU_GNG( tmpGNG, Vec_In );
    // Pega o valor do vencedor e retorna:
    *Winner = tmpGNG->NearestIndex;
    return 0;
  }


  /**
  * Fun��o de dele��o de um GNG.
  * @param tmpGNG Inst�ncia a ser deletada.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int DeleteGNG( ptGNG tmpGNG )
  {
    int cont;
    // Casos de retorno:
    if (tmpGNG == NULL)
      return -1;

    // Percorre a lista de neur�nios deletando cada um deles:
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