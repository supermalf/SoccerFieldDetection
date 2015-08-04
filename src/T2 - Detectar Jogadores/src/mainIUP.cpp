/* *************************************************************
 *
 * Class:  mainIUP.c
 * Created: 12/4/2006 - 9:45
 * Author: malf
 * $Id: $
 *
 * Description: 
 * 
 *
 * $Log: $
 *
 * *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iup.h> 
#include <iupgl.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
//#include <assert.h>

#include <im.h>
#include <im_capture.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_format_avi.h>
#include <im_image.h>
#include <im_util.h>

#include <iup.h>
#include <iupcontrols.h>
#include <iupgl.h>

extern "C"
{
#include "geom.h"
#include "IconLib.h"
//#include "RedeNeural.h"
#include "GNG.h"
#include "Vector.h"
#include "co-occurrence.h"
#include "submenu.h"
}

#include "../Aurelio_src/BKP_Performance.h"
#include "Amostras.h"


#define MAX_MASKS 3
#define RESET_REPAINTCOUNTER 10000
#define IMG_W 720  //Assumi a imagem de tamanho fixo
#define IMG_H 486


//void IconLibOpen(void);

//////////////////////////////////////////////////////////////////////////
Ihandle *img_canvas;
Ihandle *mainDlg;   
Ihandle *label;
FILE *fpLog; 

GeomPoint2D ** quadSel;     //selecao quadrada  **Para aurelio
GeomPoint2D ** quadfreeSel; //selecao de um quad ao redor do freehand   **para aurelio
GeomPoint2D ** maskfreeSel; //mascara de bits selecionda
GeomPol2D * polfreeSel;    //poligono gerado pela selecao FREE


// Variáveis criadas para realizar a análise de sensibilidade:
ImageSamples* imageSamples = NULL;
BKP_Performance* bkp_Performance = NULL;
float** inSamples = NULL;
float** outSamples=NULL;
int inSamplesSize=0;
int outSamplesSize=0;
int numSamples=0;


//int ** paintSel;  //Matriz de selecao pintada
int *** paintSel2;  //Matriz de selecao pintada
int numMask = 0;  //0 a MAX_MASKS
int ** paintZoomSel;  //Matriz de selecao pintada (ZOOM)
int *neuralTopology;

BKPNeuralNet *testeNeuralNet;
NewBKPSensAnalysis *_prfSenAnalysis;  ///< Instancia da classe de analise se sensibilidade que analisa a rede.

float *bacalhau;
float *saida;





//GeomPoint2D ** freeSel;     //selecao freehand


//Tipos de selecao
enum{ 
  QUAD_SELECT=0,
  FREE_SELECT,
  PAINT_SELECT,
  ZOOM_SELECT,
  TEXTURE_SELECT,
  NO_SELECT
};
/*
enum{
  PAINT_COLOR_PLAYER=0,
  PAINT_COLOR_PLAYER2,
  PAINT_COLOR_FIELD,
  PAINT_COLOR_LINE,
  PAINT_COLOR_REFREE,
  PAINT_COLOR_KEEPER,
  PAINT_COLOR_KEEPER2,
  PAINT_COLOR_OTHER,
  PAINT_COLOR_NOTHING
}; 
*/

//Brush Type
enum{
  PAINT_BRUSH_ROUND=0,
  PAINT_BRUSH_SQUARE
};

//Selection box
enum{
  BOX_3x3=1,
  BOX_5x5,
  BOX_7x7,
  BOX_9x9,
  BOX_11x11,
  BOX_13x13,
  BOX_15x15,
  BOX_17x17,
  BOX_21x21,
  BOX_23x23
};

static int actualSelection = TEXTURE_SELECT;
int paintselSize = 0; 
int freeselSize = 0;              //qtdade de pontos da selecao
int freeEnd = 0;                  //Flag q avisa qndo a selecao free terminou
int mouseState = 0;
int mouseButton= IUP_BUTTON3;
int brushSize = 8;            //Grossura do pincel
int brushType = PAINT_BRUSH_ROUND;
float alpha = 0.5;  //nivel de transparencia
int paintState = PAINT_COLOR_PLAYER;   
int lastPaintState = PAINT_COLOR_PLAYER;  //para nao perder a informacao ao usar o botao da direita para apagar

int zoomFactor = 1; // Fator de zoom da imagem
int zoomX1 = 0;
int zoomX2 = IMG_W/2;
int zoomY1 = 0;
int zoomY2 = IMG_H/2;
int zoomIncX = 0;
int zoomIncY = 0;
int zoomSetX = 0;   //Posicao da tela sem zoom q se deseja fazer zoom
int zoomSetY = 0;
int canvasX = 0; //bug no wheel
int canvasY = 0;


 //Contador para fazer repaint, para resolver atualizacao contante


  unsigned char* data = NULL, * dataGrey = NULL;;
  int first = 1;            //Variavel q nao deixa atualizar o campo, 1-atualiza, 0-nao
int repaintCounter = RESET_REPAINTCOUNTER;  

  imImage* image;
  //imImage * image2;
  char* filename;  
  //imFile* ifile; 

//////////////////////////////////////////////////////////////////////////
//Aurelio

  /* Variáveis utilizadas para a execução do método de GNG: */
  ptGNG testeGNG;   //< Instância de teste.
  // Criando um vetor de vetores contendo cada um dos pixels da imagem:
  ptMVector ColorVector; 
  Amostras * amostras;



//////////////////////////////////////////////////////////////////////////
//TEXTURA

  //Co-occurrence params
  int d = 12;
  //int theta = 135;
  int boxSize = BOX_9x9;

float p1_energy=0.0f, p1_entropy=0.0f, p1_dissim=0.0f, p1_contrast=0.0f, p1_idm=0.0f, p1_variance=0.0f, p1_correlation=0.0f; 
float p2_energy=0.0f, p2_entropy=0.0f, p2_dissim=0.0f, p2_contrast=0.0f, p2_idm=0.0f, p2_variance=0.0f, p2_correlation=0.0f; 
float f_energy=0.0f, f_entropy=0.0f, f_dissim=0.0f, f_contrast=0.0f, f_idm=0.0f, f_variance=0.0f, f_correlation=0.0f; 
float l_energy=0.0f, l_entropy=0.0f, l_dissim=0.0f, l_contrast=0.0f, l_idm=0.0f, l_variance=0.0f, l_correlation=0.0f; 



/**
* Recebe um vetor de floats e retorna os n maiores elementos do vetor.
* @param 
* @param 
* @param 
* @return
*/
int FloatVectorMinValuesCut( float *inVector, int vectorSize, int desiredSize, bool *flagsVector, float **retVector, bool **retflagsVector )
{
  float *tmpretVector = new float[vectorSize];
  bool  *tmpflagsVector = new bool[vectorSize];

  for( int i=0 ; i<vectorSize ; i++ )
  {
    tmpretVector[i] = inVector[i];
    tmpflagsVector[i]  = false;
  }

  int num = 0;
  while(num < desiredSize)
  {
    float maior=(float)MIN_VALUE;
    int maiorPos=-1;

    for( int j=0 ; j<vectorSize ; j++ )
    {
      if( tmpretVector[j] > maior )
      {
        maior = tmpretVector[j];
        maiorPos = j;
      }
    }
    // Nesse ponto, maior é o maior valor do vetor, e maiorPos corresponde à sua posição:
    tmpflagsVector[maiorPos] = true;
    tmpretVector[maiorPos] = (float)MIN_VALUE;
    num++;
  }

  // Retornando o vetor de flags indicando quais são as entradas que ficam, além do vetor de pesos dessas entradas:
  int pos=0;
  for( int i=0 ; i<vectorSize ; i++ )
  {
    if(tmpflagsVector[i] == true )
    {
      tmpretVector[pos] = inVector[i];
      pos++;
    }
  }
  *retflagsVector = tmpflagsVector;
  *retVector = tmpretVector;

  return 0;
}



/************************************************************************
*                                                                     
************************************************************************/
void QuantizaGNG( Amostras* amostras, int nneurons, int nlength, 
                 int lambda, float eb, float en, float alfa, float d, int amax, float *weights) 
{
  // Cria a parte do GNG: */
  int x, y;
  int NumTreinos;
  ptMVector tmpVector;
  ptMVector ColorVector; 
  int Winner;
  int w = IMG_W;
  int h = IMG_H;
  int cont = 0;
  int retSize=0;

  //Pegando a máscara de entrada do malf:
  int ** masc = amostras->_masc;

  int *retInTypesVector;      //< Vetor contendo o tipo (PAINT_COLOR_PLAYER, PAINT_COLOR_FIELD, etc.) de cada uma das amostras retornadas.
  float **retInMatrix;        //< Matriz contendo as amostras de entradas de treino.
  int retInSamlesSize;        //< Dimensão de cada uma das amostras de entrada de treino retornadas.
  float **retOutMatrix;       //< Matriz contendo cada uma das correspondentes amostrasd de saída de treino.
  int retOutSamlesSize;       //< Dimensão de cada uma das amostras de saída de treino retornadas.
  int **retCoordsMatrix;
  int retNumSamples;          //< Número de amostras entrada/saída retornadas nas matrizes.

  // Obtendo matrizes contendo somente as amostras de treino:
  amostras->getTrainSamplesMatrix( &retInTypesVector, &retInMatrix, &retInSamlesSize, &retOutMatrix, &retOutSamlesSize, 
    &retCoordsMatrix, &retNumSamples );

  // Criando um vetor de vetores contendo cada um dos pixels da imagem:
  ColorVector = (ptMVector)malloc(retNumSamples*sizeof(MVector));
  for( int cont=0 ; cont<retNumSamples ; cont++ )
  {
    // Cria o vetor:
    ColorVector[cont].dim = nlength;
    ColorVector[cont].values = (float*)malloc(nlength*sizeof(float));
    for( int i=0 ; i<nlength ; i++ )
    {
      ColorVector[cont].values[i] = retInMatrix[cont][i];
    }
  }

  // Crio a instância do GNG para teste:
  CreateGNG( &testeGNG, "teste", nneurons, nlength, lambda, eb, en, alfa, d, amax, weights );

  // Treino até que os critérios de parada sejam atingidos:
  NumTreinos = 0;
  TrainGNG( testeGNG, ColorVector, retNumSamples-1, &NumTreinos);

  // Gerando a imagem de saída:
  // Após o treinamento, obtendo a imagem de saída:
  NewEmptyVector( nlength, &tmpVector );

  // Depois de executado o treinamento, é preciso classificar os neurônios:
  // Vetor de classificação dos neurônios:
  int *Classif = new int[nneurons];
  for( int i=0 ; i<nneurons ; i++ )
    Classif[i] = 0;

  // O VETOR DE TIPOS TÁ RETORNANDO NULO!!!!
  // Utilizando cada uma das amostras na classificação:
  for( int cont=0 ; cont<retNumSamples ; cont++ )
  {
    // Para cada um dos pixels, encontra seu representante e 
    // insere o representante no lugar do pixel vencedor:
    // Encontra o vencedor:
    GetNearest( testeGNG, &ColorVector[cont], &Winner );
    if(retInTypesVector[cont] == PAINT_COLOR_PLAYER)
      Classif[Winner] ++;
    if(retInTypesVector[cont] == PAINT_COLOR_FIELD)
      Classif[Winner] --;
  }


  int *retInTypesVector2;      //< Vetor contendo o tipo (PAINT_COLOR_PLAYER, PAINT_COLOR_FIELD, etc.) de cada uma das amostras retornadas.
  float **retInMatrix2;        //< Matriz contendo as amostras de entradas de treino.
  int retInSamlesSize2;        //< Dimensão de cada uma das amostras de entrada de treino retornadas.
  float **retOutMatrix2;       //< Matriz contendo cada uma das correspondentes amostrasd de saída de treino.
  int retOutSamlesSize2;       //< Dimensão de cada uma das amostras de saída de treino retornadas.
  int **retCoordsMatrix2;
  int retNumSamples2;          //< Número de amostras entrada/saída retornadas nas matrizes.


  // Obtendo uma lista com todas as amostras, bem como sua posição:
  amostras->getSamplesMatrix( &retInMatrix2, &retInSamlesSize2, &retOutMatrix2, &retOutSamlesSize2, &retCoordsMatrix2,
    &retNumSamples2);

  int maiorX=0, maiorY=0;
  // Utilizando cada uma das amostras na classificação:
  for( int cont=0 ; cont<retNumSamples2 ; cont++ )
  {
    // Para cada um dos pixels, encontra seu representante e 
    // insere o representante no lugar do pixel vencedor:
    // Encontra o vencedor:
    for( int i=0 ; i<nlength ; i++ )
    {
      ColorVector[0].values[i] = retInMatrix2[cont][i];
    }

    GetNearest( testeGNG, &ColorVector[0], &Winner );
    y = retCoordsMatrix2[cont][0];
    x = retCoordsMatrix2[cont][1];
    if( maiorY < y )
      maiorY = y;
    if( maiorX < x )
      maiorX = x;

    if(Classif[Winner] > 30)
      amostras->_masc[y][x] =  PAINT_COLOR_PLAYER;
    else if(Classif[Winner] < 20)
      amostras->_masc[y][x] =  PAINT_COLOR_FIELD;

  }


  // Liberando a memória previamente alocada:
  DeleteGNG( testeGNG );
}


/************************************************************************
 * update_dialog_size                                                                    
 ************************************************************************/
static void update_dialog_size(void)
{
  int w,h;
  char buffer[512];
  //w=imgGetWidth(image1);
  //h=imgGetHeight(image1);
  w = IMG_W;
  h = IMG_H;
  sprintf(buffer,"%dx%d",w,h);
  IupSetAttribute(img_canvas, IUP_RASTERSIZE, buffer);
  IupSetAttribute(mainDlg, IUP_RASTERSIZE, NULL);
  IupShowXY(mainDlg, IUP_CENTER, IUP_CENTER);
   
}

//////////////////////////////////////////////////////////////////////////
//Callbacks do IUP
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 *	resize_cb
 ************************************************************************/
