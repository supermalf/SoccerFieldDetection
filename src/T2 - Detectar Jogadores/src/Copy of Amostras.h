//#include <stdio.h>
//#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "Util.h"

/*
#include <im.h>
#include <im_capture.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_format_avi.h>
*/


/**
* Estrutura de criação dos vetores de amostras.
*/
class Amostras
{

public:
  int _maxPts;          ///< Limite máximo de amostras que podem atualmente ser armazenados na estrutura.
  int _nPts;            ///< Número de amostras efetivamente sendo armazenados na estrutura.
  int _incSize;         ///< 	

  int _samplesWidth;    ///< Largura de cada uma das amostras de entrada.
  int _samplesHeight;   ///< Altura de cada uma das amostras de entrada.

  // Matrizes CONTENDO OS PIXELS DAS 3 COMPONENTES JUNTOS:
  int _inSize;          ///< Dimensão de cada uma das amostras de entrada. (samplesWidth)*(samplesHeight)*3
  int _outSize;         ///< Dimensão de cada uma das amostras de saída. (samplesWidth)*(samplesHeight)*3
  float **_inMatrix;    ///< Matriz contendo todas as amostras de entrada. M[_nPts][_inSize].
  float **_outMatrix;   ///< Matriz contendo todas as amostras de saída. M[_nPts][_outSize].

  // Matrizes CONTENDO AS 3 COMPONENTES DO PIXEL CENTRAL DE CADA UMA DAS AMOSTRAS DE ENTRADA:
  float **_out_R_PixelsMatrix; ///< Matriz contendo a componente R do pixel central referente à cada entrada.
  float **_out_G_PixelsMatrix; ///< Matriz contendo a componente G do pixel central referente à cada entrada.
  float **_out_B_PixelsMatrix; ///< Matriz contendo a componente B do pixel central referente à cada entrada.
  
  // Matrizes CONTENDO OS PIXELS DAS 3 COMPONENTES EM SEPADADO:
  int _inComponentSize;   ///< Dimensão de cada uma das amostras de entrada. (samplesWidth)*(samplesHeight)
  float **_in_R_Matrix;   ///< Matriz contendo todas as amostras de componente R de entrada.
  float **_in_G_Matrix;   ///< Matriz contendo todas as amostras de componente G de entrada.
  float **_in_B_Matrix;   ///< Matriz contendo todas as amostras de componente B de entrada.

  // Variáveis que armazenam as dimensões de cada amostra das 6 matrizes abaixo:
  int _inStatisticsSize;
  // Matrizes CONTENDO AS VARIÂNCIAS DAS COMPONENTES EM SEPARADO:
  float **_in_VarR_Matrix;  ///< Matriz contendo todas as amostras R de entrada de componente R.
  float **_in_VarG_Matrix;  ///< Matriz contendo todas as amostras R de entrada de componente R.
  float **_in_VarB_Matrix;  ///< Matriz contendo todas as amostras R de entrada de componente R.

  // Matrizes CONTENDO AS MEDIAS DAS COMPONENTES EM SEPARADO:
  float **_in_MeanR_Matrix; ///< Matriz contendo todas as amostras R de entrada de componente R.
  float **_in_MeanG_Matrix; ///< Matriz contendo todas as amostras R de entrada de componente R.
  float **_in_MeanB_Matrix; ///< Matriz contendo todas as amostras R de entrada de componente R.

  // DADOS REFERENTES A IMAGEM DE ENTRADA DE ONDE SERÃO CONSTRUÍDAS AS AMOSTRAS:
  unsigned char *data;  ///< Vetor contendo as compontentes RGB de todos os pixels da imagem de entrada.
  int _height;          ///< Altura da imagem de entrada.
  int _width;           ///< Largura da imagem de entrada.

  // Máscara da imagem de entrada:
  int **_masc;
  int _mascXdim;
  int _mascYdim;

  // Vetores TEMPORÁRIOS para armazenar temporariamente as componentes RGB das amostras de entrada NUM MESMO VETOR:
  float *_mascSample;
  // Vetores TEMPORÁRIOS para armazenar EM SEPARADO temporariamente as componentes RGB das amostras de entrada:
  float *_in_R_Vector;
  float *_in_G_Vector;
  float *_in_B_Vector;
  // Vetores TEMPORÁRIOS para armazenar temporariamente as variâncias de cada uma das janelas das amostras de entrada:
  float *_in_VarR_Vector;
  float *_in_VarG_Vector;
  float *_in_VarB_Vector;
  // Vetores TEMPORÁRIOS para armazenar temporariamente as médias de cada uma das janelas das amostras de entrada:
  float *_in_MeanR_Vector;
  float *_in_MeanG_Vector;
  float *_in_MeanB_Vector;
  // Vetores TEMPORÁRIOS para armazenar temporariamente as três componentes (RGB) do pixel central de cada amostra:
  float* _out_R_PixelsVector;
  float* _out_G_PixelsVector;
  float* _out_B_PixelsVector;




