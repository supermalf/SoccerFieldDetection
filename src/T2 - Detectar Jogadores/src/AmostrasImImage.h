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
  int _maxPts;  ///< Limite m�ximo de amostras que podem atualmente ser armazenados na estrutura.
  int _nPts;    ///< N�mero de amostras efetivamente sendo armazenados na estrutura.
  int _incSize;
  int _inSize;
  int _outSize;
  float **_inMatrix;
  float **_outMatrix;

  imImage* _imageSample;   ///< Estrutura imImage que cont�m toda informa��o da imagem original.

  // M�scara da imagem de entrada:
  int **_masc;
  int _mascXdim;
  int _mascYdim;
  float *_mascSample;

  int _samplesWidth;
  int _samplesHeight;



  /** 
  * Obt�m um pixel da imagem de entrada. Todo o c�lculo para encontrar o pixel desejado � 
  * As coordenadas devem ser recebidas como bottom up. 
  * feito baseando-se no fato de que a inst�ncia de imImage recebida foi recebida como BitMap.
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
    if( !_imageSample )
      return -1;

    int h = _imageSample->height;
    int w = _imageSample->width;

    // Caso de retorno (coordenada requerida inexistente):
    if( (x>=w) || (y>=h) )
      return -1;

    printf(".");
    // ImImages possuem pixels em bottom up na estrutura. Al�m disso, as componentes R, G e B s�o IM_PACKED:
    printf("%c", _imageSample->data[0]);
    unsigned char *dataR = (unsigned char*)_imageSample->data[0];
    unsigned char *dataG = (unsigned char*)_imageSample->data[1];
    unsigned char *dataB = (unsigned char*)_imageSample->data[2];

    // Encontrando a posi��o do pixel no vetor:
    int pos = (y*w +x);     //< n�o � IM_PACKED.
    // Carregando as componentes do pixel requerido:
    * retR = dataR[pos];
    * retG = dataG[pos];
    * retB = dataB[pos];
    return 0;
  }


  /**
  * Cria uma amostra Sld para ser utilizada externamente. Cria um vetor que ir� conter as componentes
  * RGB de todos os pixels da amostra, desde o ponto Xmin,Ymin at� o ponto Xmax, Ymax (inclusive).
  * @param imgSmpExtXmin
  * @param imgSmpExtYmin
  * @param imgSmpExtXmax
  * @param imgSmpExtYmax
  * @param imgSmpExtVector
  * @return Retorna -1 em caso de erro (imagem inexistente, corrdenadas inexistentes, etc), ou 0 caso ok.
  */
  
  int imgSmpExtSampleCreate( int imgSmpExtXmin, int imgSmpExtYmin, int imgSmpExtXmax, int imgSmpExtYmax, float** imgSmpExtVector )
  {
    int smpDimension = 3*((imgSmpExtXmax-imgSmpExtXmin+1) * (imgSmpExtYmax-imgSmpExtYmin+1));

    /*
    float *imgSmpExtSample = new float[smpDimension];

    int pos=0;
    for( int j=imgSmpExtYmin ; j<=imgSmpExtYmax ; j++ )
    {
    for( int i=imgSmpExtXmin ; i<=imgSmpExtXmax ; i++ )
    {
    unsigned char retR, retG, retB;
    imgSmpInImageGetPixel( i, j, &retR, &retG, &retB );
    imgSmpExtSample[pos] = (float)(retR/255);
    imgSmpExtSample[smpDimension+pos] = (float)(retG/255);
    imgSmpExtSample[2*smpDimension+pos] = (float)(retB/255);
    }
    }
    *imgSmpExtVector = imgSmpExtSample;
    */

    float *imgSmpExtSample = *imgSmpExtVector;
    int pos=0;
    for( int j=imgSmpExtYmin ; j<=imgSmpExtYmax ; j++ )
    {
      for( int i=imgSmpExtXmin ; i<=imgSmpExtXmax ; i++ )
      {
        unsigned char retR, retG, retB;
        amsSmpInImageGetPixel( i, j, &retR, &retG, &retB );
        imgSmpExtSample[pos] = (float)(retR/255);
        imgSmpExtSample[smpDimension+pos] = (float)(retG/255);
        imgSmpExtSample[2*smpDimension+pos] = (float)(retB/255);
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
    _mascSample = new float[_inSize];
    _masc     = masc;
    _mascXdim = mascXdim;
    _mascYdim = mascYdim;
    return 0;
  }


  /**
  * Construtor.
  */
  Amostras( int maxPts, int samplesWidth, int samplesHeight, int outSize, int incSize, char * filename )
  {
    int inSize = (samplesWidth+1)*(samplesHeight+1)*3;
    //////////////////////////////////////////////////////////////////////////
    char format2[10], compression2[10];
    int error2, image_count2;
    int width2, height2, color_mode2, data_type2;


    error2=0;
    // Abrindo a imagem de entrada:
    imFile* ifile2 = imFileOpen("Canto dir1.tga", &error2);
    if (error2 != IM_ERR_NONE) 
      error2++;

    // Obtendo informa��es a respeito do formato do arquivo de entrada:
    imFileGetInfo(ifile2, format2, compression2, &image_count2);
    for (int i = 0; i < image_count2 ; i++)
    {
      error2 = imFileReadImageInfo(ifile2, i, &width2, &height2, &color_mode2, &data_type2);
      if (error2 != IM_ERR_NONE) 
        error2++;
    }

    // A partir do arquivo de entrada obtido, carregando uma estrutura imImage:
    // Apenas repassando o ponteiro para a estrutura:
    _imageSample = imFileLoadBitmap(ifile2, 0, &error2);
    printf("%c", _imageSample->data[0]);

    //////////////////////////////////////////////////////////////////////////
    

    // Tamanho em x e y das amostras que ser�o geradas:
    _samplesWidth = samplesWidth;
    _samplesHeight = samplesHeight;

    // Demais par�metros da estrutura:
    _maxPts   = maxPts;
    _inSize   = inSize;
    _outSize  = outSize;
    _incSize  = incSize;

    // Alocando as matrizes:
    _inMatrix = new float*[incSize];
    _outMatrix = new float*[incSize];
    for( int i=0 ; i<incSize ; i++ )
    {
      _inMatrix[i]  = new float[inSize];
      _outMatrix[i] = new float[outSize];
    }

    // Ainda n�o existem amostras na estrutura:
    _nPts = 0;

    // Alocando o vetor tempor�rio auxiliar na cria��o de cada uma das amostras:
    _mascSample = new float[inSize];

    // A m�scara ser� repassada posteriormente:
    _masc = NULL;
    _mascXdim = 0;
    _mascYdim = 0;


  }


  /**
  * Destrutor.
  */
  ~Amostras()
  {
    delete _inMatrix;
    delete _outMatrix;
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
    // Realoca cada um dos vetores 
    amsRealocMatrix( &_inMatrix, _inSize, _maxPts, _incSize);
    amsRealocMatrix( &_outMatrix, _outSize, _maxPts, _incSize);
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

    // Caso contr�rio cria todas as amostras:
    for( int i=0 ; i<_mascXdim ; i++ )
      for( int j=0 ; j<_mascYdim ; j++ )
      {
        // Caso o valor desse ponto na matriz seja 0 ou 1, � preciso criar a amostra:
        if( _masc[i][j] == 0 )
        {
          // Caso existam todas as coordenadas necess�rias para a cria��o da amostra, ela � criada:
          if( (i>(_samplesWidth/2+1)) && (i<(_mascXdim-(_samplesWidth/2+2))) &&
            (j>(_samplesHeight/2+1)) && (i<(_mascYdim-(_samplesHeight/2+2))) )
          {
            // Gera a amostra:
            imgSmpExtSampleCreate( (i-_samplesWidth/2), (j-_samplesHeight/2), 
              (i+_samplesWidth/2), (j+_samplesHeight/2), &_mascSample );
            // Insere na matriz de amostras de entrada:
            float tmpSaida = 0;
            amsInsertSample( _mascSample, &tmpSaida, _inSize, 1 );
          }
        }
      }
  }


  /**
  * Obt�m uma amostra de maneira aleatoria.
  */
  int amsGetRandomSample( float **inRet, float **outRet )
  {
    int amostra = RandInt(0, _nPts);
    
    *inRet  = &_inMatrix[amostra][0]; 
    *outRet = &_outMatrix[amostra][0];

    return 0;
  }

};