int resize_cb(Ihandle *self, int width, int height)
{
 IupGLMakeCurrent(self);

 glViewport(0,0,width,height);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity(); 

 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluOrtho2D (0.0, (GLsizei)(width), 0.0, (GLsizei)(height)); 
 
 first = 1;

 return IUP_DEFAULT;
}

/************************************************************************
 *	repaint_cb
 ************************************************************************/
int repaint_cb(Ihandle *self, int setCounter)
{
  int i,j;
  repaintCounter = setCounter;
  repaintCounter ++;


 if(first && (repaintCounter>=1000))
 {
   repaintCounter = 0;
   IupGLMakeCurrent(self);  
   glClearColor(0.2f, 0.2f, 0.2f, 0);
   glClear(GL_COLOR_BUFFER_BIT);

 }

  //Desenhar imagem
  //if(data != NULL)
  {
    //RasterPos2d(0.0,0.0); 

    if(first && (data != NULL))
    {
      
      
      glRasterPos2d(0.0,0.0);
      //glDrawPixels (IMG_W-1, IMG_H-1, GL_RGB, GL_UNSIGNED_BYTE, dataGrey);
      glDrawPixels (IMG_W, IMG_H, GL_RGB, GL_UNSIGNED_BYTE, dataGrey);

      if(zoomFactor > 1)
      {
      
      glPixelZoom ((GLfloat)zoomFactor, (GLfloat)zoomFactor);
      
       glCopyPixels (zoomX1, zoomY1, zoomX2, zoomY2, GL_COLOR);
      glPixelZoom (1.0, 1.0);
      glFlush ();
      }
        
        //glCopyPixels (IMG_W/2, 0, IMG_W, IMG_H/2, GL_COLOR);
        //glPixelZoom (1.0, 1.0);



        //glPixelZoom ( zoomFactor, zoomFactor); // amplia o pixel pelo fator especificado

 

        /*
      else
      {
        if (firstZoom)
        {
          glDrawPixels (IMG_W-1, IMG_H-1, GL_RGB, GL_UNSIGNED_BYTE, data);
          firstZoom = 0;
        }
        glPixelZoom (2, 2); 
        glCopyPixels (IMG_W/2, 0, IMG_W, IMG_H/2, GL_COLOR);
        glPixelZoom (1.0, 1.0);
      }
      */
    }
    
    glFlush();
  }
  
  if(actualSelection == QUAD_SELECT)
  {
    glColor3f(0.0f,1.0f,1.0f);
    glBegin(GL_LINES);
     
      glVertex2d(quadSel[0]->x, quadSel[0]->y);
      glVertex2d(quadSel[1]->x, quadSel[1]->y);

      glVertex2d(quadSel[1]->x, quadSel[1]->y);
      glVertex2d(quadSel[2]->x, quadSel[2]->y);

      glVertex2d(quadSel[2]->x, quadSel[2]->y);
      glVertex2d(quadSel[3]->x, quadSel[3]->y);

      glVertex2d(quadSel[3]->x, quadSel[3]->y);
      glVertex2d(quadSel[0]->x, quadSel[0]->y);
    glEnd();
    glFlush();
  }
  else if (actualSelection == FREE_SELECT)
  {
    glColor3f(0.0f,1.0f,1.0f);
    glBegin(GL_LINE_STRIP);
      for(i=0; i<freeselSize; i++)
        glVertex2d(polfreeSel->pol[i].x, polfreeSel->pol[i].y);
        
    glEnd();
    
    if((freeselSize>0) && freeEnd)
    {
      //caixa de selecao
      glColor3f(1.0f,1.0f,0.0f);
      glBegin(GL_LINES);
        glVertex2d(quadfreeSel[0]->x, quadfreeSel[0]->y);
        glVertex2d(quadfreeSel[1]->x, quadfreeSel[1]->y);

        glVertex2d(quadfreeSel[1]->x, quadfreeSel[1]->y);
        glVertex2d(quadfreeSel[2]->x, quadfreeSel[2]->y);

        glVertex2d(quadfreeSel[2]->x, quadfreeSel[2]->y);
        glVertex2d(quadfreeSel[3]->x, quadfreeSel[3]->y);

        glVertex2d(quadfreeSel[3]->x, quadfreeSel[3]->y);
        glVertex2d(quadfreeSel[0]->x, quadfreeSel[0]->y);
      glEnd();
    }

    //Debug - Pintando area de selecao
    if (freeEnd)
    {
      int sizeY, sizeX;

      glBegin(GL_POINTS);


      sizeX = (int)(quadfreeSel[2]->x - quadfreeSel[0]->x + 1);
      sizeY = (int)(quadfreeSel[2]->y - quadfreeSel[0]->y + 1);

      for (i=0; i<sizeY ;i++){
        for (j=0; j<sizeX ;j++)
        {
          if (maskfreeSel[i][j].mask)
            glColor3f(0.0f,1.0f,1.0f);
          else
            glColor3f(1.0f,1.0f,0.0f);

          glVertex2i(j + (int)quadfreeSel[0]->x, i + (int)quadfreeSel[0]->y);
        }
      }
      glEnd();
    }

    glFlush();
  }
  else if ((actualSelection == PAINT_SELECT) || (actualSelection == TEXTURE_SELECT))
  {
    //glLineWidth(1);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel (GL_FLAT);

    glBegin(GL_POINTS);
    for(j=0; j<IMG_H; j++)
      for(i=0; i<IMG_W; i++)
      {
        if(zoomFactor == 1)
        {
          if(paintSel2[numMask][j][i] == PAINT_COLOR_FIELD)
          {
            glColor4f(0.0f,0.0f,0.0f,alpha);
            glVertex2d(i, j);
          }
          else if(paintSel2[numMask][j][i] == PAINT_COLOR_PLAYER)
          {
            glColor4f(1.0f,1.0f,1.0f,alpha);
            glVertex2d(i, j);
          }
          else if(paintSel2[numMask][j][i] == PAINT_COLOR_LINE)
          {
            glColor4f(1.0f,0.0f,0.0f,alpha);
            glVertex2d(i, j);
          }
          else if(paintSel2[numMask][j][i] == PAINT_COLOR_PLAYER2)
          {
            glColor4f(0.0f,1.0f,0.0f,alpha);
            glVertex2d(i, j);
          }
          else if(paintSel2[numMask][j][i] == PAINT_COLOR_KEEPER)
          {
            glColor4f(0.0f,0.0f,1.0f,alpha);
            glVertex2d(i, j);
          }
          else if(paintSel2[numMask][j][i] == PAINT_COLOR_REFREE)
          {
            glColor4f(1.0f,1.0f,0.0f,alpha);
            glVertex2d(i, j);
          }
          else if(paintSel2[numMask][j][i] == PAINT_COLOR_OTHER)
          {
            glColor4f(0.0f,1.0f,1.0f,alpha);
            glVertex2d(i, j);
          }
          else if(paintSel2[numMask][j][i] == PAINT_COLOR_KEEPER2)
          {
            glColor4f(1.0f,0.0f,1.0f,alpha);
            glVertex2d(i, j);
          }

          /*else
          {
            glColor4f(0.0f,0.0f,0.0f,alpha);
            glVertex2d(i, j);
          }*/
        }
        else //Zoom Factor > 1 
        {
          if(paintZoomSel[j][i] == PAINT_COLOR_FIELD)
          {
            glColor4f(0.0f,0.0f,0.0f,alpha);
            glVertex2d(i, j);
          }
          else if(paintZoomSel[j][i] == PAINT_COLOR_PLAYER)
          {
            glColor4f(1.0f,1.0f,1.0f,alpha);
            glVertex2d(i, j);
          }
          /*else
          {
            glColor4f(1.0f,0.0f,0.0f,alpha);
            glVertex2d(i, j);
          }*/
        }
      }
    glEnd();
    glFlush();
    glDisable (GL_BLEND);
  }
  
  else if (actualSelection == NO_SELECT)
  {
    first=1;
    repaintCounter = RESET_REPAINTCOUNTER;
  } 

  if(actualSelection == TEXTURE_SELECT) //TODO: definir quad diferente para textura?
  {
    glColor3f(0.0f,1.0f,1.0f);
    glBegin(GL_LINES);
    glVertex2d(quadSel[0]->x, quadSel[0]->y);
    glVertex2d(quadSel[1]->x, quadSel[1]->y);

    glVertex2d(quadSel[1]->x, quadSel[1]->y);
    glVertex2d(quadSel[2]->x, quadSel[2]->y);

    glVertex2d(quadSel[2]->x, quadSel[2]->y);
    glVertex2d(quadSel[3]->x, quadSel[3]->y);

    glVertex2d(quadSel[3]->x, quadSel[3]->y);
    glVertex2d(quadSel[0]->x, quadSel[0]->y);
    glEnd();

    glColor3f(1.0f,0.0f,0.0f);
    glBegin(GL_POINTS);
    glVertex2d(((quadSel[0]->x + quadSel[2]->x)/2.0),   ((quadSel[0]->y + quadSel[2]->y)/2.0));
    glVertex2d(((quadSel[0]->x + quadSel[2]->x)/2.0)+1, ((quadSel[0]->y + quadSel[2]->y)/2.0));
    glVertex2d(((quadSel[0]->x + quadSel[2]->x)/2.0)-1, ((quadSel[0]->y + quadSel[2]->y)/2.0));
    glVertex2d(((quadSel[0]->x + quadSel[2]->x)/2.0),   ((quadSel[0]->y + quadSel[2]->y)/2.0)+1);
    glVertex2d(((quadSel[0]->x + quadSel[2]->x)/2.0),   ((quadSel[0]->y + quadSel[2]->y)/2.0)-1);
    glEnd();

    glFlush();
  }
  return IUP_DEFAULT;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
int paint_ClearPoints_cb(Ihandle *self)
{
  int i,j;

  for(j=0; j<IMG_H; j++)
    for(i=0; i<IMG_W; i++)
      paintSel2[numMask][j][i] = PAINT_COLOR_NOTHING;

  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "All paint selections cleared.");

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
 * zoom_cb                                                                    
 ************************************************************************/
int zoom_cb(Ihandle *self)
{
  actualSelection = ZOOM_SELECT;
  IupSetfAttribute(label, "TITLE", "Left Click: Zoom+ <> Right Click: Zoom-");
  return IUP_DEFAULT;
}

/************************************************************************
*  zoom_plus                                                                   
************************************************************************/
void zoom_plus()
{
  int i,j,ki=0,kj=0;

  if (zoomFactor == 1)
  {
    zoomFactor = 2;

    //////////////////////////////////////////////////////////////////////////
    //Definindo area de zoom
    zoomSetX = canvasX;
    zoomSetY = IMG_H - canvasY;

    zoomX1 = zoomSetX - (IMG_W/4);
    if(zoomX1 <= 0)
    {
      zoomX1 = 0;
      zoomX2 = IMG_W/2;
    }
    else
    {
      zoomX2 = zoomSetX + (IMG_W/4);
      if(zoomX2 >= IMG_W)
      {
        zoomX1 = IMG_W/2;
        zoomX2 = IMG_W;
      }
    }

    zoomY1 = zoomSetY - (IMG_H/4);
    if(zoomY1 <= 0)
    {
      zoomY1 = 0;
      zoomY2 = IMG_H/2;
    }
    else
    {
      zoomY2 = zoomSetY + (IMG_H/4);
      if(zoomY2 >= IMG_H)
      {
        zoomY1 = IMG_H/2;
        zoomY2 = IMG_H;
      }
    }

    //////////////////////////////////////////////////////////////////////////
    //Copiando selecao para o zoom

    //zoom+
    for(j=zoomY1; j<zoomY2; j+=1)
    {
      for(i=zoomX1; i<zoomX2; i+=1)
      {
        if(paintSel2[numMask][j][i] == PAINT_COLOR_PLAYER)
        {
          paintZoomSel[kj][ki] = PAINT_COLOR_PLAYER;
          paintZoomSel[kj][ki+1] = PAINT_COLOR_PLAYER;
          paintZoomSel[kj+1][ki] = PAINT_COLOR_PLAYER; 
          paintZoomSel[kj+1][ki+1] = PAINT_COLOR_PLAYER;
        }
        else if(paintSel2[numMask][j][i] == PAINT_COLOR_FIELD)
        {
          paintZoomSel[kj][ki] = PAINT_COLOR_FIELD;
          paintZoomSel[kj][ki+1] = PAINT_COLOR_FIELD;
          paintZoomSel[kj+1][ki] = PAINT_COLOR_FIELD; 
          paintZoomSel[kj+1][ki+1] = PAINT_COLOR_FIELD;
        }
        else if(paintSel2[numMask][j][i] == PAINT_COLOR_NOTHING)
        {
          paintZoomSel[kj][ki] = PAINT_COLOR_NOTHING;
          paintZoomSel[kj][ki+1] = PAINT_COLOR_NOTHING;
          paintZoomSel[kj+1][ki] = PAINT_COLOR_NOTHING; 
          paintZoomSel[kj+1][ki+1] = PAINT_COLOR_NOTHING;
        }
        ki+=2;
      }
      kj+=2;
      ki=0;
    }
  }
  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
}

/************************************************************************
*  zoom_minus                                                                   
************************************************************************/
void zoom_minus()
{
  int i,j,ki=0,kj=0;

  if (zoomFactor > 1)
  {
    zoomFactor = 1; 

    for(j=zoomY1; j<zoomY2; j+=2)
    {
      for(i=zoomX1; i<zoomX2; i+=2)
      {
        if((i >= IMG_W)||(i+1 >= IMG_W))
          break;

        if((j >= IMG_H)||(j+1 >= IMG_H))
          break;

        if((paintZoomSel[j][i] == PAINT_COLOR_PLAYER) &&
          (paintZoomSel[j][i+1] == PAINT_COLOR_PLAYER) &&
          (paintZoomSel[j+1][i] == PAINT_COLOR_PLAYER) &&
          (paintZoomSel[j+1][i+1] == PAINT_COLOR_PLAYER))
          paintSel2[numMask][zoomY1 + (j/2)][zoomX1 + (i/2)] = PAINT_COLOR_PLAYER;

        if( (paintZoomSel[j][i] == PAINT_COLOR_FIELD) &&
          (paintZoomSel[j][i+1] == PAINT_COLOR_FIELD) &&
          (paintZoomSel[j+1][i] == PAINT_COLOR_FIELD) &&
          (paintZoomSel[j+1][i+1] == PAINT_COLOR_FIELD))
          paintSel2[numMask][zoomY1 + (j/2)][zoomX1 + (i/2)] = PAINT_COLOR_FIELD;


        if( (paintZoomSel[j][i] == PAINT_COLOR_NOTHING) &&
          (paintZoomSel[j][i+1] == PAINT_COLOR_NOTHING) &&
          (paintZoomSel[j+1][i] == PAINT_COLOR_NOTHING) &&
          (paintZoomSel[j+1][i+1] == PAINT_COLOR_NOTHING))
          paintSel2[numMask][zoomY1 + (j/2)][zoomX1 + (i/2)] = PAINT_COLOR_NOTHING;
      }
    }
  }
  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
}



/************************************************************************
 *                                                                     
 ************************************************************************/
int paint_Player_cb(Ihandle *self)
{
  paintState = PAINT_COLOR_PLAYER;
  lastPaintState = PAINT_COLOR_PLAYER;
  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "Paint a player (team 1).");
  
  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
*                                                                     
************************************************************************/
int paint_Player2_cb(Ihandle *self)
{
  paintState = PAINT_COLOR_PLAYER2;
  lastPaintState = PAINT_COLOR_PLAYER2;
  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "Paint a player (team 2).");

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
*                                                                     
************************************************************************/
int paint_Keeper_cb(Ihandle *self)
{
  paintState = PAINT_COLOR_KEEPER;
  lastPaintState = PAINT_COLOR_KEEPER;
  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "Paint a goalkeeper (team 1).");

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
*                                                                     
************************************************************************/
int paint_Keeper2_cb(Ihandle *self)
{
  paintState = PAINT_COLOR_KEEPER2;
  lastPaintState = PAINT_COLOR_KEEPER2;
  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "Paint a goalkeeper (team 2).");

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
*                                                                     
************************************************************************/
int paint_Refree_cb(Ihandle *self)
{
  paintState = PAINT_COLOR_REFREE;
  lastPaintState = PAINT_COLOR_REFREE;
  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "Paint the refree.");

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
*                                                                     
************************************************************************/
int paint_Line_cb(Ihandle *self)
{
  paintState = PAINT_COLOR_LINE;
  lastPaintState = PAINT_COLOR_LINE;
  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "Paint the line.");

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}
/************************************************************************
*                                                                     
************************************************************************/
int paint_nextMask_cb(Ihandle *self)
{
  numMask++;

  if (numMask > MAX_MASKS-1)
    numMask = MAX_MASKS-1;

  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "Mask num #%d selected.",numMask);

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
*                                                                     
************************************************************************/
int paint_prevMask_cb(Ihandle *self)
{
  numMask--;

  if (numMask < 0)
    numMask = 0;

  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "Mask num #%d selected.",numMask);

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
int paint_Field_cb(Ihandle *self)
{
  paintState = PAINT_COLOR_FIELD;
  lastPaintState = PAINT_COLOR_FIELD;
  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "Paint the field.");
  
  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
