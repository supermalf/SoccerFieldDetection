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


// Define se ser�o criadas as amostras de componentes separados:
//#define COMPONENT_MATRICES      
// Define se ser�o criadas as amostras de assimentria:
//#define ASM_MATRICES          
// Define se ser�o criadas as amostras de coeficiente de varia��o:
//#define CVAR_MATRICES




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
  // Matrizes CONTENDO AS MEDIAS DAS COMPONENTES EM SEPARADO:
  float **_in_MeanR_Matrix; ///< Matriz contendo as m�dias das amostras de entrada de componente R.
  float **_in_MeanG_Matrix; ///< Matriz contendo as m�dias das amostras de entrada de componente G.
  float **_in_MeanB_Matrix; ///< Matriz contendo as m�dias das amostras de entrada de componente B.

  // Matrizes CONTENDO AS VARI�NCIAS DAS COMPONENTES EM SEPARADO:
  float **_in_VarR_Matrix;  ///< Matriz contendo as vari�ncias das amostras de entrada de componente R.
  float **_in_VarG_Matrix;  ///< Matriz contendo as vari�ncias das amostras de entrada de componente G.
  float **_in_VarB_Matrix;  ///< Matriz contendo as vari�ncias das amostras de entrada de componente B.

  // Matrizes CONTENDO OS COEFICIENTES DE VARIA��O DAS COMPONENTES EM SEPARADO:
  float **_in_CvarR_Matrix; ///< Matriz contendo os coefs de varia��o das amostras de entrada de componente R.
  float **_in_CvarG_Matrix; ///< Matriz contendo os coefs de varia��o das amostras de entrada de componente G.
  float **_in_CvarB_Matrix; ///< Matriz contendo os coefs de varia��o das amostras de entrada de componente B.

  // Matrizes CONTENDO OS VALORES DE ASSIMETRIA DAS COMPONENTES EM SEPARADO:
  float **_in_AsmR_Matrix;  ///< Matriz contendo as assimetrias das amostras de entrada de componente R.
  float **_in_AsmG_Matrix;  ///< Matriz contendo as assimetrias das amostras de entrada de componente G.
  float **_in_AsmB_Matrix;  ///< Matriz contendo as assimetrias das amostras de entrada de componente B.
  
  // Matrizes CONTENDO OS VALORES DE ENTROPIA DAS COMPONENTES EM SEPARADO:
  float **_in_EntpR_Matrix; ///< Matriz contendo as entropias das amostras de entrada de componente R.
  float **_in_EntpG_Matrix; ///< Matriz contendo as entropias das amostras de entrada de componente G.
  float **_in_EntpB_Matrix; ///< Matriz contendo as entropias das amostras de entrada de componente B.

  int **_CoordsMatrix;        ///< Matriz contendo as coordenadas (x e y) da amostra.


  // Depois de geradas as amostras, posso escolher alguns par�metros de entrada atrav�s do vetor de flags:
  int _inSamplesSize;
  bool *_inSamplesFlagVector;

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
  // Vetores TEMPOR�RIOS para armazenar temporariamente as m�dias de cada uma das janelas das amostras de entrada:
  float *_in_MeanR_Vector;
  float *_in_MeanG_Vector;
  float *_in_MeanB_Vector;
  // Vetores TEMPOR�RIOS para armazenar temporariamente as vari�ncias de cada uma das janelas das amostras de entrada:
  float *_in_VarR_Vector;
  float *_in_VarG_Vector;
  float *_in_VarB_Vector;

  // Vetores TEMPOR�RIOS para armazenar temporariamente as os valores de coeficiente de varia��o das amostras de entrada:
  float *_in_CvarR_Vector;
  float *_in_CvarG_Vector;
  float *_in_CvarB_Vector;

  // Vetores TEMPOR�RIOS para armazenar temporariamente as os valores de assimetria das amostras de entrada:
  float *_in_AsmR_Vector;
  float *_in_AsmG_Vector;
  float *_in_AsmB_Vector;

  // Vetores TEMPOR�RIOS para armazenar temporariamente as os valores de entropia das amostras de entrada:
  float *_in_EntpR_Vector;
  float *_in_EntpG_Vector;
  float *_in_EntpB_Vector;

  // Vetores TEMPOR�RIOS para armazenar temporariamente as tr�s componentes (RGB) do pixel central de cada amostra:
  float* _out_R_PixelsVector;
  float* _out_G_PixelsVector;
  float* _out_B_PixelsVector;


  // Vetor tempor�rio utilizado para armazenar os valores dos histogramas das janelas vari�veis:
  int **Mhst_R;   //< Matriz do histograma da componente R.
  int **Mhst_G;   //< Matriz do histograma da componente G.
  int **Mhst_B;   //< Matriz do histograma da componente B.


  // Grupos de escalonamento das amostras:
  samplesData *_dataTypes;

  // Uma vez que as amostras de entrada foram criadas, o usu�rio pode selecionar quais das medi��es
  // das amostras est�o EFETIVAMENTE ligadas/desligadas. Uma vez feito isso, uma nova matriz de amostras
  // de entrada � criada, no entanto contendo somente as medi��es utilizadas atualmente. Essa � a matriz
  // abaixo:
  float **_prepInSamplesMatrix;





  /**
  * Configura o vetor de flags para a gera��o das amostras.
  * @param flagsVector Novos valores do vetor de flags, que indica quais dos componentes de uma amostra est�o 
  * efetivamente ligados.
  */
  int flagsVectorConfig( bool *flagsVector, int flagsVectorSize )
  {
    // Casos de retorno:
    if( (!flagsVector) || (flagsVectorSize!=_inSamplesSize) )
      return -1;

    // Caso contr�rio, copia os valores para o vetor de flags da classe:
    for( int i=0 ; i<_inSamplesSize ; i++ )
      _inSamplesFlagVector[i] = flagsVector[i];

    return 0;
  }


  /**
  * Conta o n�mero de entradas ativas do vetor de flags para a gera��o das amostras.
  * @return Retorna o n�mero de entradas ativas do vetor de flags.
  */
  int flagsVectorCount()
  {
    // Casos de retorno:
    if( (!_inSamplesFlagVector) )
      return -1;

    int cont=0;
    // Caso contr�rio, copia os valores para o vetor de flags da classe:
    for( int i=0 ; i<_inSamplesSize ; i++ )
      if( _inSamplesFlagVector[i] == true)
        cont++;

    return cont;
  }


  /**
  * Cria uma amostra e normaliza uma amostra Sld para ser utilizada externamente. Cria um vetor que ir� conter as componentes
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
  * @param imgSmp_R_CvarVector
  * @param imgSmp_G_CvarVector
  * @param imgSmp_B_CvarVector
  * @param imgSmp_R_AsmVector
  * @param imgSmp_G_AsmVector
  * @param imgSmp_B_AsmVector
  * @param imgSmp_R_EntpVector
  * @param imgSmp_G_EntpVector
  * @param imgSmp_B_EntpVector
  * @return Retorna -1 em caso de erro (imagem inexistente, corrdenadas inexistentes, etc), ou 0 caso ok.
  */
  int imgSmpSeparateComponentSampleCreate( int imgSmpExtXmin, int imgSmpExtYmin, int imgSmpExtXmax, int imgSmpExtYmax, 
                                            float** imgSmp_R_Vector,      float** imgSmp_G_Vector,      float** imgSmp_B_Vector, 
                                            float** imgSmp_R_MeanVector,  float** imgSmp_G_MeanVector,  float** imgSmp_B_MeanVector,
                                            float** imgSmp_R_VarVector,   float** imgSmp_G_VarVector,   float** imgSmp_B_VarVector,
                                            float** imgSmp_R_CvarVector,  float** imgSmp_G_CvarVector,  float** imgSmp_B_CvarVector,
                                            float** imgSmp_R_AsmVector,   float** imgSmp_G_AsmVector,   float** imgSmp_B_AsmVector,
                                            float** imgSmp_R_EntpVector,  float** imgSmp_G_EntpVector,  float** imgSmp_B_EntpVector )
  {
    if( (imgSmpExtXmin < 0) || (imgSmpExtXmax >= _width ) )
      return -1;
    if( (imgSmpExtYmin < 0) || (imgSmpExtYmax >= _height ) )
      return -1;


    // Dimens�o de cada um dos vetores (R, G, B) de componentes separadas:
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

    // Vetor tempor�rio contendo os coeficientes de varia��o com tamanho de janela vari�vel:
    float* imgSmp_R_CvarVectortmp = *imgSmp_R_CvarVector;
    float* imgSmp_G_CvarVectortmp = *imgSmp_G_CvarVector;
    float* imgSmp_B_CvarVectortmp = *imgSmp_B_CvarVector;

    // Vetor tempor�rio contendo os valores de assimentria com tamanho de janela vari�vel:
    float* imgSmp_R_AsmVectortmp = *imgSmp_R_AsmVector;
    float* imgSmp_G_AsmVectortmp = *imgSmp_G_AsmVector;
    float* imgSmp_B_AsmVectortmp = *imgSmp_B_AsmVector;

    // Vetor tempor�rio contendo os valores de entropia com tamanho de janela vari�vel:
    float* imgSmp_R_EntpVectortmp = *imgSmp_R_EntpVector;
    float* imgSmp_G_EntpVectortmp = *imgSmp_G_EntpVector;
    float* imgSmp_B_EntpVectortmp = *imgSmp_B_EntpVector;

     // Gerando as amostras de cores com as 3 componentes em vetores separados:
    int pos=0;


#ifdef COMPONENT_MATRICES
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
    // Valores das componentes dos vetores R, G e B em separado:
    // Escalona e insere os valores nas matrizes: 
    amsSmpAutoScale( 0.9f, 0.1f, imgSmp_R_Vectortmp, _inComponentSize);
    amsSmpAutoScale( 0.9f, 0.1f, imgSmp_G_Vectortmp, _inComponentSize);
    amsSmpAutoScale( 0.9f, 0.1f, imgSmp_B_Vectortmp, _inComponentSize);
#endif


    // Gerando as estat�sticas (M�DIA e HISTOGRAMA):
    // Para criar os valores de entropia, primeiro temos que criar os histogramas das janelas:
    // Inicializando o histograma (as matrizes foram alocadas no construtor):
    //std::fill( &Mhst_R[0][0], &Mhst_R[_inStatisticsSize][256], 0 );
    //std::fill( &Mhst_G[0][0], &Mhst_G[_inStatisticsSize][256], 0 );
    //std::fill( &Mhst_B[0][0], &Mhst_B[_inStatisticsSize][256], 0 );
    for( int f=0 ; f<_inStatisticsSize ; f++ )
      for( int g=0 ; g<256 ; g++ )
      {
        Mhst_R[f][g] = 0;
        Mhst_G[f][g] = 0;
        Mhst_B[f][g] = 0;
      }

    float meanR=0, meanG=0, meanB=0;
    pos = 0;    //< O �ndice da posi��o utilizada do vetor � setado novamente em zero.
    // Gerando as estat�sticas (m�dia):
    for( int k=0 ; k<_inStatisticsSize ; k++ )
    {
      meanR=0;      meanG=0;      meanB=0;
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
          // Gerando os histogramas:
          Mhst_R[k][retR]++;          
          Mhst_G[k][retG]++;          
          Mhst_B[k][retB]++;
          // Incrementando o n�mero de pixels:
          numPixels++;
        }
      }
      imgSmp_R_MeanVectortmp[pos] = meanR/numPixels;
      imgSmp_G_MeanVectortmp[pos] = meanG/numPixels;
      imgSmp_B_MeanVectortmp[pos] = meanB/numPixels;
      pos++;
    }
    // Valores das m�dias das componentes R, G, e B em separado:
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_R_MeanVectortmp, _inStatisticsSize);
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_G_MeanVectortmp, _inStatisticsSize);
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_B_MeanVectortmp, _inStatisticsSize);


    // Gerando as estat�sticas (VARI�NCIA):
    float varR=0, varG=0, varB=0;
    pos = 0;    //< O �ndice da posi��o utilizada do vetor � setado novamente em zero.
    // Gerando as estat�sticas (vari�ncia):
    for( int k=0 ; k<_inStatisticsSize ; k++ )
    {
      varR=0;      varG=0;      varB=0;
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
    // Valores das vari�ncias das componentes R, G, e B em separado:
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_R_VarVectortmp, _inStatisticsSize);
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_G_VarVectortmp, _inStatisticsSize);
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_B_VarVectortmp, _inStatisticsSize);


