#include "project.h"

int main(int argc, char** argv)
{
    int i;
    char *filename = "image.pgm";
    int filterSize = 3;
    int n = 64;
    int n2 =128;
    int n3 =256;
    int stride = 2;
    float radio = 2;
    float alpha = 2;
    int activation = ACTIVATION_MAX;
    MultibandKernel **kernels = generateKernelBank(filterSize,filterSize,1,n);
    MultibandImage **images = (MultibandImage **)calloc(n,sizeof(MultibandImage**));
    MultibandImage *im = ReadGrayImageIntoMultibandImage(filename);
    MultibandImage *appended, *pooled, *normalized;
    AdjRel *rectangular = Rectangular(3,3);


    //Layer 0
    normalized = normalize(im, rectangular);
    DestroyMultibandImage(&im);
    im = normalized;
    normalized = NULL;


    //First layer
    for(i=0; i< n; i++)
    {
        images[i] = MultibandCorrelation(im, kernels[i], activation);
    }
    appended = AppendManyMultibandImages(images, n);
    for(i=0; i<n; i++){
        DestroyMultibandImage(&images[i]);
        DestroyMultibandKernel(&kernels[i]);
    }
    free(images);
    free(kernels);
    pooled = pooling(appended, stride, radio, alpha);
    DestroyMultibandImage(&appended);
    normalized = normalize(pooled, rectangular);
    DestroyMultibandImage(&pooled);

    //Second layer
    images = (MultibandImage **)calloc(n2,sizeof(MultibandImage**));
    kernels = generateKernelBank(filterSize,filterSize,n,n2);
    for(i=0; i< n2; i++)
    {
        images[i] = MultibandCorrelation(normalized, kernels[i], activation);
    }
    appended = AppendManyMultibandImages(images, n2);
    DestroyMultibandImage(&normalized);
    for(i=0; i<n; i++){
        DestroyMultibandImage(&images[i]);
        DestroyMultibandKernel(&kernels[i]);
    }
    free(images);
    free(kernels);
    pooled = pooling(appended, stride, radio, alpha);
    DestroyMultibandImage(&appended);
    normalized = normalize(pooled, rectangular);
    DestroyMultibandImage(&pooled);

    //Third layer
    images = (MultibandImage **)calloc(n3,sizeof(MultibandImage**));
    kernels = generateKernelBank(filterSize,filterSize,n2,n3);
    for(i=0; i< n3; i++)
    {
        images[i] = MultibandCorrelation(normalized, kernels[i], activation);
    }
    appended = AppendManyMultibandImages(images, n3);
    DestroyMultibandImage(&normalized);
    for(i=0; i<n; i++){
        DestroyMultibandImage(&images[i]);
        DestroyMultibandKernel(&kernels[i]);
    }
    free(images);
    free(kernels);
    pooled = pooling(appended, stride, radio, alpha);
    DestroyMultibandImage(&appended);
    normalized = normalize(pooled, rectangular);
    DestroyMultibandImage(&pooled);

    //Write result

    Write2CSV(&normalized, 1, "result.csv");
    DestroyMultibandImage(&normalized);
    return 0;

}



