#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>


#include <im.h>
#include <im_capture.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_format_avi.h>



/**
* Classe que mantém uma amostra, obtida a partir de imagens coloridas tipo TGA.
- Preciso ter disponível a imagem TGA de entrada a partir da qual criar as amostras.
- Preciso ter disponível as sub-imagens amostra, sendo que essa sub-imagem amostra pode 
conter uma máscara que será utilizada para gerar as amostras de treinamento.
- Preciso ter disponível métodos que disponibilizem a possibilidade de, ao definir
um tamanho de uma janela deslizante (sliding window), gere todas as amostras de 
treinamento. 
- Preciso de métodos que permitam salvar quaisquer que sejam as imagens disponíveis, bem 
como salvar cada uma das amostras como uma nova imagem .TGA de nome especificado.
*/
class ImageSample
{
public:
  imImage* _imgSmpInImage;           ///< Ponteiro para uma estrutura imImage que contém toda informação da imagem original.
  unsigned char *_imgSmpInImageData; ///< Vetor de pixels da imagem.

  int _imgSmpSubWidth;               ///< Dimensão X da sub-imagem amostra.
  int _imgSmpSubHeight;              ///< Dimensão Y da sub-imagem amostra.
  int _imgSmpOrigBeginX;             ///< Coordenada X da posição de início da sub-imagem amostra na imagem original.
  int _imgSmpOrigBeginY;             ///< Coordenada Y da posição de início da sub-imagem amostra na imagem original.

  int _imgSmpSubImageVectorSize;     ///< Número de pixels contidos na sub-imagem amostra.
  unsigned char *_imgSmpImageDataR;  ///< Vetor de pixels da sub-imagem amostra (componente R).
  unsigned char *_imgSmpImageDataG;  ///< Vetor de pixels da sub-imagem amostra (componente G).
  unsigned char *_imgSmpImageDataB;  ///< Vetor de pixels da sub-imagem amostra (componente B).

  int _imgSmpSldWidth;               ///< Dimensão X de cada uma das imagens Sld.
  int _imgSmpSldHeight;              ///< Dimensão Y de cada uma das imagens Sld.
  int _imgSmpSldImagesSize;          ///< Número de imagens sld nas três matrizes de Sld.
  int _imgSmpSldEachVectorSize;      ///< Número de pixels contidos na em cada uma das amostras sld.

  unsigned char **_imgSmpSldSamplesR;///< Matriz de cada uma das imagens Sld geradas (componente R).
  unsigned char **_imgSmpSldSamplesG;///< Matriz de cada uma das imagens Sld geradas (componente G).
  unsigned char **_imgSmpSldSamplesB;///< Matriz de cada uma das imagens Sld geradas (componente B).


  /** 
  * Obtém um pixel da imagem de entrada. Todo o cálculo para encontrar o pixel desejado é 
  * As coordenadas devem ser recebidas como bottom up. 
  * feito baseando-se no fato de que a instância de imImage recebida foi recebida como BitMap.
  * @param x Coordenada x do pixel a ser retornado.
  * @param y Coordenada y do pixel a ser retornado.
  * @param retR Componente R do pixel retornada.
  * @param retG Componente G do pixel retornada.
  * @param retB Componente B do pixel retornada.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int imgSmpInImageGetPixel( int x, int y, unsigned char* retR, unsigned char* retG, unsigned char* retB )
  {
    // Caso de retorno (imagem de entrada indexistente):
    if( !_imgSmpInImage )
      return -1;

    int h = _imgSmpInImage->height;
    int w = _imgSmpInImage->width;

    // Caso de retorno (coordenada requerida inexistente):
    if( (x>=w) || (y>=h) )
      return -1;

    // ImImages possuem pixels em bottom up na estrutura. Além disso, as componentes R, G e B são IM_PACKED:
    unsigned char *dataR = (unsigned char*)_imgSmpInImage->data[0];
    unsigned char *dataG = (unsigned char*)_imgSmpInImage->data[1];
    unsigned char *dataB = (unsigned char*)_imgSmpInImage->data[2];

    // Encontrando a posição do pixel no vetor:
    int pos = (y*w +x);     //< não é IM_PACKED.
    // Carregando as componentes do pixel requerido:
    * retR = dataR[pos];
    * retG = dataG[pos];
    * retB = dataB[pos];
    return 0;
  }


  /**
  * Uma vez recebida a imagem de entrada, cria os vetores da sub-imagem amostra.
  */
  int imgSmpSubImageCreate()
  {
    // Caso os vetores já tenham sido alocados, libera a memória e realoca:
    imgSmpSubImageAlloc();

    // Uma vez que existem os vetores, é preciso inserir os pixels corretamente:
    int pos=0;    //< Vetor que indica a primeira posição vazia no vetor de pixels da sub-imagem amostra.
    for( int j=_imgSmpOrigBeginY ; j<_imgSmpOrigBeginY+_imgSmpSubHeight ; j++ )
      for( int i=_imgSmpOrigBeginX ; i<_imgSmpOrigBeginX+_imgSmpSubWidth ; i++ )
      {
        unsigned char retR, retG, retB;
        imgSmpInImageGetPixel( i, j, &retR, &retG, &retB );

        // Caso não exista espaço nos vetores, retorna erro:
        if( pos >= _imgSmpSubImageVectorSize ) 
          return -1;
        // Caso contrário, insere o pixel atual na sub-imagem amostra:
        _imgSmpImageDataR[pos] = retR;
        _imgSmpImageDataG[pos] = retG;
        _imgSmpImageDataB[pos] = retB;
        pos++;
      }
      return 0;
  }