#ifdef CVAR_MATRICES
    // Gerando as estat�sticas (COEFICIENTE DE VARIA��O):
    for( int k=0 ; k<_inStatisticsSize ; k++ )
    {
      imgSmp_R_CvarVectortmp[k] = (float)sqrt(imgSmp_R_VarVectortmp[k])/imgSmp_R_MeanVectortmp[k];
      imgSmp_G_CvarVectortmp[k] = (float)sqrt(imgSmp_G_VarVectortmp[k])/imgSmp_G_MeanVectortmp[k];
      imgSmp_B_CvarVectortmp[k] = (float)sqrt(imgSmp_B_VarVectortmp[k])/imgSmp_B_MeanVectortmp[k];
    }
    // Valores dos COEFICIENTES DE VARIA��O das componentes R, G, e B em separado:
    // Normaliza o vetor e depois escalona:
    NormalizeFloatVector( imgSmp_R_CvarVectortmp, _inStatisticsSize );
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_R_CvarVectortmp, _inStatisticsSize);
    NormalizeFloatVector( imgSmp_G_CvarVectortmp, _inStatisticsSize );
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_G_CvarVectortmp, _inStatisticsSize);
    NormalizeFloatVector( imgSmp_B_CvarVectortmp, _inStatisticsSize );
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_B_CvarVectortmp, _inStatisticsSize);
#endif


