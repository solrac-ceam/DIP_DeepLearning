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
#include "mc920.h"


#define ACTIVATION_MAX 0
#define ACTIVATION_NONE -1

int remove_directory(const char *path);


#endif