int paint_RoundBrush_cb(Ihandle *self)
{
  brushType = PAINT_BRUSH_ROUND;
  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "Round brush selected.");
  
  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
int paint_SquareBrush_cb(Ihandle *self)
{
  brushType = PAINT_BRUSH_SQUARE;
  actualSelection = PAINT_SELECT;
  IupSetfAttribute(label, "TITLE", "Square brush selected.");
  
  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
* paint_invert_cb                                                                     
************************************************************************/
int paint_invert_cb()
{
  int i,j;

  for (j=0; j<IMG_H; j++){
    for (i=0; i<IMG_W; i++)
    {
      if (paintSel2[numMask][j][i] == PAINT_COLOR_PLAYER)
        paintSel2[numMask][j][i] = PAINT_COLOR_FIELD;
      else if  (paintSel2[numMask][j][i] == PAINT_COLOR_FIELD)
        paintSel2[numMask][j][i] = PAINT_COLOR_PLAYER;
    }
  }
  IupSetfAttribute(label, "TITLE", "Paint Selection Inverted.");
  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}

/************************************************************************
 *	savefile_cb
 ************************************************************************/
int savefile_cb(Ihandle *self)
{
  /*
  int error;
  imFile* ifileSave;

  ifileSave = imFileNew("out.tga", "TGA", &error);
  image = imFileLoadImage(ifile, 0, &error);

  imFileWriteImageInfo(ifile, image->width, image->height, image->color_space, image->data_type);
  imFileWriteImageData(ifile, image->data);
*/
  char format[10], compression[10];
  int error, image_count, i;
  int width, height, color_mode, data_type;
  //void* data;
  imImage* image; 
  imImage* image2; 
  imFile* ifile2;

  // Abrindo a imagem de entrada:
  imFile* ifile = imFileOpen("Canto dir1.tga", &error);

  // Obtendo informações a respeito do formato do arquivo de entrada:
  imFileGetInfo(ifile, format, compression, &image_count);
  for (i = 0; i < image_count ; i++)
  {
    error = imFileReadImageInfo(ifile, i, &width, &height, &color_mode, &data_type);
    if (error != IM_ERR_NONE) 
      error++;
  }

  // A partir do arquivo de entrada obtido, carregando uma estrutura imImage:
  image = imFileLoadBitmap(ifile, 0, &error);
  image2 = imFileLoadBitmap(ifile, 0, &error);

  ifile2 = imFileNew("Canto dir1 NOVO.tga", "TGA", &error);

  //imProcessReduceBy4(image, image2); //zoom out 4X
  imProcessCrop(image, image2, 100, 100); //zoom out 4X

  error = imFileSaveImage(ifile2, image2);

  imFileClose(ifile2);
  imFileClose(ifile);

  IupSetfAttribute(label, "TITLE", "File saved.");

  return IUP_DEFAULT;
}

/************************************************************************
 *	load_cb
 ************************************************************************/
int load_cb(void) 
{
  return IUP_DEFAULT;
}

/************************************************************************
 *	about_cb
 ************************************************************************/
int about_cb(Ihandle *self)
{
  IupMessagef("About", "Trabalho 2 de FCG\n"
    " ------------------------------------------------------ \n"
    "Authors:\n"
    "Aurelio Morais Figueredo  - aurelio@tecgraf.puc-rio.br\n"
    "Mauricio A. Lage Ferreira - malf@tecgraf.puc-rio.br\n"
    " ------------------------------------------------------ \n"
    "IUP Version %s\n"
    "IM Version %s\n"
    " ------------------------------------------------------ \n"
    "OpenGL:\n"
    "  Vendor: %s\n"
    "  Renderer: %s\n"
    "  Version: %s\n"
    , IUP_RELEASE_VERSION, IM_VERSION, 
    glGetString(GL_VENDOR),glGetString(GL_RENDERER),glGetString(GL_VERSION));

  return IUP_DEFAULT;
}

/************************************************************************
 *	exit_cb
 ************************************************************************/
int exit_cb(void)
{
  return IUP_CLOSE;
}


/************************************************************************
*	
************************************************************************/
int tex_Define_cb( float* energy, float* entropy, float* dissim, float* contrast,
                   float* idm, float* variance, float* correlation)
{

  int t=0;
  int counter=0;
  int ng, i, j;
  int initW, endW, initH, endH;
  int w0, w1, h0, h1, selW, selH;
  int ** C0;
  float ** P0;

  unsigned char * greyTable;

  //Criando tabela de cores usadas na selecao da imagem
  greyTable = (unsigned char *) malloc (256 * sizeof(unsigned char));

  actualSelection = TEXTURE_SELECT;
  //printf("=====================\n=====================\n");
  //printf("D=%d - Theta=%d - Box:%dx%d\n",d,theta,(boxSize*2)+1,(boxSize*2)+1);

  //ng = cooc_boxImgGrey(data,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);

  C0 = (int**)calloc (256,sizeof(int));
  for (i=0; i<256; i++)
    C0[i] = (int*) calloc (256,sizeof(int));

  P0 = (float**)calloc (256,sizeof(float));
  for (i=0; i<256; i++)
    P0[i] = (float*) calloc (256,sizeof(float));

  /*
  //////////////////////////////////////////////////////////////////////////
  //Calculando feature para a selecao de campo (Total)
  w0 = (int)quadSel[0]->x;
  w1 = (int)quadSel[2]->x;
  h0 = (int)quadSel[0]->y;
  h1 = (int)quadSel[2]->y;

  selW = (int)fabs((float)(w1 - w0));
  selH = (int)fabs((float)(h1 - h0));

  ng = cooc_boxImgGrey(data,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);
  cooc_boxBuildMatrix(C0,ng,d,theta,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);
  cooc_normalizeMatrix(C0,P0,ng,theta,selW,selH);
  //class1_feature = cooc_energy(P0,ng);

  printf("-Energy:       %lf\n",cooc_energy(P0,ng));
  printf("-Entropy:      %lf\n",cooc_entropy(P0,ng));
  printf("-Dssimilarity: %lf\n",cooc_dissimilarity(P0,ng));
  printf("-Contrast:     %lf\n",cooc_contrast(P0,ng));
  printf("-IDM:          %lf\n",cooc_IDM(P0,ng));
  printf("-Variance:     %lf\n",cooc_variance(P0,ng));
  printf("-Correlation:  %lf\n",cooc_correlation(P0,ng));
  printf("====================================================\n");
*/

  //////////////////////////////////////////////////////////////////////////
  //Varrendo a selecao para encontrar a media do feature
  //Nao tem problema de bordas

  //quadSel[0]->x = 100;
  //quadSel[2]->x = 200;
  //quadSel[0]->y = 100;
  //quadSel[2]->y = 200;
  //quadSel[1]->x = quadSel[2]->x;
  //quadSel[1]->y = quadSel[0]->y;
  //quadSel[3]->x = quadSel[0]->x;
  //quadSel[3]->y = quadSel[2]->y;
  //repaint_cb(img_canvas, RESET_REPAINTCOUNTER);


  //quadSel[2]->x = quadSel[0]->x + 7;
  //quadSel[2]->y = quadSel[0]->y + 7;
  //quadSel[1]->x = quadSel[2]->x;
  //quadSel[1]->y = quadSel[0]->y;
  //quadSel[3]->x = quadSel[0]->x;
  //quadSel[3]->y = quadSel[2]->y;
  //repaint_cb(img_canvas, RESET_REPAINTCOUNTER);


  initW = (int)quadSel[0]->x;
  endW  = (int)quadSel[2]->x;
  initH = (int)quadSel[0]->y;
  endH  = (int)quadSel[2]->y;

  cooc_boxOrderVertex(&initW,&endW,&initH,&endH);

  counter = 0; 
  i=initW+boxSize;
  j=initH+boxSize;

  *energy = 0.0f;
  *entropy =  0.0f;
  *dissim =  0.0f;
  *contrast =  0.0f;
  *idm = 0.0f;
  *variance = 0.0f;
  *correlation = 0.0f;

  for(t=0; t<= 135; t+= 45)
  {
    for(j=initH+boxSize; j<endH; j+=(2*boxSize)+1)
    {
      for(i=initW+boxSize; i<endW; i+=(2*boxSize)+1)
      {
        w0 = i - boxSize;
        w1 = i + boxSize;
        h0 = j - boxSize;
        h1 = j + boxSize;

        selW = (int)fabs((float)(w1 - w0));
        selH = (int)fabs((float)(h1 - h0));

        ng = cooc_boxImgGrey(data,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);
        cooc_boxBuildMatrix(C0,ng,d,t,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);
        cooc_normalizeMatrix(C0,P0,ng,t,selW,selH);

        *energy += cooc_energy(P0,ng);
        *entropy += cooc_entropy(P0,ng);
        *dissim += cooc_dissimilarity(P0,ng);
        *contrast += cooc_contrast(P0,ng);
        *idm += cooc_IDM(P0,ng);
        *variance += cooc_variance(P0,ng);
        *correlation += cooc_correlation(P0,ng);
        counter++;

        //printf("%f ", class1_feature);
      }
    }  
    if(counter==0)
      counter = 1;

    *energy /= (float) counter;
    *entropy /= (float) counter;
    *dissim /= (float) counter;
    *contrast /= (float) counter;
    *idm /= (float) counter;
    *variance /= (float) counter;
    *correlation /= (float) counter;
    counter=0;
  }

  *energy /= 4;
  *entropy /= 4;
  *dissim /= 4;
  *contrast /= 4;
  *idm /= 4;
  *variance /= 4;
  *correlation /= 4;

  //printf("-(M)Energy:       %lf\n",*energy);
  //printf("-(M)Entropy:      %lf\n",*entropy);
  //printf("-(M)Dssimilarity: %lf\n",*dissim);
  //printf("-(M)Contrast:     %lf\n",*contrast);
  //printf("-(M)IDM:          %lf\n",*idm);
  //printf("-(M)Variance:     %lf\n",*variance);
  //printf("-(M)Correlation:  %lf\n",*correlation);

  return IUP_DEFAULT;
}

/************************************************************************
*	
************************************************************************/
int tex_Line_cb(void)
{
  tex_Define_cb( &l_energy,&l_entropy,&l_dissim,&l_contrast,&l_idm, &l_variance, &l_correlation);
  return IUP_DEFAULT;
}


/************************************************************************
*	
************************************************************************/
int tex_Player2_cb(void)
{
  tex_Define_cb( &p2_energy,&p2_entropy,&p2_dissim,&p2_contrast,&p2_idm, &p2_variance, &p2_correlation);
  return IUP_DEFAULT;
}

/************************************************************************
*	
************************************************************************/
int tex_Field_cb(void)
{
  tex_Define_cb( &f_energy,&f_entropy,&f_dissim,&f_contrast,&f_idm, &f_variance, &f_correlation);
  return IUP_DEFAULT;
}

/************************************************************************
*	
************************************************************************/
int tex_Player_cb(void)
{
  tex_Define_cb( &p1_energy,&p1_entropy,&p1_dissim,&p1_contrast,&p1_idm, &p1_variance, &p1_correlation);
  return IUP_DEFAULT;
}
/************************************************************************
 *                                                                     
 ************************************************************************/
int tex_Inspect_cb(void)
{
  int counter=0;
  int ng, i, j;
  int initW, endW, initH, endH;
  int w0, w1, h0, h1, selW, selH;
  int ** C0;
  float ** P0;
  float class1_feature=0.0f, class2_feature=0.0f, class3_feature=0.0f,class4_feature=0.0f,class5_feature=0.0f;
  float class6_feature=0.0f,class7_feature=0.0f;

  int theta = 0;

unsigned char * greyTable;


  //Criando tabela de cores usadas na selecao da imagem
  greyTable = (unsigned char *) malloc (256 * sizeof(unsigned char));

  printf("=====================\n=====================\n");
  printf("D=%d - Theta=%d - Box:%dx%d\n",d,theta,(boxSize*2)+1,(boxSize*2)+1);

  //ng = cooc_boxImgGrey(data,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);

  C0 = (int**)calloc (256,sizeof(int));
  for (i=0; i<256; i++)
    C0[i] = (int*) calloc (256,sizeof(int));

  P0 = (float**)calloc (256,sizeof(float));
  for (i=0; i<256; i++)
    P0[i] = (float*) calloc (256,sizeof(float));

  //////////////////////////////////////////////////////////////////////////
  //Calculando feature para a selecao de campo (Total)
  w0 = (int)quadSel[0]->x;
  w1 = (int)quadSel[2]->x;
  h0 = (int)quadSel[0]->y;
  h1 = (int)quadSel[2]->y;

  selW = (int)fabs((float)(w1 - w0));
  selH = (int)fabs((float)(h1 - h0));

  ng = cooc_boxImgGrey(data,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);
  cooc_boxBuildMatrix(C0,ng,d,theta,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);
  cooc_normalizeMatrix(C0,P0,ng,theta,selW,selH);
  //class1_feature = cooc_energy(P0,ng);

  printf("-Energy:       %lf\n",cooc_energy(P0,ng));
  printf("-Entropy:      %lf\n",cooc_entropy(P0,ng));
  printf("-Dssimilarity: %lf\n",cooc_dissimilarity(P0,ng));
  printf("-Contrast:     %lf\n",cooc_contrast(P0,ng));
  printf("-IDM:          %lf\n",cooc_IDM(P0,ng));
  printf("-Variance:     %lf\n",cooc_variance(P0,ng));
  printf("-Correlation:  %lf\n",cooc_correlation(P0,ng));
  printf("====================================================\n");


  //////////////////////////////////////////////////////////////////////////
  //Varrendo a selecao para encontrar a media do feature
  //Nao tem problema de bordas

  //quadSel[0]->x = 100;
  //quadSel[2]->x = 200;
  //quadSel[0]->y = 100;
  //quadSel[2]->y = 200;
  //quadSel[1]->x = quadSel[2]->x;
  //quadSel[1]->y = quadSel[0]->y;
  //quadSel[3]->x = quadSel[0]->x;
  //quadSel[3]->y = quadSel[2]->y;
  //repaint_cb(img_canvas, RESET_REPAINTCOUNTER);


  //quadSel[2]->x = quadSel[0]->x + 7;
  //quadSel[2]->y = quadSel[0]->y + 7;
  //quadSel[1]->x = quadSel[2]->x;
  //quadSel[1]->y = quadSel[0]->y;
  //quadSel[3]->x = quadSel[0]->x;
  //quadSel[3]->y = quadSel[2]->y;
  //repaint_cb(img_canvas, RESET_REPAINTCOUNTER);


  initW = (int)quadSel[0]->x;
  endW  = (int)quadSel[2]->x;
  initH = (int)quadSel[0]->y;
  endH  = (int)quadSel[2]->y;

  cooc_boxOrderVertex(&initW,&endW,&initH,&endH);


  counter = 0; 
  class1_feature = 0.0f;
  i=initW+boxSize;
  j=initH+boxSize;

  for(j=initH+boxSize; j<endH; j+=(2*boxSize)+1)
  {
    for(i=initW+boxSize; i<endW; i+=(2*boxSize)+1)
    {
      w0 = i - boxSize;
      w1 = i + boxSize;
      h0 = j - boxSize;
      h1 = j + boxSize;

      selW = (int)fabs((float)(w1 - w0));
      selH = (int)fabs((float)(h1 - h0));

      ng = cooc_boxImgGrey(data,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);
      cooc_boxBuildMatrix(C0,ng,d,theta,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);
      cooc_normalizeMatrix(C0,P0,ng,theta,selW,selH);

      class1_feature += cooc_energy(P0,ng);
      class2_feature += cooc_entropy(P0,ng);
      class3_feature += cooc_dissimilarity(P0,ng);
      class4_feature += cooc_contrast(P0,ng);
      class5_feature += cooc_IDM(P0,ng);
      class6_feature += cooc_variance(P0,ng);
      class7_feature += cooc_correlation(P0,ng);
      counter++;

      //printf("%f ", class1_feature);
    }
  }  
  if(counter==0)
    counter = 1;

  printf("Counter: %d\n", counter);

  class1_feature /= (float) counter;
  class2_feature /= (float) counter;
  class3_feature /= (float) counter;
  class4_feature /= (float) counter;
  class5_feature /= (float) counter;
  class6_feature /= (float) counter;
  class7_feature /= (float) counter;

  printf("-(M)Energy:       %lf\n",class1_feature);
  printf("-(M)Entropy:      %lf\n",class2_feature);
  printf("-(M)Dssimilarity: %lf\n",class3_feature);
  printf("-(M)Contrast:     %lf\n",class4_feature);
  printf("-(M)IDM:          %lf\n",class5_feature);
  printf("-(M)Variance:     %lf\n",class6_feature);
  printf("-(M)Correlation:  %lf\n",class7_feature);
    
  return IUP_DEFAULT;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
/************************************************************************
 *                                                                     
 ************************************************************************/
float tex_Min(float* values, int size)
{
    //int values[] = { 40, 10, 100, 90, 20, 25 };

    qsort (values, size, sizeof(float), compare);
    return values[0];
 

  /*
  if (dist1<dist2)
  {
    if (dist1<dist3)
      return dist1;
    else
      return dist3;
  }
  else
  {
    if (dist2<dist3)
      return dist2;
    else
      return dist3;
  }
  */
}

/************************************************************************
 *                                                                     
 ************************************************************************/
int tex_Extract_cb(void)
{
  int w0, w1, h0, h1, selW, selH;
  int ** C0;
  float ** P0;
  int ng, i, j,k1,k2,t;
  unsigned char * greyTable;
  float sel_energy=0.0f, sel_entropy=0.0f, sel_dissim=0.0f, sel_contrast=0.0f, sel_idm=0.0f, sel_variance=0.0f, sel_correlation=0.0f; 

  int counter=0;

  float meanVal = 0.0f;
  int valCount=0;
  FILE *file; 
  float dist_p= 100000000000.0f,dist_p2= 100000000000.0f,dist_f= 100000000000.0f,dist_l= 100000000000.0f;
  float* values;

  
  //Criando tabela de cores usadas na selecao da imagem
  greyTable = (unsigned char *) malloc (256 * sizeof(unsigned char));

  IupSetfAttribute(label, "TITLE", "Please Wait...");
  printf("=====================\n=====================\n");
//  printf("D=%d - Theta=%d - Box:%dx%d\n",d,theta,(boxSize*2)+1,(boxSize*2)+1);

  //ng = cooc_boxImgGrey(data,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);

  C0 = (int**)calloc (256,sizeof(int));
  for (i=0; i<256; i++)
    C0[i] = (int*) calloc (256,sizeof(int));

  P0 = (float**)calloc (256,sizeof(float));
  for (i=0; i<256; i++)
    P0[i] = (float*) calloc (256,sizeof(float));


  //////////////////////////////////////////////////////////////////////////
  //Varrendo imagem para ver se o pixel central da selecao pertence a classe 1
  
  meanVal = 0.0f;
  valCount=0;
  file = fopen("values.txt","w+");

  //for(j=boxSize; j<IMG_H; j++){
    //for(i=boxSize; i<IMG_W; i++){
  
    for(j=0+boxSize; j<IMG_H-boxSize; j+=(2*boxSize)+1){
      for(i=0+boxSize; i<IMG_W-boxSize; i+=(2*boxSize)+1){

        w0 = i - boxSize;
        w1 = i + boxSize;
        h0 = j - boxSize;
        h1 = j + boxSize;

        //if (w0 < 0) break;
        //if (w1 > IMG_W) break;
        //if (h0 < 0) break;
        //if (h1 > IMG_H) break;

        selW = (int)fabs((float)(w1 - w0));
        selH = (int)fabs((float)(h1 - h0));

        ng = cooc_boxImgGrey(data,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);
        sel_energy = 0.0f; sel_entropy = 0.0f; sel_dissim  = 0.0f;  sel_contrast  = 0.0f;  
        sel_idm  = 0.0f; sel_variance  = 0.0f; sel_correlation = 0.0f;

        for(t=0; t<=135; t+=45)
        {
          cooc_boxBuildMatrix(C0,ng,d,t,dataGrey,greyTable,w0,w1,h0,h1,IMG_W,IMG_H);
          cooc_normalizeMatrix(C0,P0,ng,t,selW,selH);

          sel_energy += cooc_energy(P0,ng);
          sel_entropy += cooc_entropy(P0,ng);
          sel_dissim += cooc_dissimilarity(P0,ng);
          sel_contrast += cooc_contrast(P0,ng);
          sel_idm += cooc_IDM(P0,ng);
          sel_variance += cooc_variance(P0,ng);
          sel_correlation += cooc_correlation(P0,ng);
        }

        sel_energy /= 4;
        sel_entropy /= 4;
        sel_dissim /= 4;
        sel_contrast /= 4;
        sel_idm /= 4;
        sel_variance /= 4;
        sel_correlation /= 4;

        //fprintf(file, "IDM: %lf\n", sel_idm);
        //valCount++;

        dist_p  = (float)sqrt ( (float)pow(sel_energy-p1_energy ,2) + 
                              (float)pow(sel_entropy-p1_entropy ,2) +
                              (float)pow(sel_dissim-p1_dissim ,2) +
                              (float)pow(sel_contrast-p1_contrast ,2) +
                              (float)pow(sel_idm-p1_idm ,2) +
                              (float)pow(sel_variance-p1_variance ,2) +
                              (float)pow(sel_correlation-p1_correlation ,2)
                              );

        dist_p2 = (float)sqrt ( (float)pow(sel_energy-p2_energy ,2) + 
                              (float)pow(sel_entropy-p2_entropy ,2) +
                              (float)pow(sel_dissim-p2_dissim ,2) +
                              (float)pow(sel_contrast-p2_contrast ,2) +
                              (float)pow(sel_idm-p2_idm ,2) +
                              (float)pow(sel_variance-p2_variance ,2) +
                              (float)pow(sel_correlation-p2_correlation ,2)
                              ); 
                               

        dist_f = (float)sqrt ( (float)pow(sel_energy-f_energy ,2) + 
                              (float)pow(sel_entropy-f_entropy ,2) +
                              (float)pow(sel_dissim-f_dissim ,2) +
                              (float)pow(sel_contrast-f_contrast ,2) +
                              (float)pow(sel_idm-f_idm ,2) +
                              (float)pow(sel_variance-f_variance ,2) +
                              (float)pow(sel_correlation-f_correlation ,2)
                              );

        dist_l = (float)sqrt ( (float)pow(sel_energy-l_energy ,2) + 
                              (float)pow(sel_entropy-l_entropy ,2) +
                              (float)pow(sel_dissim-l_dissim ,2) +
                              (float)pow(sel_contrast-l_contrast ,2) +
                              (float)pow(sel_idm-l_idm ,2) +
                              (float)pow(sel_variance-l_variance ,2) +
                              (float)pow(sel_correlation-l_correlation ,2)
                              );
                             
        values = (float *) malloc (4* sizeof (float));
        values[0] = dist_p;
        values[1] = dist_p2;
        values[2] = dist_f;
        values[3] = dist_l;


        if (  //((sel_energy >= 0.25f)) //&&
              //((sel_entropy >= 0.7f)  && (sel_entropy <= 1.2f)) &&
              //((sel_dissim >= -0.07f) && (sel_dissim <= 0.1f)) &&
              //((sel_contrast >= 1.3f) && (sel_contrast <= 3.2f)) &&
              //((sel_idm >= 1.0f)      && (sel_idm <= 1.5f)) //&&
              //((sel_variance >= 2.7f) && (sel_variance <= 8.0f)) &&
              //((sel_correlation >= -14.0f) && (sel_correlation <= -7.0f))
              tex_Min (values,4) == dist_f
              )

        {
          for(k2=0; k2<=boxSize; k2++)
          {
            for(k1=0; k1<=boxSize; k1++)
            {

              //cantos
              paintSel2[numMask][j + k2][i + k1] = PAINT_COLOR_FIELD;
              paintSel2[numMask][j - k2][i - k1] = PAINT_COLOR_FIELD;

              //diagonais
              paintSel2[numMask][j + k2][i - k1] = PAINT_COLOR_FIELD;
              paintSel2[numMask][j - k2][i + k1] = PAINT_COLOR_FIELD;
            }
          }
        }

        else if(tex_Min (values,4) == dist_p)
        {
          for(k2=0; k2<=boxSize; k2++)
          {
            for(k1=0; k1<=boxSize; k1++)
            {

              //cantos
              paintSel2[numMask][j + k2][i + k1] = PAINT_COLOR_PLAYER;
              paintSel2[numMask][j - k2][i - k1] = PAINT_COLOR_PLAYER;

              //diagonais
              paintSel2[numMask][j + k2][i - k1] = PAINT_COLOR_PLAYER;
              paintSel2[numMask][j - k2][i + k1] = PAINT_COLOR_PLAYER;
            }
          }
        }

        else if(tex_Min (values,4) == dist_p2)
        {
          for(k2=0; k2<=boxSize; k2++)
          {
            for(k1=0; k1<=boxSize; k1++)
            {

              //cantos
              paintSel2[numMask][j + k2][i + k1] = PAINT_COLOR_PLAYER2;
              paintSel2[numMask][j - k2][i - k1] = PAINT_COLOR_PLAYER2;

              //diagonais
              paintSel2[numMask][j + k2][i - k1] = PAINT_COLOR_PLAYER2;
              paintSel2[numMask][j - k2][i + k1] = PAINT_COLOR_PLAYER2;
            }
          }
        }
        else if(tex_Min (values,4) == dist_l)
        {
          for(k2=0; k2<=boxSize; k2++)
          {
            for(k1=0; k1<=boxSize; k1++)
            {

              //cantos
              paintSel2[numMask][j + k2][i + k1] = PAINT_COLOR_LINE;
              paintSel2[numMask][j - k2][i - k1] = PAINT_COLOR_LINE;

              //diagonais
              paintSel2[numMask][j + k2][i - k1] = PAINT_COLOR_LINE;
              paintSel2[numMask][j - k2][i + k1] = PAINT_COLOR_LINE;
            }
          }
        }

      }
    }
  


  //printf("meanVal: %lf\n",meanVal);
  //printf("Count: %d\n",valCount);
  //meanVal /= valCount;
  //printf("meanVal: %lf\n",meanVal);
  fclose(file);
  

  IupSetfAttribute(label, "TITLE", "Finished!");
  repaint_cb(img_canvas, RESET_REPAINTCOUNTER);
  return IUP_DEFAULT;
}

/************************************************************************
*	field_cb
************************************************************************/
int field_cb(void)
{
  actualSelection = QUAD_SELECT;
  return IUP_DEFAULT;
}

/************************************************************************
*	player_cb
************************************************************************/
int player_cb(void)
{
  actualSelection = FREE_SELECT;
  return IUP_DEFAULT;
}


/************************************************************************
*                                                                     
************************************************************************/
static char * paint_getMaskName(int type)
{
  Ihandle* getfile = IupFileDlg();
  char* filename = NULL;

  if(type)//open
  {  
    IupSetAttribute(getfile, IUP_TITLE, "Open Mask");
    IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_OPEN);
  }
  else //save
  {
    IupSetAttribute(getfile, IUP_TITLE, "Save Mask");
    IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_SAVE);
  }

  IupSetAttribute(getfile, IUP_FILTER, "*.*");
  IupSetAttribute(getfile, IUP_FILTERINFO, "Selection Mask");
  IupPopup(getfile, IUP_CENTER, IUP_CENTER);

  filename = IupGetAttribute(getfile, IUP_VALUE);
  return filename;
}

