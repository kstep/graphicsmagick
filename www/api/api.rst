.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=========================
GraphicsMagick Core C API
=========================

.. _Animate : animate.html
.. _Annotate : annotate.html
.. _Attribute : attribute.html
.. _Average : average.html
.. _`ASC CDL` : cdl.html
.. _Blob : blob.html
.. _Channel : channel.html
.. _Color : color.html
.. _Colormap : colormap.html
.. _Compare : compare.html
.. _Composite : composite.html
.. _Confirm Access : confirm_access.html
.. _Constitute : constitute.html
.. _Decorate : decorate.html
.. _Describe : describe.html
.. _Display : display.html
.. _Draw : draw.html
.. _Effect : effect.html
.. _Enhance : enhance.html
.. _Error : error.html
.. _Export : export.html
.. _FX : fx.html
.. _`Hald CLUT` : hclut.html
.. _Image : image.html
.. _Import : import.html
.. _List : list.html
.. _Magick : magick.html
.. _Memory : memory.html
.. _Monitor : monitor.html
.. _Montage : montage.html
.. _Operator : operator.html
.. _Paint : paint.html
.. _Pixel Cache : pixel_cache.html
.. _Pixel Iterator : pixel_iterator.html
.. _Plasma : plasma.html
.. _Profile : profile.html
.. _Quantize : quantize.html
.. _Registry : registry.html
.. _Resize : resize.html
.. _Resource : resource.html
.. _Segment : segment.html
.. _Shear : shear.html
.. _Signature : signature.html
.. _Statistics : statistics.html
.. _Texture : texture.html
.. _Transform : transform.html
.. _types: types.html

The GraphicsMagick core C library constitutes the implementation of
GraphicsMagick and provides the lowest-level C language programming
interface for GraphicsMagick.  The core C API provides many functions
to read, manipulate, write, or display an image.  To invoke the
functions, write your program in C (or C++) language while making
calls to the core library functions and link with libGraphicsMagick.a,
libGraphicsMagick.so, or GraphicsMagick.dll depending on your system.

The API is divided into a number of categories. While reading this
documentation, please reference the types_ documentation as required:

  * Animate_: Interactively animate an image sequence
  * Annotate_: Annotate an image with text
  * Attribute_: Access key, value image attributes
  * Average_: Average several images together
  * `ASC CDL`_ : Apply ASC CDL to image
  * Blob_: Read and write images to memory
  * Channel_: Import and export image channels as well as compute channel depth
  * Color_: Methods to deal with image colors
  * Colormap_: Methods to deal with image colormaps
  * Compare_: Compare images
  * Composite_: Composite images
  * `Confirm Access`_ : Confirm access to files and URLs.
  * Constitute_: Read, write, import, and export images
  * Decorate_: Add fancy borders to images
  * Describe_: Describe an image
  * Display_: Interactively display and edit an image
  * Draw_: Convenient methods to draw vectors and text
  * Effect_:Threshold (various), blur, despeckle, edge, emboss, enhance,
    gaussian blur ...
  * Enhance_: Contrast, equalize, gamma, level, level channel, modulate, negate,
    and normalize
  * Error_: Error reporting methods
  * Export_ : Export image pixels to common representations
  * FX_: Special effects methods
  * `Hald CLUT`_ : Apply Hald CLUT to image
  * Image_: Miscellaneous image methods
  * Import_ : Import image pixels from common representations
  * List_: Manage image lists
  * Magick_: Image format support interfaces
  * Memory_: Memory allocation methods
  * Monitor_: Progress monitor callbacks
  * Montage_: Create a montage of image thumbnails
  * Operator_: Methods to apply mathematic or boolean operators to pixels
  * Paint_: Fill pixel regions
  * `Pixel Cache`_: Low-level access to image pixels
  * `Pixel Iterator`_: Pixel iterator design pattern support functions
  * Plasma_: Plasma fractal image generator
  * Profile_: Attached profile access
  * Quantize_: Reduce image colors or assign image colors from colormap
  * Registry_: Store and retrieve images in memory by ID
  * Resize_: Resize an Image
  * Resource_: Set and get resource limits
  * Segment_: Coalese similar image colors
  * Shear_: Rotate image, shear image, or apply a 2D affine transformation
  * Signature_: Compute an image signature (checksum)
  * Statistics_: Compute image statistics
  * Texture_: Create a tiled texture image or tile an image with a texture.
  * Transform_: Chop, coalesce, deconstruct, flatten, flip, flop, mosiac, roll,
    or shave image

Here are a few sample programs to get you started.

