/*
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
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                                January 2001                                 %
%                                                                             %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"


static void InsertRow(unsigned char *p,int y,Image *image)
{
int bit,x;
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
	     for (x=0; x < ((int) image->columns-7); x+=8)
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
		 for (bit=0; bit < (int) (image->columns % 8); bit++)
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
%    o image_info: Specifies a pointer to an ImageInfo structure.
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

  /*  Open image file. */
  image=AllocateImage(image_info);

  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read ART image.
  */
  dummy=ReadBlobLSBShort(image);
  width=ReadBlobLSBShort(image);
  dummy=ReadBlobLSBShort(image);
  height=ReadBlobLSBShort(image);

  ldblk=(width+7) / 8;
  k=(-ldblk) & 1;

   if(image->blob->filesize!=(8+((long)ldblk+k)*height))
     ThrowReaderException(CorruptImageWarning,"Not a ART image file",image);


 image->columns=width;
 image->rows=height;
 image->depth=1;
 image->colors=1l << image->depth;
 
/* printf("ART header checked OK %d,%d\n",image->colors,image->depth); */

 if (!AllocateImageColormap(image,image->colors)) goto NoMemory;

/* ----- Load RLE compressed raster ----- */
 BImgBuff=(unsigned char *) malloc(ldblk);  /*Ldblk was set in the check phase*/
 if(BImgBuff==NULL)
NoMemory: ThrowReaderException(CorruptImageWarning,"Memory exhausted",image);

 for(i=0;i< (int) height;i++)
      {
      ReadBlob(image,ldblk,(char *)BImgBuff);
      ReadBlob(image,k,(char *)&dummy);
      InsertRow(BImgBuff,i,image);
      }
 if(BImgBuff!=NULL) free(BImgBuff);
 if (EOFBlob(image))
   ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
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

  entry=SetMagickInfo("ART");
  entry->decoder=ReadARTImage;
  entry->description=AllocateString("PF1: 1st Publisher");
  entry->module=AllocateString("ART");
  RegisterMagickInfo(entry);
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
  UnregisterMagickInfo("ART");
}