/************************************************************************
 *  paint_saveSelection_cb                                                                   
 ************************************************************************/
int paint_saveSelection_cb()
{
  FILE *file; 
  int i,j;

  char* maskName;  
  maskName = paint_getMaskName(0);  

  if (maskName==NULL) 
    return 0;

  file = fopen(maskName,"w+"); 

  for (j=0; j<IMG_H; j++)
  {
    for (i=0; i<IMG_W; i++)
    {
      fprintf(file, "%d ", paintSel2[numMask][j][i]);
    }
    fprintf(file, "\n");
  }

  IupSetfAttribute(label, "TITLE", "Paint Selection saved.");
  fclose(file);
  return IUP_DEFAULT;
}

/************************************************************************
*  paint_loadSelection_cb                                                                   
************************************************************************/
int paint_loadSelection_cb()
{
  FILE *file; 
  int i,j;

  char* maskName;  
  maskName = paint_getMaskName(1);  

  if (maskName==NULL) 
    return 0;

  file = fopen(maskName,"r+"); 

  for (j=0; j<IMG_H; j++)
  {
    for (i=0; i<IMG_W; i++)
    {
      fscanf (file, "%d", &paintSel2[numMask][j][i]);
    }
  }

  fclose(file);

  paintState = PAINT_COLOR_FIELD;
  lastPaintState = PAINT_COLOR_FIELD;
  actualSelection = PAINT_SELECT;

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  IupSetfAttribute(label, "TITLE", "Paint Selection loaded.");

  return IUP_DEFAULT;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
static char * get_file_name( void )
{
  Ihandle* getfile = IupFileDlg();
  char* filename = NULL;

  IupSetAttribute(getfile, IUP_TITLE, "Open Image");
  IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_OPEN);
  IupSetAttribute(getfile, IUP_FILTER, "*.tga");
  IupSetAttribute(getfile, IUP_FILTERINFO, "Truevision Targa File (*.tga)");
  IupPopup(getfile, IUP_CENTER, IUP_CENTER);  /* o posicionamento nao esta sendo respeitado no Windows */

  filename = IupGetAttribute(getfile, IUP_VALUE);
  return filename;
}

