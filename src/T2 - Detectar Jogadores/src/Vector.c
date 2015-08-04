#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Vector.h"

/**
* Cria um novo vetor. Recebe o ponteiro para os seus valores e a dimensão.
*/
int NewVector( int dim, float* values, ptMVector *New_Vector )
{
  ptMVector auxVector = (ptMVector)malloc(sizeof(MVector));

  // Casos de retorno:
  if ( values == NULL )
    return -1;

  auxVector->dim = dim;
  auxVector->values = (float*)values;
  *New_Vector = auxVector;

  return 0;
}


/**
* Cria um novo vetor. Recebe a dimensão.
*/
int NewEmptyVector( int dim, ptMVector *New_Vector )
{
  ptMVector auxVector = (ptMVector)malloc(sizeof(MVector));

  // Casos de retorno:
  if ( dim == 0 ) 
    return -1;

  // Inserindo a dimensão e o espaço para os pesos:
  auxVector->dim = dim;
  auxVector->values = (float*)calloc(dim, sizeof(float));
  // Retornando o vetor criado:
  *New_Vector = auxVector;
  return 0;
}


/**
  * Cria uma cópia de um vetor ja existente.
  */
void CopyVector( ptMVector vec, ptMVector *new_vec)
{
  int i;
  ptMVector auxVector = (ptMVector)malloc(sizeof(MVector));
  
  auxVector->dim = vec->dim;
  auxVector->values = (float*)malloc(vec->dim*sizeof(float));
  for( i=0 ; i<vec->dim ; i++ )
    (float)auxVector->values[i] = (float)vec->values[i];

  // Retornando o novo vetor:
  *new_vec = auxVector;

}

 
/**
  * Insere novos valores a um vetor já existente.
  */
int InsertValues( ptMVector vec, float * values, int dim )
{
  int i;

  // Casos de retorno:
  if ((values == NULL ) || ( dim != vec->dim ))
    return -1;

  // Inserindo cada um dos valores:
  for( i=0 ; i<vec->dim ; i++ )
    vec->values[i] = (float)values[i];
  return 0;
}


/**
* Normaliza de um vetor.
*/
void Normalize( ptMVector vec )
{
  int i;
  float temp = 0;

  for( i=0 ; i<vec->dim ; i++ )
    temp+= (float)(vec->values[i]*vec->values[i]);
  if (temp > 0)
  {
    temp = (float)sqrt(temp);
    for( i=0 ; i<vec->dim ; i++ )
      vec->values[i] = (float)(vec->values[i]/temp);
  }
}


/**
* Obtém uma versão do vetor com todas as componentes dentro de um intervalo pré-definido.
*/
void IntervalScaleIn( ptMVector vec, float maxScale, float minScale )
{
  float scale; 
  int i;
  float minValue = MAX_VALUE;
  float maxValue = MIN_VALUE;


  /* Obtém o maior e o menor valores dentre das componentes do vetor: */
  for( i=0 ; i<vec->dim ; i++ )
  {
    if( vec->values[i] < minValue )
      minValue = vec->values[i];
    if( vec->values[i] > maxValue )
      maxValue = vec->values[i];
  }

  /* Escala a ser aplicada: */
  scale   = (float)((maxScale - minScale)/(maxValue - minValue));
  
  /* Aplicando a escala ao vetor: */
  for( i=0 ; i<vec->dim ; i++ )
    vec->values[i] = (vec->values[i]-minValue)*scale + minScale;
}


/**
* Desfaz uma escala previamente aplicada a um vetor, restaurando os valores originais das componentes.
*/
void IntervalScaleOut( ptMVector vec, float maxValue, float minValue )
{
  int i;
  float scale; 
  float minScale = MAX_VALUE;
  float maxScale = MIN_VALUE;

  /* Obtém o maior e o menor valores dentre das componentes do vetor: */
  for( i=0 ; i<vec->dim ; i++ )
  {
    if( vec->values[i] < minScale )
      minScale = vec->values[i];
    if( vec->values[i] > maxScale )
      maxScale = vec->values[i];
  }

  /* Escala a ser aplicada: */
  scale   = (float)((maxScale - minScale)/(maxValue - minValue));

  /* Aplicando a escala ao vetor: */
  for( i=0 ; i<vec->dim ; i++ )
    vec->values[i] = (vec->values[i]-minScale)/scale + minValue;
}


/**
  * Multiplicação de um vetor por um escalar.
  */
void EscalarMult( ptMVector vec, float escNum )
{
  int i;

  for( i=0 ; i<vec->dim ; i++ )
    vec->values[i] = vec->values[i]* escNum;
}


/**
* Soma dos componentes de dois vetores e retorna um novo vetor com a soma.
*/
ptMVector AddRetNew( ptMVector vec1, ptMVector vec2 )
{
  int i;
  ptMVector ret_vec = (ptMVector)malloc(sizeof(MVector));
  // Casos de retorno:
  if( (vec1 == NULL) || (vec2 == NULL) || (vec1->dim != vec2->dim) )
    return NULL;
  // Alocando o novo vetor a ser retornado:
  ret_vec = (ptMVector)malloc(sizeof(MVector));

  // Somando as componentes do novo vetor:
  for( i=0 ; i<vec2->dim ; i++)
    ret_vec->values[i] = vec1->values[i] + vec2->values[i];

  // O novo vetor criado é retornado:
  return ret_vec;
}


/**
* Soma dos componentes de dois vetores e retorna a soma no primeiro vetor.
*/
void Add( ptMVector vec1, ptMVector vec2 )
{
  int i;
  // Casos de retorno:
  if( (vec1 == NULL) || (vec2 == NULL) || (vec1->dim != vec2->dim) )
    return;
  // Somando as componentes do vetor:
  for( i=0 ; i<vec2->dim ; i++)
    vec1->values[i] = vec1->values[i] + vec2->values[i];
}


/**
* Retorna o quadrado da distância de dois vetores.
*/
float QuadDist( ptMVector vec1, ptMVector vec2 )
{
  int i;
  float temp = 0;
  // Casos de retorno:
  if( ((vec1->dim != vec2->dim) && (vec1->values == NULL)) && (vec1->values == NULL) )
    return 0;
  for( i=0 ; i<vec1->dim ; i++ )
    temp+= (float)( (vec1->values[i]-vec2->values[i])*(vec1->values[i]-vec2->values[i]) );
  return temp;
}


/**
* Retorna o quadrado da distância de dois vetores, porém com pesos para cada uma das entradas.
*/
float QuadDistWeight( ptMVector vec1, ptMVector vec2, float *weights )
{
  int i;
  float temp = 0;
  // Casos de retorno:
  if( ((vec1->dim != vec2->dim) && (vec1->values == NULL)) && (vec1->values == NULL) )
    return 0;
  for( i=0 ; i<vec1->dim ; i++ )
    temp+= weights[i]*(float)( (vec1->values[i]-vec2->values[i])*(vec1->values[i]-vec2->values[i]) );
  return temp;
}



/**
* Retorna a distância Euclideana.
*/
float EuclideanDist( ptMVector vec1, ptMVector vec2 )
{
  int i;
  float temp = 0;
  // Casos de retorno:
  if( ((vec1->dim != vec2->dim) && (vec1->values == NULL)) && (vec1->values == NULL) )
    return 0;
  for( i=0 ; i<vec1->dim ; i++ )
    temp+= (float)( (vec1->values[i]-vec2->values[i])*(vec1->values[i]-vec2->values[i]) );
  temp = (float)sqrt(temp);
  return temp;
}