#ifdef ASM_MATRICES
    // Gerando as estat�sticas (ASSIMETRIA):
    float asmR=0, asmG=0, asmB=0;
    pos = 0;    //< O �ndice da posi��o utilizada do vetor � setado novamente em zero.
    // Gerando as estat�sticas (m�dia e vari�ncia):
    for( int k=0 ; k<_inStatisticsSize ; k++ )
    {
      asmR=0;
      asmG=0;
      asmB=0;
      int numPixels = 0;
      for( int j=imgSmpExtYmin+k ; j<=imgSmpExtYmax-k ; j++ )
      {
        for( int i=imgSmpExtXmin+k ; i<=imgSmpExtXmax-k ; i++ )
        {
          unsigned char retR, retG, retB;
          amsSmpInImageGetPixel( i, j, &retR, &retG, &retB );
          // Obtendo os valores das tr�s componentes, j� convertido para float:
          asmR += ABS(
                  ((float)((float)retR/255) - imgSmp_R_MeanVectortmp[pos]) * 
                  ((float)((float)retR/255) - imgSmp_R_MeanVectortmp[pos]) * 
                  ((float)((float)retR/255) - imgSmp_R_MeanVectortmp[pos])  );

          asmG += ABS(
                  ((float)((float)retG/255) - imgSmp_G_MeanVectortmp[pos]) *
                  ((float)((float)retG/255) - imgSmp_G_MeanVectortmp[pos]) *
                  ((float)((float)retG/255) - imgSmp_G_MeanVectortmp[pos])  );

          asmB += ABS(
                  ((float)((float)retB/255) - imgSmp_B_MeanVectortmp[pos]) *
                  ((float)((float)retB/255) - imgSmp_B_MeanVectortmp[pos]) *
                  ((float)((float)retB/255) - imgSmp_B_MeanVectortmp[pos])  );

          numPixels++;
        }
      }

      // Valores de vari�ncia, elevados a 2/3, a serem aplicados no denominador:
      float var3_2_R = (float)pow(imgSmp_R_VarVectortmp[pos], (3/2));
      float var3_2_G = (float)pow(imgSmp_G_VarVectortmp[pos], (3/2));
      float var3_2_B = (float)pow(imgSmp_B_VarVectortmp[pos], (3/2));

      imgSmp_R_AsmVectortmp[pos] = (asmR/numPixels)/var3_2_R;
      imgSmp_G_AsmVectortmp[pos] = (asmG/numPixels)/var3_2_G;
      imgSmp_B_AsmVectortmp[pos] = (asmB/numPixels)/var3_2_B;
      pos++;
    }
    // Valores de ASSIMETRIA das componentes R, G, e B em separado:
    // Normaliza o vetor e depois escalona:
    NormalizeFloatVector( imgSmp_R_AsmVectortmp, _inStatisticsSize );
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_R_AsmVectortmp, _inStatisticsSize);
    NormalizeFloatVector( imgSmp_G_AsmVectortmp, _inStatisticsSize );
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_G_AsmVectortmp, _inStatisticsSize);
    NormalizeFloatVector( imgSmp_B_AsmVectortmp, _inStatisticsSize );
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_B_AsmVectortmp, _inStatisticsSize);
#endif


    // Gerando as estat�sticas (ENTROPIA):
    pos = 0;    //< O �ndice da posi��o utilizada do vetor � setado novamente em zero.
    float etpR=0, etpG=0, etpB=0;
    // Preenchendo os valores dos histogramas:
    for( int k=0 ; k<_inStatisticsSize ; k++ )
    {
      // numPixels aqui representa o n�mero de pixels existentes na janela:
      int numPixels = ((imgSmpExtYmax-k)-(imgSmpExtYmin+k)+1)*((imgSmpExtXmax-k)-(imgSmpExtXmin+k)+1);

      etpR=0; etpG=0; etpB=0;
      // De posse dos valores do histograma, posso calcular os valores de entropia:
      for( int j=imgSmpExtYmin+k ; j<=imgSmpExtYmax-k ; j++ )
      {
        for( int i=imgSmpExtXmin+k ; i<=imgSmpExtXmax-k ; i++ )
        {
          unsigned char retR, retG, retB;
          float probR=0, probG=0, probB=0; 
          amsSmpInImageGetPixel( i, j, &retR, &retG, &retB );
          // Componente R:
          probR = (float)Mhst_R[k][retR]/(float)numPixels;
          etpR += probR*(float)log10(probR);
          // Componente G:
          probG = (float)Mhst_G[k][retG]/(float)numPixels;
          etpG += probG*(float)log10(probG);
          // Componente B:
          probB = (float)Mhst_R[k][retR]/(float)numPixels;
          etpB += probR*(float)log10(probR);
        }
      }

      // Inserindo os valores finais nos vetores correspondentes:
      imgSmp_R_EntpVectortmp[pos] = ABS(etpR);
      imgSmp_G_EntpVectortmp[pos] = ABS(etpG);
      imgSmp_B_EntpVectortmp[pos] = ABS(etpB);
      pos++;
    }
    // Valores de ENTROPIA das componentes R, G, e B em separado:
    // Normaliza o vetor e depois escalona:
    NormalizeFloatVector( imgSmp_R_EntpVectortmp, _inStatisticsSize );
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_R_EntpVectortmp, _inStatisticsSize);
    NormalizeFloatVector( imgSmp_G_EntpVectortmp, _inStatisticsSize );
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_G_EntpVectortmp, _inStatisticsSize);
    NormalizeFloatVector( imgSmp_B_EntpVectortmp, _inStatisticsSize );
    IntervalScaleInFloat_Zero_One_Vector( imgSmp_B_EntpVectortmp, _inStatisticsSize);



    // COORDENADAS DO PIXEL CENTRAL DA AMOSTRA:
    int j1 = (imgSmpExtYmin + imgSmpExtYmax)/2;
    int i1 = (imgSmpExtXmin + imgSmpExtXmax)/2;
    unsigned char retR, retG, retB;
    amsSmpInImageGetPixel( i1, j1, &retR, &retG, &retB );
    // Obtendo os valores das tr�s componentes, j� convertido para float:
    // ESSAS TR�S VARI�VEIS S�O DA PR�PRIA CLASSE, POR ISSO N�O S�O RECEBIDOS COMO PAR�METROS:
    _out_R_PixelsVector[0] = (float)((float)retR/255);
    _out_G_PixelsVector[0] = (float)((float)retG/255);
    _out_B_PixelsVector[0] = (float)((float)retB/255);


    // Insere os valores do pixel central da amostra de entrada na matriz:
    _out_R_PixelsVector[0] = _out_R_PixelsVector[0]*0.8f+0.1f;
    _out_G_PixelsVector[0] = _out_G_PixelsVector[0]*0.8f+0.1f;
    _out_B_PixelsVector[0] = _out_B_PixelsVector[0]*0.8f+0.1f;

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

#if 0
    // Alocando as matrizes de amostras iniciais de entrada e sa�da:
    _inMatrix = new float*[maxPts];
    _outMatrix = new float*[maxPts];
    for( int i=0 ; i<maxPts ; i++ )
    {
      _inMatrix[i]  = new float[inSize];
      _outMatrix[i] = new float[outSize];
    }
