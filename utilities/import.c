/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                 IIIII  M   M  PPPP    OOO   RRRR   TTTTT                    %
%                   I    MM MM  P   P  O   O  R   R    T                      %
%                   I    M M M  PPPP   O   O  RRRR     T                      %
%                   I    M   M  P      O   O  R R      T                      %
%                 IIIII  M   M  P       OOO   R  R     T                      %
%                                                                             %
%                                                                             %
%               Import image to a machine independent format.                 %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1992                                      %
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
%  Import is an X Window System window dumping utility.  Import allows X
%  users to store window images in a specially formatted dump file.  This
%  file can then be read by the Display utility for redisplay, printing,
%  editing, formatting, archiving, image processing, etc.  The target
%  window can be specified by id or name or be selected by clicking the
%  mouse in the desired window.  The keyboard bell is rung once at the
%  beginning of the dump and twice when the dump is completed.
%
%  The import program command syntax is:
%
%  Usage: import [options ...] [file]
%
%  Where options include:
%    -adjoin             join images into a single multi-image file
%    -border             include image borders in the output image
%    -cache threshold    megabytes of memory available to the pixel cache
%    -colors value       preferred number of colors in the image
%    -colorspace type    alternate image colorspace
%    -comment string     annotate image with comment
%    -compress type      type of image compression
%    -crop geometry      preferred size and location of the cropped image
%    -delay value        pause before selecting target window
%    -density geometry   vertical and horizontal density of the image
%    -depth value        depth of the image
%    -descend            obtain image by descending window hierarchy
%    -display server     X server to contact
%    -dispose method     GIF disposal method
%    -dither             apply Floyd/Steinberg error diffusion to image
%    -frame              include window manager frame
%    -geometry geometry  perferred size or location of the image
%    -interlace type     None, Line, Plane, or Partition
%    -label name         assign a label to an image
%    -monochrome         transform image to black and white
%    -negate             replace every pixel with its complementary color 
%    -page geometry      size and location of an image canvas
%    -pointsize value    pointsize of Postscript font
%    -quality value      JPEG/MIFF/PNG compression level
%    -rotate degrees     apply Paeth rotation to the image
%    -scene value        number of screen snapshots
%    -screen             select image from root window
%    -silent             operate silently, i.e. don't ring any bells 
%    -transparent color  make this color transparent within the image
%    -treedepth value    depth of the color tree
%    -type type          image type
%    -verbose            print detailed information about the image
%    -window id          select window with this id or name
%
%  By default, 'file' is written in the Postscript image format.  To specify
%  a particular image format, precede the filename with an image format
%  name and a colon (i.e. ps:image) or specify the image type as the
%  filename suffix (i.e. image.ps).  Specify 'file' as '-' for standard
%  input or output.
%
%
*/

