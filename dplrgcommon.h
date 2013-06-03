#ifndef _DPLRGCOMMON_H_
#define _DPLRGCOMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <float.h>
#include <omp.h>
#include "mc920.h"


#define ACTIVATION_MAX 0
#define ACTIVATION_NONE -1


typedef struct _parameters{
    int fz_c1;
    int fz_c2;
    int fz_c3;
    int fn_c1;
    int fn_c2;
    int fn_c3;
    float alpha;
    int stride;
    int n_img;
    int pz_c1;
    int pz_c2;
    int pz_c3;
    int nz_c1;
    int nz_c2;
    int nz_c3;
    int activation;
} Parameters;

int remove_directory(const char *path);


#endif