#endif


    // Alocando espa�o para a matriz de pixels de sa�da:
    _outMatrix = new float*[maxPts];
    float *tmp_outMatrix = new float[maxPts*outSize];
    for( int i=0 ; i<maxPts ; i++ )
    {
      _outMatrix[i] = &tmp_outMatrix[i*outSize];
    }

    // Alocando espa�o para as matrizes que ir�o conter cada um dos pixels centrais das amostras:
    _out_R_PixelsMatrix = new float*[maxPts];
    _out_G_PixelsMatrix = new float*[maxPts];
    _out_B_PixelsMatrix = new float*[maxPts];
    float *_out_PixelsMatrix = new float[3*maxPts*1];
    for( int i=0 ; i<maxPts ; i++ )
    {
      // Cada posi��o ir� conter apenas a componente R, G, ou B do pixel:
      _out_R_PixelsMatrix[i] = &_out_PixelsMatrix[0*maxPts*1+i];
      _out_G_PixelsMatrix[i] = &_out_PixelsMatrix[1*maxPts*1+i];
      _out_B_PixelsMatrix[i] = &_out_PixelsMatrix[2*maxPts*1+i];
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
    // 2. Matrizes das m�dias das amostras, levando em contra janelas de tamanho 1, 2, ..., _inStatisticsSize:
    _in_MeanR_Matrix = new float*[maxPts];
    _in_MeanG_Matrix = new float*[maxPts];
    _in_MeanB_Matrix = new float*[maxPts];
    // 3. Matrizes das vari�ncias das amostras, levando em contra janelas de tamanho 1, 2, ..., _inStatisticsSize:
    _in_VarR_Matrix = new float*[maxPts];
    _in_VarG_Matrix = new float*[maxPts];
    _in_VarB_Matrix = new float*[maxPts];
    // 4. Matrizes dos coeficientes de varia��o das amostras, levando em contra janelas de tamanho 1, 2, ..., _inStatisticsSize:
    _in_CvarR_Matrix = new float*[maxPts];
    _in_CvarG_Matrix = new float*[maxPts];
    _in_CvarB_Matrix = new float*[maxPts];
    // 5. Matrizes dos valores de assimetria das amostras, levando em contra janelas de tamanho 1, 2, ..., _inStatisticsSize:
    _in_AsmR_Matrix = new float*[maxPts];
    _in_AsmG_Matrix = new float*[maxPts];
    _in_AsmB_Matrix = new float*[maxPts];
    // 6. Matrizes dos valores de entropia das amostras, levando em contra janelas de tamanho 1, 2, ..., _inStatisticsSize:
    _in_EntpR_Matrix = new float*[maxPts];
    _in_EntpG_Matrix = new float*[maxPts];
    _in_EntpB_Matrix = new float*[maxPts];


#ifdef COMPONENT_MATRICES
    // Mem�ria para as tr�s matrizes de componentes R, G e B em separado:
    float* in_MatrixTotalVec = new float[3*maxPts*_inComponentSize];
#endif

    // Mem�ria para as tr�s matrizes de m�dias R, G e B em separado:
    float* in_Mean_MatrixTotalVec = new float[3*maxPts*_inStatisticsSize];

    // Mem�ria para as tr�s matrizes de vari�ncias R, G e B em separado:
    float* in_Var_MatrixTotalVec = new float[3*maxPts*_inStatisticsSize];

#ifdef CVAR_MATRICES
    // Mem�ria para as tr�s matrizes de coeficiente de varia��o R, G e B em separado:
    float* in_Cvar_MatrixTotalVec = new float[3*maxPts*_inStatisticsSize];
#endif

#ifdef ASM_MATRICES
    // Mem�ria para as tr�s matrizes de assimetria R, G e B em separado:
    float* in_Asm_MatrixTotalVec = new float[3*maxPts*_inStatisticsSize];
#endif

    // Mem�ria para as tr�s matrizes de entropia de Shannon R, G e B em separado:
    float* in_Entp_MatrixTotalVec = new float[3*maxPts*_inStatisticsSize];

    // Alocando cada uma das linhas dessa mesma matriz:
    for( int i=0 ; i< maxPts ; i++ )
    {
#ifdef COMPONENT_MATRICES
      _in_R_Matrix[i] = &in_MatrixTotalVec[0*maxPts*_inComponentSize + i*_inComponentSize];
      _in_G_Matrix[i] = &in_MatrixTotalVec[1*maxPts*_inComponentSize + i*_inComponentSize];
      _in_B_Matrix[i] = &in_MatrixTotalVec[2*maxPts*_inComponentSize + i*_inComponentSize];
#endif
      _in_VarR_Matrix[i] = &in_Var_MatrixTotalVec[0*maxPts*_inStatisticsSize + i*_inStatisticsSize];
      _in_VarG_Matrix[i] = &in_Var_MatrixTotalVec[1*maxPts*_inStatisticsSize + i*_inStatisticsSize];
      _in_VarB_Matrix[i] = &in_Var_MatrixTotalVec[2*maxPts*_inStatisticsSize + i*_inStatisticsSize];

      _in_MeanR_Matrix[i] = &in_Mean_MatrixTotalVec[0*maxPts*_inStatisticsSize + i*_inStatisticsSize];
      _in_MeanG_Matrix[i] = &in_Mean_MatrixTotalVec[1*maxPts*_inStatisticsSize + i*_inStatisticsSize];
      _in_MeanB_Matrix[i] = &in_Mean_MatrixTotalVec[2*maxPts*_inStatisticsSize + i*_inStatisticsSize];

#ifdef CVAR_MATRICES
      _in_CvarR_Matrix[i] = &in_Cvar_MatrixTotalVec[0*maxPts*_inStatisticsSize + i*_inStatisticsSize];
      _in_CvarG_Matrix[i] = &in_Cvar_MatrixTotalVec[1*maxPts*_inStatisticsSize + i*_inStatisticsSize];
      _in_CvarB_Matrix[i] = &in_Cvar_MatrixTotalVec[2*maxPts*_inStatisticsSize + i*_inStatisticsSize];
#endif

#ifdef ASM_MATRICES
      _in_AsmR_Matrix[i] = &in_Asm_MatrixTotalVec[0*maxPts*_inStatisticsSize + i*_inStatisticsSize];
      _in_AsmG_Matrix[i] = &in_Asm_MatrixTotalVec[1*maxPts*_inStatisticsSize + i*_inStatisticsSize];
      _in_AsmB_Matrix[i] = &in_Asm_MatrixTotalVec[2*maxPts*_inStatisticsSize + i*_inStatisticsSize];
#endif

      _in_EntpR_Matrix[i] = &in_Entp_MatrixTotalVec[0*maxPts*_inStatisticsSize + i*_inStatisticsSize];
      _in_EntpG_Matrix[i] = &in_Entp_MatrixTotalVec[1*maxPts*_inStatisticsSize + i*_inStatisticsSize];
      _in_EntpB_Matrix[i] = &in_Entp_MatrixTotalVec[2*maxPts*_inStatisticsSize + i*_inStatisticsSize];
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
    // Vetores TEMPOR�RIOS para armazenar temporariamente as m�dias de cada uma das janelas das amostras de entrada:
    _in_MeanR_Vector = new float[_inStatisticsSize];
    _in_MeanG_Vector = new float[_inStatisticsSize];
    _in_MeanB_Vector = new float[_inStatisticsSize];
    // Vetores TEMPOR�RIOS para armazenar temporariamente as vari�ncias de cada uma das janelas das amostras de entrada:
    _in_VarR_Vector = new float[_inStatisticsSize];
    _in_VarG_Vector = new float[_inStatisticsSize];
    _in_VarB_Vector = new float[_inStatisticsSize];
    // Vetores TEMPOR�RIOS para armazenar temporariamente as os valores de coeficiente de varia��o das amostras de entrada:
    _in_CvarR_Vector = new float[_inStatisticsSize];
    _in_CvarG_Vector = new float[_inStatisticsSize];
    _in_CvarB_Vector = new float[_inStatisticsSize];
    // Vetores TEMPOR�RIOS para armazenar temporariamente as os valores de assimetria das amostras de entrada:
    _in_AsmR_Vector = new float[_inStatisticsSize];
    _in_AsmG_Vector = new float[_inStatisticsSize];
    _in_AsmB_Vector = new float[_inStatisticsSize];
    // Vetores TEMPOR�RIOS para armazenar temporariamente as os valores de entropia das amostras de entrada:
    _in_EntpR_Vector = new float[_inStatisticsSize];
    _in_EntpG_Vector = new float[_inStatisticsSize];
    _in_EntpB_Vector = new float[_inStatisticsSize];
    // Vetores TEMPOR�RIOS para armazenar temporariamente as tr�s componentes (RGB) do pixel central de cada amostra:
    _out_R_PixelsVector = new float[1];
    _out_G_PixelsVector = new float[1];
    _out_B_PixelsVector = new float[1];


    // Encontrando o tamanho total de cada uma das amostras de sa�da:
    _inSamplesSize = 0;
#ifdef COMPONENT_MATRICES
    _inSamplesSize += 3*_inComponentSize;
#endif
#ifdef ASM_MATRICES
    _inSamplesSize += 3*_inStatisticsSize;
#endif
#ifdef CVAR_MATRICES
    _inSamplesSize += 3*_inStatisticsSize;
#endif
    // As demais estat�sticas (m�dia, vari�ncia e entropia) s�o obrigat�rias:
    _inSamplesSize += 9*_inStatisticsSize + 3;

    // Criando o vetor de flags:
    _inSamplesFlagVector = new bool[_inSamplesSize];
    // A princ�pio, todos os elementos do vetor s�o v�lidos:
    for( int i=0 ; i<_inSamplesSize ; i++ )
      _inSamplesFlagVector[i] = true;


    // INCLUINDO OS HISTOGRAMAS:
    Mhst_R = new int*[_inStatisticsSize];   
    Mhst_G = new int*[_inStatisticsSize];   
    Mhst_B = new int*[_inStatisticsSize];
    // Alocando a mem�ria para as matrizes dos histogramas:
    for( int c=0 ; c<_inStatisticsSize ; c++ )
    {
      Mhst_R[c] = new int[256];
      Mhst_G[c] = new int[256];
      Mhst_B[c] = new int[256];
    }

    // Criando o vetor que cont�m todas as medi��es das amostras de entrada:
    _dataTypes = new samplesData[_inSamplesSize];

    // Faz a matriz que conter� as amostras j� preparadas ser inicializada:
    _prepInSamplesMatrix = NULL;


    // Criando a matriz que ir� armazenar as coordenadas:
    _CoordsMatrix = new int*[maxPts];
    // Vetor tempor�rio para conter toda a mem�ria que ser� utilizada pela matriz:
    int *CoordsVector = new int[2*maxPts];
    for( int i=0 ; i<maxPts ; i++ )
    {
      _CoordsMatrix[i] = &CoordsVector[i*2];
    }

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

    // CRIANDO OS DEMAIS VALORES DAS AMOSTRAS (VARI�NCIA, M�DIA, ETC.):
    int tmp_nPts = 0;

    int jog=0, cmp=0;
    int tmpi=0;
    // AGORA ELA CRIA TODAS AS AMOSTRAS!!!
    for( int i=0 ; i<_mascXdim ; i++ )
      for( int j=0 ; j<_mascYdim ; j++ )
      {
        // Caso existam todas as coordenadas necess�rias para a cria��o da amostra, ela � criada:
        if( (i>(_samplesWidth/2+1)) && (i<(_mascXdim-(_samplesWidth/2+2))) &&
          (j>(_samplesHeight/2+1)) && (j<(_mascYdim-(_samplesHeight/2+2))) )
        {

          // Gera a amostra:
          // AQUI EU CHAMO TODAS AS COMPONENTES, MAS A FUN��O S� EFETIVAMENTE CRIAR� AS QUE ESTIVEREM DEFINIDAS:
          imgSmpSeparateComponentSampleCreate( (i-_samplesWidth/2), (j-_samplesHeight/2), 
            (i+_samplesWidth/2), (j+_samplesHeight/2), 
            &_in_R_Matrix[tmp_nPts], &_in_G_Matrix[tmp_nPts], &_in_B_Matrix[tmp_nPts],
            &_in_MeanR_Matrix[tmp_nPts], &_in_MeanG_Matrix[tmp_nPts], &_in_MeanB_Matrix[tmp_nPts], 
            &_in_VarR_Matrix[tmp_nPts], &_in_VarG_Matrix[tmp_nPts], &_in_VarB_Matrix[tmp_nPts],
            &_in_CvarR_Matrix[tmp_nPts], &_in_CvarG_Matrix[tmp_nPts], &_in_CvarB_Matrix[tmp_nPts],
            &_in_AsmR_Matrix[tmp_nPts], &_in_AsmG_Matrix[tmp_nPts], &_in_AsmB_Matrix[tmp_nPts],
            &_in_EntpR_Matrix[tmp_nPts], &_in_EntpG_Matrix[tmp_nPts], &_in_EntpB_Matrix[tmp_nPts]
            );

          // Insere os valores do pixel central da amostra de entrada na matriz:
          _out_R_PixelsMatrix[tmp_nPts][0] = _out_R_PixelsVector[0]*0.8f+0.1f;
          _out_G_PixelsMatrix[tmp_nPts][0] = _out_G_PixelsVector[0]*0.8f+0.1f;
          _out_B_PixelsMatrix[tmp_nPts][0] = _out_B_PixelsVector[0]*0.8f+0.1f;

          // Armazena as coordenadas dessa amostra na matriz de coordenadas:
          // TODAS AS COORDENADAS EST�O ARMAZENADAS NA ORDEM y,x:
          _CoordsMatrix[tmp_nPts][0] = j;
          _CoordsMatrix[tmp_nPts][1] = i;

          if( i >= tmpi )
            tmpi=i;


          
          // Tentando criar as amostras de sa�da:
          if( _masc[j][i] == PAINT_COLOR_PLAYER )  //< 0 � jogador.
          {
            _outMatrix[tmp_nPts][0] = 0.1f;
            jog++;
          }
          if( _masc[j][i] == PAINT_COLOR_FIELD )  //< 0 � campo.
          {
            _outMatrix[tmp_nPts][0] = 0.9f;
            cmp++;
          }
          if( ( _masc[j][i] != PAINT_COLOR_FIELD ) && ( _masc[j][i] != PAINT_COLOR_PLAYER ))
            _outMatrix[tmp_nPts][0] = -1;   //< Caso n�o seja nada, insere -1.

          // Caso o n�mero de amostras seja m�ximo, realoca mem�ria:
          if ( tmp_nPts == (_maxPts) ) 
            amsRealocMatrices();
          // Incrementando o n�mero de amostras j� criadas:
          tmp_nPts++;
        }
      }
      _nPts = tmp_nPts;
      return 0;
  }



  /**
  * FUN��O UTILIZADA PARA CARREGAR SOMENTE AS AMOSTRAS J� MONTADAS, LEVANDO EM CONTA OS FLAGS EXISTENTES.
  * @param retInMatrix Matriz de amostras de entrada a ser retornada.
  * @param retInSamlesSize Tamanho de cada uma das amostras de entrada contidas na matriz de entradas.
  * @param retOutMatrix Matriz contendo cada uma das amostras de sa�da.
  * @param retOutSamlesSize Dimens�o de cada uma das amostras de sa�da, ou seja, DE CADA LINHA da matriz.
  * @param retCoordsInMatrix Matriz contendo a respectiva coordenada de cada uma das amostras retornadas.
  * @param numSamples N�mero de amostras existentes na matriz.
  */
  int getSamplesMatrix( float ***retInMatrix, int *retInSamlesSize, float ***retOutMatrix, int *retOutSamlesSize, 
                        int ***retCoordsInMatrix, int *retNumSamples )
  {
    // Obtendo o n�mero de amostras contidas na estrutura:
    int numSamples = _nPts;
    // Obtendo o n�mero de elementos contidos em cada amostra (linha) da matriz:
    int inSamplesSize = flagsVectorCount();

    // Verifica se essa matriz j� foi alocada anteriormente. Caso j� tenha sido, a atual � deletada
    // e toda ela (j� na nova configura��o) � refeita:
    if( _prepInSamplesMatrix )
      delete _prepInSamplesMatrix;
    // Alocando a matriz a ser retornada:
    _prepInSamplesMatrix = new float*[numSamples];
    // Vetor contendo toda a por��o da mem�ria que ser� utilizada pela matriz:
    float *allSamplesVector = new float[numSamples*inSamplesSize];

    for( int i=0 ; i<numSamples ; i++ )
    {
      int pos=0;
      int on_pos=0;
      _prepInSamplesMatrix[i] = &allSamplesVector[i*inSamplesSize];

      // Copiando os valores de m�dias das tr�s componentes:
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_MeanR_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_MeanG_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_MeanB_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      // Copiando os valores de vari�ncias das tr�s componentes:
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_VarR_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_VarG_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_VarB_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }

      // Inserindo o valor das componentes RGB do pixel central (o pixel sendo analisado):
      if( _inSamplesFlagVector[pos] == true)
      {
        _prepInSamplesMatrix[i][on_pos] = _out_R_PixelsMatrix[i][0];
        on_pos++;
      }
      pos++;
      if( _inSamplesFlagVector[pos] == true)
      {
        _prepInSamplesMatrix[i][on_pos] = _out_G_PixelsMatrix[i][0];
        on_pos++;
      }
      pos++;
      if( _inSamplesFlagVector[pos] == true)
      {
        _prepInSamplesMatrix[i][on_pos] = _out_B_PixelsMatrix[i][0];
        on_pos++;
      }
      pos++;

      // Copiando os valores de entropia das tr�s componentes:
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_EntpR_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_EntpG_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_EntpB_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }

#ifdef CVAR_MATRICES
      // Copiando os valores de coeficientes de varia��o das tr�s componentes:
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_CvarR_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_CvarG_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
        _prepInSamplesMatrix[i][on_pos] = _in_CvarB_Matrix[i][j];
        on_pos++;
        }
        pos++;
      }
