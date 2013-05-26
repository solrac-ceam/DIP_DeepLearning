#include "deeplearning.h"
#include <omp.h>

MultibandImage *normalize(MultibandImage *img, AdjRel *A){

    int     xq, yq, band, yp, xp,i;
    float   val;
    MultibandImage  *normalized = CreateMultibandImage(img->nx, img->ny, img->nbands);


    for (yp=0; yp < img->ny; yp++)
        for (xp=0; xp < img->nx; xp++) {
            val = 0.0;
            for (i=0; i < A->n; i++) {
                xq = xp + A->adj[i].dx;
                yq = yp + A->adj[i].dy;
                if ((xq >= 0)&&(xq < img->nx)&&(yq >= 0)&&(yq < img->ny)){
                    for (band=0; band<img->nbands; band++){
                        val += ((img->band[yq][xq].val[band])*(img->band[yq][xq].val[band]));
                    }
                }
             }
             for (band=0; band<img->nbands; band++){
                  normalized->band[yp][xp].val[band] = (val != 0.0) ? ((img->band[yp][xp].val[band])/sqrt(val)): 0.0;
              }


    }
    return(normalized);

}

MultibandImage *pooling(MultibandImage *img, int stride, float radio, float alpha){

  AdjRel *A = RectangularKernel(round(radio), round(radio));
  int     xq, yq, band, yp, xp,i;
  MultibandImage  *pooling = CreateMultibandImage(img->nx/stride + (((img->nx % stride) != 0) ? 1:0), img->ny/stride + (((img->ny % stride) != 0) ? 1:0), img->nbands);
  float   val;
  for(band=0; band<img->nbands; band++)
     for (yp=0; yp < img->ny; yp+=stride)
        for (xp=0; xp < img->nx; xp+=stride){
            val = 0.0;
            for (i=0; i < A->n; i++) {
                xq = xp + A->adj[i].dx;
                yq = yp + A->adj[i].dy;
                if ((xq >= 0)&&(xq < img->nx)&&(yq >= 0)&&(yq < img->ny)){
                    val += pow(img->band[yq][xq].val[band], alpha);
                }
            }
          }
          pooling->band[yp/stride][xp/stride].val[band] = pow(val, 1/alpha);
     }
     return(pooling);
}

MultibandImage *MultibandCorrelation(MultibandImage *img, MultibandKernel *K, int activation_type)
{
    int     xp, yp, i;
    int     xq, yq,j;
    MultibandImage  *corr = CreateMultibandImage(img->nx, img->ny, 1);
    float   val;


    for (yp=0; yp < img->ny; yp++)
    for (xp=0; xp < img->nx; xp++) {
        val = 0.0;
        for (i=0; i < K->A->n; i++) {
            xq = xp + K->A->adj[i].dx;
            yq = yp + K->A->adj[i].dy;
            if ((xq >= 0)&&(xq < img->nx)&&(yq >= 0)&&(yq < img->ny)){
                for(j=0; j< img->nbands; j++){
                    val += ((img->band[yq][xq].val[j]) * (K->w[i][j]));
                }
            }
        }
        corr->band[yp][xp].val[0] = activation(val, activation_type);
    }

    return(corr);
}

float activation(float value, int type){

    switch(type){
        case ACTIVATION_MAX: return activation_max(value);
                             break;
    }
    return value;
}

float activation_max(float value){
    return (value>0.0)?value:0;
}