  /**
  * Cria uma amostra Sld para ser utilizada externamente. Cria um vetor que irá conter as componentes
  * RGB de todos os pixels da amostra, desde o ponto Xmin,Ymin até o ponto Xmax, Ymax (inclusive).
  * @param imgSmpExtXmin Coordenada X do ponto inicial dessa amostra na imagem de entrada.
  * @param imgSmpExtYmin Coordenada Y do ponto inicial dessa amostra na imagem de entrada.
  * @param imgSmpExtXmax Coordenada X do ponto final dessa amostra na imagem de entrada.
  * @param imgSmpExtYmax Coordenada Y do ponto final dessa amostra na imagem de entrada.
  * @param imgSmp_R_Vector Vetor onde os valores das componentes R dos pixels da amostra serão inseridos.
  * @param imgSmp_G_Vector Vetor onde os valores das componentes R dos pixels da amostra serão inseridos.
  * @param imgSmp_B_Vector Vetor onde os valores das componentes R dos pixels da amostra serão inseridos.
  * @param imgSmp_R_MeanVector
  * @param imgSmp_G_MeanVector
  * @param imgSmp_B_MeanVector
  * @param imgSmp_R_VarVector
  * @param imgSmp_G_VarVector
  * @param imgSmp_B_VarVector
  * @return Retorna -1 em caso de erro (imagem inexistente, corrdenadas inexistentes, etc), ou 0 caso ok.
  */
  int imgSmpSeparateComponentSampleCreate( int imgSmpExtXmin, int imgSmpExtYmin, int imgSmpExtXmax, int imgSmpExtYmax, 
                                            float** imgSmp_R_Vector, float** imgSmp_G_Vector, float** imgSmp_B_Vector, 
                                            float** imgSmp_R_MeanVector, float** imgSmp_G_MeanVector, float** imgSmp_B_MeanVector,
                                            float** imgSmp_R_VarVector, float** imgSmp_G_VarVector, float** imgSmp_B_VarVector )
  {
    if( (imgSmpExtXmin < 0) || (imgSmpExtXmax >= _width ) )
      return -1;
    if( (imgSmpExtYmin < 0) || (imgSmpExtYmax >= _height ) )
      return -1;

    // Dimensão de cada um dos vetores (R, G, B):
    int smpDimension = _inComponentSize;

    // Vetor temporário contendo as componentes:
    float* imgSmp_R_Vectortmp = *imgSmp_R_Vector;
    float* imgSmp_G_Vectortmp = *imgSmp_G_Vector;
    float* imgSmp_B_Vectortmp = *imgSmp_B_Vector;

    // Vetor temporário contendo as médias com tamanho da janela variável:
    float* imgSmp_R_MeanVectortmp = *imgSmp_R_MeanVector;
    float* imgSmp_G_MeanVectortmp = *imgSmp_G_MeanVector;
    float* imgSmp_B_MeanVectortmp = *imgSmp_B_MeanVector;

    // Vetor temporário contendo as variâncias com tamanho de janela variável:
    float* imgSmp_R_VarVectortmp = *imgSmp_R_VarVector;
    float* imgSmp_G_VarVectortmp = *imgSmp_G_VarVector;
    float* imgSmp_B_VarVectortmp = *imgSmp_B_VarVector;


    // Gerando as amostras de cores com as 3 componentes em vetores separados:
    int pos=0;
    for( int j=imgSmpExtYmin ; j<=imgSmpExtYmax ; j++ )
    {
      for( int i=imgSmpExtXmin ; i<=imgSmpExtXmax ; i++ )
      {
        unsigned char retR, retG, retB;
        amsSmpInImageGetPixel( i, j, &retR, &retG, &retB );
        // Inserindo os valores no vetor de componentes:
        imgSmp_R_Vectortmp[pos] = (float)((float)retR/255);
        imgSmp_G_Vectortmp[pos] = (float)((float)retG/255);
        imgSmp_B_Vectortmp[pos] = (float)((float)retB/255);
        pos++;
      }
    }

    // Gerando as estatísticas (MÉDIA):
    float meanR=0, meanG=0, meanB=0;
    pos = 0;    //< O índice da posição utilizada do vetor é setado novamente em zero.
    
    // Gerando as estatísticas (média e variância):
    for( int k=0 ; k<_inStatisticsSize ; k++ )
    {
      meanR=0;
      meanG=0;
      meanB=0;
      int numPixels = 0;
      for( int j=imgSmpExtYmin+k ; j<=imgSmpExtYmax-k ; j++ )
      {
        for( int i=imgSmpExtXmin+k ; i<=imgSmpExtXmax-k ; i++ )
        {
          unsigned char retR, retG, retB;
          amsSmpInImageGetPixel( i, j, &retR, &retG, &retB );
          // Obtendo os valores das três componentes, já convertido para float:
          meanR += (float)((float)retR/255);
          meanG += (float)((float)retG/255);
          meanB += (float)((float)retB/255);
          numPixels++;
        }
      }
      imgSmp_R_MeanVectortmp[pos] = meanR/numPixels;
      imgSmp_G_MeanVectortmp[pos] = meanG/numPixels;
      imgSmp_B_MeanVectortmp[pos] = meanB/numPixels;
      pos++;
    }

    // Gerando as estatísticas (VARIÂNCIA):
    float varR=0, varG=0, varB=0;
    pos = 0;    //< O índice da posição utilizada do vetor é setado novamente em zero.
    // Gerando as estatísticas (média e variância):
    for( int k=0 ; k<_inStatisticsSize ; k++ )
    {
      varR=0;
      varG=0;
      varB=0;
      int numPixels = 0;
      for( int j=imgSmpExtYmin+k ; j<=imgSmpExtYmax-k ; j++ )
      {
        for( int i=imgSmpExtXmin+k ; i<=imgSmpExtXmax-k ; i++ )
        {
          unsigned char retR, retG, retB;
          amsSmpInImageGetPixel( i, j, &retR, &retG, &retB );
          // Obtendo os valores das três componentes, já convertido para float:
          varR += ((float)((float)retR/255) - imgSmp_R_MeanVectortmp[pos]) * ((float)((float)retR/255) - imgSmp_R_MeanVectortmp[pos]);
          varG += ((float)((float)retG/255) - imgSmp_G_MeanVectortmp[pos]) * ((float)((float)retG/255) - imgSmp_G_MeanVectortmp[pos]);
          varB += ((float)((float)retB/255) - imgSmp_B_MeanVectortmp[pos]) * ((float)((float)retB/255) - imgSmp_B_MeanVectortmp[pos]);
          numPixels++;
        }
      }
      imgSmp_R_VarVectortmp[pos] = varR/numPixels;
      imgSmp_G_VarVectortmp[pos] = varG/numPixels;
      imgSmp_B_VarVectortmp[pos] = varB/numPixels;
      pos++;
    }

    // Coordenadas do pixel central da amostra:
    int j1 = (imgSmpExtYmin + imgSmpExtYmax)/2;
    int i1 = (imgSmpExtXmin + imgSmpExtXmax)/2;
    unsigned char retR, retG, retB;
    amsSmpInImageGetPixel( i1, j1, &retR, &retG, &retB );
    // Obtendo os valores das três componentes, já convertido para float:
    // ESSAS TRÊS VARIÁVEIS SÃO DA PRÓPRIA CLASSE, POR ISSO NÃO SÃO RECEBIDOS COMO PARÂMETROS:
    _out_R_PixelsVector[0] = (float)((float)retR/255);
    _out_G_PixelsVector[0] = (float)((float)retG/255);
    _out_B_PixelsVector[0] = (float)((float)retB/255);

    return 0;
  }




