#include "exifwriter.h"

#include "jhead.h"
#define LOG_TAG "ExifWriter"

#include <utils/Log.h>

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define TAG_ORIENTATION            0x0112
#define TAG_MAKE                   0x010F
#define TAG_MODEL                  0x0110


float *float2degminsec( float deg ) 
{
  float *res = malloc( sizeof(float)*3 ) ;
  res[0] =  floorf( deg ) ;
  float min = ( deg - res[0] ) * 60. ;
  res[1] = floorf( min ) ;
  res[2] = ( min - res[1] ) * 60. ;
  return res ;
}


//
// original source from 
// http://stackoverflow.com/questions/95727/how-to-convert-floats-to-human-readable-fractions
//
char * float2rationnal( float src ) 
{
  long m[2][2] ;
  float x, startx ;
  long maxden = 1000 ;
  long ai ;
  
  startx = x = src ;
  
  /* initialize matrix */
  m[0][0] = m[1][1] = 1;
  m[0][1] = m[1][0] = 0;

  /* loop finding terms until denom gets too big */
  while (m[1][0] *  ( ai = (long)x ) + m[1][1] <= maxden) {
      long t;
      t = m[0][0] * ai + m[0][1];
      m[0][1] = m[0][0];
      m[0][0] = t;
      t = m[1][0] * ai + m[1][1];
      m[1][1] = m[1][0];
      m[1][0] = t;
      if(x==(float)ai) break;     // AF: division by zero
      x = 1/(x - (float) ai);
      if(x>(float)0x7FFFFFFF) break;  // AF: representation failure
  }   
  

  /* now remaining x is between 0 and 1/ai */
  /* approx as either 0 or 1/m where m is max that will fit in maxden */
  /* first try zero */
  LOGV("%ld/%ld, error = %e\n", m[0][0], m[1][0],
	  startx - ((float) m[0][0] / (float) m[1][0]));

  /* now try other possibility */
  ai = (maxden - m[1][1]) / m[1][0];
  m[0][0] = m[0][0] * ai + m[0][1];
  m[1][0] = m[1][0] * ai + m[1][1];
  LOGV("%ld/%ld, error = %e\n", m[0][0], m[1][0],
	  startx - ((float) m[0][0] / (float) m[1][0]));
  
  char *res = (char *)malloc( 256 * sizeof(char) ) ;
  
  snprintf( res, 256, "%ld/%ld", m[0][0], m[1][0] ) ;
  return res ;
}

char * coord2degminsec( float src ) 
{
    char *res = (char *)malloc( 256 * sizeof(char) ) ;
    float *dms = float2degminsec( fabs(src) ) ;
    strcpy( res, float2rationnal(dms[0]) ) ;
    strcat( res , "," ) ;
    strcat( res , float2rationnal(dms[1]) ) ;
    strcat( res , "," ) ;
    strcat( res , float2rationnal(dms[2]) ) ;   
    free( dms ) ;
    return res ;
}

     static void dump_to_file(const char *fname,
                              uint8_t *buf, uint32_t size)
     {
         int nw, cnt = 0;
         uint32_t written = 0;

         LOGD("opening file [%s]\n", fname);
         int fd = open(fname, O_RDWR | O_CREAT);
         if (fd < 0) {
             LOGE("failed to create file [%s]: %s", fname, strerror(errno));
             return;
         }

         LOGD("writing %d bytes to file [%s]\n", size, fname);
         while (written < size) {
             nw = write(fd,
                          buf + written,
                          size - written);
             if (nw < 0) {
                 LOGE("failed to write to file [%s]: %s",
                      fname, strerror(errno));
                 break;
             }
             written += nw;
             cnt++;
         }
         LOGD("done writing %d bytes to file [%s] in %d passes\n",
              size, fname, cnt);
         close(fd);
     }