#endif

#ifdef ASM_MATRICES
      // Copiando os valores de assimetria das tr�s componentes:
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_AsmR_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_AsmG_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inStatisticsSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_AsmB_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
#endif

#ifdef COMPONENT_MATRICES
      // Copiando as tr�s componentes, R, G e B:
      for( int j=0 ; j<_inComponentSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_R_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inComponentSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_G_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
      for( int j=0 ; j<_inComponentSize ; j++ )
      {
        if( _inSamplesFlagVector[pos] == true)
        {
          _prepInSamplesMatrix[i][on_pos] = _in_B_Matrix[i][j];
          on_pos++;
        }
        pos++;
      }
#endif

    }

    // Retornando as matrizes criadas:
    *retInMatrix      = _prepInSamplesMatrix;
    *retInSamlesSize  = inSamplesSize; 
    *retOutMatrix     = _outMatrix;
    *retOutSamlesSize = 1;
    *retCoordsInMatrix = _CoordsMatrix;
    *retNumSamples    = numSamples;

    return 0;
  }



  /**
  * FUN��O UTILIZADA PARA CARREGAR SOMENTE AS AMOSTRAS J� MONTADAS QUE SEJAM AMOSTRAS DE TREINO.
  * Fun��o que aloca matrizes e retorna, a partir das amostras atualmente preparadas, somente aquelas que 
  * s�o amostras de treino.
  * @param retInTypesVector Tipo de cada uma das amostras de treino retornadas (jogador, campo, etc).
  * @param retInMatrix Matriz de amostras de entrada DE TREINO retornada.
  * @param retInSamlesSize Tamanho de cada uma das amostras de entrada contidas na matriz de entradas.
  * @param retOutMatrix Matriz contendo cada uma das amostras DE TREINO de sa�da.
  * @param retOutSamlesSize Dimens�o de cada uma das amostras de sa�da, ou seja, DE CADA LINHA da matriz.
  * @param coordsMatrix Matrix contendo as coordenadas de cada uma das amostras de treino retornadas.
  * @param numSamples N�mero de amostras existentes na matriz.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int getTrainSamplesMatrix( int **retInTypesVector, float ***retInMatrix, int *retInSamlesSize, 
                             float ***retOutMatrix, int *retOutSamlesSize, int ***retCoordsMatrix, int *retNumSamples )
  {
    // Vetor de ponteiros de amostras de entrada de treino:
    float **tmpInTrainMatrix = new float*[_nPts];
    // Vetor de ponteiros de amostras de sa�da de treino:
    float **tmpOutTrainMatrix = new float*[_nPts];
    // Vetor contendo os tipos de cada uma das amostras (jogador, campo, etc.):
    int *tmpSamplesType = new int[_nPts];
    // Matriz contendo cada uma das coordenadas das amostras de treino retornadas:
    int **tmpCoordsMatrix = new int*[_nPts];

    int pos=0;
    int trainpos=0;

   
    for( int i=0 ; i<_mascXdim ; i++ )
      for( int j=0 ; j<_mascYdim ; j++ )
      {
        // Caso existam todas as coordenadas necess�rias para a cria��o da amostra, ela � criada:
        if( (i>(_samplesWidth/2+1)) && (i<(_mascXdim-(_samplesWidth/2+2))) &&
          (j>(_samplesHeight/2+1)) && (j<(_mascYdim-(_samplesHeight/2+2))) )
        {
          if( (_masc[j][i] == PAINT_COLOR_PLAYER) || (_masc[j][i] == PAINT_COLOR_FIELD) )
          {
            tmpInTrainMatrix[trainpos] = _prepInSamplesMatrix[pos];
            tmpOutTrainMatrix[trainpos] = _outMatrix[pos];
            tmpCoordsMatrix[trainpos] = _CoordsMatrix[pos];
            tmpSamplesType[trainpos] = _masc[j][i];
            trainpos++;
          }
          // Incrementa o apontador da amostra referente a posi��o atual (coordenada):
          pos++;
        }
      }
    
 /*
    int jog=0, cmp=0;
      // Para cada uma das matrizes criadas:
      for( int k=0 ; k<_nPts ; k++ )
      {
        if( (ABS(_outMatrix[k][0]-0.1f)<1e-4 ) || (ABS(_outMatrix[k][0]-0.9f)<1e-4) )
        {
          tmpInTrainMatrix[trainpos] = _prepInSamplesMatrix[k];
          tmpOutTrainMatrix[trainpos] = _outMatrix[k];
          if( (ABS(_outMatrix[k][0]-0.1f)<1e-4 ) )
            jog++;
          if(ABS(_outMatrix[k][0]-0.9f)<1e-4)
            cmp++;

          tmpCoordsMatrix[trainpos] = _CoordsMatrix[k];
          //tmpSamplesType[trainpos] = _masc[j][i];
          trainpos++;
        }
      }
*/
      *retInTypesVector = tmpSamplesType;
      *retInMatrix = tmpInTrainMatrix;
      *retInSamlesSize = _inSamplesSize;
      *retOutMatrix = tmpOutTrainMatrix;
      *retOutSamlesSize = 1;
      *retNumSamples = trainpos;
      return 0;
  }



  /**
  * Fun��o que obt�m um ponteiro para a amostra referente ao pixel cujas coordenadas s�o recebidas.
  * @param retInVector Vetor retornado, contendo a amostra.
  * @param InVectorSize Tamanho da amostra gerada, ou seja, a dimens�o do vetor.
  * @param i Coordenada x do pixel que se deseja classificar.
  * @param j Coordenada y do pixel que se deseja classificar.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int getOneSampleVector( float *sample, int nlength, int x, int y)
  {
    int pos = 0;
    for( int i=0 ; i<_mascXdim ; i++ )
    {
      for( int j=0 ; j<_mascYdim ; j++ )
      {
        // Incrementa o contador at� que a amostra seja encontrada:
        if( (i>(_samplesWidth/2+1)) && (i<(_mascXdim-(_samplesWidth/2+2))) &&
          (j>(_samplesHeight/2+1)) && (j<(_mascYdim-(_samplesHeight/2+2))) )
        {
          // Caso a posi��o seja encontrada, retorna o vetor que representa essa amostra:
          if((x==i) && (y==j))
          {
            sample = _prepInSamplesMatrix[pos];
            return 0;
          }
          // Caso contr�rio, apenas incrementa o contador:
          pos++;
        }
      }
    }
    // Caso chegue aqui (posi��o n�o encontrada, retorna erro:
    return -1;
  }


  /************************************************************************/
  /************************************************************************/
  /************************************************************************/
  /*                       Fun��es menos utilizadas:                      */
  /************************************************************************/
  /************************************************************************/
  /************************************************************************/


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
  * Destrutor.
  */
  ~Amostras()
  {
    //delete _inMatrix;
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

    // Realoca as matrizes de coeficientes de varia��o das amostras de entrada:
    amsRealocMatrix( &_in_CvarR_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_CvarG_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_CvarB_Matrix, _inStatisticsSize, _maxPts, _incSize);

    // Realoca as matrizes de assimetria das amostras de entrada:
    amsRealocMatrix( &_in_AsmR_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_AsmG_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_AsmB_Matrix, _inStatisticsSize, _maxPts, _incSize);

    // Realoca as matrizes de entropia das amostras de entrada:
    amsRealocMatrix( &_in_EntpR_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_EntpG_Matrix, _inStatisticsSize, _maxPts, _incSize);
    amsRealocMatrix( &_in_EntpB_Matrix, _inStatisticsSize, _maxPts, _incSize);

    // Realoca as matrizes de pixels centrais de cada uma das amostras:
    amsRealocMatrix( &_out_R_PixelsMatrix, 1, _maxPts, _incSize);
    amsRealocMatrix( &_out_G_PixelsMatrix, 1, _maxPts, _incSize);
    amsRealocMatrix( &_out_B_PixelsMatrix, 1, _maxPts, _incSize);

    // Incrementa o n�mero m�ximo de amostras que podem ser inseridas nas matrizes:
    _maxPts += _incSize;
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


};





