  /** 
  * Obtém um pixel da imagem de entrada. Todo o cálculo para encontrar o pixel desejado é 
  * feito baseando-se no fato de que a instância de imImage recebida foi recebida como BitMap.
  * As coordenadas devem ser recebidas como bottom up. 
  * @param x Coordenada x do pixel a ser retornado.
  * @param y Coordenada y do pixel a ser retornado.
  * @param retR Componente R do pixel retornada.
  * @param retG Componente G do pixel retornada.
  * @param retB Componente B do pixel retornada.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int amsSmpInImageGetPixel( int x, int y, unsigned char* retR, unsigned char* retG, unsigned char* retB )
  {
    // Caso de retorno (imagem de entrada indexistente):
    if( !data )
      return -1;

    int h = _height;
    int w = _width;


    // Caso de retorno (coordenada requerida inexistente):
    if( (x>=w) || (y>=h) )
      return -1;

    // ImImages possuem pixels em bottom up na estrutura. Além disso, as componentes R, G e B são IM_PACKED:

    // Encontrando a posição do pixel no vetor:
    int pos = (y*w +x);     //< é IM_PACKED.
    // Carregando as componentes do pixel requerido:
    * retR = data[3*pos];
    * retG = data[3*pos+1];
    * retB = data[3*pos+2];
    return 0;
  }


  /**
  * Cria uma amostra Sld para ser utilizada externamente. Cria um vetor que irá conter as componentes
  * RGB de todos os pixels da amostra, desde o ponto Xmin,Ymin até o ponto Xmax, Ymax (inclusive).
  * @param imgSmpExtXmin Coordenada X do ponto inicial dessa amostra na imagem de entrada.
  * @param imgSmpExtYmin Coordenada Y do ponto inicial dessa amostra na imagem de entrada.
  * @param imgSmpExtXmax Coordenada X do ponto inicial dessa amostra na imagem de saída.
  * @param imgSmpExtYmax Coordenada Y do ponto inicial dessa amostra na imagem de saída.
  * @param imgSmpExtVector Vetor onde os pixels das três componentes serão inseridos.
  * @return Retorna -1 em caso de erro (imagem inexistente, corrdenadas inexistentes, etc), ou 0 caso ok.
  */
  
  int imgSmpExtSampleCreate( int imgSmpExtXmin, int imgSmpExtYmin, int imgSmpExtXmax, int imgSmpExtYmax, float** imgSmpExtVector )
  {
    if( (imgSmpExtXmin < 0) || (imgSmpExtXmax > 719 /* width */) )
      return -1;
    if( (imgSmpExtYmin < 0) || (imgSmpExtYmax > 485 /* height */) )
      return -1;
    int smpDimension = _inSize;

    float *imgSmpExtSample = *imgSmpExtVector;
    int pos=0;
    int initpos = smpDimension/3;
    for( int j=imgSmpExtYmin ; j<=imgSmpExtYmax ; j++ )
    {
      for( int i=imgSmpExtXmin ; i<=imgSmpExtXmax ; i++ )
      {
        unsigned char retR, retG, retB;
        amsSmpInImageGetPixel( i, j, &retR, &retG, &retB );
        imgSmpExtSample[pos] = (float)((float)retR/255);
        imgSmpExtSample[initpos+pos] = (float)((float)retG/255);
        imgSmpExtSample[2*initpos+pos] = (float)((float)retB/255);
        pos++;
      }
    }

    return 0;
  }


  /**
  * Insere a máscara.
  */
  int amsSetMasc( int **masc, int mascXdim, int mascYdim )
  {
    if( !masc )
      return -1;

    // Alocando espaço para o vetor auxiliar de criação das amostras:
    //_mascSample = new float[_inSize];
    _masc     = masc;
    _mascXdim = mascXdim;
    _mascYdim = mascYdim;
    return 0;
  }


