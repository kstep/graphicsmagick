/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                             AAA   V   V  IIIII                              %
%                            A   A  V   V    I                                %
%                            AAAAA  V   V    I                                %
%                            A   A   V V     I                                %
%                            A   A    V    IIIII                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
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
#include "defines.h"

/*
  Define declarations.
*/
#define RIFF_RIFF 0x52494646
#define RIFF_LIST 0x4C495354
#define RIFF_movi 0x6D6F7669

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s A V I                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsAVI returns True if the image format type, identified by the
%  magick string, is Audio/Video Interleaved file format.
%
%  The format of the IsAVI method is:
%
%      unsigned int IsAVI(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsAVI returns True if the image format type is AVI.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsAVI(const unsigned char *magick,
  const unsigned int length)
{
  if (length < 4)
    return(False);
  if (LocaleNCompare((char *) magick,"RIFF",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d A V I I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadAVIImage reads an AVI X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadAVIImage method is:
%
%      Image *ReadAVIImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadAVIImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadAVIImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    id[5];

  Image
    *image;

  off_t
    chunk_size,
    movi_offset,
    riff_size;

  unsigned int
    status;

  unsigned long
    use_index_flag;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  id[4]='\0';
  movi_offset=0;
  riff_size=1;
  use_index_flag=False;
  while ((riff_size > 0) && !EOFBlob(image))
  {
    (void) ReadBlob(image,4,id);
    chunk_size=LSBFirstReadLong(image);
    riff_size-=8;
    if (image_info->verbose)
      {
        (void) fprintf(stdout,"AVI cid %s\n",id);
        (void) fprintf(stdout,"  chunk size %08llx\n",chunk_size);
      }
    if (LocaleCompare(id,"RIFF") == 0)
      {
        (void) ReadBlob(image,4,id);
        riff_size=2*chunk_size-4;
        if (image_info->verbose)
          (void) fprintf(stdout,"  RIFF form type %s\n",id);
        continue;
      }
    if (LocaleCompare(id,"LIST") == 0)
      {
        (void) ReadBlob(image,4,id);
        if ((LocaleCompare(id,"movi") == 0) || !use_index_flag)
          riff_size+=(chunk_size-4);  /* don't count LISTs */
        else
          {
            off_t
              offset;

            movi_offset=TellBlob(image)-4;
            (void) SeekBlob(image,0,SEEK_END);
            offset=TellBlob(image);
            if (image_info->verbose)
              (void) fprintf(stdout,
                "movi LIST: eof %d movi_offset %d ck_size %d\n",
                offset,movi_offset,chunk_size);
            if (offset >= (movi_offset+chunk_size))
              {
                if (chunk_size & 0x01)
                  chunk_size++;
                (void) SeekBlob(image,movi_offset+chunk_size,SEEK_SET);
              }
            else
              {
                use_index_flag=False;
                (void) SeekBlob(image,movi_offset+4,SEEK_SET);
                riff_size+=chunk_size-4;
              }
          }
        if (image_info->verbose)
          (void) fprintf(stdout,"  List type %s\n",id);
        continue;
      }
    ThrowReaderException(CorruptImageWarning,
      "AVI support for this chunk not yet available",image);
  }
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r A V I I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterAVIImage adds attributes for the AVI image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterAVIImage method is:
%
%      RegisterAVIImage(void)
%
*/
ModuleExport void RegisterAVIImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("AVI");
  entry->decoder=ReadAVIImage;
  entry->magick=IsAVI;
  entry->description=AllocateString("Audio/Visual Interleaved");
  entry->module=AllocateString("AVI");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r A V I I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterAVIImage removes format registrations made by the
%  AVI module from the list of supported formats.
%
%  The format of the UnregisterAVIImage method is:
%
%      UnregisterAVIImage(void)
%
*/
ModuleExport void UnregisterAVIImage(void)
{
  UnregisterMagickInfo("AVI");
}
