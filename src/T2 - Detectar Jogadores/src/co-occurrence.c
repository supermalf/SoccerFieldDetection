/* *************************************************************
*
* Class:  
* Created: 
* Author: malf
* $Id: $
*
* Description: 
* 
* $Log: $
*
Funcoes:

- Recebe uma imImage e converte para tons de cinza
- Quantizar a imagem para 16, 32, 64, 128 cores de cinza (fazer)
- Construir matriz das 4 direcoes para histograma 3d
- Normalizar matriz para obter as probabilidades


Ideia: Varrer selecao de campo com uma janela AxA buscando uma media das janelas para o
valor final do feature.

Amanha: 
- Ver outras matrizes (90,135,45)
ok- outras distancias, d=1,...
- Eficiencia do buildMatrix
- caixas de selecao para jogadores
- mais selecoes de campo? selecao de linha? times diferentes?
ok- outros features
ok- mais tons de cinza? 32? 64?
- funcao contar tempo

-Abordagem com todos os 4 pixels das matrizes ao mesmo tempo (ver keyres)



TODO:
- ideia com janela

* *************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include "co-occurrence.h"

//#define COOC_VERBOSE

/************************************************************************
*       
Transforma o data de entrada colorido para tons de cinza e retorna o 
numero de cores diferentes.
************************************************************************/
int cooc_imgGrey(unsigned char* colorImage, unsigned char * greyImage,
                 unsigned char* greyTable, int w, int h)
{
  int i, totalColors=0;
  unsigned char * uniqueColorsImage;
  float r,g,b;

  uniqueColorsImage = (unsigned char *) calloc (256,sizeof(unsigned char));

  for (i=0; i<w*h*3; i+= 3)
  {
    r = 0.30f*(colorImage[i]/255.0f);
    g = 0.59f*(colorImage[i+1]/255.0f);
    b = 0.11f*(colorImage[i+2]/255.0f);

    greyImage[i]   = (unsigned char) ((r+g+b)*255);
    greyImage[i+1] = greyImage[i];
    greyImage[i+2] = greyImage[i];

    uniqueColorsImage [greyImage[i]] = 1;
  }

  //contando cores
  for (i=0; i<256; i++) 
    if (uniqueColorsImage[i])
    {
      greyTable[totalColors] = i;
      totalColors++;
      #ifdef COOC_VERBOSE
      printf("c:%d\n", i);
      #endif
     
    }
    #ifdef COOC_VERBOSE
    printf("=====================\n");
    #endif

    return totalColors;
}

/************************************************************************
 *       
 Transforma o um pedaco data de entrada colorido para tons de cinza e retorna o 
 numero de cores diferentes.

colorImage - Imagem Colorida de entrada
greyImage - Imagem em tons de cinza de saida
greyTable - Tabela de tons de cinza utilizados de saida
w0, w1, h0, h1 - Vertices da caixa de selecao da imagem
imgH, imgW - Medidas originais da imagem

 ************************************************************************/
int cooc_boxImgGrey(unsigned char* colorImage, unsigned char * greyImage, unsigned char* greyTable, 
                    int w0, int w1, int h0, int h1, int imgW, int imgH)
{
  int i,j, totalColors=0, temp;
  unsigned char * uniqueColorsImage;
  float r,g,b;
  int selW,selH;
  int initW, endW;

  uniqueColorsImage = (unsigned char *) calloc (256,sizeof(unsigned char));

  //Colocando o menor no h0, w0
  //cooc_boxOrderVertex(&w0,&w1,&h0,&h1);
  if(h0>h1)
  {
    temp = h0;
    h0 = h1;
    h1 = temp;
  }
  if(w0>w1)
  {
    temp = w0;
    w0 = w1;
    w1 = temp;
  }

  selW = w1 - w0;
  selH = h1 - h0;


  for (j=0; j<=selH; j++)
  {
    initW = ((imgW*(h0+j))+w0)*3;
    endW  = ((imgW*(h0+j))+w1)*3;

    for (i=initW; i<=endW; i+=3)
    {
      r = 0.30f*(colorImage[i]/255.0f);
      g = 0.59f*(colorImage[i+1]/255.0f);
      b = 0.11f*(colorImage[i+2]/255.0f);

      greyImage[i]   = (unsigned char) ((r+g+b)*255);
      greyImage[i+1] = greyImage[i];
      greyImage[i+2] = greyImage[i];

      uniqueColorsImage [greyImage[i]] = 1;
    }
  }
 
  //contando cores
  for (i=0; i<256; i++) 
    if (uniqueColorsImage[i])
    {
      greyTable[totalColors] = i;
      totalColors++;
      #ifdef COOC_VERBOSE
      printf("c:%d\n", i);
      #endif
    }

  #ifdef COOC_VERBOSE
  printf("=====================\n");
  #endif

  return totalColors;
}