  /**
  * Aloca espaco para os vetores da sub-imagem amostra. Caso os vetores tenham sido anteriormente alocados,
  * libera a memória alocada e realoca do tamanho apropriado.
  */
  int imgSmpSubImageAlloc()
  {
    // Casos de retorno:
    if( (_imgSmpSubWidth == -1) || (_imgSmpSubHeight == -1) )
      return -1;

    // Em caso de alocação anterior, libera a memória anteriormente alocada:
    if( (_imgSmpImageDataR) || (_imgSmpImageDataG) || (_imgSmpImageDataB) )
    {
      delete _imgSmpImageDataR;
      delete _imgSmpImageDataG;
      delete _imgSmpImageDataB;
    }

    // Aloca as matrizes contendo o tamanho adequado:
    _imgSmpImageDataR = new unsigned char[_imgSmpSubImageVectorSize];
    _imgSmpImageDataG = new unsigned char[_imgSmpSubImageVectorSize];
    _imgSmpImageDataB = new unsigned char[_imgSmpSubImageVectorSize];

    return 0;
  }


  /**
  * Aloca espaco para a matriz de SldImages. Caso as matrizes tenham sido anteriormente alocadas,
  * libera a memória alocada e realoca do tamanho apropriado.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int imgSmpSldSamplesAlloc()
  {
    // Casos de retorno:
    if( (_imgSmpSldWidth == -1) || (_imgSmpSldHeight == -1) )
      return -1;

    // Em caso de alocação anterior, libera a memória anteriormente alocada:
    if( (_imgSmpSldSamplesR) || (_imgSmpSldSamplesG) || (_imgSmpSldSamplesB) )
    {
      delete _imgSmpSldSamplesR;
      delete _imgSmpSldSamplesG;
      delete _imgSmpSldSamplesB;
    }

    // Aloca as matrizes contendo o tamanho adequado:
    _imgSmpSldSamplesR = new unsigned char*[_imgSmpSldImagesSize];
    _imgSmpSldSamplesG = new unsigned char*[_imgSmpSldImagesSize];
    _imgSmpSldSamplesB = new unsigned char*[_imgSmpSldImagesSize];
    // Aloca o espaco para cada um dos vetores:
    _imgSmpSldEachVectorSize = _imgSmpSldWidth * _imgSmpSldHeight;
    for( int i=0 ; i<_imgSmpSldImagesSize ; i++ ) //< Resolver problema de alocação depois.
    {
      _imgSmpSldSamplesR[i] = new unsigned char[_imgSmpSldEachVectorSize];
      _imgSmpSldSamplesG[i] = new unsigned char[_imgSmpSldEachVectorSize];
      _imgSmpSldSamplesB[i] = new unsigned char[_imgSmpSldEachVectorSize];
    }

    return 0;
  }


  /**
  * Configura os valores das coordenadas da posição de início da sub-imagem amostra na imagem original.
  * @param imgSmpOrigBeginX Coordenada X da posição de início da sub-imagem amostra na imagem original.
  * @param imgSmpOrigBeginY Coordenada Y da posição de início da sub-imagem amostra na imagem original.
  * @return Retorna -1 em caso de erro (imagem de entrada inexistente), ou 0 caso ok.
  */
  int imgSmpOrigBeginSet( int imgSmpOrigBeginX, int imgSmpOrigBeginY )
  {
    // Caso não exista imagem de entrada, retorna erro:
    if( !_imgSmpInImage )
    {
      _imgSmpOrigBeginX = 0;
      _imgSmpOrigBeginY = 0;
      return -1;
    }

    // Caso contrário, insere os novos valores e retorna ok:
    _imgSmpOrigBeginX = imgSmpOrigBeginX;
    _imgSmpOrigBeginY = imgSmpOrigBeginY;
    return 0;
  }


