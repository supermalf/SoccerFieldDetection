#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <math.h>

/**********************************************************
*    Código inspirado na implementação original de        *
*    "NeuroEvolution of Augmenting Topologies (NEAT)"     *
*    www.cs.utexas.edu/users/kstanley/neat.html           *
**********************************************************/

#define MAX_VALUE  1e12
#define MIN_VALUE -1e12


/**
* Retorna um número inteiro aleatório n entre x e y (x <= n <= x).
* @param x Inteiro x.
* @param y Inteiro y.
* @return Número aleatório inteiro retornado.
*/
inline int RandInt(int x,int y) 
{
  return rand()%(y-x+1)+x; 
}


/**
* Retorna um número float aleatório entre 0 e 1.
* @return Número aleatório float retornado.
*/
inline double RandFloat()
{
  return (rand())/(RAND_MAX+1.0); 
}


/**
* Retorna um número booleano aleatório.
* @return Número aleatório booleano retornado.
*/
inline int RandBool()
{
  if (RandInt(0,1)) 
    return 1; 
  else 
    return 0;
}


/**
* Retorna um número n float aleatório -1 < n < 1.
* @return Número n float aleatório -1 < n < 1 retornado.
*/
inline double RandomClamped()
{
  return RandFloat() - RandFloat();
}


/**
* Recebe três números tipo double e restringe o valor do primeiro como 
* estando entre o segundo e o terceiro: min <= arg <= max.
* @param arg Número a ter seu valor restringido.
* @param min Valor mínimo da restrição.
* @param max Valor máximo da restrição.
*/
inline void Clamp(double *arg, double min, double max)
{
  if (*arg < min)
    *arg = min;
  if (*arg > max)
    *arg = max;
}


/**
* Arredonda o valor recebido para o inteiro mais próximo.
* @param val Valor em ponto flutuante a ter o valor arredondado.
* @return Retorna o valor do inteiro mais próximo.
*/
inline int Rounded(double val)
{
  int    integral = (int)val;
  double mantissa = val - integral;
  if (mantissa < 0.5)
    return integral;
  else
    return integral + 1;
}


/**
* Arredonda o valor recebido para o inteiro mais próximo,
* dependendo de o valor da mantissa ser maior ou menor que o offset.
* @param val Valor em ponto flutuante a ter o valor arredondado.
* @param offset Offset a ter seu valor comparado com a mantissa.
* @return Retorna o valor do inteiro mais próximo, seguindo a restrição.
*/
inline int RoundUnderOffset(double val, double offset)
{
  int    integral = (int)val;
  double mantissa = val - integral;
  if (mantissa < offset)
    return integral;
  else
    return integral + 1;
}

/**
* Obtém uma versão do vetor com todas as componentes dentro de um intervalo pré-definido.
*/
inline void IntervalScaleInFloatVector( float *values, int dim, float maxScale, float minScale )
{
  float scale; 
  int i;
  float minValue = (float)MAX_VALUE;
  float maxValue = (float)MIN_VALUE;


  /* Obtém o maior e o menor valores dentre das componentes do vetor: */
  for( i=0 ; i<dim ; i++ )
  {
    if( values[i] < minValue )
      minValue = values[i];
    if( values[i] > maxValue )
      maxValue = values[i];
  }

  /* Escala a ser aplicada: */
  scale   = (float)((maxScale - minScale)/(maxValue - minValue));

  /* Aplicando a escala ao vetor: */
  for( i=0 ; i<dim ; i++ )
    values[i] = (values[i]-minValue)*scale + minScale;
}


/**
* Obtém uma versão do vetor com todas as componentes dentro de um intervalo entre 0.1 e 0.9.
* Esse Vetor deve possuir todas as suas componentes entre 0 e 1.
*/
inline void IntervalScaleInFloat_Zero_One_Vector( float *values, int dim )
{
  float scale; 
  int i;
  /* Aplicando a escala ao vetor: */
  for( i=0 ; i<dim ; i++ )
    values[i] = 0.1f + values[i]*0.8;
}


/**
* Função que normaliza um vetor de floats.
*/
inline int NormalizeFloatVector( float *values, int dim )
{
  if( !values )
    return -1;
  float norm=0;
  // Norma:
  for( int i=0 ; i<dim ; i++ )
    norm+= values[i] * values[i];
  norm = (float)sqrt(norm);
  for( int i=0 ; i<dim ; i++ )
    values[i] = values[i]/dim;
  return 0;
}


/**
* Retorna o maior entre dois números.
* @param a Primeiro número. 
* @param b Segundo número.
* @return Retorna o maior entre os dois números recebidos.
*/
#define Greater(a, b) ((a>=b)?(a):(b))


/**
* Retorna o menor entre dois números.
* @param a Primeiro número. 
* @param b Segundo número.
* @return Retorna o menor entre os dois números recebidos.
*/
#define Minor(a, b) ((a<=b)?(a):(b))


#endif