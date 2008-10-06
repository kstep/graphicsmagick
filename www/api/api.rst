====================
GraphicsMagick C API
====================

.. _Animate : animate.html
.. _Annotate : annotate.html
.. _Attribute : attribute.html
.. _Blob : blob.html
.. _Channel : channel.html
.. _Color : color.html
.. _Compare : compare.html
.. _Composite : composite.html
.. _Constitute : constitute.html
.. _Decorate : decorate.html
.. _Display : display.html
.. _Draw : draw.html
.. _Effect : effect.html
.. _Enhance : enhance.html
.. _Error : error.html
.. _FX : fx.html
.. _Image : image.html
.. _List : list.html
.. _Magick : magick.html
.. _Memory : memory.html
.. _Monitor : monitor.html
.. _Montage : montage.html
.. _Operator : operator.html
.. _Paint : paint.html
.. _Pixel Cache : pixel_cache.html
.. _Pixel Iterator : pixel_iterator.html
.. _Profile : profile.html
.. _Quantize : quantize.html
.. _Registry : registry.html
.. _Resize : resize.html
.. _Resource : resource.html
.. _Segment : segment.html
.. _Shear : shear.html
.. _Signature : signature.html
.. _Transform : transform.html

GraphicsMagick has a number of functions that allow you to read, manipulate,
write, or display an image. These functions are accessible through the various
tools or the object-oriented Perl interface, PerlMagick. However, you can also
access the functions directly from your program through the Magick Application
Programmer Interface. To invoke the functions, write your program in your
favorite language while making calls to the Magick image functions and link
with libGraphicsMagick.a, libGraphicsMagick.so, or GraphicsMagick.dll depending
on your system.

The API is divided into a number of categories:

  * Animate_: Interactively animate an image sequence
  * Annotate_: Annotate an image with text
  * Attribute_: Access key, value image attributes
  * Blob_: Read and write images to memory
  * Channel_: Import and export image channels as well as compute channel depth
  * Color_: Methods to deal with image colors
  * Compare_: Compare images
  * Composite_: Composite images
  * Constitute_: Read, write, import, and export images
  * Decorate_: Add fancy borders to images
  * Display_: Interactively display and edit an image
  * Draw_: Convenient methods to draw vectors and text
  * Effect_:Threshold (various), blur, despeckle, edge, emboss, enhance,
    gaussian blur ...
  * Enhance_: Contrast, equalize, gamma, level, level channel, modulate, negate,
    and normalize
  * Error_: Error reporting methods
  * FX_: Special effects methods
  * Image_: Miscellaneous image methods
  * List_: Manage image lists
  * Magick_: Image format support interfaces
  * Memory_: Memory allocation methods
  * Monitor_: Progress monitor callbacks
  * Montage_: Create a montage of image thumbnails
  * Operator_: Methods to apply mathematic or boolean operators to pixels
  * Paint_: Fill pixel regions
  * `Pixel Cache`_: Low-level access to image pixels
  * `Pixel Iterator`_: Pixel iterator design pattern support functions
  * Profile_: Attached profile access
  * Quantize_: Reduce image colors or assign image colors from colormap
  * Registry_: Store and retrieve images in memory by ID
  * Resize_: Resize an Image
  * Resource_: Set and get resource limits
  * Segment_: Coalese similar image colors
  * Shear_: Rotate image, shear image, or apply a 2D affine transformation
  * Signature_: Compute an image signature (checksum)
  * Transform_: Chop, coalesce, deconstruct, flatten, flip, flop, mosiac, roll,
    or shave image

Here is a sample program to get you started. To find out about all the
functions that are available, read the source code. Each function is delineated
with a full rows of percent signs with comments describing the parameters
required for the function and what it does. For ease in finding a function,
they are sorted in alphabetical order.

Here is a full example of a program, demo.c, which reads multiple input files
(possibly animation files) specified on the command line, resizes the image
frames to 106x80, and writes the resulting animation to disk::

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

Now we need to compile. On Unix, the command would look something like this::

  gcc -o demo demo.c -O `GraphicsMagick-config --cppflags --ldflags --libs`

As a usage example, with the input files in1.gif, in2.png, and in3.jpg, create
the animation file out.miff::

  demo in1.gif in2.png in3.jpg out.miff

The resulting animation may be played on an X11 display using 'gm animate
out.miff'.

The GraphicsMagick-config script reproduces the options which were used to
compile the GraphicsMagick utilities. Using compatible options ensures that
your program will compile and run.

Another example is smile.c. Compile and execute it to display a smiley face on
your X server.

-------------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2008