/************************************************************************
*	openfile_cb
************************************************************************/
int openfile_cb(void)
{
  int error;//,i,image_count;
  int width, height, color_mode, data_type;
  void* dataAux;
  //imImage* image;
  imFile* ifile; 

filename = get_file_name();  

  if (filename==NULL) return 0;

  if (strstr(filename,".tga")||strstr(filename,".TGA"))
  {
    ifile = imFileOpen(filename, &error);
    if (!ifile) printf("Error: Couldn't open the image file.");
  }
  else
    IupMessage("Aviso","Formato do arquivo de imagem não reconhecido\n");

  update_dialog_size();

  error    = imFileReadImageInfo(ifile, 0, &width, &height, &color_mode, &data_type);
  dataAux  = (void *)malloc(imImageDataSize(width, height, color_mode, data_type));
  data     = (unsigned char *) malloc(imImageDataSize(width, height, color_mode, data_type));
  dataGrey = (unsigned char *) malloc(imImageDataSize(width, height, color_mode, data_type));
  error    = imFileReadImageData(ifile, dataAux, IM_RGB, IM_PACKED );
  memcpy(data, dataAux,imImageDataSize(width, height, color_mode, data_type) );
  memcpy(dataGrey, dataAux,imImageDataSize(width, height, color_mode, data_type) );

  if (error != IM_ERR_NONE) printf("Error: Couldn't load image info: %d\n",error);

  imFileClose(ifile);

  //fprintf(fpLog,"OPEN %s\n",filename);

  ///Apagando mask
  {
    int i,j;

    for(j=0; j<IMG_H; j++)
      for(i=0; i<IMG_W; i++)
        paintSel2[numMask][j][i] = PAINT_COLOR_NOTHING;
  }
  

  IupSetfAttribute(label, "TITLE", "Image %s opened.", filename);

  /*
  {
    //////////////////////////////////////////////////////////////////////////
    
    int ** C0;
    float ** P0;
    int ng, i;
    unsigned char * greyTable;

    greyTable = (unsigned char *) malloc (256 * sizeof(unsigned char));

    ng = cooc_imgGrey(data,data,greyTable, IMG_W, IMG_H);

    C0 = (int**)calloc (ng,sizeof(int));
    for (i=0; i<ng; i++)
      C0[i] = (int*) calloc (ng,sizeof(int));

    P0 = (float**)calloc (ng,sizeof(float));
    for (i=0; i<ng; i++)
      P0[i] = (float*) calloc (ng,sizeof(float));

    cooc_buildMatrix(C0,ng,1,0,IMG_W,IMG_H,data,greyTable);
    cooc_normalizeMatrix(C0,P0,ng,0,IMG_W,IMG_H);
    cooc_energy(P0,ng);
  }
  */

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER);
  return IUP_DEFAULT;
  
/*
  char format[10], compression[10];
  int error, image_count;
  int width, height, color_mode, data_type;

  // Abrindo a imagem de entrada:
  imFile* ifile = imFileOpen("Canto dir1.tga", &error);

  // Obtendo informações a respeito do formato do arquivo de entrada:
  imFileGetInfo(ifile, format, compression, &image_count);
  for (int i = 0; i < image_count ; i++)
  {
    error = imFileReadImageInfo(ifile, i, &width, &height, &color_mode, &data_type);
    if (error != IM_ERR_NONE) 
      error++;
  }

  // A partir do arquivo de entrada obtido, carregando uma estrutura imImage:
  image = imFileLoadBitmap(ifile, 0, &error);

  data = (unsigned char *) malloc(width * height * 3);
  data = (unsigned char *)image->data;

  imFileClose(ifile);
  return IUP_DEFAULT;
  */

}

/************************************************************************
*	closefile_cb
************************************************************************/
int closefile_cb(void)
{
  //imFileClose(ifile);
  free(data);
  data = NULL;

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER ); 

  //label = IupGetHandle("label");
  //IupStoreAttribute(label,IUP_TITLE,buffer);  
  IupSetfAttribute(label, "TITLE", "Image closed.");


  return IUP_DEFAULT;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
int paint_End_cb (void)
{
  /* Gerando uma semente nova, para os números aleatórios: */
  srand( (unsigned)time( NULL ) );


  //////////////////////////////////////////////////////////////////////////
  //Growing Neural Gas
  amostras = new Amostras(IMG_W*IMG_H+1, 9, 9, 1, IMG_W*IMG_H+1, IMG_W, IMG_H, data);
  amostras->amsSetMasc( paintSel2[numMask], IMG_W, IMG_H );
  amostras->amsAllSamplesCreate();

  // Vetor de flags que liga/desliga uma determinada medição:
  int inSamplesSize = amostras->_inSamplesSize;
  bool *flagsVector =  new bool[inSamplesSize];
  for( int i=0 ; i<inSamplesSize ; i++ )
    flagsVector[i] = false;

  // Somente pra teste, tentando criar uma rede neural e treiná-la:
  // Criando a rede neural:
  int entryNet =inSamplesSize;
  int nLayers = 3;
  neuralTopology = new int[nLayers];
  neuralTopology[0] = entryNet;
  neuralTopology[1] = 3;
  neuralTopology[2] = 1;
  //neuralTopology[3] = 1;

  // Ligo os entryNet primeiros:
  for( int i=0 ; i<entryNet ; i++ )
    flagsVector[i] = true;
  amostras->flagsVectorConfig( &flagsVector[0], inSamplesSize );

  float **tmpinSamples;
  float **tmpoutSamples;
  int **tmpcoordsSamples;

  // Depois de criadas as amostras, obtendo duas matrizes, uma de entrada e outra de saída,
  // contendo todas as amostras de entrada/saída:
  amostras->getSamplesMatrix( &tmpinSamples, &inSamplesSize, &tmpoutSamples, &outSamplesSize, &tmpcoordsSamples, &numSamples );

  int *rInTypesVector;      //< Vetor contendo o tipo (PAINT_COLOR_PLAYER, PAINT_COLOR_FIELD, etc.) de cada uma das amostras retornadas.
  int **rCoordsMatrix;

  // Obtendo matrizes contendo somente as amostras de treino:
  amostras->getTrainSamplesMatrix( &rInTypesVector, &inSamples, &inSamplesSize, &outSamples, &outSamplesSize, 
    &rCoordsMatrix, &numSamples );


  // Imprimindo todas as amostras para verificar sua corretude:
#if 0
  FILE *f = fopen( "amostras.txt", "w" );
  for( int cont=0 ; cont<numSamples ; cont+=10 )
  {
    // Imprimindo as entradas:
    fprintf( f, "Entrada %d: \n", cont );
    for( int cont2=0 ; cont2<inSamplesSize ; cont2++ )
    {
      if( cont2%5 == 0 )
        fprintf(f, "\n");
      fprintf( f, "\t%lf", inSamples[cont][cont2] );
    }
    fprintf( f, "\nSaida: \n" );
    fprintf( f, "\t%lf", outSamples[cont][0] );
    fprintf( f, "\n\n\n" );
  }

  // Fechando o arquivo:
  fclose(f);
#endif


  int retExecutedTrains=0;
  float retRMSError=0;
  float retBestPerc=0;

  // Criando 10 redes neurais bkp e pegando a melhor:
  BKPNeuralNet **myNeuralNets = new BKPNeuralNet*[10];
  BKPNeuralNet *bestRetNet = NULL;  //< Irá armazenar a melhor rede criada.
  float bestPerc = (float)MIN_VALUE;         //< Irá armazenar a porcentagem de acerto da melhor rede.
  for( int i=0 ; i< 10 ; i++ )
  {
    retExecutedTrains=0;
    retRMSError=0;
    retBestPerc = (float)MIN_VALUE;

    /* Gerando uma semente nova, para os números aleatórios: */
    srand( (unsigned)time( NULL ) );
    myNeuralNets[i]= new BKPNeuralNet( nLayers, neuralTopology );
    myNeuralNets[i]->NetInit();
    myNeuralNets[i]->AutoTrain( inSamples, outSamples, entryNet, outSamplesSize, 
      numSamples-1, 15000, 5, 0.001f, 3000, RANDOM_TRAIN, 0.2f, 0.6f, &retExecutedTrains, &retRMSError, &retBestPerc );
    if( retBestPerc > bestPerc )    //< Caso essa seja a melhor rede até aqui, ela é armazenada.
    {
      bestPerc = retBestPerc;
      bestRetNet = myNeuralNets[i];
    }

  }

#if 0
  // Criando a rede:
  testeNeuralNet = new BKPNeuralNet( nLayers, neuralTopology );
  int ret=0;
  while( ret < 10)
  {
    /* Gerando uma semente nova, para os números aleatórios: */
    srand( (unsigned)time( NULL ) );

    testeNeuralNet->NetInit();
    testeNeuralNet->AutoTrain( inSamples, outSamples, entryNet, outSamplesSize, 
      numSamples-1, 15000, 5, 0.001f, 3000, RANDOM_TRAIN, 0.2f, 0.6f, &retExecutedTrains, &retRMSError, &retBestPerc );
    if( retBestPerc < 0.8f )
    {
      //delete testeNeuralNet;
      printf("Nao convergencia: %lf\n",retBestPerc );
      ret++;     //< A rede não convergiu e o procedimento terá que ser refeito.
    }
    if( retBestPerc >= 0.8f )
    {
      ret =10;
      printf("Convergencia: %lf\n",retBestPerc );
    }
  }
#endif

  // Imprimo a taxa de acerto da melhor rede e atualizo o ponteiro global de redes para apontar para a melhor rede treinada:
  printf("Convergencia: %lf\n",bestPerc );
  testeNeuralNet = bestRetNet;

  // Depois de executado o treinamento, é necessário fazer a análise de sensibilidade:
  _prfSenAnalysis = new NewBKPSensAnalysis( numSamples-1, entryNet, outSamplesSize, inSamples, outSamples, testeNeuralNet);
  _prfSenAnalysis->sensitivityBuildMatrices();    //< De posse da rede treinada, constrói as matrizes de sensibilidade.
  _prfSenAnalysis->sensitivityNormalizeMatrices();  //< Escalona as saídas.
  // Obtém o vetor de sensibilidades da matriz desejada, e obtém também os valores da média e variância dos resultados:
  float* resultVector = NULL;
  float retMean = NULL;
  float retVar = NULL;
  _prfSenAnalysis->sensitivityStatistics( 0, &retMean, &retVar, MEAN_SQUARE_AVERAGE );
  _prfSenAnalysis->sensitivityGetResultVector( 0, &resultVector, MEAN_SQUARE_AVERAGE );
  _prfSenAnalysis->sensitivityPrintData( "result.txt" );


  float *newResultsVector;
  bool *newflagsVector;
  // Pegando somente as primeiras 16 entradas (as de mais pesos):
  //FloatVectorMinValuesCut( resultVector, entryNet, 16, &flagsVector[0], &newResultsVector, &newflagsVector);

  // Atualiza o tamanho da entrada:
  //entryNet = 16;

  // Normaliza o vetor de resultados de análise de sensibilidade, para que possa ser utilizado:
  NormalizeFloatVector(resultVector, entryNet);

  // Reconfigura as amostras:
  //amostras->flagsVectorConfig( &flagsVector[0], inSamplesSize );

  // Depois de reconfiguradas as amostras, obtemos novamente as entradas, na nova configuração:
  //amostras->getSamplesMatrix( &tmpinSamples, &inSamplesSize, &tmpoutSamples, &outSamplesSize, &tmpcoordsSamples, &numSamples );


  // Obtendo matrizes contendo somente as amostras de treino LIGADAS ATUALMENTE:
  //amostras->getTrainSamplesMatrix( &rInTypesVector, &inSamples, &inSamplesSize, &outSamples, &outSamplesSize, &rCoordsMatrix, &numSamples );



  // QuantizaGNG( amostras, 128, 3, 200, 0.2f, 0.006f, 0.5f, 0.995f, 50 );

  IupSetfAttribute(label, "TITLE", "Please Wait...");
  QuantizaGNG( amostras, 128, entryNet, 400, 0.05f, 0.0006f, 0.5f, 0.9f, 88, resultVector );
  IupSetfAttribute(label, "TITLE", "Done!");
  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 


  /*
  // Criando a rede neural:
  int nLayers = 2;
  neuralTopology = new int[nLayers];
  neuralTopology[0] = amostras->_inSize;
  neuralTopology[1] = 1;
  //neuralTopology[2] = 1;

  testeNeuralNet = new BKPNeuralNet( nLayers, neuralTopology );
  testeNeuralNet->NetInit();

  // Alocando a variável bacalhau:
  bacalhau = new float[amostras->_inSize];
  saida = new float[1];

  for( int i=0 ; i<1000 ; i++ )
  {
  float *in;
  float *out;
  //printf("%d, ", i);
  amostras->amsGetRandomSample( &in, &out );
  if(Rounded((double)out[0]) == 0)
  printf("0 ");
  if(Rounded((double)out[0]) == 1)
  printf("1 ");
  testeNeuralNet->Train( amostras->_inSize, in, 1, out, 0.2f, 0.8f );
  testeNeuralNet->Use( amostras->_inSize, in );
  testeNeuralNet->GetOut( saida );
  printf("%d. %lf,  ", i, saida[0]);
  if(saida[0] < 0.00003)
  saida[0] = saida[0];
  if(i==1499)
  i=i;
  }

  // Depois de executado o treinamento, treino a rede:
  for( int x=5 ; x<IMG_W ; x++ )
  {
  for( int y=5 ; y<IMG_H ; y++ )
  {
  amostras->imgSmpExtSampleCreate( x-2, y-2, x+2, y+2, &bacalhau );
  testeNeuralNet->Use( amostras->_inSize, bacalhau );
  testeNeuralNet->GetOut( saida );
  if( Rounded((double)saida[0]) == 0)
  paintSel[y][x] = 0;
  if( Rounded((double)saida[0]) == 1)
  paintSel[y][x] = 2;
  }
  }

  */

  delete amostras;
  delete testeNeuralNet;


  //////////////////////////////////////////////////////////////////////////

  IupSetfAttribute(label, "TITLE", "Paint finished");
  return IUP_DEFAULT;
}


