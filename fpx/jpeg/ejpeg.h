/* -*-C-*-
******************************************************************************
*
* File:         ejpeg.h
* RCS:          $Header$
* Description:  header file for JPEG encoder library.
* Author:       Hugh P. Nguyen
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

/* CHG_FILE_ERR - unified JPEG error values by prepending EJPEG_ to the following
        error codes and offsetting the values by 0x0100 */
# define EJPEG_ERROR_PARSER  0x0101        /* error during parsing */
# define EJPEG_ERROR_MEM     0x0102        /* memory can not be allocated */
# define EJPEG_ERROR_EOF     0x0103        /* unexpected EOF (or end of data stream) */
# define EJPEG_ERROR_FORMAT  0x0104        /* unsupported color format */

#define IMG_FORMAT_0    0x00   /* Un-supported image format */
#define IMG_FORMAT_1    0x01   /* Grayscale image */
#define IMG_FORMAT_11   0x02   /* Two-component non-interleaved image */
#define IMG_FORMAT_111  0x03   /* Three-component non-interleaved image */
#define IMG_FORMAT_1111 0x04   /* Four-component non-interleaved image */
#define IMG_FORMAT_411  0x05   /* Three-component 4:1:1  image */

JPEGEXPORT
int No_JPEG_Header_Flag;
