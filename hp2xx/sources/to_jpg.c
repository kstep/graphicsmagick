/*
   Copyright (c) 2002 Martin Kroeker All rights reserved.
   
   Distributed by Free Software Foundation, Inc.

This file is part of HP2xx.

HP2xx is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the GNU General Public License, Version 2 or later, for full details.

Everyone is granted permission to copy, modify and redistribute
HP2xx, but only under the conditions described in the GNU General Public
License.  A copy of this license is supposed to have been
given to you along with HP2xx so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.
--------------------------------------------------------------------

to_jpg.c - JPEG image converter part of hp2xx 

2002/04/07 MK derived from to_tif.c based on libjpeg's example.c and 
              libjpeg.doc
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "bresnham.h"
#include "pendef.h"
#include "hp2xx.h"
#include "jpeglib.h"

int PicBuf_to_JPG (const GEN_PAR *pg, const OUT_PAR *po)
{
  FILE	*w=NULL;
  RowBuf	*row=NULL;
  int		i,x,y, W, H, D, B, S;
  JSAMPROW row_pointer[1];
  unsigned char		*jpgbuf;
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
        
  if (!pg->quiet)
    Eprintf ("\nWriting JPEG output\n");

  W=po->picbuf->nr;
  H=po->picbuf->nc;
  D=po->picbuf->depth;
  B=po->picbuf->nb;


  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  cinfo.image_width = H;      /* image width and height, in pixels */
  cinfo.image_height = W;
  if (D==1){
  cinfo.input_components = 1;
  cinfo.in_color_space = JCS_GRAYSCALE;
  }else{
  cinfo.input_components = 3;     /* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB; /* colorspace of input image */
  }
  jpeg_set_defaults(&cinfo);

  if (*po->outfile != '-') {    
     if ((w=fopen(po->outfile,"wb"))==NULL){
      PError ("hp2xx -- opening output file");
      return ERROR;
  	}
    jpeg_stdio_dest(&cinfo, w); 
   }else{
    jpeg_stdio_dest(&cinfo, stdout);
   }
  
#if 0
     switch(po->specials) {
     case 0: /* no compression */
     case 1:
            TIFFSetField(w, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
            break;
     case 2: /* Run Length Encoding */
            TIFFSetField(w, TIFFTAG_COMPRESSION, COMPRESSION_CCITTRLE);
            D=1;
            break;
     case 3: /* Group 3 Fax monochrome */
            TIFFSetField(w, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX3);
            D=1;
            break;
     case 4: /* Group 4 Fax monochrome */
            TIFFSetField(w, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4);
            D=1;
            break;
     case 5: /* LZW is patented by Unisys - only license holders should use next line*/ 
        /*  TIFFSetField(w, TIFFTAG_COMPRESSION, COMPRESSION_LZW);*/
            break;
     case 6: /* JPEG formats */
            TIFFSetField(w, TIFFTAG_COMPRESSION, COMPRESSION_OJPEG);
            break;
     case 7: 
            TIFFSetField(w, TIFFTAG_COMPRESSION, COMPRESSION_JPEG);
            break;
     case 8:
            TIFFSetField(w, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
            break;
     default:
            Eprintf("\nCompression Format - not supported yet\n");
            break;
  }
#endif
  
  S=3*po->picbuf->nc;
  if(D==1)S=po->picbuf->nc; 
  if ((jpgbuf=malloc(S*sizeof(unsigned char))) == NULL){
    Eprintf("malloc error!\n");
    if (*po->outfile != '-' ) fclose(w);
    return 1;
  }

jpeg_start_compress(&cinfo, TRUE);


  for (y=0; y<W; ++y)
  {
    if ((row=get_RowBuf(po->picbuf, (W-1)-y))==NULL)
      break;
    memset(jpgbuf,0,S);
    i=0;
    for (x=0; x<H; ++x){
      Byte C=index_from_RowBuf(row, x, po->picbuf);
      if (D==1){
	switch(C){
	  case xxBackground:
	jpgbuf[i++]=255;
	    break;
	  default:
	jpgbuf[i++]=0;
	break;
	}
      }else{
	jpgbuf[i++]=pt.clut[C][0];
	jpgbuf[i++]=pt.clut[C][1];
	jpgbuf[i++]=pt.clut[C][2];
    }
    }
	row_pointer[0]= &jpgbuf[0];
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  free(jpgbuf);
  jpeg_finish_compress(&cinfo);
  if (*po->outfile != '-') fclose(w);
  
  return 0;
}