/************************************************************************
 *	createSubmenu
 ************************************************************************/
static void createSubmenu()
{
  Ihandle *menu;
  Ihandle *sub1_menu, *sub2_menu, *sub3_menu, *sub4_menu, *sub5_menu, *sub6_menu, *sub7_menu;
  Ihandle *file_menu, *view_menu, *paint_menu, *paintMask_menu, *select_menu, *texture_menu, *help_menu;
  Ihandle *item_open, *item_close, *item_save, *item_exit;
  Ihandle *item_selectPlayer, *item_selectField;
  Ihandle *item_nextMask, *item_prevMask;
  Ihandle *item_paintClear, *item_paintPlayer, *item_paintPlayer2, *item_paintField, *item_paintRefree;
  Ihandle *item_paintLine, *item_paintKeeper, *item_paintKeeper2;
  Ihandle *item_paintEnd, *item_paintInvert, *item_paintSaveSelection, *item_paintLoadSelection;
  Ihandle *item_paintRoundBrush, * item_paintSquareBrush;
  Ihandle *item_texField, *item_texPlayer, *item_texPlayer2, *item_texExtract, *item_texInspect, *item_texLine;
  Ihandle *item_zoom;
  Ihandle *item_about;


  //File Menu
  item_open = IupItem ("Open image", "openfile_cb");
  IupSetAttribute(item_open, IUP_KEY, "O");
  IupSetFunction("openfile_cb", (Icallback)openfile_cb);

  item_save = IupItem ("Save image", "savefile_cb");
  IupSetAttribute(item_save, IUP_KEY, "S");
  IupSetFunction("savefile_cb", (Icallback)savefile_cb);

  item_close = IupItem ("Close image", "closefile_cb");
  IupSetAttribute(item_close, IUP_KEY, "C");
  IupSetFunction("closefile_cb", (Icallback)closefile_cb);

  item_exit = IupItem ("Exit", "exit_cb");
  IupSetAttribute(item_exit, IUP_KEY, "x");
  IupSetFunction("exit_cb", (Icallback)exit_cb);


  //Aquire Select Menu
  item_selectField = IupItem ("Select Field", "field_cb");
  IupSetAttribute(item_selectField, IUP_KEY, "F");
  IupSetFunction("field_cb", (Icallback)field_cb);

  item_selectPlayer = IupItem ("Select Player", "player_cb");
  IupSetAttribute(item_selectPlayer, IUP_KEY, "P");
  IupSetFunction("player_cb", (Icallback)player_cb);


  //Mask Paint Menu
  item_nextMask = IupItem ("Next Mask -->", "paint_nextMask_cb");
  IupSetAttribute(item_nextMask, IUP_KEY, ">");
  IupSetFunction("paint_nextMask_cb", (Icallback)paint_nextMask_cb);

  item_prevMask = IupItem ("Previous Mask <--", "paint_prevMask_cb");
  IupSetAttribute(item_prevMask, IUP_KEY, "<");
  IupSetFunction("paint_prevMask_cb", (Icallback)paint_prevMask_cb);

  item_paintSaveSelection = IupItem ("Save paint selection", "paint_saveSelection_cb");
  IupSetAttribute(item_paintSaveSelection, IUP_KEY, "S");
  IupSetFunction("paint_saveSelection_cb", (Icallback)paint_saveSelection_cb);

  item_paintLoadSelection = IupItem ("Load paint selection", "paint_loadSelection_cb");
  IupSetAttribute(item_paintLoadSelection, IUP_KEY, "L");
  IupSetFunction("paint_loadSelection_cb", (Icallback)paint_loadSelection_cb);

  item_paintClear = IupItem ("Clear Paint", "paint_ClearPoints_cb");
  IupSetAttribute(item_paintClear, IUP_KEY, "C");
  IupSetFunction("paint_ClearPoints_cb", (Icallback)paint_ClearPoints_cb);

  item_paintRoundBrush = IupItem ("Select Round brush", "paint_RoundBrush_cb");
  IupSetAttribute(item_paintRoundBrush, IUP_KEY, "R");
  IupSetFunction("paint_RoundBrush_cb", (Icallback)paint_RoundBrush_cb);

  item_paintSquareBrush = IupItem ("Select Square brush", "paint_SquareBrush_cb");
  IupSetAttribute(item_paintSquareBrush, IUP_KEY, "S");
  IupSetFunction("paint_SquareBrush_cb", (Icallback)paint_SquareBrush_cb);

  item_paintInvert = IupItem ("Invert Paint", "paint_invert_cb");
  IupSetAttribute(item_paintInvert, IUP_KEY, "I");
  IupSetFunction("paint_invert_cb", (Icallback)paint_invert_cb);


  //Aquire Paint Menu
  item_paintField = IupItem ("Paint Field", "paint_Field_cb");
  IupSetAttribute(item_paintField, IUP_KEY, "i");
  IupSetFunction("paint_Field_cb", (Icallback)paint_Field_cb);

  item_paintPlayer = IupItem ("Paint Player (Team 1)", "paint_Player_cb");
  IupSetAttribute(item_paintPlayer, IUP_KEY, "y");
  IupSetFunction("paint_Player_cb", (Icallback)paint_Player_cb);

  item_paintPlayer2 = IupItem ("Paint Player (Team 2)", "paint_Player2_cb");
  IupSetAttribute(item_paintPlayer2, IUP_KEY, "2");
  IupSetFunction("paint_Player2_cb", (Icallback)paint_Player2_cb);

  item_paintKeeper = IupItem ("Paint Keeper (Team 1)", "paint_Keeper_cb");
  IupSetAttribute(item_paintKeeper, IUP_KEY, "K");
  IupSetFunction("paint_Keeper_cb", (Icallback)paint_Keeper_cb);

  item_paintKeeper2 = IupItem ("Paint Keeper (Team 2)", "paint_Keeper2_cb");
  IupSetAttribute(item_paintKeeper2, IUP_KEY, "K");
  IupSetFunction("paint_Keeper2_cb", (Icallback)paint_Keeper2_cb);

  item_paintRefree = IupItem ("Paint Refree", "paint_Refree_cb");
  IupSetAttribute(item_paintRefree, IUP_KEY, "R");
  IupSetFunction("paint_Refree_cb", (Icallback)paint_Refree_cb);

  item_paintLine = IupItem ("Paint Line", "paint_Line_cb");
  IupSetAttribute(item_paintLine, IUP_KEY, "L");
  IupSetFunction("paint_Line_cb", (Icallback)paint_Line_cb);

  item_paintEnd = IupItem ("End Paint", "paint_End_cb");
  IupSetAttribute(item_paintEnd, IUP_KEY, "E");
  IupSetFunction("paint_End_cb", (Icallback)paint_End_cb);

  //View Menu
  item_zoom = IupItem ("Zoom", "zoom_cb");
  IupSetAttribute(item_zoom, IUP_KEY, "Z");
  IupSetFunction("zoom_cb", (Icallback)zoom_cb);

  //Texture Menu
  item_texField = IupItem ("Define Class 1 (Black)", "tex_Field_cb");
  IupSetAttribute(item_texField, IUP_KEY, "f");
  IupSetFunction("tex_Field_cb", (Icallback)tex_Field_cb);

  item_texPlayer = IupItem ("Define Class 2 (White)", "tex_Player_cb");
  IupSetAttribute(item_texPlayer, IUP_KEY, "p");
  IupSetFunction("tex_Player_cb", (Icallback)tex_Player_cb);

  item_texPlayer2 = IupItem ("Define Class 3 (Green)", "tex_Player2_cb");
  IupSetAttribute(item_texPlayer2, IUP_KEY, "2");
  IupSetFunction("tex_Player2_cb", (Icallback)tex_Player2_cb);

  item_texLine = IupItem ("Define Class 4 (Red)", "tex_Line_cb");
  IupSetAttribute(item_texLine, IUP_KEY, "l");
  IupSetFunction("tex_Line_cb", (Icallback)tex_Line_cb);

  item_texExtract = IupItem ("Extract Texture", "tex_Extract_cb");
  IupSetAttribute(item_texExtract, IUP_KEY, "E");
  IupSetFunction("tex_Extract_cb", (Icallback)tex_Extract_cb);

  item_texInspect = IupItem ("Inspect Free Region", "tex_Inspect_cb");
  IupSetAttribute(item_texInspect, IUP_KEY, "I");
  IupSetFunction("tex_Inspect_cb", (Icallback)tex_Inspect_cb);


  //Help Menu
  item_about = IupItem ("About", "about_cb");
  IupSetAttribute(item_about, IUP_KEY, "A");
  IupSetFunction("about_cb", (Icallback)about_cb);

  file_menu = IupMenu(item_open, 
                      item_close,
                      //item_save,
                      IupSeparator(),
                      item_exit,
                      NULL);

  paint_menu = IupMenu(   item_paintField,
                          item_paintPlayer,
                          item_paintPlayer2,
                          item_paintKeeper,
                          item_paintKeeper2,
                          item_paintRefree,
                          item_paintLine,
                          IupSeparator(),
                          item_paintEnd,
                          NULL);

  paintMask_menu = IupMenu(   item_nextMask,
                              item_prevMask,
                              IupSeparator(),
                              item_paintClear,
                              item_paintSaveSelection,
                              item_paintLoadSelection,
                              item_paintInvert,
                              IupSeparator(),
                              item_paintRoundBrush,
                              item_paintSquareBrush,
                              NULL);

  select_menu = IupMenu( item_selectField,
                         item_selectPlayer,   
                         NULL);

  texture_menu = IupMenu(item_texField, 
                         item_texPlayer, 
                         item_texPlayer2,
                         item_texLine,
                         item_texInspect,
                         IupSeparator(),
                         item_texExtract,
                         NULL);

  view_menu = IupMenu(item_zoom,
                      NULL);

  help_menu = IupMenu(item_about, 
                      NULL);

  sub1_menu = IupSubmenu("File", file_menu);
  sub2_menu = IupSubmenu("Paint Acquire", paint_menu);
  sub3_menu = IupSubmenu("Paint Mask Options", paintMask_menu);
  sub4_menu = IupSubmenu("Select Acquire", select_menu);
  sub5_menu = IupSubmenu("Texture Acquire", texture_menu);
  sub6_menu = IupSubmenu("View", view_menu);
  sub7_menu = IupSubmenu("Help", help_menu);

  menu = IupMenu( sub1_menu, 
                  sub2_menu,
                  sub3_menu,
                  sub4_menu,
                  sub5_menu,
                  sub6_menu,
                  sub7_menu,
                  NULL);

  IupSetHandle("mymenu", menu);
}

