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
* Estrutura de cria��o dos vetores de amostras.
*/
class Amostras
{

public:
  int _maxPts;          ///< Limite m�ximo de amostras que podem atualmente ser armazenados na estrutura.
  int _nPts;            ///< N�mero de amostras efetivamente sendo armazenados na estrutura.
  int _incSize;         ///< 	

  int _samplesWidth;    ///< Largura de cada uma das amostras de entrada.
  int _samplesHeight;   ///< Altura de cada uma das amostras de entrada.

  // Matrizes CONTENDO OS PIXELS DAS 3 COMPONENTES JUNTOS:
  int _inSize;          ///< Dimens�o de cada uma das amostras de entrada. (samplesWidth)*(samplesHeight)*3
  int _outSize;         ///< Dimens�o de cada uma das amostras de sa�da. (samplesWidth)*(samplesHeight)*3
  float **_inMatrix;    ///< Matriz contendo todas as amostras de entrada. M[_nPts][_inSize].
  float **_outMatrix;   ///< Matriz contendo todas as amostras de sa�da. M[_nPts][_outSize].

  // Matrizes CONTENDO AS 3 COMPONENTES DO PIXEL CENTRAL DE CADA UMA DAS AMOSTRAS DE ENTRADA:
  float **_out_R_PixelsMatrix; ///< Matriz contendo a componente R do pixel central referente � cada entrada.
  float **_out_G_PixelsMatrix; ///< Matriz contendo a componente G do pixel central referente � cada entrada.
  float **_out_B_PixelsMatrix; ///< Matriz contendo a componente B do pixel central referente � cada entrada.
  
  // Matrizes CONTENDO OS PIXELS DAS 3 COMPONENTES EM SEPADADO:
  int _inComponentSize;   ///< Dimens�o de cada uma das amostras de entrada. (samplesWidth)*(samplesHeight)
  float **_in_R_Matrix;   ///< Matriz contendo todas as amostras de componente R de entrada.
  float **_in_G_Matrix;   ///< Matriz contendo todas as amostras de componente G de entrada.
  float **_in_B_Matrix;   ///< Matriz contendo todas as amostras de componente B de entrada.

  // Vari�veis que armazenam as dimens�es de cada amostra das 6 matrizes abaixo:
  int _inStatisticsSize;
  // Matrizes CONTENDO AS VARI�NCIAS DAS COMPONENTES EM SEPARADO:
  float **_in_VarR_Matrix;  ///< Matriz contendo todas as amostras R de entrada de componente R.
  float **_in_VarG_Matrix;  ///< Matriz contendo todas as amostras R de entrada de componente R.
  float **_in_VarB_Matrix;  ///< Matriz contendo todas as amostras R de entrada de componente R.

  // Matrizes CONTENDO AS MEDIAS DAS COMPONENTES EM SEPARADO:
  float **_in_MeanR_Matrix; ///< Matriz contendo todas as amostras R de entrada de componente R.
  float **_in_MeanG_Matrix; ///< Matriz contendo todas as amostras R de entrada de componente R.
  float **_in_MeanB_Matrix; ///< Matriz contendo todas as amostras R de entrada de componente R.

  // DADOS REFERENTES A IMAGEM DE ENTRADA DE ONDE SER�O CONSTRU�DAS AS AMOSTRAS:
  unsigned char *data;  ///< Vetor contendo as compontentes RGB de todos os pixels da imagem de entrada.
  int _height;          ///< Altura da imagem de entrada.
  int _width;           ///< Largura da imagem de entrada.

  // M�scara da imagem de entrada:
  int **_masc;
  int _mascXdim;
  int _mascYdim;