This example program (convert.c) simply converts from one file name to
another (and will automatically change formats based on file
extension)::

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>
  #include <sys/types.h>
  #include <magick/api.h>

  int main ( int argc, char **argv )
  {
    Image
      *image = (Image *) NULL;

    char
      infile[MaxTextExtent],
      outfile[MaxTextExtent];

    int
      arg = 1,
      exit_status = 0;

    ImageInfo
      *imageInfo;

    ExceptionInfo
      exception;

    InitializeMagick(NULL);
    imageInfo=CloneImageInfo(0);
    GetExceptionInfo(&exception);

    if (argc != 3)
      {
        (void) fprintf ( stderr, "Usage: %s infile outfile\n", argv[0] );
        (void) fflush(stderr);
        exit_status = 1;
        goto program_exit;
      }

    (void) strncpy(infile, argv[arg], MaxTextExtent-1 );
    arg++;
    (void) strncpy(outfile, argv[arg], MaxTextExtent-1 );

    (void) strcpy(imageInfo->filename, infile);
    image = ReadImage(imageInfo, &exception);
    if (image == (Image *) NULL)
      {
        CatchException(&exception);
        exit_status = 1;
        goto program_exit;
      }

    (void) strcpy(image->filename, outfile);
    if (!WriteImage (imageInfo,image))
      {
        CatchException(&image->exception);
        exit_status = 1;
        goto program_exit;
      }

   program_exit:

    if (image != (Image *) NULL)
      DestroyImage(image);

    if (imageInfo != (ImageInfo *) NULL)
      DestroyImageInfo(imageInfo);
    DestroyMagick();

    return exit_status;
  }

This example program (demo.c) which reads multiple input files
(possibly animation files) specified on the command line, resizes the
image frames to 106x80, and writes the resulting animation to disk::

  #include <stdio.h>
  #include <string.h>
  #include <time.h>
  #include <sys/types.h>
  #include <magick/api.h>
  
  int main(int argc,char **argv)
  {
    ExceptionInfo
      exception;
  
    Image
      *image,
      *images,
      *resize_image,
      *thumbnails;
  
    ImageInfo
      *image_info;
  
    int
      i;
  
    /*
      Initialize the image info structure and read the list of files
      provided by the user as a image sequence
    */
    InitializeMagick(*argv);
    GetExceptionInfo(&exception);
    image_info=CloneImageInfo((ImageInfo *) NULL);
    images=NewImageList();
    for (i=1; i< argc-1; i++)
      {
        (void) strcpy(image_info->filename,argv[i]);
        printf("Reading %s ...", image_info->filename);
        image=ReadImage(image_info,&exception);
        printf(" %lu frames\n", GetImageListLength(image));
        if (exception.severity != UndefinedException)
          CatchException(&exception);
        if (image)
          (void) AppendImageToList(&images,image);
      }
  
    if (!images)
      {
        printf("Failed to read any images!\n");
        exit(1);
      }
    /*
      Create a thumbnail image sequence
    */
    thumbnails=NewImageList();
    while ((image=RemoveFirstImageFromList(&images)) != (Image *) NULL)
      {
        resize_image=ResizeImage(image,106,80,LanczosFilter,1.0,&exception);
        DestroyImage(image);
        if (resize_image == (Image *) NULL)
          {
            CatchException(&exception);
            continue;
          }
        (void) AppendImageToList(&thumbnails,resize_image);
      }
    /*
      Write the thumbnail image sequence to file
    */
    if (thumbnails)
      {
        (void) strcpy(thumbnails->filename,argv[argc-1]);
        image_info->adjoin=MagickTrue;
        printf("Writing %s ... %lu frames\n", thumbnails->filename,
               GetImageListLength(thumbnails));
        WriteImage(image_info,thumbnails);
      }
  
    /*
      Release resources
    */
    DestroyImageList(thumbnails);
    DestroyImageInfo(image_info);
    DestroyExceptionInfo(&exception);
    DestroyMagick();
    return(0);
  }

To compile on Unix, the command would look something like this::

  gcc -o demo demo.c -O `GraphicsMagick-config --cppflags --ldflags --libs`

As a usage example, with the input files in1.gif, in2.png, and in3.jpg, create
the animation file out.miff::

  demo in1.gif in2.png in3.jpg out.miff

The resulting animation may be played on an X11 display using 'gm animate
out.miff'.

The GraphicsMagick-config script reproduces the options which were used to
compile the GraphicsMagick utilities. Using compatible options ensures that
your program will compile and run.

-------------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015

