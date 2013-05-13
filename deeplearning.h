#ifndef _DEEP_LEARNING_H_
#define _DEEP_LEARNING_H_

#include "dplrgcommon.h"
#include "multibandImage.h"
#include "multibandKernel.h"

MultibandImage *pooling(MultibandImage *img, int stride, float radio, float alpha);
MultibandImage *normalize(MultibandImage *img, AdjRel *A);
MultibandImage *MultibandCorrelation(MultibandImage *img, MultibandKernel *K, int activation);
float activation(float value, int type);
float activation_max(float value);


#endif
