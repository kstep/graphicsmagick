/*
% Copyright (C) 2003 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                   CCCC  IIIII  N   N  EEEEE  OOO   N   N                    %
%                  C        I    NN  N  E     O   O  NN  N                    %
%                  C        I    N N N  EEEE  O   O  N N N                    %
%                  C        I    N  NN  E     O   O  N  NN                    %
%                   CCCC  IIIII  N   N  EEEEE  OOO   N   N                    %
%                                                                             %
%                                                                             %
%                    Read/Write Kodak Cineon Image Format.                    %
%                Cineon Image Format is a subset of SMTPE DPX                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                             Kelly Bergougnoux                               %
%                               October 2003                                  %
%                                                                             %
%                                                                             %
%  Copyright (C) 1999-2003 ImageMagick Studio, a non-profit organization      %
%  dedicated to making software imaging solutions freely available.           %
%                                                                             %
%  This software and documentation is provided "as is," and the copyright     %
%  holders and contributing author(s) make no representations or warranties,  %
%  express or implied, including but not limited to, warranties of            %
%  merchantability or fitness for any particular purpose or that the use of   %
%  the software or documentation will not infringe any third party patents,   %
%  copyrights, trademarks or other rights.                                    %
%                                                                             %
%  The copyright holders and contributing author(s) will not be held liable   %
%  for any direct, indirect, special or consequential damages arising out of  %
%  any use of the software or documentation, even if advised of the           %
%  possibility of such damage.                                                %
%                                                                             %
%  Permission is hereby granted to use, copy, modify, and distribute this     %
%  source code, or portions hereof, documentation and executables, for any    %
%  purpose, without fee, subject to the following restrictions:               %
%                                                                             %
%    1. The origin of this source code must not be misrepresented.            %
%    2. Altered versions must be plainly marked as such and must not be       %
%       misrepresented as being the original source.                          %
%    3. This Copyright notice may not be removed or altered from any source   %
%       or altered source distribution.                                       %
%                                                                             %
%  The copyright holders and contributing author(s) specifically permit,      %
%  without fee, and encourage the use of this source code as a component for  %
%  supporting image processing in commercial products.  If you use this       %
%  source code in a product, acknowledgment is not required but would be      %
%  be appreciated.                                                            %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Cineon image file format draft is available at
%  http://www.cineon.com/ff_draft.php.
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/cache.h"
#include "magick/error.h"
#include "magick/list.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Forward declaractions.
*/
static unsigned int
  WriteCINEONImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s C I N E O N                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsCINEON() returns True if the image format type, identified by the magick
