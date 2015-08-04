/**
 *	@file image.h Image: operacoes imagens em formato TGA.
 *
 *	@author 
 *          - Maira Noronha
 *          - Marcelo Gattass
 *
 *	@date
 *			Ultima Modificacao:		23 de Abril de 2003
 *
 *	@version 2.0
 */

#ifndef IMAGE_H
#define IMAGE_H


/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/
/**
 *   Imagem com um buffer rgb.
 */
struct Image_imp 
{
/** 
 * largura (width) em pixels
 */
  unsigned short width;
/**
 * altura (height) em pixels   
 */
  unsigned short height; 
/**
 * buffer RGB                  
 */
  unsigned char  *buf;              
};  

typedef struct Image_imp * Image;

/************************************************************************/
/* Funcoes Exportadas                                                   */
/************************************************************************/

Image   imageCreate (int w, int h);

Image imageLoad(char *filename);


#endif