  /**
  * Construtor.
  * @param maxPts Número máximo de amostras de entrada que poderão inicialmente serem inseridos na estrutura.
  * Haverá realocação de memória sempre que necessário.
  * @param samplesWidth Largura de cada uma das amostras de entrada que serão geradas.
  * @param samplesHeight Altura de cada uma das amostras de entrada que serão geradas.
  * @param outSize Tamanho de cada uma das amostras de saída
  * @param incSize Incremento no número de amostras a ser acrescentado no vetor de amostras de entrada, caso 
  * seja necessária a realocação em algum momento futuro.
  * @param width Largura da imagem de entrada.
  * @param height Altura da imagem de entrada.
  * @param mydata Dados referentes aos pixels da imagem de entrada.
  */
  Amostras( int maxPts, int samplesWidth, int samplesHeight, int outSize, int incSize, int width, int height, 
            unsigned char *mydata)
  {
    // Inserindo os dados referentes à imagem de entrada:
    data = mydata;
    _width = width;
    _height = height;

    // Tamanho de cada uma das amostras (R+G+B):
    int inSize = (samplesWidth)*(samplesHeight)*3;

    // Tamanho em x e y das amostras que serão geradas:
    _samplesWidth = samplesWidth;
    _samplesHeight = samplesHeight;

    // Demais parâmetros da estrutura:
    _maxPts   = maxPts;
    _inSize   = inSize;
    _outSize  = outSize;
    _incSize  = incSize;

    // Alocando as matrizes de amostras iniciais de entrada e saída:
    _inMatrix = new float*[maxPts];
    _outMatrix = new float*[maxPts];
    for( int i=0 ; i<maxPts ; i++ )
    {
      _inMatrix[i]  = new float[inSize];
      _outMatrix[i] = new float[outSize];
    }

    // Alocando espaço para as matrizes que irão conter cada um dos pixels centrais das amostras:
    _out_R_PixelsMatrix = new float*[maxPts];
    _out_G_PixelsMatrix = new float*[maxPts];
    _out_B_PixelsMatrix = new float*[maxPts];
    for( int i=0 ; i<maxPts ; i++ )
    {
      // Cada posição irá conter apenas a componente R, G, ou B do pixel:
      _out_R_PixelsMatrix[i] = new float[1];
      _out_G_PixelsMatrix[i] = new float[1];
      _out_B_PixelsMatrix[i] = new float[1];
    }

    // Alocando os outros dados da estrutura:
    // Mantém o tamanho de cada uma das amostras das componentes R, G e B separadas:
    _inComponentSize = _samplesWidth*_samplesHeight;    

    // Mantém o tamanho de cada uma das amostras de média e variância das amostras:
    _inStatisticsSize = Minor(_samplesWidth, _samplesHeight);
    _inStatisticsSize = _inStatisticsSize/2;

    // Alocando as demais matrizes:
    // 1. Matrizes contendo as amostras em componentes separadas:
    _in_R_Matrix = new float*[maxPts];
    _in_G_Matrix = new float*[maxPts];
    _in_B_Matrix = new float*[maxPts];
    // 2. Matrizes das variâncias das amostras, levando em contra janelas de tamanho 1, 2, ..., _inStatisticsSize:
    _in_VarR_Matrix = new float*[maxPts];
    _in_VarG_Matrix = new float*[maxPts];
    _in_VarB_Matrix = new float*[maxPts];
    // 3. Matrizes das médias das amostras, levando em contra janelas de tamanho 1, 2, ..., _inStatisticsSize:
    _in_MeanR_Matrix = new float*[maxPts];
    _in_MeanG_Matrix = new float*[maxPts];
    _in_MeanB_Matrix = new float*[maxPts];

    for( int i=0 ; i< maxPts ; i++ )
    {
      _in_R_Matrix[i] = new float[_inComponentSize];
      _in_G_Matrix[i] = new float[_inComponentSize];
      _in_B_Matrix[i] = new float[_inComponentSize];

      _in_VarR_Matrix[i] = new float[_inStatisticsSize];
      _in_VarG_Matrix[i] = new float[_inStatisticsSize];
      _in_VarB_Matrix[i] = new float[_inStatisticsSize];

      _in_MeanR_Matrix[i] = new float[_inStatisticsSize];
      _in_MeanG_Matrix[i] = new float[_inStatisticsSize];
      _in_MeanB_Matrix[i] = new float[_inStatisticsSize];
    }
    
    // Ainda não existem amostras na estrutura:
    _nPts = 0;

    // A máscara será repassada posteriormente:
    _masc = NULL;
    _mascXdim = 0;
    _mascYdim = 0;

    // Vetores TEMPORÁRIOS utilizados apenas para manter cada amostra enquanto estiver sendo criada:
    // Alocando o vetor temporário auxiliar na criação de cada uma das amostras:
    _mascSample = new float[inSize];
    // Vetores TEMPORÁRIOS para armazenar temporariamente as componentes RGB das amostras de entrada:
    _in_R_Vector = new float[_inComponentSize];
    _in_G_Vector = new float[_inComponentSize];
    _in_B_Vector = new float[_inComponentSize];
    // Vetores TEMPORÁRIOS para armazenar temporariamente as variâncias de cada uma das janelas das amostras de entrada:
    _in_VarR_Vector = new float[_inStatisticsSize];
    _in_VarG_Vector = new float[_inStatisticsSize];
    _in_VarB_Vector = new float[_inStatisticsSize];
    // Vetores TEMPORÁRIOS para armazenar temporariamente as médias de cada uma das janelas das amostras de entrada:
    _in_MeanR_Vector = new float[_inStatisticsSize];
    _in_MeanG_Vector = new float[_inStatisticsSize];
    _in_MeanB_Vector = new float[_inStatisticsSize];
    // Vetores TEMPORÁRIOS para armazenar temporariamente as três componentes (RGB) do pixel central de cada amostra:
    _out_R_PixelsVector = new float[1];
    _out_G_PixelsVector = new float[1];
    _out_B_PixelsVector = new float[1];

  }



