/**
 ** TBI: all PNF stuff goes here
 **/

#include <stdio.h>
#include <stdlib.h>
#include "to_png.h"
#include <png.h>

#define PDNCOL 256 

void pdImagePNG(im, fd)
        pdImagePtr im;
        FILE *fd;
{
	png_structp png_ptr;
	png_infop   info_ptr;
	png_uint_32 width, height;
	int bit_depth /*, color_type, interlace_type*/;
/*	png_uint_32 num_palette; */
	struct png_color_struct *palette;
	int ci;
	png_bytep *row_pointers;


	/* allocate and init png_struct */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
	if (!png_ptr)
		return;

	/* allocate and init png_info */
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return;
	}

	/* set up error handlimg for libpng */
	if (setjmp(png_ptr->jmpbuf))
	{    
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return;
	}

	/* initialize I/O */
	png_init_io(png_ptr, fd);
	
	/* turn on or off filtering, and/or choose specific filters */
	png_set_filter(png_ptr, 0,
		PNG_FILTER_NONE | PNG_FILTER_SUB | PNG_FILTER_PAETH);

	/* set the zlib compression level */
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	/* set other zlib parameters */
	png_set_compression_mem_level(png_ptr, 8);
	png_set_compression_strategy(png_ptr, Z_DEFAULT_STRATEGY);
	png_set_compression_window_bits(png_ptr, 15);
	png_set_compression_method(png_ptr, 8);

	width     = im->sx;
	height    = im->sy;
	bit_depth = 8; /* for now */

	/* write the image header */
	png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
		PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	/* allocate palette */
	palette = png_malloc(png_ptr, PDNCOL * sizeof (png_color));

	/* copy palette data */
	for (ci = 0; ci < PDNCOL; ci++) 
	{
		palette[ci].red   = im->red[ci];
		palette[ci].green = im->green[ci];
		palette[ci].blue  = im->blue[ci];
	}

	/* set palette */
	png_set_PLTE(png_ptr, info_ptr, palette, PDNCOL);

	/* Write the file header information.  REQUIRED */
	png_write_info(png_ptr, info_ptr);

	/* pack pixels into bytes */
	png_set_packing(png_ptr);

	row_pointers = im->pixels;

	/* write whole image */
	png_write_image(png_ptr, row_pointers);

	/* It is REQUIRED to call this to finish writing the rest of the file */
	png_write_end(png_ptr, info_ptr);

	/* if you malloced the palette, free it here */
	free(info_ptr->palette);

	/* if you allocated any text comments, free them here */

	/* clean up after the write, and free any memory allocated */
	png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

	return;

}