  // Vetores TEMPOR�RIOS para armazenar temporariamente as componentes RGB das amostras de entrada NUM MESMO VETOR:
  float *_mascSample;
  // Vetores TEMPOR�RIOS para armazenar EM SEPARADO temporariamente as componentes RGB das amostras de entrada:
  float *_in_R_Vector;
  float *_in_G_Vector;
  float *_in_B_Vector;
  // Vetores TEMPOR�RIOS para armazenar temporariamente as vari�ncias de cada uma das janelas das amostras de entrada:
  float *_in_VarR_Vector;
  float *_in_VarG_Vector;
  float *_in_VarB_Vector;
  // Vetores TEMPOR�RIOS para armazenar temporariamente as m�dias de cada uma das janelas das amostras de entrada:
  float *_in_MeanR_Vector;
  float *_in_MeanG_Vector;
  float *_in_MeanB_Vector;
  // Vetores TEMPOR�RIOS para armazenar temporariamente as tr�s componentes (RGB) do pixel central de cada amostra:
  float* _out_R_PixelsVector;
  float* _out_G_PixelsVector;
  float* _out_B_PixelsVector;




  /**
  * Cria uma amostra Sld para ser utilizada externamente. Cria um vetor que ir� conter as componentes
  * RGB de todos os pixels da amostra, desde o ponto Xmin,Ymin at� o ponto Xmax, Ymax (inclusive).
  * @param imgSmpExtXmin Coordenada X do ponto inicial dessa amostra na imagem de entrada.
  * @param imgSmpExtYmin Coordenada Y do ponto inicial dessa amostra na imagem de entrada.
  * @param imgSmpExtXmax Coordenada X do ponto final dessa amostra na imagem de entrada.
  * @param imgSmpExtYmax Coordenada Y do ponto final dessa amostra na imagem de entrada.
  * @param imgSmp_R_Vector Vetor onde os valores das componentes R dos pixels da amostra ser�o inseridos.
  * @param imgSmp_G_Vector Vetor onde os valores das componentes R dos pixels da amostra ser�o inseridos.
  * @param imgSmp_B_Vector Vetor onde os valores das componentes R dos pixels da amostra ser�o inseridos.
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

    // Dimens�o de cada um dos vetores (R, G, B):
    int smpDimension = _inComponentSize;

    // Vetor tempor�rio contendo as componentes:
    float* imgSmp_R_Vectortmp = *imgSmp_R_Vector;
    float* imgSmp_G_Vectortmp = *imgSmp_G_Vector;
    float* imgSmp_B_Vectortmp = *imgSmp_B_Vector;

    // Vetor tempor�rio contendo as m�dias com tamanho da janela vari�vel:
    float* imgSmp_R_MeanVectortmp = *imgSmp_R_MeanVector;
    float* imgSmp_G_MeanVectortmp = *imgSmp_G_MeanVector;
    float* imgSmp_B_MeanVectortmp = *imgSmp_B_MeanVector;

    // Vetor tempor�rio contendo as vari�ncias com tamanho de janela vari�vel:
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

    // Gerando as estat�sticas (M�DIA):
    float meanR=0, meanG=0, meanB=0;
    pos = 0;    //< O �ndice da posi��o utilizada do vetor � setado novamente em zero.
    
    // Gerando as estat�sticas (m�dia e vari�ncia):
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
          // Obtendo os valores das tr�s componentes, j� convertido para float:
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

    // Gerando as estat�sticas (VARI�NCIA):
    float varR=0, varG=0, varB=0;
    pos = 0;    //< O �ndice da posi��o utilizada do vetor � setado novamente em zero.
    // Gerando as estat�sticas (m�dia e vari�ncia):
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
          // Obtendo os valores das tr�s componentes, j� convertido para float:
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
    // Obtendo os valores das tr�s componentes, j� convertido para float:
    // ESSAS TR�S VARI�VEIS S�O DA PR�PRIA CLASSE, POR ISSO N�O S�O RECEBIDOS COMO PAR�METROS:
    _out_R_PixelsVector[0] = (float)((float)retR/255);
    _out_G_PixelsVector[0] = (float)((float)retG/255);
    _out_B_PixelsVector[0] = (float)((float)retB/255);

    return 0;
  }




  /** 
  * Obt�m um pixel da imagem de entrada. Todo o c�lculo para encontrar o pixel desejado � 
  * feito baseando-se no fato de que a inst�ncia de imImage recebida foi recebida como BitMap.
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

    // ImImages possuem pixels em bottom up na estrutura. Al�m disso, as componentes R, G e B s�o IM_PACKED:

    // Encontrando a posi��o do pixel no vetor:
    int pos = (y*w +x);     //< � IM_PACKED.
    // Carregando as componentes do pixel requerido:
    * retR = data[3*pos];
    * retG = data[3*pos+1];
    * retB = data[3*pos+2];
    return 0;
  }


  /**
  * Cria uma amostra Sld para ser utilizada externamente. Cria um vetor que ir� conter as componentes
  * RGB de todos os pixels da amostra, desde o ponto Xmin,Ymin at� o ponto Xmax, Ymax (inclusive).
  * @param imgSmpExtXmin Coordenada X do ponto inicial dessa amostra na imagem de entrada.
  * @param imgSmpExtYmin Coordenada Y do ponto inicial dessa amostra na imagem de entrada.
  * @param imgSmpExtXmax Coordenada X do ponto inicial dessa amostra na imagem de sa�da.
  * @param imgSmpExtYmax Coordenada Y do ponto inicial dessa amostra na imagem de sa�da.
  * @param imgSmpExtVector Vetor onde os pixels das tr�s componentes ser�o inseridos.
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
  * Insere a m�scara.
  */
  int amsSetMasc( int **masc, int mascXdim, int mascYdim )
  {
    if( !masc )
      return -1;

    // Alocando espa�o para o vetor auxiliar de cria��o das amostras:
    //_mascSample = new float[_inSize];
    _masc     = masc;
    _mascXdim = mascXdim;
    _mascYdim = mascYdim;
    return 0;
  }