/************************************************************************
 *  button_cb                                                                   
 ************************************************************************/
int button_cb(Ihandle* self, int botao, int estado, int x, int y)
{
  int sizeX, sizeY;
  int k, i, j;
  GeomPoint2D minPoint, maxPoint, testPoint;

  //first = 1;
  mouseState = estado;


  //Botao da direita
  if(botao == IUP_BUTTON3)
  {
    mouseButton = IUP_BUTTON3;

    if(actualSelection == QUAD_SELECT){}
    else if (actualSelection == FREE_SELECT){}
    else if (actualSelection == PAINT_SELECT)
    {       
      paintState = PAINT_COLOR_NOTHING;

      if(estado) //Apertando
        IupSetAttribute(img_canvas, "CURSOR" , "CURSOR_RUBBER");

      else //Soltando
        IupSetAttribute(img_canvas, "CURSOR" , "CROSS");
    }

    else if (actualSelection == ZOOM_SELECT)
    { 
      if(estado) //Apertando
        IupSetAttribute(img_canvas, "CURSOR" , "CURSOR_ZOOM-");

      else //Soltando
      {
        zoom_minus();
        IupSetAttribute(img_canvas, "CURSOR" , "CROSS");
      }
    }
    else if (actualSelection == TEXTURE_SELECT)
    {
      if(estado)
        tex_Inspect_cb();
    }
  }

  //Botao da esquerda
  else if(botao == IUP_BUTTON1)
  {
    mouseButton = IUP_BUTTON1;

    if(actualSelection == QUAD_SELECT)
    {
      first = 1;
      if(estado) //apertando
      {
        quadSel[0]->x = x;
        quadSel[0]->y = IMG_H - y;

        quadSel[1]->x = quadSel[0]->x;
        quadSel[1]->y = quadSel[0]->y;

        quadSel[2]->x = quadSel[0]->x;
        quadSel[2]->y = quadSel[0]->y;

        quadSel[3]->x = quadSel[0]->x;
        quadSel[3]->y = quadSel[0]->y;
      }
      else //soltando
      {
        //Corrigindo Selecao fora do canvas
        if (quadSel[0]->x > IMG_W) 
          quadSel[0]->x = IMG_W-4;
        if (quadSel[1]->x > IMG_W) 
          quadSel[1]->x = IMG_W-4;
        if (quadSel[2]->x > IMG_W) 
          quadSel[2]->x = IMG_W-4;
        if (quadSel[3]->x > IMG_W) 
          quadSel[3]->x = IMG_W-4;

        if (quadSel[0]->y > IMG_H) 
          quadSel[0]->y = IMG_H-4;
        if (quadSel[1]->y > IMG_H) 
          quadSel[1]->y = IMG_H-4;
        if (quadSel[2]->y > IMG_H) 
          quadSel[2]->y = IMG_H-4;
        if (quadSel[3]->y > IMG_H) 
          quadSel[3]->y = IMG_H-4;

        if (quadSel[0]->x < 0) 
          quadSel[0]->x = 0;
        if (quadSel[1]->x < 0)
          quadSel[1]->x = 0;
        if (quadSel[2]->x < 0)
          quadSel[2]->x = 0;
        if (quadSel[3]->x < 0)
          quadSel[3]->x = 0;

        if (quadSel[0]->y < 0)  
          quadSel[0]->y = 0;
        if (quadSel[1]->y < 0) 
          quadSel[1]->y = 0;
        if (quadSel[2]->y < 0) 
          quadSel[2]->y = 0;
        if (quadSel[3]->y < 0) 
          quadSel[3]->y = 0;

        IupSetfAttribute(label, "TITLE", "Field selected.");
      }
    }
    else if (actualSelection == FREE_SELECT)
    {  
      if(estado) //Apertando
      {
        freeselSize = 0;
        freeEnd = 0;

        quadfreeSel[0]->x = IMG_W; //Salvara aki o menor valor
        quadfreeSel[0]->y = IMG_H;

        quadfreeSel[2]->x = 0; //Salvara aki o maior valor
        quadfreeSel[2]->y = 0;

        //poligono - Zerando informacoes
        polfreeSel->min.x = IMG_W; //Salvara aki o maior valor
        polfreeSel->min.y = IMG_H;

        polfreeSel->max.x = 0; //Salvara aki o maior valor
        polfreeSel->max.y = 0;

        polfreeSel->np = 0;
        polfreeSel->fechado = 1;

        //apagando mascara
        sizeY = (int)(quadfreeSel[2]->y - quadfreeSel[0]->y + 1);

        for (i=0; i<sizeY; i++)
          free (maskfreeSel[i]);
        free (maskfreeSel);

      }

      else //soltando
      {
        freeEnd = 1;

        quadfreeSel[1]->x = quadfreeSel[2]->x;
        quadfreeSel[1]->y = quadfreeSel[0]->y;
        quadfreeSel[3]->x = quadfreeSel[0]->x;
        quadfreeSel[3]->y = quadfreeSel[2]->y;

        {
          //gerar poligono

          //colocando ultimo ponto que eh identico ao 1o
          polfreeSel->pol[freeselSize].x = polfreeSel->pol[0].x;
          polfreeSel->pol[freeselSize].y = polfreeSel->pol[0].y; 
          freeselSize++;


          minPoint.x = quadfreeSel[0]->x;
          minPoint.y = quadfreeSel[0]->y;
          maxPoint.x = quadfreeSel[2]->x;
          maxPoint.y = quadfreeSel[2]->y;

          polfreeSel->np = freeselSize+1;  //+1 pois comeca em zero
          polfreeSel->min = minPoint;
          polfreeSel->max = maxPoint;
          polfreeSel->fechado = 1;
          
          /*
          //Debug - Imprimir Pontos
          printf("Pontos: %d\n", freeselSize);
          printf("MAX: (%.2f,%.2f) - MIN: (%.2f,%.2f)\n", 
                    polfreeSel->max.x,polfreeSel->max.y,polfreeSel->min.x,polfreeSel->min.y);

          for (k=0; k<freeselSize ;k++) 
            printf(": %.2f, %.2f\n", polfreeSel->pol[k].x, polfreeSel->pol[k].y);
          */


          IupSetfAttribute(label, "TITLE", "Getting information, please wait...");

          sizeX = (int)(quadfreeSel[2]->x - quadfreeSel[0]->x + 1);
          sizeY = (int)(quadfreeSel[2]->y - quadfreeSel[0]->y + 1);

          maskfreeSel = (GeomPoint2D**) malloc (sizeY * sizeof(GeomPoint2D));
          for (i=0; i<sizeY; i++)
            maskfreeSel[i] = (GeomPoint2D*) malloc (sizeX * sizeof(GeomPoint2D));

          for (i=0; i<sizeY ;i++)
            for (j=0; j<sizeX ;j++)
            {
              testPoint.x = j + (int)quadfreeSel[0]->x;
              testPoint.y = i + (int)quadfreeSel[0]->y;

              k = GeomIn(polfreeSel, testPoint);

              maskfreeSel[i][j].mask = (float) k;
              maskfreeSel[i][j].x = testPoint.x;
              maskfreeSel[i][j].y = testPoint.y;

            }

          IupSetfAttribute(label, "TITLE", "Player selected.");

        }
      }
    }
    else if (actualSelection == PAINT_SELECT)
    {  

      if(estado) //Apertando
      {
        paintState = lastPaintState;
        //first = 1;
        //paintselSize = 0;
        //first = 0;
        //repaint_cb(img_canvas); 
        IupSetAttribute(img_canvas, "CURSOR" , "PEN");
      }
      else //Soltando
      {
        lastPaintState = paintState;
        first = 1;
        //paintSel[paintselSize]->x = 
        //
        //repaint_cb(img_canvas); 
        IupSetAttribute(img_canvas, "CURSOR" , "CROSS");
      }
    }
    else if (actualSelection == ZOOM_SELECT)
    { 
      if(estado) //Apertando
        IupSetAttribute(img_canvas, "CURSOR" , "CURSOR_ZOOM+");

      else //Soltando
      {
        zoom_plus();
        IupSetAttribute(img_canvas, "CURSOR" , "CROSS");
      }
    }
    else if(actualSelection == TEXTURE_SELECT)
    {
      first = 1;
      if(estado) //apertando
      {
        quadSel[0]->x = x;
        quadSel[0]->y = IMG_H - y;

        quadSel[1]->x = quadSel[0]->x;
        quadSel[1]->y = quadSel[0]->y;

        quadSel[2]->x = quadSel[0]->x;
        quadSel[2]->y = quadSel[0]->y;

        quadSel[3]->x = quadSel[0]->x;
        quadSel[3]->y = quadSel[0]->y;
      }
      else //soltando
      {
        //Corrigindo Selecao fora do canvas
        if (quadSel[0]->x > IMG_W) 
          quadSel[0]->x = IMG_W-4;
        if (quadSel[1]->x > IMG_W) 
          quadSel[1]->x = IMG_W-4;
        if (quadSel[2]->x > IMG_W) 
          quadSel[2]->x = IMG_W-4;
        if (quadSel[3]->x > IMG_W) 
          quadSel[3]->x = IMG_W-4;

        if (quadSel[0]->y > IMG_H) 
          quadSel[0]->y = IMG_H-4;
        if (quadSel[1]->y > IMG_H) 
          quadSel[1]->y = IMG_H-4;
        if (quadSel[2]->y > IMG_H) 
          quadSel[2]->y = IMG_H-4;
        if (quadSel[3]->y > IMG_H) 
          quadSel[3]->y = IMG_H-4;

        if (quadSel[0]->x < 0) 
          quadSel[0]->x = 0;
        if (quadSel[1]->x < 0)
          quadSel[1]->x = 0;
        if (quadSel[2]->x < 0)
          quadSel[2]->x = 0;
        if (quadSel[3]->x < 0)
          quadSel[3]->x = 0;

        if (quadSel[0]->y < 0)  
          quadSel[0]->y = 0;
        if (quadSel[1]->y < 0) 
          quadSel[1]->y = 0;
        if (quadSel[2]->y < 0) 
          quadSel[2]->y = 0;
        if (quadSel[3]->y < 0) 
          quadSel[3]->y = 0;

        IupSetfAttribute(label, "TITLE", "Texture Field selected.");
      }
    }
  }
 
 repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;	
}

/************************************************************************
 *  motion_cb                                                                   
 ************************************************************************/
