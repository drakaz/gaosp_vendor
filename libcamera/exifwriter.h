#ifndef ANDROID_HARDWARE_EXIFWRITER_H
#define ANDROID_HARDWARE_EXIFWRITER_H

#include <stdint.h>

typedef struct
{
    uint32_t timestamp;  /* seconds since 1/6/1980          */
    double   latitude;   /* degrees, WGS ellipsoid */
    double   longitude;  /* degrees                */
    int16_t  altitude;   /* meters                          */
} camera_position_type;


void writeExif( void *origData, void *destData , int origSize , uint32_t *resultSize, int orientation, camera_position_type *pt) ;

#endif