/************************************************************************
 *    
 Constroi a matriz de co-correlacao P para uma dada distancia e um dado angulo
 matrix [y][x]
 ng - numero de tons de cinza
 d - distancia
 theta - angulo

 TODO: Otimizar a matriz jah q ela eh simetrica

 (Funcionando apenas para zero grau)
 ************************************************************************/
void cooc_buildMatrix (int ** O, int ng, int d, int theta, int w, int h,
                  unsigned char * image, unsigned char * greyTable )
{
  int i,x,y,c,counter;
  unsigned char color, nextColor, myColor, myNextColor;

  if(theta != 0)
    return;
   
  for(y=0; y<ng ;y++)
  {
    myColor = greyTable[y]; 

    for(x=0; x<ng ;x++)
    {
      counter = 0;
      myNextColor = greyTable[x]; 

      for (i=0; i<w*h*3; i+= 3)
      {
        color   = image[i];

        //Percorrendo as distancias d
        for (c=3; c/3<=d; c+=3)
        {
          nextColor = image[i+c]; //0 grau

          if((color == myColor) && (nextColor == myNextColor) && ((i/(w*3)) == ((i+3)/(w*3))))
          {
            counter++;
            break;
          }
        }
      }

      if(x==y)
        O[y][x] = 2*counter;
      else
        O[y][x] = counter;

      #ifdef COOC_VERBOSE
      printf("%d ", O[y][x]);
      #endif
    }
    #ifdef COOC_VERBOSE
    printf("\n");
    #endif
  }
  #ifdef COOC_VERBOSE
  printf("=====================\n");
  #endif
}

/************************************************************************
 * mesma versao acima porem para uma caixa de selecao                                                                    
 ************************************************************************/