  /**
  * Obtém os valores das coordenadas da posição de início da sub-imagem amostra na imagem original.
  * @param imgSmpOrigBeginX Coordenada X da posição de início da sub-imagem amostra na imagem original retornada.
  * @param imgSmpOrigBeginY Coordenada Y da posição de início da sub-imagem amostra na imagem original retornada.
  * @return Retorna -1 em caso de erro (imagem de entrada inexistente, coordenadas não compatíveis, etc), 
  * ou 0 caso ok.
  */
  int imgSmpOrigBeginGet( int *imgSmpOrigBeginX, int *imgSmpOrigBeginY )
  {
    // Caso não exista imagem de entrada, retorna NULL:
    if( !_imgSmpInImage )
    {
      _imgSmpOrigBeginX = 0;
      _imgSmpOrigBeginY = 0;
      return -1;
    }

    // Caso contrário, obtém os novos valores e retorna ok:
    *imgSmpOrigBeginX = _imgSmpOrigBeginX;
    *imgSmpOrigBeginY = _imgSmpOrigBeginY;
    return 0;
  }


  /**
  * Configura as dimensões da sub-imagem amostra.
  * @param imgSmpSubWidth Dimensão X da sub-imagem amostra.
  * @param imgSmpSubHeight Dimensão Y da sub-imagem amostra.
  * @return Retorna -1 em caso de erro (imagem de entrada inexistente), ou 0 caso ok.
  */
  int imgSmpDimensionsSet( int imgSmpSubWidth, int imgSmpSubHeight )
  {
    // Caso não exista imagem de entrada, retorna erro:
    if( !_imgSmpInImage )
    {
      _imgSmpSubWidth = 0;
      _imgSmpSubHeight = 0;
      return -1;
    }

    // Caso contrário, insere os novos valores e retorna ok:
    _imgSmpSubWidth = imgSmpSubWidth;
    _imgSmpSubHeight = imgSmpSubHeight;
    return 0;
  }


  /**
  * Obtém as dimensões da sub-imagem amostra.
  * @param imgSmpSubWidth Dimensão X da sub-imagem amostra retornada.
  * @param imgSmpSubHeight Dimensão Y da sub-imagem amostra retornada.
  * @return Retorna -1 em caso de erro (imagem de entrada inexistente), ou 0 caso ok.
  */
  int imgSmpDimensionsGet( int *imgSmpSubWidth, int *imgSmpSubHeight )
  {
    // Caso não exista imagem de entrada, retorna erro:
    if( !_imgSmpInImage )
    {
      _imgSmpSubWidth = 0;
      _imgSmpSubHeight = 0;
      return -1;
    }

    // Caso contrário, obtém os novos valores e retorna ok:
    *imgSmpSubWidth = _imgSmpSubWidth;
    *imgSmpSubHeight = _imgSmpSubHeight;
    return 0;
  }


  /**
  * Insere uma imagem de entrada a ser utilizada na geração da sub-imagem amostra.
  * @param imgSmpInImage Instância de imImage que passará a ser utilizada na geração da sub-imagem amostra.
  * @return Retorna -1 em caso de erro (imagem de entrada inexistente), ou 0 caso ok.
  */
  int imgSmpInImageSet( imImage* imgSmpInImage )
  {
    // Caso não exista imagem de entrada, retorna erro:
    if( !imgSmpInImage )
      return -1;

    // Caso contrário, insere a nova imagem e retorna ok:
    _imgSmpInImage = imgSmpInImage;
    return 0;
  }


  /**
  * Obtém um ponteiro para a imagem de entrada atualmente sendo utilizada na geração da sub-imagem amostra.
  * @param imgSmpInImage Instância de imImage a sendo atualmente utilizada (retornada).
  * @return Retorna -1 em caso de erro (imagem de entrada inexistente), ou 0 caso ok.
  */
  int imgSmpInImageGet( imImage** imgSmpInImage )
  {
    // Caso não exista imagem de entrada, retorna erro:
    if( !_imgSmpInImage )
      return -1;

    // Caso contrário, insere a nova imagem e retorna ok:
    *imgSmpInImage = _imgSmpInImage;
    return 0;
  }


