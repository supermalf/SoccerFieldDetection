

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "Util.h"

#define MAX_VALUE  1e8
#define MIN_VALUE -1e8

/**
  * Vetor de dimens�o arbitr�ria.
  */
typedef struct mvector
{
  int dim;              ///< Dimens�o (n�mero de componetes).
  float *values;        ///< Valores.
} MVector, *ptMVector;


/**
* Cria um novo vetor. Recebe o ponteiro para os seus valores e a dimens�o.
* @param dim Dimens�o do vetor a ser criado.
* @param values Valores a serem inseridos.
* @param New_Vector Ponteiro para o novo vetor.
* @return Retorna -1 em caso de erro, ou 0 caso ok.
*/
int NewVector( int dim, float* values, ptMVector *New_Vector );


/**
* Cria um novo vetor. Recebe a dimens�o.
* @param dim Dimens�o do vetor a ser criado.
* @param New_Vector Ponteiro para o novo vetor.
* @return Retorna -1 em caso de erro, ou 0 caso ok.
*/
int NewEmptyVector( int dim, ptMVector *New_Vector );


/**
* Cria uma c�pia de um vetor ja existente.
* @param vec Vetor a ser copiado.
* @param new_vec C�pia do vetor retornada.
* @return Retorna -1 em caso de erro, ou 0 caso ok.
*/
void CopyVector( ptMVector vec, ptMVector *new_vec);


/**
* Insere novos valores a um vetor j� existente.
* @param vec Vetor onde os novos valores ser�o inseridos.
* @param values Valores (internamente os valores ser�o copiados um a um).
* @param dim Dimens�o do vetor a ser inserido.
* @return Retorna -1 em caso de erro, ou 0 caso ok.
*/
int InsertValues( ptMVector vec, float *values, int dim );


/**
  * Normaliza um vetor.
  * @param vec Vetor a ser normalizado.
  */
void Normalize( ptMVector vec );


/**
* Obt�m uma escala do vetor com todas as componentes dentro de um intervalo pr�-definido.
* @param vec Vetor a ter os valores escalonados.
* @param maxScale Valor m�ximo do escalonamento.
* @param minScale Valor m�nimo do escalonamento.
*/
void IntervalScaleIn( ptMVector vec, float maxScale, float minScale );


/**
* Desfaz uma escala previamente aplicada a um vetor, restaurando os valores originais das componentes.
* @param vec Vetor a ter os valores escalonados.
* @param maxValue
* @param minValue
*/
void IntervalScaleOut( ptMVector vec, float maxValue, float minValue );


/**
* Multiplica��o de um vetor por um escalar.
* @param vec Vetor a ter os componentes multiplicados.
* @param escNum Escalar pelo qual os componentes do vetor ser�o multiplicado.
*/
void EscalarMult( ptMVector vec, float escNum );


/**
* Soma dos componentes de dois vetores e retorna um novo vetor com a soma.
*/
ptMVector AddRetNew( ptMVector vec1, ptMVector vec2 );


/**
* Soma dos componentes de dois vetores e retorna a soma no primeiro vetor.
*/
void Add( ptMVector vec1, ptMVector vec2 );


/**
* Retorna o quadrado da dist�ncia de dois vetores.
*/
float QuadDist( ptMVector vec1, ptMVector vec2 );


/**
* Retorna o quadrado da dist�ncia de dois vetores, por�m com pesos para cada uma das entradas.
*/
float QuadDistWeight( ptMVector vec1, ptMVector vec2, float *weights );


/**
  * Retorna a dist�ncia Euclideana.
  */
float EuclideanDist( ptMVector vec1, ptMVector vec2 );

#endif