  /**
  * Destrutor.
  */
  ~Amostras()
  {
    delete _inMatrix;
    delete _outMatrix;
    delete _in_R_Matrix;
    delete _in_G_Matrix;
    delete _in_B_Matrix;
    delete _in_VarR_Matrix;
    delete _in_VarG_Matrix;
    delete _in_VarB_Matrix;
    delete _in_MeanR_Matrix;
    delete _in_MeanG_Matrix;
    delete _in_MeanB_Matrix;
    delete _mascSample;
    delete _in_R_Vector;
    delete _in_G_Vector;
    delete _in_B_Vector;
    delete _in_VarR_Vector;
    delete _in_VarG_Vector;
    delete _in_VarB_Vector;
    delete _in_MeanR_Vector;
    delete _in_MeanG_Vector;
    delete _in_MeanB_Vector;
    delete _out_R_PixelsVector;
    delete _out_G_PixelsVector;
    delete _out_B_PixelsVector;
  }


  /**
  * Função auxiliar. Copia o valor de um vetor para dentro de uma determinada posição de uma matriz que
  * tenha o número de colunas de mesma dimensão desse vetor.
  * @param inVector Vetor de entrada da amostra a ser copiado para a matriz.
  * @param inVectorSize Dimensão do vetor recebido.
  * @param inMatrix Matriz na qual os valores contidos no vetor serão inseridos.
  * @param inMatrixXdim Dimensão da porção da matriz para onde o vetor será copiado.
  * @param inMatrixYPos Linha da matriz na qual o vetor será inserido.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int MatrixInsertVector( float *inVector, int inVectorSize, float** inMatrix, int inMatrixXdim, int inMatrixYPos )
  {
    // Casos retorno:
    if( (!inVector) || (!inMatrix) )
      return -1;
    if( inVectorSize != inMatrixXdim )
      return -1;

    // Copia os valores do vetor para a posição apropriada da matriz:
    for( int i=0 ; i<inVectorSize ; i++ )
      inMatrix[inMatrixYPos][i] = inVector[i];

    return 0;
  }



  /**
  * Insere uma nova amostra na estrutura.
  * @param inVector Vetor de entrada da amostra a ser copiado para a estrutura.
  * @param outVector Vetor de saída da amostra, a ser copiado para a estrutura.
  * @param inVectorSize Dimensão do vetor de entrada.
  * @param outVectorSize Dimensão do vetor de saída.
  * @return Retorna -1 em caso de erro (vetores incompatíveis, etc), ou 0 caso ok.
  */
  int amsInsertSample( float *inVector, float *outVector, int inVectorSize, int outVectorSize )
  {

    // Casos de retorno (vetores nulos):
    if( (!inVector) || (!outVector) )
      return -1;
    // Casos de retorno (dimensões incompatíveis):
    if( (inVectorSize!=_inSize) || (outVectorSize!=_outSize) )
      return -1;

    // Caso o número de amostras seja máximo, realoca memória:
    if ( _nPts == (_maxPts) ) 
      amsRealocMatrices();

    // Copiando os dado de entrada:
    for( int i=0 ; i<_inSize ; i++ )
      _inMatrix[_nPts][i] = inVector[i];

    // Copiando os dado de saída:
    for( int i=0 ; i<_outSize ; i++ )
      _outMatrix[_nPts][i] = outVector[i];
    // Incrementa o número de amostras existentes na estrutura:
    _nPts++;

    return 0;
  }


  /**
  * Realoca um vetor, aumentando seu tamanho.
  * @param vec Vetor a ser realocado.
  * @param size Tamanho atual do vetor.
  * @param incsize Incremento de memória a ser incrementado ao vetor.
  */
  void amsRealocVector( float **vec, int size, int incsize )
  {
    int new_size = size + incsize;
    float *tmpVec = *vec;
    float *new_vec = new float[new_size];
    for( int i=0 ; i<size ; i++ )
      new_vec[i] = tmpVec[i];
    delete tmpVec;
    *vec = new_vec;
  }


  /**
  * Aumenta o número de linhas de uma matriz.
  * @param matrix Matriz a ter seu número de linhas aumentado.
  * @param columnSize Dimensão de cada uma das colunas da matriz. Essa dimensão será mantida.
  * @param rowSize Número de linhas atualmente contidas na matriz. Esse número de linhas será aumentado de rowIncSize unidades.
  * @param rowIncSize O número de linhas da matriz será aumentado de rowIncSize unidades.
  */
  void amsRealocMatrix( float ***matrix, int columnSize, int rowSize, int rowIncSize )
  {

    int newRowSize = rowSize + rowIncSize;    //< Novo número de linhas desejado.
    float **tmpMatrix = *matrix;

    // Alocando espaço para a matriz aumentada:
    float **newMatrix = new float*[newRowSize];
    for( int i=0 ; i<newRowSize ; i++ )
    {
      newMatrix[i] = new float[columnSize];   
      if( i<rowSize )
      {
        for( int j=0 ; j<columnSize ; j++ )
          newMatrix[i][j] = tmpMatrix[i][j];
      }
    }
    delete tmpMatrix;
    *matrix = newMatrix;
  }


