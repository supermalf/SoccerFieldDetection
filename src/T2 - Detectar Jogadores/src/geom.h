/* *************************************************************
*
* Class:  geom.h
* Created: 21/4/2006 - 10:35
* Author: malf
* $Id: $
*
* Description: 
* 
*
* $Log: $
*
* *************************************************************/

/* Inclusão do modulo de definição */

#ifndef _GEOM_H_
#define _GEOM_H_

#if defined(__cplusplus)
extern "C" {
#endif

/* Definição da estrutura de ponto 2D. */
typedef struct {
 double x;/*!< Coordenada x do ponto. */
 double y;/*!< Coordenada y do ponto. */
 float mask; //Mascara q informa se o ponto eh visivel ou nao
} GeomPoint2D;

/* Definição da estrutura de polígono 2D. */
typedef struct {
 int np;/*!< Número de vértices(pontos que delimitam) do polígono.*/
 GeomPoint2D *pol; /* Vetor de pontos */
 GeomPoint2D min; /*!< Ponto mínimo. */
 GeomPoint2D max; /*!< Ponto máximo. */
 int fechado; /*!< Informa se o polígono é aberto ou fechado. */
} GeomPol2D;

/* Definição da estrutura de ponto 3D. */
typedef struct {
 double x;/*!< Coordenada x do ponto. */
 double y;/*!< Coordenada y do ponto. */
 float z; /*!< Coordenada z do ponto. */
} GeomPoint3D;

/* Tolerância. */
#define GEOM_ABSTOL 1e-10


/**
  * Calcula a norma de um vetor.
  * @param v Vetor a ter sua norma calculada.
  * @return Norma do vetor calculada.
  */
#define geomNorm(v) sqrt((v).x*(v).x + (v).y*(v).y)

  
  
  
/**
  * Calcula a o quadrado da norma de um vetor.
  * @param v Vetor a ter o quadrado de sua norma calculada.
  * @return Quadrado da norma do vetor calculado.
  */
#define geomNorm2(v) ((v).x*(v).x + (v).y*(v).y)




/**
  * Dados dois vetores, calcula a distância euclideana entre eles.
  * @param p1 Primeiro vetor.
  * @param p2 Segundo vetor.
  * @return Distância euclideana.
  */
#define geomDistance(p1,p2) sqrt(((p1).x-(p2).x)*((p1).x-(p2).x)+((p1).y-(p2).y)*((p1).y-(p2).y))




/**
  * Dados dois vetores, calcula o quadrado da distância euclideana entre eles.
  * @param p1 Primeiro vetor.
  * @param p2 Segundo vetor.
  * @return Quadrado da distância euclideana.
  */
#define geomDistance2(p1,p2) (((p1).x-(p2).x)*((p1).x-(p2).x)+((p1).y-(p2).y)*((p1).y-(p2).y))




/**
  * Calcula o pseudo-ângulo, tendo recebido as duas coordenadas (x e y).
  * @param x Coordenada x.
  * @param y Coordenada y.
  * @return Retorna o pseudo-ângulo.
  */
#define geomPAngle(x,y) ((ABS((x))<GEOM_ABSTOL && ABS((y))<GEOM_ABSTOL)? 0.0 : ((y)>=0.0 && (y)<=(x))? (y)/(x) : ((y)>ABS((x)))? 2.0-(x)/(y) : ((x)<0 && ABS((y))<=ABS((x)))? 4.0+(y)/(x) : ((y)<-ABS((x)))? 6.0-(x)/(y) : 8.0+(y)/(x))




/**
  * Função que calcula o ângulo. 
  * @param ang1 Primeiro ângulo.
  * @param ang1 Segundo ângulo.
  * @param ang Resultado retornado.
  */
#define geomAngle(ang,ang1,ang2) {double aux = ((ang2)<(ang1))? 8.0+((ang2)-(ang1)) : (ang2)-(ang1); (ang) += ((aux > 4.0)? aux - 8.0 : aux);}




/* Valor indicativo de ponto nulo. */
float GeomSetDummy(float geom_dummy);
float GeomGetDummy(void);



/**
  * Função que retorna o fecho convexo (algoritmo de Jarvis) dado um conjunto 'X' de 'np' pontos. 
  * @param npP Número de pontos de entrada.
  * @param P Pontos de entrada.
  * @return Retorna um conjunto de pontos que delimitam(são os vértices) o polígono.
  */
GeomPol2D* GeomGetOutline(int npP, GeomPoint3D *P);




/**
  * Função que verifica se o ponto 'p' encontra-se no interior do polígono 'P'. 
  * @param P Polígono no qual se quer verificar se o ponto é interno.
  * @param p Ponto que se deseja verificar ser ou não interno.
  * @return 0 indica que o ponto é externo, e 1 indica que é interno ao polígono.
  */
int GeomIn(GeomPol2D *P, GeomPoint2D p);




/**
  * Função que verifica se o ponto 'p' encontra-se no interior do polígono fechado 'P'. 
  * @param P Polígono no qual se quer verificar se o ponto é interno.
  * @param p Ponto que se deseja verificar ser ou não interno.
  * @return 0 indica que o ponto é externo, e 1 indica que é interno ao polígono.
  */
int GeomInClosePol(GeomPol2D *P, GeomPoint2D p);




/**
  * Função que verifica se um segmento de extremidades 'x0' e 'X' cruza o polígono 'P' de 'npol' pontos. 
  * @param x0 Primeira extremidade do segmento.
  * @param X Segunda extremidade do segmento.
  * @param npol Número de pontos(vértices) que definem o polígono.
  * @param P Polígono. 
  * @return 0 indica que o segmento não cruza o polígono, e 1 indica que cruza o polígono.
  */
  int GeomCross(GeomPoint2D x0, GeomPoint3D X, int npol, GeomPol2D *P);




/**
 * Função que simplifica(diminui) o número de pontos, a partir do centróide dos pontos contidos em cada célula.
 * @param npPIn Número de pontos de entrada.
 * @param PIn Pontos de entrada.
 * @param 0x Origem do grid em x.                 
 * @param 0y Origem do grid em y.                 
 * @param nx Número de elementos do grid em x.    
 * @param ny Número de elementos do grid em y.    
 * @param cx Espaçamento do grid em x.            
 * @param cy Espaçamento do grid em y.            
 * @param npPOut Número de pontos de saída.
 * @param PIn Pontos de saída.
 */
void GeomSimplifyPts(int npPIn, GeomPoint3D *PIn,
                     double ox, double oy, int nx, int ny, double cx, double cy,
                     int *npPOut, GeomPoint3D **POut);




/**
 * Função que gera um grid primário a partir da média ponderada pelo inverso da distância dos pontos próximos. 
 * @param npP Número de pontos de entrada.
 * @param P Pontos de entrada.
 * @param 0x Origem do grid em x.                 
 * @param 0y Origem do grid em y.                 
 * @param nx Número de elementos do grid em x.    
 * @param ny Número de elementos do grid em y.    
 * @param cx Espaçamento do grid em x.            
 * @param cy Espaçamento do grid em y.
 * @param zaux Coordenadas z que são geradas pelo método para cada um dos pontos de entrada.
 * @param naux Para cada um dos z gerados, retorna se é um ponto válido ou não(1 ou 0).
 */
void GeomCreateGrid1(int npP, GeomPoint3D *P,
                     double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux);




/**
 * Função que gera um grid primário a partir dos pontos mais próximos a cada nó do grid. 
 * @param npP Número de pontos de entrada.
 * @param P Pontos de entrada.
 * @param 0x Origem do grid em x.                 
 * @param 0y Origem do grid em y.                 
 * @param nx Número de elementos do grid em x.    
 * @param ny Número de elementos do grid em y.    
 * @param cx Espaçamento do grid em x.            
 * @param cy Espaçamento do grid em y.
 * @param zaux Coordenadas z que são geradas pelo método para cada um dos pontos de entrada.
 * @param naux Para cada um dos z gerados, retorna se é um ponto válido ou não(1 ou 0).
 */
void GeomCreateGrid2(int npP, GeomPoint3D *P,
                     double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux);



/**
 * Função que gera um grid primário a partir dos pontos mais próximos a cada nó do grid. 
 * @param npP Número de pontos de entrada.
 * @param P Pontos de entrada.
 * @param 0x Origem do grid em x.                 
 * @param 0y Origem do grid em y.                 
 * @param nx Número de elementos do grid em x.    
 * @param ny Número de elementos do grid em y.    
 * @param cx Espaçamento do grid em x.            
 * @param cy Espaçamento do grid em y.
 * @param zaux Coordenadas z que são geradas pelo método para cada um dos pontos de entrada.
 * @param naux Para cada um dos z gerados, retorna se é um ponto válido ou não(1 ou 0).
 * @param distprox Distância.
 */
void GeomCreateGrid2Near(int npP, GeomPoint3D *P,
                         double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux,
                         double distprox);



/**
 * Função que gera um grid primário a partir dos pontos mais próximos a cada nó do grid. 
 * @param npP Número de pontos de entrada.
 * @param P Pontos de entrada.
 * @param 0x Origem do grid em x.                 
 * @param 0y Origem do grid em y.                 
 * @param nx Número de elementos do grid em x.    
 * @param ny Número de elementos do grid em y.    
 * @param cx Espaçamento do grid em x.            
 * @param cy Espaçamento do grid em y.
 * @param zaux Coordenadas z que são geradas pelo método para cada um dos pontos de entrada.
 * @param naux Para cada um dos z gerados, retorna se é um ponto válido ou não(1 ou 0).
 * @param distprox Distância.
 */
void GeomCreateGrid2Outline(int npP, GeomPoint3D *P, GeomPol2D* outline,
                            double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux,
                            double distprox);

double GeomAreaPol(GeomPol2D pol);

#if defined(__cplusplus)
}
#endif

#endif
