#ifndef _MULTIBANDIMAGE_H_
#define _MULTIBANDIMAGE_H_

#include "mc920.h"

/*Agregado por Pablo Fonseca */

typedef struct _bands {
  int *val;     /* Vector of Bands */
} Bands;

typedef struct _multibandImage {
  int   nx,ny;      /* dimensoes da imagem */
  int   nbands;     /* numero de bandas*/
  Bands **band;      /* matriz com as basdas dos pixels */
  float dx,dy;      /* tamanho do pixel em unidades de comprimento */
  char  unid[10];   /* unidade de comprimento */
} MultibandImage;

MultibandImage   *CreateMultibandImage(int nx, int ny, int nbands);
void              DestroyMultibandImage(MultibandImage **I);
MultibandImage   *ReadMultibandImage(char *filename);
void              WriteMultibandImage(MultibandImage *I, char *filename);
/*Fin de Agregado por Pablo Fonseca */

#endif // _MULTIBANDIMAGE_H_