  /**
  * Obtém a dimensão dos vetores de pixels da sub-imagem amostra.
  * @return Retorna a dimensão de cada um dos vetores de pixels (mesma para as três componentes) da sub-imagem amostra.
  * Caso esses vetores ainda não tenham sido alocados, retorna -1.
  */
  int imgSmpSubImageVectorSizeGet()
  {
    // Caso não exista imagem de entrada, retorna erro:
    if( !_imgSmpInImage )
      return -1;

    // Caso contrário, retorna a dimensão dos vetores de pixels da sub-imagem amostra:
    return _imgSmpSubImageVectorSize;
  }


  /**
  * Construtor. Recebe uma instância de imImage contendo a imagem de entrada (a partir da qual será gerada a sub-imagem amostra),
  * as dimensões da sub-imagem amostra (sub-imagem amostra é um "recorte" na imagem de entrada), a posição inicial da sub-imagem amostra
  * na imagem de entrada, além de uma string contendo o nome do arquivo a ser gerado caso se queira salvar a sub-imagem amostra
  * como uma nova imagem.
  * @param imgSmpInImage Instância da classe imImage contendo a imagem de entrada.
  * @param imgSmpSubWidth Dimensão X da sub-imagem amostra a ser criada.
  * @param imgSmpSubHeight Dimensão Y da sub-imagem amostra a ser criada.
  * @param imgSmpOrigBeginX Coordenada X do ponto inicial da sub-imagem amostra na imagem de entrada.
  * @param imgSmpOrigBeginY Coordenada Y do ponto inicial da sub-imagem amostra na imagem de entrada.
  * @param imgSmpSldWidth Dimensão X das amostras Sld a serem criadas a partir da sub-imagem amostra.
  * @param imgSmpSldHeight Dimensão Y das amostras Sld a serem criadas a partir da sub-imagem amostra.
  */
  ImageSample( imImage* imgSmpInImage, int imgSmpSubWidth, int imgSmpSubHeight, 
    int imgSmpOrigBeginX, int imgSmpOrigBeginY, int imgSmpSldWidth, int imgSmpSldHeight )
  {
    // Caso a instância da imagem de entrada não exista, retorna:
    if( !imgSmpInImage )
    {
      _imgSmpInImage      = NULL;
      _imgSmpInImageData  = NULL;
      _imgSmpImageDataR   = NULL;
      _imgSmpImageDataG   = NULL;
      _imgSmpImageDataB   = NULL;
      _imgSmpSubImageVectorSize = 0;      
      _imgSmpSldSamplesR   = NULL;
      _imgSmpSldSamplesG   = NULL;      
      _imgSmpSldSamplesB   = NULL;
      return;
    }

    // Vetores das componentes (RGB) dos pixels da sub-imagem amostra:
    _imgSmpImageDataR   = NULL;
    _imgSmpImageDataG   = NULL;
    _imgSmpImageDataB   = NULL;
    // Matrizes das componentes (RGB) dos pixels das imagens sld:
    _imgSmpSldSamplesR   = NULL;
    _imgSmpSldSamplesG   = NULL;      
    _imgSmpSldSamplesB   = NULL;

    // Caso contrário, insere os valores na estrutura:
    _imgSmpInImage    = imgSmpInImage;
    _imgSmpSubWidth      = imgSmpSubWidth;
    _imgSmpSubHeight     = imgSmpSubHeight;
    _imgSmpOrigBeginX = imgSmpOrigBeginX;
    _imgSmpOrigBeginY = imgSmpOrigBeginY;
    _imgSmpSldWidth = imgSmpSldWidth;
    _imgSmpSldHeight = imgSmpSldHeight;

    // Encontra o tamanho da sub-imagem amostra e aloca memória para o vetor:
    _imgSmpSubImageVectorSize = _imgSmpSubWidth * _imgSmpSubHeight;
    // Chamando a função que aloca os vetores das componentes dos pixels:
    imgSmpSubImageAlloc();

    // Encontrando o número de amostras Sld que serão criadas, levando em conta os 
    // parâmetros recebidos:
    _imgSmpSldImagesSize = (_imgSmpSubWidth-_imgSmpSldWidth+1)*(_imgSmpSubHeight-_imgSmpSldHeight+1);
    // Número de pixels em cada uma das imagens sld:
    _imgSmpSldEachVectorSize = _imgSmpSldWidth * _imgSmpSldHeight;
  }