  /**
  * Realoca as matrizes da estrutura, aumentando seu tamanho, caso seja necessário.
  */
  void amsRealocMatrices()
  {
    // Realoca a matriz de amostras de entrada:
    amsRealocMatrix( &_inMatrix, _inSize, _maxPts, _incSize);

    // Realoca a matriz de amostras de saída:
    amsRealocMatrix( &_outMatrix, _outSize, _maxPts, _incSize);

    // Realoca as matrizes de pixels de componentes RGB separados das amostras de entrada:
    amsRealocMatrix( &_in_R_Matrix, _inComponentSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_G_Matrix, _inComponentSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_B_Matrix, _inComponentSize, _maxPts, _incSize);

    // Realoca as matrizes de médias das amostras de entrada:
    amsRealocMatrix( &_in_MeanR_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_MeanG_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_MeanB_Matrix, _inStatisticsSize, _maxPts, _incSize);

    // Realoca as matrizes de variâncias das amostras de entrada:
    amsRealocMatrix( &_in_VarR_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_VarG_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_VarB_Matrix, _inStatisticsSize, _maxPts, _incSize);

    // Realoca as matrizes de pixels centrais de cada uma das amostras:
    amsRealocMatrix( &_in_MeanR_Matrix, 1, _maxPts, _incSize);
    amsRealocMatrix( &_in_MeanG_Matrix, 1, _maxPts, _incSize);
    amsRealocMatrix( &_in_MeanB_Matrix, 1, _maxPts, _incSize);

    // Incrementa o número máximo de amostras que podem ser inseridas nas matrizes:
    _maxPts += _incSize;
  }


