/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   SSSSS  33333                              %
%                            P   P  SS        33                              %
%                            PPPP    SSS    333                               %
%                            P         SS     33                              %
%                            P      SSSSS  33333                              %
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
%  Copyright 1999 E. I. du Pont de Nemours and Company                        %
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
%  E. I. du Pont de Nemours and Company be liable for any claim, damages or   %
%  other liability, whether in an action of contract, tort or otherwise,      %
%  arising from, out of or in connection with ImageMagick or the use or other %
%  dealings in ImageMagick.                                                   %
%                                                                             %
%  Except as contained in this notice, the name of the E. I. du Pont de       %
%  Nemours and Company shall not be used in advertising or otherwise to       %
%  promote the sale, use or other dealings in ImageMagick without prior       %
%  written authorization from the E. I. du Pont de Nemours and Company.       %
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
#if defined(HasTIFF)
#define CCITTParam  "-1"
#else
#define CCITTParam  "0"
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P S 3 I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePS3Image translates an image to encapsulated Postscript
%  Level III for printing.  If the supplied geometry is null, the image is
%  centered on the Postscript page.  Otherwise, the image is positioned as
%  specified by the geometry.
%
%  The format of the WritePS3Image routine is:
%
%      status=WritePS3Image(image_info,image)
%
%  A description of each parameter follows:
%
%    o status: Method WritePS3Image return True if the image is printed.
%      False is returned if the image file cannot be opened for printing.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export unsigned int WritePS3Image(const ImageInfo *image_info,Image *image)
{
  char
    date[MaxTextExtent],
    density[MaxTextExtent];

  CompressionType
    compression;

  double
    x_scale,
    y_scale;

  float
    dx_resolution,
    dy_resolution,
    x_resolution,
    y_resolution;

  int
    count,
    status,
    x,
    y;

  register RunlengthPacket
    *p;

  register int
    i,
    j;

  SegmentInfo
    bounding_box;

  time_t
    timer;

  unsigned char
    *pixels;

  unsigned int
    height,
    page,
    scene,
    text_size,
    width;

  unsigned long
    number_packets;

  if (!image->matte)
    WritePS2Image(image_info,image);
  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    WriterExit(FileOpenWarning,"Unable to open file",image);
  compression=image_info->compression;
#if defined(HasLZW)
  if (compression == UndefinedCompression)
    compression=LZWCompression;
#endif
  page=1;
  scene=0;
  do
  {
    /*
      Scale image to size of Postscript page.
    */
    text_size=0;
    if (image->label != (char *) NULL)
      text_size=MultilineCensus(image->label)*image_info->pointsize+12;
    width=image->columns;
    height=image->rows;
    x=0;
    y=text_size;
    if (image_info->page != (char *) NULL)
      (void) ParseImageGeometry(image_info->page,&x,&y,&width,&height);
    else
      if (image->page != (char *) NULL)
        (void) ParseImageGeometry(image->page,&x,&y,&width,&height);
      else
        (void) ParseImageGeometry(PSPageGeometry,&x,&y,&width,&height);
    /*
      Scale relative to dots-per-inch.
    */
    dx_resolution=72.0;
    dy_resolution=72.0;
    x_resolution=72.0;
    (void) strcpy(density,PSDensityGeometry);
    count=sscanf(density,"%fx%f",&x_resolution,&y_resolution);
    if (count != 2)
      y_resolution=x_resolution;
    if (image_info->density != (char *) NULL)
      {
        count=sscanf(image_info->density,"%fx%f",&x_resolution,&y_resolution);
        if (count != 2)
          y_resolution=x_resolution;
      }
    x_scale=(width*dx_resolution)/x_resolution;
    width=(unsigned int) (x_scale+0.5);
    y_scale=(height*dy_resolution)/y_resolution;
    height=(unsigned int) (y_scale+0.5);
    if (page == 1)
      {
        /*
          Output Postscript header.
        */
        (void) fprintf(image->file,"%%!PS-Adobe-3.0 Resource-ProcSet\n");
        (void) fprintf(image->file,"%%%%Creator: (ImageMagick)\n");
        (void) fprintf(image->file,"%%%%Title: (%.1024s)\n",image->filename);
        timer=time((time_t *) NULL);
        (void) localtime(&timer);
        (void) strcpy(date,ctime(&timer));
        date[Extent(date)-1]='\0';
        (void) fprintf(image->file,"%%%%CreationDate: (%.1024s)\n",date);
        bounding_box.x1=x;
        bounding_box.y1=y;
        bounding_box.x2=x+width-1;
        bounding_box.y2=y+(height+text_size)-1;
        if (image_info->adjoin && (image->next != (Image *) NULL))
          (void) fprintf(image->file,"%%%%BoundingBox: (atend)\n");
        else
          (void) fprintf(image->file,"%%%%BoundingBox: %g %g %g %g\n",
            bounding_box.x1,bounding_box.y1,bounding_box.x2,bounding_box.y2);
        if (image->label != (char *) NULL)
          (void) fprintf(image->file,
            "%%%%DocumentNeededResources: font Helvetica\n");
        (void) fprintf(image->file,"%%%%LanguageLevel: 2\n");
        (void) fprintf(image->file,"%%%%Orientation: Portrait\n");
        (void) fprintf(image->file,"%%%%PageOrder: Ascend\n");
        (void) fprintf(image->file,"%%%%Pages: %u\n",GetNumberScenes(image));
        (void) fprintf(image->file,"%%%%EndComments\n");
      }
    (void) fprintf(image->file,"%%%%Page:  1 %u\n",page++);
    (void) fprintf(image->file,"%%%%PageBoundingBox: %d %d %d %d\n",x,y,
      x+(int) width-1,y+(int) (height+text_size)-1);
    if (x < bounding_box.x1)
      bounding_box.x1=x;
    if (y < bounding_box.y1)
      bounding_box.y1=y;
    if ((x+(int) width-1) > bounding_box.x2)
      bounding_box.x2=x+width-1;
    if ((y+(int) (height+text_size)-1) > bounding_box.y2)
      bounding_box.y2=y+(height+text_size)-1;
    if (image->label != (char *) NULL)
      (void) fprintf(image->file,"%%%%PageResources: font Helvetica\n");
    /*
      Output image data.
    */
    (void) fprintf(image->file,"currentfile /ASCII85Decode filter ");
    if (compression != NoCompression)
      (void) fprintf(image->file,"/%s filter ",
        compression == ZipCompression ? "FlateDecode" :
        compression == LZWCompression ? "LZWDecode" : "RunLengthDecode");
    (void) fprintf(image->file,"/ReusableStreamDecode filter\n");
    p=image->pixels;
    switch (compression)
    {
      case RunlengthEncodedCompression:
      default:
      {
        register unsigned char
          *q;

        /*
          Allocate pixel array.
        */
        number_packets=4*image->columns*image->rows;
        pixels=(unsigned char *)
          AllocateMemory(number_packets*sizeof(unsigned char));
        if (pixels == (unsigned char *) NULL)
          WriterExit(ResourceLimitWarning,"Memory allocation failed",
            image);
        /*
          Dump Packbit encoded pixels.
        */
        q=pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
              *q++=DownScale(p->index);
              *q++=DownScale(p->red);
              *q++=DownScale(p->green);
              *q++=DownScale(p->blue);
          }
          p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(SaveImageText,i,image->packets);
        }
        if (compression == ZipCompression)
          status=ZLIBEncodeImage(image->file,number_packets,
            image_info->quality,pixels);
        else
          if (compression == LZWCompression)
            status=LZWEncodeImage(image->file,number_packets,pixels);
          else
            status=PackbitsEncodeImage(image->file,number_packets,pixels);
        if (!status)
          {
            CloseImage(image);
            return(False);
          }
        FreeMemory((char *) pixels);
        break;
      }
      case NoCompression:
      {
        /*
          Dump uncompressed DirectColor packets.
        */
        Ascii85Initialize();
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            Ascii85Encode(DownScale(p->index),image->file);
            Ascii85Encode(DownScale(p->red),image->file);
            Ascii85Encode(DownScale(p->green),image->file);
            Ascii85Encode(DownScale(p->blue),image->file);
          }
          p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(SaveImageText,i,image->packets);
        }
        Ascii85Flush(image->file);
        break;
      }
    }
    (void) fprintf(image->file,"\n");
    (void) fprintf(image->file,"\n");
    (void) fprintf(image->file,"/datastream exch def\n");
    (void) fprintf(image->file,"/DeviceRGB setcolorspace\n");
    (void) fprintf(image->file,"/ImageDataDictionary 8 dict def\n");
    (void) fprintf(image->file,"ImageDataDictionary begin\n");
    (void) fprintf(image->file,"  /ImageType 1 def\n");
    (void) fprintf(image->file,"  /Width %u def\n",image->columns);
    (void) fprintf(image->file,"  /Height %u def\n",image->rows);
    (void) fprintf(image->file,"  /BitsPerComponent 8 def\n");
    (void) fprintf(image->file,"  /DataSource datastream def\n");
    (void) fprintf(image->file,"  /MultipleDataSources false def\n");
    (void) fprintf(image->file,"  /ImageMatrix [ %d 0 0 %d neg 0 %d ] def\n",
      image->columns,image->rows,image->rows);
    (void) fprintf(image->file,"  /Decode [ 0 1 0 1 0 1 ] def\n");
    (void) fprintf(image->file,"end\n");
    (void) fprintf(image->file,"\n");
    (void) fprintf(image->file,"/ImageMaskDictionary 8 dict def\n");
    (void) fprintf(image->file,"ImageMaskDictionary begin\n");
    (void) fprintf(image->file,"  /ImageType 1 def\n");
    (void) fprintf(image->file,"  /Width %u def\n",image->columns);
    (void) fprintf(image->file,"  /Height %u def\n",image->rows);
    (void) fprintf(image->file,"  /BitsPerComponent 8 def\n");
    (void) fprintf(image->file,"  /MultipleDataSources false def\n");
    (void) fprintf(image->file,"  /ImageMatrix [ %d 0 0 %d neg 0 %d ] def\n",
      image->columns,image->rows,image->rows);
    (void) fprintf(image->file,"  /Decode [ 0 1 ] def\n");
    (void) fprintf(image->file,"end\n");
    (void) fprintf(image->file,"\n");
    (void) fprintf(image->file,"/MaskedImageDictionary 7 dict def\n");
    (void) fprintf(image->file,"MaskedImageDictionary begin\n");
    (void) fprintf(image->file,"  /ImageType 3 def\n");
    (void) fprintf(image->file,"  /InterleaveType 1 def\n");
    (void) fprintf(image->file,"  /MaskDict ImageMaskDictionary def\n");
    (void) fprintf(image->file,"  /DataDict ImageDataDictionary def\n");
    (void) fprintf(image->file,"end\n");
    (void) fprintf(image->file,"\n");
    (void) fprintf(image->file,"gsave\n");
    (void) fprintf(image->file,"%d %d translate\n",x,y);
    (void) fprintf(image->file,"%f %f scale\n",x_scale,y_scale);
    (void) fprintf(image->file,"ImageMaskDictionary /Decode [ 1 0 ] put\n");
    (void) fprintf(image->file,"MaskedImageDictionary image\n");
    (void) fprintf(image->file,"grestore                    \n");
    (void) fprintf(image->file,"\n");
    (void) fprintf(image->file,"showpage\n");
    (void) fprintf(image->file,"%%%%EndData\n");
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  if (page > 1)
    (void) fprintf(image->file,"%%%%BoundingBox: %g %g %g %g\n",
      bounding_box.x1,bounding_box.y1,bounding_box.x2,bounding_box.y2);
  (void) fprintf(image->file,"%%%%EOF\n");
  CloseImage(image);
  return(True);
}