void cooc_boxBuildMatrix (int ** O, int ng, int d, int theta,
                          unsigned char * image, unsigned char * greyTable,
                          int w0, int w1, int h0, int h1, int imgW, int imgH)
{
  int i,j,x,y, counter, temp, c;
  int selW,selH;
  int initW, endW;
  int factorW, factorH;
  unsigned char color, nextColor, myColor, myNextColor;

  //Colocando o menor no h0, w0
  if(h0>h1)
  {
    temp = h0;
    h0 = h1;
    h1 = temp;
  }
  if(w0>w1)
  {
    temp = w0;
    w0 = w1;
    w1 = temp;
  }
  
  selW = w1 - w0;
  selH = h1 - h0;


  //Fator de busca na matriz
  if(theta == 0)
  {
    factorW=0;
    factorH=1;
  }
  else if(theta == 90)
  {
    factorW=1;
    factorH=0;
  }
  else if ((theta == 45) || (theta == 135))
  {
    factorW=1;
    factorH=1;
  }

  //Construindo matriz
  for(y=0; y<ng ;y++)
  {
    myColor = greyTable[y]; 

    for(x=0; x<ng ;x++)
    {
      counter = 0;
      myNextColor = greyTable[x]; 

      for (j=0; j<=selH-factorH; j++)
      {
        initW = ((imgW*(h0+j))+w0)*3;
        endW  = ((imgW*(h0+j))+w1)*3;

        for (i=initW; i<=endW-factorW; i+=3)
        {
          color = image[i];

          //Percorrendo as distancias d
          for (c=1; c<=d; c++)
          {
            //Pegando proximo de acordo com o angulo
            if(theta == 0)
            {
              nextColor = image[i+(c*3)];
              if((color == myColor) && (nextColor == myNextColor) && ((i+3) < endW))
              {
                counter++;
                break;
              }
            }
            else if(theta == 90)
            {
              nextColor = image[(((imgW*(h0+j+c))+w0)*3) + (i-initW)];
              if((color == myColor) && (nextColor == myNextColor) && ((j+1)<selH))
              {
                counter++;
                break;
              }
            }
            else if(theta == 45)
            {
              nextColor = image[(((imgW*(h0+j+c))+w0)*3) + (i-initW) + 3];
              if((color == myColor) && (nextColor == myNextColor) && ((i+3) < endW) && ((j+1)<selH))
              {
                counter++;
                break;
              }
            }
            else if(theta == 135)
            {
              nextColor = image[(((imgW*(h0+j+c))+w0)*3) + (i-initW) - 3];
              if((color == myColor) && (nextColor == myNextColor) && ((i-3) >= initW) && ((j+1)<selH))
              {
                counter++;
                break;
              }
            }
          }
        }
      }

      if(x==y)
        O[y][x] = 2*counter;
      else
        O[y][x] = counter;

      #ifdef COOC_VERBOSE
      printf("%d ", O[y][x]);
      #endif
    }
    #ifdef COOC_VERBOSE
    printf("\n");
    #endif
  }
  #ifdef COOC_VERBOSE
  printf("=====================\n");
  #endif
}

/************************************************************************
 *    
 O - Matrz de ocorrencias
 P - matriz de probabilidades
 ************************************************************************/
void cooc_normalizeMatrix(int ** O, float ** P, int ng, int theta, int w, int h)
{
  int x,y;
  float factor; //Fator de normalizacao da matriz

  if (theta == 0)
    factor = 2.0f*(float)h*((float)w-1.0f);
  else if (theta == 90)
    factor = 2.0f*(float)w*((float)h-1.0f);
  else if ((theta == 45) || (theta == 135))
    factor = 2.0f*((float)w-1.0f)*((float)h-1.0f);
  else  //erro!
    return;

  for(y=0; y<ng ;y++)
  {
    for(x=0; x<ng ;x++)
    {
      P[y][x] = O[y][x] / factor;
      #ifdef COOC_VERBOSE
      printf("%lf ", P[y][x]);
      #endif
    }
    #ifdef COOC_VERBOSE
    printf("\n");
    #endif
  }
  #ifdef COOC_VERBOSE
  printf("=====================\n");
  #endif
}

/************************************************************************
 * 
 Apenas ordena os vertices da selecao de um box em ordem dos eixos X/Y
 ************************************************************************/
void cooc_boxOrderVertex(int* w0, int* w1, int* h0, int* h1)
{
  int temp;

  if(*h0>*h1)
  {
    temp = *h0;
    *h0 = *h1;
    *h1 = temp;
  }
  if(*w0>*w1)
  {
    temp = *w0;
    *w0 = *w1;
    *w1 = temp;
  }
}

/************************************************************************
 *       
 Energia(Segundo momento angular)

 Esta medida avalia a uniformidade textural, que é a repetição de pares de níveis de
 cinza. Quando a área de interesse apresenta textura uniforme (valores de níveis de cinza
 próximos) o valor de energia tende para 1. Caso a área não seja uniforme o valor da
 energia tende a 0 (zero).

 Mede a uniformidade da imagem

 ************************************************************************/
