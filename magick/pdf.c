/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   DDDD   FFFFF                              %
%                            P   P  D   D  F                                  %
%                            PPPP   D   D  FFF                                %
%                            P      D   D  F                                  %
%                            P      DDDD   F                                  %
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
^L
/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"
#include "proxy.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P D F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPDFImage reads a Portable Document Format image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadPDFImage routine is:
%
%      image=ReadPDFImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPDFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Export Image *ReadPDFImage(const ImageInfo *image_info)
{
#define MediaBox  "/MediaBox ["

  char
    density[MaxTextExtent],
    command[MaxTextExtent],
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    options[MaxTextExtent],
    postscript_filename[MaxTextExtent];

  DelegateInfo
    delegate_info;

  double
    dx_resolution,
    dy_resolution;

  FILE
    *file;

  Image
    *image,
    *next_image;

  int
    count,
    status;

  long int
    filesize;

  RectangleInfo
    box,
    page_info;

  register char
    *p;

  register int
    c;

  SegmentInfo
    bounding_box;

  unsigned int
    height,
    portrait,
    width;

  if (image_info->monochrome)
    {
      if (!GetDelegateInfo("gs-mono",(char *) NULL,&delegate_info))
        return((Image *) NULL);
    }
  else
    if (!GetDelegateInfo("gs-color",(char *) NULL,&delegate_info))
      return((Image *) NULL);
  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    ReaderExit(FileOpenWarning,"Unable to open file",image);
  /*
    Open temporary output file.
  */
  TemporaryFilename(postscript_filename);
  file=fopen(postscript_filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    ReaderExit(FileOpenWarning,"Unable to write file",image);
  /*
    Set the page geometry.
  */
  dx_resolution=72.0;
  dy_resolution=72.0;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
     (void) strcpy(density,PSDensityGeometry);
      count=sscanf(density,"%fx%f",&image->x_resolution,&image->y_resolution);
      if (count != 2)
        image->y_resolution=image->x_resolution;
    }
  FormatString(density,"%gx%g",image->x_resolution,image->y_resolution);
  page_info.width=612;
  page_info.height=792;
  page_info.x=0;
  page_info.y=0;
  (void) ParseImageGeometry(PSPageGeometry,&page_info.x,&page_info.y,
    &page_info.width,&page_info.height);
  portrait=True;
  /*
    Determine page geometry from the PDF media box.
  */
  box.width=0;
  box.height=0;
  for (p=command; ; )
  {
    c=fgetc(image->file);
    if (c == EOF)
      break;
    (void) fputc(c,file);
    *p++=(char) c;
    if ((c != '\n') && (c != '\r') && ((p-command) < (MaxTextExtent-1)))
      continue;
    *p='\0';
    p=command;
    /*
      Continue unless this is a MediaBox statement.
    */
    if (strncmp(command,"/Rotate 90",10) == 0)
      portrait=False;
    if (strncmp(MediaBox,command,Extent(MediaBox)) != 0)
      continue;
    count=sscanf(command,"/MediaBox [ %f %f %f %f",&bounding_box.x1,
      &bounding_box.y1,&bounding_box.x2,&bounding_box.y2);
    if (count != 4)
      continue;
    if ((bounding_box.x1 > bounding_box.x2) ||
        (bounding_box.y1 > bounding_box.y2))
      continue;
    /*
      Set Postscript render geometry.
    */
    width=(unsigned int) (bounding_box.x2-bounding_box.x1+1);
    if ((float) ((int) bounding_box.x2) != bounding_box.x2)
      width++;
    height=(unsigned int) (bounding_box.y2-bounding_box.y1+1);
    if ((float) ((int) bounding_box.y2) != bounding_box.y2)
      height++;
    if ((width <= box.width) && (height <= box.height))
      continue;
    page_info.width=width;
    page_info.height=height;
    box=page_info;
  }
  if (image_info->page != (char *) NULL)
    (void) ParseImageGeometry(image_info->page,&page_info.x,&page_info.y,
      &page_info.width,&page_info.height);
  FormatString(geometry,"%ux%u",(unsigned int) (((page_info.width*
    image->x_resolution)/dx_resolution)+0.5),(unsigned int)
    (((page_info.height*image->y_resolution)/dy_resolution)+0.5));
  if (ferror(file))
    {
      MagickWarning(FileOpenWarning,"An error has occurred writing to file",
        postscript_filename);
      (void) fclose(file);
      return((Image *) NULL);
    }
  (void) fclose(file);
  CloseImage(image);
  filesize=image->filesize;
  DestroyImage(image);
  /*
    Use Ghostscript to convert Postscript image.
  */
  *options='\0';
  if (image_info->subrange != 0)
    FormatString(options,"-dFirstPage=%u -dLastPage=%u",
      image_info->subimage+1,image_info->subimage+image_info->subrange);
  (void) strcpy(filename,image_info->filename);
  TemporaryFilename((char *) image_info->filename);
  FormatString(command,delegate_info.commands,image_info->antialias ? 4 : 1,
    image_info->antialias ? 4 : 1,geometry,density,options,image_info->filename,
    postscript_filename);
  ProgressMonitor(RenderPostscriptText,0,8);
  status=SystemCommand(image_info->verbose,command);
  ProgressMonitor(RenderPostscriptText,7,8);
  if (status)
    {
      MagickWarning(CorruptImageWarning,"Portable Document delegation failed",
        image_info->filename);
      (void) remove(postscript_filename);
      return((Image *) NULL);
    }
  image=ReadPNMImage(image_info);
  (void) remove(postscript_filename);
  (void) remove(image_info->filename);
  if (image == (Image *) NULL)
    {
      MagickWarning(CorruptImageWarning,"Portable Document delegation failed",
        image_info->filename);
      return((Image *) NULL);
    }
  (void) strcpy((char *) image_info->filename,filename);
  do
  {
    (void) strcpy(image->magick,"PDF");
    (void) strcpy(image->filename,image_info->filename);
    image->filesize=filesize;
    if (!portrait)
      {
        Image
          *rotated_image;

        /*
          Rotate image.
        */
        image->orphan=True;
        rotated_image=RotateImage(image,90,False,True);
        image->orphan=False;
        if (rotated_image != (Image *) NULL)
          {
            DestroyImage(image);
            image=rotated_image;
          }
      }
    next_image=image->next;
    if (next_image != (Image *) NULL)
      image=next_image;
  } while (next_image != (Image *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P D F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePDFImage writes an image in the Portable Document image
%  format.
%
%  The format of the WritePDFImage routine is:
%
%      status=WritePDFImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePDFImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
Export unsigned int WritePDFImage(const ImageInfo *image_info,Image *image)
{
#define ObjectsPerImage  12

  char
    date[MaxTextExtent],
    density[MaxTextExtent],
    geometry[MaxTextExtent],
    **labels;

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

  Image
    encode_image,
    *tile_image;

  RectangleInfo
    media_info;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  register int
    i,
    j;

  time_t
    timer;

  unsigned char
    *pixels;

  unsigned int
    height,
    info_id,
    object,
    pages_id,
    root_id,
    scene,
    text_size,
    width;

  unsigned long
    length,
    number_packets,
    *xref;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    WriterExit(FileOpenWarning,"Unable to open file",image);
  if ((image->file == stdout) || image->pipe)
    {
      /*
        Write standard output or pipe to temporary file.
      */
      encode_image=(*image);
      TemporaryFilename(image->filename);
      image->temporary=True;
      OpenImage(image_info,image,WriteBinaryType);
      if (image->file == (FILE *) NULL)
        WriterExit(FileOpenWarning,"Unable to open file",image);
    }
  compression=image_info->compression;
#if defined(HasZLIB)
  if (compression == UndefinedCompression)
    compression=ZipCompression;
#else
#if defined(HasLZW)
  if (compression == UndefinedCompression)
    compression=LZWCompression;
#endif
#endif
  /*
    Allocate X ref memory.
  */
  xref=(unsigned long *) AllocateMemory(2048*sizeof(unsigned long));
  if (xref == (unsigned long *) NULL)
    WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Write Info object.
  */
  object=0;
  (void) fprintf(image->file,"%%PDF-1.1 \n");
  xref[object++]=ftell(image->file);
  info_id=object;
  (void) fprintf(image->file,"%u 0 obj\n",object);
  (void) fprintf(image->file,"<<\n");
  timer=time((time_t *) NULL);
  (void) localtime(&timer);
  (void) strcpy(date,ctime(&timer));
  date[Extent(date)-1]='\0';
  (void) fprintf(image->file,"/CreationDate (%.1024s)\n",date);
  (void) fprintf(image->file,"/Producer (%.1024s)\n",MagickVersion);
  (void) fprintf(image->file,">>\n");
  (void) fprintf(image->file,"endobj\n");
  /*
    Write Catalog object.
  */
  xref[object++]=ftell(image->file);
  root_id=object;
  (void) fprintf(image->file,"%u 0 obj\n",object);
  (void) fprintf(image->file,"<<\n");
  (void) fprintf(image->file,"/Type /Catalog\n");
  (void) fprintf(image->file,"/Pages %u 0 R\n",object+1);
  (void) fprintf(image->file,">>\n");
  (void) fprintf(image->file,"endobj\n");
  /*
    Write Pages object.
  */
  xref[object++]=ftell(image->file);
  pages_id=object;
  (void) fprintf(image->file,"%u 0 obj\n",object);
  (void) fprintf(image->file,"<<\n");
  (void) fprintf(image->file,"/Type /Pages\n");
  (void) fprintf(image->file,"/Kids [ %u 0 R ",object+1);
  count=pages_id+ObjectsPerImage+1;
  if (image_info->adjoin)
    {
      Image
        *kid_image;

      /*
        Predict page object id's.
      */
      kid_image=image;
      for ( ; kid_image->next != (Image *) NULL; count+=ObjectsPerImage)
      {
        (void) fprintf(image->file,"%d 0 R ",count);
        kid_image=kid_image->next;
      }
      xref=(unsigned long *)
        ReallocateMemory((char *) xref,(count+2048)*sizeof(unsigned long));
      if (xref == (unsigned long *) NULL)
        WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
    }
  (void) fprintf(image->file,"]\n");
  (void) fprintf(image->file,"/Count %u\n",(count-pages_id)/ObjectsPerImage);
  (void) fprintf(image->file,">>\n");
  (void) fprintf(image->file,"endobj\n");
  scene=0;
  do
  {
    /*
      Scale image to size of Portable Document page.
    */
    text_size=0;
    if (image->label != (char *) NULL)
      text_size=MultilineCensus(image->label)*image_info->pointsize+12;
    width=image->columns;
    height=image->rows;
    x=0;
    y=text_size;
    FormatString(geometry,"%ux%u",image->columns,image->rows);
    if (image_info->page != (char *) NULL)
      (void) strcpy(geometry,image_info->page);
    else
      if (image->page != (char *) NULL)
        (void) strcpy(geometry,image->page);
      else
        if (Latin1Compare(image_info->magick,"PDF") == 0)
          (void) strcpy(geometry,PSPageGeometry);
    (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
    (void) GetGeometry(geometry,&media_info.x,&media_info.y,
      &media_info.width,&media_info.height);
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
    /*
      Write Page object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"<<\n");
    (void) fprintf(image->file,"/Type /Page\n");
    (void) fprintf(image->file,"/Parent %u 0 R\n",pages_id);
    (void) fprintf(image->file,"/Resources <<\n");
    (void) fprintf(image->file,"/Font << /F%u %u 0 R >>\n",image->scene,
      object+4);
    (void) fprintf(image->file,"/XObject << /Im%u %u 0 R >>\n",image->scene,
      object+5);
    (void) fprintf(image->file,"/ProcSet %u 0 R >>\n",object+3);
    (void) fprintf(image->file,"/MediaBox [ %d %d %d %d ]\n",0,0,
      media_info.width,media_info.height);
    (void) fprintf(image->file,"/Contents %u 0 R\n",object+1);
    (void) fprintf(image->file,"/Thumb %u 0 R\n",object+8);
    (void) fprintf(image->file,">>\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Contents object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"<<\n");
    (void) fprintf(image->file,"/Length %u 0 R\n",object+1);
    (void) fprintf(image->file,">>\n");
    (void) fprintf(image->file,"stream\n");
    length=ftell(image->file);
    (void) fprintf(image->file,"q\n");
    labels=StringToList(image->label);
    if (labels != (char **) NULL)
      {
        for (i=0; labels[i] != (char *) NULL; i++)
        {
          (void) fprintf(image->file,"BT\n");
          (void) fprintf(image->file,"/F%u %u Tf\n",image->scene,
            image_info->pointsize);
          (void) fprintf(image->file,"%d %u Td\n",x,y+height+
            i*image_info->pointsize+12);
          (void) fprintf(image->file,"(%.1024s) Tj\n",labels[i]);
          (void) fprintf(image->file,"ET\n");
          FreeMemory(labels[i]);
        }
        FreeMemory((char *) labels);
      }
    (void) fprintf(image->file,"%f 0 0 %f %d %d cm\n",x_scale,y_scale,x,y);
    (void) fprintf(image->file,"/Im%u Do\n",image->scene);
    (void) fprintf(image->file,"Q\n");
    length=ftell(image->file)-length;
    (void) fprintf(image->file,"endstream\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"%lu\n",length);
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Procset object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      (void) fprintf(image->file,"[ /PDF /Text /ImageC");
    else
      if (IsFaxImage(image))
        (void) fprintf(image->file,"[ /PDF /Text /ImageB");
      else
        (void) fprintf(image->file,"[ /PDF /Text /ImageI");
    (void) fprintf(image->file," ]\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Font object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"<<\n");
    (void) fprintf(image->file,"/Type /Font\n");
    (void) fprintf(image->file,"/Subtype /Type1\n");
    (void) fprintf(image->file,"/Name /F%u\n",image->scene);
    (void) fprintf(image->file,"/BaseFont /Helvetica\n");
    (void) fprintf(image->file,"/Encoding /MacRomanEncoding\n");
    (void) fprintf(image->file,">>\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write XObject object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"<<\n");
    (void) fprintf(image->file,"/Type /XObject\n");
    (void) fprintf(image->file,"/Subtype /Image\n");
    (void) fprintf(image->file,"/Name /Im%u\n",image->scene);
    if (compression == NoCompression)
      (void) fprintf(image->file,"/Filter /ASCII85Decode\n");
    else
      if (!IsFaxImage(image))
        (void) fprintf(image->file,"/Filter [ /ASCII85Decode /%.1024s ]\n",
          compression == ZipCompression ? "FlateDecode" :
          compression == LZWCompression ? "LZWDecode" : "RunLengthDecode");
      else
        {
          (void) fprintf(image->file,
            "/Filter [ /ASCII85Decode /CCITTFaxDecode ]\n");
          (void) fprintf(image->file,
            "/DecodeParms [ << >> << /K %.1024s /Columns %d /Rows %d >> ]\n",
            CCITTParam,image->columns,image->rows);
        }
    (void) fprintf(image->file,"/Width %u\n",image->columns);
    (void) fprintf(image->file,"/Height %u\n",image->rows);
    (void) fprintf(image->file,"/ColorSpace %u 0 R\n",object+2);
    (void) fprintf(image->file,"/BitsPerComponent %d\n",
      IsFaxImage(image) ? 1 : 8);
    (void) fprintf(image->file,"/Length %u 0 R\n",object+1);
    (void) fprintf(image->file,">>\n");
    (void) fprintf(image->file,"stream\n");
    length=ftell(image->file);
    p=image->pixels;
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      switch (compression)
      {
        case RunlengthEncodedCompression:
        default:
        {
          /*
            Allocate pixel array.
          */
          number_packets=(image->colorspace == CMYKColorspace ? 4 : 3)*
            image->columns*image->rows;
          pixels=(unsigned char *)
            AllocateMemory(number_packets*sizeof(unsigned char));
          if (pixels == (unsigned char *) NULL)
            WriterExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          /*
            Dump runlength encoded pixels.
          */
          q=pixels;
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              if (image->matte && (p->index == Transparent))
                {
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                }
              else
                if (image->colorspace != CMYKColorspace)
                  {
                    *q++=DownScale(p->red);
                    *q++=DownScale(p->green);
                    *q++=DownScale(p->blue);
                  }
                else
                  {
                    *q++=DownScale(p->red);
                    *q++=DownScale(p->green);
                    *q++=DownScale(p->blue);
                    *q++=DownScale(p->index);
                  }
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
          FreeMemory((char *) pixels);
          if (!status)
            {
              CloseImage(image);
              return(False);
            }
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
              if (image->matte && (p->index == Transparent))
                {
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                }
              else
                if (image->colorspace != CMYKColorspace)
                  {
                    Ascii85Encode(DownScale(p->red),image->file);
                    Ascii85Encode(DownScale(p->green),image->file);
                    Ascii85Encode(DownScale(p->blue),image->file);
                  }
                else
                  {
                    Ascii85Encode(DownScale(p->red),image->file);
                    Ascii85Encode(DownScale(p->green),image->file);
                    Ascii85Encode(DownScale(p->blue),image->file);
                    Ascii85Encode(DownScale(p->index),image->file);
                  }
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
    else
      if (IsFaxImage(image))
        {
          register unsigned char
            bit,
            byte,
            polarity;

          polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
          if (image->colors == 2)
            polarity=
              Intensity(image->colormap[0]) < Intensity(image->colormap[1]);
          bit=0;
          byte=0;
          x=0;
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              if (Latin1Compare(CCITTParam,"0") == 0)
                (void) HuffmanEncodeImage((ImageInfo *) image_info,image);
              else
                (void) Huffman2DEncodeImage((ImageInfo *) image_info,image);
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  byte<<=1;
                  if (p->index == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      Ascii85Encode(byte,image->file);
                      bit=0;
                      byte=0;
                    }
                  x++;
                  if (x == (int) image->columns)
                    {
                      /*
                        Advance to the next scanline.
                      */
                      if (bit != 0)
                        Ascii85Encode(byte << (8-bit),image->file);
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick(y,image->rows))
                          ProgressMonitor(SaveImageText,y,image->rows);
                      bit=0;
                      byte=0;
                      x=0;
                      y++;
                   }
                }
                p++;
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Allocate pixel array.
              */
              number_packets=image->columns*image->rows;
              pixels=(unsigned char *)
                AllocateMemory(number_packets*sizeof(unsigned char));
              if (pixels == (unsigned char *) NULL)
                WriterExit(ResourceLimitWarning,"Memory allocation failed",
                  image);
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  *q++=(unsigned char) p->index;
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
              FreeMemory((char *) pixels);
              if (!status)
                {
                  CloseImage(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  Ascii85Encode((unsigned char) p->index,image->file);
                p++;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(i,image->packets))
                    ProgressMonitor(SaveImageText,i,image->packets);
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
    length=ftell(image->file)-length;
    (void) fprintf(image->file,"\nendstream\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"%lu\n",length);
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Colorspace object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    if (image->colorspace == CMYKColorspace)
      (void) fprintf(image->file,"/DeviceCMYK\n");
    else
      if (!IsPseudoClass(image) && !IsGrayImage(image))
        (void) fprintf(image->file,"/DeviceRGB\n");
      else
        if (IsFaxImage(image))
          (void) fprintf(image->file,"/DeviceGray\n");
        else
          (void) fprintf(image->file,"[ /Indexed /DeviceRGB %u %u 0 R ]\n",
            image->colors-1,object+3);
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Thumb object.
    */
    width=image->columns;
    height=image->rows;
    x=0;
    y=0;
    image->orphan=True;
    (void) ParseImageGeometry("106x106+0+0>",&x,&y,&width,&height);
    if (image->class == PseudoClass)
      tile_image=SampleImage(image,width,height);
    else
      tile_image=ZoomImage(image,width,height);
    image->orphan=False;
    if (tile_image == (Image *) NULL)
      WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"<<\n");
    if (compression == NoCompression)
      (void) fprintf(image->file,"/Filter /ASCII85Decode\n");
    else
      if (!IsFaxImage(image))
        (void) fprintf(image->file,"/Filter [ /ASCII85Decode /%.1024s ]\n",
          compression == ZipCompression ? "FlateDecode" :
          compression == LZWCompression ? "LZWDecode" : "RunLengthDecode");
      else
        {
          (void) fprintf(image->file,
            "/Filter [ /ASCII85Decode /CCITTFaxDecode ]\n");
          (void) fprintf(image->file,
            "/DecodeParms [ << >> << /Columns %d /Rows %d >> ]\n",
            tile_image->columns,tile_image->rows);
        }
    (void) fprintf(image->file,"/Width %u\n",tile_image->columns);
    (void) fprintf(image->file,"/Height %u\n",tile_image->rows);
    (void) fprintf(image->file,"/ColorSpace %u 0 R\n",object-1);
    (void) fprintf(image->file,"/BitsPerComponent %d\n",
      IsFaxImage(tile_image) ? 1 : 8);
    (void) fprintf(image->file,"/Length %u 0 R\n",object+1);
    (void) fprintf(image->file,">>\n");
    (void) fprintf(image->file,"stream\n");
    length=ftell(image->file);
    p=tile_image->pixels;
    if (!IsPseudoClass(tile_image) && !IsGrayImage(tile_image))
      switch (compression)
      {
        case RunlengthEncodedCompression:
        default:
        {
          /*
            Allocate pixel array.
          */
          number_packets=(image->colorspace == CMYKColorspace ? 4 : 3)*
            image->columns*image->rows;
          pixels=(unsigned char *)
            AllocateMemory(number_packets*sizeof(unsigned char));
          if (pixels == (unsigned char *) NULL)
            {
              DestroyImage(tile_image);
              WriterExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            }
          /*
            Dump runlength encoded pixels.
          */
          q=pixels;
          for (i=0; i < (int) tile_image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              if (tile_image->matte && (p->index == Transparent))
                {
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                }
              else
                {
                  *q++=DownScale(p->red);
                  *q++=DownScale(p->green);
                  *q++=DownScale(p->blue);
                  if (image->colorspace == CMYKColorspace)
                    *q++=DownScale(p->index);
                }
            }
            p++;
          }
          if (compression == ZipCompression)
            status=ZLIBEncodeImage(image->file,number_packets,
              image_info->quality,pixels);
          else
            if (compression == LZWCompression)
              status=LZWEncodeImage(image->file,number_packets,pixels);
            else
              status=PackbitsEncodeImage(image->file,number_packets,pixels);
          FreeMemory((char *) pixels);
          if (!status)
            {
              CloseImage(image);
              return(False);
            }
          break;
        }
        case NoCompression:
        {
          /*
            Dump uncompressed DirectColor packets.
          */
          Ascii85Initialize();
          for (i=0; i < (int) tile_image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              if (tile_image->matte && (p->index == Transparent))
                {
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                }
              else
                {
                  Ascii85Encode(DownScale(p->red),image->file);
                  Ascii85Encode(DownScale(p->green),image->file);
                  Ascii85Encode(DownScale(p->blue),image->file);
                  if (image->colorspace == CMYKColorspace)
                    Ascii85Encode(DownScale(p->index),image->file);
                }
            }
            p++;
          }
          Ascii85Flush(image->file);
          break;
        }
      }
    else
      if (IsFaxImage(tile_image))
        {
          register unsigned char
            bit,
            byte,
            polarity;

          polarity=Intensity(tile_image->colormap[0]) > (MaxRGB >> 1);
          if (image->colors == 2)
            polarity=Intensity(tile_image->colormap[0]) <
              Intensity(tile_image->colormap[1]);
          bit=0;
          byte=0;
          x=0;
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Allocate pixel array.
              */
              number_packets=((tile_image->columns+7) >> 3)*tile_image->rows;
              pixels=(unsigned char *)
                AllocateMemory(number_packets*sizeof(unsigned char));
              if (pixels == (unsigned char *) NULL)
                {
                  DestroyImage(tile_image);
                  WriterExit(ResourceLimitWarning,"Memory allocation failed",
                    image);
                }
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (i=0; i < (int) tile_image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  byte<<=1;
                  if (p->index == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      *q++=byte;
                      bit=0;
                      byte=0;
                    }
                  x++;
                  if (x == (int) tile_image->columns)
                    {
                      /*
                        Advance to the next scanline.
                      */
                      if (bit != 0)
                        *q++=byte << (8-bit);
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick(y,tile_image->rows))
                          ProgressMonitor(SaveImageText,y,tile_image->rows);
                      bit=0;
                      byte=0;
                      x=0;
                      y++;
                   }
                }
                p++;
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image->file,number_packets,
                  image_info->quality,pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image->file,number_packets,pixels);
                else
                  status=PackbitsEncodeImage(image->file,number_packets,pixels);
              FreeMemory((char *) pixels);
              if (!status)
                {
                  CloseImage(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) tile_image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  byte<<=1;
                  if (p->index == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      Ascii85Encode(byte,image->file);
                      bit=0;
                      byte=0;
                    }
                  x++;
                  if (x == (int) tile_image->columns)
                    {
                      /*
                        Advance to the next scanline.
                      */
                      if (bit != 0)
                        Ascii85Encode(byte << (8-bit),image->file);
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick(y,tile_image->rows))
                          ProgressMonitor(SaveImageText,y,tile_image->rows);
                      bit=0;
                      byte=0;
                      x=0;
                      y++;
                   }
                }
                p++;
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Allocate pixel array.
              */
              number_packets=tile_image->columns*tile_image->rows;
              pixels=(unsigned char *)
                AllocateMemory(number_packets*sizeof(unsigned char));
              if (pixels == (unsigned char *) NULL)
                {
                  DestroyImage(tile_image);
                  WriterExit(ResourceLimitWarning,
                    "Memory allocation failed",image);
                }
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (i=0; i < (int) tile_image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  *q++=(unsigned char) p->index;
                p++;
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image->file,number_packets,
                  image_info->quality,pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image->file,number_packets,pixels);
                else
                  status=PackbitsEncodeImage(image->file,number_packets,pixels);
              FreeMemory((char *) pixels);
              if (!status)
                {
                  CloseImage(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) tile_image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  Ascii85Encode((unsigned char) p->index,image->file);
                p++;
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
    DestroyImage(tile_image);
    length=ftell(image->file)-length;
    (void) fprintf(image->file,"\nendstream\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"%lu\n",length);
    (void) fprintf(image->file,"endobj\n");
    if ((image->class == DirectClass) || IsFaxImage(image))
      {
        xref[object++]=0;
        xref[object++]=0;
      }
    else
      {
        /*
          Write Colormap object.
        */
        xref[object++]=ftell(image->file);
        (void) fprintf(image->file,"%u 0 obj\n",object);
        (void) fprintf(image->file,"<<\n");
        (void) fprintf(image->file,"/Filter /ASCII85Decode \n");
        (void) fprintf(image->file,"/Length %u 0 R\n",object+1);
        (void) fprintf(image->file,">>\n");
        (void) fprintf(image->file,"stream\n");
        length=ftell(image->file);
        Ascii85Initialize();
        for (i=0; i < (int) image->colors; i++)
        {
          Ascii85Encode(DownScale(image->colormap[i].red),image->file);
          Ascii85Encode(DownScale(image->colormap[i].green),image->file);
          Ascii85Encode(DownScale(image->colormap[i].blue),image->file);
        }
        Ascii85Flush(image->file);
        length=ftell(image->file)-length;
        (void) fprintf(image->file,"\nendstream\n");
        (void) fprintf(image->file,"endobj\n");
        /*
          Write Length object.
        */
        xref[object++]=ftell(image->file);
        (void) fprintf(image->file,"%u 0 obj\n",object);
        (void) fprintf(image->file,"%lu\n",length);
        (void) fprintf(image->file,"endobj\n");
      }
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  /*
    Write Xref object.
  */
  length=ftell(image->file)-xref[0]+10;
  (void) fprintf(image->file,"xref\n");
  (void) fprintf(image->file,"0 %u\n",object+1);
  (void) fprintf(image->file,"0000000000 65535 f \n");
  for (i=0; i < (int) object; i++)
    (void) fprintf(image->file,"%010lu 00000 n \n",xref[i]);
  (void) fprintf(image->file,"trailer\n");
  (void) fprintf(image->file,"<<\n");
  (void) fprintf(image->file,"/Size %u\n",object+1);
  (void) fprintf(image->file,"/Info %u 0 R\n",info_id);
  (void) fprintf(image->file,"/Root %u 0 R\n",root_id);
  (void) fprintf(image->file,">>\n");
  (void) fprintf(image->file,"startxref\n");
  (void) fprintf(image->file,"%lu\n",length);
  (void) fprintf(image->file,"%%%%EOF\n");
  FreeMemory((char *) xref);
  CloseImage(image);
  if (image->temporary)
    {
      FILE
        *file;

      int
        c;

      /*
        Copy temporary file to standard output or pipe.
      */
      file=fopen(image->filename,ReadBinaryType);
      if (file == (FILE *) NULL)
        WriterExit(FileOpenWarning,"Unable to open file",image);
      for (c=fgetc(file); c != EOF; c=fgetc(file))
        (void) putc(c,encode_image.file);
      (void) fclose(file);
      (void) remove(image->filename);
      image->temporary=False;
      CloseImage(&encode_image);
    }
  return(True);
}