  /**
  * Salva a sub-imagem amostra como uma nova imagem TGA RGB. Recebe uma string 
  * correspondendo ao nome do arquivo da sub-imagem amostra a ser salvo. 
  * @param filename string correspondendo ao nome do arquivo da sub-imagem amostra a ser salvo. 
  * @return Retorna -1 em caso de erro (strings de nome de arquivo inexistente, imagem de entrada 
  * inexistente, etc), ou 0 caso ok.
  */
  int imgSmpImageSmpSave( char* filename )
  {
    // Casos de retorno:
    if( (!filename) || (!_imgSmpInImage) || (_imgSmpSubImageVectorSize<=0) )
      return -1;

    // Caso contrário, cria a imImage a ser retornada:
    int error;

    imFile* ifile = imFileNew(filename, "TGA", &error);
    if (error != IM_ERR_NONE) 
      return -1;

    // Intanciando a nova imImage:
    imImage *thisImage = imImageCreate( _imgSmpSubWidth, _imgSmpSubHeight, IM_RGB, IM_BYTE);

    // Criando o vetor de pixels no formato apropriado:
    unsigned char *pixelsData = (unsigned char*)thisImage->data[0];

    // Copiando os dados para o novo vetor:
    int i=0;
    for( int j=0 ; j<_imgSmpSubWidth*_imgSmpSubHeight ; j++ )
      pixelsData[i*_imgSmpSubWidth*_imgSmpSubHeight+j] = _imgSmpImageDataR[j];
    i++;
    for( int j=0 ; j<_imgSmpSubImageVectorSize ; j++ )
      pixelsData[i*_imgSmpSubWidth*_imgSmpSubHeight+j] = _imgSmpImageDataG[j];
    i++;
    for( int j=0 ; j<_imgSmpSubImageVectorSize ; j++ )
      pixelsData[i*_imgSmpSubWidth*_imgSmpSubHeight+j] = _imgSmpImageDataB[j];

    error = imFileSaveImage(ifile, thisImage);
    if (error != IM_ERR_NONE) 
      return -1;

    imFileClose(ifile); 
    if (error != IM_ERR_NONE) 
      return -1;

    return 0;
  }


  /**
  * Salva uma das amostras sld como uma nova imagem TGA RGB. Recebe uma string 
  * correspondendo ao nome do arquivo da amostra sld a ser salvo. 
  * @param filename string correspondendo ao nome do arquivo da amostra sld a ser salvo. 
  * @return Retorna -1 em caso de erro (strings de nome de arquivo inexistente, imagem de entrada 
  * inexistente, etc), ou 0 caso ok.
  */
  int imgSmpImageSldSampleSave( char* filename, int sldSample )
  {
    // Casos de retorno:
    if( (!filename) || (!_imgSmpInImage) || (_imgSmpSldEachVectorSize<=0) )
      return -1;

    // Caso contrário, cria a imImage a ser retornada:
    int error;

    imFile* ifile = imFileNew(filename, "TGA", &error);
    if (error != IM_ERR_NONE) 
      return -1;

    // Intanciando a nova imImage:
    imImage *thisImage = imImageCreate( _imgSmpSldWidth, _imgSmpSldHeight, IM_RGB, IM_BYTE);

    // Criando o vetor de pixels no formato apropriado:
    unsigned char *pixelsData = (unsigned char*)thisImage->data[0];

    // Copiando os dados para o novo vetor:
    int i=0;
    for( int j=0 ; j<_imgSmpSldEachVectorSize ; j++ )
      pixelsData[i*_imgSmpSldWidth*_imgSmpSldHeight+j] = _imgSmpSldSamplesR[sldSample][j];
    i++;
    for( int j=0 ; j<_imgSmpSldEachVectorSize ; j++ )
      pixelsData[i*_imgSmpSldWidth*_imgSmpSldHeight+j] = _imgSmpSldSamplesG[sldSample][j];
    i++;
    for( int j=0 ; j<_imgSmpSldEachVectorSize ; j++ )
      pixelsData[i*_imgSmpSldWidth*_imgSmpSldHeight+j] = _imgSmpSldSamplesB[sldSample][j];

    error = imFileSaveImage(ifile, thisImage);
    if (error != IM_ERR_NONE) 
      return -1;

    imFileClose(ifile); 
    if (error != IM_ERR_NONE) 
      return -1;

    return 0;
  }