float cooc_energy(float** P, int ng)
{
  int x,y;
  float val = 0.0f;

  for(y=0; y<ng ;y++)
    for(x=0; x<ng ;x++)
      val += (float)pow(P[y][x], 2.0f);

  #ifdef COOC_VERBOSE
  printf("Energy: %lf\n", val);
  #endif

  return val;
}

/************************************************************************
 *                             
  Entropia

 Mede a desordem em uma imagem. Quando a imagem não apresenta textura uniforme,
 os valores da entropia tendem a ser muito baixos. A entropia alcança seu valor máximo
 quando os pixels na área de interesse apresentam níveis de cinza com valores aleatórios.
 A entropia apresenta uma correlação linear negativa com a energia e não é
 correlacionada com a medida de correlação.

 Mede a quantidade de variação local da imagem

 ************************************************************************/
float cooc_entropy(float** P, int ng)
{
  int x,y;
  float val = 0.0f;

  for(y=0; y<ng ;y++)
    for(x=0; x<ng ;x++)
    {
      if(P[y][x] <= 0)
        val = 0.0f;
      else
        val += P[y][x] * (float)log(P[y][x]);
    }
      

  #ifdef COOC_VERBOSE
  printf("Entropy: %lf\n", -val);
  #endif

  return -val;
}

/************************************************************************
*                             
Probabilidade maxima na matriz

************************************************************************/
float cooc_maxProbability(float** P, int ng)
{
  int x,y;
  float val = 0.0f;

  for(y=0; y<ng ;y++)
    for(x=0; x<ng ;x++)
      if(P[y][x] > val)
        val = P[y][x]; 

  #ifdef COOC_VERBOSE
  printf("Max Probability: %lf\n", val);
  #endif

  return val;
}
/************************************************************************
 *     
 Contraste

 O contraste mede a presença de transição abrupta de níveis de cinza, isto é, as bordas.
 Baixos valores indicam a ausência de bordas na área de interesse. Neste caso, a matriz
 de co-ocorrência de níveis de cinza apresenta valores concentrados em torno de sua
 diagonal principal.

 Mede a quantidade de variação local da imagem

 ************************************************************************/
float cooc_contrast(float** P, int ng)
{
  int x,y;
  float val = 0.0f;

  for(y=0; y<ng ;y++)
    for(x=0; x<ng ;x++)
      val += P[y][x] * (float)pow((y - x),2);
    
  #ifdef COOC_VERBOSE
  printf("Contrast: %lf\n", val);
  #endif

  return val;
}

/************************************************************************
 *         
 Inverse difference moment
 (Homogeneidade local)

 Este parâmetro, também chamado de Momento da Diferença Inversa (Baraldi e
 Permiggiani, 1995), mede a homogeneidade da imagem e assume valores grandes
 quando os pares de pixels, na área de interesse, têm pequena diferença de níveis de
 cinza. A homogeneidade apresenta uma correlação negativa com o contraste.

 Mede a homogeneidade da imagem

 ************************************************************************/
float cooc_IDM(float** P, int ng)
{
  int x,y;
  float val = 0.0f;

  for(y=0; y<ng ;y++)
    for(x=0; x<ng ;x++)
    {
      val += P[y][x] / (1+(float)pow((y - x),2));
    }

    #ifdef COOC_VERBOSE
    printf("Inverse difference moment: %lf\n", val);
    #endif

    return val;
}

