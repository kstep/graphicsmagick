/* -*-C-*-
******************************************************************************
*
* File:         djpeg.h
* RCS:          $Header$
* Description:  header file for JPEG DECODER library.
* Author:       Hugh P. Nguyen
* Created:      Fri Dec  4 11:04:22 1992
* Modified:     Fri Jul 14 11:04:53 1995 by Gregory S. Yovanof
* Language:     C
* Package:      Hewlett-Packard JPEG Encoder/Decoder
*
* Copyright (c) 1999 Digital Imaging Group, Inc.
* For conditions of distribution and use, see copyright notice
* in Flashpix.h
*
******************************************************************************
*/

#ifndef _DJPEG_H
#define _DJPEG_H

/* CHG_FILE_ERR - unified JPEG error values by offsetting the values by 0x0300 */
/* System error */
# define ERROR_MEM        0x0320  /* memory cannot be allocated */

/* These are UNRECOVERABLE errors, no image is written */
# define ERROR_NOT_JPG    0x0301  /* not a JPEG file */
# define ERROR_NO_HUF     0x0302  /* missing Huffman table from Scan */
# define ERROR_NO_QUAN    0x0303  /* missing Quantization table from Scan */
# define ERROR_NO_FRAME   0x0304  /* no Frame header before Scan */
# define ERROR_MULT_FRAME 0x0305  /* multiple frame headers found */
# define ERROR_NO_IMAGE   0x0306  /* no Image found between SOI and EOI */
# define ERROR_COMP       0x0307  /* # image components is not 1, 2, 3 or 4 */
# define ERROR_SOF        0x0308  /* Non baseline JPEG, markers SOF1-F found */
# define ERROR_DNL        0x0309  /* Non baseline JPEG, height of frame is 0, it
                                   needs to be defined by DNL marker */
# define ERROR_EOF        0x030a  /* unexpected EOF */
# define ERROR_DATA       0x030b  /* corrupted or non-JPG data */
# define ERROR_IMG_FORMAT 0x030c  /* ERROR: Un-supported image format */

/* These are RECOVERABLE errors, the image - though corrupted - was fixed
   and written */
# define WARNING_EOF      0x0310  /* unexpected EOF */
# define WARNING_DATA     0x0311  /* corrupted data */


#define IMG_FORMAT_0    0x00   /* Un-supported image format */
#define IMG_FORMAT_1    0x01   /* Grayscale image */
#define IMG_FORMAT_11   0x02   /* Two-component non-interleaved image */
#define IMG_FORMAT_111  0x03   /* Three-component non-interleaved image */
#define IMG_FORMAT_1111 0x04   /* Four-component non-interleaved image */
#define IMG_FORMAT_411  0x05   /* Three-component 4:1:1  image */

#define HDR_JPG_FORMAT   0x00  /* Standard JPEG header */
#define HDR_TABLE_FORMAT 0x01  /* Header in JPEG_STRUCT format */

JPEGEXPORT
int DT_Decode_Tile();
JPEGEXPORT
int DJ_Decode_JPEG_Tile_Header();
JPEGEXPORT
int DJ_Decode_JPEG_Tile_Data();
JPEGEXPORT
int DJ_Decode_JPEG_Tile_Data_To_Memory();

#endif  /* _DJPEG_H */
