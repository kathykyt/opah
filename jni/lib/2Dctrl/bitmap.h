#ifndef __BITMAP__H__
#define __BITMAP__H__

#include "color.h"

typedef struct _Bitmap {
    int width;
    int height;
    Color *data;
}Bitmap; 

#endif
