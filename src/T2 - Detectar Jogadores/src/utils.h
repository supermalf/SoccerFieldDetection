#ifndef _UTILS_H_
#define _UTILS_H_

#ifndef ABS
/**
  * Retorna o m�dulo.
  * @param a Valor do qual se quer obter o m�dulo.
  * @return Retorna o m�dulo do valor passado.
  */
#define ABS(a) (((a)<0)?-(a):(a))
#endif

#ifndef SQR
/**
  * Encontra a raiz quadrada do valor passado como par�metro.
  * @param a Valor do qual ser� calculada a raiz quadrada.
  * @return Retorna a raiz quadrada do valor passado.
  */
#define SQR(a) ((a)*(a))
#endif

#ifndef MAX
/**
  * Recebe dois valores e retorna o maior deles.
  * @param a Primeiro valor passado � fun��o.
  * @param b Segundo valor passado � fun��o.
  * @return Retorna o maior dos dois valores recebidos.
  */
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN
/**
  * Recebe dois valores e retorna o menor deles.
  * @param a Primeiro valor passado � fun��o.
  * @param b Segundo valor passado � fun��o.
  * @return Retorna o menor dos dois valores recebidos.
  */
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef SWAP
/**
  * Recebe duas vari�veis e troca os valores, isto �, o valor da primeira passa a ser o da segunda e vice-versa.
  * @param a Primeira vari�vel.
  * @param b Segunda vari�vel.
  */
#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}
#endif

#ifndef _PI_
#define _PI_ 3.1415926535897932384626433832795
#endif

#endif