int motion_cb(Ihandle* self, int x, int y)
{
  static char buffer[40];	

  canvasX = x;
  canvasY = y;

  if((actualSelection == QUAD_SELECT) && (mouseButton == IUP_BUTTON1))
  {
    if (mouseState == 1) 
    {

        quadSel[2]->x = x;
        quadSel[2]->y = IMG_H - y;

        quadSel[1]->x = quadSel[2]->x;
        quadSel[1]->y = quadSel[0]->y;

        quadSel[3]->x = quadSel[0]->x;
        quadSel[3]->y = quadSel[2]->y;

        //first = 0;
          repaint_cb(img_canvas, 0); 
    }
  }
  else if ((actualSelection == FREE_SELECT) && (mouseButton == IUP_BUTTON1))
  {
    if (mouseState == 1) 
    {
      //poligono
      polfreeSel->pol[freeselSize].x = x;
      polfreeSel->pol[freeselSize].y = IMG_H - y;


      //selecao
      //freeSel[freeselSize]->x = x;
      //freeSel[freeselSize]->y = IMG_H - y;
      freeselSize++;

      if(x < quadfreeSel[0]->x)
        quadfreeSel[0]->x = x;

      if(x > quadfreeSel[2]->x)
        quadfreeSel[2]->x = x;

      if(IMG_H - y < quadfreeSel[0]->y)
        quadfreeSel[0]->y = IMG_H - y;

      if(IMG_H - y > quadfreeSel[2]->y)
        quadfreeSel[2]->y = IMG_H - y;

      
      first = 0;
      repaint_cb(img_canvas,RESET_REPAINTCOUNTER); 
    }
  }
  else if (actualSelection == PAINT_SELECT)
  {
    if (mouseState == 1) //apertando
    {      

      /*
                      [1] 

                       o      
          __________________________
                      [2]

                       x
                      xox
                       x
          _________________________
                       [3]

                        x
                       xxx
                      xxoxx
                       xxx
                        x

      */

      if((y>=0) && (y<IMG_H) && (x>=0) && (x<IMG_W)) //verificando pontos dentro do canvas
      { 
        int intSize;
        int i,j;
        int roundSize = 2;
        
        int** paintAuxSel;

        if(zoomFactor == 1)
          paintAuxSel = paintSel2[numMask];
        else
          paintAuxSel = paintZoomSel;
        

        //Camada interna
        intSize = brushSize - 2; //Tamanho da camada (largura)
        intSize *= zoomFactor;

        for(j=0; j<intSize; j++)
        {
          for(i=0; i<intSize; i++)
          {

            //cantos
            if(((IMG_H - y + j) < IMG_H) && ((IMG_H - y + j) >= 0) &&
               ((x + i) < IMG_W) && ((x + i) >= 0))
                paintAuxSel[IMG_H - y + j][x + i] = paintState;

            if(((IMG_H - y - j) < IMG_H) && ((IMG_H - y - j) >= 0) &&
              ((x - i) < IMG_W) && ((x - i) >= 0))
              paintAuxSel[IMG_H - y - j][x - i] = paintState;

            //diagonais
            if(((IMG_H - y + j) < IMG_H) && ((IMG_H - y + j) >= 0) &&
              ((x - i) < IMG_W) && ((x - i) >= 0))
              paintAuxSel[IMG_H - y + j][x - i] = paintState;

            if(((IMG_H - y - j) < IMG_H) && ((IMG_H - y - j) >= 0) &&
              ((x + i) < IMG_W) && ((x + i) >= 0))
              paintAuxSel[IMG_H - y - j][x + i] = paintState;

            //arredondando
            if(brushType == PAINT_BRUSH_ROUND)
            {
              if(((IMG_H - y + j/2) < IMG_H) && ((IMG_H - y + j/2) >= 0) &&
                ((x + i + roundSize) < IMG_W) && ((x + i + roundSize) >= 0))
                  paintAuxSel[IMG_H - y + j/2][x + i + roundSize] = paintState;

              if(((IMG_H - y - j/2) < IMG_H) && ((IMG_H - y - j/2) >= 0) &&
                ((x + i + roundSize) < IMG_W) && ((x + i + roundSize) >= 0))
                  paintAuxSel[IMG_H - y - j/2][x + i + roundSize] = paintState;

              if(((IMG_H - y + j + roundSize) < IMG_H) && ((IMG_H - y + j + roundSize) >= 0) &&
                ((x + i/2) < IMG_W) && ((x + i/2) >= 0))
                  paintAuxSel[IMG_H - y + j + roundSize][x + i/2] = paintState;

              if(((IMG_H - y + j + roundSize) < IMG_H) && ((IMG_H - y + j + roundSize) >= 0) &&
                ((x - i/2) < IMG_W) && ((x - i/2) >= 0))
                  paintAuxSel[IMG_H - y + j + roundSize][x - i/2] = paintState;

              if(((IMG_H - y + j/2) < IMG_H) && ((IMG_H - y + j/2) >= 0) &&
                ((x - i - roundSize) < IMG_W) && ((x - i - roundSize) >= 0))
                  paintAuxSel[IMG_H - y + j/2][x - i - roundSize] = paintState;

              if(((IMG_H - y - j/2) < IMG_H) && ((IMG_H - y - j/2) >= 0) &&
                ((x - i - roundSize) < IMG_W) && ((x - i - roundSize) >= 0))
                paintAuxSel[IMG_H - y - j/2][x - i - roundSize] = paintState;

              if(((IMG_H - y - j - roundSize) < IMG_H) && ((IMG_H - y - j - roundSize) >= 0) &&
                ((x + i/2) < IMG_W) && ((x + i/2) >= 0))
                paintAuxSel[IMG_H - y - j - roundSize][x + i/2] = paintState;

              if(((IMG_H - y - j - roundSize) < IMG_H) && ((IMG_H - y - j - roundSize) >= 0) &&
                ((x - i/2) < IMG_W) && ((x - i/2) >= 0))
                  paintAuxSel[IMG_H - y - j - roundSize][x - i/2] = paintState;
            }
          }
        }

      }

      //Atualizacao da tela para borracha
      if(paintState == PAINT_COLOR_NOTHING)
        first = 1;
      else
        first = 0;

      repaint_cb(img_canvas,0); 
    }
  }
  else if((actualSelection == TEXTURE_SELECT) && (mouseButton == IUP_BUTTON1))
    {
      if (mouseState == 1) 
      {

        quadSel[2]->x = x;
        quadSel[2]->y = IMG_H - y;

        quadSel[1]->x = quadSel[2]->x;
        quadSel[1]->y = quadSel[0]->y;

        quadSel[3]->x = quadSel[0]->x;
        quadSel[3]->y = quadSel[2]->y;

        //first = 0;
        repaint_cb(img_canvas, 0); 
      }
    }

  return IUP_DEFAULT;
}


/************************************************************************
 *  wheel_cb                                                                  
 ************************************************************************/
int wheel_cb(Ihandle *self, float delta, int x, int y, char *r)
{
  /*
  self: identifier of the canvas that activated the function's execution.
  delta: the amount the wheel was rotated in notches.
  x, y: position in the canvas where the event has occurred, in pixels.
  r: status of mouse buttons and certain keyboard keys at the moment the 
      event was generated. The following macros must be used for verification:.


      for(j=0; j<intSize; j++)
      {
      for(i=0; i<intSize; i++)
      {
      //cantos
      if(((IMG_H - y + j) < IMG_H) && ((IMG_H - y + j) >= 0) &&
      ((x + i) < IMG_W) && ((x + i) >= 0))
      paintSel[IMG_H - y + j][x + i] = paintState;
  */
  
  //ZOOM
  //int width = IMG_W, height=IMG_H, color_space, data_type, palette_count;
  //long *palette;
  //void* buffer;

  //image = imImageCreate(width, height, color_space, data_type);
  //image = imImageInit(width, height, color_space, data_type, buffer, palette, palette_count);

  //image = imImageCreate(width, height, color_mode, data_type);
  //image2 = imImageCreate(width, height, color_mode, data_type);
  //imProcessPixelate  (image, image2, 40);

/*
if(delta > 0)
{
  printf("FRONT\n");
  glReadBuffer (GL_FRONT);
}
else
{
  printf("BACK\n");
  glReadBuffer (GL_FRONT_AND_BACK);
  IupGLSwapBuffers(self);
}
  //IupGLSwapBuffers(self);
  //repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
*/

  /*
  static GLint screeny;
  screeny = IMG_H - (GLint) y;
  glRasterPos2i (x, screeny);
  glPixelZoom (zoomFactor, zoomFactor);
  glCopyPixels (0, 0, IMG_W/2, IMG_H/2, GL_COLOR);
  glPixelZoom (1.0, 1.0);
  glFlush ();
  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  */

  
  //zoom+
  if (delta > 0)
  {
    //IupSetAttribute(img_canvas, "CURSOR" , "CURSOR_ZOOM+");
    zoom_plus();
  }

  //zoom -
  else if (delta<0)
  {
    //IupSetAttribute(img_canvas, "CURSOR" , "CURSOR_ZOOM-");
    zoom_minus();
  }

  //IupSetAttribute(img_canvas, "CURSOR" , "CROSS");
  return IUP_DEFAULT;	
}


/************************************************************************
 * slider_brush_cb
 Funcionalidade do IUPVAL para controlar o brushSize
 ************************************************************************/
int slider_brush_cb(Ihandle *self, double value)
{
  brushSize = (int)(value*20.0);
  if(brushSize<3)
    brushSize=3;

  IupSetfAttribute(label, "TITLE", "Brush Size: %d", brushSize);
  return IUP_DEFAULT;
}

/************************************************************************
* slider_alpha_cb
************************************************************************/
int slider_alpha_cb(Ihandle *self, double value)
{
  alpha = (float)value;

  IupSetfAttribute(label, "TITLE", "Alpha: %.2f", alpha);

  repaint_cb(img_canvas, RESET_REPAINTCOUNTER); 
  return IUP_DEFAULT;
}


/************************************************************************
 *	init
 ************************************************************************/
int init(void)
{
  Ihandle *statusbar,  *box;
  Ihandle *toolbar;//, *load, * tranf, *undo, *save;
  Ihandle *val, *val2;
  time_t now = time(NULL);  
  int i,j,k;

  quadSel = (GeomPoint2D**)malloc (4*sizeof(GeomPoint2D));
  for (i=0; i<4 ;i++)
    quadSel[i] = (GeomPoint2D*)malloc (sizeof(GeomPoint2D));

  quadfreeSel = (GeomPoint2D**)malloc (4*sizeof(GeomPoint2D));
  for (i=0; i<4 ;i++)
    quadfreeSel[i] = (GeomPoint2D*)malloc (sizeof(GeomPoint2D));

  polfreeSel = (GeomPol2D*)malloc (sizeof(GeomPol2D));
  polfreeSel->pol = (GeomPoint2D*)malloc (IMG_H * IMG_W * sizeof(GeomPoint2D));

  //paintSel = (int**)calloc (IMG_H,sizeof(int));
  //for (i=0; i<IMG_H; i++)
    //paintSel[i] = (int*) calloc (IMG_W,sizeof(int));


  paintSel2 = (int***)calloc (MAX_MASKS,sizeof(int));
  for (j=0; j<MAX_MASKS; j++)
  {
    paintSel2[j] = (int**)calloc (IMG_H,sizeof(int));

    for (i=0; i<IMG_H; i++)
      paintSel2[j][i] = (int*) calloc (IMG_W,sizeof(int));
  }

  for(k=0; k<MAX_MASKS; k++)
    for(j=0; j<IMG_H; j++)
      for(i=0; i<IMG_W; i++)
        paintSel2[k][j][i] = PAINT_COLOR_NOTHING;


  paintZoomSel = (int**)calloc (IMG_H,sizeof(int));
  for (i=0; i<IMG_H; i++)
    paintZoomSel[i] = (int*) calloc (IMG_W,sizeof(int));

  for(j=0; j<IMG_H; j++)
    for(i=0; i<IMG_W; i++)
    {
      //paintSel[j][i] = PAINT_COLOR_NOTHING;
      paintZoomSel[j][i] = PAINT_COLOR_NOTHING;
    }


  //Slider
  val = IupVal("HORIZONTAL");
  IupSetAttribute(val, "MOUSEMOVE_CB", "slider_brush_cb");
  IupSetAttribute(val, "VALUE", "0.4");
  IupSetFunction("slider_brush_cb", (Icallback)slider_brush_cb);

  val2 = IupVal("HORIZONTAL");
  IupSetAttribute(val2, "MOUSEMOVE_CB", "slider_alpha_cb");
  IupSetAttribute(val2, "VALUE", "0.5");
  IupSetFunction("slider_alpha_cb", (Icallback)slider_alpha_cb);

  /* creates the toolbar and its buttons 
  load = IupButton("", "load_cb");
  IupSetAttribute(load,"TIP","Carrega uma imagem.");
  IupSetAttribute(load,"IMAGE","icon_lib_open");
  IupSetFunction("load_cb", (Icallback)load_cb);

  tranf = IupButton("", "transf_cb");
  IupSetAttribute(tranf,"TIP","Transfere para o outro canvvas.");
  IupSetAttribute(tranf,"IMAGE","icon_lib_transfer");

  undo = IupButton("","undo_cb"),
  IupSetAttribute(undo,"TIP","Desfaz processamento (qdo possivel).");
  IupSetAttribute(undo,"IMAGE","icon_lib_undo");

  save = IupButton("", "save_cb");
  IupSetAttribute(save,"TIP","Salva imagem processada.");
  IupSetAttribute(save,"IMAGE","icon_lib_save");
  */
  createSubmenu();

  toolbar = IupHbox(
      IupFill(),
      val,
      val2,
      IupFill(),
      //load, 
      //tranf,
      //undo,
      //save,
     NULL);

  IupSetAttribute(toolbar, "ALIGNMENT", "ACENTER");
 
  img_canvas = IupGLCanvas("repaint_cb"); 
  //IupSetAttribute(img_canvas,IUP_RASTERSIZE,"320x240");
  IupSetAttribute(img_canvas, "RESIZE_CB", "resize_cb");
  IupSetAttribute(img_canvas, "CURSOR" , "CROSS");
 // IupSetAttribute(img_canvas, "EXPAND" , "NO");
  IupSetAttribute(img_canvas, "BUFFER" , "SINGLE");


  IupSetAttribute(img_canvas, "BUTTON_CB","button_cb");
  IupSetAttribute(img_canvas, "WHEEL_CB","wheel_cb");
  IupSetAttribute(img_canvas, "MOTION_CB","motion_cb");

  IupSetFunction("repaint_cb", NULL);
  IupSetFunction("resize_cb", (Icallback) resize_cb);

  IupSetFunction("button_cb", (Icallback) button_cb);
  IupSetFunction("wheel_cb", (Icallback) wheel_cb);
  IupSetFunction("motion_cb", (Icallback) motion_cb);


  /* the status bar is just a label to put some usefull information in run time */
  label = IupLabel("status");
  IupSetAttribute(label, "EXPAND", "HORIZONTAL");
  IupSetAttribute(label, "FONT", "COURIER_NORMAL_10");
  statusbar = IupSetAttributes(IupHbox(
                IupFrame(IupHbox(label, NULL)), 
                NULL), "MARGIN=5x5");

  /* this is the most external box that puts together
     the toolbar, the two canvas and the status bar */
  box = IupVbox(
          toolbar,
          IupSetAttributes(IupHbox(
            img_canvas, 
            NULL), "GAP=5"),
          statusbar, 
          NULL);

  /* create the mainDlg and set its attributes */

  mainDlg = IupDialog(box);

    IupSetAttribute(mainDlg, IUP_MENU, "mymenu");

  IupSetAttribute(mainDlg, "CLOSE_CB", "app_exit_cb");
  IupSetAttribute(mainDlg, "TITLE", "Trabalho 2 - FGC - Aurelio & Mauricio");
  IupSetAttribute(mainDlg, "TOPMOST", "NO");
  //IupSetAttribute(mainDlg, "RESIZE", "NO");
  //IupSetAttribute(mainDlg, "MAXBOX", "NO");
  IupShowXY(mainDlg, IUP_CENTER, IUP_CENTER);

  fpLog = fopen("log.txt","at");
  fprintf(fpLog,"****************************************************************\n");
  fprintf(fpLog,"Data: %s\n",ctime(&now));


  IupSetFunction("repaint_cb", (Icallback) repaint_cb);

  update_dialog_size( );

  first = 1;
  repaint_cb(img_canvas,RESET_REPAINTCOUNTER);  

  IupSetfAttribute(label, "TITLE", "idle");


  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


  return 1;
}

/************************************************************************
 *	main
 ************************************************************************/
void main(void) 
{
    IupOpen();
    IupControlsOpen();
    IupGLCanvasOpen();
    IconLibOpen();
    if ( init() )
      IupMainLoop();
    IupClose();
}