#if 0


/**
* Cria uma amostra de entrada de teste.
* @param retInVector Vetor retornado, contendo a amostra.
* @param InVectorSize Tamanho da amostra gerada, ou seja, a dimens�o do vetor.
* @param i Coordenada x do pixel que se deseja classificar.
* @param j Coordenada y do pixel que se deseja classificar.
* @param retInVectorSize N�mero de elementos EFETIVAMENTE contidos no vetor retornado.
* @return Retorna -1 em caso de erro (vetor nulo ou tamanho insuficiente), -2 (coordenadas inv�lidas), ou 0 caso ok.
*/
int getOneSampleVector( float *retInVector, int InVectorSize, int i, int j, int *retInVectorSize )
{
  // Obtendo o n�mero de elementos contidos em cada amostra:
  //int inSamplesSize = _inSamplesSize;
  int inSamplesSize = flagsVectorCount();

  // Casos de retorno:
  if( !(retInVector) || (InVectorSize < inSamplesSize ) )
    return -1;

  // Caso as coordenadas escolhidas sejam inv�lidas, retorna -2:
  if( !((i>(_samplesWidth/2+1)) && (i<(_mascXdim-(_samplesWidth/2+2))) &&
    (j>(_samplesHeight/2+1)) && (j<(_mascYdim-(_samplesHeight/2+2)))) )
    return -2;

  // Caso existam todas as coordenadas necess�rias para a cria��o da amostra, ela � criada:
  if( (i>(_samplesWidth/2+1)) && (i<(_mascXdim-(_samplesWidth/2+2))) &&
    (j>(_samplesHeight/2+1)) && (j<(_mascYdim-(_samplesHeight/2+2))) )
  {
    // Gera a amostra:
    imgSmpSeparateComponentSampleCreate( (i-_samplesWidth/2), (j-_samplesHeight/2), 
      (i+_samplesWidth/2), (j+_samplesHeight/2), 
      &_in_R_Vector, &_in_G_Vector, &_in_B_Vector,
      &_in_MeanR_Vector, &_in_MeanG_Vector, &_in_MeanB_Vector, 
      &_in_VarR_Vector, &_in_VarG_Vector, &_in_VarB_Vector,
      &_in_CvarR_Vector, &_in_CvarG_Vector, &_in_CvarB_Vector,
      &_in_AsmR_Vector, &_in_AsmG_Vector, &_in_AsmB_Vector,
      &_in_EntpR_Vector, &_in_EntpG_Vector, &_in_EntpB_Vector
      );
  }

  int pos=0;
  int on_pos=0;

  // Copiando os valores de m�dias das tr�s componentes:
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_MeanR_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_MeanG_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_MeanB_Vector[j];
      on_pos++;
    }
    pos++;
  }
  // Copiando os valores de vari�ncias das tr�s componentes:
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_VarR_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_VarG_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_VarB_Vector[j];
      on_pos++;
    }
    pos++;
  }

  // Inserindo o valor das componentes RGB do pixel central (o pixel sendo analisado):
  if( _inSamplesFlagVector[pos] == true)
  {
    retInVector[on_pos] = _out_R_PixelsVector[0];
    on_pos++;
  }
  pos++;
  if( _inSamplesFlagVector[pos] == true)
  {
    retInVector[on_pos] = _out_G_PixelsVector[0];
    on_pos++;
  }
  pos++;
  if( _inSamplesFlagVector[pos] == true)
  {
    retInVector[on_pos] = _out_B_PixelsVector[0];
    on_pos++;
  }
  pos++;

  // Copiando os valores de entropia das tr�s componentes:
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_EntpR_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_EntpG_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_EntpB_Vector[j];
      on_pos++;
    }
    pos++;
  }

  // Copiando os valores de coeficientes de varia��o das tr�s componentes:
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_CvarR_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_CvarG_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_CvarB_Vector[j];
      on_pos++;
    }
    pos++;
  }

  // Copiando os valores de assimetria das tr�s componentes:
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_AsmR_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_AsmG_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inStatisticsSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_AsmB_Vector[j];
      on_pos++;
    }
    pos++;
  }