  /**
  * Função auxiliar utilizada no preenchimento de uma das amostras Sld.
  * Preenche uma única amostra Sld a partir dos parâmetros de entrada recebidos.
  * @param imgSmpSldBeginX Coordenada X de início dessa amostra Sld dentro da imagem de entrada.
  * @param imgSmpSldBeginY Coordenada Y de início dessa amostra Sld dentro da imagem de entrada.
  * @param imgSmpSldMatrixPos Posição dos pixels dessa amostra Sld deve ocupar dentro das matrizes.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int imgSmpSldSampleInsertPixels( int imgSmpSldBeginX, int imgSmpSldBeginY, int imgSmpSldMatrixPos )
  {
    // Casos de retorno:
    if( (_imgSmpSldWidth == -1) || (_imgSmpSldHeight == -1) || ( !_imgSmpInImage ) )
      return -1;

    // Copiando os dados para o novo vetor:
    int pos=0;    //< Vetor que indica a primeira posição vazia no vetor de pixels da sub-imagem amostra.
    for( int j=imgSmpSldBeginY ; j<imgSmpSldBeginY+_imgSmpSldHeight ; j++ )
      for( int i=imgSmpSldBeginX ; i<imgSmpSldBeginX+_imgSmpSldWidth ; i++ )
      {
        unsigned char retR, retG, retB;
        imgSmpInImageGetPixel( i, j, &retR, &retG, &retB );

        // Caso contrário, insere o pixel atual na sub-imagem amostra:
        _imgSmpSldSamplesR[imgSmpSldMatrixPos][pos] = retR;
        _imgSmpSldSamplesG[imgSmpSldMatrixPos][pos] = retG;
        _imgSmpSldSamplesB[imgSmpSldMatrixPos][pos] = retB;
        pos++;
      }
      return 0;
  }


  /**
  * Uma vez recebida a imagem de entrada e uma vez criada a sub-imagem amostra, cria as
  * imagens Sld a partir dos parâmetros estabelecidos.
  * @return Retorna -1 em caso de erro, ou 0 caso ok.
  */
  int imgSmpSldSamplesCreate()
  {
    // Casos de retorno:
    if( (_imgSmpSldWidth == -1) || (_imgSmpSldHeight == -1) || ( !_imgSmpInImage ) )
      return -1;

    // Caso contrário, aloca espaço para a matriz de imagens Sld caso necessário:
    imgSmpSldSamplesAlloc();

    // Uma vez alocadas, as matrizes de pixels devem ser preenchidas:
    int pos=0; 
    for( int i=_imgSmpOrigBeginX ; i<_imgSmpOrigBeginX+_imgSmpSubWidth-_imgSmpSldWidth ; i++ )
      for( int j=_imgSmpOrigBeginY ; j<_imgSmpOrigBeginY+_imgSmpSubHeight-_imgSmpSldHeight ; j++ )
      {
        imgSmpSldSampleInsertPixels( i, j, pos );
        pos++;
        if(pos == 199)
          return 0;     //< Resolver problemas de alocação.
      }
      return 0;
  }


  /**
  * Cria uma amostra Sld para ser utilizada externamente. Cria um vetor que irá conter as componentes
  * RGB de todos os pixels da amostra, desde o ponto Xmin,Ymin até o ponto Xmax, Ymax (inclusive).
  * @param imgSmpExtXmin
  * @param imgSmpExtYmin
  * @param imgSmpExtXmax
  * @param imgSmpExtYmax
  * @param imgSmpExtVector
  * @return Retorna -1 em caso de erro (imagem inexistente, corrdenadas inexistentes, etc), ou 0 caso ok.
  */
  int imgSmpExtSampleCreate( int imgSmpExtXmin, int imgSmpExtYmin, int imgSmpExtXmax, int imgSmpExtYmax, 
    float** imgSmpExtVector )
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
        imgSmpInImageGetPixel( i, j, &retR, &retG, &retB );
        imgSmpExtSample[pos] = (float)(retR/255);
        imgSmpExtSample[smpDimension+pos] = (float)(retG/255);
        imgSmpExtSample[2*smpDimension+pos] = (float)(retB/255);
      }
    }

    return 0;
  }



};


