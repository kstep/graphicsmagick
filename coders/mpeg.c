/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        M   M  PPPP   EEEEE   GGGG                           %
%                        MM MM  P   P  E      G                               %
%                        M M M  PPPP   EEE    G  GG                           %
%                        M   M  P      E      G   G                           %
%                        M   M  P      EEEEE   GGGG                           %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1999                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
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
#include "define.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteMPEGImage(const ImageInfo *image_info,Image *image);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M P E G                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMPEG returns True if the image format type, identified by the
%  magick string, is MPEG.
%
%  The format of the IsMPEG method is:
%
%      unsigned int IsMPEG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsMPEG returns True if the image format type is MPEG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsMPEG(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\000\000\001\263",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M P E G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMPEGImage reads an binary file in the MPEG video stream format
%  and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.  This method differs from
%  the other decoder methods in that only the Photoshop resource (MPEG)
%  information is useful in the returned image.
%
%  The format of the ReadMPEGImage method is:
%
%      Image *ReadMPEGImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMPEGImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMPEGImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image,
    *next_image;

  ImageInfo
    *clone_info;

  register Image
    *p;

  register long
    i;

  unsigned int
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  CloseBlob(image);
  /*
    Convert MPEG to PPM with delegate.
  */
  image=AllocateImage(image_info);
  clone_info=CloneImageInfo(image_info);
  (void) InvokeDelegate(clone_info,image,"mpeg-decode",(char *) NULL,exception);
  DestroyImage(image);
  /*
    Read PPM files.
  */
  image=(Image *) NULL;
  for (i=(long) clone_info->subimage; ; i++)
  {
    FormatString(clone_info->filename,"%.1024s%d.ppm",clone_info->unique,i);
    if (!IsAccessible(clone_info->filename))
      break;
    next_image=ReadImage(clone_info,exception);
    if (next_image == (Image *) NULL)
      break;
    next_image->scene=i;
    if (image == (Image *) NULL)
      image=next_image;
    else
      {
        /*
          Link image into image list.
        */
        for (p=image; p->next != (Image *) NULL; p=p->next);
        next_image->previous=p;
        p->next=next_image;
      }
    if (clone_info->subrange != 0)
      if (i >= (long) (clone_info->subimage+clone_info->subrange-1))
        break;
  }
  while (image->previous != (Image *) NULL)
    image=image->previous;
  /*
    Free resources.
  */
  for (i=0; ; i++)
  {
    FormatString(clone_info->filename,"%.1024s%d.ppm",clone_info->unique,i);
    if (!IsAccessible(clone_info->filename))
      break;
    (void) remove(clone_info->filename);
  }
  DestroyImageInfo(clone_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M P E G I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMPEGImage adds attributes for the MPEG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMPEGImage method is:
%
%      RegisterMPEGImage(void)
%
*/
ModuleExport void RegisterMPEGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MPEG");
  entry->decoder=ReadMPEGImage;
  entry->encoder=WriteMPEGImage;
  entry->magick=IsMPEG;
  entry->description=AllocateString("MPEG Video Stream");
  entry->module=AllocateString("MPEG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("MPG");
  entry->decoder=ReadMPEGImage;
  entry->encoder=WriteMPEGImage;
  entry->magick=IsMPEG;
  entry->description=AllocateString("MPEG Video Stream");
  entry->module=AllocateString("MPEG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("M2V");
  entry->decoder=ReadMPEGImage;
  entry->encoder=WriteMPEGImage;
  entry->magick=IsMPEG;
  entry->description=AllocateString("MPEG Video Stream");
  entry->module=AllocateString("MPEG");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M P E G I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMPEGImage removes format registrations made by the
%  BIM module from the list of supported formats.
%
%  The format of the UnregisterBIMImage method is:
%
%      UnregisterMPEGImage(void)
%
*/
ModuleExport void UnregisterMPEGImage(void)
{
  (void) UnregisterMagickInfo("MPEG");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M P E G I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMPEGImage writes an image to a file in MPEG video stream format.
%  Lawrence Livermore National Laboratory (LLNL) contributed code to adjust
%  the MPEG parameters to correspond to the compression quality setting.
%
%  The format of the WriteMPEGImage method is:
%
%      unsigned int WriteMPEGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteMPEGImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

static unsigned int WriteMPEGParameterFiles(const ImageInfo *image_info,
  Image *image)
{
  char
    filename[MaxTextExtent];

  double
    q;

  FILE
    *file,
    *parameter_file;

  int
    quant;

  register Image
    *p;

  register int
    i;

  static int
    q_matrix[]=
    {
       8, 16, 19, 22, 26, 27, 29, 34,
      16, 16, 22, 24, 27, 29, 34, 37,
      19, 22, 26, 27, 29, 34, 34, 38,
      22, 22, 26, 27, 29, 34, 37, 40,
      22, 26, 27, 29, 32, 35, 40, 48,
      26, 27, 29, 32, 35, 40, 48, 58,
      26, 27, 29, 34, 38, 46, 56, 69,
      27, 29, 35, 38, 46, 56, 69, 83
    };

  unsigned int
    mpeg;

  unsigned long
    count;

  /*
    Write parameter file (see mpeg2encode documentation for details).
  */
  file=fopen(image_info->unique,"w");
  if (file == (FILE *) NULL)
    return(False);
  (void) fprintf(file,"MPEG\n");  /* comment */
  (void) fprintf(file,"%.1024s.%%lu\n",image->filename); /* source frame file */
  (void) fprintf(file,"-\n");  /* reconstructed frame file */
  if (image_info->quality == DefaultCompressionQuality)
    (void) fprintf(file,"-\n");  /* default intra quant matrix */
  else
    {
      /*
        Write intra quant matrix file.
      */
      FormatString(filename,"%.1024s.iqm",image_info->unique);
      (void) fprintf(file,"%s\n",filename);
      parameter_file=fopen(filename,"w");
      if (parameter_file == (FILE *) NULL)
        return(False);
      if (image_info->quality < DefaultCompressionQuality)
        {
          q=Max((DefaultCompressionQuality-image_info->quality)/8.0,1.0);
          for (i=0; i < 64; i++)
          {
            quant=(int) Min(Max(q*q_matrix[i]+0.5,1.0),255.0);
            (void) fprintf(parameter_file," %d",quant);
            if ((i % 8) == 7)
              (void) fprintf(parameter_file,"\n");
          }
        }
      else
        {
          q=Max((image_info->quality-DefaultCompressionQuality)*2.0,1.0);
          for (i=0; i < 64; i++)
          {
            quant=(int) Min(Max(q_matrix[i]/q,1.0),255.0);
            (void) fprintf(parameter_file," %d",quant);
            if ((i % 8) == 7)
              (void) fprintf(parameter_file,"\n");
          }
        }
      (void) fclose(parameter_file);
    }
  if (image_info->quality == DefaultCompressionQuality)
    (void) fprintf(file,"-\n");  /* default non intra quant matrix */
  else
    {
      /*
        Write non intra quant matrix file.
      */
      FormatString(filename,"%.1024s.niq",image_info->unique);
      (void) fprintf(file,"%s\n",filename);
      parameter_file=fopen(filename,"w");
      if (parameter_file == (FILE *) NULL)
        return(False);
      q=Min(Max(66.0-(2*image_info->quality)/3.0,1.0),255);
      for (i=0; i < 64; i++)
      {
        (void) fprintf(parameter_file," %d",(int) q);
        if ((i % 8) == 7)
          (void) fprintf(parameter_file,"\n");
      }
      (void) fclose(parameter_file);
    }
  (void) fprintf(file,"%.1024s.log\n",image_info->unique);  /* statistics log */
  (void) fprintf(file,"1\n");  /* input picture file format */
  count=0;
  for (p=image; p != (Image *) NULL; p=p->next)
    count+=Max((p->delay+1)/3,1);
  (void) fprintf(file,"%lu\n",count); /* number of frames */
  (void) fprintf(file,"0\n");  /* number of first frame */
  (void) fprintf(file,"00:00:00:00\n");  /* timecode of first frame */
  mpeg=LocaleCompare(image->magick,"M2V") != 0;
  if (image_info->quality > 98)
    (void) fprintf(file,"1\n");
  else
    (void) fprintf(file,"%d\n",mpeg ? 12 : 15);
  if (image_info->quality > 98)
    (void) fprintf(file,"1\n");
  else
    (void) fprintf(file,"3\n");
  (void) fprintf(file,"%d\n",mpeg ? 1 : 0);  /* ISO/IEC 11172-2 stream */
  (void) fprintf(file,"0\n");  /* select frame picture coding */
  (void) fprintf(file,"%lu\n",image->columns+(image->columns & 0x01 ? 1 : 0));
  (void) fprintf(file,"%lu\n",image->rows+(image->rows & 0x01 ? 1 : 0));
  (void) fprintf(file,"%d\n",mpeg ? 8 : 2);  /* aspect ratio */
  (void) fprintf(file,"%d\n",mpeg ? 3 : 5);  /* frame rate code */
  (void) fprintf(file,"%.1f\n",mpeg ? 1152000.0 : 5000000.0);  /* bit rate */
  (void) fprintf(file,"%d\n",mpeg ? 20 : 112);  /* vbv buffer size */
  (void) fprintf(file,"0\n");  /* low delay */
  (void) fprintf(file,"%d\n",mpeg ? 1 : 0);  /* constrained parameter */
  (void) fprintf(file,"%d\n",mpeg ? 4 : 1);  /* profile ID */
  (void) fprintf(file,"%d\n",mpeg ? 8 : 4);  /* level ID */
  (void) fprintf(file,"%d\n",mpeg ? 1 : 0);  /* progressive sequence */
  (void) fprintf(file,"1\n");  /* chrome format */
  (void) fprintf(file,"%d\n",mpeg ? 1 : 2);  /* video format */
  (void) fprintf(file,"5\n");  /* color primaries */
  (void) fprintf(file,"5\n");  /* transfer characteristics */
  (void) fprintf(file,"%d\n",mpeg ? 5 : 4);  /* matrix coefficients */
  (void) fprintf(file,"%lu\n",image->columns+(image->columns & 0x01 ? 1 : 0));
  (void) fprintf(file,"%lu\n",image->rows+(image->rows & 0x01 ? 1 : 0));
  (void) fprintf(file,"0\n");  /* intra dc precision */
  (void) fprintf(file,"%d\n",mpeg ? 0 : 1);  /* top field */
  (void) fprintf(file,"%d %d %d\n",mpeg ? 1 : 0,mpeg ? 1 : 0, mpeg ? 1 : 0);
  (void) fprintf(file,"0 0 0\n");  /* concealment motion vector */
  (void) fprintf(file,"%d %d %d\n",mpeg ? 0 : 1,mpeg ? 0 : 1,mpeg ? 0 : 1);
  (void) fprintf(file,"%d 0 0\n",mpeg ? 0 : 1);  /* intra vlc format */
  (void) fprintf(file,"0 0 0\n");  /* alternate scan */
  (void) fprintf(file,"0\n");  /* repeat first field */
  (void) fprintf(file,"%d\n",mpeg ? 1 : 0);  /* progressive frame */
  (void) fprintf(file,"0\n");  /* intra slice refresh period */
  (void) fprintf(file,"0\n");  /* reaction parameter */
  (void) fprintf(file,"0\n");  /* initial average activity */
  (void) fprintf(file,"0\n");
  (void) fprintf(file,"0\n");
  (void) fprintf(file,"0\n");
  (void) fprintf(file,"0\n");
  (void) fprintf(file,"0\n");
  (void) fprintf(file,"0\n");
  (void) fprintf(file,"2 2 11 11\n");
  (void) fprintf(file,"1 1 3 3\n");
  (void) fprintf(file,"1 1 7 7\n");
  (void) fprintf(file,"1 1 7 7\n");
  (void) fprintf(file,"1 1 3 3\n");
  (void) fclose(file);
  return(True);
}

static unsigned int WriteMPEGImage(const ImageInfo *image_info,Image *image)
{
  char
    basename[MaxTextExtent],
    *blob,
    filename[MaxTextExtent];


  Image
    *coalesce_image,
    *next_image;

  ImageInfo
    *clone_info;

  int
    file;

  register Image
    *p;

  register long
    i;

  size_t
    length;

  unsigned int
    status;

  unsigned long
    count,
    scene;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  CloseBlob(image);
  /*
    Determine if the sequence of images have identical page info.
  */
  coalesce_image=image;
  for (next_image=image; next_image != (Image *) NULL; )
  {
    if ((image->columns != next_image->columns) ||
        (image->rows != next_image->rows))
      break;
    if ((image->page.x != next_image->page.x) ||
        (image->page.y != next_image->page.y))
      break;
    next_image=next_image->next;
  }
  if (next_image != (Image *) NULL)
    {
      coalesce_image=CoalesceImages(image,&image->exception);
      if (coalesce_image == (Image *) NULL)
        return(False);
    }
  /*
    Write YUV files.
  */
  TemporaryFilename(basename);
  FormatString(coalesce_image->filename,basename);
  clone_info=CloneImageInfo(image_info);
  (void) strncpy(clone_info->unique,basename,MaxTextExtent-1);
  status=WriteMPEGParameterFiles(clone_info,coalesce_image);
  if (status == False)
    {
      if (coalesce_image != image)
        DestroyImage(coalesce_image);
      ThrowWriterException(DelegateWarning,"Unable to write MPEG parameters",
        image)
    }
  count=0;
  for (p=coalesce_image; p != (Image *) NULL; p=p->next)
  {
    blob=(char *) NULL;
    length=0;
    scene=p->scene;
    for (i=0; i < (long) Max((p->delay+1)/3,1); i++)
    {
      p->scene=count++;
      FormatString(p->filename,"%.1024s.%%lu.yuv",basename);
      switch (i)
      {
        case 0:
        {
          status=WriteImage(clone_info,p);
          FormatString(filename,"%.1024s.%lu.yuv",basename,p->scene);
          break;
        }
        case 1:
          blob=(char *) FileToBlob(filename,&length,&image->exception);
        default:
        {
          FormatString(filename,"%.1024s.%lu.yuv",basename,p->scene);
	  file=open(filename,O_WRONLY | O_CREAT | O_BINARY,0777);
          if (file == -1)
            break;
          (void) write(file,blob,length);
          (void) close(file);
          break;
        }
      }
    }
    p->scene=scene;
    if (blob != (char *) NULL)
      LiberateMemory((void **) &blob);
    if (status == False)
      break;
  }
  /*
    Convert YUV to MPEG.
  */
  (void) strncpy(coalesce_image->filename,clone_info->unique,MaxTextExtent-1);
  status=InvokeDelegate(clone_info,coalesce_image,(char *) NULL,"mpeg-encode",
    &image->exception);
  DestroyImageInfo(clone_info);
  /*
    Free resources.
  */
  count=0;
  for (p=coalesce_image; p != (Image *) NULL; p=p->next)
  {
    for (i=0; i < (long) Max((p->delay+1)/3,1); i++)
    {
      FormatString(p->filename,"%.1024s.%lu.yuv",basename,count++);
      (void) remove(p->filename);
    }
    (void) strncpy(p->filename,image_info->filename,MaxTextExtent-1);
  }
  (void) remove(basename);
  FormatString(filename,"%.1024s.iqm",basename);
  (void) remove(filename);
  FormatString(filename,"%.1024s.niq",basename);
  (void) remove(filename);
  FormatString(filename,"%.1024s.log",basename);
  (void) remove(filename);
  if (coalesce_image != image)
    DestroyImage(coalesce_image);
  return(status);
}
