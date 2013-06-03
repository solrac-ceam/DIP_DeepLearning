#include "dplrgcommon.h"
#include "deeplearning.h"



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


void body(Parameters param, MultibandKernel **kernels1, MultibandKernel **kernels2, MultibandKernel **kernels3)
    /*int filterSize,
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
    int normalizationSize)*/
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
        layer_result = layer0(im, param.nz_c1);
        DestroyMultibandImage(&im);
        im = layer_result;

        //Layer 1
        layer_result = layer_n(im, param.fn_c1, 1, kernels1, param.activation, param.stride, param.pz_c1, param.alpha, param.nz_c1);
        DestroyMultibandImage(&im);
        im = layer_result;

        //Layer 2
        layer_result = layer_n(im, param.fn_c2, param.fn_c1, kernels2, param.activation, param.stride, param.pz_c2, param.alpha, param.nz_c2);
        DestroyMultibandImage(&im);
        im = layer_result;

        //Layer 3
        layer_result = layer_n(im, param.fn_c3, param.fn_c2, kernels3, param.activation, param.stride, param.pz_c3, param.alpha, param.nz_c3);
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

void exit_with_help()
{
    printf(
"This software has..."
"USAGE:\n"
"   <fz_c1> <fz_c2> <fz_c3> <fn_c1> <fn_c2> <fn_c3> <alpha> <stride> <n_img>\n"
"   OR\n"
"   -d <fz_c1> <fz_c2> <fz_c3> <fn_c1> <fn_c2> <fn_c3> <alpha> <stride> <n_img> <pz_c1> <pz_c2> <pz_c3> <nz_c1> <nz_c2> <nz_c3>\n\n"
"       fz_c1   : Filter zise of layer 1\n"
"       fz_c2   : Filter zise of layer 2\n"
"       fz_c3   : Filter zise of layer 3\n"
"       fn_c1   : Filter number of layer 1\n"
"       fn_c2   : Filter number of layer 2\n"
"       fn_c3   : Filter number of layer 3\n"
"       alpha   : alfha of polling\n"
"       stride  : Stride of pulling\n"
"       n_img   : Number of images to be proceseed\n"
    );
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    Parameters param;

    // [-d] <fz_c1> <fz_c2> <fz_c3> <fn_c1> <fn_c2> <fn_c3> <alpha> <stride> <n_img> [<pz_c1> <pz_c2> <pz_c3> <nz_c1> <nz_c2> <nz_c3>]
    // -d : diferentes los tamaños de filtro, normalización y pulling
    int i = 1;
    bool diferents = false;
    if ((argc == 10) || (argc == 17)){
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-d") == 0) {
                diferents = true;
                i++;
            }
            else {
                exit_with_help();
            }
        }

        if (diferents && argc != 17) exit_with_help();

        param.fz_c1 = atoi(argv[i++]);
        param.fz_c2 = atoi(argv[i++]);
        param.fz_c3 = atoi(argv[i++]);
        param.fn_c1 = atoi(argv[i++]);
        param.fn_c2 = atoi(argv[i++]);
        param.fn_c3 = atoi(argv[i++]);
        param.alpha = atof(argv[i++]);
        param.stride = atoi(argv[i++]);
        param.n_img = atoi(argv[i++]);

        if (diferents) {
            param.pz_c1 = atoi(argv[i++]);
            param.pz_c2 = atoi(argv[i++]);
            param.pz_c3 = atoi(argv[i++]);
            param.nz_c1 = atoi(argv[i++]);
            param.nz_c2 = atoi(argv[i++]);
            param.nz_c3 = atoi(argv[i++]);
        }
        else{
            param.pz_c1 = param.nz_c1 = param.fz_c1;
            param.pz_c2 = param.nz_c2 = param.fz_c2;
            param.pz_c3 = param.nz_c3 = param.fz_c3;
        }

        param.activation = ACTIVATION_MAX;
        //return EXIT_SUCCESS;
    }
    else {
        printf("fallo argumentos: %d\n", argc);
        exit_with_help();
    }

    //printf(" params: %d, %d, %d, %d, %d, %d, %d\n", param.fz_c1, param.fz_c2, param.fz_c3, param.fn_c1, param.fn_c2, param.fn_c3, param.n_img);

    int loops;

    /*int filterSize = atoi(argv[1]);
    int n = atoi(argv[2]);
    int n2 =atoi(argv[3]);
    int n3 =atoi(argv[4]);
    int stride = atoi(argv[5]);
    float radio = atof(argv[6]);
    float alpha = atof(argv[7]);
    int normalizationSize = atoi(argv[8]);
    int numimages = atoi(argv[9]);
    int activation = ACTIVATION_MAX;*/


    srand(time(NULL));
    //Generate Random Filter Banks
    MultibandKernel **kernels1 = generateKernelBank(param.fz_c1, param.fz_c1, 1, param.fn_c1);
    MultibandKernel **kernels2 = generateKernelBank(param.fz_c2, param.fz_c2, param.fn_c1,param.fn_c2);
    MultibandKernel **kernels3 = generateKernelBank(param.fz_c3, param.fz_c3, param.fn_c2, param.fn_c3);
    printf("Created Multiband Kernels for convolution\n");



    printf("Started bucle\n");

    #pragma omp parallel for private(loops) num_threads(8)
    for(loops=0; loops<param.n_img; loops++){
        printf("Image %d\n", loops+1);
        body(param, kernels1, kernels2, kernels3);
    }

    for(i=0; i<param.fn_c1; i++){
        DestroyMultibandKernel(&kernels1[i]);
    }
    for(i=0; i<param.fn_c2; i++){
        DestroyMultibandKernel(&kernels2[i]);
    }
    for(i=0; i<param.fn_c3; i++){
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