  /**
  * Cria todas as amostras.
  * É necessário que a matriz da máscara tenha sido inserida.
  */
  int amsAllSamplesCreate()
  {
    // Casos de retorno:
    if( (_mascXdim == 0) || (_mascYdim == 0) )
      return -1;

    /*
    // Caso contrário cria todas as amostras:
    for( int i=0 ; i<_mascXdim ; i++ )
      for( int j=0 ; j<_mascYdim ; j++ )
      {
        // Caso o valor desse ponto na matriz seja 0 ou 1, é preciso criar a amostra:
        if( _masc[j][i] == 0 )  //< 0 é jogador.
        {
          // Caso existam todas as coordenadas necessárias para a criação da amostra, ela é criada:
          if( (i>(_samplesWidth/2+1)) && (i<(_mascXdim-(_samplesWidth/2+2))) &&
            (j>(_samplesHeight/2+1)) && (i<(_mascYdim-(_samplesHeight/2+2))) )
          {
            // Gera a amostra:
            imgSmpExtSampleCreate( (i-_samplesWidth/2), (j-_samplesHeight/2), 
              (i+_samplesWidth/2), (j+_samplesHeight/2), &_mascSample );
            // Insere na matriz de amostras de entrada:
            float tmpSaida = 0.1f;
            amsInsertSample( _mascSample, &tmpSaida, _inSize, 1 );
          }
        }
        // Caso o valor desse ponto na matriz seja 0 ou 1, é preciso criar a amostra:
        if( _masc[j][i] == 1 )      //< 1 é fundo.
        {
          // Caso existam todas as coordenadas necessárias para a criação da amostra, ela é criada:
          if( (i>(_samplesWidth/2+1)) && (i<(_mascXdim-(_samplesWidth/2+2))) &&
            (j>(_samplesHeight/2+1)) && (i<(_mascYdim-(_samplesHeight/2+2))) )
          {
            // Gera a amostra:
            imgSmpExtSampleCreate( (i-_samplesWidth/2), (j-_samplesHeight/2), 
              (i+_samplesWidth/2), (j+_samplesHeight/2), &_mascSample );
            // Insere na matriz de amostras de entrada:
            float tmpSaida = 0.9f;
            amsInsertSample( _mascSample, &tmpSaida, _inSize, 1 );
          }
        }
      }
      */

      // CRIANDO OS DEMAIS VALORES DAS AMOSTRAS (VARIÂNCIA, MÉDIA, ETC.):
      int tmp_nPts = 0;
      for( int i=0 ; i<_mascXdim ; i++ )
        for( int j=0 ; j<_mascYdim ; j++ )
        {
          // Caso o valor desse ponto na matriz seja 0 ou 1, é preciso criar a amostra:
          if( (_masc[j][i] == 0) || (_masc[j][i] == 1) )  //< 0 é jogador, 1 é campo.
          {
            // Caso existam todas as coordenadas necessárias para a criação da amostra, ela é criada:
            if( (i>(_samplesWidth/2+1)) && (i<(_mascXdim-(_samplesWidth/2+2))) &&
              (j>(_samplesHeight/2+1)) && (i<(_mascYdim-(_samplesHeight/2+2))) )
            {
              // Gera a amostra:
              imgSmpSeparateComponentSampleCreate( (i-_samplesWidth/2), (j-_samplesHeight/2), 
                (i+_samplesWidth/2), (j+_samplesHeight/2), &_in_R_Vector, &_in_G_Vector, &_in_B_Vector,
                &_in_MeanR_Vector, &_in_MeanG_Vector, &_in_MeanB_Vector, 
                &_in_VarR_Vector, &_in_VarG_Vector, &_in_VarB_Vector );

              // Insere os valores nas demais matrizes de entrada:
              // Valores das componentes dos vetores R, G e B em separado:
              // Escalona e insere os valores nas matrizes: 
              amsSmpAutoScale( 0.9f, 0.1f, _in_R_Vector, _inComponentSize);
              MatrixInsertVector( _in_R_Vector, _inComponentSize, _in_R_Matrix, _inComponentSize, tmp_nPts );
              
              amsSmpAutoScale( 0.9f, 0.1f, _in_G_Vector, _inComponentSize);
              MatrixInsertVector( _in_G_Vector, _inComponentSize, _in_G_Matrix, _inComponentSize, tmp_nPts );

              amsSmpAutoScale( 0.9f, 0.1f, _in_B_Vector, _inComponentSize);
              MatrixInsertVector( _in_B_Vector, _inComponentSize, _in_B_Matrix, _inComponentSize, tmp_nPts );

              // Valores das médias das componentes R, G, e B em separado:
              IntervalScaleInFloat_Zero_One_Vector( _in_MeanR_Vector, _inStatisticsSize);
              MatrixInsertVector( _in_MeanR_Vector, _inStatisticsSize, _in_MeanR_Matrix, _inStatisticsSize, tmp_nPts );
              
              IntervalScaleInFloat_Zero_One_Vector( _in_MeanG_Vector, _inStatisticsSize);
              MatrixInsertVector( _in_MeanG_Vector, _inStatisticsSize, _in_MeanG_Matrix, _inStatisticsSize, tmp_nPts  );
              
              IntervalScaleInFloat_Zero_One_Vector( _in_MeanB_Vector, _inStatisticsSize);
              MatrixInsertVector( _in_MeanB_Vector, _inStatisticsSize, _in_MeanB_Matrix, _inStatisticsSize, tmp_nPts );

              // Valores das variâncias das componentes R, G, e B em separado:
              IntervalScaleInFloat_Zero_One_Vector( _in_VarR_Vector, _inStatisticsSize);
              MatrixInsertVector( _in_VarR_Vector, _inStatisticsSize, _in_VarR_Matrix, _inStatisticsSize, tmp_nPts );
              
              IntervalScaleInFloat_Zero_One_Vector( _in_VarG_Vector, _inStatisticsSize);
              MatrixInsertVector( _in_VarG_Vector, _inStatisticsSize, _in_VarG_Matrix, _inStatisticsSize, tmp_nPts );
              
              IntervalScaleInFloat_Zero_One_Vector( _in_VarB_Vector, _inStatisticsSize);
              MatrixInsertVector( _in_VarB_Vector, _inStatisticsSize, _in_VarB_Matrix, _inStatisticsSize, tmp_nPts );
              
              // Insere os valores do pixel central da amostra de entrada na matriz:
              _out_R_PixelsMatrix[tmp_nPts][0] = _out_R_PixelsVector[0]*0.8f+0.1f;
              _out_G_PixelsMatrix[tmp_nPts][0] = _out_G_PixelsVector[0]*0.8f+0.1f;
              _out_B_PixelsMatrix[tmp_nPts][0] = _out_B_PixelsVector[0]*0.8f+0.1f;
              
              // Tentando criar as amostras de saída:
              if( _masc[j][i] == 0 )  //< 0 é jogador.
                _outMatrix[tmp_nPts][0] = 0.1f;
              if( _masc[j][i] == 1 )  //< 0 é jogador.
                _outMatrix[tmp_nPts][0] = 0.9f;

              // Caso o número de amostras seja máximo, realoca memória:
              if ( tmp_nPts == (_maxPts) ) 
                amsRealocMatrices();
              // Incrementando o número de amostras já criadas:
              tmp_nPts++;
            }
          }
        }
      _nPts = tmp_nPts;
      return 0;
  }


  /**
  * Obtém uma amostra de maneira aleatoria.
  */
  int amsGetRandomSample( float **inRet, float **outRet )
  {
    int amostra = RandInt(0, _nPts);
    
    *inRet  = _inMatrix[amostra]; 
    *outRet = _outMatrix[amostra];

    return 0;
  }


  /**
  * Obtém uma amostra de maneira aleatoria.
  */
  int amsGetSample( int sampleNum, float **inRet, float **outRet )
  {
    int amostra = RandInt(0, _nPts);

    *inRet  = _inMatrix[amostra]; 
    *outRet = _outMatrix[amostra];

    return 0;
  }



  /**
  * Obtém uma versão do vetor com todas as componentes dentro de um intervalo pré-definido.
  * Utiliza as informações disponíveis no _inDataType e no _inPrepFlags para re-gerar os vetores 
  * de entrada da rede.
  * @param maxScale Valor máximo da escala a ser aplicada.
  * @param minScale Valor mínimo da escala a ser aplicada.
  * @param vect Vetor a ser escalonado.
  */
  void amsSmpAutoScale( float maxScale, float minScale, float *vect )
  {
    float scale; 
    float minValue = (float)1e12;
    float maxValue = (float)-1e12;


    // Obtém o maior e o menor valores dentre das componentes do vetor: 
    for( int i=0 ; i<_inSize ; i++ )
    {
      if( vect[i] < minValue )
        minValue = vect[i];
      if( vect[i] > maxValue )
        maxValue = vect[i];
    }
    // Escala a ser aplicada: 
    scale   = (float)((maxScale - minScale)/(maxValue - minValue));
    // Aplicando a escala ao vetor: 
    for( int i=0 ; i<_inSize ; i++ )
      vect[i] = (vect[i]-minValue)*scale + minScale;
  }