%  string, is CINEON.
%
%  The format of the IsCINEON method is:
%
%      unsigned int IsCINEON(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsCINEON(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\200\052\137\327",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C I N E O N I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadCINEONImage() reads an CINEON X image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a point to the
%  new image.
%
%  The format of the ReadCINEONImage method is:
%
%      Image *ReadCINEONImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadCINEONImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define MonoColorType  1
#define RGBColorType  3

  char
    magick[4];

  Image
    *image;

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  size_t
    count;    

  unsigned char
    colortype;    

  unsigned int
    status;

  unsigned long
    headersize,
    pixel;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    {
      DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Read CINEON image.
  */
  count=ReadBlob(image,4,(char *) magick);
  if ((count == 0) ||
      ((LocaleNCompare((char *) magick,"\200\052\137\327",4) != 0)))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  headersize=ReadBlobMSBLong(image); 
  for (i=0; i < 185; i++)
    (void) ReadBlobByte(image);
  colortype=ReadBlobByte(image);
  for (i=0; i < 4; i++)
    (void) ReadBlobByte(image);
  image->depth=ReadBlobByte(image) > 8 ? 16  : 8;
  (void) ReadBlobByte(image);
  image->columns= ReadBlobMSBLong(image);
  image->rows= ReadBlobMSBLong(image);
  (void) SeekBlob(image,headersize,SEEK_SET);
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Convert CINEON raster image to pixel packets.
  */
  switch ((int) colortype)
  {
    case (MonoColorType):
    {
      q=SetImagePixels(image,0,0,image->columns,image->rows);
      for (x=0; x < (long) ((image->columns*image->rows)/3); x++)
      {
        pixel=ReadBlobMSBLong(image);
        q->red=(Quantum) ((double) MaxRGB*((pixel >> 0) & 0x3ff)/1023+0.5);
        q->green=q->red;
        q->blue=q->red;
        q++;
        q->red=(Quantum) ((double) MaxRGB*((pixel >> 10) & 0x3ff)/1023+0.5);
        q->green=q->red;
        q->blue=q->red;
        q++;
        q->red=(Quantum) ((double) MaxRGB*((pixel >> 20) & 0x3ff)/1023+0.5);
        q->green=q->red;
        q->blue=q->red;
        q++;
      }
      break;
    }
    case (RGBColorType):
    {
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          pixel=ReadBlobMSBLong(image);
          q->red=(Quantum) ((double) MaxRGB*((pixel >> 22) & 0x3ff)/1023+0.5);
          q->green=(Quantum) ((double) MaxRGB*((pixel >> 12) & 0x3ff)/1023+0.5);
          q->blue=(Quantum) ((double) MaxRGB*((pixel >> 2) & 0x3ff)/1023+0.5);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            if (!MagickMonitor(LoadImageText,y,image->rows,exception))
              break;
      }
      break;
    }
    default:
      ThrowReaderException(CorruptImageError,ImageTypeNotSupported,image);
  }
  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
  CloseBlob(image);
  return(GetFirstImageInList(image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r C I N E O N I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCINEONImage adds attributes for the CINEON image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCINEONImage method is:
%
%      RegisterCINEONImage(void)
%
*/
ModuleExport void RegisterCINEONImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CIN");
  entry->decoder=(DecoderHandler) ReadCINEONImage;
  entry->encoder=(EncoderHandler) WriteCINEONImage;
  entry->magick=(MagickHandler) IsCINEON;
  entry->description=AcquireString("Cineon Image File");
  entry->module=AcquireString("CINEON");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C I N E O N I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCINEONImage removes format registrations made by the
%  CINEON module from the list of supported formats.
%
%  The format of the UnregisterCINEONImage method is:
%
%      UnregisterCINEONImage(void)
%
*/
ModuleExport void UnregisterCINEONImage(void)
{
  (void) UnregisterMagickInfo("CIN");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e C I N E O N I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteCINEONImage() writes an image in CINEON encoded image format.
%
%  The format of the WriteCINEONImage method is:
%
%      unsigned int WriteCINEONImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o image_info: The image info.
%
%    o image:  The image.
%
%
*/
static unsigned int WriteCINEONImage(const ImageInfo *image_info,Image *image)
{
  long
    y;

  register const PixelPacket
    *p;
  
  register long
    i,
    x;

  unsigned int
    status;
 
  unsigned long
    pixel;

  unsigned long
    maxr,
    maxg,
    maxb,
    minr,
    ming,
    minb;

  /* 
    Get the max values for each component... I guess it "needs" it...
  */
  maxr=maxg=maxb=0;
  minr=ming=minb=MaxRGB;
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      if ( p->red > maxr ) maxr = p->red;
      if ( p->green > maxg ) maxg = p->green;
      if ( p->blue > maxb ) maxb = p->blue;
      if ( p->red < minr ) minr = p->red;
      if ( p->green < ming ) ming = p->green;
      if ( p->blue < minb ) minb = p->blue;
      p++;
    }
  }
  /*
    Div by 64 to get 10bit values since that's all I'll do right now.
  */
  maxr/=64;
  maxg/=64;
  maxb/=64;
  minr/=64;
  ming/=64;
  minb/=64;
  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    return(status);
  /* Magick Number... */
  (void) TransformColorspace(image,RGBColorspace);
  (void) WriteBlobMSBLong(image,0x802A5FD7);
  /* Offset to image */
  (void) WriteBlobMSBLong(image,0x0800);
  /* Generic section header length */
  (void) WriteBlobMSBLong(image,0x400);	
  /* Industry Specific lenght */
  (void) WriteBlobMSBLong(image,0x400);	
  /* variable length section */
  (void) WriteBlobMSBLong(image,0x0);
  /* Total image file size */
  (void) WriteBlobMSBLong(image,4*image->columns*image->rows+0x2000); 
  /* Version number of header format */
  (void) WriteBlobString(image,"V4.5");	
  (void) WriteBlobMSBLong(image,0x0);
  /* Filename */
  (void) WriteBlobString(image,image->filename);
  for ( i =0 ; i < ( 100 - strlen(image->filename) ); i++ )
    (void) WriteBlobByte(image,0);
  /* Creation Date. */
  (void) WriteBlobString(image,"yyyy:mm:dd  ");
  /* Creation Time. */
  (void) WriteBlobString(image,"hh:mm:ssxxx ");	
  for ( i =0 ; i < 36 ; i++)
    (void) WriteBlobByte(image,0);
  /* 0 left to right top to bottom */
  (void) WriteBlobByte(image,0);
  /* 3 channels */
  (void) WriteBlobByte(image,3);
  /* alignment */
  (void) WriteBlobByte(image,0x0);
  (void) WriteBlobByte(image,0x0);
  /* Do R */ 
  /* Channel 1 designator byte 0 */
  (void) WriteBlobByte(image,0);
  /* Channel 1 designator byte 1 */
  (void) WriteBlobByte(image,1);
  /* Bits per pixel... */
  (void) WriteBlobByte(image,10);
  /* alignment */
  (void) WriteBlobByte(image,0);
  /* pixels per line */
  (void) WriteBlobMSBLong(image,image->columns);
  /* lines per image */
  (void) WriteBlobMSBLong(image,image->rows);
  /* Minimum data value */
  (void) WriteBlobMSBLong(image,minr);
  /* Minimum quantity represented */
  (void) WriteBlobMSBLong(image,0x0);
  /* Maximum data value */
  (void) WriteBlobMSBLong(image,maxr);
  /* Maximum quantity represented */
  (void) WriteBlobMSBLong(image,0x40000000);
  /* Do G */ 
  /* see above. */
  (void) WriteBlobByte(image,0);
  (void) WriteBlobByte(image,2);
  (void) WriteBlobByte(image,10);
  (void) WriteBlobByte(image,0);
  (void) WriteBlobMSBLong(image,image->columns);
  (void) WriteBlobMSBLong(image,image->rows);   
  (void) WriteBlobMSBLong(image,ming);	
  (void) WriteBlobMSBLong(image,0x0);
  (void) WriteBlobMSBLong(image,maxg);
  (void) WriteBlobMSBLong(image,0x40000000);
  /* Go B */
  /* see above. */
  (void) WriteBlobByte(image,0);
  (void) WriteBlobByte(image,3);
  (void) WriteBlobByte(image,10);
  (void) WriteBlobByte(image,0);
  (void) WriteBlobMSBLong(image,image->columns);
  (void) WriteBlobMSBLong(image,image->rows); 
  (void) WriteBlobMSBLong(image,minb);
  (void) WriteBlobMSBLong(image,0x0);
  (void) WriteBlobMSBLong(image,maxb);
  (void) WriteBlobMSBLong(image,0x40000000);
  /* pad channel 4-8 */
  for (i=0; i < 5*28; i++)
    (void) WriteBlobByte(image,0x00);
  /* k here's where it gets ugly and I should be shot */
  /* White point(colour temp.) x */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* White point(colour temp.) y */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Red primary chromaticity x */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Red primary chromaticity y */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Green primary chromaticity x */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Green primary chromaticity y */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Blue primary chromaticity x */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Blue primary chromaticity y */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Label Text... */
  for (i=0; i < 200+28; i++)
    (void) WriteBlobByte(image,0x00);
  /* pixel interleave (rgbrgbr...) */
  (void) WriteBlobByte(image,0);
  /* Packing longword (32bit) boundaries */
  (void) WriteBlobByte(image,5);
  /* Data unsigned */
  (void) WriteBlobByte(image,0);
  /* Image sense: positive image */
  (void) WriteBlobByte(image,0);
  /* End of line padding */
  (void) WriteBlobMSBLong(image,0x0);
  /* End of channel padding */
  (void) WriteBlobMSBLong(image,0x0);
  /* Reseved for future Use.. */
  for (i=0; i < 20; i++)
    (void) WriteBlobByte(image,0x00);

  /*
    Generic Orientation Header
  */

  /* X offset */
  (void) WriteBlobMSBLong(image,0x0);
  /* Y offset */
  (void) WriteBlobMSBLong(image,0x0);
  /* Filename */
  (void) WriteBlobString(image,image->filename);
  for ( i =0 ; i < ( 100 - strlen(image->filename) ); i++ ) 
    (void) WriteBlobByte(image,0);
  /* date. who cares ? */
  for ( i =0 ; i < 12 ; i++ ) 
    (void) WriteBlobByte(image,0);
  /* time who cares ? */
  for ( i =0 ; i < 12 ; i++ ) 
    (void) WriteBlobByte(image,0);
  (void) WriteBlobString(image,"GraphicsMagick");
  for (i = 0; i < 64-11 ; i++)
    (void) WriteBlobByte(image,0);
  for (i = 0; i < 32 ; i++)
    (void) WriteBlobByte(image,0);
  for (i = 0; i < 32 ; i++)
    (void) WriteBlobByte(image,0);
  /* X input device pitch */
  (void) WriteBlobMSBLong(image,0x4326AB85);
  /* Y input device pitch */
  (void) WriteBlobMSBLong(image,0x4326AB85);
  /* Image gamma */
  (void) WriteBlobMSBLong(image,0x3F800000);
  /* Reserved for future use */
  for (i = 0; i < 40 ; i++)
    (void) WriteBlobByte(image,0);
  for ( i = 0 ; i < 4 ; i++)
    (void) WriteBlobByte(image,0);
  (void) WriteBlobMSBLong(image,0x0);
  (void) WriteBlobMSBLong(image,0x0);
  for ( i = 0 ; i < 32 ; i++)
    (void) WriteBlobByte(image,0);
  (void) WriteBlobMSBLong(image,0x0);
  (void) WriteBlobMSBLong(image,0x0);
  for ( i = 0 ; i < 32 ; i++)
    (void) WriteBlobByte(image,0);
  for ( i = 0 ; i < 200 ; i++)
    (void) WriteBlobByte(image,0);
  for ( i = 0 ; i < 740 ; i++)
    (void) WriteBlobByte(image,0);
  /*
    Convert pixel packets to CINEON raster image.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      pixel=(((long) ((1023L*p->red+MaxRGB/2)/MaxRGB) & 0x3ff) << 22) |
        (((long) ((1023L*p->green+MaxRGB/2)/MaxRGB) &0x3ff) << 12) |
        (((long) ((1023L*p->blue+MaxRGB/2)/MaxRGB) &0x3ff) << 2);
      (void) WriteBlobMSBLong(image,pixel);
      p++;
    }
  }
  CloseBlob(image);  
  return(status);
}
