/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%             DDDD   IIIII  SSSSS  PPPP   L       AAA   Y   Y                 %
%             D   D    I    SS     P   P  L      A   A   Y Y                  %
%             D   D    I     SSS   PPPP   L      AAAAA    Y                   %
%             D   D    I       SS  P      L      A   A    Y                   %
%             DDDD   IIIII  SSSSS  P      LLLLL  A   A    Y                   %
%                                                                             %
%                                                                             %
%                     Interactively Display an Image.                         %
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
%  Display is a machine architecture independent image processing
%  and display program.  It can display any image in the MIFF format on
%  any workstation display running X.  Display first determines the
%  hardware capabilities of the workstation.  If the number of unique
%  colors in the image is less than or equal to the number the workstation
%  can support, the image is displayed in an X window.  Otherwise the
%  number of colors in the image is first reduced to match the color
%  resolution of the workstation before it is displayed.
%
%  This means that a continuous-tone 24 bits-per-pixel image can display on a
%  8 bit pseudo-color device or monochrome device.  In most instances the
%  reduced color image closely resembles the original.  Alternatively, a
%  monochrome or pseudo-color image can display on a continuous-tone 24
%  bits-per-pixel device.
%
%  The Display program command syntax is:
%
%  Usage: display [options ...] file [ [options ...] file ...]
%
%  Where options include:
%    -backdrop          display image centered on a backdrop
%    -border geometry   surround image with a border of color
%    -cache threshold   megabytes of memory available to the pixel cache
%    -colormap type     Shared or Private
%    -colors value      preferred number of colors in the image
%    -colorspace type   alternate image colorspace
%    -comment string    annotate image with comment",
%    -compress type     type of image compression
%    -contrast          enhance or reduce the image contrast
%    -crop geometry     preferred size and location of the cropped image
%    -delay value       display the next image after pausing
%    -density geometry  vertical and horizontal density of the image
%    -depth value       depth of the image
%    -despeckle         reduce the speckles within an image
%    -display server    display image to this X server
%    -dispose method    GIF disposal method
%    -dither            apply Floyd/Steinberg error diffusion to image
%    -edge factor       apply a filter to detect edges in the image
%    -enhance           apply a digital filter to enhance a noisy image
%    -filter type       use this filter when resizing an image
%    -flip              flip image in the vertical direction
%    -flop              flop image in the horizontal direction
%    -frame geometry    surround image with an ornamental border
%    -gamma value       level of gamma correction
%    -geometry geometry preferred size and location of the Image window
%    -immutable         displayed image cannot be modified
%    -interlace type    None, Line, Plane, or Partition
%    -label name        assign a label to an image
%    -map type          display image using this Standard Colormap
%    -matte             store matte channel if the image has one
%    -monochrome        transform image to black and white
%    -negate            replace every pixel with its complementary color
%    -page geometry     size and location of an image canvas
%    -quality value     JPEG/MIFF/PNG compression level
%    -raise value       lighten/darken image edges to create a 3-D effect
%    -remote command    execute a command in an remote display process
%    -roll geometry     roll an image vertically or horizontally
%    -rotate degrees    apply Paeth rotation to the image
%    -sample geometry   scale image with pixel sampling
%    -scene value       image scene number
%    -segment value     segment an image
%    -sharpen geometry  sharpen the image
%    -size geometry     width and height of image
%    -texture filename  name of texture to tile onto the image background
%    -treedepth value   depth of the color tree
%    -update seconds    detect when image file is modified and redisplay
%    -verbose           print detailed information about the image
%    -visual type       display image using this visual type
%    -window id         display image to background of this window
%    -window_group id   exit program when this window id is destroyed
%    -write filename    write image to a file
%
%  In addition to those listed above, you can specify these standard X
%  resources as command line options:  -background, -bordercolor,
%  -borderwidth, -font, -foreground, -iconGeometry, -iconic, -mattecolor,
%  -name, -shared_memory, -usePixmap, or -title.
%
%  By default, the image format of `file' is determined by its magic
%  number.  To specify a particular image format, precede the filename
%  with an image format name and a colon (i.e. ps:image) or specify the
%  image type as the filename suffix (i.e. image.ps).  Specify 'file' as
%  '-' for standard input or output.
%
%  Buttons:
%    1    press to map or unmap the Command widget
%    2    press and drag to magnify a region of an image
%    3    press to load an image from a visual image directory
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
%  Method Usage displays the program command syntax.
%
%  The format of the Usage method is:
%
%      void Usage(void)
%
%  A description of each parameter follows:
%
%
*/
static void Usage(void)
{
  const char
    **p;

  static const char
    *buttons[]=
    {
      "1    press to map or unmap the Command widget",
      "2    press and drag to magnify a region of an image",
      "3    press to load an image from a visual image directory",
      (char *) NULL
    },
    *options[]=
    {
      "-backdrop          display image centered on a backdrop",
      "-border geometry   surround image with a border of color",
      "-cache threshold   megabytes of memory available to the pixel cache",
      "-colormap type     Shared or Private",
      "-colors value      preferred number of colors in the image",
      "-colorspace type   alternate image colorspace",
      "-comment string    annotate image with comment",
      "-compress type     type of image compression",
      "-contrast          enhance or reduce the image contrast",
      "-crop geometry     preferred size and location of the cropped image",
      "-delay value       display the next image after pausing",
      "-density geometry  vertical and horizontal density of the image",
      "-depth value       depth of the image",
      "-despeckle         reduce the speckles within an image",
      "-display server    display image to this X server",
      "-dispose method    GIF disposal method",
      "-dither            apply Floyd/Steinberg error diffusion to image",
      "-edge factor       apply a filter to detect edges in the image",
      "-enhance           apply a digital filter to enhance a noisy image",
      "-filter type       use this filter when resizing an image",
      "-flip              flip image in the vertical direction",
      "-flop              flop image in the horizontal direction",
      "-frame geometry    surround image with an ornamental border",
      "-gamma value       level of gamma correction",
      "-geometry geometry preferred size and location of the Image window",
      "-immutable         displayed image cannot be modified",
      "-interlace type    None, Line, Plane, or Partition",
      "-label name        assign a label to an image",
      "-map type          display image using this Standard Colormap",
      "-matte             store matte channel if the image has one",
      "-monochrome        transform image to black and white",
      "-negate            replace every pixel with its complementary color ",
      "-page geometry     size and location of an image canvas",
      "-quality value     JPEG/MIFF/PNG compression level",
      "-raise value       lighten/darken image edges to create a 3-D effect",
      "-remote command    execute a command in an remote display process",
      "-roll geometry     roll an image vertically or horizontally",
      "-rotate degrees    apply Paeth rotation to the image",
      "-scene value       image scene number",
      "-segment value     segment an image",
      "-sample geometry   scale image with pixel sampling",
      "-sharpen geometry  sharpen the image",
      "-size geometry     width and height of image",
      "-texture filename  name of texture to tile onto the image background",
      "-treedepth value   depth of the color tree",
      "-update seconds    detect when image file is modified and redisplay",
      "-verbose           print detailed information about the image",
      "-visual type       display image using this visual type",
      "-window id         display image to background of this window",
      "-window_group id   exit program when this window id is destroyed",
      "-write filename    write image to a file",
      (char *) NULL
    };

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf(
    "Usage: %.1024s [-options ...] file [ [-options ...] file ...]\n",
    SetClientName((char *) NULL));
  (void) printf("\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
    "\nIn addition to those listed above, you can specify these standard X\n");
  (void) printf(
    "resources as command line options:  -background, -bordercolor,\n");
  (void) printf(
    "-borderwidth, -font, -foreground, -iconGeometry, -iconic, -mattecolor,\n");
  (void) printf("-name, -shared_memory, -usePixmap, or -title.\n");
  (void) printf(
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) printf(
    "number.  To specify a particular image format, precede the filename\n");
  (void) printf(
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) printf(
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
  (void) printf("'-' for standard input or output.\n");
  (void) printf("\nButtons: \n");
  for (p=buttons; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
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

#if defined(WIN32)
int WINAPI WinMain(HINSTANCE instance,HINSTANCE last,LPSTR command,int state)
{
  char
    **argv;

  int
    argc,
    main(int,char **);

  argv=StringToArgv(command,&argc);
  return(main(argc,argv));
}
#endif

int main(int argc,char **argv)
{
#if defined(HasX11)
  char
    *client_name,
    *option,
    *resource_value,
    *server_name;

  Display
    *display;

  double
    sans;

  ExceptionInfo
    exception;

  Image
    *image,
    *next;

  ImageInfo
    *image_info;

  int
    image_number,
    x;

  QuantizeInfo
    *quantize_info;

  register int
    i,
    j;

  unsigned int
    first_scene,
    *image_marker,
    last_image,
    last_scene,
    scene,
    status;

  unsigned long
    state;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  /*
    Initialize command line arguments.
  */
  ReadCommandlLine(argc,&argv);
  if (LocaleCompare("display",argv[0]) == 0)
    InitializeMagick(GetExecutionPath(argv[0]));
  else
    InitializeMagick(*argv);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickError(ResourceLimitError,"Memory allocation failed",(char *) NULL);
  /*
    Set defaults.
  */
  SetNotifyHandlers;
  display=(Display *) NULL;
  GetExceptionInfo(&exception);
  first_scene=0;
  image_number=0;
  last_image=0;
  last_scene=0;
  image_marker=(unsigned int *) AcquireMemory((argc+1)*sizeof(unsigned int));
  if (image_marker == (unsigned int *) NULL)
    MagickError(ResourceLimitError,"Unable to display image",
      "Memory allocation failed");
  for (i=0; i <= argc; i++)
    image_marker[i]=argc;
  resource_database=(XrmDatabase) NULL;
  state=0;
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
          MagickError(OptionError,"Missing server name",option);
        server_name=argv[i];
        break;
      }
    if (LocaleNCompare("help",option+1,2) == 0)
      Usage();
  }
  /*
    Get user defaults from X resource database.
  */
  display=XOpenDisplay(server_name);
  if (display == (Display *) NULL)
    MagickError(XServerError,"Unable to connect to X server",
      XDisplayName(server_name));
  (void) XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource_info);
  image_info=resource_info.image_info;
  quantize_info=resource_info.quantize_info;
  image_info->density=
    XGetResourceInstance(resource_database,client_name,"density",(char *) NULL);
  if (image_info->density == (char *) NULL)
    image_info->density=XGetScreenDensity(display);
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
    XGetResourceInstance(resource_database,client_name,"quality","75");
  image_info->quality=atoi(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"verbose","False");
  image_info->verbose=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"dither","True");
  quantize_info->dither=IsTrue(resource_value);
  /*
    Parse command line.
  */
  for (i=1; ((i <= argc) && !(state & ExitState)); i++)
  {
    if (i < argc)
      option=argv[i];
    else
      if (image_number != 0)
        break;
      else
        if (!isatty(STDIN_FILENO))
          option=(char *) "-";
        else
          option=(char *) "logo:Untitled";
    if ((strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      switch (*(option+1))
      {
        case 'b':
        {
          if (LocaleNCompare("backdrop",option+1,5) == 0)
            {
              resource_info.backdrop=(*option == '-');
              break;
            }
          if ((LocaleNCompare("background",option+1,5) == 0) ||
              (LocaleNCompare("bg",option+1,2) == 0))
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing color",option);
                  (void) CloneString(&resource_info.background_color,argv[i]);
                  (void) QueryColorDatabase(argv[i],
                    &image_info->background_color);
                }
              break;
            }
          if (LocaleCompare("border",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("bordercolor",option+1,7) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing color",option);
                  (void) CloneString(&resource_info.border_color,argv[i]);
                  (void) QueryColorDatabase(argv[i],&image_info->border_color);
                }
              break;
            }
          if (LocaleNCompare("borderwidth",option+1,7) == 0)
            {
              resource_info.border_width=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing width",option);
                  resource_info.border_width=atoi(argv[i]);
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
                  SetCacheThreshold(atoi(argv[i]));
                }
              break;
            }
          if (LocaleNCompare("colormap",option+1,6) == 0)
            {
              resource_info.colormap=PrivateColormap;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  resource_info.colormap=UndefinedColormap;
                  if (LocaleCompare("private",option) == 0)
                    resource_info.colormap=PrivateColormap;
                  if (LocaleCompare("shared",option) == 0)
                    resource_info.colormap=SharedColormap;
                  if (resource_info.colormap == UndefinedColormap)
                    MagickError(OptionError,"Invalid colormap type",option);
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
                  quantize_info->number_colors=atoi(argv[i]);
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
          if (LocaleNCompare("contrast",option+1,3) == 0)
            break;
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
          if (LocaleNCompare("debug",option+1,3) == 0)
            {
              resource_info.debug=(*option == '-');
              break;
            }
          if (LocaleNCompare("delay",option+1,3) == 0)
            {
              resource_info.delay=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing seconds",option);
                  resource_info.delay=atoi(argv[i]);
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
                  image_info->depth=atoi(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("despeckle",option+1,3) == 0)
            break;
          if (LocaleCompare("display",option+1) == 0)
            {
              (void) CloneString(&image_info->server_name,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing server name",option);
                  image_info->server_name=argv[i];
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
        case 'e':
        {
          if (LocaleNCompare("edge",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing factor",option);
                }
              break;
            }
          if (LocaleNCompare("enhance",option+1,2) == 0)
            break;
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'f':
        {
          if (LocaleNCompare("filter",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  FilterTypes
                    filter;

                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  filter=UndefinedFilter;
                  if (LocaleCompare("Point",option) == 0)
                    filter=PointFilter;
                  if (LocaleCompare("Box",option) == 0)
                    filter=BoxFilter;
                  if (LocaleCompare("Triangle",option) == 0)
                    filter=TriangleFilter;
                  if (LocaleCompare("Hermite",option) == 0)
                    filter=HermiteFilter;
                  if (LocaleCompare("Hanning",option) == 0)
                    filter=HanningFilter;
                  if (LocaleCompare("Hamming",option) == 0)
                    filter=HammingFilter;
                  if (LocaleCompare("Blackman",option) == 0)
                    filter=BlackmanFilter;
                  if (LocaleCompare("Gaussian",option) == 0)
                    filter=GaussianFilter;
                  if (LocaleCompare("Quadratic",option) == 0)
                    filter=QuadraticFilter;
                  if (LocaleCompare("Cubic",option) == 0)
                    filter=CubicFilter;
                  if (LocaleCompare("Catrom",option) == 0)
                    filter=CatromFilter;
                  if (LocaleCompare("Mitchell",option) == 0)
                    filter=MitchellFilter;
                  if (LocaleCompare("Lanczos",option) == 0)
                    filter=LanczosFilter;
                  if (LocaleCompare("Bessel",option) == 0)
                    filter=BesselFilter;
                  if (LocaleCompare("Sinc",option) == 0)
                    filter=SincFilter;
                  if (filter == UndefinedFilter)
                    MagickError(OptionError,"Invalid filter type",option);
                }
              break;
            }
          if (LocaleNCompare("flip",option+1,3) == 0)
            break;
          if (LocaleNCompare("flop",option+1,3) == 0)
            break;
          if (LocaleNCompare("font",option+1,3) == 0)
            {
              (void) CloneString(&image_info->font,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing font name",option);
                  image_info->font=argv[i];
                }
              if ((image_info->font == (char *) NULL) ||
                  (*image_info->font != '@'))
                (void) CloneString(&resource_info.font,image_info->font);
              break;
            }
          if ((LocaleNCompare("foreground",option+1,3) == 0) ||
              (LocaleNCompare("fg",option+1,2) == 0))
           {
             if (*option == '-')
               {
                 i++;
                 if (i == argc)
                   MagickError(OptionError,"Missing foreground",option);
                 (void) CloneString(&resource_info.foreground_color,argv[i]);
               }
              break;
           }
          if (LocaleNCompare("frame",option+1,2) == 0)
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
        case 'g':
        {
          if (LocaleNCompare("gamma",option+1,3) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                MagickError(OptionError,"Missing value",option);
              break;
            }
          if (LocaleNCompare("geometry",option+1,2) == 0)
            {
              (void) CloneString(&resource_info.image_geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&resource_info.image_geometry,argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'h':
        {
          if (LocaleNCompare("help",option+1,2) == 0)
            Usage();
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'i':
        {
          if (LocaleNCompare("iconGeometry",option+1,5) == 0)
            {
              (void) CloneString(&resource_info.icon_geometry,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&resource_info.icon_geometry,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("iconic",option+1,5) == 0)
            {
              resource_info.iconic=(*option == '-');
              break;
            }
          if (LocaleNCompare("immutable",option+1,5) == 0)
            {
              resource_info.immutable=(*option == '-');
              break;
            }
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
                  if (LocaleCompare("None",option) == 0)
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
          if (LocaleNCompare("magnify",option+1,3) == 0)
            {
              resource_info.magnify=2;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing level",option);
                  resource_info.magnify=atoi(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("map",option+1,3) == 0)
            {
              argv[i]=(char *) "+sans";
              resource_info.map_type=(char *) NULL;
              if (*option == '-')
                {
                  argv[i]=(char *) "-sans";
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing map type",option);
                  resource_info.map_type=argv[i];
                }
              break;
            }
          if (LocaleCompare("matte",option+1) == 0)
            break;
          if (LocaleNCompare("mattecolor",option+1,6) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing color",option);
                  (void) CloneString(&resource_info.matte_color,argv[i]);
                  (void) QueryColorDatabase(argv[i],&image_info->matte_color);
                }
              break;
            }
          if (LocaleNCompare("monochrome",option+1,3) == 0)
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
          if (LocaleNCompare("name",option+1,2) == 0)
            {
              (void) CloneString(&resource_info.name,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing name",option);
                  (void) CloneString(&resource_info.name,argv[i]);
                }
              break;
            }
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
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'q':
        {
          if (LocaleNCompare("quality",option+1,2) == 0)
            {
              image_info->quality=75;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing quality",option);
                  image_info->quality=atoi(argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'r':
        {
          if (LocaleNCompare("raise",option+1,2) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing bevel width",option);
              break;
            }
          if (LocaleNCompare("remote",option+1,3) == 0)
            {
              i++;
              if (i == argc)
                MagickError(OptionError,"Missing command",option);
              XRemoteCommand(display,resource_info.window_id,argv[i]);
              Exit(0);
            }
          if (LocaleNCompare("roll",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
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
          if (LocaleNCompare("sample",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("scene",option+1,3) == 0)
            {
              first_scene=0;
              last_scene=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing scene number",option);
                  first_scene=atoi(argv[i]);
                  last_scene=first_scene;
                  (void) sscanf(argv[i],"%u-%u",&first_scene,&last_scene);
                }
              break;
            }
          if (LocaleNCompare("segment",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing threshold",option);
                }
              break;
            }
          if (LocaleNCompare("sharpen",option+1,5) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleNCompare("shared_memory",option+1,5) == 0)
            {
              resource_info.use_shared_memory=(*option == '-');
              break;
            }
          if (LocaleNCompare("size",option+1,2) == 0)
            {
              (void) CloneString(&image_info->size,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&image_info->size,argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 't':
        {
          if (LocaleNCompare("text_font",option+1,5) == 0)
            {
              (void) CloneString(&resource_info.text_font,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing font name",option);
                  (void) CloneString(&resource_info.text_font,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("texture",option+1,5) == 0)
            {
              (void) CloneString(&image_info->texture,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing filename",option);
                  (void) CloneString(&image_info->texture,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("title",option+1,4) == 0)
            {
              (void) CloneString(&resource_info.title,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing title",option);
                  (void) CloneString(&resource_info.title,argv[i]);
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
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'u':
        {
          if (LocaleNCompare("update",option+1,2) == 0)
            {
              resource_info.update=(*option == '-');
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing seconds",option);
                  resource_info.update=atoi(argv[i]);
                }
              break;
            }
          if (LocaleNCompare("use_pixmap",option+1,2) == 0)
            {
              resource_info.use_pixmap=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'v':
        {
          if (LocaleNCompare("verbose",option+1,4) == 0)
            {
              image_info->verbose=(*option == '-');
              break;
            }
          if (LocaleNCompare("visual",option+1,3) == 0)
            {
              (void) CloneString(&resource_info.visual_type,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing visual class",option);
                  (void) CloneString(&resource_info.visual_type,argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'w':
        {
          if (LocaleCompare("window",option+1) == 0)
            {
              (void) CloneString(&resource_info.window_id,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing id, name, or 'root'",
                      option);
                  (void) CloneString(&resource_info.window_id,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("window_group",option+1,7) == 0)
            {
              (void) CloneString(&resource_info.window_group,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing id, name, or 'root'",
                      option);
                  (void) CloneString(&resource_info.window_group,argv[i]);
                }
              break;
            }
          if (LocaleNCompare("write",option+1,2) == 0)
            {
              (void) CloneString(&resource_info.write_filename,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing file name",option);
                  (void) CloneString(&resource_info.write_filename,argv[i]);
                  if (IsAccessible(resource_info.write_filename))
                    {
                      char
                        answer[2];

                      (void) fprintf(stderr,"Overwrite %.1024s? ",
                        resource_info.write_filename);
                      (void) fgets(answer,sizeof(answer),stdin);
                      if (!((*answer == 'y') || (*answer == 'Y')))
                        Exit(0);
                    }
                }
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
    else
      {
        /*
          Option is a file name.
        */
        for (scene=first_scene; scene <= last_scene ; scene++)
        {
          /*
            Read image.
          */
          (void) strncpy(image_info->filename,option,MaxTextExtent-1);
          if (first_scene != last_scene)
            {
              char
                filename[MaxTextExtent];

              /*
                Form filename for multi-part images.
              */
              FormatString(filename,image_info->filename,scene);
              if (LocaleCompare(filename,image_info->filename) == 0)
                FormatString(filename,"%.1024s.%u",image_info->filename,scene);
              (void) strncpy(image_info->filename,filename,MaxTextExtent-1);
            }
          (void) strcpy(image_info->magick,"MIFF");
          image_info->colorspace=quantize_info->colorspace;
          image_info->dither=quantize_info->dither;
          image=ReadImage(image_info,&exception);
          if (exception.severity != UndefinedException)
            MagickWarning(exception.severity,exception.reason,
              exception.description);
          if (image == (Image *) NULL)
            continue;
          do
          {
            /*
              Transmogrify image as defined by the image processing options.
            */
            resource_info.quantum=1;
            status=MogrifyImage(image_info,i,argv,&image);
            if (status == False)
              CatchImageException(image);
            if (first_scene != last_scene)
              image->scene=scene;
            /*
              Display image to X server.
            */
            if (resource_info.window_id != (char *) NULL)
              {
                /*
                  Display image to a specified X window.
                */
                status=XDisplayBackgroundImage(display,&resource_info,image);
                if (status)
                  state|=RetainColorsState;
              }
            else
              do
              {
                Image
                  *nexus;

                /*
                  Display image to X server.
                */
                nexus=
                  XDisplayImage(display,&resource_info,argv,argc,&image,&state);
                if (nexus == (Image *) NULL)
                  break;
                while ((nexus != (Image *) NULL) && (!(state & ExitState)))
                {
                  if (nexus->montage != (char *) NULL)
                    {
                      /*
                        User selected a visual directory image (montage).
                      */
                      DestroyImages(image);
                      image=nexus;
                      break;
                    }
                  status=MogrifyImage(image_info,i,argv,&nexus);
                  if (status == False)
                    CatchImageException(nexus);
                  if (first_scene != last_scene)
                    image->scene=scene;
                  next=XDisplayImage(display,&resource_info,argv,argc,
                    &nexus,&state);
                  if ((next == (Image *) NULL) &&
                      (nexus->next != (Image *) NULL))
                    {
                      DestroyImages(image);
                      image=nexus->next;
                      nexus=(Image *) NULL;
                    }
                  else
                    {
                      if (nexus != image)
                        DestroyImages(nexus);
                      nexus=next;
                    }
                }
              } while (!(state & ExitState));
            if (resource_info.write_filename != (char *) NULL)
              {
                /*
                  Write image.
                */
                (void) strncpy(image->filename,resource_info.write_filename,
                  MaxTextExtent-1);
                (void) SetImageInfo(image_info,True,&image->exception);
                status=WriteImage(image_info,image);
                if (status == False)
                  CatchImageException(image);
              }
            if (image_info->verbose)
              DescribeImage(image,stderr,False);
            /*
              Proceed to next/previous image.
            */
            next=image;
            if (state & FormerImageState)
              for (j=0; j < resource_info.quantum; j++)
              {
                next=next->previous;
                if (next == (Image *) NULL)
                  break;
              }
            else
              for (j=0; j < resource_info.quantum; j++)
              {
                next=next->next;
                if (next == (Image *) NULL)
                  break;
              }
            if (next != (Image *) NULL)
              image=next;
          } while ((next != (Image *) NULL) && !(state & ExitState));
          /*
            Free image resources.
          */
          DestroyImages(image);
          if (!(state & FormerImageState))
            {
              last_image=image_number;
              image_marker[i]=image_number++;
            }
          else
            {
              /*
                Proceed to previous image.
              */
              for (i--; i > 0; i--)
                if ((int) image_marker[i] == (image_number-2))
                  break;
              image_number--;
            }
          if (state & ExitState)
            break;
        }
      }
    /*
      Determine if we should proceed to the first image.
    */
    if (image_number < 0)
      {
        if (state & FormerImageState)
          {
            for (i=1; i < (argc-2); i++)
              if (image_marker[i] == last_image)
                break;
            image_number=image_marker[i]+1;
          }
        continue;
      }
    if (image_number > 0)
      if ((i == (argc-1)) || (argc == 1))
        if (!(state & ExitState))
          if (resource_info.window_id == (char *) NULL)
            {
              i=0;
              image_number=0;
            }
  }
  if (state & RetainColorsState)
    {
      XRetainWindowColors(display,XRootWindow(display,XDefaultScreen(display)));
      XSync(display,False);
    }
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(0);
#endif
  return(False);
}