/************************************************************************
*                                                                     
************************************************************************/
float cooc_mean(float** P, int ng)
{
  int x,y;
  float val = 0.0f;

  for(y=0; y<ng ;y++)
    for(x=0; x<ng ;x++)
    {
      val += x * P[y][x];
    }

  #ifdef COOC_VERBOSE
  printf("Mean: %lf\n", val);
  #endif

  return val;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
float cooc_mean_x(float** P, int ng)
{
  int x,y;
  float val = 0.0f;
  float temp = 0.0f;

  for(x=0; x<ng ;x++)
  {
    for(y=0; y<ng ;y++)
    {
      temp += P[y][x];
    }
     val += x * temp;
  }

  #ifdef COOC_VERBOSE
  printf("Mean X: %lf\n", val);
  #endif

  return val;
}

/************************************************************************
*                                                                     
************************************************************************/
float cooc_mean_y(float** P, int ng)
{
  int x,y;
  float val = 0.0f;
  float temp = 0.0f;

  for(y=0; y<ng ;y++)
  {
    for(x=0; x<ng ;x++)
    {
      temp += P[y][x];
    }
    val += y * temp;
  }

  #ifdef COOC_VERBOSE
  printf("Mean Y: %lf\n", val);
  #endif

  return val;
}

/************************************************************************
*                                                                     
************************************************************************/
float cooc_deviation_x(float** P, int ng)
{
  int x,y;
  float val = 0.0f;
  float temp = 0.0f;
  float meanX = 0.0f;

  meanX = cooc_mean_x(P,ng);

  for(x=0; x<ng ;x++)
  {
    for(y=0; y<ng ;y++)
      temp += P[y][x];

    val += (float)pow(x-meanX, 2)*temp;
  }

  val = (float)sqrt(val);

  #ifdef COOC_VERBOSE
  printf("Std Deviation X: %lf\n", val);
  #endif

  return val;
}

/************************************************************************
*                                                                     
************************************************************************/
float cooc_deviation_y(float** P, int ng)
{
  int x,y;
  float val = 0.0f;
  float temp = 0.0f;
  float meanY = 0.0f;

  meanY = cooc_mean_y(P,ng);

  for(y=0; y<ng ;y++)
  {
    for(x=0; x<ng ;x++)
      temp += P[y][x];

    val += (float)pow(y-meanY, 2)*temp;
  }

  val = (float)sqrt(val);

  #ifdef COOC_VERBOSE
  printf("Std Deviation Y: %lf\n", val);
  #endif

  return val;
}


/************************************************************************
 *         
 Correlacao

 Mede a dependência linear entre os níveis de cinza de pares de pixels. Valores próximos
 a um implicam numa forte relação entre os níveis de cinza dos pixels. A medida
 correlação não é correlacionada com a energia, isto é, altos valores de correlação podem
 ser encontrados em baixos ou altos valores de energia, para a mesma área de interesse.

 Mede a dependência linear de um nível de cinza em relação aos seus vizinhos

 ************************************************************************/

float cooc_correlation(float** P, int ng)
{
  int x,y;
  float val = 0.0f;
  float meanX, meanY, desvX, desvY;

  meanX = cooc_mean_x(P,ng);
  meanY = cooc_mean_y(P,ng);
  desvX = cooc_deviation_x(P,ng);
  desvY = cooc_deviation_y(P,ng);

  for(y=0; y<ng ;y++)
    for(x=0; x<ng ;x++)
      if((desvX*desvY) > 0)
        val += (((x*y*P[y][x])-(meanX*meanY)) / (desvX*desvY));
    

  #ifdef COOC_VERBOSE
  printf("Correlation: %lf\n", val);
  #endif

  return val;
}

/************************************************************************
*      
Variancia

Mede a dispersão em relação à média
************************************************************************/
float cooc_variance(float** P, int ng)
{
  int x,y;
  float val = 0.0f;
  float mean;

  mean = cooc_mean(P,ng);

  for(y=0; y<ng ;y++)
    for(x=0; x<ng ;x++)
    {
      val += (float) pow(x-mean,2)* P[y][x];
    }


  #ifdef COOC_VERBOSE
  printf("Variance: %lf\n", val);
  #endif

  return val;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
float cooc_dissimilarity(float** P, int ng)
{
  int x,y;
  float val = 0.0f;
  float mean;

  mean = cooc_mean(P,ng);

  for(y=0; y<ng ;y++)
    for(x=0; x<ng ;x++)
    {
      val += (y-x)* P[y][x];
    }


    #ifdef COOC_VERBOSE
    printf("Dissimilarity: %lf\n", val);
    #endif

    return val;
}