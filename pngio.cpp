/*
 PNGIO.CPP	v0.06
 Simple wrapper to libPNG.

 Copyright (c) 2004 Francesco Orsenigo <francesco.orsenigo@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; see the file COPYING.  If not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA.
*/

#include <png.h>
#include <stdio.h>
#include <setjmp.h>	// sucks
#include <stdlib.h>
#include <errno.h>
#include "pngio.h"

#define MAX_WIDTH	8192	// maximum allowed image width
#define MAX_HEIGHT	4096	// maximum allowed image height

#define HEADER_BF	8	// bytesize of PNG file header
#define CHANNELS	4	// always 4 bytes per pixel


/*=============================================================================
 * PNG_VERSION
 * Just returns a pointer to a string with libPNG and Zlib versions.
 */
char* pngVersion()
{
 static char ver[64];
 sprintf(ver, "libpng comp%s, use%s and zlib comp%s,use%s",
	PNG_LIBPNG_VER_STRING, png_libpng_ver,
	ZLIB_VERSION, zlib_version);
 return ver;
}





/*=============================================================================
 * PNG EMPTY
 * Creates an empty png.
 */
pngT* pngEmpty(int w, int h)
{
 pngT* png = (pngT*)malloc(sizeof(pngT) + w * h * CHANNELS);
 png->w = w;
 png->h = h;
 rgbaT zero = {0,0,0,255};
 for(int k = 0; k < w*h; k++) png->p[k] = zero;
 return png; 
}



/*=============================================================================
 * PNG FREE
 * Removes a png from memory.
 */
void pngFree(pngT* png)
{
 free(png);
}





/*=============================================================================
 * PNG LOAD
 * Loads a PNG file into a png_t structure.
 * Each pixel is stored in strict RGBA format, 32bpp (8bit per each plane).
 */
pngT* pngLoad(const char* fn)
{
 png_structp png_ptr = NULL;
 png_infop info_ptr = NULL;

 FILE* fp;
 unsigned char bf[HEADER_BF];

 pngT* bmp = NULL;
 unsigned y, w, h;
 unsigned char *img, **rows = NULL;


 // open file
 fp = fopen(fn, "rb");
 if(fp == NULL) return NULL;	// errno set by fopen()


 // read header
 w = fread(bf, 1, HEADER_BF, fp);
 if(w < HEADER_BF)
 {
    if(feof(fp))	errno = EINVAL;
    else		errno = EIO;
    goto ERROR;
 }


 // check header
 if(png_sig_cmp(bf, 0, 8))
 {
    errno = EINVAL;
    goto ERROR;
 }


 // create first structure
 png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
 if(!png_ptr)	// should not happen?
 {
    errno = ENOBUFS;
    goto ERROR;
 }

 // create second structure
 info_ptr = png_create_info_struct(png_ptr);
 if(!info_ptr)	// again, unlikely to happen?
 {
    errno = ENOBUFS;
    goto ERROR;
 }



 // set jumps
 // i don't like it...
 if(setjmp(png_jmpbuf(png_ptr)))
 {
    errno = ECANCELED;		// this is an error from libpng...
    goto ERROR;
 }

 // prepare to read image
 png_init_io(png_ptr, fp);
 png_set_sig_bytes(png_ptr, 8);
 png_read_info(png_ptr, info_ptr);


 // handle depth
 png_set_strip_16(png_ptr);
 png_set_expand(png_ptr);				// ? don't have much idea on what it does, but works...

 // handle channels
 png_set_palette_to_rgb(png_ptr);			// palette -> RGB //	PNG_COLOR_TYPE_PALETTE
 png_set_gray_to_rgb(png_ptr);				// grayscale->RGB //	PNG_COLOR_TYPE_GRAY PNG_COLOR_TYPE_GRAY_ALPHA
 png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);	// RGB -> RGBA //	PNG_COLOR_RGB


 // read images data
 w = png_get_image_width(png_ptr, info_ptr);
 h = png_get_image_height(png_ptr, info_ptr);


 // limit width and height
 if(w > MAX_WIDTH || h > MAX_HEIGHT)
 {
    errno = EFBIG;
    goto ERROR;
 }


 // allocate bitmap
 bmp = pngEmpty(w, h);
 if(bmp == NULL) goto ERROR;	// should not happen




 // allocate temporary rows
 rows = (unsigned char**)malloc(h * sizeof(unsigned char *));
 if(rows == NULL) goto ERROR;	// should not happen

 // init 'img' and 'rows'
 img = bmp->d;
 for(y = 0; y < h; y++) rows[y] = img + y * w * CHANNELS;

 // finally, do the actual reading
 png_read_image(png_ptr, rows);
 png_read_end(png_ptr, NULL);

 // set w and h
 bmp->w = w;
 bmp->h = h;

 // remove garbage
 png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
 free(rows);
 fclose(fp);

 // happy ending
 return bmp;

 ERROR:
 if(NULL != png_ptr)	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
 if(NULL != bmp)	free(bmp);
 if(NULL != rows)	free(rows);
 fclose(fp);
 return NULL;
}





/*=============================================================================
 * PNG SAVE
 * Saves a PNG file from a buffer, given width and height.
 * Each pixel must be in strict RGBA format, 32bpp (8bit per each plane).
 */
int pngSave(const char* fn, pngT* png)
{
 pngSave(fn, png->d, png->w, png->h);
}



int pngSave(const char* fn, unsigned char* img, int w, int h)
{
 png_structp png_ptr;
 png_infop info_ptr;
 
 FILE* fp;
 int y;
 unsigned char* rows[h];


 // create file, brutally overwriting everything that was there before
 fp = fopen(fn, "wb");
 if(fp == NULL) return -1;	// errno set by fopen()

  // create first structure
 png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
 if(!png_ptr)	// should not happen?
 {
    errno = ENOBUFS;
    goto ERROR;
 }

 // create second structure
 info_ptr = png_create_info_struct(png_ptr);
 if(!info_ptr)	// again, unlikely to happen?
 {
    errno = ENOBUFS;
    goto ERROR;
 }


 // set jumps
 // i don't like it...
 if(setjmp(png_jmpbuf(png_ptr)))
 {
    errno = ECANCELED;		// this is an error from libpng...
    goto ERROR;
 }


 // prepare to write image
 png_init_io(png_ptr, fp);
 png_set_compression_level(png_ptr, 4); //Z_BEST_COMPRESSION);
 png_set_IHDR(png_ptr, info_ptr, w, h, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

 // set rows
 for(y = 0; y < h; y++) rows[y] = img + y * w * CHANNELS;

 // do the actual write
 png_write_info(png_ptr, info_ptr);
 png_write_image(png_ptr, rows);
 png_write_end(png_ptr, info_ptr);

 // remove garbage
 png_destroy_write_struct(&png_ptr, &info_ptr);
 fclose(fp);

 // happy ending
 return 0;

 // unhappy ending
 ERROR:
 if(NULL != png_ptr)	png_destroy_write_struct(&png_ptr, &info_ptr);
 fclose(fp);
 return -1;
}





//EOF =========================================================================