#ifdef COMPONENT_MATRICES
  // Copiando as tr�s componentes, R, G e B:
  for( int j=0 ; j<_inComponentSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_R_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inComponentSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_G_Vector[j];
      on_pos++;
    }
    pos++;
  }
  for( int j=0 ; j<_inComponentSize ; j++ )
  {
    if( _inSamplesFlagVector[pos] == true)
    {
      retInVector[on_pos] = _in_B_Vector[j];
      on_pos++;
    }
    pos++;
  }
#endif

  // Retornando as matrizes criadas:
  *retInVectorSize = on_pos;

  return 0;
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

  // CRIANDO OS DEMAIS VALORES DAS AMOSTRAS (VARI�NCIA, M�DIA, ETC.):
  int tmp_nPts = 0;
  int AsmErrors = 0;
  int EntpErrors = 0;


  // AGORA ELA CRIA SOMENTE AS AMOSTRAS DE TREINAMENTO!!!
  for( int i=0 ; i<_mascXdim ; i++ )
    for( int j=0 ; j<_mascYdim ; j++ )
    {
      // Caso o valor desse ponto na matriz seja 0 ou 1, � preciso criar a amostra:
      if( (_masc[j][i] == PAINT_COLOR_PLAYER) || (_masc[j][i] == PAINT_COLOR_FIELD) )  //< 0 � jogador, 1 � campo.
      {
        // Caso existam todas as coordenadas necess�rias para a cria��o da amostra, ela � criada:
        if( (i>(_samplesWidth/2+1)) && (i<(_mascXdim-(_samplesWidth/2+2))) &&
          (j>(_samplesHeight/2+1)) && (j<(_mascYdim-(_samplesHeight/2+2))) )
        {
          // Gera a amostra:
          imgSmpSeparateComponentSampleCreate( (i-_samplesWidth/2), (j-_samplesHeight/2), 
            (i+_samplesWidth/2), (j+_samplesHeight/2), 
            &_in_R_Vector, &_in_G_Vector, &_in_B_Vector,
            &_in_MeanR_Vector, &_in_MeanG_Vector, &_in_MeanB_Vector, 
            &_in_VarR_Vector, &_in_VarG_Vector, &_in_VarB_Vector,
            &_in_CvarR_Vector, &_in_CvarG_Vector, &_in_CvarB_Vector,
            &_in_AsmR_Vector, &_in_AsmG_Vector, &_in_AsmB_Vector,
            &_in_EntpR_Vector, &_in_EntpG_Vector, &_in_EntpB_Vector
            );

          // Insere os valores nas demais matrizes de entrada:
#ifdef COMPONENT_MATRICES
          // Valores das componentes dos vetores R, G e B em separado:
          MatrixInsertVector( _in_R_Vector, _inComponentSize, _in_R_Matrix, _inComponentSize, tmp_nPts );
          MatrixInsertVector( _in_G_Vector, _inComponentSize, _in_G_Matrix, _inComponentSize, tmp_nPts );
          MatrixInsertVector( _in_B_Vector, _inComponentSize, _in_B_Matrix, _inComponentSize, tmp_nPts );
#endif

          // Valores das m�dias das componentes R, G, e B em separado:
          MatrixInsertVector( _in_MeanR_Vector, _inStatisticsSize, _in_MeanR_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_MeanG_Vector, _inStatisticsSize, _in_MeanG_Matrix, _inStatisticsSize, tmp_nPts  );
          MatrixInsertVector( _in_MeanB_Vector, _inStatisticsSize, _in_MeanB_Matrix, _inStatisticsSize, tmp_nPts );

          // Valores das vari�ncias das componentes R, G, e B em separado:
          MatrixInsertVector( _in_VarR_Vector, _inStatisticsSize, _in_VarR_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_VarG_Vector, _inStatisticsSize, _in_VarG_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_VarB_Vector, _inStatisticsSize, _in_VarB_Matrix, _inStatisticsSize, tmp_nPts );

          // Valores dos COEFICIENTES DE VARIA��O das componentes R, G, e B em separado:
          MatrixInsertVector( _in_CvarR_Vector, _inStatisticsSize, _in_CvarR_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_CvarG_Vector, _inStatisticsSize, _in_CvarG_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_CvarB_Vector, _inStatisticsSize, _in_CvarB_Matrix, _inStatisticsSize, tmp_nPts );

          // Valores de ASSIMETRIA das componentes R, G, e B em separado:
          MatrixInsertVector( _in_AsmR_Vector, _inStatisticsSize, _in_AsmR_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_AsmG_Vector, _inStatisticsSize, _in_AsmG_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_AsmB_Vector, _inStatisticsSize, _in_AsmB_Matrix, _inStatisticsSize, tmp_nPts );

          // Valores de ENTROPIA das componentes R, G, e B em separado:

          MatrixInsertVector( _in_EntpR_Vector, _inStatisticsSize, _in_EntpR_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_EntpG_Vector, _inStatisticsSize, _in_EntpG_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_EntpB_Vector, _inStatisticsSize, _in_EntpB_Matrix, _inStatisticsSize, tmp_nPts );

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
* Cria todas as amostras.
* � necess�rio que a matriz da m�scara tenha sido inserida.

int amsAllSamplesCreate()
{
  // Casos de retorno:
  if( (_mascXdim == 0) || (_mascYdim == 0) )
    return -1;

  // CRIANDO OS DEMAIS VALORES DAS AMOSTRAS (VARI�NCIA, M�DIA, ETC.):
  int tmp_nPts = 0;

  // AGORA ELA CRIA TODAS AS AMOSTRAS!!!
  for( int i=0 ; i<_mascXdim ; i++ )
    for( int j=0 ; j<_mascYdim ; j++ )
    {
      // Caso existam todas as coordenadas necess�rias para a cria��o da amostra, ela � criada:
      if( (i>(_samplesWidth/2+1)) && (i<(_mascXdim-(_samplesWidth/2+2))) &&
        (j>(_samplesHeight/2+1)) && (j<(_mascYdim-(_samplesHeight/2+2))) )
      {

        // Gera a amostra:
        imgSmpSeparateComponentSampleCreate( (i-_samplesWidth/2), (j-_samplesHeight/2), 
          (i+_samplesWidth/2), (j+_samplesHeight/2), 
          &_in_R_Matrix[tmp_nPts], &_in_G_Matrix[tmp_nPts], &_in_B_Matrix[tmp_nPts],
          &_in_MeanR_Matrix[tmp_nPts], &_in_MeanG_Matrix[tmp_nPts], &_in_MeanB_Matrix[tmp_nPts], 
          &_in_VarR_Matrix[tmp_nPts], &_in_VarG_Matrix[tmp_nPts], &_in_VarB_Matrix[tmp_nPts],
          &_in_CvarR_Matrix[tmp_nPts], &_in_CvarG_Matrix[tmp_nPts], &_in_CvarB_Matrix[tmp_nPts],
          &_in_AsmR_Matrix[tmp_nPts], &_in_AsmG_Matrix[tmp_nPts], &_in_AsmB_Matrix[tmp_nPts],
          &_in_EntpR_Matrix[tmp_nPts], &_in_EntpG_Matrix[tmp_nPts], &_in_EntpB_Matrix[tmp_nPts]
          );

#if 0
          // Gera a amostra:
          imgSmpSeparateComponentSampleCreate( (i-_samplesWidth/2), (j-_samplesHeight/2), 
            (i+_samplesWidth/2), (j+_samplesHeight/2), 
            &_in_R_Vector, &_in_G_Vector, &_in_B_Vector,
            &_in_MeanR_Vector, &_in_MeanG_Vector, &_in_MeanB_Vector, 
            &_in_VarR_Vector, &_in_VarG_Vector, &_in_VarB_Vector,
            &_in_CvarR_Vector, &_in_CvarG_Vector, &_in_CvarB_Vector,
            &_in_AsmR_Vector, &_in_AsmG_Vector, &_in_AsmB_Vector,
            &_in_EntpR_Vector, &_in_EntpG_Vector, &_in_EntpB_Vector
            );

          // Insere os valores nas demais matrizes de entrada:
#ifdef COMPONENT_MATRICES
          // Valores das componentes dos vetores R, G e B em separado:
          MatrixInsertVector( _in_R_Vector, _inComponentSize, _in_R_Matrix, _inComponentSize, tmp_nPts );
          MatrixInsertVector( _in_G_Vector, _inComponentSize, _in_G_Matrix, _inComponentSize, tmp_nPts );
          MatrixInsertVector( _in_B_Vector, _inComponentSize, _in_B_Matrix, _inComponentSize, tmp_nPts );
#endif

          // Valores das m�dias das componentes R, G, e B em separado:
          MatrixInsertVector( _in_MeanR_Vector, _inStatisticsSize, _in_MeanR_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_MeanG_Vector, _inStatisticsSize, _in_MeanG_Matrix, _inStatisticsSize, tmp_nPts  );
          MatrixInsertVector( _in_MeanB_Vector, _inStatisticsSize, _in_MeanB_Matrix, _inStatisticsSize, tmp_nPts );

          // Valores das vari�ncias das componentes R, G, e B em separado:
          MatrixInsertVector( _in_VarR_Vector, _inStatisticsSize, _in_VarR_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_VarG_Vector, _inStatisticsSize, _in_VarG_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_VarB_Vector, _inStatisticsSize, _in_VarB_Matrix, _inStatisticsSize, tmp_nPts );

#ifdef CVAR_MATRICES
          // Valores dos COEFICIENTES DE VARIA��O das componentes R, G, e B em separado:
          MatrixInsertVector( _in_CvarR_Vector, _inStatisticsSize, _in_CvarR_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_CvarG_Vector, _inStatisticsSize, _in_CvarG_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_CvarB_Vector, _inStatisticsSize, _in_CvarB_Matrix, _inStatisticsSize, tmp_nPts );
#endif

#ifdef ASM_MATRICES
          // Valores de ASSIMETRIA das componentes R, G, e B em separado:
          MatrixInsertVector( _in_AsmR_Vector, _inStatisticsSize, _in_AsmR_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_AsmG_Vector, _inStatisticsSize, _in_AsmG_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_AsmB_Vector, _inStatisticsSize, _in_AsmB_Matrix, _inStatisticsSize, tmp_nPts );
#endif

          // Valores de ENTROPIA das componentes R, G, e B em separado:
          MatrixInsertVector( _in_EntpR_Vector, _inStatisticsSize, _in_EntpR_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_EntpG_Vector, _inStatisticsSize, _in_EntpG_Matrix, _inStatisticsSize, tmp_nPts );
          MatrixInsertVector( _in_EntpB_Vector, _inStatisticsSize, _in_EntpB_Matrix, _inStatisticsSize, tmp_nPts );

#endif

          // Insere os valores do pixel central da amostra de entrada na matriz:
          _out_R_PixelsMatrix[tmp_nPts][0] = _out_R_PixelsVector[0]*0.8f+0.1f;
          _out_G_PixelsMatrix[tmp_nPts][0] = _out_G_PixelsVector[0]*0.8f+0.1f;
          _out_B_PixelsMatrix[tmp_nPts][0] = _out_B_PixelsVector[0]*0.8f+0.1f;

          // Tentando criar as amostras de sa�da:
          if( _masc[j][i] == PAINT_COLOR_PLAYER )  //< 0 � jogador.
            _outMatrix[tmp_nPts][0] = 0.1f;
          if( _masc[j][i] == PAINT_COLOR_FIELD )  //< 0 � campo.
            _outMatrix[tmp_nPts][0] = 0.9f;
          else
            _outMatrix[tmp_nPts][0] = -1;   //< Caso n�o seja nada, insere -1.

          // Caso o n�mero de amostras seja m�ximo, realoca mem�ria:
          if ( tmp_nPts == (_maxPts) ) 
            amsRealocMatrices();
          // Incrementando o n�mero de amostras j� criadas:
          tmp_nPts++;
      }
    }
    _nPts = tmp_nPts;
    return 0;
}
*/
#endif