void writeExif( void *origData, void *destData , int origSize , uint32_t *resultSize, int orientation,camera_position_type  *pt ) {
  const char *filename = "/data/temp.jpg" ;
  
    dump_to_file( filename, (uint8_t *)origData, origSize ) ;
    chmod( filename, S_IRWXU ) ;
    ResetJpgfile() ;
    

    memset(&ImageInfo, 0, sizeof(ImageInfo));
    ImageInfo.FlashUsed = -1;
    ImageInfo.MeteringMode = -1;
    ImageInfo.Whitebalance = -1;

    int gpsTag = 0 ; 
    if( pt != NULL ) {
            gpsTag = 6 ;
    }
    
    
    ExifElement_t *t = (ExifElement_t *)malloc( sizeof(ExifElement_t)*(3+gpsTag) ) ;
    
    ExifElement_t *it = t ;
  // Store file date/time.
  (*it).Tag = TAG_ORIENTATION ;
  (*it).Format = FMT_USHORT ;
  (*it).DataLength = 1 ;
  unsigned short v ;
  if( orientation == 90 ) {
    (*it).Value = "6" ;
  } else if( orientation == 180 ) {
    (*it).Value = "3" ;
  } else {
    (*it).Value = "1" ;
  }
  (*it).GpsTag = FALSE ; 
  
  it++;

  (*it).Tag = TAG_MAKE ;
  (*it).Format = FMT_STRING ;
  (*it).Value = "Samsung" ;
  (*it).DataLength = 8 ;
  (*it).GpsTag = FALSE ;
  
  it++ ;
  
  (*it).Tag = TAG_MODEL ;
  (*it).Format = FMT_STRING ;
  (*it).Value = "Galaxy with GAOSP" ;
  (*it).DataLength = 18 ;
  (*it).GpsTag = FALSE ;
  
  
    if( pt != NULL ) {
    	LOGD("pt->latitude == %f", pt->latitude ) ;
    	LOGD("pt->longitude == %f", pt->longitude ) ;
    	LOGD("pt->altitude == %d", pt->altitude ) ;

    it++ ;
    (*it).Tag = 0x01 ;
    (*it).Format = FMT_STRING ;
    if( pt->latitude > 0 ) {
      (*it).Value = "N" ;
    } else {
      (*it).Value = "S" ;
    }
    (*it).DataLength = 2 ;
    (*it).GpsTag = TRUE ;	
    
    it++ ;
    char *mylat = coord2degminsec( pt->latitude ) ;
    
    (*it).Tag = 0x02 ;
    (*it).Format = FMT_URATIONAL ;
    (*it).Value = mylat ;
    (*it).DataLength = 3 ;
    (*it).GpsTag = TRUE ;
    free( mylat ) ;

    it++ ;
    (*it).Tag = 0x03 ;
    (*it).Format = FMT_STRING ;
    if( (*pt).longitude > 0 ) {
      (*it).Value = "E" ;
    } else {
      (*it).Value = "W" ;
    }
    (*it).DataLength = 2 ;
    (*it).GpsTag = TRUE ;	
    
    it++ ;
     char *mylong = coord2degminsec( (*pt).longitude ) ;
    
    (*it).Tag = 0x04 ;
    (*it).Format = FMT_URATIONAL ;
    (*it).Value = mylong ;
    (*it).DataLength = 3 ;
    (*it).GpsTag = TRUE ; 

    free( mylong ) ;
    
    it++ ;
    (*it).Tag = 0x05 ;
    (*it).Format = FMT_USHORT ;
    if( (*pt).altitude > 0 ) {
      (*it).Value = "0" ;
    } else {
       (*it).Value = "1" ;
    }
    (*it).DataLength = 1 ;
    (*it).GpsTag = TRUE ;	
    
    it++ ;
     char *myalt = float2rationnal( fabs( (*pt).altitude ) ) ;
    
    (*it).Tag = 0x06 ;
    (*it).Format = FMT_SRATIONAL ;
    (*it).Value = myalt ;
    (*it).DataLength = 1 ;
    (*it).GpsTag = TRUE ;
    
    free( myalt ) ;
      
    }

   {
        struct stat st;
        if (stat(filename, &st) >= 0) {
            ImageInfo.FileDateTime = st.st_mtime;
            ImageInfo.FileSize = st.st_size;
        }
    }
    strncpy(ImageInfo.FileName, filename, PATH_MAX);
    
    ReadMode_t ReadMode;
    ReadMode = READ_METADATA;
    ReadMode |= READ_IMAGE;
    int res = ReadJpegFile(filename, (ReadMode_t)ReadMode );
    
    create_EXIF( t, 3, gpsTag);
    
        WriteJpegFile(filename);
	chmod( filename, S_IRWXU ) ;
        DiscardData();    
    
	FILE *src ;
	src = fopen( filename, "r") ;

	fseek( src, 0L, SEEK_END ) ;
	(*resultSize) = ftell(src) ;
	fseek( src, 0L, SEEK_SET ) ;

	int read = fread( destData, 1, (*resultSize), src ) ;
   
     unlink( filename );
}