/*
  Include declarations.
*/
#include "magick/magick.h"
#include "magick/define.h"
#include "magick/xwindow.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U s a g e                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure Usage displays the program usage;
%
%  The format of the Usage method is:
%
%      void Usage()
%
%
*/
static void Usage(void)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-adjoin             join images into a single multi-image file",
      "-border             include image borders in the output image",
      "-cache threshold    megabytes of memory available to the pixel cache",
      "-colors value       preferred number of colors in the image",
      "-colorspace type    alternate image colorspace",
      "-comment string     annotate image with comment",
      "-compress type      type of image compression",
      "-crop geometry      preferred size and location of the cropped image",
      "-delay value        display the next image after pausing",
      "-density geometry   vertical and horizontal density of the image",
      "-depth value        depth of the image",
      "-descend            obtain image by descending window hierarchy",
      "-display server     X server to contact",
      "-dispose method     GIF disposal method",
      "-dither             apply Floyd/Steinberg error diffusion to image",
      "-frame              include window manager frame",
      "-geometry geometry  perferred size or location of the image",
      "-interlace type     None, Line, Plane, or Partition",
      "-label name         assign a label to an image",
      "-monochrome         transform image to black and white",
      "-negate             replace every pixel with its complementary color ",
      "-page geometry      size and location of an image canvas",
      "-pointsize value    pointsize of Postscript font",
      "-quality value      JPEG/MIFF/PNG compression level",
      "-rotate degrees     apply Paeth rotation to the image",
      "-scene value        number of screen snapshots",
      "-treedepth value    depth of the color tree",
      "-transparent color  make this color transparent within the image",
      "-silent             operate silently, i.e. don't ring any bells ",
      "-screen             select image from root window",
      "-transparent color  make this color transparent within the image",
      "-treedepth value    depth of the color tree",
      "-type type          image type",
      "-verbose            print detailed information about the image",
      "-window id          select window with this id or name",
      (char *) NULL
    };

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf("Usage: %.1024s [options ...] [file]\n",
    SetClientName((char *) NULL));
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
  "\nBy default, 'file' is written in the MIFF image format.  To\n");
  (void) printf(
    "specify a particular image format, precede the filename with an image\n");
  (void) printf(
    "format name and a colon (i.e. ps:image) or specify the image type as\n");
  (void) printf(
    "the filename suffix (i.e. image.ps).  Specify 'file' as '-' for\n");
  (void) printf("standard input or output.\n");
  Exit(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    M a i n                                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
int main(int argc,char **argv)
{
#if defined(HasX11)
  char
    *client_name,
    *filename,
    *option,
    *resource_value,
    *server_name,
    *target_window;

  Display
    *display;

  double
    sans;

  Image
    *image,
    *next_image;

  ImageInfo
    *image_info;

  int
    j,
    x;

  long
    number_scenes;

  QuantizeInfo
    *quantize_info;

  register Image
    *p;

  register int
    i;

  unsigned int
    status;

  XImportInfo
    ximage_info;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  /*
    Initialize command line arguments.
  */
  ReadCommandlLine(argc,&argv);
  if (LocaleCompare("import",argv[0]) == 0)
    InitializeMagick(GetExecutionPath(argv[0]));
  else
    InitializeMagick(*argv);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickError(ResourceLimitError,"Memory allocation failed",(char *) NULL);
  /*
    Check for server name specified on the command line.
  */
  server_name=(char *) NULL;
  for (i=1; i < argc; i++)
  {
    /*
      Check command line for server name.
    */
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      continue;
    if (LocaleCompare("display",option+1) == 0)
      {
        /*
          User specified server name.
        */
        i++;
        if (i == argc)
          MagickError(OptionError,"Missing server name on -display",option);
        server_name=argv[i];
        break;
      }
    if (LocaleNCompare("help",option+1,2) == 0)
      Usage();
  }
  /*
    Get user defaults from X resource database.
  */
  SetNotifyHandlers;
  display=XOpenDisplay(server_name);
  if (display == (Display *) NULL)
    MagickError(OptionError,"Unable to connect to X server",
      XDisplayName(server_name));
 (void) XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetImportInfo(&ximage_info);
  XGetResourceInfo(resource_database,client_name,&resource_info);
  image_info=resource_info.image_info;
  quantize_info=resource_info.quantize_info;
  resource_value=
    XGetResourceInstance(resource_database,client_name,"border","False");
  ximage_info.borders=IsTrue(resource_value);
  resource_info.delay=0;
  resource_info.pause=0;
  resource_value=
    XGetResourceInstance(resource_database,client_name,"delay","6");
  (void) XParseGeometry(resource_value,&x,&x,&resource_info.delay,
    &resource_info.pause);
  image_info->density=XGetResourceInstance(resource_database,client_name,
    "density",(char *) NULL);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"descend","True");
  ximage_info.descend=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"frame","False");
  ximage_info.frame=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"interlace","none");
  image_info->interlace=UndefinedInterlace;
  if (LocaleCompare("None",resource_value) == 0)
    image_info->interlace=NoInterlace;
  if (LocaleCompare("Line",resource_value) == 0)
    image_info->interlace=LineInterlace;
  if (LocaleCompare("Plane",resource_value) == 0)
    image_info->interlace=PlaneInterlace;
  if (LocaleCompare("Partition",resource_value) == 0)
    image_info->interlace=PartitionInterlace;
  if (image_info->interlace == UndefinedInterlace)
    MagickWarning(OptionWarning,"Unrecognized interlace type",resource_value);
  image_info->page=XGetResourceInstance(resource_database,client_name,
    "pageGeometry",(char *) NULL);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"quality","85");
  image_info->quality=atol(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"screen","False");
  ximage_info.screen=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"silent","False");
  ximage_info.silent=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"verbose","False");
  image_info->verbose=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"dither","True");
  quantize_info->dither=IsTrue(resource_value);
  number_scenes=1;
  /*
    Check command syntax.
  */
  filename=(char *) NULL;
  target_window=(char *) NULL;
  j=0;
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) < 2) || ((*option != '-') && (*option != '+')))
      {
        filename=argv[i];
        j=i;
      }
    else
      switch(*(option+1))
      {
        case 'a':
        {
          if (LocaleNCompare("adjoin",option+1,2) == 0)
            {
              image_info->adjoin=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'b':
        {
          if (LocaleCompare("border",option+1) == 0)
            {
              ximage_info.borders=(*option == '-');
              break;
            }
          if (LocaleNCompare("bordercolor",option+1,7) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing border color",option);
                  (void) QueryColorDatabase(argv[i],&image_info->border_color);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'c':
        {
          if (LocaleNCompare("cache",option+1,3) == 0)
            {
              SetCacheThreshold(0);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing threshold",option);
                  SetCacheThreshold(atol(argv[i]));
                }
              break;
            }
          if (LocaleNCompare("colors",option+1,7) == 0)
            {
              quantize_info->number_colors=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing colors",option);
                  quantize_info->number_colors=atol(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("colorspace",option+1,7) == 0)
            {
              quantize_info->colorspace=RGBColorspace;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  quantize_info->colorspace=UndefinedColorspace;
                  if (LocaleCompare("cmyk",option) == 0)
                    quantize_info->colorspace=CMYKColorspace;
                  if (LocaleCompare("gray",option) == 0)
                    {
                      quantize_info->colorspace=GRAYColorspace;
                      quantize_info->number_colors=256;
                      quantize_info->tree_depth=8;
                    }
                  if (LocaleCompare("ohta",option) == 0)
                    quantize_info->colorspace=OHTAColorspace;
                  if (LocaleCompare("rgb",option) == 0)
                    quantize_info->colorspace=RGBColorspace;
                  if (LocaleCompare("srgb",option) == 0)
                    quantize_info->colorspace=sRGBColorspace;
                  if (LocaleCompare("transparent",option) == 0)
                    quantize_info->colorspace=TransparentColorspace;
                  if (LocaleCompare("xyz",option) == 0)
                    quantize_info->colorspace=XYZColorspace;
                  if (LocaleCompare("ycbcr",option) == 0)
                    quantize_info->colorspace=YCbCrColorspace;
                  if (LocaleCompare("ycc",option) == 0)
                    quantize_info->colorspace=YCCColorspace;
                  if (LocaleCompare("yiq",option) == 0)
                    quantize_info->colorspace=YIQColorspace;
                  if (LocaleCompare("ypbpr",option) == 0)
                    quantize_info->colorspace=YPbPrColorspace;
                  if (LocaleCompare("yuv",option) == 0)
                    quantize_info->colorspace=YUVColorspace;
                  if (quantize_info->colorspace == UndefinedColorspace)
                    MagickError(OptionError,"Invalid colorspace type",option);
                }
              break;
            }
          if (LocaleNCompare("comment",option+1,4) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing comment",option);
                }
              break;
            }
          if (LocaleNCompare("compress",option+1,5) == 0)
            {
              image_info->compression=NoCompression;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  image_info->compression=UndefinedCompression;
                  if (LocaleCompare("None",option) == 0)
                    image_info->compression=NoCompression;
                  if (LocaleCompare("BZip",option) == 0)
                    image_info->compression=BZipCompression;
                  if (LocaleCompare("Fax",option) == 0)
                    image_info->compression=FaxCompression;
                  if (LocaleCompare("Group4",option) == 0)
                    image_info->compression=Group4Compression;
                  if (LocaleCompare("JPEG",option) == 0)
                    image_info->compression=JPEGCompression;
                  if (LocaleCompare("Lossless",option) == 0)
                    image_info->compression=LosslessJPEGCompression;
                  if (LocaleCompare("LZW",option) == 0)
                    image_info->compression=LZWCompression;
                  if (LocaleCompare("RLE",option) == 0)
                    image_info->compression=RunlengthEncodedCompression;
                  if (LocaleCompare("Zip",option) == 0)
                    image_info->compression=ZipCompression;
                  if (image_info->compression == UndefinedCompression)
                    MagickError(OptionError,"Invalid compression type",option);
                }
              break;
            }
          if (LocaleNCompare("crop",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'd':
        {
          if (LocaleNCompare("delay",option+1,3) == 0)
            {
              resource_info.delay=0;
              resource_info.pause=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing seconds",option);
                  (void) XParseGeometry(argv[i],&x,&x,&resource_info.delay,
                    &resource_info.pause);
                }
              break;
            }
          if (LocaleNCompare("density",option+1,3) == 0)
            {
              (void) CloneString(&image_info->density,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&image_info->density,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("depth",option+1,3) == 0)
            {
              image_info->depth=QuantumDepth;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing image depth",option);
                  image_info->depth=atol(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("descend",option+1,3) == 0)
            {
              ximage_info.descend=(*option == '-');
              break;
            }
          if (LocaleCompare("display",option+1) == 0)
            {
              (void) CloneString(&image_info->server_name,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing server name",option);
                  (void) CloneString(&image_info->server_name,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("dispose",option+1,5) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing method",option);
                }
              break;
            }
          if (LocaleNCompare("dither",option+1,3) == 0)
            {
              quantize_info->dither=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'f':
        {
          if (LocaleNCompare("frame",option+1,2) == 0)
            {
              argv[i]=(char *) "-ignore";  /* resolve option confict */
              ximage_info.frame=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'g':
        {
          if (LocaleNCompare("geometry",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'h':
        {
          if (LocaleNCompare("help",option+1,2) == 0)
            {
              Usage();
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'i':
        {
          if (LocaleNCompare("interlace",option+1,5) == 0)
            {
              image_info->interlace=NoInterlace;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  image_info->interlace=UndefinedInterlace;
                  if (LocaleCompare("No",option) == 0)
                    image_info->interlace=NoInterlace;
                  if (LocaleCompare("Line",option) == 0)
                    image_info->interlace=LineInterlace;
                  if (LocaleCompare("Plane",option) == 0)
                    image_info->interlace=PlaneInterlace;
                  if (LocaleCompare("Partition",option) == 0)
                    image_info->interlace=PartitionInterlace;
                  if (image_info->interlace == UndefinedInterlace)
                    MagickError(OptionError,"Invalid interlace type",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'l':
        {
          if (LocaleNCompare("label",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing label name",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'm':
        {
          if (LocaleNCompare("monochrome",option+1,2) == 0)
            {
              image_info->monochrome=(*option == '-');
              if (image_info->monochrome)
                {
                  quantize_info->number_colors=2;
                  quantize_info->tree_depth=8;
                  quantize_info->colorspace=GRAYColorspace;
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'n':
        {
          if (LocaleNCompare("negate",option+1,2) == 0)
            break;
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'p':
        {
          if (LocaleNCompare("page",option+1,3) == 0)
            {
              (void) CloneString(&image_info->page,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing page geometry",option);
                  image_info->page=PostscriptGeometry(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("ping",option+1,2) == 0)
            {
              MagickWarning(OptionError,"Deprecated option",option);
              break;
            }
          if (LocaleNCompare("pointsize",option+1,2) == 0)
            {
              image_info->pointsize=12;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing size",option);
                  image_info->pointsize=atof(argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'q':
        {
          if (LocaleNCompare("quality",option+1,2) == 0)
            {
              image_info->quality=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing quality",option);
                  image_info->quality=atol(argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'r':
        {
          if (LocaleNCompare("rotate",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                     MagickError(OptionError,"Missing degrees",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 's':
        {
          if (LocaleNCompare("scene",option+1,3) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing scene",option);
              number_scenes=atol(argv[i]);
              break;
            }
          if (LocaleNCompare("screen",option+1,3) == 0)
            {
              ximage_info.screen=(*option == '-');
              break;
            }
          if (LocaleNCompare("silent",option+1,3) == 0)
            {
              ximage_info.silent=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 't':
        {
          if (LocaleNCompare("transparent",option+1,7) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing color",option);
                }
              break;
            }
          if (LocaleNCompare("treedepth",option+1,3) == 0)
            {
              quantize_info->tree_depth=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing depth",option);
                  quantize_info->tree_depth=atoi(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("type",option+1,4) == 0)
            {
              image_info->type=UndefinedType;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  if (LocaleCompare("Bilevel",option) == 0)
                    image_info->type=BilevelType;
                  if (LocaleCompare("Grayscale",option) == 0)
                    image_info->type=GrayscaleType;
                  if (LocaleCompare("GrayscaleMatte",option) == 0)
                    image_info->type=GrayscaleMatteType;
                  if (LocaleCompare("Palette",option) == 0)
                    image_info->type=PaletteType;
                  if (LocaleCompare("PaletteMatte",option) == 0)
                    image_info->type=PaletteMatteType;
                  if (LocaleCompare("TrueColor",option) == 0)
                    image_info->type=TrueColorType;
                  if (LocaleCompare("TrueColorMatte",option) == 0)
                    image_info->type=TrueColorMatteType;
                  if (LocaleCompare("ColorSeparation",option) == 0)
                    image_info->type=ColorSeparationType;
                  if (LocaleCompare("ColorSeparationMatte",option) == 0)
                    image_info->type=ColorSeparationMatteType;
                  if (LocaleCompare("Optimize",option) == 0)
                    image_info->type=OptimizeType;
                  if (image_info->type == UndefinedType)
                    MagickError(OptionError,"Invalid image type",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'w':
        {
          i++;
          if (i == argc)
            MagickError(OptionError,"Missing id, name, or 'root'",option);
          (void) CloneString(&target_window,argv[i]);
          break;
        }
        case 'v':
        {
          if (LocaleNCompare("verbose",option+1,4) == 0)
            {
              image_info->verbose=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case '?':
        {
          Usage();
          break;
        }
        default:
        {
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
      }
  }
  if (filename == (char *) NULL)
    filename=(char *) "magick.miff";
  /*
    Read image from X server.
  */
  if (target_window != (char *) NULL)
    (void) strncpy(image_info->filename,target_window,MaxTextExtent-1);
  (void) sleep(resource_info.pause);
  image_info->colorspace=quantize_info->colorspace;
  image_info->dither=quantize_info->dither;
  image=(Image *) NULL;
  for (i=0; i < (long) Max(number_scenes,1); i++)
  {
    next_image=XImportImage(image_info,&ximage_info);
    if (next_image == (Image *) NULL)
      continue;
    (void) strncpy(next_image->filename,filename,MaxTextExtent-1);
    (void) strcpy(next_image->magick,"PS");
    next_image->scene=i;
    next_image->previous=image;
    if (image != (Image *) NULL)
      image->next=next_image;
    image=next_image;
    XDelay(display,(unsigned long) resource_info.delay*10);
  }
  if (image == (Image *) NULL)
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  (void) SetImageInfo(image_info,True,&image->exception);
  status=0;
  for (p=image; p != (Image *) NULL; p=p->next)
  {
    status=WriteImage(image_info,p);
    CatchImageException(p);
    if (image_info->adjoin)
      break;
  }
  if (image_info->verbose)
    DescribeImage(image,stderr,False);
  DestroyImages(image);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(0);
#endif
  return(False);
}