  /**
  * Construtor.
  * @param maxPts N�mero m�ximo de amostras de entrada que poder�o inicialmente serem inseridos na estrutura.
  * Haver� realoca��o de mem�ria sempre que necess�rio.
  * @param samplesWidth Largura de cada uma das amostras de entrada que ser�o geradas.
  * @param samplesHeight Altura de cada uma das amostras de entrada que ser�o geradas.
  * @param outSize Tamanho de cada uma das amostras de sa�da
  * @param incSize Incremento no n�mero de amostras a ser acrescentado no vetor de amostras de entrada, caso 
  * seja necess�ria a realoca��o em algum momento futuro.
  * @param width Largura da imagem de entrada.
  * @param height Altura da imagem de entrada.
  * @param mydata Dados referentes aos pixels da imagem de entrada.
  */
  Amostras( int maxPts, int samplesWidth, int samplesHeight, int outSize, int incSize, int width, int height, 
            unsigned char *mydata)
  {
    // Inserindo os dados referentes � imagem de entrada:
    data = mydata;
    _width = width;
    _height = height;

    // Tamanho de cada uma das amostras (R+G+B):
    int inSize = (samplesWidth)*(samplesHeight)*3;

    // Tamanho em x e y das amostras que ser�o geradas:
    _samplesWidth = samplesWidth;
    _samplesHeight = samplesHeight;

    // Demais par�metros da estrutura:
    _maxPts   = maxPts;
    _inSize   = inSize;
    _outSize  = outSize;
    _incSize  = incSize;

    // Alocando as matrizes de amostras iniciais de entrada e sa�da:
    _inMatrix = new float*[maxPts];
    _outMatrix = new float*[maxPts];
    for( int i=0 ; i<maxPts ; i++ )
    {
      _inMatrix[i]  = new float[inSize];
      _outMatrix[i] = new float[outSize];
    }

    // Alocando espa�o para as matrizes que ir�o conter cada um dos pixels centrais das amostras:
    _out_R_PixelsMatrix = new float*[maxPts];
    _out_G_PixelsMatrix = new float*[maxPts];
    _out_B_PixelsMatrix = new float*[maxPts];
    for( int i=0 ; i<maxPts ; i++ )
    {
      // Cada posi��o ir� conter apenas a componente R, G, ou B do pixel:
      _out_R_PixelsMatrix[i] = new float[1];
      _out_G_PixelsMatrix[i] = new float[1];
      _out_B_PixelsMatrix[i] = new float[1];
    }

    // Alocando os outros dados da estrutura:
    // Mant�m o tamanho de cada uma das amostras das componentes R, G e B separadas:
    _inComponentSize = _samplesWidth*_samplesHeight;    

    // Mant�m o tamanho de cada uma das amostras de m�dia e vari�ncia das amostras:
    _inStatisticsSize = Minor(_samplesWidth, _samplesHeight);
    _inStatisticsSize = _inStatisticsSize/2;

    // Alocando as demais matrizes:
    // 1. Matrizes contendo as amostras em componentes separadas:
    _in_R_Matrix = new float*[maxPts];
    _in_G_Matrix = new float*[maxPts];
    _in_B_Matrix = new float*[maxPts];
    // 2. Matrizes das vari�ncias das amostras, levando em contra janelas de tamanho 1, 2, ..., _inStatisticsSize:
    _in_VarR_Matrix = new float*[maxPts];
    _in_VarG_Matrix = new float*[maxPts];
    _in_VarB_Matrix = new float*[maxPts];
    // 3. Matrizes das m�dias das amostras, levando em contra janelas de tamanho 1, 2, ..., _inStatisticsSize:
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
    
    // Ainda n�o existem amostras na estrutura:
    _nPts = 0;

    // A m�scara ser� repassada posteriormente:
    _masc = NULL;
    _mascXdim = 0;
    _mascYdim = 0;

    // Vetores TEMPOR�RIOS utilizados apenas para manter cada amostra enquanto estiver sendo criada:
    // Alocando o vetor tempor�rio auxiliar na cria��o de cada uma das amostras:
    _mascSample = new float[inSize];
    // Vetores TEMPOR�RIOS para armazenar temporariamente as componentes RGB das amostras de entrada:
    _in_R_Vector = new float[_inComponentSize];
    _in_G_Vector = new float[_inComponentSize];
    _in_B_Vector = new float[_inComponentSize];
    // Vetores TEMPOR�RIOS para armazenar temporariamente as vari�ncias de cada uma das janelas das amostras de entrada:
    _in_VarR_Vector = new float[_inStatisticsSize];
    _in_VarG_Vector = new float[_inStatisticsSize];
    _in_VarB_Vector = new float[_inStatisticsSize];
    // Vetores TEMPOR�RIOS para armazenar temporariamente as m�dias de cada uma das janelas das amostras de entrada:
    _in_MeanR_Vector = new float[_inStatisticsSize];
    _in_MeanG_Vector = new float[_inStatisticsSize];
    _in_MeanB_Vector = new float[_inStatisticsSize];
    // Vetores TEMPOR�RIOS para armazenar temporariamente as tr�s componentes (RGB) do pixel central de cada amostra:
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
  * Fun��o auxiliar. Copia o valor de um vetor para dentro de uma determinada posi��o de uma matriz que
  * tenha o n�mero de colunas de mesma dimens�o desse vetor.
  * @param inVector Vetor de entrada da amostra a ser copiado para a matriz.
  * @param inVectorSize Dimens�o do vetor recebido.
  * @param inMatrix Matriz na qual os valores contidos no vetor ser�o inseridos.
  * @param inMatrixXdim Dimens�o da por��o da matriz para onde o vetor ser� copiado.
  * @param inMatrixYPos Linha da matriz na qual o vetor ser� inserido.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int MatrixInsertVector( float *inVector, int inVectorSize, float** inMatrix, int inMatrixXdim, int inMatrixYPos )
  {
    // Casos retorno:
    if( (!inVector) || (!inMatrix) )
      return -1;
    if( inVectorSize != inMatrixXdim )
      return -1;

    // Copia os valores do vetor para a posi��o apropriada da matriz:
    for( int i=0 ; i<inVectorSize ; i++ )
      inMatrix[inMatrixYPos][i] = inVector[i];

    return 0;
  }



  /**
  * Insere uma nova amostra na estrutura.
  * @param inVector Vetor de entrada da amostra a ser copiado para a estrutura.
  * @param outVector Vetor de sa�da da amostra, a ser copiado para a estrutura.
  * @param inVectorSize Dimens�o do vetor de entrada.
  * @param outVectorSize Dimens�o do vetor de sa�da.
  * @return Retorna -1 em caso de erro (vetores incompat�veis, etc), ou 0 caso ok.
  */
  int amsInsertSample( float *inVector, float *outVector, int inVectorSize, int outVectorSize )
  {

    // Casos de retorno (vetores nulos):
    if( (!inVector) || (!outVector) )
      return -1;
    // Casos de retorno (dimens�es incompat�veis):
    if( (inVectorSize!=_inSize) || (outVectorSize!=_outSize) )
      return -1;

    // Caso o n�mero de amostras seja m�ximo, realoca mem�ria:
    if ( _nPts == (_maxPts) ) 
      amsRealocMatrices();

    // Copiando os dado de entrada:
    for( int i=0 ; i<_inSize ; i++ )
      _inMatrix[_nPts][i] = inVector[i];

    // Copiando os dado de sa�da:
    for( int i=0 ; i<_outSize ; i++ )
      _outMatrix[_nPts][i] = outVector[i];
    // Incrementa o n�mero de amostras existentes na estrutura:
    _nPts++;

    return 0;
  }


  /**
  * Realoca um vetor, aumentando seu tamanho.
  * @param vec Vetor a ser realocado.
  * @param size Tamanho atual do vetor.
  * @param incsize Incremento de mem�ria a ser incrementado ao vetor.
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
  * Aumenta o n�mero de linhas de uma matriz.
  * @param matrix Matriz a ter seu n�mero de linhas aumentado.
  * @param columnSize Dimens�o de cada uma das colunas da matriz. Essa dimens�o ser� mantida.
  * @param rowSize N�mero de linhas atualmente contidas na matriz. Esse n�mero de linhas ser� aumentado de rowIncSize unidades.
  * @param rowIncSize O n�mero de linhas da matriz ser� aumentado de rowIncSize unidades.
  */
  void amsRealocMatrix( float ***matrix, int columnSize, int rowSize, int rowIncSize )
  {

    int newRowSize = rowSize + rowIncSize;    //< Novo n�mero de linhas desejado.
    float **tmpMatrix = *matrix;

    // Alocando espa�o para a matriz aumentada:
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
  * Realoca as matrizes da estrutura, aumentando seu tamanho, caso seja necess�rio.
  */
  void amsRealocMatrices()
  {
    // Realoca a matriz de amostras de entrada:
    amsRealocMatrix( &_inMatrix, _inSize, _maxPts, _incSize);

    // Realoca a matriz de amostras de sa�da:
    amsRealocMatrix( &_outMatrix, _outSize, _maxPts, _incSize);

    // Realoca as matrizes de pixels de componentes RGB separados das amostras de entrada:
    amsRealocMatrix( &_in_R_Matrix, _inComponentSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_G_Matrix, _inComponentSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_B_Matrix, _inComponentSize, _maxPts, _incSize);

    // Realoca as matrizes de m�dias das amostras de entrada:
    amsRealocMatrix( &_in_MeanR_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_MeanG_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_MeanB_Matrix, _inStatisticsSize, _maxPts, _incSize);

    // Realoca as matrizes de vari�ncias das amostras de entrada:
    amsRealocMatrix( &_in_VarR_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_VarG_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_VarB_Matrix, _inStatisticsSize, _maxPts, _incSize);

    // Realoca as matrizes de pixels centrais de cada uma das amostras:
    amsRealocMatrix( &_in_MeanR_Matrix, 1, _maxPts, _incSize);
    amsRealocMatrix( &_in_MeanG_Matrix, 1, _maxPts, _incSize);
    amsRealocMatrix( &_in_MeanB_Matrix, 1, _maxPts, _incSize);

    // Incrementa o n�mero m�ximo de amostras que podem ser inseridas nas matrizes:
    _maxPts += _incSize;
  }


  /**
  * Cria todas as amostras.
  * � necess�rio que a matriz da m�scara tenha sido inserida.
  */
  int amsAllSamplesCreate()
  {
    // Casos de retorno:
    if( (_mascXdim == 0) || (_mascYdim == 0) )
      return -1;

    /*
    // Caso contr�rio cria todas as amostras:
    for( int i=0 ; i<_mascXdim ; i++ )
      for( int j=0 ; j<_mascYdim ; j++ )
      {
        // Caso o valor desse ponto na matriz seja 0 ou 1, � preciso criar a amostra:
        if( _masc[j][i] == 0 )  //< 0 � jogador.
        {
          // Caso existam todas as coordenadas necess�rias para a cria��o da amostra, ela � criada:
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
        // Caso o valor desse ponto na matriz seja 0 ou 1, � preciso criar a amostra:
        if( _masc[j][i] == 1 )      //< 1 � fundo.
        {
          // Caso existam todas as coordenadas necess�rias para a cria��o da amostra, ela � criada:
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

      // CRIANDO OS DEMAIS VALORES DAS AMOSTRAS (VARI�NCIA, M�DIA, ETC.):
      int tmp_nPts = 0;
      for( int i=0 ; i<_mascXdim ; i++ )
        for( int j=0 ; j<_mascYdim ; j++ )
        {
          // Caso o valor desse ponto na matriz seja 0 ou 1, � preciso criar a amostra:
          if( (_masc[j][i] == 0) || (_masc[j][i] == 1) )  //< 0 � jogador, 1 � campo.
          {
            // Caso existam todas as coordenadas necess�rias para a cria��o da amostra, ela � criada:
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

              // Valores das m�dias das componentes R, G, e B em separado:
              IntervalScaleInFloat_Zero_One_Vector( _in_MeanR_Vector, _inStatisticsSize);
              MatrixInsertVector( _in_MeanR_Vector, _inStatisticsSize, _in_MeanR_Matrix, _inStatisticsSize, tmp_nPts );
              
              IntervalScaleInFloat_Zero_One_Vector( _in_MeanG_Vector, _inStatisticsSize);
              MatrixInsertVector( _in_MeanG_Vector, _inStatisticsSize, _in_MeanG_Matrix, _inStatisticsSize, tmp_nPts  );
              
              IntervalScaleInFloat_Zero_One_Vector( _in_MeanB_Vector, _inStatisticsSize);
              MatrixInsertVector( _in_MeanB_Vector, _inStatisticsSize, _in_MeanB_Matrix, _inStatisticsSize, tmp_nPts );

              // Valores das vari�ncias das componentes R, G, e B em separado:
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
              
              // Tentando criar as amostras de sa�da:
              if( _masc[j][i] == 0 )  //< 0 � jogador.
                _outMatrix[tmp_nPts][0] = 0.1f;
              if( _masc[j][i] == 1 )  //< 0 � jogador.
                _outMatrix[tmp_nPts][0] = 0.9f;

              // Caso o n�mero de amostras seja m�ximo, realoca mem�ria:
              if ( tmp_nPts == (_maxPts) ) 
                amsRealocMatrices();
              // Incrementando o n�mero de amostras j� criadas:
              tmp_nPts++;
            }
          }
        }
      _nPts = tmp_nPts;
      return 0;
  }


  /**
  * Obt�m uma amostra de maneira aleatoria.
  */
  int amsGetRandomSample( float **inRet, float **outRet )
  {
    int amostra = RandInt(0, _nPts);
    
    *inRet  = _inMatrix[amostra]; 
    *outRet = _outMatrix[amostra];

    return 0;
  }


  /**
  * Obt�m uma amostra de maneira aleatoria.
  */
  int amsGetSample( int sampleNum, float **inRet, float **outRet )
  {
    int amostra = RandInt(0, _nPts);

    *inRet  = _inMatrix[amostra]; 
    *outRet = _outMatrix[amostra];

    return 0;
  }



  /**
  * Obt�m uma vers�o do vetor com todas as componentes dentro de um intervalo pr�-definido.
  * Utiliza as informa��es dispon�veis no _inDataType e no _inPrepFlags para re-gerar os vetores 
  * de entrada da rede.
  * @param maxScale Valor m�ximo da escala a ser aplicada.
  * @param minScale Valor m�nimo da escala a ser aplicada.
  * @param vect Vetor a ser escalonado.
  */
  void amsSmpAutoScale( float maxScale, float minScale, float *vect )
  {
    float scale; 
    float minValue = (float)1e12;
    float maxValue = (float)-1e12;


    // Obt�m o maior e o menor valores dentre das componentes do vetor: 
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
  * Obt�m uma vers�o do vetor com todas as componentes dentro de um intervalo pr�-definido.
  * Utiliza as informa��es dispon�veis no _inDataType e no _inPrepFlags para re-gerar os vetores 
  * de entrada da rede.
  * @param maxScale Valor m�ximo da escala a ser aplicada.
  * @param minScale Valor m�nimo da escala a ser aplicada.
  * @param vect Vetor a ser escalonado.
  * @param inSize N�mero de componentes do vetor.
  */
  void amsSmpAutoScale( float maxScale, float minScale, float *vect, int inSize )
  {
    float scale; 
    float minValue = (float)1e12;
    float maxValue = (float)-1e12;


    // Obt�m o maior e o menor valores dentre das componentes do vetor: 
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
  * Depois de criadas todas as amostras, essa fun��o � utilizada para carregar (caso necess�rio) 
  * todas as amostras de entrada/sa�da em duas matrizes externas. A fun��o recebe um ponteiro para as matrizes e aloca
  * todo o espa�o necess�rio para incluir todas as amostras. Retorna as matrizes alocadas e preenchidas, 
  * al�m de retornar tamb�m o n�mero de amostras e o tamanho de cada uma.
  * @param retInMatrix Matriz de amostras de entrada a ser retornada.
  * @param retInSamlesSize Tamanho de cada uma das amostras de entrada contidas na matriz de entradas.
  * @param retOutMatrix Matriz contendo cada uma das amostras de sa�da.
  * @param retOutSamlesSize Dimens�o de cada uma das amostras de sa�da (linha) da matriz.
  * @param numSamples N�mero de amostras existentes nas duas matrizes.
  */
  int getSamplesMatrix( float ***retInMatrix, int *retInSamlesSize, float ***retOutMatrix, int *retOutSamlesSize, int *retNumSamples )
  {
    // Obtendo o n�mero de amostras contidas na estrutura:
    int numSamples = _nPts;
    // Obtendo o n�mero de elementos contidos em cada amostra (linha) da matriz:
    int inSamplesSize = 3*_inComponentSize + 6*_inStatisticsSize + 3;

    float**tmpRetInMatrix = new float*[numSamples];
    for( int i=0 ; i<numSamples ; i++ )
    {
      int pos=0;
      tmpRetInMatrix[i] = new float[inSamplesSize];

      // Copiando os valores de m�dias das tr�s componentes:
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
      // Copiando os valores de vari�ncias das tr�s componentes:
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
      // Copiando as tr�s componentes, R, G e B:
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

    // Matriz que ir� conter todas as amostras de sa�da:
    float**tmpRetOutMatrix = new float*[numSamples];
    // Fazendo uma c�pia da matriz de amostras de sa�da:
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
