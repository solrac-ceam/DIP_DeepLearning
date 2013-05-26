#include "dplrgcommon.h"
#include "deeplearning.h"
#include <omp.h>




MultibandImage *layer0(MultibandImage *im, int normalizationSize){
    AdjRel *rectangular = RectangularKernel(normalizationSize,normalizationSize);
    MultibandImage *normalized;
    normalized = normalize(im, rectangular);
    DestroyAdjRel(&rectangular);
    return normalized;

}

MultibandImage *layer_n(MultibandImage *im, int n_filters, int prior_n_filters, MultibandKernel **kernels, int activation, int stride, float radio, float alpha,
                        int normalizationSize){
    int i;
    MultibandImage **images;
    MultibandImage *pooled, *normalized, *appended;
    AdjRel *rectangular = RectangularKernel(normalizationSize,normalizationSize);

    images = (MultibandImage **)calloc(n_filters,sizeof(MultibandImage**));


    for(i=0; i< n_filters; i++)
    {
        images[i] = MultibandCorrelation(im, kernels[i], activation);
    }
    appended = AppendManyMultibandImages(images, n_filters);
    for(i=0; i<n_filters; i++){
        DestroyMultibandImage(&images[i]);
    }
    free(images);
    pooled = pooling(appended, stride, radio, alpha);
    DestroyMultibandImage(&appended);
    normalized = normalize(pooled, rectangular);
    DestroyMultibandImage(&pooled);
    DestroyAdjRel(&rectangular);
    return normalized;
}


void body(
    int filterSize,
    int n,
    int n2,
    int n3,
    int stride,
    float radio,
    float alpha,
    int activation,
    MultibandKernel **kernels1,
    MultibandKernel **kernels2,
    MultibandKernel **kernels3,
    int normalizationSize)
{
    int aux = 0;
    char c;
    char filename[200];
    //Read image
    MultibandImage *im;
    MultibandImage *layer_result;
        #pragma omp critical
        {
            while((c=getchar()) && c!='\n')
               filename[aux++] = c;
            filename[aux] = '\0';
        }

        im = ReadGrayImageIntoMultibandImage(filename);
        //Layer 0
        layer_result = layer0(im, normalizationSize);
        DestroyMultibandImage(&im);
        im = layer_result;

        //Layer 1
        layer_result = layer_n(im, n, 1, kernels1, activation, stride, radio, alpha, normalizationSize);
        DestroyMultibandImage(&im);
        im = layer_result;

        //Layer 2
        layer_result = layer_n(im, n2, n, kernels2, activation, stride, radio, alpha, normalizationSize);
        DestroyMultibandImage(&im);
        im = layer_result;

        //Layer 3
        layer_result = layer_n(im, n3, n2, kernels3, activation, stride, radio, alpha, normalizationSize);
        DestroyMultibandImage(&im);
        im = layer_result;


        //Write result
        #pragma omp critical
        {
            Write2CSV(&im, 1, "result.csv", filename);
        }
        //Destroy
        DestroyMultibandImage(&im);
}

int main(int argc, char** argv)
{
    int loops,i;


    int filterSize = atoi(argv[1]);
    int n = atoi(argv[2]);
    int n2 =atoi(argv[3]);
    int n3 =atoi(argv[4]);
    int stride = atoi(argv[5]);
    float radio = atof(argv[6]);
    float alpha = atof(argv[7]);
    int normalizationSize = atoi(argv[8]);
    int numimages = atoi(argv[9]);
    int activation = ACTIVATION_MAX;


    srand(time(NULL));
    //Generate Random Filter Banks
    MultibandKernel **kernels1 = generateKernelBank(filterSize,filterSize,1,n);
    MultibandKernel **kernels2 = generateKernelBank(filterSize,filterSize,n,n2);
    MultibandKernel **kernels3 = generateKernelBank(filterSize,filterSize,n2,n3);
    printf("Created Multiband Kernels for convolution\n");



    printf("Started bucle\n");

    #pragma omp parallel for private(loops) num_threads(8)
    for(loops=0; loops<numimages; loops++){
        printf("Image %d\n", loops+1);
        body(filterSize, n, n2, n3, stride, radio, alpha, activation, kernels1, kernels2, kernels3,  normalizationSize);
    }

    for(i=0; i<n; i++){
        DestroyMultibandKernel(&kernels1[i]);
    }
    for(i=0; i<n2; i++){
        DestroyMultibandKernel(&kernels2[i]);
    }
    for(i=0; i<n3; i++){
        DestroyMultibandKernel(&kernels3[i]);
    }

    free(kernels1);
    free(kernels2);
    free(kernels3);


    /*struct stat st = {0};
    if (stat("directorioPrueba", &st) == -1) {
        if (mkdir("directorioPrueba", 0777) == -1){
            Error("cannot create directory.", "WriteMultibandImageToGrayImages");
        }
    }
    char name[30] = "";
    char extencion[10] = "";
    char file[] = "nombrearchivo.extencion";
    char *ptc;

    ptc = strchr(file, '.');
    strcpy(extencion, ptc+1);
    *ptc = '\0';
    strcpy(name, file);

    printf("name: %s, ext: %s", name, extencion);
    FILE *fp=NULL;
    fp = fopen("directorioPrueba/img1.img", "wb");
    fclose(fp);*/
    return 0;

}



