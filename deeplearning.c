#include "deeplearning.h"


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
    int     ypa, ypb, xpc, xpd, y_max, x_max, dmax;

    /* dividimos la imagen en 5 partes:
    *
    *  +---------------------+
    *  |                     |
    *  |          a          |
    *  +----+-----------+----+
    *  |    |           |    |
    *  |    |  cuadro   |    |
    *  |  c |           | d  |
    *  |    |  interno  |    |
    *  |    |           |    |
    *  +----+-----------+----+
    *  |          b          |
    *  |                     |
    *  +---------------------+
    *
    */

    dmax = MaximumDisplacementMK(K);
    y_max = img->ny - dmax;
    x_max = img->nx - dmax;

    if (((x_max - 1) <= dmax) || ((y_max - 1) <= dmax)) {
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
        return corr;
    }
    // cuadro interno
    for (yp = dmax; yp < y_max; yp++) {
        for (xp = dmax; xp < x_max; xp++ ) {
            val = 0.0;
            for (i=0; i < K->A->n; i++) {
                xq = xp + K->A->adj[i].dx;
                yq = yp + K->A->adj[i].dy;
                for(j=0; j< img->nbands; j++) {
                    val += ((img->band[yq][xq].val[j]) * (K->w[i][j]));
                }
            }
            corr->band[yp][xp].val[0] = activation(val, activation_type);
        }
    }

    //  Parte a y b
    for (ypa = 0, ypb = y_max; ypa < dmax; ypa++, ypb++) {
        for (xp = 0; xp < img->nx; xp++) {
            //parte a
            val = 0.0;
            for (i=0; i < K->A->n; i++) {
                xq = xp + K->A->adj[i].dx;
                yq = ypa + K->A->adj[i].dy;
                if ((xq >= 0)&&(xq < img->nx)&&(yq >= 0)&&(yq < img->ny)) {
                    for(j=0; j< img->nbands; j++) {
                        val += ((img->band[yq][xq].val[j]) * (K->w[i][j]));
                    }
                }
            }
            corr->band[ypa][xp].val[0] = activation(val, activation_type);
            //parte b
            val = 0.0;
            for (i=0; i < K->A->n; i++) {
                xq = xp + K->A->adj[i].dx;
                yq = ypb + K->A->adj[i].dy;
                if ((xq >= 0)&&(xq < img->nx)&&(yq >= 0)&&(yq < img->ny)) {
                    for(j=0; j< img->nbands; j++) {
                        val += ((img->band[yq][xq].val[j]) * (K->w[i][j]));
                    }
                }
            }
            corr->band[ypb][xp].val[0] = activation(val, activation_type);
        }
    }

    // Parte c y d
    for (yp = dmax; yp < y_max; yp++) {
        for (xpc = 0, xpd = x_max; xpc < dmax; xpc++, xpd++) {
            //parte c
            val = 0.0;
            for (i=0; i < K->A->n; i++) {
                xq = xpc + K->A->adj[i].dx;
                yq = yp + K->A->adj[i].dy;
                if ((xq >= 0)&&(xq < img->nx)) {
                    for(j=0; j< img->nbands; j++) {
                        val += ((img->band[yq][xq].val[j]) * (K->w[i][j]));
                    }
                }
            }
            corr->band[yp][xpc].val[0] = activation(val, activation_type);
            //parte d
            val = 0.0;
            for (i=0; i < K->A->n; i++) {
                xq = xpd + K->A->adj[i].dx;
                yq = yp + K->A->adj[i].dy;
                if ((xq >= 0)&&(xq < img->nx)) {
                    for(j=0; j< img->nbands; j++) {
                        val += ((img->band[yq][xq].val[j]) * (K->w[i][j]));
                    }
                }
            }
            corr->band[yp][xpd].val[0] = activation(val, activation_type);
        }
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
