/* -*-C-*-
*************************************************************************
*
* File:         process.h
* RCS:          $Header$
* Description:  Color Rotation & Subsampling routines
* Author:       Gregory S. Yovanof
* Created:      Fri Dec  4 11:04:22 1992
* Initial Source Release:     Wed Jan 17 1996
* Language:     C
* Package:      Hewlett-Packard JPEG Encoder/Decoder
*
* Copyright (c) 1999 Digital Imaging Group, Inc.
* For conditions of distribution and use, see copyright notice
* in Flashpix.h
*
******************************************************************************
*/

/*
**  All of the following routines have the same arguments...
**
**  unsigned char *src     --the data to act upon
**  unsigned char *dst     --where to put the results (can be the same as src)
**  int tileSize   --assuming you have 64x64 tiles, pass 64
**  int bpp    --how many bytes per pixel
**           note that the subsampling routines do not support
**           anything other than 3 bytes/pixel, but will happily
**           destroy any data sent to them
**           even the YCbCr routine will destroy anything but
**           3 or 4 bits/pixel data
*/

/*
** This routine strips color information from an opponent color stream.  Upon completion of
** its assigned duty, the destination buffer will be filled with LLLLabLLLLabLLLLab,
** assuming of course that it was originally LabLabLabLabLab.
**
*/   
int SubSample411(unsigned char *src, 
unsigned char *dst, 
int tileSize, 
int bpp);

/* This routine does an RGB to YCbCr conversion.  After complettion, the
** destination buffer will be filled with YCbCrYCbCrYCbCrYCbCrYCbCr,
** assuming of course that it was originally RGBRGBRGBRGBRGB.
** The routine also handles RGBa data, and in that situation, you'll
** get YCbCraYCbCraYCbCra...
*/   
int RGBtoYCrCb(unsigned char *src, 
unsigned char *dst, 
int tileSize, 
int bpp);

/* This routine does an RGB to YCbCr conversion, then strips color information.  After
** complettion, the destination buffer will be filled with YYYYCbCrYYYYCbCrYYYYCbCr,
** assuming of course that it was originally RGBRGBRGBRGBRGB.  Note that the routine does
** support conversion in place (i.e. src and dst can be the same buffer).
** This routine takes care of 3 channel and 4 channel data.
*/   
int RGBtoYCrCb_SubSample411(unsigned char *src, 
unsigned char *dst, 
int tileSize, 
int bpp);

/*
** This routine strips color information from an opponent color stream.  Upon completion of
** its assigned duty, the destination buffer will be filled with LLabLLabLLab,
** assuming of course that it was originally LabLabLabLabLab.
** This routine takes care of 3 channel and 4 channel data.
**
*/   
int SubSample422(unsigned char *src, 
unsigned char *dst, 
int tileSize, 
int bpp);

/* This routine does an RGB to YCbCr conversion, then strips color information.
** This is not done particularly efficiently, but it's important that it get done
** correctly and on time.  So, rather than write an integrated routine like the 411
** subsampling, the individual pieces are called, simplifying coding and testing.
*/
int RGBtoYCrCb_SubSample422(unsigned char *src, 
unsigned char *dst, 
int tileSize, 
int bpp);

