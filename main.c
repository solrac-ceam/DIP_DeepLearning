#include "project.h"
#include <sys/stat.h>
#include <sys/types.h>

int main()
{
    int i;
    char *filename = "parasito_01_001.pgm";
    int filterSize = 3;
    int n = 64;
    int stride = 2;
    float radio = 2;
    float alpha = 2;
    MultibandKernel **kernels = generateKernelBank(filterSize,filterSize,1,n);
    MultibandImage **images = (MultibandImage **)calloc(n,sizeof(MultibandImage**));
    MultibandImage *im = ReadGrayImageIntoMultibandImage(filename);
    MultibandImage *appended, *pooled, *normalized;
    AdjRel *rectangular = Rectangular(3,3);

    for(i=0; i< n; i++)
    {
        images[i] = MultibandCorrelation(im, kernels[i], ACTIVATION_MAX);
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
    Write2CSV(&normalized, 1, "result.cvs");
    DestroyMultibandImage(&normalized);

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

    printf("name: %s, ext: %s", name, extencion);*/
    return 0;

}