  /**
  * Obtém uma versão do vetor com todas as componentes dentro de um intervalo pré-definido.
  * Utiliza as informações disponíveis no _inDataType e no _inPrepFlags para re-gerar os vetores 
  * de entrada da rede.
  * @param maxScale Valor máximo da escala a ser aplicada.
  * @param minScale Valor mínimo da escala a ser aplicada.
  * @param vect Vetor a ser escalonado.
  * @param inSize Número de componentes do vetor.
  */
  void amsSmpAutoScale( float maxScale, float minScale, float *vect, int inSize )
  {
    float scale; 
    float minValue = (float)1e12;
    float maxValue = (float)-1e12;


    // Obtém o maior e o menor valores dentre das componentes do vetor: 
    for( int i=0 ; i<inSize ; i++ )
    {
      if( vect[i] < minValue )
        minValue = vect[i];
      if( vect[i] > maxValue )
        maxValue = vect[i];
    }
    // Escala a ser aplicada: 
    scale   = (float)((maxScale - minScale)/(maxValue - minValue));
    // Aplicando a escala ao vetor: 
    for( int i=0 ; i<inSize ; i++ )
      vect[i] = (vect[i]-minValue)*scale + minScale;
  }






  /**
  * Depois de criadas todas as amostras, essa função é utilizada para carregar (caso necessário) 
  * todas as amostras de entrada/saída em duas matrizes externas. A função recebe um ponteiro para as matrizes e aloca
  * todo o espaço necessário para incluir todas as amostras. Retorna as matrizes alocadas e preenchidas, 
  * além de retornar também o número de amostras e o tamanho de cada uma.
  * @param retInMatrix Matriz de amostras de entrada a ser retornada.
  * @param retInSamlesSize Tamanho de cada uma das amostras de entrada contidas na matriz de entradas.
  * @param retOutMatrix Matriz contendo cada uma das amostras de saída.
  * @param retOutSamlesSize Dimensão de cada uma das amostras de saída (linha) da matriz.
  * @param numSamples Número de amostras existentes nas duas matrizes.
  */
  int getSamplesMatrix( float ***retInMatrix, int *retInSamlesSize, float ***retOutMatrix, int *retOutSamlesSize, int *retNumSamples )
  {
    // Obtendo o número de amostras contidas na estrutura:
    int numSamples = _nPts;
    // Obtendo o número de elementos contidos em cada amostra (linha) da matriz:
    int inSamplesSize = 3*_inComponentSize + 6*_inStatisticsSize + 3;

    float**tmpRetInMatrix = new float*[numSamples];
    for( int i=0 ; i<numSamples ; i++ )
    {
      int pos=0;
      tmpRetInMatrix[i] = new float[inSamplesSize];

      // Copiando os valores de médias das três componentes:
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        tmpRetInMatrix[i][pos] = _in_MeanR_Matrix[i][j];
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        tmpRetInMatrix[i][pos] = _in_MeanG_Matrix[i][j];
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        tmpRetInMatrix[i][pos] = _in_MeanB_Matrix[i][j];
        pos++;
      }
      // Copiando os valores de variâncias das três componentes:
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        tmpRetInMatrix[i][pos] = _in_VarR_Matrix[i][j];
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        tmpRetInMatrix[i][pos] = _in_VarG_Matrix[i][j];
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        tmpRetInMatrix[i][pos] = _in_VarB_Matrix[i][j];
        pos++;
      }
      // Inserindo o valor das componentes RGB do pixel central (o pixel sendo analisado):
      tmpRetInMatrix[i][pos] = _out_R_PixelsMatrix[i][0];
      pos++;
      tmpRetInMatrix[i][pos] = _out_G_PixelsMatrix[i][0];
      pos++;
      tmpRetInMatrix[i][pos] = _out_B_PixelsMatrix[i][0];
      pos++;
      // Copiando as três componentes, R, G e B:
      for( int j=0 ; j<_inComponentSize ; j++ )
      {
        tmpRetInMatrix[i][pos] = _in_R_Matrix[i][j];
        pos++;
      }
      for( int j=0 ; j<_inComponentSize ; j++ )
      {
        tmpRetInMatrix[i][pos] = _in_G_Matrix[i][j];
        pos++;
      }
      for( int j=0 ; j<_inComponentSize ; j++ )
      {
        tmpRetInMatrix[i][pos] = _in_B_Matrix[i][j];
        pos++;
      }
    }

    // Matriz que irá conter todas as amostras de saída:
    float**tmpRetOutMatrix = new float*[numSamples];
    // Fazendo uma cópia da matriz de amostras de saída:
    for( int i=0 ; i<numSamples ; i++ )
    {
      tmpRetOutMatrix[i] = new float[1];
      tmpRetOutMatrix[i][0] = _outMatrix[i][0];
      printf("%lf\n", _outMatrix[i][0]);
    }

    // Retornando as matrizes criadas:
    *retInMatrix      = tmpRetInMatrix;
    *retInSamlesSize  = inSamplesSize; 
    *retOutMatrix     = tmpRetOutMatrix;
    *retOutSamlesSize = 1;
    *retNumSamples    = numSamples;

    return 0;
  }


};
