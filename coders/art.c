/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                             AAA   RRRR   TTTTT                              %
%                            A   A  R   R    T                                %
%                            AAAAA  RRRR     T                                %
%                            A   A  R R      T                                %
%                            A   A  R  R     T                                %
%                                                                             %
%                                                                             %
%                    Read/Write GrahpicsMagick Image Format.                    %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                                January 2001                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/cache.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"


static void InsertRow(unsigned char *p,int y,Image *image)
{
int bit; long x;
register PixelPacket *q;
IndexPacket index;
register IndexPacket *indexes;


 switch (image->depth)
      {
      case 1:  /* Convert bitmap scanline. */
       {
       q=SetImagePixels(image,0,y,image->columns,1);
       if (q == (PixelPacket *) NULL)
       break;
       indexes=GetIndexes(image);
       for (x=0; x < ((long) image->columns-7); x+=8)
    {
    for (bit=0; bit < 8; bit++)
       {
       index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
       indexes[x+bit]=index;
       *q++=image->colormap[index];
       }
    p++;
    }
       if ((image->columns % 8) != 0)
     {
     for (bit=0; bit < (long) (image->columns % 8); bit++)
         {
         index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
         indexes[x+bit]=index;
         *q++=image->colormap[index];
         }
     p++;
     }
        if (!SyncImagePixels(image))
     break;
/*            if (image->previous == (Image *) NULL)
     if (QuantumTick(y,image->rows))
       ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);*/
      break;
      }
       }
}



/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d A R T I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadARTImage reads an ART X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadARTImage method is:
%
%      Image *ReadARTImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadARTImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadARTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image *image;
  int i;
  unsigned width,height,dummy;
  long ldblk;
  unsigned char *BImgBuff=NULL;
  unsigned char k;
  unsigned int status;

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
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Read ART image.
  */
  dummy=ReadBlobLSBShort(image);
  width=ReadBlobLSBShort(image);
  dummy=ReadBlobLSBShort(image);
  height=ReadBlobLSBShort(image);

  ldblk=(long) ((width+7) / 8);
  k=(unsigned char) ((-ldblk) & 0x01);

   if(GetBlobSize(image)!=(8+((long)ldblk+k)*height))
     ThrowReaderException(CorruptImageError,NotAnARTImageFile,image);


 image->columns=width;
 image->rows=height;
 image->depth=1;
 image->colors=1l << image->depth;
 
/* printf("ART header checked OK %d,%d\n",image->colors,image->depth); */

 if (!AllocateImageColormap(image,image->colors)) goto NoMemory;

/* ----- Load RLE compressed raster ----- */
 BImgBuff=(unsigned char *) malloc(ldblk);  /*Ldblk was set in the check phase*/
 if(BImgBuff==NULL)
NoMemory:
  ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);

 for(i=0;i< (int) height;i++)
      {
      (void) ReadBlob(image,ldblk,(char *)BImgBuff);
      (void) ReadBlob(image,k,(char *)&dummy);
      InsertRow(BImgBuff,i,image);
      }
 if(BImgBuff!=NULL) free(BImgBuff);
 if (EOFBlob(image))
   ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
      image->filename);
 CloseBlob(image);
 return(image);
}



/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r A R T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterARTImage adds attributes for the ART image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterARTImage method is:
%
%      RegisterARTImage(void)
%
*/
ModuleExport void RegisterARTImage(void)
{
  MagickInfo
    *entry;

  static const char
    *ARTNote=
    {
      "Format originally used on the Macintosh (MacPaint?) and later\n"
      "used for PFS: 1st Publisher clip art.  NOT the AOL ART format."
    };

  entry=SetMagickInfo("ART");
  entry->decoder=(DecoderHandler) ReadARTImage;
  entry->description=AcquireString("PFS: 1st Publisher");
  entry->module=AcquireString("ART");
  entry->note=AcquireString(ARTNote);
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r A R T I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterARTImage removes format registrations made by the
%  ART module from the list of supported formats.
%
%  The format of the UnregisterARTImage method is:
%
%      UnregisterARTImage(void)
%
*/
ModuleExport void UnregisterARTImage(void)
{
  (void) UnregisterMagickInfo("ART");
}
