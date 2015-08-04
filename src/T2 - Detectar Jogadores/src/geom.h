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

/* Inclus�o do modulo de defini��o */

#ifndef _GEOM_H_
#define _GEOM_H_

#if defined(__cplusplus)
extern "C" {
#endif

/* Defini��o da estrutura de ponto 2D. */
typedef struct {
 double x;/*!< Coordenada x do ponto. */
 double y;/*!< Coordenada y do ponto. */
 float mask; //Mascara q informa se o ponto eh visivel ou nao
} GeomPoint2D;

/* Defini��o da estrutura de pol�gono 2D. */
typedef struct {
 int np;/*!< N�mero de v�rtices(pontos que delimitam) do pol�gono.*/
 GeomPoint2D *pol; /* Vetor de pontos */
 GeomPoint2D min; /*!< Ponto m�nimo. */
 GeomPoint2D max; /*!< Ponto m�ximo. */
 int fechado; /*!< Informa se o pol�gono � aberto ou fechado. */
} GeomPol2D;

/* Defini��o da estrutura de ponto 3D. */
typedef struct {
 double x;/*!< Coordenada x do ponto. */
 double y;/*!< Coordenada y do ponto. */
 float z; /*!< Coordenada z do ponto. */
} GeomPoint3D;

/* Toler�ncia. */
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
  * Dados dois vetores, calcula a dist�ncia euclideana entre eles.
  * @param p1 Primeiro vetor.
  * @param p2 Segundo vetor.
  * @return Dist�ncia euclideana.
  */
#define geomDistance(p1,p2) sqrt(((p1).x-(p2).x)*((p1).x-(p2).x)+((p1).y-(p2).y)*((p1).y-(p2).y))




/**
  * Dados dois vetores, calcula o quadrado da dist�ncia euclideana entre eles.
  * @param p1 Primeiro vetor.
  * @param p2 Segundo vetor.
  * @return Quadrado da dist�ncia euclideana.
  */
#define geomDistance2(p1,p2) (((p1).x-(p2).x)*((p1).x-(p2).x)+((p1).y-(p2).y)*((p1).y-(p2).y))




/**
  * Calcula o pseudo-�ngulo, tendo recebido as duas coordenadas (x e y).
  * @param x Coordenada x.
  * @param y Coordenada y.
  * @return Retorna o pseudo-�ngulo.
  */
#define geomPAngle(x,y) ((ABS((x))<GEOM_ABSTOL && ABS((y))<GEOM_ABSTOL)? 0.0 : ((y)>=0.0 && (y)<=(x))? (y)/(x) : ((y)>ABS((x)))? 2.0-(x)/(y) : ((x)<0 && ABS((y))<=ABS((x)))? 4.0+(y)/(x) : ((y)<-ABS((x)))? 6.0-(x)/(y) : 8.0+(y)/(x))




/**
  * Fun��o que calcula o �ngulo. 
  * @param ang1 Primeiro �ngulo.
  * @param ang1 Segundo �ngulo.
  * @param ang Resultado retornado.
  */
#define geomAngle(ang,ang1,ang2) {double aux = ((ang2)<(ang1))? 8.0+((ang2)-(ang1)) : (ang2)-(ang1); (ang) += ((aux > 4.0)? aux - 8.0 : aux);}




/* Valor indicativo de ponto nulo. */
float GeomSetDummy(float geom_dummy);
float GeomGetDummy(void);



/**
  * Fun��o que retorna o fecho convexo (algoritmo de Jarvis) dado um conjunto 'X' de 'np' pontos. 
  * @param npP N�mero de pontos de entrada.
  * @param P Pontos de entrada.
  * @return Retorna um conjunto de pontos que delimitam(s�o os v�rtices) o pol�gono.
  */
GeomPol2D* GeomGetOutline(int npP, GeomPoint3D *P);




/**
  * Fun��o que verifica se o ponto 'p' encontra-se no interior do pol�gono 'P'. 
  * @param P Pol�gono no qual se quer verificar se o ponto � interno.
  * @param p Ponto que se deseja verificar ser ou n�o interno.
  * @return 0 indica que o ponto � externo, e 1 indica que � interno ao pol�gono.
  */
int GeomIn(GeomPol2D *P, GeomPoint2D p);




/**
  * Fun��o que verifica se o ponto 'p' encontra-se no interior do pol�gono fechado 'P'. 
  * @param P Pol�gono no qual se quer verificar se o ponto � interno.
  * @param p Ponto que se deseja verificar ser ou n�o interno.
  * @return 0 indica que o ponto � externo, e 1 indica que � interno ao pol�gono.
  */
int GeomInClosePol(GeomPol2D *P, GeomPoint2D p);




/**
  * Fun��o que verifica se um segmento de extremidades 'x0' e 'X' cruza o pol�gono 'P' de 'npol' pontos. 
  * @param x0 Primeira extremidade do segmento.
  * @param X Segunda extremidade do segmento.
  * @param npol N�mero de pontos(v�rtices) que definem o pol�gono.
  * @param P Pol�gono. 
  * @return 0 indica que o segmento n�o cruza o pol�gono, e 1 indica que cruza o pol�gono.
  */
  int GeomCross(GeomPoint2D x0, GeomPoint3D X, int npol, GeomPol2D *P);




/**
 * Fun��o que simplifica(diminui) o n�mero de pontos, a partir do centr�ide dos pontos contidos em cada c�lula.
 * @param npPIn N�mero de pontos de entrada.
 * @param PIn Pontos de entrada.
 * @param 0x Origem do grid em x.                 
 * @param 0y Origem do grid em y.                 
 * @param nx N�mero de elementos do grid em x.    
 * @param ny N�mero de elementos do grid em y.    
 * @param cx Espa�amento do grid em x.            
 * @param cy Espa�amento do grid em y.            
 * @param npPOut N�mero de pontos de sa�da.
 * @param PIn Pontos de sa�da.
 */
void GeomSimplifyPts(int npPIn, GeomPoint3D *PIn,
                     double ox, double oy, int nx, int ny, double cx, double cy,
                     int *npPOut, GeomPoint3D **POut);




/**
 * Fun��o que gera um grid prim�rio a partir da m�dia ponderada pelo inverso da dist�ncia dos pontos pr�ximos. 
 * @param npP N�mero de pontos de entrada.
 * @param P Pontos de entrada.
 * @param 0x Origem do grid em x.                 
 * @param 0y Origem do grid em y.                 
 * @param nx N�mero de elementos do grid em x.    
 * @param ny N�mero de elementos do grid em y.    
 * @param cx Espa�amento do grid em x.            
 * @param cy Espa�amento do grid em y.
 * @param zaux Coordenadas z que s�o geradas pelo m�todo para cada um dos pontos de entrada.
 * @param naux Para cada um dos z gerados, retorna se � um ponto v�lido ou n�o(1 ou 0).
 */
void GeomCreateGrid1(int npP, GeomPoint3D *P,
                     double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux);




/**
 * Fun��o que gera um grid prim�rio a partir dos pontos mais pr�ximos a cada n� do grid. 
 * @param npP N�mero de pontos de entrada.
 * @param P Pontos de entrada.
 * @param 0x Origem do grid em x.                 
 * @param 0y Origem do grid em y.                 
 * @param nx N�mero de elementos do grid em x.    
 * @param ny N�mero de elementos do grid em y.    
 * @param cx Espa�amento do grid em x.            
 * @param cy Espa�amento do grid em y.
 * @param zaux Coordenadas z que s�o geradas pelo m�todo para cada um dos pontos de entrada.
 * @param naux Para cada um dos z gerados, retorna se � um ponto v�lido ou n�o(1 ou 0).
 */
void GeomCreateGrid2(int npP, GeomPoint3D *P,
                     double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux);



/**
 * Fun��o que gera um grid prim�rio a partir dos pontos mais pr�ximos a cada n� do grid. 
 * @param npP N�mero de pontos de entrada.
 * @param P Pontos de entrada.
 * @param 0x Origem do grid em x.                 
 * @param 0y Origem do grid em y.                 
 * @param nx N�mero de elementos do grid em x.    
 * @param ny N�mero de elementos do grid em y.    
 * @param cx Espa�amento do grid em x.            
 * @param cy Espa�amento do grid em y.
 * @param zaux Coordenadas z que s�o geradas pelo m�todo para cada um dos pontos de entrada.
 * @param naux Para cada um dos z gerados, retorna se � um ponto v�lido ou n�o(1 ou 0).
 * @param distprox Dist�ncia.
 */
void GeomCreateGrid2Near(int npP, GeomPoint3D *P,
                         double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux,
                         double distprox);



/**
 * Fun��o que gera um grid prim�rio a partir dos pontos mais pr�ximos a cada n� do grid. 
 * @param npP N�mero de pontos de entrada.
 * @param P Pontos de entrada.
 * @param 0x Origem do grid em x.                 
 * @param 0y Origem do grid em y.                 
 * @param nx N�mero de elementos do grid em x.    
 * @param ny N�mero de elementos do grid em y.    
 * @param cx Espa�amento do grid em x.            
 * @param cy Espa�amento do grid em y.
 * @param zaux Coordenadas z que s�o geradas pelo m�todo para cada um dos pontos de entrada.
 * @param naux Para cada um dos z gerados, retorna se � um ponto v�lido ou n�o(1 ou 0).
 * @param distprox Dist�ncia.
 */
void GeomCreateGrid2Outline(int npP, GeomPoint3D *P, GeomPol2D* outline,
                            double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux,
                            double distprox);

double GeomAreaPol(GeomPol2D pol);

#if defined(__cplusplus)
}
#endif

#endif
