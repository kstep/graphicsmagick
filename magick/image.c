/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                       IIIII  M   M   AAA   GGGG  EEEEE                      %
%                         I    MM MM  A   A G      E                          %
%                         I    M M M  AAAAA G  GG  EEE                        %
%                         I    M   M  A   A G   G  E                          %
%                       IIIII  M   M  A   A  GGGG  EEEEE                      %
%                                                                             %
%                                                                             %
%                           ImageMagick Image Methods                         %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                  July 1992                                  %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

/*
  Image defines.
*/
#define MatteMatch(color,target,delta) \
  (ColorMatch(color,target,delta) && ((color).index == (target).index))
#define MaxStacksize  (1 << 15)
#define Push(up,left,right,delta) \
  if ((p < (segment_stack+MaxStacksize)) && (((up)+(delta)) >= 0) && \
      (((up)+(delta)) < (int) image->rows)) \
    { \
      p->y1=(up); \
      p->x1=(left); \
      p->x2=(right); \
      p->y2=(delta); \
      p++; \
    }

/*
  Constant declaration.
*/
const char
  *Alphabet = "`-=[]\\;',./~!@#$%^&*()_+{}|:\"<>?" \
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
  *DefaultImageQuality = "75",
  *DefaultPointSize = "12";

const InterlaceType
  DefaultInterlace = NoInterlace;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AllocateImage allocates an Image structure and initializes each
%  field to a default value.
%
%  The format of the AllocateImage routine is:
%
%      allocated_image=AllocateImage(image_info)
%
%  A description of each parameter follows:
%
%    o allocated_image: Method AllocateImage returns a pointer to an image
%      structure initialized to default values.  A null image is returned if
%      there is a memory shortage.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Export Image *AllocateImage(const ImageInfo *image_info)
{
  Image
    *allocated_image;

  int
    flags;

  XColor
    color;

  /*
    Allocate image structure.
  */
  allocated_image=(Image *) AllocateMemory(sizeof(Image));
  if (allocated_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to allocate image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Initialize Image structure.
  */
  allocated_image->file=(FILE *) NULL;
  allocated_image->exempt=False;
  allocated_image->status=False;
  allocated_image->temporary=False;
  *allocated_image->filename='\0';
  allocated_image->filesize=0;
  allocated_image->pipe=False;
  (void) strcpy(allocated_image->magick,"MIFF");
  allocated_image->comments=(char *) NULL;
  allocated_image->label=(char *) NULL;
  allocated_image->text=(char *) NULL;
  allocated_image->id=UndefinedId;
  allocated_image->class=DirectClass;
  allocated_image->matte=False;
  allocated_image->compression=RunlengthEncodedCompression;
  allocated_image->columns=0;
  allocated_image->rows=0;
  allocated_image->depth=QuantumDepth;
  allocated_image->tile_info.width=0;
  allocated_image->tile_info.height=0;
  allocated_image->tile_info.x=0;
  allocated_image->tile_info.y=0;
  allocated_image->offset=0;
  allocated_image->interlace=DefaultInterlace;
  allocated_image->scene=0;
  allocated_image->units=UndefinedResolution;
  allocated_image->x_resolution=0.0;
  allocated_image->y_resolution=0.0;
  allocated_image->montage=(char *) NULL;
  allocated_image->directory=(char *) NULL;
  allocated_image->colormap=(ColorPacket *) NULL;
  allocated_image->colors=0;
  allocated_image->colorspace=RGBColorspace;
  allocated_image->rendering_intent=UndefinedIntent;
  allocated_image->gamma=0.0;
  allocated_image->chromaticity.red_primary.x=0.0;
  allocated_image->chromaticity.red_primary.y=0.0;
  allocated_image->chromaticity.green_primary.x=0.0;
  allocated_image->chromaticity.green_primary.y=0.0;
  allocated_image->chromaticity.blue_primary.x=0.0;
  allocated_image->chromaticity.blue_primary.y=0.0;
  allocated_image->chromaticity.white_point.x=0.0;
  allocated_image->chromaticity.white_point.y=0.0;
  allocated_image->color_profile.length=0;
  allocated_image->color_profile.info=(unsigned char *) NULL;
  allocated_image->iptc_profile.length=0;
  allocated_image->iptc_profile.info=(unsigned char *) NULL;
  allocated_image->pixels=(RunlengthPacket *) NULL;
  allocated_image->packet=(RunlengthPacket *) NULL;
  allocated_image->packets=0;
  allocated_image->packet_size=0;
  allocated_image->packed_pixels=(unsigned char *) NULL;
  allocated_image->geometry=(char *) NULL;
  allocated_image->page=(char *) NULL;
  allocated_image->dispose=0;
  allocated_image->delay=0;
  allocated_image->iterations=1;
  allocated_image->fuzz=0;
  allocated_image->filter=MitchellFilter;
  allocated_image->blur=1.0;
  (void) XQueryColorDatabase(BackgroundColor,&color);
  allocated_image->background_color.red=XDownScale(color.red);
  allocated_image->background_color.green=XDownScale(color.green);
  allocated_image->background_color.blue=XDownScale(color.blue);
  allocated_image->background_color.index=Transparent;
  (void) XQueryColorDatabase(BorderColor,&color);
  allocated_image->border_color.red=XDownScale(color.red);
  allocated_image->border_color.green=XDownScale(color.green);
  allocated_image->border_color.blue=XDownScale(color.blue);
  allocated_image->border_color.index=Transparent;
  (void) XQueryColorDatabase(MatteColor,&color);
  allocated_image->matte_color.red=XDownScale(color.red);
  allocated_image->matte_color.green=XDownScale(color.green);
  allocated_image->matte_color.blue=XDownScale(color.blue);
  allocated_image->matte_color.index=Transparent;
  allocated_image->total_colors=0;
  allocated_image->normalized_mean_error=0.0;
  allocated_image->normalized_maximum_error=0.0;
  allocated_image->mean_error_per_pixel=0;
  allocated_image->signature=(char *) NULL;
  *allocated_image->magick_filename='\0';
  allocated_image->magick_columns=0;
  allocated_image->magick_rows=0;
  allocated_image->magick_time=time((time_t *) NULL);
  allocated_image->tainted=False;
  allocated_image->orphan=False;
  allocated_image->previous=(Image *) NULL;
  allocated_image->list=(Image *) NULL;
  allocated_image->next=(Image *) NULL;
  if (image_info == (ImageInfo *) NULL)
    return(allocated_image);
  /*
    Transfer image info.
  */
  (void) strcpy(allocated_image->filename,image_info->filename);
  (void) strcpy(allocated_image->magick_filename,image_info->filename);
  (void) strcpy(allocated_image->magick,image_info->magick);
  if (image_info->size != (char *) NULL)
    {
      int
        y;

      (void) sscanf(image_info->size,"%ux%u",
        &allocated_image->columns,&allocated_image->rows);
      flags=XParseGeometry(image_info->size,&allocated_image->offset,&y,
        &allocated_image->columns,&allocated_image->rows);
      if ((flags & HeightValue) == 0)
        allocated_image->rows=allocated_image->columns;
      allocated_image->tile_info.width=allocated_image->columns;
      allocated_image->tile_info.height=allocated_image->rows;
    }
  if (image_info->tile != (char *) NULL)
    if (!IsSubimage(image_info->tile,False))
      {
        (void) sscanf(image_info->tile,"%ux%u",
          &allocated_image->columns,&allocated_image->rows);
        flags=XParseGeometry(image_info->tile,&allocated_image->tile_info.x,
          &allocated_image->tile_info.y,&allocated_image->columns,
          &allocated_image->rows);
        if ((flags & HeightValue) == 0)
          allocated_image->rows=allocated_image->columns;
      }
  allocated_image->compression=image_info->compression;
  allocated_image->interlace=image_info->interlace;
  allocated_image->units=image_info->units;
  if (image_info->density != (char *) NULL)
    {
      int
        count;

      count=sscanf(image_info->density,"%fx%f",&allocated_image->x_resolution,
        &allocated_image->y_resolution);
      if (count != 2)
        allocated_image->y_resolution=allocated_image->x_resolution;
    }
  if (image_info->page != (char *) NULL)
    allocated_image->page=PostscriptGeometry(image_info->page);
  if (image_info->dispose != (char *) NULL)
    allocated_image->dispose=atoi(image_info->dispose);
  if (image_info->delay != (char *) NULL)
    allocated_image->delay=atoi(image_info->delay);
  if (image_info->iterations != (char *) NULL)
    allocated_image->iterations=atoi(image_info->iterations);
  allocated_image->depth=image_info->depth;
  if (image_info->background_color != (char *) NULL)
    {
      (void) XQueryColorDatabase(image_info->background_color,&color);
      allocated_image->background_color.red=XDownScale(color.red);
      allocated_image->background_color.green=XDownScale(color.green);
      allocated_image->background_color.blue=XDownScale(color.blue);
    }
  if (image_info->border_color != (char *) NULL)
    {
      (void) XQueryColorDatabase(image_info->border_color,&color);
      allocated_image->border_color.red=XDownScale(color.red);
      allocated_image->border_color.green=XDownScale(color.green);
      allocated_image->border_color.blue=XDownScale(color.blue);
    }
  if (image_info->matte_color != (char *) NULL)
    {
      (void) XQueryColorDatabase(image_info->matte_color,&color);
      allocated_image->matte_color.red=XDownScale(color.red);
      allocated_image->matte_color.green=XDownScale(color.green);
      allocated_image->matte_color.blue=XDownScale(color.blue);
    }
  return(allocated_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e N e x t I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AllocateNextImage allocates an Image structure and initializes each
%  field to a default value.
%
%  The format of the AllocateNextImage routine is:
%
%      AllocateImage(image_info,image)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export void AllocateNextImage(const ImageInfo *image_info,Image *image)
{
  /*
    Allocate image structure.
  */
  assert(image != (Image *) NULL);
  image->next=AllocateImage(image_info);
  if (image->next == (Image *) NULL)
    return;
  (void) strcpy(image->next->filename,image->filename);
  if (image_info != (ImageInfo *) NULL)
    (void) strcpy(image->next->filename,image_info->filename);
  image->next->file=image->file;
  image->next->filesize=image->filesize;
  image->next->scene=image->scene+1;
  image->next->previous=image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n i m a t e I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AnimateImages displays one or more images to an X window.
%
%  The format of the AllocateNextImage routine is:
%
%      status=AnimateImages(image_info,image)
%
%  A description of each parameter follows:
%
%    o status: Method DisplayImages returns True if the images are displayed
%      in an X window, otherwise False is returned.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int AnimateImages(const ImageInfo *image_info,Image *image)
{
  char
    *client_name;

  Display
    *display;

  XrmDatabase
    resource_database;

  XResourceInfo
    resource;

  display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    return(False);
  XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource);
  *resource.image_info=(*image_info);
  resource.immutable=True;
  if (image_info->delay != (char *) NULL)
    resource.delay=atoi(image_info->delay);
  (void) XAnimateImages(display,&resource,&client_name,1,image);
  XCloseDisplay(display);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n n o t a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AnnotateImage annotates an image with text.  Optionally the
%  annotation can include the image filename, type, width, height, or scene
%  number by embedding special format characters.
%
%  The format of the AnnotateImage routine is:
%
%      AnnotateImage(image,annotate_info)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o annotate_info: The address of a AnnotateInfo structure.
%
%
*/
Export void AnnotateImage(Image *image,const AnnotateInfo *annotate_info)
{
  AnnotateInfo
    *local_info;

  char
    size[MaxTextExtent],
    *text,
    **textlist;

  Image
    *box_image,
    *annotate_image;

  int
    x,
    y;

  register int
    i;

  unsigned int
    height,
    length,
    number_lines,
    width;

  /*
    Ensure the annotation info is valid.
  */
  assert(image != (Image *) NULL);
  assert(annotate_info != (AnnotateInfo *) NULL);
  if (!UncondenseImage(image))
    return;
  /*
    Translate any embedded format characters (e.g. %f).
  */
  local_info=CloneAnnotateInfo(annotate_info->image_info,annotate_info);
  text=TranslateText((ImageInfo *) NULL,image,local_info->text);
  if (text == (char *) NULL)
    return;
  textlist=StringToList(text);
  if (textlist == (char **) NULL)
    return;
  length=Extent(textlist[0]);
  for (i=1; textlist[i] != (char *) NULL; i++)
    if (Extent(textlist[i]) > (int) length)
      length=Extent(textlist[i]);
  number_lines=i;
  text=(char *) AllocateMemory(length+MaxTextExtent);
  if (text == (char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to annotate image",
        "Memory allocation failed");
      return;
    }
  width=image->columns;
  height=image->rows;
  x=0;
  y=0;
  if (local_info->geometry != (char *) NULL)
    {
      int
        flags;

      /*
        User specified annotation geometry.
      */
      flags=XParseGeometry(local_info->geometry,&x,&y,&width,&height);
      if ((flags & XNegative) != 0)
        x+=image->columns;
      if ((flags & WidthValue) == 0)
        width-=x;
      if ((flags & YNegative) != 0)
        y+=image->rows;
      if ((flags & HeightValue) == 0)
        height-=y;
    }
  /*
    Annotate image.
  */
  for (i=0; textlist[i] != (char *) NULL; i++)
  {
    if (*textlist[i] == '\0')
      {
        FreeMemory(textlist[i]);
        continue;
      }
    /*
      Convert text to image.
    */
    FormatString(local_info->image_info->filename,"%.1024s",textlist[i]);
    FreeMemory(textlist[i]);
    annotate_image=ReadLABELImage(local_info->image_info);
    if (annotate_image == (Image *) NULL)
      {
        MagickWarning(ResourceLimitWarning,"Unable to annotate image",
          (char *) NULL);
        for ( ; textlist[i] != (char *) NULL; i++)
          FreeMemory(textlist[i]);
        FreeMemory((char *) textlist);
        break;
      }
    /*
      Composite text onto the image.
    */
    switch (local_info->gravity)
    {
      case NorthWestGravity:
      {
        local_info->bounds.x=x;
        local_info->bounds.y=i*local_info->bounds.height+y;
        break;
      }
      case NorthGravity:
      {
        local_info->bounds.x=x+(width >> 1)-(annotate_image->columns >> 1);
        local_info->bounds.y=y+i*local_info->bounds.height;
        break;
      }
      case NorthEastGravity:
      {
        local_info->bounds.x=x+width-annotate_image->columns;
        local_info->bounds.y=y+i*local_info->bounds.height;
        break;
      }
      case WestGravity:
      {
        local_info->bounds.x=x;
        local_info->bounds.y=y+(height >> 1)-
          (number_lines*local_info->bounds.height >> 1)+
          i*local_info->bounds.height;
        break;
      }
      case ForgetGravity:
      case StaticGravity:
      case CenterGravity:
      default:
      {
        local_info->bounds.x=x+(width >> 1)-(annotate_image->columns >> 1);
        local_info->bounds.y=y+(height >> 1)-
          (number_lines*local_info->bounds.height >> 1)+
          i*local_info->bounds.height;
        break;
      }
      case EastGravity:
      {
        local_info->bounds.x=x+width-annotate_image->columns;
        local_info->bounds.y=y+(height >> 1)-
          (number_lines*local_info->bounds.height >> 1)+
          i*local_info->bounds.height;
        break;
      }
      case SouthWestGravity:
      {
        local_info->bounds.x=x;
        local_info->bounds.y=y+height-(i+1)*local_info->bounds.height;
        break;
      }
      case SouthGravity:
      {
        local_info->bounds.x=x+(width >> 1)-(annotate_image->columns >> 1);
        local_info->bounds.y=y+height-(i+1)*local_info->bounds.height;
        break;
      }
      case SouthEastGravity:
      {
        local_info->bounds.x=x+width-annotate_image->columns;
        local_info->bounds.y=y+height-(i+1)*local_info->bounds.height;
        break;
      }
    }
    if (local_info->image_info->box != (char *) NULL)
      {
        /*
          Surround text with a bounding box.
        */
        FormatString(local_info->image_info->filename,"xc:%.1024s",
          local_info->image_info->box);
        FormatString(size,"%ux%u",annotate_image->columns,annotate_image->rows);
        (void) CloneString(&local_info->image_info->size,size);
        box_image=ReadImage(local_info->image_info);
        if (box_image != (Image *) NULL)
          {
            CompositeImage(image,ReplaceCompositeOp,box_image,
              local_info->bounds.x,local_info->bounds.y);
            DestroyImage(box_image);
          }
      }
    CompositeImage(image,AnnotateCompositeOp,annotate_image,
      local_info->bounds.x,local_info->bounds.y);
    DestroyImage(annotate_image);
  }
  DestroyAnnotateInfo(local_info);
  FreeMemory(text);
  for ( ; textlist[i] != (char *) NULL; i++)
    FreeMemory(textlist[i]);
  FreeMemory((char *) textlist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A p p e n d I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AppendImages appends a set of images.  All the input images must
%  have the same width or height.  Images of the same width are stacked
%  top-to-bottom.  Images of the same height are stacked left-to-right.
%  If the stack is false, rectangular images are stacked left-to-right
%  otherwise top-to-bottom.
%
%  The format of the AppendImage routine is:
%
%      AppendImages(images,stack)
%
%  A description of each parameter follows:
%
%    o images: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o stack: An unsigned value other than stacks rectangular images
%      top-to-bottom otherwise left-to-right.
%
%
*/
Export Image *AppendImages(Image *images,const unsigned int stack)
{
#define AppendImageText  "  Appending image sequence...  "

  Image
    *appended_image,
    *image;

  register int
    i;

  register RunlengthPacket
    *p,
    *q;

  unsigned int
    height,
    max_packets,
    scene,
    width;

  /*
    Ensure the images have the same column width.
  */
  assert(images != (Image *) NULL);
  if (images->next == (Image *) NULL)
    {
      MagickWarning(OptionWarning,"Unable to append image",
        "image sequence required");
      return((Image *) NULL);
    }
  for (image=images->next; image != (Image *) NULL; image=image->next)
    if ((image->columns != images->columns) &&
        (image->rows != images->rows))
      {
        MagickWarning(OptionWarning,"Unable to append image",
          "image widths or heights differ");
        return((Image *) NULL);
      }
  width=images->columns;
  height=images->rows;
  max_packets=images->packets;
  for (image=images->next; image != (Image *) NULL; image=image->next)
  {
    width+=image->columns;
    height+=image->rows;
    max_packets+=image->packets;
  }
  /*
    Initialize append image attributes.
  */
  images->orphan=True;
  if ((images->columns != images->next->columns) || !stack)
    appended_image=CloneImage(images,width,images->rows,False);
  else
    appended_image=CloneImage(images,images->columns,images->rows,False);
  images->orphan=False;
  if (appended_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to append image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  scene=0;
  if ((images->columns != images->next->columns) || !stack)
    {
      register int
        x;

      /*
        Stack left-to-right.
      */
      x=0;
      for (image=images; image != (Image *) NULL; image=image->next)
      {
        if (image->class == DirectClass)
          appended_image->class=DirectClass;
        CompositeImage(appended_image,ReplaceCompositeOp,image,x,0);
        x+=image->columns;
        ProgressMonitor(AppendImageText,scene,GetNumberScenes(images));
        scene++;
      }
    }
  else
    {
      /*
        Stack top-to-bottom.
      */
      appended_image->rows=height;
      appended_image->packets=max_packets;
      appended_image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
        appended_image->pixels,appended_image->packets*sizeof(RunlengthPacket));
      if (appended_image->pixels == (RunlengthPacket *) NULL)
        {
          DestroyImage(appended_image);
          MagickWarning(ResourceLimitWarning,"Unable to append image",
            "Memory allocation failed");
          return((Image *) NULL);
        }
      q=appended_image->pixels;
      for (image=images; image != (Image *) NULL; image=image->next)
      {
        if (image->class == DirectClass)
          appended_image->class=DirectClass;
        p=image->pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          *q=(*p);
          p++;
          q++;
        }
        ProgressMonitor(AppendImageText,scene,GetNumberScenes(images));
        scene++;
      }
    }
  if (appended_image->class == PseudoClass)
    {
      unsigned int
        global_colormap;

      /*
        Determine if the sequence of images has the identical colormap.
      */
      global_colormap=True;
      for (image=images; image != (Image *) NULL; image=image->next)
      {
        if ((image->class == DirectClass) ||
            (image->colors != images->colors))
          {
            global_colormap=False;
            break;
          }
        for (i=0; i < (int) images->colors; i++)
          if (!ColorMatch(image->colormap[i],images->colormap[i],image->fuzz))
            {
              global_colormap=False;
              break;
            }
      }
      if (!global_colormap)
        appended_image->class=DirectClass;
    }
  return(appended_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A v e r a g e I m a g e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AverageImages averages a set of images.  All the input images must
%  be the same size in pixels.
%
%  The format of the AverageImage routine is:
%
%      averaged_image=AverageImages(images)
%
%  A description of each parameter follows:
%
%    o averaged_image: Method AverageImages returns the mean pixel value
%      for an image sequence.
%
%    o images: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export Image *AverageImages(const Image *images)
{
#define AverageImageText  "  Average image sequence...  "

  typedef struct _SumPacket
  {
    double
      red,
      green,
      blue,
      index;
  } SumPacket;

  const Image
    *image;

  Image
    *averaged_image;

  int
    x;

  register int
    i,
    j;

  register RunlengthPacket
    *p,
    *q;

  SumPacket
    *sum;

  unsigned int
    number_scenes;

  assert(images != (Image *) NULL);
  if (images->next == (Image *) NULL)
    {
      MagickWarning(OptionWarning,"Unable to average image",
        "image sequence required");
      return((Image *) NULL);
    }
  /*
    Ensure the images are the same size.
  */
  for (image=images; image != (Image *) NULL; image=image->next)
  {
    if ((image->columns != images->columns) ||
        (image->rows != images->rows))
      {
        MagickWarning(OptionWarning,"Unable to average image",
          "images are not the same size");
        return((Image *) NULL);
      }
  }
  /*
    Allocate sum accumulation buffer.
  */
  sum=(SumPacket *)
    AllocateMemory(images->columns*images->rows*sizeof(SumPacket));
  if (sum == (SumPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to average image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  for (i=0; i < (int) (images->columns*images->rows); i++)
  {
    sum[i].red=0.0;
    sum[i].green=0.0;
    sum[i].blue=0.0;
    sum[i].index=0.0;
  }
  /*
    Initialize average image attributes.
  */
  ((Image *) images)->orphan=True;
  averaged_image=CloneImage(images,images->columns,images->rows,False);
  ((Image *) images)->orphan=False;
  if (averaged_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to average image",
        "Memory allocation failed");
      FreeMemory((char *) sum);
      return((Image *) NULL);
    }
  /*
    Compute sum over each pixel color component.
  */
  averaged_image->class=DirectClass;
  number_scenes=0;
  for (image=images; image != (Image *) NULL; image=image->next)
  {
    x=0;
    p=image->pixels;
    for (i=0; i < (int) image->packets; i++)
    {
      for (j=0; j <= (int) p->length; j++)
      {
        sum[x].red+=p->red;
        sum[x].green+=p->green;
        sum[x].blue+=p->blue;
        sum[x].index+=p->index;
        x++;
      }
      p++;
    }
    number_scenes++;
  }
  /*
    Average image pixels.
  */
  q=averaged_image->pixels;
  for (i=0; i < (int) averaged_image->packets; i++)
  {
    q->red=(Quantum) ((sum[i].red+number_scenes/2.0)/number_scenes);
    q->green=(Quantum) ((sum[i].green+number_scenes/2.0)/number_scenes);
    q->blue=(Quantum) ((sum[i].blue+number_scenes/2.0)/number_scenes);
    q->index=(Quantum) ((sum[i].index+number_scenes/2.0)/number_scenes);
    q->length=0;
    q++;
    if (QuantumTick(i,averaged_image->packets))
      ProgressMonitor(AverageImageText,i,averaged_image->packets);
  }
  FreeMemory((char *) sum);
  return(averaged_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B o r d e r I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method BorderImage takes an image and puts a border around it of a
%  particular color.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the BorderImage routine is:
%
%      bordered_image=BorderImage(image,border_info)
%
%  A description of each parameter follows:
%
%    o bordered_image: Method BorderImage returns a pointer to the bordered
%      image.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o border_info: Specifies a pointer to a structure of type Rectangle which
%      defines the border region.
%
*/
Export Image *BorderImage(const Image *image,const RectangleInfo *border_info)
{
  ColorPacket
    matte_color;

  Image
    *bordered_image;

  FrameInfo
    frame_info;

  assert(image != (Image *) NULL);
  assert(border_info != (RectangleInfo *) NULL);
  frame_info.width=image->columns+(border_info->width << 1);
  frame_info.height=image->rows+(border_info->height << 1);
  frame_info.x=border_info->width;
  frame_info.y=border_info->height;
  frame_info.inner_bevel=0;
  frame_info.outer_bevel=0;
  matte_color=image->matte_color;
  ((Image *) image)->matte_color=image->border_color;
  bordered_image=FrameImage(image,&frame_info);
  bordered_image->matte_color=matte_color;
  ((Image *) image)->matte_color=matte_color;
  return(bordered_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C h o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ChopImage creates a new image that is a subregion of an existing
%  one.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ChopImage routine is:
%
%      chop_image=ChopImage(image,chop_info)
%
%  A description of each parameter follows:
%
%    o chop_image: Method ChopImage returns a pointer to the chop
%      image.  A null image is returned if there is a memory shortage or
%      if the image width or height is zero.
%
%    o image: The address of a structure of type Image.
%
%    o chop_info: Specifies a pointer to a RectangleInfo which defines the
%      region of the image to crop.
%
%
*/
Export Image *ChopImage(const Image *image,const RectangleInfo *chop_info)
{
#define ChopImageText  "  Chopping image...  "

  Image
    *chopped_image;

  int
    y;

  RectangleInfo
    local_info;

  register int
    runlength,
    x;

  register RunlengthPacket
    *p,
    *q;

  unsigned int
    height;

  /*
    Check chop geometry.
  */
  assert(image != (Image *) NULL);
  assert(chop_info != (RectangleInfo *) NULL);
  if (((chop_info->x+(int) chop_info->width) < 0) ||
      ((chop_info->y+(int) chop_info->height) < 0) ||
      (chop_info->x >= (int) image->columns) ||
      (chop_info->y >= (int) image->rows))
    {
      MagickWarning(OptionWarning,"Unable to chop image",
        "geometry does not contain image");
      return((Image *) NULL);
    }
  local_info=(*chop_info);
  if ((local_info.x+(int) local_info.width) > (int) image->columns)
    local_info.width=(unsigned int) ((int) image->columns-local_info.x);
  if ((local_info.y+(int) local_info.height) > (int) image->rows)
    local_info.height=(unsigned int) ((int) image->rows-local_info.y);
  if (local_info.x < 0)
    {
      local_info.width-=(unsigned int) (-local_info.x);
      local_info.x=0;
    }
  if (local_info.y < 0)
    {
      local_info.height-=(unsigned int) (-local_info.y);
      local_info.y=0;
    }
  /*
    Initialize chop image attributes.
  */
  chopped_image=CloneImage(image,image->columns-local_info.width,
    image->rows-local_info.height,False);
  if (chopped_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to chop image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Extract chop image.
  */
  p=image->pixels;
  runlength=p->length+1;
  q=chopped_image->pixels;
  for (y=0; y < local_info.y; y++)
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      if ((x < local_info.x) || (x >= (int) (local_info.x+local_info.width)))
        {
          *q=(*p);
          q->length=0;
          q++;
        }
    }
  /*
    Skip pixels up to the chop image.
  */
  for (x=0; x < (int) (local_info.height*image->columns); x++)
    if (runlength != 0)
      runlength--;
    else
      {
        p++;
        runlength=p->length;
      }
  /*
    Extract chop image.
  */
  height=image->rows-(local_info.y+local_info.height);
  for (y=0; y < (int) height; y++)
  {
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      if ((x < local_info.x) || (x >= (int) (local_info.x+local_info.width)))
        {
          *q=(*p);
          q->length=0;
          q++;
        }
    }
    if (QuantumTick(y,height))
      ProgressMonitor(ChopImageText,y,height);
  }
  return(chopped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e A n n o t a t e I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloneAnnotateInfo makes a duplicate of the given annotate info, or if
%  annotate info is NULL, a new one.
%
%  The format of the CloneAnnotateInfo routine is:
%
%      cloned_info=CloneAnnotateInfo(image_info,annotate_info)
%
%  A description of each parameter follows:
%
%    o cloned_info: Method CloneAnnotateInfo returns a duplicate of the given
%      annotate info, or if annotate info is NULL a new one.
%
%    o image_info: a structure of type info.
%
%    o annotate_info: a structure of type info.
%
%
*/
Export AnnotateInfo *CloneAnnotateInfo(const ImageInfo *image_info,
  const AnnotateInfo *annotate_info)
{
  AnnotateInfo
    *cloned_info;

  cloned_info=(AnnotateInfo *) AllocateMemory(sizeof(AnnotateInfo));
  if (cloned_info == (AnnotateInfo *) NULL)
    MagickError(ResourceLimitWarning,"Unable to clone annotate info",
      "Memory allocation failed");
  if (annotate_info == (AnnotateInfo *) NULL)
    {
      GetAnnotateInfo(image_info,cloned_info);
      return(cloned_info);
    }
  *cloned_info=(*annotate_info);
  if (annotate_info->image_info != (ImageInfo *) NULL)
    cloned_info->image_info=CloneImageInfo(annotate_info->image_info);
  if (annotate_info->geometry != (char *) NULL)
    cloned_info->geometry=AllocateString(annotate_info->geometry);
  if (annotate_info->text != (char *) NULL)
    cloned_info->text=AllocateString(annotate_info->text);
  if (annotate_info->primitive != (char *) NULL)
    cloned_info->primitive=AllocateString(annotate_info->primitive);
  if (annotate_info->font_name != (char *) NULL)
    cloned_info->font_name=AllocateString(annotate_info->font_name);
  if (annotate_info->tile != (Image *) NULL)
    cloned_info->tile=CloneImage(annotate_info->tile,
      annotate_info->tile->columns,annotate_info->tile->rows,True);
  return(cloned_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloneImage returns a copy of all fields of the input image.  The
%  the pixel memory is allocated but the pixel data is not copied.
%
%  The format of the CloneImage routine is:
%
%      clone_image=CloneImage(image,columns,rows,clone_pixels)
%
%  A description of each parameter follows:
%
%    o clone_image: Method CloneImage returns a pointer to the image after
%      copying.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o columns: An integer that specifies the number of columns in the copied
%      image.
%
%    o rows: An integer that specifies the number of rows in the copied
%      image.
%
%    o clone_pixels: Specifies whether the pixel data is copied.  Must be
%      either True or False;
%
%
*/
Export Image *CloneImage(const Image *image,const unsigned int columns,
  const unsigned int rows,const unsigned int clone_pixels)
{
  Image
    *clone_image;

  register int
    i;

  /*
    Allocate image structure.
  */
  assert(image != (Image *) NULL);
  clone_image=(Image *) AllocateMemory(sizeof(Image));
  if (clone_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Allocate the image pixels.
  */
  *clone_image=(*image);
  clone_image->columns=columns;
  clone_image->rows=rows;
  if (!clone_pixels)
    clone_image->packets=clone_image->columns*clone_image->rows;
  clone_image->pixels=(RunlengthPacket *) AllocateMemory((unsigned int)
    clone_image->packets*sizeof(RunlengthPacket));
  if (clone_image->pixels == (RunlengthPacket *) NULL)
    return((Image *) NULL);
  if (!clone_pixels)
    {
      clone_image->tainted=True;
      SetImage(clone_image);
    }
  else
    {
      register RunlengthPacket
        *p,
        *q;

      /*
        Copy image pixels.
      */
      p=image->pixels;
      q=clone_image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        *q=(*p);
        p++;
        q++;
      }
    }
  clone_image->packed_pixels=(unsigned char *) NULL;
  clone_image->comments=(char *) NULL;
  if (image->comments != (char *) NULL)
    (void) CloneString(&clone_image->comments,image->comments);
  clone_image->label=(char *) NULL;
  if (image->label != (char *) NULL)
    (void) CloneString(&clone_image->label,image->label);
  clone_image->montage=(char *) NULL;
  if (clone_pixels)
    if (image->montage != (char *) NULL)
      (void) CloneString(&clone_image->montage,image->montage);
  clone_image->directory=(char *) NULL;
  if (clone_pixels)
    if (image->directory != (char *) NULL)
      (void) CloneString(&clone_image->directory,image->directory);
  clone_image->signature=(char *) NULL;
  if (clone_pixels)
    if (image->signature != (char *) NULL)
      (void) CloneString(&clone_image->signature,image->signature);
  clone_image->page=(char *) NULL;
  if (clone_pixels)
    if (image->page != (char *) NULL)
      (void) CloneString(&clone_image->page,image->page);
  if (image->colormap != (ColorPacket *) NULL)
    {
      /*
        Allocate and copy the image colormap.
      */
      clone_image->colormap=(ColorPacket *)
        AllocateMemory(image->colors*sizeof(ColorPacket));
      if (clone_image->colormap == (ColorPacket *) NULL)
        return((Image *) NULL);
      for (i=0; i < (int) image->colors; i++)
        clone_image->colormap[i]=image->colormap[i];
    }
  if (image->color_profile.length > 0)
    {
      /*
        Allocate and copy the image ICC profile.
      */
      clone_image->color_profile.info=(unsigned char *)
        AllocateMemory(image->color_profile.length*sizeof(unsigned char));
      if (clone_image->color_profile.info == (unsigned char *) NULL)
        return((Image *) NULL);
      for (i=0; i < (int) image->color_profile.length; i++)
        clone_image->color_profile.info[i]=image->color_profile.info[i];
    }
  if (image->iptc_profile.length > 0)
    {
      /*
        Allocate and copy the image IPTC profile.
      */
      clone_image->iptc_profile.info=(unsigned char *)
        AllocateMemory(image->iptc_profile.length*sizeof(unsigned char));
      if (clone_image->iptc_profile.info == (unsigned char *) NULL)
        return((Image *) NULL);
      for (i=0; i < (int) image->iptc_profile.length; i++)
        clone_image->iptc_profile.info[i]=image->iptc_profile.info[i];
    }
  if (image->orphan)
    {
      clone_image->file=(FILE *) NULL;
      clone_image->previous=(Image *) NULL;
      clone_image->next=(Image *) NULL;
    }
  else
    {
      /*
        Link image into image list.
      */
      if (clone_image->previous != (Image *) NULL)
        clone_image->previous->next=clone_image;
      if (clone_image->next != (Image *) NULL)
        clone_image->next->previous=clone_image;
    }
  clone_image->orphan=False;
  return(clone_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloneImageInfo makes a duplicate of the given image info, or if
%  image info is NULL, a new one.
%
%  The format of the CloneImageInfo routine is:
%
%      cloned_info=CloneImageInfo(image_info)
%
%  A description of each parameter follows:
%
%    o cloned_info: Method CloneImageInfo returns a duplicate of the given
%      image info, or if image info is NULL a new one.
%
%    o image_info: a structure of type info.
%
%
*/
Export ImageInfo *CloneImageInfo(const ImageInfo *image_info)
{
  ImageInfo
    *cloned_info;

  cloned_info=(ImageInfo *) AllocateMemory(sizeof(ImageInfo));
  if (cloned_info == (ImageInfo *) NULL)
    MagickError(ResourceLimitWarning,"Unable to clone image info",
      "Memory allocation failed");
  if (image_info == (ImageInfo *) NULL)
    {
      GetImageInfo(cloned_info);
      return(cloned_info);
    }
  *cloned_info=(*image_info);
  if (image_info->server_name != (char *) NULL)
    cloned_info->server_name=AllocateString(image_info->server_name);
  if (image_info->size != (char *) NULL)
    cloned_info->size=AllocateString(image_info->size);
  if (image_info->tile != (char *) NULL)
    cloned_info->tile=AllocateString(image_info->tile);
  if (image_info->density != (char *) NULL)
    cloned_info->density=AllocateString(image_info->density);
  if (image_info->page != (char *) NULL)
    cloned_info->page=AllocateString(image_info->page);
  if (image_info->dispose != (char *) NULL)
    cloned_info->dispose=AllocateString(image_info->dispose);
  if (image_info->delay != (char *) NULL)
    cloned_info->delay=AllocateString(image_info->delay);
  if (image_info->iterations != (char *) NULL)
    cloned_info->iterations=AllocateString(image_info->iterations);
  if (image_info->texture != (char *) NULL)
    cloned_info->texture=AllocateString(image_info->texture);
  if (image_info->box != (char *) NULL)
    cloned_info->box=AllocateString(image_info->box);
  if (image_info->font != (char *) NULL)
    cloned_info->font=AllocateString(image_info->font);
  if (image_info->pen != (char *) NULL)
    cloned_info->pen=AllocateString(image_info->pen);
  if (image_info->background_color != (char *) NULL)
    cloned_info->background_color=AllocateString(image_info->background_color);
  if (image_info->border_color != (char *) NULL)
    cloned_info->border_color=AllocateString(image_info->border_color);
  if (image_info->matte_color != (char *) NULL)
    cloned_info->matte_color=AllocateString(image_info->matte_color);
  return(cloned_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o s e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloseImage closes a file associated with the image.  If the
%  filename prefix is '|', the file is a pipe and is closed with PipeClose.
%
%  The format of the CloseImage routine is:
%
%      CloseImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
Export void CloseImage(Image *image)
{
  /*
    Close image file.
  */
  assert(image != (Image *) NULL);
  if (image->file == (FILE *) NULL)
    return;
  (void) fflush(image->file);
  image->status=ferror(image->file);
  (void) fseek(image->file,0L,SEEK_END);
  image->filesize=ftell(image->file);
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
  if (image->pipe)
    (void) pclose(image->file);
  else
#endif
    if (!image->exempt)
      (void) fclose(image->file);
  image->file=(FILE *) NULL;
  if (!image->orphan)
    {
      while (image->previous != (Image *) NULL)
        image=image->previous;
      for ( ; image != (Image *) NULL; image=image->next)
        image->file=(FILE *) NULL;
    }
  errno=0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o a l e s c e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CoalesceImages merges a sequence of images.  This is useful for GIF
%  animation sequences that have page offsets and disposal methods.
%
%  The format of the CoalesceImages routine is:
%
%      CoalesceImages(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export void CoalesceImages(Image *image)
{
  Image
    *cloned_image,
    *p;

  int
    x,
    y;

  unsigned int
    sans;

  assert(image != (Image *) NULL);
  /*
    Determine if all the images in the sequence are the same size.
  */
  for (p=image->next; p != (Image *) NULL; p=p->next)
    if ((p->columns != p->previous->columns) ||
        (p->rows != p->previous->rows))
      break;
  if (p == (Image *) NULL)
    return;
  /*
    Coalesce the image sequence.
  */
  image->matte=False;
  if (image->page)
    {
      FreeMemory((char *) image->page);
      image->page=(char *) NULL;
    }
  for (p=image->next; p != (Image *) NULL; p=p->next)
  {
    x=0;
    y=0;
    (void) XParseGeometry(p->page,&x,&y,&sans,&sans);
    p->orphan=True;
    cloned_image=CloneImage(p,p->columns,p->rows,True);
    p->orphan=False;
    if (cloned_image == (Image *) NULL)
      {
        MagickWarning(ResourceLimitWarning,"Unable to coalesce image",
          "Memory allocation failed");
        return;
      }
    p->columns=p->previous->columns;
    p->rows=p->previous->rows;
    p->packets=p->columns*p->rows;
    p->pixels=(RunlengthPacket *) ReallocateMemory((char *)
      p->pixels,p->packets*sizeof(RunlengthPacket));
    if (p->pixels == (RunlengthPacket *) NULL)
      {
        MagickWarning(ResourceLimitWarning,"Unable to coalesce image",
          "Memory allocation failed");
        return;
      }
    if (p->page)
      {
        FreeMemory((char *) p->page);
        p->page=(char *) NULL;
      }
    CompositeImage(p,ReplaceCompositeOp,p->previous,0,0);
    CompositeImage(p,cloned_image->matte ? OverCompositeOp : ReplaceCompositeOp,
      cloned_image,x,y);
    DestroyImage(cloned_image);
    p->matte=False;
    CondenseImage(p);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o l o r F l o o d f i l l I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ColorFloodfillImage floodfills the designated area with a color.
%  The floodfill algorithm is strongly based on a similar algorithm in
%  "Graphics Gems" by Paul Heckbert.
%
%  The format of the ColorFloodfillImage routine is:
%
%      ColorFloodfillImage(image,target,tile,x,y,method)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o target: A RunlengthPacket structure.  This is the RGB value of the target
%      color.
%
%    o tile: An image representing the image to tile onto the floodplane.
%
%    o x,y: Unsigned integers representing the current location of the pen.
%
%    o method: drawing method of type PrimitiveType: floodfill or fill to
%      border.
%
%
*/
Export void ColorFloodfillImage(Image *image,const RunlengthPacket *target,
  Image *tile,const int x_offset,const int y_offset,const PaintMethod method)
{
  ColorPacket
    color;

  int
    offset,
    skip,
    start,
    x1,
    x2;

  register int
    i,
    x,
    y;

  register RunlengthPacket
    *pixel,
    *q;

  register XSegment
    *p;

  register unsigned char
    *r;

  XSegment
    *segment_stack;

  unsigned char
    *markers;

  /*
    Check boundary conditions.
  */
  assert(image != (Image *) NULL);
  assert(tile != (Image *) NULL);
  if ((x_offset < 0) || (x_offset >= (int) image->columns))
    return;
  if ((y_offset < 0) || (y_offset >= (int) image->rows))
    return;
  if (!UncondenseImage(image))
    return;
  if (!UncondenseImage(tile))
    return;
  /*
    Set floodfill color.
  */
  x=x_offset;
  y=y_offset;
  color.red=tile->pixels[0].red;
  color.green=tile->pixels[0].green;
  color.blue=tile->pixels[0].blue;
  markers=(unsigned char *)
    AllocateMemory(image->rows*image->columns*sizeof(unsigned char));
  if (markers == (unsigned char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to floodfill image",
        "Memory allocation failed");
      return;
    }
  for (i=0; i < (int) (image->rows*image->columns); i++)
    markers[i]=False;
  if (ColorMatch(color,*target,image->fuzz))
    return;
  /*
    Allocate segment stack.
  */
  segment_stack=(XSegment *) AllocateMemory(MaxStacksize*sizeof(XSegment));
  if (segment_stack == (XSegment *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to floodfill image",
        "Memory allocation failed");
      if (tile != (Image *) NULL)
        {
          FreeMemory((char *) markers);
          DestroyImage(tile);
        }
      return;
    }
  /*
    Push initial segment on stack.
  */
  image->class=DirectClass;
  start=0;
  p=segment_stack;
  Push(y,x,x,1);
  Push(y+1,x,x,-1);
  while (p > segment_stack)
  {
    /*
      Pop segment off stack.
    */
    p--;
    x1=p->x1;
    x2=p->x2;
    offset=p->y2;
    y=p->y1+offset;
    /*
      Recolor neighboring pixels.
    */
    pixel=PixelOffset(image,x1,y);
    for (x=x1; x >= 0 ; x--)
    {
      if (method == FloodfillMethod)
        {
          if (!ColorMatch(*pixel,*target,image->fuzz))
            break;
        }
      else
        if (ColorMatch(*pixel,*target,image->fuzz) ||
            ColorMatch(*pixel,color,image->fuzz))
          break;
      pixel->red=color.red;
      pixel->green=color.green;
      pixel->blue=color.blue;
      if (markers != (unsigned char *) NULL)
        markers[y*image->columns+x]=True;
      pixel--;
    }
    skip=x >= x1;
    if (!skip)
      {
        start=x+1;
        if (start < x1)
          Push(y,start,x1-1,-offset);
        x=x1+1;
      }
    do
    {
      if (!skip)
        {
          pixel=PixelOffset(image,x,y);
          for ( ; x < (int) image->columns; x++)
          {
            if (method == FloodfillMethod)
              {
                if (!ColorMatch(*pixel,*target,image->fuzz))
                  break;
              }
            else
              if (ColorMatch(*pixel,*target,image->fuzz) ||
                  ColorMatch(*pixel,color,image->fuzz))
                break;
            pixel->red=color.red;
            pixel->green=color.green;
            pixel->blue=color.blue;
            if (markers != (unsigned char *) NULL)
              markers[y*image->columns+x]=True;
            pixel++;
          }
          Push(y,start,x-1,offset);
          if (x > (x2+1))
            Push(y,x2+1,x-1,-offset);
        }
      skip=False;
      pixel=PixelOffset(image,x,y);
      for (x++; x <= x2; x++)
      {
        pixel++;
        if (method == FloodfillMethod)
          {
            if (ColorMatch(*pixel,*target,image->fuzz))
              break;
          }
        else
          if (!ColorMatch(*pixel,*target,image->fuzz) &&
              !ColorMatch(*pixel,color,image->fuzz))
            break;
      }
      start=x;
    } while (x <= x2);
  }
  /*
    Tile image onto floodplane.
  */
  r=markers;
  q=image->pixels;
  for (y=0; y < (int) image->rows; y++)
  {
    for (x=0; x < (int) image->columns; x++)
    {
      if (*r)
        {
          pixel=PixelOffset(tile,x % tile->columns,y % tile->rows);
          if (!tile->matte)
            {
              q->red=pixel->red;
              q->green=pixel->green;
              q->blue=pixel->blue;
            }
          else
            {
              q->red=(Quantum) ((long) (pixel->red*pixel->index+
                q->red*(Opaque-pixel->index))/Opaque);
              q->green=(Quantum) ((long) (pixel->green*pixel->index+
                q->green*(Opaque-pixel->index))/Opaque);
              q->blue=(Quantum) ((long) (pixel->blue*pixel->index+
                q->blue*(Opaque-pixel->index))/Opaque);
              q->index=(unsigned short) ((long) (pixel->index*pixel->index+
                q->index*(Opaque-pixel->index))/Opaque);
            }
        }
      r++;
      q++;
    }
  }
  FreeMemory((char *) markers);
  FreeMemory((char *) segment_stack);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o l o r i z e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ColorizeImage colorizes an image with the pen color.  The amount
%  of the coloring is controlled with the opacity levels.
%
%  The format of the ColorizeImage routine is:
%
%      ColorizeImage(image,opaque_color,pen_color)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o opaque_color,
%      pen_color: A character string that contain an X11 color string.
%
%
*/
Export void ColorizeImage(Image *image,const char *opacity,
  const char *pen_color)
{
#define ColorizeImageText  "  Colorizing the image...  "

  ColorPacket
    target;

  int
    blue_opacity,
    count,
    green_opacity,
    red_opacity;

  long
    value;

  register int
    i;

  register RunlengthPacket
    *p;

  unsigned int
    status;

  XColor
    target_color;

  /*
    Determine RGB values of the pen color.
  */
  assert(image != (Image *) NULL);
  if (opacity == (char *) NULL)
    return;
  status=XQueryColorDatabase(pen_color,&target_color);
  if (status == False)
    return;
  target.red=XDownScale(target_color.red);
  target.green=XDownScale(target_color.green);
  target.blue=XDownScale(target_color.blue);
  red_opacity=100;
  green_opacity=100;
  blue_opacity=100;
  count=sscanf(opacity,"%d/%d/%d",&red_opacity,&green_opacity,&blue_opacity);
  if (count == 1)
    {
      if (red_opacity == 0)
        return;
      green_opacity=red_opacity;
      blue_opacity=red_opacity;
    }
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Colorize DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        value=(long) (p->red*(100-red_opacity)+target.red*red_opacity)/100;
        p->red=(Quantum) ((value < 0) ? 0 : (value > MaxRGB) ? MaxRGB : value);
        value=(long)
          (p->green*(100-green_opacity)+target.green*green_opacity)/100;
        p->green=(Quantum)
          ((value < 0) ? 0 : (value > MaxRGB) ? MaxRGB : value);
        value=(long) (p->blue*(100-blue_opacity)+target.blue*blue_opacity)/100;
        p->blue=(Quantum) ((value < 0) ? 0 : (value > MaxRGB) ? MaxRGB : value);
        p++;
        if (QuantumTick(i,image->packets))
          ProgressMonitor(ColorizeImageText,i,image->packets);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Colorize PseudoClass image.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        value=(long)
          (image->colormap[i].red*(100-red_opacity)+target.red*red_opacity)/100;
        image->colormap[i].red=(Quantum)
          ((value < 0) ? 0 : (value > MaxRGB) ? MaxRGB : value);
        value=(long) (image->colormap[i].green*(100-green_opacity)+
          target.green*green_opacity)/100;
        image->colormap[i].green=(Quantum)
          ((value < 0) ? 0 : (value > MaxRGB) ? MaxRGB : value);
        value=(long) (image->colormap[i].blue*(100-blue_opacity)+
          target.blue*blue_opacity)/100;
        image->colormap[i].blue=(Quantum)
          ((value < 0) ? 0 : (value > MaxRGB) ? MaxRGB : value);
      }
      SyncImage(image);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m m e n t I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CommentImage initializes an image comment.  Optionally the
%  comment can include the image filename, type, width, height, or scene
%  number by embedding special format characters.
%
%  The format of the CommentImage routine is:
%
%      CommentImage(image,comments)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o comments: The address of a character string containing the comment
%      format.
%
%
*/
Export void CommentImage(Image *image,const char *comments)
{
  if (image->comments != (char *) NULL)
    FreeMemory((char *) image->comments);
  image->comments=TranslateText((ImageInfo *) NULL,image,comments);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p r e s s C o l o r m a p                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CompressColormap compresses an image colormap removing any
%  duplicate and unused color entries.
%
%  The format of the CompressColormap routine is:
%
%      CompressColormap(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
Export void CompressColormap(Image *image)
{
  ColorPacket
    *colormap;

  int
    number_colors;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned short
    index;

  /*
    Determine if colormap can be compressed.
  */
  assert(image != (Image *) NULL);
  if (image->class != PseudoClass)
    return;
  number_colors=image->colors;
  for (i=0; i < (int) image->colors; i++)
    image->colormap[i].flags=False;
  image->colors=0;
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    if (!image->colormap[p->index].flags)
      {
        /*
          Eliminate duplicate colors.
        */
        for (j=0; j < number_colors; j++)
          if ((j != p->index) && image->colormap[j].flags)
            if (ColorMatch(image->colormap[p->index],image->colormap[j],0))
              break;
        if (j != number_colors)
          image->colormap[p->index].index=image->colormap[j].index;
        else
          image->colormap[p->index].index=image->colors++;
        image->colormap[p->index].flags=True;
      }
    p++;
  }
  if ((int) image->colors == number_colors)
    return;  /* no duplicate or unused entries */
  /*
    Compress colormap.
  */
  colormap=(ColorPacket *) AllocateMemory(image->colors*sizeof(ColorPacket));
  if (colormap == (ColorPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to compress colormap",
        "Memory allocation failed");
      image->colors=number_colors;
      return;
    }
  /*
    Eliminate unused colormap entries.
  */
  for (i=0; i < number_colors; i++)
    if (image->colormap[i].flags)
      {
        index=image->colormap[i].index;
        colormap[index].red=image->colormap[i].red;
        colormap[index].green=image->colormap[i].green;
        colormap[index].blue=image->colormap[i].blue;
      }
  /*
    Remap pixels.
  */
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    p->index=image->colormap[p->index].index;
    p++;
  }
  FreeMemory((char *) image->colormap);
  image->colormap=colormap;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p o s i t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CompositeImage returns the second image composited onto the
%  first at the specified offsets.
%
%  The format of the CompositeImage routine is:
%
%      CompositeImage(image,compose,composite_image,x_offset,y_offset)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o compose: Specifies an image composite operator.
%
%    o composite_image: The address of a structure of type Image.
%
%    o x_offset: An integer that specifies the column offset of the composited
%      image.
%
%    o y_offset: An integer that specifies the row offset of the composited
%      image.
%
%
*/
Export void CompositeImage(Image *image,const CompositeOperator compose,
  Image *composite_image,const int x_offset,const int y_offset)
{
  int
    y;

  long
    blue,
    green,
    index,
    red;

  Quantum
    shade;

  register int
    i,
    runlength,
    x;

  register RunlengthPacket
    *p,
    *q;

  /*
    Check composite geometry.
  */
  assert(image != (Image *) NULL);
  assert(composite_image != (Image *) NULL);
  if (((x_offset+(int) image->columns) < 0) ||
      ((y_offset+(int) image->rows) < 0) ||
      (x_offset > (int) image->columns) || (y_offset > (int) image->rows))
    {
      MagickWarning(ResourceLimitWarning,"Unable to composite image",
        "geometry does not contain image");
      return;
    }
  /*
    Image must be uncompressed.
  */
  if (!UncondenseImage(image))
    return;
  switch (compose)
  {
    case XorCompositeOp:
    case PlusCompositeOp:
    case MinusCompositeOp:
    case AddCompositeOp:
    case SubtractCompositeOp:
    case DifferenceCompositeOp:
    case BumpmapCompositeOp:
    case BlendCompositeOp:
    case ReplaceRedCompositeOp:
    case ReplaceGreenCompositeOp:
    case ReplaceBlueCompositeOp:
    {
      image->class=DirectClass;
      break;
    }
    case ReplaceMatteCompositeOp:
    {
      image->class=DirectClass;
      image->matte=True;
      break;
    }
    case DisplaceCompositeOp:
    {
      ColorPacket
        interpolated_color;

      double
        x_displace,
        y_displace;

      double
        horizontal_scale,
        vertical_scale;

      Image
        *displaced_image;

      register RunlengthPacket
        *r;

      /*
        Allocate the displaced image.
      */
      composite_image->orphan=True;
      displaced_image=CloneImage(composite_image,composite_image->columns,
        composite_image->rows,False);
      composite_image->orphan=False;
      if (displaced_image == (Image *) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Unable to display image",
            "Memory allocation failed");
          return;
        }
      horizontal_scale=20.0;
      vertical_scale=20.0;
      if (composite_image->geometry != (char *) NULL)
        {
          int
            count;

          /*
            Determine the horizontal and vertical displacement scale.
          */
          count=sscanf(composite_image->geometry,"%lfx%lf\n",
            &horizontal_scale,&vertical_scale);
          if (count == 1)
            vertical_scale=horizontal_scale;
        }
      /*
        Shift image pixels as defined by a displacement map.
      */
      p=composite_image->pixels;
      runlength=p->length+1;
      r=displaced_image->pixels;
      for (y=0; y < (int) composite_image->rows; y++)
      {
        if (((y_offset+y) < 0) || ((y_offset+y) >= (int) image->rows))
          {
            p+=composite_image->columns;
            continue;
          }
        q=image->pixels+(y_offset+y)*image->columns+x_offset;
        for (x=0; x < (int) composite_image->columns; x++)
        {
          if (runlength != 0)
            runlength--;
          else
            {
              p++;
              runlength=p->length;
            }
          if (((x_offset+x) < 0) || ((x_offset+x) >= (int) image->columns))
            {
              q++;
              continue;
            }
          x_displace=(horizontal_scale*((double) Intensity(*p)-
            ((MaxRGB+1) >> 1)))/((MaxRGB+1) >> 1);
          y_displace=x_displace;
          if (composite_image->matte)
            y_displace=(vertical_scale*((double) p->index-
              ((MaxRGB+1) >> 1)))/((MaxRGB+1) >> 1);
          interpolated_color=
            InterpolateColor(image,x_offset+x+x_displace,y_offset+y+y_displace);
          r->red=interpolated_color.red;
          r->green=interpolated_color.green;
          r->blue=interpolated_color.blue;
          r->index=interpolated_color.index;
          r->length=0;
          q++;
          r++;
        }
      }
      composite_image=displaced_image;
      break;
    }
    case ReplaceCompositeOp:
    {
      /*
        Promote image to DirectClass if colormaps differ.
      */
      if (image->class == PseudoClass)
        {
          if ((composite_image->class == DirectClass) ||
              (composite_image->colors != image->colors))
            image->class=DirectClass;
          else
            {
              int
                status;

              status=memcmp((char *) composite_image->colormap,
                (char *) image->colormap,composite_image->colors*
                sizeof(ColorPacket));
              if (status != 0)
                image->class=DirectClass;
            }
        }
      if (image->matte && !composite_image->matte)
        MatteImage(composite_image);
      break;
    }
    default:
    {
      /*
        Initialize image matte data.
      */
      if (!image->matte)
        {
          image->class=DirectClass;
          if (compose != AnnotateCompositeOp)
            MatteImage(image);
        }
      if (!composite_image->matte)
        {
          p=composite_image->pixels;
          red=p->red;
          green=p->green;
          blue=p->blue;
          if (IsMonochromeImage(composite_image))
            {
              red=composite_image->background_color.red;
              green=composite_image->background_color.green;
              blue=composite_image->background_color.blue;
            }
          for (i=0; i < (int) composite_image->packets; i++)
          {
            p->index=Opaque;
            if ((p->red == red) && (p->green == green) &&
                (p->blue == blue))
              p->index=Transparent;
            p++;
          }
          composite_image->class=DirectClass;
          composite_image->matte=True;
        }
      break;
    }
  }
  /*
    Initialize composited image.
  */
  composite_image->tainted=True;
  p=composite_image->pixels;
  runlength=p->length+1;
  for (y=0; y < (int) composite_image->rows; y++)
  {
    if (((y_offset+y) < 0) || ((y_offset+y) >= (int) image->rows))
      {
        p+=composite_image->columns;
        continue;
      }
    q=image->pixels+(y_offset+y)*image->columns+x_offset;
    for (x=0; x < (int) composite_image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      if (((x_offset+x) < 0) || ((x_offset+x) >= (int) image->columns))
        {
          q++;
          continue;
        }
      switch (compose)
      {
        case AnnotateCompositeOp:
        case OverCompositeOp:
        default:
        {
          if (p->index == Transparent)
            {
              red=q->red;
              green=q->green;
              blue=q->blue;
              index=q->index;
            }
          else
            if (p->index == Opaque)
              {
                red=p->red;
                green=p->green;
                blue=p->blue;
                index=p->index;
              }
            else
              {
                red=(long) (p->red*p->index+q->red*(Opaque-p->index))/Opaque;
                green=(long)
                  (p->green*p->index+q->green*(Opaque-p->index))/Opaque;
                blue=(long) (p->blue*p->index+q->blue*(Opaque-p->index))/Opaque;
                index=(long)
                  (p->index*p->index+q->index*(Opaque-p->index))/Opaque;
              }
          break;
        }
        case InCompositeOp:
        {
          red=(long) (p->red*q->index)/Opaque;
          green=(long) (p->green*q->index)/Opaque;
          blue=(long) (p->blue*q->index)/Opaque;
          index=(long) (p->index*q->index)/Opaque;
          break;
        }
        case OutCompositeOp:
        {
          red=(long) (p->red*(Opaque-q->index))/Opaque;
          green=(long) (p->green*(Opaque-q->index))/Opaque;
          blue=(long) (p->blue*(Opaque-q->index))/Opaque;
          index=(long) (p->index*(Opaque-q->index))/Opaque;
          break;
        }
        case AtopCompositeOp:
        {
          red=(long) (p->red*q->index+q->red*(Opaque-p->index))/Opaque;
          green=(long) (p->green*q->index+q->green*(Opaque-p->index))/Opaque;
          blue=(long) (p->blue*q->index+q->blue*(Opaque-p->index))/Opaque;
          index=(long) (p->index*q->index+q->index*(Opaque-p->index))/Opaque;
          break;
        }
        case XorCompositeOp:
        {
          red=(long) (p->red*(Opaque-q->index)+q->red*(Opaque-p->index))/Opaque;
          green=(long)
            (p->green*(Opaque-q->index)+q->green*(Opaque-p->index))/Opaque;
          blue=(long)
            (p->blue*(Opaque-q->index)+q->blue*(Opaque-p->index))/Opaque;
          index=(long)
            (p->index*(Opaque-q->index)+q->index*(Opaque-p->index))/Opaque;
          break;
        }
        case PlusCompositeOp:
        {
          red=(long) p->red+(long) q->red;
          green=(long) p->green+(long) q->green;
          blue=(long) p->blue+(long) q->blue;
          index=(long) p->index+(long) q->index;
          break;
        }
        case MinusCompositeOp:
        {
          red=(long) p->red-(long) q->red;
          green=(long) p->green-(long) q->green;
          blue=(long) p->blue-(long) q->blue;
          index=Opaque;
          break;
        }
        case AddCompositeOp:
        {
          red=(long) p->red+(long) q->red;
          if (red > MaxRGB)
            red-=(MaxRGB+1);
          green=(long) p->green+(long) q->green;
          if (green > MaxRGB)
            green-=(MaxRGB+1);
          blue=(long) p->blue+(long) q->blue;
          if (blue > MaxRGB)
            blue-=(MaxRGB+1);
          index=(long) p->index+(long) q->index;
          if (index > Opaque)
            index-=(Opaque+1);
          break;
        }
        case SubtractCompositeOp:
        {
          red=(long) p->red-(long) q->red;
          if (red < 0)
            red+=(MaxRGB+1);
          green=(long) p->green-(long) q->green;
          if (green < 0)
            green+=(MaxRGB+1);
          blue=(long) p->blue-(long) q->blue;
          if (blue < 0)
            blue+=(MaxRGB+1);
          index=(long) p->index-(long) q->index;
          if (index < 0)
            index+=(MaxRGB+1);
          break;
        }
        case DifferenceCompositeOp:
        {
          red=AbsoluteValue((long) p->red-(long) q->red);
          green=AbsoluteValue((long) p->green-(long) q->green);
          blue=AbsoluteValue((long) p->blue-(long) q->blue);
          index=AbsoluteValue((long) p->index-(long) q->index);
          break;
        }
        case BumpmapCompositeOp:
        {
          shade=Intensity(*p);
          red=(long) (q->red*shade)/Opaque;
          green=(long) (q->green*shade)/Opaque;
          blue=(long) (q->blue*shade)/Opaque;
          index=(long) (q->index*shade)/Opaque;
          break;
        }
        case ReplaceCompositeOp:
        {
          red=p->red;
          green=p->green;
          blue=p->blue;
          index=p->index;
          break;
        }
        case ReplaceRedCompositeOp:
        {
          red=DownScale(Intensity(*p));
          green=q->green;
          blue=q->blue;
          index=q->index;
          break;
        }
        case ReplaceGreenCompositeOp:
        {
          red=q->red;
          green=DownScale(Intensity(*p));
          blue=q->blue;
          index=q->index;
          break;
        }
        case ReplaceBlueCompositeOp:
        {
          red=q->red;
          green=q->green;
          blue=DownScale(Intensity(*p));
          index=q->index;
          break;
        }
        case ReplaceMatteCompositeOp:
        {
          red=q->red;
          green=q->green;
          blue=q->blue;
          index=DownScale(Intensity(*p));
          break;
        }
        case BlendCompositeOp:
        {
          red=(long) (p->red*p->index+q->red*q->index)/Opaque;
          green=(long) (p->green*p->index+q->green*q->index)/Opaque;
          blue=(long) (p->blue*p->index+q->blue*q->index)/Opaque;
          index=Opaque;
          break;
        }
        case DisplaceCompositeOp:
        {
          red=p->red;
          green=p->green;
          blue=p->blue;
          index=p->index;
          break;
        }
      }
      q->red=(Quantum) ((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red);
      q->green=(Quantum) ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green);
      q->blue=(Quantum) ((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue);
      q->index=(unsigned short) ((index < Transparent) ? Transparent :
        (index > Opaque) ? Opaque : index);
      q->length=0;
      q++;
    }
  }
  if (compose == BlendCompositeOp)
    image->matte=False;
  if (compose == DisplaceCompositeOp)
    {
      image->matte=False;
      DestroyImage(composite_image);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n d e n s e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CondenseImage compresses an image to the minimum number of
%  runlength-encoded packets.
%
%  The format of the CondenseImage routine is:
%
%      CondenseImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
Export void CondenseImage(Image *image)
{
  register int
    i,
    runlength;

  register long
    packets;

  register RunlengthPacket
    *p,
    *q;

  assert(image != (Image *) NULL);
  if (image->pixels == (RunlengthPacket *) NULL)
    return;
  /*
    Compress image pixels.
  */
  p=image->pixels;
  runlength=p->length+1;
  packets=0;
  q=image->pixels;
  q->length=MaxRunlength;
  if (image->matte || (image->colorspace == CMYKColorspace))
    for (i=0; i < (int) (image->columns*image->rows); i++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      if ((p->red == q->red) && (p->green == q->green) &&
          (p->blue == q->blue) && (p->index == q->index) &&
          ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          *q=(*p);
          q->length=0;
        }
    }
  else
    for (i=0; i < (int) (image->columns*image->rows); i++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      if ((p->red == q->red) && (p->green == q->green) &&
          (p->blue == q->blue) && ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          *q=(*p);
          q->length=0;
        }
    }
  SetRunlengthPackets(image,packets);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o n t r a s t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ContrastImage enhances the intensity differences between the
%  lighter and darker elements of the image.
%
%  The format of the ContrastImage routine is:
%
%      ContrastImage(image,sharpen)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o sharpen: If True, the intensity is increased otherwise it is
%      decreased.
%
%
*/
Export void ContrastImage(Image *image,const unsigned int sharpen)
{
#define DullContrastImageText  "  Dulling image contrast...  "
#define SharpenContrastImageText  "  Sharpening image contrast...  "

  int
    sign;

  register int
    i;

  register RunlengthPacket
    *p;

  assert(image != (Image *) NULL);
  sign=sharpen ? 1 : -1;
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Contrast enhance DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        Contrast(sign,&p->red,&p->green,&p->blue);
        p++;
        if (QuantumTick(i,image->packets))
          {
            if (sharpen)
              ProgressMonitor(SharpenContrastImageText,i,image->packets);
            else
              ProgressMonitor(DullContrastImageText,i,image->packets);
          }
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Contrast enhance PseudoClass image.
      */
      for (i=0; i < (int) image->colors; i++)
        Contrast(sign,&image->colormap[i].red,&image->colormap[i].green,
          &image->colormap[i].blue);
      SyncImage(image);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C r e a t e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CreateImage creates an image from the specified normalized pixel data
%  and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the CreateImage routine is:
%
%      image=CreateImage(width,height,red,green,blue,opacity)
%
%  A description of each parameter follows:
%
%    o image:  Method CreateImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o width: Specifies the width in pixels of the image.
%
%    o height: Specifies the height in pixels of the image.
%
%    o red,green,blue,opacity: This array of normalized float values [0..1]
%      contain the red, green, blue, and opacity components of the pixel data.
%      The length of the arrays must equal the area specified by the width and
%      height values.  If you do not want to initialize a particular
%      component, specify it as NULL.
%
%
*/
Image *CreateImage(const unsigned int width,const unsigned int height,
  const float *red,const float *green,const float *blue,const float *opacity)
{
  Image
    *image;

  register int
    i;

  RunlengthPacket
    *q;

  if ((width*height) == 0)
    {
      MagickWarning(CorruptImageWarning,"impossible image size",(char *) NULL);
      return((Image *) NULL);
    }
  /*
    Allocate image structure.
  */
  image=AllocateImage((ImageInfo *) NULL);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Initialize image structure.
  */
  image->matte=opacity != (float *) NULL;
  image->columns=width;
  image->rows=height;
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return((Image *) NULL);
    }
  SetImage(image);
  /*
    Convert a rectangular array of float pixels to runlength-encoded.
  */
  q=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    if (red != (float *) NULL)
      q->red=(Quantum) (MaxRGB*red[i]);
    if (green != (float *) NULL)
      q->green=(Quantum) (MaxRGB*green[i]);
    if (blue != (float *) NULL)
      q->blue=(Quantum) (MaxRGB*blue[i]);
    if (opacity != (float *) NULL)
      q->index=(unsigned short) (Opaque*opacity[i]);
    q->length=0;
    q++;
  }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C r o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CropImage creates a new image that is a subregion of an existing
%  one.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.  This routine is optimized to preserve
%  the runlength encoding.  That is, the cropped image will always use less
%  memory than the original.
%
%  The format of the CropImage routine is:
%
%      cropped_image=CropImage(image,crop_info)
%
%  A description of each parameter follows:
%
%    o cropped_image: Method CropImage returns a pointer to the cropped
%      image.  A null image is returned if there is a memory shortage or
%      if the image width or height is zero.
%
%    o image: The address of a structure of type Image.
%
%    o crop_info: Specifies a pointer to a RectangleInfo which defines the
%      region of the image to crop.
%
%
*/
Export Image *CropImage(const Image *image,const RectangleInfo *crop_info)
{
#define CropImageText  "  Cropping image...  "

  Image
    *cropped_image;

  int
    y;

  RectangleInfo
    local_info;

  register int
    runlength,
    x;

  register long
    max_packets;

  register RunlengthPacket
    *p,
    *q;

  /*
    Check crop geometry.
  */
  assert(image != (Image *) NULL);
  assert(crop_info != (const RectangleInfo *) NULL);
  if (((crop_info->x+(int) crop_info->width) < 0) ||
      ((crop_info->y+(int) crop_info->height) < 0) ||
      (crop_info->x >= (int) image->columns) ||
      (crop_info->y >= (int) image->rows))
    {
      MagickWarning(OptionWarning,"Unable to crop image",
        "geometry does not contain image");
      return((Image *) NULL);
    }
  local_info=(*crop_info);
  if ((local_info.x+(int) local_info.width) > (int) image->columns)
    local_info.width=(unsigned int) ((int) image->columns-local_info.x);
  if ((local_info.y+(int) local_info.height) > (int) image->rows)
    local_info.height=(unsigned int) ((int) image->rows-local_info.y);
  if (local_info.x < 0)
    {
      local_info.width-=(unsigned int) (-local_info.x);
      local_info.x=0;
    }
  if (local_info.y < 0)
    {
      local_info.height-=(unsigned int) (-local_info.y);
      local_info.y=0;
    }
  if ((local_info.width == 0) && (local_info.height == 0))
    {
      int
        x_border,
        y_border;

      register int
        i;

      RunlengthPacket
        corners[4];

      /*
        Set bounding box to the image dimensions.
      */
      x_border=local_info.x;
      y_border=local_info.y;
      local_info.width=0;
      local_info.height=0;
      local_info.x=image->columns;
      local_info.y=image->rows;
      p=image->pixels;
      runlength=p->length+1;
      corners[0]=(*p);
      for (i=1; i <= (int) (image->rows*image->columns); i++)
      {
        if (runlength != 0)
          runlength--;
        else
          {
            p++;
            runlength=p->length;
          }
        if (i == (int) image->columns)
          corners[1]=(*p);
        if (i == (int) (image->rows*image->columns-image->columns+1))
          corners[2]=(*p);
        if (i == (int) (image->rows*image->columns))
          corners[3]=(*p);
      }
      p=image->pixels;
      runlength=p->length+1;
      for (y=0; y < (int) image->rows; y++)
      {
        for (x=0; x < (int) image->columns; x++)
        {
          if (runlength != 0)
            runlength--;
          else
            {
              p++;
              runlength=p->length;
            }
          if (!ColorMatch(*p,corners[0],image->fuzz))
            if (x < local_info.x)
              local_info.x=x;
          if (!ColorMatch(*p,corners[1],image->fuzz))
            if (x > (int) local_info.width)
              local_info.width=x;
          if (!ColorMatch(*p,corners[0],image->fuzz))
            if (y < local_info.y)
              local_info.y=y;
          if (!ColorMatch(*p,corners[2],image->fuzz))
            if (y > (int) local_info.height)
              local_info.height=y;
        }
      }
      if ((local_info.width != 0) || (local_info.height != 0))
        {
          local_info.width-=local_info.x-1;
          local_info.height-=local_info.y-1;
        }
      local_info.width+=x_border*2;
      local_info.height+=y_border*2;
      local_info.x-=x_border;
      if (local_info.x < 0)
        local_info.x=0;
      local_info.y-=y_border;
      if (local_info.y < 0)
        local_info.y=0;
      if ((((int) local_info.width+local_info.x) > (int) image->columns) ||
          (((int) local_info.height+local_info.y) > (int) image->rows))
        {
          MagickWarning(OptionWarning,"Unable to crop image",
            "geometry does not contain image");
          return((Image *) NULL);
        }
    }
  if ((local_info.width == 0) || (local_info.height == 0))
    {
      MagickWarning(OptionWarning,"Unable to crop image",
        "geometry dimensions are zero");
      return((Image *) NULL);
    }
  if ((local_info.width == image->columns) &&
      (local_info.height == image->rows) && (local_info.x == 0) &&
      (local_info.y == 0))
    return((Image *) NULL);
  /*
    Initialize cropped image attributes.
  */
  cropped_image=CloneImage(image,local_info.width,local_info.height,True);
  if (cropped_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to crop image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Skip pixels up to the cropped image.
  */
  p=image->pixels;
  runlength=p->length+1;
  for (x=0; x < (int) (local_info.y*image->columns+local_info.x); x++)
    if (runlength != 0)
      runlength--;
    else
      {
        p++;
        runlength=p->length;
      }
  /*
    Extract cropped image.
  */
  max_packets=0;
  q=cropped_image->pixels;
  SetRunlengthEncoder(q);
  for (y=0; y < (int) (cropped_image->rows-1); y++)
  {
    /*
      Transfer scanline.
    */
    for (x=0; x < (int) cropped_image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      if ((p->red == q->red) && (p->green == q->green) &&
          (p->blue == q->blue) && (p->index == q->index) &&
          ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (max_packets != 0)
            q++;
          max_packets++;
          *q=(*p);
          q->length=0;
        }
    }
    /*
      Skip to next scanline.
    */
    for (x=0; x < (int) (image->columns-cropped_image->columns); x++)
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
    if (QuantumTick(y,cropped_image->rows))
      ProgressMonitor(CropImageText,y,cropped_image->rows-1);
  }
  /*
    Transfer last scanline.
  */
  for (x=0; x < (int) cropped_image->columns; x++)
  {
    if (runlength != 0)
      runlength--;
    else
      {
        p++;
        runlength=p->length;
      }
    if ((p->red == q->red) && (p->green == q->green) &&
        (p->blue == q->blue) && (p->index == q->index) &&
        ((int) q->length < MaxRunlength))
      q->length++;
    else
      {
        if (max_packets != 0)
          q++;
        max_packets++;
        *q=(*p);
        q->length=0;
      }
  }
  cropped_image->packets=max_packets;
  cropped_image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
    cropped_image->pixels,cropped_image->packets*sizeof(RunlengthPacket));
  return(cropped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C y c l e C o l o r m a p I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CycleColormapImage cycles the image colormap by a specified
%  amount.
%
%  The format of the CycleColormapImage routine is:
%
%      CycleColormapImage(image,amount)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o amount:  An unsigned value that specifies the offset of the colormap.
%
%
*/
Export void CycleColormapImage(Image *image,const int amount)
{
#define CycleColormapImageText  "  Cycling image...  "

  int
    index;

  register int
    i;

  register RunlengthPacket
    *q;

  assert(image != (Image *) NULL);
  if (image->class == DirectClass)
    {
      QuantizeInfo
        quantize_info;

      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=MaxColormapSize;
      (void) QuantizeImage(&quantize_info,image);
    }
  q=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    index=((int) q->index+amount) % image->colors;
    if (index < 0)
      index+=image->colors;
    q->index=(unsigned short) index;
    q++;
  }
  SyncImage(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s c r i b e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DescribeImage describes an image by printing its attributes to
%  stdout.
%
%  The format of the DescribeImage routine is:
%
%      DescribeImage(image,file,verbose)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o file: send the image attributes to this file.
%
%    o verbose: an unsigned value other than zero prints detailed information
%      about the image.
%
%
*/
Export void DescribeImage(Image *image,FILE *file,const unsigned int verbose)
{
  char
    color[MaxTextExtent],
    **textlist;

  Image
    *p;

  MagickInfo
    *magick_info;

  register int
    i;

  unsigned int
    count;

  unsigned long
    number_colors;

  assert(image != (Image *) NULL);
  if (file == (FILE *) NULL)
    file=stdout;
  if (!verbose)
    {
      /*
        Display detailed info about the image.
      */
      if (*image->magick_filename != '\0')
        if (Latin1Compare(image->magick_filename,image->filename) != 0)
          (void) fprintf(file,"%.1024s=>",image->magick_filename);
       if ((image->previous == (Image *) NULL) &&
           (image->next == (Image *) NULL) && (image->scene == 0))
        (void) fprintf(file,"%.1024s ",image->filename);
      else
        (void) fprintf(file,"%.1024s[%u] ",image->filename,image->scene);
      if ((image->magick_columns != 0) || (image->magick_rows != 0))
        if ((image->magick_columns != image->columns) ||
            (image->magick_rows != image->rows))
          (void) fprintf(file,"%ux%u=>",image->magick_columns,
            image->magick_rows);
      if (image->page == (char *) NULL)
        (void) fprintf(file,"%ux%u ",image->columns,image->rows);
      else
        {
          int
            x,
            y;

          unsigned int
            sans;

          x=0;
          y=0;
          (void) XParseGeometry(image->page,&x,&y,&sans,&sans);
          (void) fprintf(file,"%ux%u%+d%+d ",image->columns,image->rows,x,y);
        }
      if (image->class == DirectClass)
        {
          (void) fprintf(file,"DirectClass ");
          if (image->total_colors != 0)
            (void) fprintf(file,"%luc ",image->total_colors);
        }
      else
        if (image->total_colors <= image->colors)
          (void) fprintf(file,"PseudoClass %uc ",image->colors);
        else
          {
            (void) fprintf(file,"PseudoClass %lu=>%uc ",image->total_colors,
              image->colors);
            (void) fprintf(file,"%u/%.6f/%.6fe ",image->mean_error_per_pixel,
              image->normalized_mean_error,image->normalized_maximum_error);
          }
      if (image->filesize != 0)
        {
          if (image->filesize >= (1 << 24))
            (void) fprintf(file,"%ldmb ",image->filesize/1024/1024);
          else
            if (image->filesize >= (1 << 14))
              (void) fprintf(file,"%ldkb ",image->filesize/1024);
            else
              (void) fprintf(file,"%ldb ",image->filesize);
        }
      (void) fprintf(file,"%.1024s %lds\n",image->magick,time((time_t *) NULL)-
        image->magick_time+1);
      return;
    }
  /*
    Display verbose info about the image.
  */
  number_colors=GetNumberColors(image,(FILE *) NULL);
  (void) fprintf(file,"Image: %.1024s\n",image->filename);
  magick_info=(MagickInfo *) GetMagickInfo(image->magick);
  if ((magick_info == (MagickInfo *) NULL) ||
      (*magick_info->description == '\0'))
    (void) fprintf(file,"  format: %.1024s\n",image->magick);
  else
    (void) fprintf(file,"  format: %.1024s (%s)\n",image->magick,
      magick_info->description);
  (void) fprintf(file,"  type: ");
  switch (GetImageType(image))
  {
    case BilevelType: (void) fprintf(file,"bilevel"); break;
    case GrayscaleType: (void) fprintf(file,"grayscale"); break;
    case PaletteType: (void) fprintf(file,"palette"); break;
    case TrueColorType: (void) fprintf(file,"true color"); break;
    case MatteType: (void) fprintf(file,"true color with transparency"); break;
    case ColorSeparationType: (void) fprintf(file,"color separated"); break;
    default: (void) fprintf(file,"undefined"); break;
  }
  (void) fprintf(file,"\n");
  if (image->class == DirectClass)
    (void) fprintf(file,"  class: DirectClass\n");
  else
    (void) fprintf(file,"  class: PseudoClass\n");
  if (!image->matte)
    (void) fprintf(file,"  matte: False\n");
  else
    if ((strcmp(image->magick,"GIF") != 0) || image->tainted)
      (void) fprintf(file,"  matte: True\n");
    else
      {
        RunlengthPacket
          *q;

        q=image->pixels;
        for (i=0; i < (int) (image->packets-1); i++)
        {
          if (q->index == Transparent)
            break;
          q++;
        }
        (void) fprintf(file,"  matte: (%3d,%3d,%3d) #%02x%02x%02x\n",
          q->red,q->green,q->blue,q->red,q->green,q->blue);
      }
  if (image->class == DirectClass)
    (void) fprintf(file,"  colors: %lu\n",number_colors);
  else
    if (number_colors <= image->colors)
      (void) fprintf(file,"  colors: %u\n",image->colors);
    else
      (void) fprintf(file,"  colors: %lu=>%u\n",number_colors,image->colors);
  if (image->class == DirectClass)
    {
      if (number_colors < 1024)
        (void) GetNumberColors(image,file);
    }
  else
    {
      char
        name[MaxTextExtent];

      register ColorPacket
        *p;

      /*
        Display image colormap.
      */
      p=image->colormap;
      for (i=0; i < (int) image->colors; i++)
      {
        (void) fprintf(file,"    %d: (%3d,%3d,%3d)",i,p->red,p->green,p->blue);
        (void) fprintf(file,"  ");
        (void) QueryColorName(p,name);
        (void) fprintf(file,"  %.1024s",name);
        (void) fprintf(file,"\n");
        p++;
      }
    }
  if (image->mean_error_per_pixel != 0)
    (void) fprintf(file,"  mean error per pixel: %d\n",
      image->mean_error_per_pixel);
  if (image->normalized_mean_error != 0)
    (void) fprintf(file,"  normalized mean error: %.6f\n",
      image->normalized_mean_error);
  if (image->normalized_maximum_error != 0)
    (void) fprintf(file,"  normalized maximum error: %.6f\n",
      image->normalized_maximum_error);
  if (image->rendering_intent == SaturationIntent)
    (void) fprintf(file,"  rendering-intent: saturation\n");
  else
    if (image->rendering_intent == PerceptualIntent)
      (void) fprintf(file,"  rendering-intent: perceptual\n");
    else
      if (image->rendering_intent == AbsoluteIntent)
        (void) fprintf(file,"  rendering-intent: absolute\n");
    else
      if (image->rendering_intent == RelativeIntent)
        (void) fprintf(file,"  rendering-intent: relative\n");
  if (image->gamma != 0.0)
    (void) fprintf(file,"  gamma: %g\n",image->gamma);
  if (image->chromaticity.white_point.x != 0.0)
    {
      /*
        Display image chromaticity.
      */
      (void) fprintf(file,"  chromaticity:\n");
      (void) fprintf(file,"    red primary: (%g,%g)\n",
        image->chromaticity.red_primary.x,image->chromaticity.red_primary.y);
      (void) fprintf(file,"    green primary: (%g,%g)\n",
        image->chromaticity.green_primary.x,
        image->chromaticity.green_primary.y);
      (void) fprintf(file,"    blue primary: (%g,%g)\n",
        image->chromaticity.blue_primary.x,image->chromaticity.blue_primary.y);
      (void) fprintf(file,"    white point: (%g,%g)\n",
        image->chromaticity.white_point.x,image->chromaticity.white_point.y);
    }
  if (image->color_profile.length > 0)
    (void) fprintf(file,"  color profile: %u bytes\n",
      image->color_profile.length);
  if (image->iptc_profile.length > 0)
    (void) fprintf(file,"  IPTC profile: %u bytes\n",
      image->iptc_profile.length);
  if ((image->magick_columns != 0) || (image->magick_rows != 0))
    if ((image->magick_columns != image->columns) ||
        (image->magick_rows != image->rows))
      (void) fprintf(file,"  base geometry: %ux%u\n",image->magick_columns,
        image->magick_rows);
  (void) fprintf(file,"  geometry: %ux%u\n",image->columns,image->rows);
  if ((image->tile_info.width*image->tile_info.height) != 0)
    (void) fprintf(file,"  tile geometry: %ux%u%+d%+d\n",image->tile_info.width,
      image->tile_info.height,image->tile_info.x,image->tile_info.y);
  if ((image->x_resolution != 0.0) && (image->y_resolution != 0.0))
    {
      /*
        Display image resolution.
      */
      (void) fprintf(file,"  resolution: %gx%g",image->x_resolution,
        image->y_resolution);
      if (image->units == UndefinedResolution)
        (void) fprintf(file," pixels\n");
      else
        if (image->units == PixelsPerInchResolution)
          (void) fprintf(file," pixels/inch\n");
        else
          if (image->units == PixelsPerCentimeterResolution)
            (void) fprintf(file," pixels/centimeter\n");
          else
            (void) fprintf(file,"\n");
    }
  (void) fprintf(file,"  depth: %u\n",image->depth);
  if (image->filesize != 0)
    {
      if (image->filesize >= (1 << 24))
        (void) fprintf(file,"  filesize: %ldmb\n",image->filesize/1024/1024);
      else
        if (image->filesize >= (1 << 14))
          (void) fprintf(file,"  filesize: %ldkb\n",image->filesize/1024);
        else
          (void) fprintf(file,"  filesize: %ldb\n",image->filesize);
    }
  if (image->interlace == NoInterlace)
    (void) fprintf(file,"  interlace: None\n");
  else
    if (image->interlace == LineInterlace)
      (void) fprintf(file,"  interlace: Line\n");
    else
      if (image->interlace == PlaneInterlace)
        (void) fprintf(file,"  interlace: Plane\n");
    else
      if (image->interlace == PartitionInterlace)
        (void) fprintf(file,"  interlace: Partition\n");
  (void) QueryColorName(&image->background_color,color);
  (void) fprintf(file,"  background-color: %.1024s\n",color);
  (void) QueryColorName(&image->border_color,color);
  (void) fprintf(file,"  border-color: %.1024s\n",color);
  (void) QueryColorName(&image->matte_color,color);
  (void) fprintf(file,"  matte-color: %.1024s\n",color);
  if (image->page != (char *) NULL)
    (void) fprintf(file,"  page geometry: %.1024s\n",image->page);
  if (image->dispose != 0)
    (void) fprintf(file,"  dispose method: %d\n",image->dispose);
  if (image->delay != 0)
    (void) fprintf(file,"  delay: %d\n",image->delay);
  if (image->iterations != 1)
    (void) fprintf(file,"  iterations: %d\n",image->iterations);
  p=image;
  while (p->previous != (Image *) NULL)
    p=p->previous;
  for (count=1; p->next != (Image *) NULL; count++)
    p=p->next;
  if (count > 1)
    (void) fprintf(file,"  scene: %u of %u\n",image->scene,count);
  else
    if (image->scene != 0)
      (void) fprintf(file,"  scene: %u\n",image->scene);
  if (image->label != (char *) NULL)
    (void) fprintf(file,"  label: %.1024s\n",image->label);
  (void) fprintf(file,"  compression: ");
  switch (image->compression)
  {
    case NoCompression: (void) fprintf(file,"None\n"); break;
    case BZipCompression: (void) fprintf(file,"BZip\n"); break;
    case FaxCompression: (void) fprintf(file,"Fax\n"); break;
    case Group4Compression: (void) fprintf(file,"Group 4\n"); break;
    case JPEGCompression: (void) fprintf(file,"JPEG\n"); break;
    case LZWCompression: (void) fprintf(file,"LZW\n"); break;
    case RunlengthEncodedCompression:
      (void) fprintf(file,"Runlength Encoded\n"); break;
    case ZipCompression: (void) fprintf(file,"Zip\n"); break;
    default: break;
  }
  if (image->packets < (image->columns*image->rows))
    (void) fprintf(file,"  runlength packets: %lu of %u\n",image->packets,
      image->columns*image->rows);
  if (image->tainted)
    (void) fprintf(file,"  tainted: True\n");
  else
    (void) fprintf(file,"  tainted: False\n");
  SignatureImage(image);
  (void) fprintf(file,"  signature: %.1024s\n",image->signature);
  if (image->comments != (char *) NULL)
    {
      /*
        Display image comment.
      */
      (void) fprintf(file,"  comments:\n");
      textlist=StringToList(image->comments);
      if (textlist != (char **) NULL)
        {
          for (i=0; textlist[i] != (char *) NULL; i++)
          {
            (void) fprintf(file,"  %.1024s\n",textlist[i]);
            FreeMemory(textlist[i]);
          }
          FreeMemory((char *) textlist);
        }
    }
  if (image->montage != (char *) NULL)
    (void) fprintf(file,"  montage: %.1024s\n",image->montage);
  if (image->directory != (char *) NULL)
    {
      ErrorHandler
        handler;

      Image
        *tile;

      ImageInfo
        image_info;

      register char
        *p,
        *q;

      /*
        Display visual image directory.
      */
      GetImageInfo(&image_info);
      (void) CloneString(&image_info.size,"64x64");
      (void) fprintf(file,"  directory:\n");
      for (p=image->directory; *p != '\0'; p++)
      {
        q=p;
        while ((*q != '\n') && (*q != '\0'))
          q++;
        (void) strncpy(image_info.filename,p,q-p);
        image_info.filename[q-p]='\0';
        p=q;
        (void) fprintf(file,"    %.1024s",image_info.filename);
        handler=SetWarningHandler((ErrorHandler) NULL);
        tile=ReadImage(&image_info);
        (void) SetWarningHandler(handler);
        if (tile == (Image *) NULL)
          {
            (void) fprintf(file,"\n");
            continue;
          }
        (void) fprintf(file," %ux%u %.1024s\n",tile->magick_columns,
          tile->magick_rows,tile->magick);
        if (tile->comments != (char *) NULL)
          {
            /*
              Display tile comment.
            */
            textlist=StringToList(tile->comments);
            if (textlist != (char **) NULL)
              {
                for (i=0; textlist[i] != (char *) NULL; i++)
                {
                  (void) fprintf(file,"    %.1024s\n",textlist[i]);
                  FreeMemory(textlist[i]);
                }
                FreeMemory((char *) textlist);
              }
          }
        DestroyImage(tile);
      }
    }
  (void) fflush(file);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y A n n o t a t e I n f o                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyAnnotateInfo deallocates memory associated with an
%  AnnotateInfo structure.
%
%  The format of the DestroyAnnotateInfo routine is:
%
%      DestroyAnnotateInfo(annotate_info)
%
%  A description of each parameter follows:
%
%    o annotate_info: Specifies a pointer to an AnnotateInfo structure.
%
%
*/
Export void DestroyAnnotateInfo(AnnotateInfo *annotate_info)
{
  assert(annotate_info != (AnnotateInfo *) NULL);
  DestroyImageInfo(annotate_info->image_info);
  if (annotate_info->geometry != (char *) NULL)
    FreeMemory((char *) annotate_info->geometry);
  annotate_info->geometry=(char *) NULL;
  if (annotate_info->text != (char *) NULL)
    FreeMemory((char *) annotate_info->text);
  annotate_info->text=(char *) NULL;
  if (annotate_info->primitive != (char *) NULL)
    FreeMemory((char *) annotate_info->primitive);
  annotate_info->primitive=(char *) NULL;
  if (annotate_info->font_name != (char *) NULL)
    FreeMemory((char *) annotate_info->font_name);
  annotate_info->font_name=(char *) NULL;
  if (annotate_info->tile != (Image *) NULL)
    DestroyImage(annotate_info->tile);
  annotate_info->tile=(Image *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyImage deallocates memory associated with an image.
%
%  The format of the DestroyImage routine is:
%
%      DestroyImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
Export void DestroyImage(Image *image)
{
  /*
    Close image.
  */
  assert(image != (Image *) NULL);
  if (image->file != (FILE *) NULL)
    {
      CloseImage(image);
      if (image->temporary)
        (void) remove(image->filename);
    }
  /*
    Deallocate the image comments.
  */
  if (image->comments != (char *) NULL)
    FreeMemory((char *) image->comments);
  /*
    Deallocate the image label.
  */
  if (image->label != (char *) NULL)
    FreeMemory((char *) image->label);
  /*
    Deallocate the image montage directory.
  */
  if (image->montage != (char *) NULL)
    FreeMemory((char *) image->montage);
  if (image->directory != (char *) NULL)
    FreeMemory((char *) image->directory);
  /*
    Deallocate the image colormap.
  */
  if (image->colormap != (ColorPacket *) NULL)
    FreeMemory((char *) image->colormap);
  /*
    Deallocate the image ICC profile.
  */
  if (image->color_profile.length > 0)
    FreeMemory((char *) image->color_profile.info);
  /*
    Deallocate the image IPTC profile.
  */
  if (image->iptc_profile.length > 0)
    FreeMemory((char *) image->iptc_profile.info);
  /*
    Deallocate the image signature.
  */
  if (image->signature != (char *) NULL)
    FreeMemory((char *) image->signature);
  /*
    Deallocate the image pixels.
  */
  if (image->pixels != (RunlengthPacket *) NULL)
    FreeMemory((char *) image->pixels);
  if (image->packed_pixels != (unsigned char *) NULL)
    FreeMemory((char *) image->packed_pixels);
  /*
    Deallocate the image page geometry.
  */
  if (image->page != (char *) NULL)
    FreeMemory((char *) image->page);
  if (!image->orphan)
    {
      /*
        Unlink from linked list.
      */
      if (image->previous != (Image *) NULL)
        {
          if (image->next != (Image *) NULL)
            image->previous->next=image->next;
          else
            image->previous->next=(Image *) NULL;
        }
      if (image->next != (Image *) NULL)
        {
          if (image->previous != (Image *) NULL)
            image->next->previous=image->previous;
          else
            image->next->previous=(Image *) NULL;
        }
    }
  /*
    Deallocate the image structure.
  */
  FreeMemory((char *) image);
  image=(Image *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyImageInfo deallocates memory associated with an ImageInfo
%  structure.
%
%  The format of the DestroyImageInfo routine is:
%
%      DestroyImageInfo(image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Export void DestroyImageInfo(ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  if (image_info->server_name != (char *) NULL)
    FreeMemory((char *) image_info->server_name);
  image_info->server_name=(char *) NULL;
  if (image_info->size != (char *) NULL)
    FreeMemory((char *) image_info->size);
  image_info->size=(char *) NULL;
  if (image_info->tile != (char *) NULL)
    FreeMemory((char *) image_info->tile);
  image_info->tile=(char *) NULL;
  if (image_info->density != (char *) NULL)
    FreeMemory((char *) image_info->density);
  image_info->density=(char *) NULL;
  if (image_info->page != (char *) NULL)
    FreeMemory((char *) image_info->page);
  image_info->page=(char *) NULL;
  if (image_info->dispose != (char *) NULL)
    FreeMemory((char *) image_info->dispose);
  image_info->dispose=(char *) NULL;
  if (image_info->delay != (char *) NULL)
    FreeMemory((char *) image_info->delay);
  image_info->delay=(char *) NULL;
  if (image_info->iterations != (char *) NULL)
    FreeMemory((char *) image_info->iterations);
  image_info->iterations=(char *) NULL;
  if (image_info->texture != (char *) NULL)
    FreeMemory((char *) image_info->texture);
  image_info->texture=(char *) NULL;
  if (image_info->box != (char *) NULL)
    FreeMemory((char *) image_info->box);
  image_info->box=(char *) NULL;
  if (image_info->font != (char *) NULL)
    FreeMemory((char *) image_info->font);
  image_info->font=(char *) NULL;
  if (image_info->pen != (char *) NULL)
    FreeMemory((char *) image_info->pen);
  image_info->pen=(char *) NULL;
  if (image_info->view != (char *) NULL)
    FreeMemory((char *) image_info->view);
  image_info->view=(char *) NULL;
  if (image_info->background_color != (char *) NULL)
    FreeMemory((char *) image_info->background_color);
  image_info->background_color=(char *) NULL;
  if (image_info->border_color != (char *) NULL)
    FreeMemory((char *) image_info->border_color);
  image_info->border_color=(char *) NULL;
  if (image_info->matte_color != (char *) NULL)
    FreeMemory((char *) image_info->matte_color);
  image_info->matte_color=(char *) NULL;
  FreeMemory((ImageInfo *) image_info);
  image_info=(ImageInfo *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyImages deallocates memory associated with a linked list
%  of images.
%
%  The format of the DestroyImages routine is:
%
%      DestroyImages(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
Export void DestroyImages(Image *image)
{
  Image
    *next_image;

  /*
    Proceed to the top of the image list.
  */
  assert(image != (Image *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  do
  {
    /*
      Destroy this image.
    */
    next_image=image->next;
    if (next_image != (Image *)NULL)
      next_image->previous=(Image *)NULL;
    DestroyImage(image);
    image=next_image;
  } while (image != (Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M o n t a g e I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyMontageInfo deallocates memory associated with an MontageInfo
%  structure.
%
%  The format of the DestroyMontageInfo routine is:
%
%      DestroyMontageInfo(montage_info)
%
%  A description of each parameter follows:
%
%    o montage_info: Specifies a pointer to an MontageInfo structure.
%
%
*/
Export void DestroyMontageInfo(MontageInfo *montage_info)
{
  assert(montage_info != (MontageInfo *) NULL);
  if (montage_info->geometry != (char *) NULL)
    FreeMemory((char *) montage_info->geometry);
  montage_info->geometry=(char *) NULL;
  if (montage_info->tile != (char *) NULL)
    FreeMemory((char *) montage_info->tile);
  montage_info->tile=(char *) NULL;
  if (montage_info->background_color != (char *) NULL)
    FreeMemory((char *) montage_info->background_color);
  montage_info->background_color=(char *) NULL;
  if (montage_info->border_color != (char *) NULL)
    FreeMemory((char *) montage_info->border_color);
  montage_info->border_color=(char *) NULL;
  if (montage_info->matte_color != (char *) NULL)
    FreeMemory((char *) montage_info->matte_color);
  montage_info->matte_color=(char *) NULL;
  if (montage_info->title != (char *) NULL)
    FreeMemory((char *) montage_info->title);
  montage_info->title=(char *) NULL;
  if (montage_info->frame != (char *) NULL)
    FreeMemory((char *) montage_info->frame);
  montage_info->frame=(char *) NULL;
  if (montage_info->texture != (char *) NULL)
    FreeMemory((char *) montage_info->texture);
  montage_info->texture=(char *) NULL;
  if (montage_info->pen != (char *) NULL)
    FreeMemory((char *) montage_info->pen);
  montage_info->pen=(char *) NULL;
  if (montage_info->font != (char *) NULL)
    FreeMemory((char *) montage_info->font);
  montage_info->font=(char *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D i s p l a y I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DisplayImages displays one or more images to an X window.
%
%  The format of the AllocateNextImage routine is:
%
%      status=DisplayImages(image_info,image)
%
%  A description of each parameter follows:
%
%    o status: Method DisplayImages returns True if the images are displayed
%      in an X window, otherwise False is returned.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int DisplayImages(const ImageInfo *image_info,Image *image)
{
  char
    *client_name;

  Display
    *display;

  Image
    *next;

  unsigned long
    state;

  XrmDatabase
    resource_database;

  XResourceInfo
    resource;

  display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    return(False);
  XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource);
  *resource.image_info=(*image_info);
  resource.immutable=True;
  if (image_info->delay != (char *) NULL)
    resource.delay=atoi(image_info->delay);
  for (next=image; next; next=next->next)
  {
    state=DefaultState;
    (void) XDisplayImage(display,&resource,&client_name,1,&next,&state);
    if (state & ExitState)
      break;
  }
  XCloseDisplay(display);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawImage draws a primitive (line, rectangle, ellipse) on the
%  image.
%
%  The format of the DrawImage routine is:
%
%      DrawImage(image,annotate_info)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o annotate_info: The address of a DrawInfo structure.
%
%
*/
Export void DrawImage(Image *image,const AnnotateInfo *annotate_info)
{
#define DrawImageText  "  Drawing on image...  "

  AnnotateInfo
    *local_info;

  char
    keyword[MaxTextExtent],
    *primitive;

  double
    mid;

  int
    j,
    n,
    y;

  PointInfo
    point,
    target;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  RunlengthPacket
    pixel;

  SegmentInfo
    bounds;

  register char
    *p;

  register int
    i,
    x;

  register RunlengthPacket
    *q;

  unsigned int
    indirection,
    length,
    number_coordinates;

  unsigned short
    opacity;

  /*
    Ensure the annotation info is valid.
  */
  assert(image != (Image *) NULL);
  assert(annotate_info != (AnnotateInfo *) NULL);
  assert(annotate_info->primitive != (char *) NULL);
  assert(annotate_info->tile != (Image *) NULL);
  if (*annotate_info->primitive == '\0')
    return;
  if (!UncondenseImage(image))
    return;
  local_info=CloneAnnotateInfo(annotate_info->image_info,annotate_info);
  if (!UncondenseImage(local_info->tile))
    {
      DestroyAnnotateInfo(local_info);
      return;
    }
  primitive=local_info->primitive;
  indirection=(*primitive == '@');
  if (indirection)
    {
      FILE
        *file;

      int
        c;

      register char
        *q;

      /*
        Read text from a file.
      */
      file=(FILE *) fopen(primitive+1,"r");
      if (file == (FILE *) NULL)
        {
          MagickWarning(FileOpenWarning,"Unable to read primitive file",
            primitive+1);
          DestroyAnnotateInfo(local_info);
          return;
        }
      length=MaxTextExtent;
      primitive=(char *) AllocateMemory(length);
      q=primitive;
      while (primitive != (char *) NULL)
      {
        c=fgetc(file);
        if (c == '#')
          {
            /*
              Eat comments.
            */
            for (c=fgetc(file); c != EOF; c=fgetc(file))
              if ((c == '\r') || (c == '\n'))
                break;
            continue;
          }
        if (c == EOF)
          break;
        if ((q-primitive+1) >= (int) length)
          {
            *q='\0';
            length<<=1;
            primitive=(char *) ReallocateMemory(primitive,length);
            if (primitive == (char *) NULL)
              break;
            q=primitive+Extent(primitive);
          }
        *q++=(unsigned char) c;
      }
      (void) fclose(file);
      if (primitive == (char *) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Unable to draw image",
            "Memory allocation failed");
          DestroyAnnotateInfo(local_info);
          return;
        }
      *q='\0';
    }
  /*
    Allocate primitive info memory.
  */
  number_coordinates=2048;
  primitive_info=(PrimitiveInfo *)
    AllocateMemory(number_coordinates*sizeof(PrimitiveInfo));
  local_info->geometry=(char *) AllocateMemory(MaxTextExtent*sizeof(char));
  local_info->text=(char *) AllocateMemory(Extent(primitive)*sizeof(char));
  if ((primitive_info == (PrimitiveInfo *) NULL) ||
      (local_info->geometry == (char *) NULL) ||
      (local_info->text == (char *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Unable to draw image",
        "Memory allocation failed");
      if (indirection)
        FreeMemory((char *) primitive);
      DestroyAnnotateInfo(local_info);
      return;
    }
  /*
    Parse the primitive attributes.
  */
  primitive_type=UndefinedPrimitive;
  p=primitive;
  bounds.x1=image->columns-1;
  bounds.y1=image->rows-1;
  bounds.x2=0;
  bounds.y2=0;
  for (i=0; *p != '\0'; )
  {
    /*
      Define primitive.
    */
    while (isspace((int) (*p)))
      p++;
    for (x=0; isalpha((int) (*p)); x++)
      keyword[x]=(*p++);
    keyword[x]='\0';
    if (*keyword == '\0')
      break;
    primitive_type=UndefinedPrimitive;
    if (Latin1Compare("Point",keyword) == 0)
      primitive_type=PointPrimitive;
    if (Latin1Compare("Line",keyword) == 0)
      primitive_type=LinePrimitive;
    if (Latin1Compare("Rectangle",keyword) == 0)
      primitive_type=RectanglePrimitive;
    if (Latin1Compare("FillRectangle",keyword) == 0)
      primitive_type=FillRectanglePrimitive;
    if (Latin1Compare("Circle",keyword) == 0)
      primitive_type=CirclePrimitive;
    if (Latin1Compare("FillCircle",keyword) == 0)
      primitive_type=FillCirclePrimitive;
    if (Latin1Compare("Ellipse",keyword) == 0)
      primitive_type=EllipsePrimitive;
    if (Latin1Compare("FillEllipse",keyword) == 0)
      primitive_type=FillEllipsePrimitive;
    if (Latin1Compare("Polygon",keyword) == 0)
      primitive_type=PolygonPrimitive;
    if (Latin1Compare("FillPolygon",keyword) == 0)
      primitive_type=FillPolygonPrimitive;
    if (Latin1Compare("Color",keyword) == 0)
      primitive_type=ColorPrimitive;
    if (Latin1Compare("Matte",keyword) == 0)
      primitive_type=MattePrimitive;
    if (Latin1Compare("Text",keyword) == 0)
      primitive_type=TextPrimitive;
    if (Latin1Compare("Image",keyword) == 0)
      primitive_type=ImagePrimitive;
    if (primitive_type == UndefinedPrimitive)
      break;
    j=i;
    for (x=0; *p != '\0'; x++)
    {
      /*
        Define points.
      */
      while (isspace((int) (*p)))
        p++;
      if (!IsGeometry(p))
        break;
      point.x=0;
      point.y=0;
      n=0;
      (void) sscanf(p,"%f%f%n",&point.x,&point.y,&n);
      if (n == 0)
        (void) sscanf(p,"%f,%f%n",&point.x,&point.y,&n);
      if (n == 0)
        (void) sscanf(p,"%f, %f%n",&point.x,&point.y,&n);
      if (n == 0)
        (void) sscanf(p,"%f %f%n",&point.x,&point.y,&n);
      if (n == 0)
        {
          MagickWarning(OptionWarning,
            "Non-conforming drawing primitive definition",p);
          break;
        }
      if (point.x < bounds.x1)
        bounds.x1=point.x;
      if (point.y < bounds.y1)
        bounds.y1=point.y;
      if (point.x > bounds.x2)
        bounds.x2=point.x;
      if (point.y > bounds.y2)
        bounds.y2=point.y;
      primitive_info[i].primitive=primitive_type;
      primitive_info[i].coordinates=0;
      primitive_info[i].x=point.x;
      primitive_info[i].y=point.y;
      primitive_info[i].method=FloodfillMethod;
      p+=n;
      while (isspace((int) (*p)) || (*p == ','))
        p++;
      i++;
      if (i < (int) (number_coordinates-360-1))
        continue;
      number_coordinates<<=1;
      primitive_info=(PrimitiveInfo *) ReallocateMemory(primitive_info,
        number_coordinates*sizeof(PrimitiveInfo));
      if (primitive_info != (PrimitiveInfo *) NULL)
        continue;
      MagickWarning(ResourceLimitWarning,"Unable to draw image",
        "Memory allocation failed");
      if (indirection)
        FreeMemory((char *) primitive);
      DestroyAnnotateInfo(local_info);
      return;
    }
    primitive_info[j].coordinates=x;
    primitive_info[j].method=FloodfillMethod;
    primitive_info[j].text=(char *) NULL;
    switch (primitive_type)
    {
      case PointPrimitive:
      default:
      {
        if (primitive_info[j].coordinates != 1)
          primitive_type=UndefinedPrimitive;
        break;
      }
      case LinePrimitive:
      case RectanglePrimitive:
      case FillRectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          primitive_type=UndefinedPrimitive;
        break;
      }
      case CirclePrimitive:
      case FillCirclePrimitive:
      {
        double
          radius;

        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        /*
          Determine circle bounding box.
        */
        x=(int) (primitive_info[j+1].x-primitive_info[j].x);
        y=(int) (primitive_info[j+1].y-primitive_info[j].y);
        radius=
          sqrt((double) (x*x+y*y))+local_info->image_info->linewidth/2.0+0.5;
        point.x=Max(primitive_info[j].x-radius,0);
        point.y=Max(primitive_info[j].y-radius,0);
        if (point.x < bounds.x1)
          bounds.x1=point.x;
        if (point.y < bounds.y1)
          bounds.y1=point.y;
        point.x=Min(primitive_info[j].x+radius,image->columns-1);
        point.y=Min(primitive_info[j].y+radius,image->rows-1);
        if (point.x > bounds.x2)
          bounds.x2=point.x;
        if (point.y > bounds.y2)
          bounds.y2=point.y;
        break;
      }
      case EllipsePrimitive:
      case FillEllipsePrimitive:
      {
        double
          n;

        PointInfo
          degrees,
          end,
          start;

        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        /*
          Arc's are just short segmented polygons.
        */
        primitive_info[j].primitive=PolygonPrimitive;
        if (primitive_type == FillEllipsePrimitive)
          primitive_info[j].primitive=FillPolygonPrimitive;
        start.x=primitive_info[j].x;
        start.y=primitive_info[j].y;
        end.x=primitive_info[j+1].x/2;
        end.y=primitive_info[j+1].y/2;
        degrees.x=primitive_info[j+2].x;
        degrees.y=primitive_info[j+2].y;
        while (degrees.y < degrees.x)
          degrees.y+=360;
        i=j;
        if ((primitive_type == FillEllipsePrimitive) &&
            (fmod(degrees.y-degrees.x,360.0) != 0.0))
          {
            if (start.x < bounds.x1)
              bounds.x1=start.x;
            if (start.y < bounds.y1)
              bounds.y1=start.y;
            if (end.x > bounds.x2)
              bounds.x2=end.x;
            if (end.y > bounds.y2)
              bounds.y2=end.y;
            primitive_info[j].coordinates++;
            i++;
          }
        for (n=(degrees.x+1.0); n <= degrees.y; n+=1.0)
        {
          point.x=cos(DegreesToRadians(fmod(n,360.0)))*end.x+start.x;
          point.y=sin(DegreesToRadians(fmod(n,360.0)))*end.y+start.y;
          if (point.x < bounds.x1)
            bounds.x1=point.x;
          if (point.y < bounds.y1)
            bounds.y1=point.y;
          if (point.x > bounds.x2)
            bounds.x2=point.x;
          if (point.y > bounds.y2)
            bounds.y2=point.y;
          primitive_info[i].coordinates=0;
          primitive_info[i].x=point.x;
          primitive_info[i].y=point.y;
          primitive_info[j].coordinates++;
          i++;
        }
        break;
      }
      case PolygonPrimitive:
      case FillPolygonPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          primitive_type=UndefinedPrimitive;
        break;
      }
      case ColorPrimitive:
      case MattePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        /*
          Define method.
        */
        while (isspace((int) (*p)))
          p++;
        for (x=0; isalpha((int) (*p)); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        if (*keyword == '\0')
          break;
        if (Latin1Compare("point",keyword) == 0)
          primitive_info[j].method=PointMethod;
        else
          if (Latin1Compare("replace",keyword) == 0)
            primitive_info[j].method=ReplaceMethod;
          else
            if (Latin1Compare("floodfill",keyword) == 0)
              primitive_info[j].method=FloodfillMethod;
            else
              if (Latin1Compare("filltoborder",keyword) == 0)
                primitive_info[j].method=FillToBorderMethod;
              else
                if (Latin1Compare("reset",keyword) == 0)
                  primitive_info[j].method=ResetMethod;
                else
                  primitive_type=UndefinedPrimitive;
        while (isspace((int) (*p)))
          p++;
        break;
      }
      case TextPrimitive:
      case ImagePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        if (*p != '\0')
          {
            primitive_info[j].text=p;
            if (*p == '"')
              {
                for (p++; *p != '\0'; p++)
                  if ((*p == '"') && (*(p-1) != '\\'))
                    break;
              }
            else
              if (*p == '\'')
                {
                  for (p++; *p != '\0'; p++)
                    if ((*p == '\'') && (*(p-1) != '\\'))
                      break;
                }
              else
                for (p++;  *p != '\0'; p++)
                  if (isspace((int) *p) && (*(p-1) != '\\'))
                    break;
            if (*p != '\0')
              p++;
          }
        break;
      }
    }
    if (primitive_type == UndefinedPrimitive)
      break;
  }
  primitive_info[i].primitive=UndefinedPrimitive;
  if (primitive_type == UndefinedPrimitive)
    {
      MagickWarning(OptionWarning,
        "Non-conforming drawing primitive definition",keyword);
      FreeMemory((char *) primitive_info);
      if (indirection)
        FreeMemory((char *) primitive);
      DestroyAnnotateInfo(local_info);
      return;
    }
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
    if ((primitive_info[i].method == ReplaceMethod) ||
        (primitive_info[i].method == ResetMethod))
      {
        /*
          Replace and reset methods affect the entire image.
        */
        bounds.x1=0;
        bounds.y1=0;
        bounds.x2=image->columns-1;
        bounds.y2=image->rows-1;
      }
  /*
    Account for linewidth.
  */
  mid=local_info->image_info->linewidth/2.0;
  if ((bounds.x1 != bounds.x2) || (bounds.y1 != bounds.y2))
    {
      bounds.x1=Max(bounds.x1-mid,0);
      bounds.y1=Max(bounds.y1-mid,0);
      bounds.x2=Min(bounds.x2+ceil(mid),image->columns-1);
      bounds.y2=Min(bounds.y2+ceil(mid),image->rows-1);
    }
  /*
    Draw the primitive on the image.
  */
  image->class=DirectClass;
  for (y=(int) bounds.y1; y <= (int) bounds.y2; y++)
  {
    target.y=y;
    q=PixelOffset(image,bounds.x1,y);
    for (x=(int) bounds.x1; x <= (int) bounds.x2; x++)
    {
      target.x=x;
      opacity=InsidePrimitive(primitive_info,local_info,&target,image);
      if (!local_info->image_info->antialias)
        if (opacity != Transparent)
          opacity=Opaque;
      if (opacity != Transparent)
        {
          pixel=(*PixelOffset(local_info->tile,x % local_info->tile->columns,
            y % local_info->tile->rows));
          q->red=(Quantum) ((long) (pixel.red*opacity+q->red*
            (Opaque-opacity))/Opaque);
          q->green=(Quantum) ((long) (pixel.green*opacity+q->green*
            (Opaque-opacity))/Opaque);
          q->blue=(Quantum) ((long) (pixel.blue*opacity+q->blue*
            (Opaque-opacity))/Opaque);
          if (local_info->tile->matte)
            {
              q->index=(unsigned short) ((long) (pixel.index*opacity+q->index*
                (Opaque-opacity))/Opaque);
              q->red=(Quantum) ((long) (pixel.red*pixel.index+q->red*
                (Opaque-pixel.index))/Opaque);
              q->green=(Quantum) ((long) (pixel.green*pixel.index+q->green*
                (Opaque-pixel.index))/Opaque);
              q->blue=(Quantum) ((long) (pixel.blue*pixel.index+q->blue*
                (Opaque-pixel.index))/Opaque);
              q->index=(unsigned short) ((long) (pixel.index*pixel.index+
                q->index*(Opaque-pixel.index))/Opaque);
            }
        }
      q++;
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(DrawImageText,y,image->rows);
  }
  /*
    Free resources.
  */
  FreeMemory((char *) primitive_info);
  if (indirection)
    FreeMemory((char *) primitive);
  DestroyAnnotateInfo(local_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E q u a l i z e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method EqualizeImage performs histogram equalization on the reference
%  image.
%
%  The format of the EqualizeImage routine is:
%
%      EqualizeImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export void EqualizeImage(Image *image)
{
#define EqualizeImageText  "  Equalizing image...  "

  Quantum
    *equalize_map;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  unsigned int
    high,
    *histogram,
    low,
    *map;

  /*
    Allocate and initialize histogram arrays.
  */
  assert(image != (Image *) NULL);
  histogram=(unsigned int *) AllocateMemory((MaxRGB+1)*sizeof(unsigned int));
  map=(unsigned int *) AllocateMemory((MaxRGB+1)*sizeof(unsigned int));
  equalize_map=(Quantum *) AllocateMemory((MaxRGB+1)*sizeof(Quantum));
  if ((histogram == (unsigned int *) NULL) || (map == (unsigned int *) NULL) ||
      (equalize_map == (Quantum *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Unable to equalize image",
        "Memory allocation failed");
      return;
    }
  /*
    Form histogram.
  */
  for (i=0; i <= MaxRGB; i++)
    histogram[i]=0;
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    histogram[Intensity(*p)]+=(p->length+1);
    p++;
  }
  /*
    Integrate the histogram to get the equalization map.
  */
  j=0;
  for (i=0; i <= MaxRGB; i++)
  {
    j+=histogram[i];
    map[i]=j;
  }
  FreeMemory((char *) histogram);
  if (map[MaxRGB] == 0)
    {
      FreeMemory((char *) equalize_map);
      FreeMemory((char *) map);
      return;
    }
  /*
    Equalize.
  */
  low=map[0];
  high=map[MaxRGB];
  for (i=0; i <= MaxRGB; i++)
    equalize_map[i]=(Quantum)
      ((((double) (map[i]-low))*MaxRGB)/Max(high-low,1));
  FreeMemory((char *) map);
  /*
    Stretch the histogram.
  */
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Equalize DirectClass packets.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        p->red=equalize_map[p->red];
        p->green=equalize_map[p->green];
        p->blue=equalize_map[p->blue];
        p++;
        if (QuantumTick(i,image->packets))
          ProgressMonitor(EqualizeImageText,i,image->packets);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Equalize PseudoClass packets.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        image->colormap[i].red=equalize_map[image->colormap[i].red];
        image->colormap[i].green=equalize_map[image->colormap[i].green];
        image->colormap[i].blue=equalize_map[image->colormap[i].blue];
      }
      SyncImage(image);
      break;
    }
  }
  FreeMemory((char *) equalize_map);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l i p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FlipImage creates a new image that reflects each scanline in the
%  vertical direction It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the FlipImage routine is:
%
%      flipped_image=FlipImage(image)
%
%  A description of each parameter follows:
%
%    o flipped_image: Method FlipImage returns a pointer to the image
%      after reflecting.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export Image *FlipImage(const Image *image)
{
#define FlipImageText  "  Flipping image...  "

  Image
    *flipped_image;

  int
    y;

  register int
    runlength,
    x;

  register RunlengthPacket
    *p,
    *q,
    *s;

  RunlengthPacket
    *scanline;

  /*
    Initialize flipped image attributes.
  */
  assert(image != (Image *) NULL);
  flipped_image=CloneImage(image,image->columns,image->rows,False);
  if (flipped_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to flip image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scan line buffer and column offset buffers.
  */
  scanline=(RunlengthPacket *)
    AllocateMemory(image->columns*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to reflect image",
        "Memory allocation failed");
      DestroyImage(flipped_image);
      return((Image *) NULL);
    }
  /*
    Flip each row.
  */
  p=image->pixels;
  runlength=p->length+1;
  q=flipped_image->pixels+flipped_image->packets-1;
  for (y=0; y < (int) flipped_image->rows; y++)
  {
    /*
      Read a scan line.
    */
    s=scanline;
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Flip each column.
    */
    s=scanline+image->columns;
    for (x=0; x < (int) flipped_image->columns; x++)
    {
      s--;
      *q=(*s);
      q->length=0;
      q--;
    }
    if (QuantumTick(y,flipped_image->rows))
      ProgressMonitor(FlipImageText,y,flipped_image->rows);
  }
  FreeMemory((char *) scanline);
  return(flipped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FlopImage creates a new image that reflects each scanline in the
%  horizontal direction It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the FlopImage routine is:
%
%      flopped_image=FlopImage(image)
%
%  A description of each parameter follows:
%
%    o flopped_image: Method FlopImage returns a pointer to the image
%      after reflecting.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export Image *FlopImage(const Image *image)
{
#define FlopImageText  "  Flopping image...  "

  Image
    *flopped_image;

  int
    y;

  register int
    runlength,
    x;

  register RunlengthPacket
    *p,
    *q,
    *s;

  RunlengthPacket
    *scanline;

  /*
    Initialize flopped image attributes.
  */
  assert(image != (Image *) NULL);
  flopped_image=CloneImage(image,image->columns,image->rows,False);
  if (flopped_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to reflect image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scan line buffer and column offset buffers.
  */
  scanline=(RunlengthPacket *)
    AllocateMemory(image->columns*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to reflect image",
        "Memory allocation failed");
      DestroyImage(flopped_image);
      return((Image *) NULL);
    }
  /*
    Flop each row.
  */
  p=image->pixels;
  runlength=p->length+1;
  q=flopped_image->pixels;
  for (y=0; y < (int) flopped_image->rows; y++)
  {
    /*
      Read a scan line.
    */
    s=scanline;
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Flop each column.
    */
    s=scanline+image->columns;
    for (x=0; x < (int) flopped_image->columns; x++)
    {
      s--;
      *q=(*s);
      q->length=0;
      q++;
    }
    if (QuantumTick(y,flopped_image->rows))
      ProgressMonitor(FlopImageText,y,flopped_image->rows);
  }
  FreeMemory((char *) scanline);
  return(flopped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F r a m e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FrameImage takes an image and puts a frame around it of a
%  particular color.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the FrameImage routine is:
%
%      framed_image=FrameImage(image,frame_info)
%
%  A description of each parameter follows:
%
%    o framed_image: Method FrameImage returns a pointer to the framed
%      image.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o frame_info: Specifies a pointer to a FrameInfo structure which
%      defines the framed region.
%
%
*/
Export Image *FrameImage(const Image *image,const FrameInfo *frame_info)
{
#define FrameImageText  "  Adding frame to image...  "

  Image
    *framed_image;

  int
    height,
    width,
    y;

  register int
    runlength,
    x;

  register RunlengthPacket
    *p,
    *q;

  RunlengthPacket
    accentuate,
    highlight,
    matte,
    shadow,
    trough;

  unsigned int
    bevel_width;

  /*
    Check frame geometry.
  */
  assert(image != (Image *) NULL);
  assert(frame_info != (FrameInfo *) NULL);
  if ((frame_info->outer_bevel < 0) || (frame_info->inner_bevel < 0))
    {
      MagickWarning(OptionWarning,"Unable to frame image",
        "bevel width is negative");
      return((Image *) NULL);
    }
  bevel_width=frame_info->outer_bevel+frame_info->inner_bevel;
  width=(int) frame_info->width-frame_info->x-bevel_width;
  height=(int) frame_info->height-frame_info->y-bevel_width;
  if ((width < (int) image->columns) || (height < (int) image->rows))
    {
      MagickWarning(OptionWarning,"Unable to frame image",
        "frame is less than image size");
      return((Image *) NULL);
    }
  /*
    Initialize framed image attributes.
  */
  framed_image=CloneImage(image,frame_info->width,frame_info->height,False);
  if (framed_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to frame image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  framed_image->class=DirectClass;
  /*
    Initialize 3D effects color.
  */
  matte.red=image->matte_color.red;
  matte.green=image->matte_color.green;
  matte.blue=image->matte_color.blue;
  matte.index=Opaque;
  matte.length=0;
  accentuate.red=(unsigned int) (matte.red*AccentuateModulate+
    (MaxRGB-AccentuateModulate)*MaxRGB)/MaxRGB;
  accentuate.green=(unsigned int) (matte.green*AccentuateModulate+
    (MaxRGB-AccentuateModulate)*MaxRGB)/MaxRGB;
  accentuate.blue=(unsigned int) (matte.blue*AccentuateModulate+
    (MaxRGB-AccentuateModulate)*MaxRGB)/MaxRGB;
  accentuate.index=Opaque;
  accentuate.length=0;
  highlight.red=(unsigned int) (matte.red*HighlightModulate+
    (MaxRGB-HighlightModulate)*MaxRGB)/MaxRGB;
  highlight.green=(unsigned int) (matte.green*HighlightModulate+
    (MaxRGB-HighlightModulate)*MaxRGB)/MaxRGB;
  highlight.blue=(unsigned int) (matte.blue*HighlightModulate+
    (MaxRGB-HighlightModulate)*MaxRGB)/MaxRGB;
  highlight.index=Opaque;
  highlight.length=0;
  shadow.red=(unsigned int) (matte.red*ShadowModulate)/MaxRGB;
  shadow.green=(unsigned int) (matte.green*ShadowModulate)/MaxRGB;
  shadow.blue=(unsigned int) (matte.blue*ShadowModulate)/MaxRGB;
  shadow.index=Opaque;
  shadow.length=0;
  trough.red=(unsigned int) (matte.red*TroughModulate)/MaxRGB;
  trough.green=(unsigned int) (matte.green*TroughModulate)/MaxRGB;
  trough.blue=(unsigned int) (matte.blue*TroughModulate)/MaxRGB;
  trough.index=Opaque;
  trough.length=0;
  /*
    Put an ornamental border around the image.
  */
  q=framed_image->pixels;
  for (y=0; y < frame_info->outer_bevel; y++)
  {
    for (x=0; x < (int) (framed_image->columns-y); x++)
      if (x < y)
        *q++=highlight;
      else
        *q++=accentuate;
    for ( ; x < (int) framed_image->columns; x++)
      *q++=shadow;
  }
  for (y=0; y < (int) (frame_info->y-bevel_width); y++)
  {
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=highlight;
    for (x=0; x < (int) (framed_image->columns-2*frame_info->outer_bevel); x++)
      *q++=matte;
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=shadow;
  }
  for (y=0; y < frame_info->inner_bevel; y++)
  {
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=highlight;
    for (x=0; x < (int) (frame_info->x-bevel_width); x++)
      *q++=matte;
    for (x=0; x < (int) (image->columns+(frame_info->inner_bevel << 1)-y); x++)
      if (x < y)
        *q++=shadow;
      else
        *q++=trough;
    for ( ; x < (int) (image->columns+(frame_info->inner_bevel << 1)); x++)
      *q++=highlight;
    width=frame_info->width-frame_info->x-image->columns-bevel_width;
    for (x=0; x < width; x++)
      *q++=matte;
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=shadow;
  }
  p=image->pixels;
  runlength=p->length+1;
  for (y=0; y < (int) image->rows; y++)
  {
    /*
      Initialize scanline with border color.
    */
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=highlight;
    for (x=0; x < (int) (frame_info->x-bevel_width); x++)
      *q++=matte;
    for (x=0; x < frame_info->inner_bevel; x++)
      *q++=shadow;
    /*
      Transfer scanline.
    */
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      *q=(*p);
      q->length=0;
      q++;
    }
    for (x=0; x < frame_info->inner_bevel; x++)
      *q++=highlight;
    width=frame_info->width-frame_info->x-image->columns-bevel_width;
    for (x=0; x < width; x++)
      *q++=matte;
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=shadow;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(FrameImageText,y,image->rows);
  }
  for (y=frame_info->inner_bevel-1; y >= 0; y--)
  {
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=highlight;
    for (x=0; x < (int) (frame_info->x-bevel_width); x++)
      *q++=matte;
    for (x=0; x < y; x++)
      *q++=shadow;
    for ( ; x < (int) (image->columns+(frame_info->inner_bevel << 1)); x++)
      if (x >= (int) (image->columns+(frame_info->inner_bevel << 1)-y))
        *q++=highlight;
      else
        *q++=accentuate;
    width=frame_info->width-frame_info->x-image->columns-bevel_width;
    for (x=0; x < (int) width; x++)
      *q++=matte;
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=shadow;
  }
  height=frame_info->height-frame_info->y-image->rows-bevel_width;
  for (y=0; y < height; y++)
  {
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=highlight;
    for (x=0; x < (int) (framed_image->columns-2*frame_info->outer_bevel); x++)
      *q++=matte;
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=shadow;
  }
  for (y=frame_info->outer_bevel-1; y >= 0; y--)
  {
    for (x=0; x < y; x++)
      *q++=highlight;
    for ( ; x < (int) framed_image->columns; x++)
      if (x >= (int) (framed_image->columns-y))
        *q++=shadow;
      else
        *q++=trough;
  }
  return(framed_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     G a m m a I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GammaImage converts the reference image to gamma corrected colors.
%
%  The format of the GammaImage routine is:
%
%      GammaImage(image,gamma)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o gamma: A character string indicating the level of gamma correction.
%
%
*/
Export void GammaImage(Image *image,const char *gamma)
{
#define GammaImageText  "  Gamma correcting the image...  "

  ColorPacket
    *gamma_map;

  float
    blue_gamma,
    green_gamma,
    red_gamma;

  int
    count;

  register int
    i;

  register RunlengthPacket
    *p;

  assert(image != (Image *) NULL);
  if (gamma == (char *) NULL)
    return;
  red_gamma=1.0;
  green_gamma=1.0;
  blue_gamma=1.0;
  count=sscanf(gamma,"%f,%f,%f",&red_gamma,&green_gamma,&blue_gamma);
  count=sscanf(gamma,"%f/%f/%f",&red_gamma,&green_gamma,&blue_gamma);
  if (count == 1)
    {
      if (red_gamma == 1.0)
        return;
      green_gamma=red_gamma;
      blue_gamma=red_gamma;
    }
  /*
    Allocate and initialize gamma maps.
  */
  gamma_map=(ColorPacket *) AllocateMemory((MaxRGB+1)*sizeof(ColorPacket));
  if (gamma_map == (ColorPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to gamma image",
        "Memory allocation failed");
      return;
    }
  for (i=0; i <= MaxRGB; i++)
  {
    gamma_map[i].red=0;
    gamma_map[i].green=0;
    gamma_map[i].blue=0;
  }
  /*
    Initialize gamma table.
  */
  for (i=0; i <= MaxRGB; i++)
  {
    if (red_gamma != 0.0)
      gamma_map[i].red=(Quantum)
        ((pow((double) i/MaxRGB,1.0/red_gamma)*MaxRGB)+0.5);
    if (green_gamma != 0.0)
      gamma_map[i].green=(Quantum)
        ((pow((double) i/MaxRGB,1.0/green_gamma)*MaxRGB)+0.5);
    if (blue_gamma != 0.0)
      gamma_map[i].blue=(Quantum)
        ((pow((double) i/MaxRGB,1.0/blue_gamma)*MaxRGB)+0.5);
  }
  image->tainted=True;
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Gamma-correct DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        p->red=gamma_map[p->red].red;
        p->green=gamma_map[p->green].green;
        p->blue=gamma_map[p->blue].blue;
        p++;
        if (QuantumTick(i,image->packets))
          ProgressMonitor(GammaImageText,i,image->packets);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Gamma-correct PseudoClass image.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        image->colormap[i].red=gamma_map[image->colormap[i].red].red;
        image->colormap[i].green=gamma_map[image->colormap[i].green].green;
        image->colormap[i].blue=gamma_map[image->colormap[i].blue].blue;
      }
      SyncImage(image);
      break;
    }
  }
  if (image->gamma != 0.0)
    image->gamma*=(red_gamma+green_gamma+blue_gamma)/3.0;
  FreeMemory((char *) gamma_map);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t A n n o t a t e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetAnnotateInfo initializes the AnnotateInfo structure.
%
%  The format of the GetAnnotateInfo routine is:
%
%      GetAnnotateInfo(image_info,annotate_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o annotate_info: Specifies a pointer to a AnnotateInfo structure.
%
%
*/
Export void GetAnnotateInfo(const ImageInfo *image_info,
  AnnotateInfo *annotate_info)
{
  Image
    *annotate_image;

  assert(image_info != (ImageInfo *) NULL);
  assert(annotate_info != (AnnotateInfo *) NULL);
  annotate_info->image_info=CloneImageInfo(image_info);
  annotate_info->gravity=NorthWestGravity;
  annotate_info->geometry=(char *) NULL;
  annotate_info->text=(char *) NULL;
  annotate_info->primitive=(char *) NULL;
  annotate_info->font_name=(char *) NULL;
  annotate_info->bounds.width=annotate_info->image_info->pointsize;
  annotate_info->bounds.height=annotate_info->image_info->pointsize;
  annotate_info->bounds.x=0;
  annotate_info->bounds.y=0;
  /*
    Get tile.
  */
  if (annotate_info->image_info->pen == (char *) NULL)
    (void) strcpy(annotate_info->image_info->filename,"xc:black");
  else
    if (*annotate_info->image_info->pen == '@')
      (void) strcpy(annotate_info->image_info->filename,
        annotate_info->image_info->pen+1);
    else
      (void) FormatString(annotate_info->image_info->filename,"xc:%.1024s",
        annotate_info->image_info->pen);
  annotate_info->tile=ReadImage(annotate_info->image_info);
  if (annotate_info->image_info->font == (char *) NULL)
    return;
  /*
    Get font bounds.
  */
  FormatString(annotate_info->image_info->filename,"%.1024s",Alphabet);
  annotate_image=ReadLABELImage(annotate_info->image_info);
  if (annotate_image == (Image *) NULL)
    return;
  if (annotate_image->label != (char *) NULL)
    annotate_info->font_name=AllocateString(annotate_image->label);
  annotate_info->bounds.width=
    (annotate_image->columns+(strlen(Alphabet) >> 1))/strlen(Alphabet);
  annotate_info->bounds.height=annotate_image->rows;
  DestroyImage(annotate_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageInfo initializes the ImageInfo structure.
%
%  The format of the GetImageInfo routine is:
%
%      GetImageInfo(image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Export void GetImageInfo(ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  image_info->file=(FILE *) NULL;
  *image_info->filename='\0';
  *image_info->magick='\0';
  image_info->affirm=False;
  image_info->temporary=False;
  image_info->subimage=0;
  image_info->subrange=0;
  image_info->server_name=(char *) NULL;
  image_info->size=(char *) NULL;
  image_info->tile=(char *) NULL;
  image_info->page=(char *) NULL;
  image_info->dispose=(char *) NULL;
  image_info->delay=(char *) NULL;
  image_info->iterations=(char *) NULL;
  image_info->texture=(char *) NULL;
  image_info->view=(char *) NULL;
  image_info->box=(char *) NULL;
  image_info->font=(char *) NULL;
  image_info->pen=(char *) NULL;
  image_info->density=(char *) NULL;
  image_info->linewidth=1;
  image_info->adjoin=True;
  image_info->antialias=True;
  image_info->depth=QuantumDepth;
  image_info->dither=True;
  image_info->monochrome=False;
  image_info->pointsize=atoi(DefaultPointSize);
  image_info->fuzz=0;
  image_info->quality=atoi(DefaultImageQuality);
  image_info->verbose=False;
  image_info->colorspace=UndefinedColorspace;
  image_info->compression=UndefinedCompression;
  image_info->interlace=DefaultInterlace;
  image_info->units=UndefinedResolution;
  image_info->preview_type=JPEGPreview;
  image_info->group=0L;
  image_info->background_color=(char *) NULL;
  image_info->border_color=(char *) NULL;
  image_info->matte_color=(char *) NULL;
  TemporaryFilename(image_info->unique);
  (void) strcat(image_info->unique,"u");
  image_info->ping=False;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e T y p e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageType returns the type of image (e.g. bilevel, palette, etc).
%
%  The format of the GetImageType routine is:
%
%      type=GetImageType(image)
%
%  A description of each parameter follows:
%
%    o type: Method GetImageType returns a ImageType enum that specifies the
%      type of the specified image (e.g. bilevel, palette, etc).
%
%    o image: The address of a structure of type Image.
%
%
*/
Export ImageType GetImageType(Image *image)
{
  assert(image != (Image *) NULL);
  if (image->colorspace == CMYKColorspace)
    return(ColorSeparationType);
  if (IsMonochromeImage(image))
    return(BilevelType);
  if (IsGrayImage(image))
    return(GrayscaleType);
  if (IsPseudoClass(image))
    return(PaletteType);
  if (!image->matte)
    return(TrueColorType);
  return(MatteType);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M o n t a g e I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetMontageInfo initializes the MontageInfo structure.
%
%  The format of the GetMontageInfo routine is:
%
%      GetMontageInfo(montage_info)
%
%  A description of each parameter follows:
%
%    o montage_info: Specifies a pointer to a MontageInfo structure.
%
%
*/
Export void GetMontageInfo(MontageInfo *montage_info)
{
  assert(montage_info != (MontageInfo *) NULL);
  *montage_info->filename='\0';
  montage_info->geometry=AllocateString(DefaultTileGeometry);
  montage_info->tile=AllocateString("6x4");
  montage_info->background_color=AllocateString("#c0c0c0");
  montage_info->border_color=(char *) NULL;
  montage_info->matte_color=AllocateString("#bdbdbd");
  montage_info->title=(char *) NULL;
  montage_info->frame=(char *) NULL;
  montage_info->texture=(char *) NULL;
  montage_info->pen=(char *) NULL;
  montage_info->font=(char *) NULL;
  montage_info->pointsize=atoi(DefaultPointSize);
  montage_info->border_width=0;
  montage_info->gravity=CenterGravity;
  montage_info->shadow=False;
  montage_info->compose=ReplaceCompositeOp;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t N u m b e r S c e n e s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNumberScenes returns the number of scenes in an image sequence.
%
%  The format of the GetNumberScenes routine is:
%
%      scenes=GetNumberScenes(image)
%
%  A description of each parameter follows:
%
%    o scenes:  Method GetNumberScenes returns the number of scenes in an
%      image sequence.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int GetNumberScenes(const Image *image)
{
  const Image
    *next_image;

  unsigned int
    number_scenes;

  /*
    Compute the number of scenes in the image.
  */
  assert(image != (const Image *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  next_image=image;
  for (number_scenes=0; next_image != (Image *) NULL; number_scenes++)
    next_image=next_image->next;
  return(number_scenes);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P i x e l s                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixels returns the pixel data of an image as normalized red,
%  green, blue, and opacity values.
%
%  The format of the GetPixels routine is:
%
%      GetPixels(image,red,green,blue,opacity)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o red,green,blue,opacity: These arrays are returned with the correponding
%      red, green, blue, and opacity values from the image.  The length of the
%      arrays must equal the area specified by the columns and row values
%      values of the Image structure.  If you do not want to initialize a
%      particular component, specify it as NULL.
%
%
*/
void GetPixels(const Image *image,float *red,float *green,float *blue,
  float *opacity)
{
  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  assert(image != (Image *) NULL);
  x=0;
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    for (j=0; j <= (int) p->length; j++)
    {
      if (red != (float *) NULL)
        red[x]=(float) p->red/MaxRGB;
      if (green != (float *) NULL)
        green[x]=(float) p->green/MaxRGB;
      if (blue != (float *) NULL)
        blue[x]=(float) p->blue/MaxRGB;
      if (opacity != (float *) NULL)
        opacity[x]=image->matte ? (float) p->index/Opaque : 0;
      x++;
    }
    p++;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s G e o m e t r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsGeometry returns True if the geometry specification is valid
%  as determined by XParseGeometry.
%
%  The format of the IsGeometry routine is:
%
%      status=IsGeometry(geometry)
%
%  A description of each parameter follows:
%
%    o status: Method IsGeometry returns True if the geometry specification
%      is valid otherwise False is returned.
%
%    o geometry: This string is the geometry specification.
%
%
*/
Export unsigned int IsGeometry(const char *geometry)
{
  double
    value;

  int
    x,
    y;

  unsigned int
    flags,
    height,
    width;

  if (geometry == (const char *) NULL)
    return(False);
  flags=XParseGeometry((char *) geometry,&x,&y,&width,&height);
  return((flags != NoValue) || sscanf(geometry,"%lf",&value));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s G r a y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsGrayImage returns True if the image is grayscale otherwise
%  False is returned.  If the image is DirectClass and grayscale, it is demoted
%  to PseudoClass.
%
%  The format of the IsGrayImage routine is:
%
%      status=IsGrayImage(image)
%
%  A description of each parameter follows:
%
%    o status: Method IsGrayImage returns True if the image is grayscale
%      otherwise False is returned.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export unsigned int IsGrayImage(Image *image)
{
  register int
    i;

  unsigned int
    gray_scale;

  /*
    Determine if image is grayscale.
  */
  assert(image != (Image *) NULL);
  gray_scale=True;
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      register RunlengthPacket
        *p;

      if (image->matte)
        return(False);
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        if (!IsGray(*p))
          {
            gray_scale=False;
            break;
          }
        p++;
      }
      if (gray_scale)
        {
          QuantizeInfo
            quantize_info;

          GetQuantizeInfo(&quantize_info);
          quantize_info.colorspace=GRAYColorspace;
          (void) QuantizeImage(&quantize_info,image);
          SyncImage(image);
        }
      break;
    }
    case PseudoClass:
    {
      for (i=0; i < (int) image->colors; i++)
        if (!IsGray(image->colormap[i]))
          {
            gray_scale=False;
            break;
          }
      break;
    }
  }
  return(gray_scale);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s M o n o c h r o m e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMonochromeImage returns True if the image is monochrome otherwise
%  False is returned.  If the image is DirectClass and monochrome, it is
%  demoted to PseudoClass.
%
%  The format of the IsMonochromeImage routine is:
%
%      status=IsMonochromeImage(image)
%
%  A description of each parameter follows:
%
%    o status: Method IsMonochromeImage returns True if the image is
%      monochrome otherwise False is returned.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export unsigned int IsMonochromeImage(Image *image)
{
  /*
    Determine if image is monochrome.
  */
  assert(image != (Image *) NULL);
  if (image->pixels == (RunlengthPacket *) NULL)
    return(False);
  if (!IsGrayImage(image))
    return(False);
  if (image->colors > 2)
    return(False);
  if ((Intensity(image->colormap[0]) != 0) &&
      (Intensity(image->colormap[0]) != MaxRGB))
    return(False);
  if (image->colors == 2)
    if ((Intensity(image->colormap[1]) != 0) &&
        (Intensity(image->colormap[1]) != MaxRGB))
      return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s S u b i m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSubimage returns True if the geometry is a valid subimage
%  specification (e.g. [1], [1-9], [1,7,4]).
%
%  The format of the IsSubimage routine is:
%
%      status=IsSubimage(geometry,pedantic)
%
%  A description of each parameter follows:
%
%    o status: Method IsSubimage returns True if the geometry is a valid
%      subimage specification otherwise False is returned.
%
%    o geometry: This string is the geometry specification.
%
%    o pedantic: A value other than 0 invokes a more restriction set of
%      conditions for a valid specification (e.g. [1], [1-4], [4-1]).
%
%
*/
Export unsigned int IsSubimage(const char *geometry,const unsigned int pedantic)
{
  int
    x,
    y;

  unsigned int
    flags,
    height,
    width;

  if (geometry == (const char *) NULL)
    return(False);
  flags=XParseGeometry((char *) geometry,&x,&y,&width,&height);
  if (pedantic)
    return((flags != NoValue) && !(flags & HeightValue));
  return(IsGeometry(geometry) && !(flags & HeightValue));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s T a i n t e d                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsTainted returns True if the image has been altered since it
%  was first read or if any image in the sequence has a difference magic or
%  filename.
%
%  The format of the IsTainted routine is:
%
%      status=IsTainted(image)
%
%  A description of each parameter follows:
%
%    o status: Method IsTainted returns True if the image has been altered
%      since it was first read.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int IsTainted(const Image *image)
{
  char
    magick[MaxTextExtent],
    filename[MaxTextExtent];

  register const Image
    *p;

  assert(image != (Image *) NULL);
  (void) strcpy(magick,image->magick);
  (void) strcpy(filename,image->filename);
  for (p=image; p != (Image *) NULL; p=p->next)
  {
    if (p->tainted)
      return(True);
    if (Latin1Compare(p->magick,magick) != 0)
      return(True);
    if (Latin1Compare(p->filename,filename) != 0)
      return(True);
  }
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L a b e l I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LabelImage initializes an image label.  Optionally the label
%  can include the image filename, type, width, height, or scene number by
%  embedding special format characters.
%
%  The format of the LabelImage routine is:
%
%      LabelImage(image,label)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o label: The address of a character string containing the label format.
%
%
*/
Export void LabelImage(Image *image,const char *label)
{
  if (image->label != (char *) NULL)
    FreeMemory((char *) image->label);
  image->label=TranslateText((ImageInfo *) NULL,image,label);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     L a y e r I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LayerImage extracts the specified layer from the references image.
%
%  The format of the LayerImage routine is:
%
%      LayerImage(image,layer)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o layer: A value of type LayerType that identifies which layer to extract.
%
%
*/
Export void LayerImage(Image *image,const LayerType layer)
{
#define LayerImageText  "  Extracting the layer from the image...  "

  QuantizeInfo
    quantize_info;

  register int
    i;

  register RunlengthPacket
    *p;

  assert(image != (Image *) NULL);
  if ((layer == MatteLayer) && !image->matte)
    {
      MagickWarning(OptionWarning,"Unable to extract layer",
        "image does not have a matte layer");
      return;
    }
  /*
    Layer DirectClass packets.
  */
  image->class=DirectClass;
  image->matte=False;
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    switch (layer)
    {
      case RedLayer:
      {
        p->green=p->red;
        p->blue=p->red;
        break;
      }
      case GreenLayer:
      {
        p->red=p->green;
        p->blue=p->green;
        break;
      }
      case BlueLayer:
      {
        p->red=p->blue;
        p->green=p->blue;
        break;
      }
      case MatteLayer:
      default:
      {
        p->red=p->index;
        p->green=p->index;
        p->blue=p->index;
        break;
      }
    }
    p++;
    if (QuantumTick(i,image->packets))
      ProgressMonitor(LayerImageText,i,image->packets);
  }
  /*
    Treat layer as gray scale image.
  */
  GetQuantizeInfo(&quantize_info);
  quantize_info.colorspace=GRAYColorspace;
  (void) QuantizeImage(&quantize_info,image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     L i s t T o G r o u p I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListToGroupImage converts a linked list of images to a sequential
%  array.
%
%  The format of the ListToGroupImage routine is:
%
%      images=ListToGroupImage(images,number_images)
%
%  A description of each parameter follows:
%
%    o images: Method ListToGroupImage converts a linked list of images to
%      a sequential array and returns the array..
%
%    o images: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o number_images:  A pointer to an unsigned integer.  The number of images
%      in the image array is returned here.
%
%
*/
Export Image **ListToGroupImage(const Image *image,unsigned int *number_images)
{
  Image
    **images,
    *next_image;

  register int
    i;

  /*
    Determine the number of images in the list.
  */
  assert(image != (Image *) NULL);
  assert(number_images != (unsigned int *) NULL);
  next_image=(Image *) image;
  for (i=0; next_image != (Image *) NULL; i++)
    next_image=next_image->next;
  images=(Image **) AllocateMemory(i*sizeof(Image *));
  if (images == (Image **) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to convert image list",
        "Memory allocation failed");
      return((Image **) NULL);
    }
  *number_images=i;
  /*
    Add each image in the linked list to the group.
  */
  next_image=(Image *) image;
  for (i=0; next_image != (Image *) NULL; i++)
  {
    images[i]=next_image;
    next_image=next_image->next;
  }
  return(images);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g n i f y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagnifyImage creates a new image that is a integral size greater
%  than an existing one.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  MagnifyImage scans the reference image to create a magnified image by
%  bilinear interpolation.  The magnified image columns and rows become:
%
%    number_columns << 1
%    number_rows << 1
%
%  The format of the MagnifyImage routine is:
%
%      magnified_image=MagnifyImage(image)
%
%  A description of each parameter follows:
%
%    o magnified_image: Method MagnifyImage returns a pointer to the image
%      after magnification.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export Image *MagnifyImage(Image *image)
{
#define MagnifyImageText  "  Magnifying the image...  "

  Image
    *magnified_image;

  int
    y;

  register int
    runlength,
    x;

  register RunlengthPacket
    *p,
    *q,
    *r;

  /*
    Initialize magnified image attributes.
  */
  assert(image != (Image *) NULL);
  magnified_image=CloneImage(image,image->columns << 1,image->rows << 1,False);
  if (magnified_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to zoom image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  magnified_image->class=DirectClass;
  /*
    Initialize zoom image pixels.
  */
  p=image->pixels;
  runlength=p->length+1;
  q=magnified_image->pixels;
  for (y=0; y < (int) image->rows; y++)
  {
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      *q=(*p);
      q->length=0;
      q++;
    }
    q+=image->columns;
  }
  /*
    Magnify each row.
  */
  for (y=0; y < (int) image->rows; y++)
  {
    p=magnified_image->pixels+(image->rows-1-y)*magnified_image->columns+
      (image->columns-1);
    q=magnified_image->pixels+((image->rows-1-y) << 1)*magnified_image->columns+
      ((image->columns-1) << 1);
    *q=(*p);
    *(q+1)=(*(p));
    for (x=1; x < (int) image->columns; x++)
    {
      p--;
      q-=2;
      *q=(*p);
      (q+1)->red=(((int) p->red)+((int) (p+1)->red)+1) >> 1;
      (q+1)->green=(((int) p->green)+((int) (p+1)->green)+1) >> 1;
      (q+1)->blue=(((int) p->blue)+((int) (p+1)->blue)+1) >> 1;
      (q+1)->index=(((int) p->index)+((int) (p+1)->index)+1) >> 1;
      (q+1)->length=0;
    }
  }
  for (y=0; y < (int) (image->rows-1); y++)
  {
    p=magnified_image->pixels+(y << 1)*magnified_image->columns;
    q=p+magnified_image->columns;
    r=q+magnified_image->columns;
    for (x=0; x < (int) (image->columns-1); x++)
    {
      q->red=(((int) p->red)+((int) r->red)+1) >> 1;
      q->green=(((int) p->green)+((int) r->green)+1) >> 1;
      q->blue=(((int) p->blue)+((int) r->blue)+1) >> 1;
      q->index=(((int) p->index)+((int) r->index)+1) >> 1;
      q->length=0;
      (q+1)->red=(((int) p->red)+((int) (p+2)->red)+((int) r->red)+
        ((int) (r+2)->red)+2) >> 2;
      (q+1)->green=(((int) p->green)+((int) (p+2)->green)+((int) r->green)+
        ((int) (r+2)->green)+2) >> 2;
      (q+1)->blue=(((int) p->blue)+((int) (p+2)->blue)+((int) r->blue)+
        ((int) (r+2)->blue)+2) >> 2;
      (q+1)->index=(((int) p->index)+((int) (p+2)->index)+((int) r->index)+
        ((int) (r+2)->index)+2) >> 2;
      (q+1)->length=0;
      q+=2;
      p+=2;
      r+=2;
    }
    q->red=(((int) p->red)+((int) r->red)+1) >> 1;
    q->green=(((int) p->green)+((int) r->green)+1) >> 1;
    q->blue=(((int) p->blue)+((int) r->blue)+1) >> 1;
    q->index=(((int) p->index)+((int) r->index)+1) >> 1;
    q->length=0;
    p++;
    q++;
    r++;
    q->red=(((int) p->red)+((int) r->red)+1) >> 1;
    q->green=(((int) p->green)+((int) r->green)+1) >> 1;
    q->blue=(((int) p->blue)+((int) r->blue)+1) >> 1;
    q->index=(((int) p->index)+((int) r->index)+1) >> 1;
    q->length=0;
    p++;
    q++;
    r++;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(MagnifyImageText,y,image->rows);
  }
  p=magnified_image->pixels+(2*image->rows-2)*magnified_image->columns;
  q=magnified_image->pixels+(2*image->rows-1)*magnified_image->columns;
  for (x=0; x < (int) image->columns; x++)
  {
    *q++=(*p++);
    *q++=(*p++);
  }
  return(magnified_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a t t e F l o o d f i l l I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MatteFloodfillImage floodfills the designated area with a matte
%  value.  The floodfill algorithm is strongly based on a similar algorithm in
%  "Graphics Gems" by Paul Heckbert.
%
%  The format of the MatteFloodfillImage routine is:
%
%      MatteFloodfillImage(image,target,matte,x,y,method)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o target: A RunlengthPacket structure.  This is the RGB value of the target
%      color.
%
%    o matte: A integer value representing the amount of transparency.
%
%    o x,y: Unsigned integers representing the current location of the pen.
%
%    o method: drawing method of type PrimitiveType: floodfill or fill to
%      border.
%
%
*/
Export void MatteFloodfillImage(Image *image,const RunlengthPacket *target,
  const unsigned int matte,const int x_offset,const int y_offset,
  const PaintMethod method)
{
  int
    offset,
    skip,
    start,
    x1,
    x2;

  register int
    x,
    y;

  register RunlengthPacket
    *pixel;

  register XSegment
    *p;

  XSegment
    *segment_stack;

  /*
    Check boundary conditions.
  */
  assert(image != (Image *) NULL);
  if ((x_offset < 0) || (x_offset >= (int) image->columns))
    return;
  if ((y_offset < 0) || (y_offset >= (int) image->rows))
    return;
  if (target->index == (unsigned short) matte)
    return;
  if (!UncondenseImage(image))
    return;
  pixel=PixelOffset(image,x_offset,y_offset);
  if (pixel->index == (unsigned short) matte)
    return;
  /*
    Allocate segment stack.
  */
  x=x_offset;
  y=y_offset;
  segment_stack=(XSegment *) AllocateMemory(MaxStacksize*sizeof(XSegment));
  if (segment_stack == (XSegment *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to recolor image",
        "Memory allocation failed");
      return;
    }
  /*
    Push initial segment on stack.
  */
  image->class=DirectClass;
  if (!image->matte)
    MatteImage(image);
  start=0;
  p=segment_stack;
  Push(y,x,x,1);
  Push(y+1,x,x,-1);
  while (p > segment_stack)
  {
    /*
      Pop segment off stack.
    */
    p--;
    x1=p->x1;
    x2=p->x2;
    offset=p->y2;
    y=p->y1+offset;
    /*
      Recolor neighboring pixels.
    */
    pixel=PixelOffset(image,x1,y);
    for (x=x1; x >= 0 ; x--)
    {
      if (method == FloodfillMethod)
        {
          if (!MatteMatch(*pixel,*target,image->fuzz))
            break;
        }
      else
        if (MatteMatch(*pixel,*target,image->fuzz) ||
            (pixel->index == (unsigned short) matte))
          break;
      pixel->index=(unsigned short) matte;
      pixel--;
    }
    skip=x >= x1;
    if (!skip)
      {
        start=x+1;
        if (start < x1)
          Push(y,start,x1-1,-offset);
        x=x1+1;
      }
    do
    {
      if (!skip)
        {
          pixel=PixelOffset(image,x,y);
          for ( ; x < (int) image->columns; x++)
          {
            if (method == FloodfillMethod)
              {
                if (!MatteMatch(*pixel,*target,image->fuzz))
                  break;
              }
            else
              if (MatteMatch(*pixel,*target,image->fuzz) ||
                  (pixel->index == (unsigned short) matte))
                break;
            pixel->index=(unsigned short) matte;
            pixel++;
          }
          Push(y,start,x-1,offset);
          if (x > (x2+1))
            Push(y,x2+1,x-1,-offset);
        }
      skip=False;
      pixel=PixelOffset(image,x,y);
      for (x++; x <= x2 ; x++)
      {
        pixel++;
        if (method == FloodfillMethod)
          {
            if (MatteMatch(*pixel,*target,image->fuzz))
              break;
          }
        else
          if (!MatteMatch(*pixel,*target,image->fuzz) &&
              (pixel->index != (unsigned short) matte))
            break;
      }
      start=x;
    } while (x <= x2);
  }
  FreeMemory((char *) segment_stack);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M a t t e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MatteImage initializes the matte channel of the reference image to
%  opaque.
%
%  The format of the MatteImage routine is:
%
%      MatteImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export void MatteImage(Image *image)
{
  register int
    i;

  register RunlengthPacket
    *p;

  assert(image != (Image *) NULL);
  image->class=DirectClass;
  image->matte=True;
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    p->index=Opaque;
    p++;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M i n i f y I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MinifyImage creates a new image that is a integral size less than
%  an existing one.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  MinifyImage scans the reference image to create a minified image by computing
%  the weighted average of a 4x4 cell centered at each reference pixel.  The
%  target pixel requires two columns and two rows of the reference pixels.
%  Therefore the minified image columns and rows become:
%
%    number_columns/2
%    number_rows/2
%
%  Weights assume that the importance of neighboring pixels is negately
%  proportional to the square of their distance from the target pixel.
%
%  The scan only processes pixels that have a full set of neighbors.  Pixels
%  in the top, bottom, left, and right pairs of rows and columns are omitted
%  from the scan.
%
%  The format of the MinifyImage routine is:
%
%      minified_image=MinifyImage(image)
%
%  A description of each parameter follows:
%
%    o minified_image: Method MinifyImage returns a pointer to the image
%      after reducing.  A null image is returned if there is a memory
%      shortage or if the image size is less than IconSize*2.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export Image *MinifyImage(Image *image)
{
#define Minify(weight) \
  total_red+=(weight)*(s->red); \
  total_green+=(weight)*(s->green); \
  total_blue+=(weight)*(s->blue); \
  total_matte+=(weight)*(s->index); \
  s++;
#define MinifyImageText  "  Minifying image...  "

  Image
    *minified_image;

  int
    y;

  register int
    runlength,
    x;

  register long
    packets;

  register RunlengthPacket
    *p,
    *q,
    *s,
    *s0,
    *s1,
    *s2,
    *s3;

  RunlengthPacket
    *scanline;

  unsigned int
    blue,
    green,
    red;

  unsigned long
    max_packets,
    total_matte,
    total_blue,
    total_green,
    total_red;

  unsigned short
    index;

  assert(image != (Image *) NULL);
  if ((image->columns < 4) || (image->rows < 4))
    return((Image *) NULL);
  /*
    Initialize minified image attributes.
  */
  max_packets=Max(image->packets >> 2,1);
  minified_image=CloneImage(image,max_packets,1,False);
  if (minified_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to reduce image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  minified_image->class=DirectClass;
  minified_image->columns=image->columns >> 1;
  minified_image->rows=image->rows >> 1;
  packets=0;
  /*
    Allocate image buffer and scanline buffer for 4 rows of the image.
  */
  scanline=(RunlengthPacket *)
    AllocateMemory(4*(image->columns+1)*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to reduce image",
        "Memory allocation failed");
      DestroyImage(minified_image);
      return((Image *) NULL);
    }
  /*
    Preload the first 2 rows of the image.
  */
  p=image->pixels;
  runlength=p->length+1;
  for (x=0; x < (int) (4*(image->columns+1)); x++)
    scanline[x]=(*p);
  s=scanline;
  for (x=0; x < (int) (image->columns << 1); x++)
  {
    if (runlength != 0)
      runlength--;
    else
      {
        p++;
        runlength=p->length;
      }
    *s=(*p);
    s++;
  }
  /*
    Reduce each row.
  */
  p=image->pixels;
  runlength=p->length+1;
  q=minified_image->pixels;
  SetRunlengthEncoder(q);
  for (y=0; y < (int) (image->rows-1); y+=2)
  {
    /*
      Initialize sliding window pointers.
    */
    s0=scanline+image->columns*((y+0) % 4);
    s1=scanline+image->columns*((y+1) % 4);
    s2=scanline+image->columns*((y+2) % 4);
    s3=scanline+image->columns*((y+3) % 4);
    /*
      Read another scan line.
    */
    s=s2;
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Read another scan line.
    */
    s=s3;
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    for (x=0; x < (int) (image->columns-1); x+=2)
    {
      /*
        Compute weighted average of target pixel color components.

        These particular coefficients total to 128.  Use 128/2-1 or 63 to
        insure correct round off.
      */
      total_red=0;
      total_green=0;
      total_blue=0;
      total_matte=0;
      s=s0;
      Minify(3); Minify(7);  Minify(7);  Minify(3);
      s=s1;
      Minify(7); Minify(15); Minify(15); Minify(7);
      s=s2;
      Minify(7); Minify(15); Minify(15); Minify(7);
      s=s3;
      Minify(3); Minify(7);  Minify(7);  Minify(3);
      s0+=2;
      s1+=2;
      s2+=2;
      s3+=2;
      red=(Quantum) ((total_red+63) >> 7);
      green=(Quantum) ((total_green+63) >> 7);
      blue=(Quantum) ((total_blue+63) >> 7);
      index=(unsigned short) ((total_matte+63) >> 7);
      if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
          (index == q->index) && ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          if (packets == (int) max_packets)
            {
              max_packets<<=1;
              minified_image->pixels=(RunlengthPacket *) ReallocateMemory(
                (char *) minified_image->pixels,max_packets*
                sizeof(RunlengthPacket));
              if (minified_image->pixels == (RunlengthPacket *) NULL)
                {
                  MagickWarning(ResourceLimitWarning,"Unable to reduce image",
                    "Memory allocation failed");
                  DestroyImage(minified_image);
                  return((Image *) NULL);
                }
              q=minified_image->pixels+packets-1;
            }
          q->red=red;
          q->green=green;
          q->blue=blue;
          q->index=index;
          q->length=0;
        }
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(MinifyImageText,y,image->rows-1);
  }
  minified_image->packets=packets;
  minified_image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
    minified_image->pixels,minified_image->packets*sizeof(RunlengthPacket));
  FreeMemory((char *) scanline);
  return(minified_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o d u l a t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ModulateImage modulates the hue, saturation, and brightness of an
%  image.
%
%  The format of the ModulateImage routine is:
%
%      ModulateImage(image,modulate)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o modulate: A character string indicating the percent change in brightness,
%      saturation, and hue in floating point notation separated by commas
%      (e.g. 10.1,0.0,3.1).
%
%
*/
Export void ModulateImage(Image *image,const char *modulate)
{
#define ModulateImageText  "  Modulating image...  "

  double
    percent_brightness,
    percent_hue,
    percent_saturation;

  register int
    i;

  register RunlengthPacket
    *p;

  /*
    Initialize gamma table.
  */
  assert(image != (Image *) NULL);
  if (modulate == (char *) NULL)
    return;
  percent_hue=0.0;
  percent_brightness=0.0;
  percent_saturation=0.0;
  (void) sscanf(modulate,"%lf,%lf,%lf",&percent_brightness,&percent_saturation,
    &percent_hue);
  (void) sscanf(modulate,"%lf/%lf/%lf",&percent_brightness,&percent_saturation,
    &percent_hue);
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Modulate the color for a DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        Modulate(percent_hue,percent_saturation,percent_brightness,
          &p->red,&p->green,&p->blue);
        p++;
        if (QuantumTick(i,image->packets))
          ProgressMonitor(ModulateImageText,i,image->packets);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Modulate the color for a PseudoClass image.
      */
      for (i=0; i < (int) image->colors; i++)
        Modulate(percent_hue,percent_saturation,percent_brightness,
          &image->colormap[i].red,&image->colormap[i].green,
          &image->colormap[i].blue);
      SyncImage(image);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o g r i f y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MogrifyImage applies image processing options to an image as
%  prescribed by command line options.
%
%  The format of the MogrifyImage routine is:
%
%      MogrifyImage(image_info,argc,argv,image)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export void MogrifyImage(const ImageInfo *image_info,const int argc,char **argv,
  Image **image)
{
  char
    *geometry,
    *option;

  Image
    *map_image,
    *region_image;

  ImageInfo
    *local_info;

  int
    flags,
    x,
    y;

  QuantizeInfo
    quantize_info;

  RectangleInfo
    region_info;

  register int
    i;

  unsigned int
    compress,
    gravity,
    matte,
    height,
    width;

  XColor
    target_color;

  /*
    Verify option length.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image != (Image **) NULL);
  for (i=1; i < argc; i++)
    if (Extent(argv[i]) > (MaxTextExtent/2-1))
      {
        MagickWarning(ResourceLimitWarning,"Option length exceeds limit",
          argv[i]);
        return;
      }
  /*
    Initialize routine variables.
  */
  local_info=CloneImageInfo(image_info);
  GetQuantizeInfo(&quantize_info);
  compress=(*image)->packets < (((*image)->columns*(*image)->rows*3) >> 2);
  geometry=(char *) NULL;
  gravity=ForgetGravity;
  map_image=(Image *) NULL;
  quantize_info.number_colors=0;
  quantize_info.tree_depth=0;
  quantize_info.dither=True;
  if (local_info->monochrome)
    if (!IsMonochromeImage(*image))
      {
        quantize_info.number_colors=2;
        quantize_info.tree_depth=8;
        quantize_info.colorspace=GRAYColorspace;
      }
  region_image=(Image *) NULL;
  region_info.width=(*image)->columns;
  region_info.height=(*image)->rows;
  region_info.x=0;
  region_info.y=0;
  /*
    Transmogrify the image.
  */
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((Extent(option) <= 1) || ((*option != '-') && (*option != '+')))
      continue;
    if (strncmp("antialias",option+1,3) == 0)
      {
        local_info->antialias=(*option == '-');
        continue;
      }
    if (strncmp("-background",option,6) == 0)
      {
        (void) CloneString(&local_info->background_color,argv[++i]);
        (void) XQueryColorDatabase(local_info->background_color,&target_color);
        (*image)->background_color.red=XDownScale(target_color.red);
        (*image)->background_color.green=XDownScale(target_color.green);
        (*image)->background_color.blue=XDownScale(target_color.blue);
        continue;
      }
    if (strncmp("-blur",option,4) == 0)
      {
        double
          factor;

        Image
          *blurred_image;

        /*
          Blur an image.
        */
        factor=atof(argv[++i]);
        blurred_image=BlurImage(*image,factor);
        if (blurred_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=blurred_image;
          }
        continue;
      }
    if (Latin1Compare("-border",option) == 0)
      {
        Image
          *bordered_image;

        RectangleInfo
          border_info;

        /*
          Surround image with a border of solid color.
        */
        border_info.width=0;
        border_info.height=0;
        border_info.x=0;
        border_info.y=0;
        flags=XParseGeometry(argv[++i],&border_info.x,&border_info.y,
          &border_info.width,&border_info.height);
        if ((flags & HeightValue) == 0)
          border_info.height=border_info.width;
        bordered_image=BorderImage(*image,&border_info);
        if (bordered_image != (Image *) NULL)
          {
            DestroyImage(*image);
            bordered_image->class=DirectClass;
            *image=bordered_image;
          }
        continue;
      }
    if (strncmp("-bordercolor",option,8) == 0)
      {
        (void) CloneString(&local_info->border_color,argv[++i]);
        (void) XQueryColorDatabase(local_info->border_color,&target_color);
        (*image)->border_color.red=XDownScale(target_color.red);
        (*image)->border_color.green=XDownScale(target_color.green);
        (*image)->border_color.blue=XDownScale(target_color.blue);
        continue;
      }
    if (Latin1Compare("-box",option) == 0)
      {
        (void) CloneString(&local_info->box,argv[++i]);
        continue;
      }
    if (strncmp("-charcoal",option,3) == 0)
      {
        char
          *commands[7];

        QuantizeInfo
          quantize_info;

        /*
          Charcoal drawing.
        */
        i++;
        GetQuantizeInfo(&quantize_info);
        quantize_info.dither=quantize_info.dither;
        quantize_info.colorspace=GRAYColorspace;
        (void) QuantizeImage(&quantize_info,*image);
        SyncImage(*image);
        commands[0]=SetClientName((char *) NULL);
        commands[1]="-edge";
        commands[2]=argv[i];
        commands[3]="-blur";
        commands[4]=argv[i];
        commands[5]="-normalize";
        commands[6]="-negate";
        MogrifyImage(local_info,7,commands,image);
        (void) QuantizeImage(&quantize_info,*image);
        SyncImage(*image);
        continue;
      }
    if (strncmp("-colorize",option,8) == 0)
      {
        ColorizeImage(*image,argv[++i],local_info->pen);
        continue;
      }
    if (Latin1Compare("-colors",option) == 0)
      {
        quantize_info.number_colors=atoi(argv[++i]);
        continue;
      }
    if (strncmp("-colorspace",option,8) == 0)
      {
        option=argv[++i];
        if (Latin1Compare("cmyk",option) == 0)
          {
            RGBTransformImage(*image,CMYKColorspace);
            quantize_info.colorspace=CMYKColorspace;
          }
        if (Latin1Compare("gray",option) == 0)
          {
            quantize_info.colorspace=GRAYColorspace;
            if (quantize_info.number_colors == 0)
              quantize_info.number_colors=256;
            quantize_info.tree_depth=8;
          }
        if (Latin1Compare("ohta",option) == 0)
          quantize_info.colorspace=OHTAColorspace;
        if (Latin1Compare("rgb",option) == 0)
          {
            TransformRGBImage(*image,RGBColorspace);
            quantize_info.colorspace=RGBColorspace;
          }
        if (Latin1Compare("srgb",option) == 0)
          quantize_info.colorspace=sRGBColorspace;
        if (Latin1Compare("transparent",option) == 0)
          quantize_info.colorspace=TransparentColorspace;
        if (Latin1Compare("xyz",option) == 0)
          quantize_info.colorspace=XYZColorspace;
        if (Latin1Compare("ycbcr",option) == 0)
          quantize_info.colorspace=YCbCrColorspace;
        if (Latin1Compare("ycc",option) == 0)
          quantize_info.colorspace=YCCColorspace;
        if (Latin1Compare("yiq",option) == 0)
          quantize_info.colorspace=YIQColorspace;
        if (Latin1Compare("ypbpr",option) == 0)
          quantize_info.colorspace=YPbPrColorspace;
        if (Latin1Compare("yuv",option) == 0)
          quantize_info.colorspace=YUVColorspace;
        local_info->colorspace=quantize_info.colorspace;
        continue;
      }
    if (strncmp("comment",option+1,4) == 0)
      {
        if (*option == '-')
          CommentImage(*image,argv[++i]);
        else
          CommentImage(*image,(char *) NULL);
        continue;
      }
    if (strncmp("contrast",option+1,3) == 0)
      {
        ContrastImage(*image,(unsigned int) (*option == '-'));
        continue;
      }
    if (strncmp("-crop",option,3) == 0)
      {
        TransformImage(image,argv[++i],(char *) NULL);
        continue;
      }
    if (strncmp("-cycle",option,3) == 0)
      {
        /*
          Cycle an image colormap.
        */
        CycleColormapImage(*image,atoi(argv[++i]));
        continue;
      }
    if (strncmp("-density",option,4) == 0)
      {
        int
          count;

        /*
          Set image density.
        */
        (void) CloneString(&local_info->density,argv[++i]);
        count=sscanf(local_info->density,"%fx%f",
          &(*image)->x_resolution,&(*image)->y_resolution);
        if (count != 2)
          (*image)->y_resolution=(*image)->x_resolution;
      }
    if (strncmp("-despeckle",option,4) == 0)
      {
        Image
          *despeckled_image;

        /*
          Reduce the speckles within an image.
        */
        despeckled_image=DespeckleImage(*image);
        if (despeckled_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=despeckled_image;
          }
        continue;
      }
    if (strncmp("-display",option,6) == 0)
      {
        (void) CloneString(&local_info->server_name,argv[++i]);
        continue;
      }
    if (strncmp("dither",option+1,3) == 0)
      {
        local_info->dither=(*option == '-');
        quantize_info.dither=(*option == '-');
        continue;
      }
    if (strncmp("-draw",option,3) == 0)
      {
        AnnotateInfo
          annotate_info;

        /*
          Draw image.
        */
        GetAnnotateInfo(local_info,&annotate_info);
        (void) CloneString(&annotate_info.primitive,argv[++i]);
        if (geometry != (char *) NULL)
          (void) CloneString(&annotate_info.geometry,geometry);
        if (gravity != ForgetGravity)
          annotate_info.gravity=gravity;
        DrawImage(*image,&annotate_info);
        DestroyAnnotateInfo(&annotate_info);
        continue;
      }
    if (strncmp("-edge",option,3) == 0)
      {
        double
          factor;

        Image
          *edged_image;

        /*
          Detect edges in the image.
        */
        factor=atof(argv[++i]);
        edged_image=EdgeImage(*image,factor);
        if (edged_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=edged_image;
          }
        continue;
      }
    if (strncmp("-emboss",option,3) == 0)
      {
        Image
          *embossed_image;

        /*
          Emboss image.
        */
        embossed_image=EmbossImage(*image);
        if (embossed_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=embossed_image;
          }
        continue;
      }
    if (strncmp("-enhance",option,3) == 0)
      {
        Image
          *enhanced_image;

        /*
          Enhance image.
        */
        enhanced_image=EnhanceImage(*image);
        if (enhanced_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=enhanced_image;
          }
        continue;
      }
    if (strncmp("-equalize",option,3) == 0)
      {
        /*
          Equalize image.
        */
        EqualizeImage(*image);
        continue;
      }
    if (strncmp("filter",option+1,3) == 0)
      {
        if (*option == '-')
          {
            FilterType
              filter;

            option=argv[++i];
            if (Latin1Compare("Point",option) == 0)
              filter=PointFilter;
            if (Latin1Compare("Box",option) == 0)
              filter=BoxFilter;
            if (Latin1Compare("Triangle",option) == 0)
              filter=TriangleFilter;
            if (Latin1Compare("Hermite",option) == 0)
              filter=HermiteFilter;
            if (Latin1Compare("Hanning",option) == 0)
              filter=HanningFilter;
            if (Latin1Compare("Hamming",option) == 0)
              filter=HammingFilter;
            if (Latin1Compare("Blackman",option) == 0)
              filter=BlackmanFilter;
            if (Latin1Compare("Gaussian",option) == 0)
              filter=GaussianFilter;
            if (Latin1Compare("Quadratic",option) == 0)
              filter=QuadraticFilter;
            if (Latin1Compare("Cubic",option) == 0)
              filter=CubicFilter;
            if (Latin1Compare("Catrom",option) == 0)
              filter=CatromFilter;
            if (Latin1Compare("Mitchell",option) == 0)
              filter=MitchellFilter;
            if (Latin1Compare("Lanczos",option) == 0)
              filter=LanczosFilter;
            if (Latin1Compare("Bessel",option) == 0)
              filter=BesselFilter;
            if (Latin1Compare("Sinc",option) == 0)
              filter=SincFilter;
            (*image)->filter=filter;
          }
        continue;
      }
    if (strncmp("-flip",option,4) == 0)
      {
        Image
          *flipped_image;

        /*
          Flip image scanlines.
        */
        flipped_image=FlipImage(*image);
        if (flipped_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=flipped_image;
          }
        continue;
      }
    if (strncmp("-flop",option,4) == 0)
      {
        Image
          *flopped_image;

        /*
          Flop image scanlines.
        */
        flopped_image=FlopImage(*image);
        if (flopped_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=flopped_image;
          }
        continue;
      }
    if (Latin1Compare("-frame",option) == 0)
      {
        Image
          *framed_image;

        FrameInfo
          frame_info;

        /*
          Surround image with an ornamental border.
        */
        frame_info.width=0;
        frame_info.height=0;
        frame_info.outer_bevel=0;
        frame_info.inner_bevel=0;
        flags=XParseGeometry(argv[++i],&frame_info.outer_bevel,
          &frame_info.inner_bevel,&frame_info.width,&frame_info.height);
        if ((flags & HeightValue) == 0)
          frame_info.height=frame_info.width;
        if ((flags & XValue) == 0)
          frame_info.outer_bevel=(frame_info.width >> 2)+1;
        if ((flags & YValue) == 0)
          frame_info.inner_bevel=frame_info.outer_bevel;
        frame_info.x=frame_info.width;
        frame_info.y=frame_info.height;
        frame_info.width=(*image)->columns+(frame_info.width << 1);
        frame_info.height=(*image)->rows+(frame_info.height << 1);
        framed_image=FrameImage(*image,&frame_info);
        if (framed_image != (Image *) NULL)
          {
            DestroyImage(*image);
            framed_image->class=DirectClass;
            *image=framed_image;
          }
        continue;
      }
    if (strncmp("-fuzz",option,3) == 0)
      {
        (*image)->fuzz=atoi(argv[++i]);
        continue;
      }
    if (Latin1Compare("-font",option) == 0)
      {
        (void) CloneString(&local_info->font,argv[++i]);
        continue;
      }
    if (strncmp("gamma",option+1,2) == 0)
      {
        if (*option == '+')
          (*image)->gamma=atof(argv[++i]);
        else
          GammaImage(*image,argv[++i]);
        continue;
      }
    if (strncmp("-geometry",option,4) == 0)
      {
        Image
          *zoomed_image;

        /*
          Resize image.
        */
        width=(*image)->columns;
        height=(*image)->rows;
        x=0;
        y=0;
        (void) CloneString(&geometry,argv[++i]);
        (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
        zoomed_image=ZoomImage(*image,width,height);
        if (zoomed_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=zoomed_image;
          }
        continue;
      }
    if (strncmp("gravity",option+1,2) == 0)
      {
        gravity=NorthWestGravity;
        if (*option == '-')
          {
            option=argv[++i];
            if (Latin1Compare("NorthWest",option) == 0)
              gravity=NorthWestGravity;
            if (Latin1Compare("North",option) == 0)
              gravity=NorthGravity;
            if (Latin1Compare("NorthEast",option) == 0)
              gravity=NorthEastGravity;
            if (Latin1Compare("West",option) == 0)
              gravity=WestGravity;
            if (Latin1Compare("Center",option) == 0)
              gravity=CenterGravity;
            if (Latin1Compare("East",option) == 0)
              gravity=EastGravity;
            if (Latin1Compare("SouthWest",option) == 0)
              gravity=SouthWestGravity;
            if (Latin1Compare("South",option) == 0)
              gravity=SouthGravity;
            if (Latin1Compare("SouthEast",option) == 0)
              gravity=SouthEastGravity;
          }
        continue;
      }
    if (strncmp("-implode",option,4) == 0)
      {
        double
          amount;

        Image
          *imploded_image;

        /*
          Implode image.
        */
        amount=atof(argv[++i]);
        imploded_image=ImplodeImage(*image,amount);
        if (imploded_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=imploded_image;
          }
        continue;
      }
    if (strncmp("label",option+1,3) == 0)
      {
        if (*option == '-')
          LabelImage(*image,argv[++i]);
        else
          LabelImage(*image,(char *) NULL);
        continue;
      }
    if (strncmp("layer",option+1,3) == 0)
      {
        LayerType
          layer;

        layer=UndefinedLayer;
        if (*option == '-')
          {
            option=argv[++i];
            if (Latin1Compare("Red",option) == 0)
              layer=RedLayer;
            if (Latin1Compare("Green",option) == 0)
              layer=GreenLayer;
            if (Latin1Compare("Blue",option) == 0)
              layer=BlueLayer;
            if (Latin1Compare("Matte",option) == 0)
              layer=MatteLayer;
          }
        LayerImage(*image,layer);
        continue;
      }
    if (strncmp("-linewidth",option,3) == 0)
      {
        local_info->linewidth=atoi(argv[++i]);
        continue;
      }
    if (Latin1Compare("-map",option) == 0)
      {
        /*
          Transform image colors to match this set of colors.
        */
        (void) strcpy(local_info->filename,argv[++i]);
        map_image=ReadImage(local_info);
        continue;
      }
    if (Latin1Compare("matte",option+1) == 0)
      {
        if (*option == '-')
          if (!(*image)->matte)
            MatteImage(*image);
        (*image)->matte=(*option == '-');
        continue;
      }
    if (strncmp("-mattecolor",option,7) == 0)
      {
        (void) CloneString(&local_info->matte_color,argv[++i]);
        (void) XQueryColorDatabase(local_info->matte_color,&target_color);
        (*image)->matte_color.red=XDownScale(target_color.red);
        (*image)->matte_color.green=XDownScale(target_color.green);
        (*image)->matte_color.blue=XDownScale(target_color.blue);
        continue;
      }
    if (strncmp("-modulate",option,4) == 0)
      {
        ModulateImage(*image,argv[++i]);
        continue;
      }
    if (strncmp("-monochrome",option,4) == 0)
      {
        local_info->monochrome=True;
        quantize_info.number_colors=2;
        quantize_info.tree_depth=8;
        quantize_info.colorspace=GRAYColorspace;
        continue;
      }
    if (strncmp("negate",option+1,3) == 0)
      {
        NegateImage(*image,*option == '+');
        continue;
      }
    if (strncmp("noise",option+1,4) == 0)
      {
        Image
          *noisy_image;

        /*
          Reduce noise in image.
        */
        if (*option == '-')
          noisy_image=ReduceNoiseImage(*image);
        else
          {
            NoiseType
              noise_type;

            option=argv[++i];
            noise_type=UniformNoise;
            if (Latin1Compare("Gaussian",option) == 0)
              noise_type=GaussianNoise;
            if (Latin1Compare("multiplicative",option) == 0)
              noise_type=MultiplicativeGaussianNoise;
            if (Latin1Compare("impulse",option) == 0)
              noise_type=ImpulseNoise;
            if (Latin1Compare("laplacian",option) == 0)
              noise_type=LaplacianNoise;
            if (Latin1Compare("Poisson",option) == 0)
              noise_type=PoissonNoise;
            noisy_image=AddNoiseImage(*image,noise_type);
          }
        if (noisy_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=noisy_image;
          }
        continue;
      }
    if (strncmp("-normalize",option,4) == 0)
      {
        NormalizeImage(*image);
        continue;
      }
    if (strncmp("-opaque",option,3) == 0)
      {
        OpaqueImage(*image,argv[++i],local_info->pen);
        continue;
      }
    if (strncmp("-paint",option,4) == 0)
      {
        Image
          *painted_image;

        /*
          Oil paint image.
        */
        painted_image=OilPaintImage(*image,atoi(argv[++i]));
        if (painted_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=painted_image;
          }
        continue;
      }
    if (Latin1Compare("-pen",option) == 0)
      {
        (void) CloneString(&local_info->pen,argv[++i]);
        continue;
      }
    if (strncmp("pointsize",option+1,2) == 0)
      {
        local_info->pointsize=atoi(argv[++i]);
        continue;
      }
    if (strncmp("profile",option+1,4) == 0)
      {
        Image
          *profile;

        if (*option == '+')
          {
            /*
              Remove a ICC or IPTC profile from the image.
            */
            option=argv[++i];
            if (Latin1Compare("icc",option) == 0)
              {
                if ((*image)->color_profile.length != 0)
                  FreeMemory((char *) (*image)->color_profile.info);
                (*image)->color_profile.length=0;
                (*image)->color_profile.info=(unsigned char *) NULL;
              }
            if (Latin1Compare("iptc",option) == 0)
              {
                if ((*image)->iptc_profile.length != 0)
                  FreeMemory((char *) (*image)->iptc_profile.info);
                (*image)->iptc_profile.length=0;
                (*image)->iptc_profile.info=(unsigned char *) NULL;
              }
            continue;
          }
        /*
          Add a ICC or IPTC profile to the image.
        */
        (void) strcpy(local_info->filename,argv[++i]);
        profile=ReadImage(local_info);
        if (profile == (Image *) NULL)
          continue;
        if (Latin1Compare("icc",profile->magick) == 0)
          {
            if ((*image)->color_profile.length != 0)
              FreeMemory((char *) (*image)->color_profile.info);
            (*image)->color_profile.length=profile->color_profile.length;
            (*image)->color_profile.info=profile->color_profile.info;
            profile->color_profile.length=0;
            profile->color_profile.info=(unsigned char *) NULL;
          }
        if (Latin1Compare("iptc",profile->magick) == 0)
          {
            if ((*image)->iptc_profile.length != 0)
              FreeMemory((char *) (*image)->iptc_profile.info);
            (*image)->iptc_profile.length=profile->iptc_profile.length;
            (*image)->iptc_profile.info=profile->iptc_profile.info;
            profile->iptc_profile.length=0;
            profile->iptc_profile.info=(unsigned char *) NULL;
          }
        DestroyImage(profile);
        continue;
      }
    if (strncmp("raise",option+1,2) == 0)
      {
        RectangleInfo
          raise_info;

        /*
          Surround image with a raise of solid color.
        */
        raise_info.width=0;
        raise_info.height=0;
        raise_info.x=0;
        raise_info.y=0;
        flags=XParseGeometry(argv[++i],&raise_info.x,&raise_info.y,
          &raise_info.width,&raise_info.height);
        if ((flags & HeightValue) == 0)
          raise_info.height=raise_info.width;
        RaiseImage(*image,&raise_info,*option == '-');
        continue;
      }
    if (strncmp("region",option+1,3) == 0)
      {
        Image
          *cropped_image;

        if (region_image != (Image *) NULL)
          {
            /*
              Composite region.
            */
            matte=region_image->matte;
            CompositeImage(region_image,
              (*image)->matte ? OverCompositeOp : ReplaceCompositeOp,*image,
              region_info.x,region_info.y);
            DestroyImage(*image);
            *image=region_image;
            (*image)->matte=matte;
          }
        if (*option == '+')
          continue;
        /*
          Apply transformations to a selected region of the image.
        */
        region_info.width=(*image)->columns;
        region_info.height=(*image)->rows;
        region_info.x=0;
        region_info.y=0;
        (void) XParseGeometry(argv[++i],&region_info.x,&region_info.y,
          &region_info.width,&region_info.height);
        cropped_image=CropImage(*image,&region_info);
        if (cropped_image == (Image *) NULL)
          continue;
        region_image=(*image);
        *image=cropped_image;
        continue;
      }
    if (strncmp("-roll",option,4) == 0)
      {
        Image
          *rolled_image;

        /*
          Roll image.
        */
        x=0;
        y=0;
        flags=XParseGeometry(argv[++i],&x,&y,&width,&height);
        rolled_image=RollImage(*image,x,y);
        if (rolled_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=rolled_image;
          }
        continue;
      }
    if (strncmp("-rotate",option,4) == 0)
      {
        double
          degrees;

        Image
          *rotated_image;

        /*
          Check for conditional image rotation.
        */
        i++;
        if (strchr(argv[i],'>') != (char *) NULL)
          if ((*image)->columns <= (*image)->rows)
            break;
        if (strchr(argv[i],'<') != (char *) NULL)
          if ((*image)->columns >= (*image)->rows)
            break;
        /*
          Rotate image.
        */
        degrees=atof(argv[i]);
        rotated_image=RotateImage(*image,degrees,False,True);
        if (rotated_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=rotated_image;
          }
        continue;
      }
    if (strncmp("-sample",option,3) == 0)
      {
        Image
          *sampled_image;

        /*
          Sample image with pixel replication.
        */
        width=(*image)->columns;
        height=(*image)->rows;
        x=0;
        y=0;
        (void) ParseImageGeometry(argv[++i],&x,&y,&width,&height);
        sampled_image=SampleImage(*image,width,height);
        if (sampled_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=sampled_image;
          }
        continue;
      }
    if (strncmp("sans",option+1,2) == 0)
      if (*option == '-')
        i++;
    if (Latin1Compare("-scene",option) == 0)
      {
        (*image)->scene=atoi(argv[++i]);
        continue;
      }
    if (strncmp("-segment",option,4) == 0)
      {
        double
          cluster_threshold,
          smoothing_threshold;

        /*
          Segment image.
        */
        cluster_threshold=1.0;
        smoothing_threshold=1.5;
        (void) sscanf(argv[++i],"%lfx%lf",&cluster_threshold,
          &smoothing_threshold);
        (void) SegmentImage(*image,quantize_info.colorspace,local_info->verbose,
          (double) cluster_threshold,(double) smoothing_threshold);
        SyncImage(*image);
        continue;
      }
    if (strncmp("shade",option+1,5) == 0)
      {
        double
          azimuth,
          elevation;

        Image
          *shaded_image;

        /*
          Shade image.
        */
        azimuth=30.0;
        elevation=30.0;
        if (*option == '-')
          (void) sscanf(argv[++i],"%lfx%lf",&azimuth,&elevation);
        shaded_image=ShadeImage(*image,*option == '-',(double) azimuth,
          (double) elevation);
        if (shaded_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=shaded_image;
          }
        continue;
      }
    if (strncmp("-sharpen",option,5) == 0)
      {
        double
          factor;

        Image
          *sharpened_image;

        /*
          Sharpen an image.
        */
        factor=atof(argv[++i]);
        sharpened_image=SharpenImage(*image,factor);
        if (sharpened_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=sharpened_image;
          }
        continue;
      }
    if (strncmp("-shear",option,4) == 0)
      {
        double
          x_shear,
          y_shear;

        Image
          *sheared_image;

        /*
          Shear image.
        */
        x_shear=0.0;
        y_shear=0.0;
        (void) sscanf(argv[++i],"%lfx%lf",&x_shear,&y_shear);
        sheared_image=
          ShearImage(*image,(double) x_shear,(double) y_shear,False);
        if (sheared_image != (Image *) NULL)
          {
            DestroyImage(*image);
            sheared_image->class=DirectClass;
            *image=sheared_image;
          }
        continue;
      }
    if (strncmp("-solarize",option,3) == 0)
      {
        SolarizeImage(*image,atof(argv[++i]));
        continue;
      }
    if (strncmp("-spread",option,3) == 0)
      {
        unsigned int
          amount;

        Image
          *spread_image;

        /*
          Spread an image.
        */
        amount=atoi(argv[++i]);
        spread_image=SpreadImage(*image,amount);
        if (spread_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=spread_image;
          }
        continue;
      }
    if (strncmp("-swirl",option,3) == 0)
      {
        double
          degrees;

        Image
          *swirled_image;

        /*
          Swirl image.
        */
        degrees=atof(argv[++i]);
        swirled_image=SwirlImage(*image,degrees);
        if (swirled_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=swirled_image;
          }
        continue;
      }
    if (strncmp("-threshold",option,3) == 0)
      {
        double
          threshold;

        /*
          Threshold image.
        */
        threshold=atof(argv[++i]);
        ThresholdImage(*image,threshold);
        continue;
      }
    if (strncmp("-transparent",option,4) == 0)
      {
        TransparentImage(*image,argv[++i]);
        continue;
      }
    if (strncmp("-treedepth",option,4) == 0)
      {
        quantize_info.tree_depth=atoi(argv[++i]);
        continue;
      }
    if (strncmp("units",option+1,3) == 0)
      {
        (*image)->units=UndefinedResolution;
        if (*option == '-')
          {
            option=argv[++i];
            if (Latin1Compare("PixelsPerInch",option) == 0)
              (*image)->units=PixelsPerInchResolution;
            if (Latin1Compare("PixelsPerCentimeter",option) == 0)
              (*image)->units=PixelsPerCentimeterResolution;
          }
        continue;
      }
    if (Latin1Compare("wave",option+1) == 0)
      {
        double
          amplitude,
          wavelength;

        Image
          *waved_image;

        /*
          Wave image.
        */
        amplitude=25.0;
        wavelength=150.0;
        if (*option == '-')
          (void) sscanf(argv[++i],"%lfx%lf",&amplitude,&wavelength);
        waved_image=WaveImage(*image,(double) amplitude,(double) wavelength);
        if (waved_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=waved_image;
          }
        continue;
      }
  }
  if (region_image != (Image *) NULL)
    {
      /*
        Composite transformed region onto image.
      */
      matte=region_image->matte;
      CompositeImage(region_image,
        (*image)->matte ? OverCompositeOp : ReplaceCompositeOp,*image,
        region_info.x,region_info.y);
      DestroyImage(*image);
      *image=region_image;
      (*image)->matte=matte;
    }
  if (compress)
    CondenseImage(*image);
  if ((quantize_info.number_colors != 0) ||
      (quantize_info.colorspace == GRAYColorspace))
    {
      /*
        Reduce the number of colors in the image.
      */
      if (((*image)->class == DirectClass) ||
          ((*image)->colors > quantize_info.number_colors) ||
          (quantize_info.colorspace == GRAYColorspace))
        (void) QuantizeImage(&quantize_info,*image);
      else
        CompressColormap(*image);
      /*
        Measure quantization error.
      */
      if (local_info->verbose)
        (void) QuantizationError(*image);
      SyncImage(*image);
    }
  if (map_image != (Image *) NULL)
    {
      (void) MapImage(*image,map_image,quantize_info.dither);
      DestroyImage(map_image);
    }
  /*
    Free resources.
  */
  if (geometry != (char *) NULL)
    FreeMemory((char *) geometry);
  DestroyImageInfo(local_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o g r i f y I m a g e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MogrifyImages applies image processing options to a sequence of
%  images as prescribed by command line options.
%
%  The format of the MogrifyImage routine is:
%
%      MogrifyImages(image_info,argc,argv,images)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%    o images: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export void MogrifyImages(const ImageInfo *image_info,const int argc,
  char **argv,Image **images)
{
#define MogrifyImageText  "  Transforming images...  "

  Image
    *image,
    *mogrify_image;

  register int
    i;

  MonitorHandler
    handler;

  unsigned int
    number_images;

  assert(image_info != (ImageInfo *) NULL);
  assert(images != (Image **) NULL);
  image=(*images);
  for (number_images=1; image->next != (Image *) NULL; number_images++)
    image=image->next;
  ProgressMonitor(MogrifyImageText,0,number_images);
  handler=SetMonitorHandler((MonitorHandler) NULL);
  MogrifyImage(image_info,argc,argv,images);
  (void) SetMonitorHandler(handler);
  image=(*images);
  mogrify_image=(*images)->next;
  if (image_info->verbose)
    DescribeImage(image,stdout,False);
  for (i=1; mogrify_image != (Image *) NULL; i++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    MogrifyImage(image_info,argc,argv,&mogrify_image);
    image->next=mogrify_image;
    image->next->previous=image;
    image=image->next;
    if (image_info->verbose)
      DescribeImage(mogrify_image,stdout,False);
    mogrify_image=mogrify_image->next;
    (void) SetMonitorHandler(handler);
    ProgressMonitor(MogrifyImageText,i,number_images);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o n t a g e I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MontageImages creates a composite image by combining several
%  separate images.
%
%  The format of the MontageImages routine is:
%
%      MontageImages(images,montage_info)
%
%  A description of each parameter follows:
%
%    o images: Specifies a pointer to an array of Image structures.
%
%    o montage_info: Specifies a pointer to a MontageInfo structure.
%
%
*/

static void FormatLabel(ImageInfo *image_info,char *label,
  const unsigned int width,unsigned int *font_height)
{
  Image
    *image;

  MonitorHandler
    handler;

  register char
    *p,
    *q;

  if (label == (const char *) NULL)
    return;
  if (*label == '\0')
    return;
  if (strchr(label,'\n') != (char *) NULL)
    return;
  /*
    Format label to fit within a specified width.
  */
  handler=SetMonitorHandler((MonitorHandler) NULL);
  p=label;
  for (q=p+1; *q != '\0'; q++)
  {
    (void) strcpy(image_info->filename,"label:");
    (void) strncat(image_info->filename+6,p,(int) (q-p));
    image=ReadImage(image_info);
    if (image == (Image *) NULL)
      break;
    if (image->columns > width)
      {
        while (!isspace((int) (*q)) && (q > p))
          q--;
        if (q == p)
          break;
        *q='\n';
        p=q+1;
      }
    if (image->rows > *font_height)
      *font_height=image->rows;
    DestroyImage(image);
  }
  (void) SetMonitorHandler(handler);
}

static int SceneCompare(const void *x,const void *y)
{
  Image
    **image_1,
    **image_2;

  image_1=(Image **) x;
  image_2=(Image **) y;
  return((int) (*image_1)->scene-(int) (*image_2)->scene);
}

Export Image *MontageImages(const Image *images,const MontageInfo *montage_info)
{
#define MontageImageText  "  Creating visual image directory...  "
#define TileImageText  "  Creating image tiles...  "

  AnnotateInfo
    annotate_info;

  char
    geometry[MaxTextExtent];

  FrameInfo
    frame_info;

  Image
    *image,
    **image_list,
    **master_list,
    *montage_image,
    *texture,
    *tiled_image;

  ImageInfo
    *local_info;

  int
    x,
    x_offset,
    y,
    y_offset;

  MonitorHandler
    handler;

  register int
    i;

  register RunlengthPacket
    *p;

  RectangleInfo
    bounding_box,
    tile_info;

  unsigned int
    border_width,
    bevel_width,
    concatenate,
    count,
    font_height,
    height,
    images_per_page,
    max_height,
    number_images,
    number_lines,
    tile,
    tiles,
    tiles_per_column,
    tiles_per_row,
    tiles_per_page,
    title_offset,
    total_tiles,
    width;

  assert(images != (Image *) NULL);
  assert(montage_info != (MontageInfo *) NULL);
  /*
    Convert image list to an image group.
  */
  image_list=ListToGroupImage(images,&number_images);
  if (image_list == (Image **) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to montage image_list",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  master_list=image_list;
  /*
    Create image tiles.
  */
  for (tile=0; tile < number_images; tile++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    width=image_list[tile]->columns;
    height=image_list[tile]->rows;
    x=0;
    y=0;
    (void) ParseImageGeometry(montage_info->geometry,&x,&y,&width,&height);
    image_list[tile]->orphan=True;
    tiled_image=ZoomImage(image_list[tile],width,height);
    image_list[tile]->orphan=False;
    if (tiled_image == (Image *) NULL)
      {
        for (i=0; i < (int) tile; i++)
          DestroyImage(image_list[i]);
        (void) SetMonitorHandler(handler);
        return((Image *) NULL);
      }
    image_list[tile]=tiled_image;
    (void) SetMonitorHandler(handler);
    ProgressMonitor(TileImageText,tile,number_images);
  }
  /*
    Sort image_list by increasing tile number.
  */
  for (tile=0; tile < number_images; tile++)
    if (image_list[tile]->scene == 0)
      break;
  if (tile == number_images)
    qsort((void *) image_list,number_images,sizeof(Image *),
      (int (*)(const void *, const void *)) SceneCompare);
  /*
    Determine tiles per row and column.
  */
  tiles_per_row=1;
  tiles_per_column=1;
  while ((tiles_per_row*tiles_per_column) < number_images)
  {
    tiles_per_row++;
    tiles_per_column++;
  }
  if (montage_info->tile != (char *) NULL)
    {
      tiles_per_column=number_images;
      x=0;
      y=0;
      (void) XParseGeometry(montage_info->tile,&x,&y,&tiles_per_row,
        &tiles_per_column);
    }
  /*
    Determine tile sizes.
  */
  border_width=montage_info->border_width;
  bevel_width=0;
  if (montage_info->frame != (char *) NULL)
    {
      int
        flags;

      frame_info.width=0;
      frame_info.height=0;
      frame_info.outer_bevel=0;
      frame_info.inner_bevel=0;
      flags=XParseGeometry(montage_info->frame,&frame_info.outer_bevel,
        &frame_info.inner_bevel,&frame_info.width,&frame_info.height);
      if ((flags & HeightValue) == 0)
        frame_info.height=frame_info.width;
      if ((flags & XValue) == 0)
        frame_info.outer_bevel=(frame_info.width >> 2)+1;
      if ((flags & YValue) == 0)
        frame_info.inner_bevel=frame_info.outer_bevel;
      frame_info.x=frame_info.width;
      frame_info.y=frame_info.height;
      bevel_width=Max(frame_info.inner_bevel,frame_info.outer_bevel);
      border_width=Max(frame_info.width,frame_info.height);
    }
  tile_info.x=montage_info->border_width;
  tile_info.y=montage_info->border_width;
  tile_info.width=image_list[0]->columns;
  tile_info.height=image_list[0]->rows;
  concatenate=False;
  if (montage_info->geometry != (char *) NULL)
    {
      int
        flags;

      /*
        Initialize tile geometry.
      */
      (void) strcpy(geometry,montage_info->geometry);
      tile_info.x=0;
      tile_info.y=0;
      if (strchr(geometry,'%') == (char *) NULL)
        flags=GetGeometry(geometry,&tile_info.x,&tile_info.y,
          &tile_info.width,&tile_info.height);
      else
        flags=ParseImageGeometry(geometry,&tile_info.x,&tile_info.y,
          &tile_info.width,&tile_info.height);
      if ((tile_info.x == 0) && (tile_info.y == 0))
        concatenate=!((flags & WidthValue) || (flags & HeightValue));
      if (tile_info.x < 0)
        tile_info.x=0;
      if (tile_info.y < 0)
        tile_info.y=0;
    }
  for (tile=1; tile < number_images; tile++)
  {
    if (image_list[tile]->columns > tile_info.width)
      tile_info.width=image_list[tile]->columns;
    if (image_list[tile]->rows > tile_info.height)
      tile_info.height=image_list[tile]->rows;
  }
  /*
    Initialize annotate info.
  */
  local_info=CloneImageInfo((ImageInfo *) NULL);
  (void) CloneString(&local_info->pen,montage_info->pen);
  (void) CloneString(&local_info->font,montage_info->font);
  local_info->pointsize=montage_info->pointsize;
  (void) CloneString(&local_info->background_color,
    montage_info->background_color);
  (void) CloneString(&local_info->border_color,montage_info->border_color);
  GetAnnotateInfo(local_info,&annotate_info);
  annotate_info.gravity=NorthGravity;
  texture=(Image *) NULL;
  if (montage_info->texture != (char *) NULL)
    {
      (void) strcpy(local_info->filename,montage_info->texture);
      texture=ReadImage(local_info);
    }
  /*
    Initialize font info.
  */
  font_height=annotate_info.bounds.height;
  FormatLabel(local_info,montage_info->title,((tile_info.width+
    (border_width << 1))*Min(number_images,tiles_per_column)) >> 1,
    &font_height);
  for (tile=0; tile < number_images; tile++)
    FormatLabel(local_info,image_list[tile]->label,tile_info.width+
      (border_width << 1),&font_height);
  /*
    Determine the number of lines in an image label.
  */
  title_offset=0;
  if (montage_info->title != (char *) NULL)
    title_offset=((font_height*MultilineCensus(montage_info->title)) << 1)+
      (tile_info.y << 1);
  number_lines=0;
  for (tile=0; tile < number_images; tile++)
    if (MultilineCensus(image_list[tile]->label) > (int) number_lines)
      number_lines=MultilineCensus(image_list[tile]->label);
  /*
    Allocate image structure.
  */
  montage_image=AllocateImage(local_info);
  DestroyImageInfo(local_info);
  if (montage_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to montage image_list",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  montage_image->scene=1;
  images_per_page=(number_images-1)/(tiles_per_row*tiles_per_column)+1;
  tiles=0;
  total_tiles=number_images;
  for (i=0; i < (int) images_per_page; i++)
  {
    /*
      Determine bounding box.
    */
    tiles_per_page=Min(number_images,tiles_per_row*tiles_per_column);
    x_offset=0;
    y_offset=title_offset;
    max_height=0;
    bounding_box.width=0;
    bounding_box.height=0;
    for (tile=0; tile < tiles_per_page; tile++)
    {
      width=concatenate ? image_list[tile]->columns : tile_info.width;
      x_offset+=width+(tile_info.x+border_width)*2;
      if (x_offset > (int) bounding_box.width)
        bounding_box.width=x_offset;
      if (image_list[tile]->rows > max_height)
        max_height=image_list[tile]->rows;
      if (((tile+1) == tiles_per_page) || (((tile+1) % tiles_per_row) == 0))
        {
          x_offset=0;
          height=concatenate ? max_height : tile_info.height;
          y_offset+=height+(tile_info.y+border_width)*2+(font_height+4)*
            number_lines+(montage_info->shadow ? 4 : 0)+(concatenate ? 0 : 2);
          if (y_offset > (int) bounding_box.height)
            bounding_box.height=y_offset;
          max_height=0;
        }
    }
    /*
      Initialize Image structure.
    */
    (void) strcpy(montage_image->filename,montage_info->filename);
    montage_image->columns=bounding_box.width;
    montage_image->rows=bounding_box.height;
    montage_image->packets=montage_image->columns*montage_image->rows;
    montage_image->pixels=(RunlengthPacket *) AllocateMemory((unsigned int)
      montage_image->packets*sizeof(RunlengthPacket));
    if (montage_image->pixels == (RunlengthPacket *) NULL)
      {
        MagickWarning(ResourceLimitWarning,"Unable to montage image_list",
          "Memory allocation failed");
        DestroyImages(montage_image);
        return((Image *) NULL);
      }
    /*
      Set montage geometry.
    */
    montage_image->montage=(char *) AllocateMemory(MaxTextExtent*sizeof(char));
    count=1;
    for (tile=0; tile < tiles_per_page; tile++)
      count+=Extent(image_list[tile]->filename)+1;
    montage_image->directory=(char *) AllocateMemory(count*sizeof(char));
    if ((montage_image->montage == (char *) NULL) ||
        (montage_image->directory == (char *) NULL))
      {
        MagickWarning(ResourceLimitWarning,"Unable to montage image_list",
          "Memory allocation failed");
        DestroyImages(montage_image);
        return((Image *) NULL);
      }
    x_offset=0;
    y_offset=title_offset;
    FormatString(montage_image->montage,"%dx%d%+d%+d",
      (int) (tile_info.width+(tile_info.x+border_width)*2),
      (int) (tile_info.height+(tile_info.y+border_width)*2+(font_height+4)*
      number_lines+(montage_info->shadow ? 4 : 0)),x_offset,y_offset);
    *montage_image->directory='\0';
    for (tile=0; tile < tiles_per_page; tile++)
    {
      (void) strcat(montage_image->directory,image_list[tile]->filename);
      (void) strcat(montage_image->directory,"\n");
    }
    /*
      Initialize montage image to background color.
    */
    SetImage(montage_image);
    handler=SetMonitorHandler((MonitorHandler) NULL);
    if (texture != (Image *) NULL)
      TextureImage(montage_image,texture);
    if (montage_info->title != (char *) NULL)
      {
        /*
          Annotate composite image with title.
        */
        FormatString(geometry,"%ux%u%+d%+d",montage_image->columns,
          font_height << 1,0,tile_info.y+4);
        (void) CloneString(&annotate_info.geometry,geometry);
        (void) CloneString(&annotate_info.text,montage_info->title);
        AnnotateImage(montage_image,&annotate_info);
      }
    (void) SetMonitorHandler(handler);
    /*
      Copy tile image_list to the composite image.
    */
    x_offset=tile_info.x;
    y_offset=title_offset+tile_info.y;
    max_height=0;
    for (tile=0; tile < tiles_per_page; tile++)
    {
      /*
        Copy this tile to the composite image.
      */
      handler=SetMonitorHandler((MonitorHandler) NULL);
      image=image_list[tile];
      width=concatenate ? image->columns : tile_info.width;
      if (image->rows > max_height)
        max_height=image->rows;
      height=concatenate ? max_height : tile_info.height;
      if (border_width != 0)
        {
          Image
            *bordered_image;

          RectangleInfo
            border_info;

          /*
            Put a border around the image.
          */
          border_info.width=border_width;
          border_info.height=border_width;
          if (montage_info->frame != (char *) NULL)
            {
              border_info.width=(width-image->columns+1) >> 1;
              border_info.height=(height-image->rows+1) >> 1;
            }
          image->orphan=True;
          bordered_image=BorderImage(image,&border_info);
          image->orphan=False;
          if (bordered_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=bordered_image;
            }
        }
      /*
        Gravitate image as specified by the tile gravity.
      */
      switch (montage_info->gravity)
      {
        case NorthWestGravity:
        {
          x=0;
          y=0;
          break;
        }
        case NorthGravity:
        {
          x=((width+(border_width << 1))-image->columns) >> 1;
          y=0;
          break;
        }
        case NorthEastGravity:
        {
          x=(width+(border_width << 1))-image->columns;
          y=0;
          break;
        }
        case WestGravity:
        {
          x=0;
          y=((height+(border_width << 1))-image->rows) >> 1;
          break;
        }
        case ForgetGravity:
        case StaticGravity:
        case CenterGravity:
        default:
        {
          x=((width+(border_width << 1))-image->columns) >> 1;
          y=((height+(border_width << 1))-image->rows) >> 1;
          break;
        }
        case EastGravity:
        {
          x=(width+(border_width << 1))-image->columns;
          y=((height+(border_width << 1))-image->rows) >> 1;
          break;
        }
        case SouthWestGravity:
        {
          x=0;
          y=(height+(border_width << 1))-image->rows;
          break;
        }
        case SouthGravity:
        {
          x=((width+(border_width << 1))-image->columns) >> 1;
          y=(height+(border_width << 1))-image->rows;
          break;
        }
        case SouthEastGravity:
        {
          x=(width+(border_width << 1))-image->columns;
          y=(height+(border_width << 1))-image->rows;
          break;
        }
      }
      if ((montage_info->frame != (char *) NULL) && (bevel_width != 0))
        {
          FrameInfo
            tile_info;

          Image
            *framed_image;

          /*
            Put an ornamental border around this tile.
          */
          tile_info=frame_info;
          tile_info.width=width+(frame_info.width << 1);
          tile_info.height=height+(frame_info.height << 1)+(font_height+4)*
            MultilineCensus(image->label);
          image->orphan=True;
          framed_image=FrameImage(image,&tile_info);
          image->orphan=False;
          if (framed_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=framed_image;
            }
          x=0;
          y=0;
        }
      if (Latin1Compare(image->magick,"NULL") != 0)
        {
          /*
            Composite background image with tile image.
          */
          if (image->matte)
            CompositeImage(montage_image,montage_info->compose,image,x_offset+x,
              y_offset+y);
          else
            CompositeImage(montage_image,ReplaceCompositeOp,image,x_offset+x,
              y_offset+y);
          montage_image->matte=False;
          if (montage_info->shadow)
            {
              register int
                columns,
                rows;

              /*
                Put a shadow under the tile to show depth.
              */
              for (rows=0; rows < ((int) image->rows-4); rows++)
              {
                p=montage_image->pixels+montage_image->columns*
                  (y_offset+y+rows+4)+x_offset+x+image->columns;
                for (columns=0; columns < Min(tile_info.x,4); columns++)
                {
                  if (p >= (montage_image->pixels+montage_image->packets))
                    continue;
                  Modulate(0.0,0.0,-25.0+4*columns,&p->red,&p->green,&p->blue);
                  p++;
                }
              }
              for (rows=0; rows < Min(tile_info.y,4); rows++)
              {
                p=montage_image->pixels+montage_image->columns*
                  (y_offset+y+image->rows+rows)+x_offset+x+4;
                for (columns=0; columns < (int) image->columns; columns++)
                {
                  if (p >= (montage_image->pixels+montage_image->packets))
                    continue;
                  Modulate(0.0,0.0,-25.0+4*rows,&p->red,&p->green,&p->blue);
                  p++;
                }
              }
            }
          if (image->label != (char *) NULL)
            {
              /*
                Annotate composite image tile with label.
              */
              FormatString(geometry,"%ux%u%+d%+d",
                (montage_info->frame ? image->columns : width)-
                (border_width << 1),font_height,(int) (x_offset+border_width),
                (int) (montage_info->frame ? y_offset+height+
                (border_width << 1)-bevel_width-2 : y_offset+tile_info.height+
                (border_width << 1)+(montage_info->shadow ? 4 : 0)+2));
              (void) CloneString(&annotate_info.geometry,geometry);
              (void) CloneString(&annotate_info.text,image->label);
              AnnotateImage(montage_image,&annotate_info);
            }
        }
      x_offset+=width+(tile_info.x+border_width)*2;
      if (((tile+1) == tiles_per_page) || (((tile+1) % tiles_per_row) == 0))
        {
          x_offset=tile_info.x;
          y_offset+=height+(tile_info.y+border_width)*2+(font_height+4)*
            number_lines+(montage_info->shadow ? 4 : 0);
          max_height=0;
        }
      DestroyImage(image);
      (void) SetMonitorHandler(handler);
      ProgressMonitor(MontageImageText,tiles,total_tiles);
      tiles++;
    }
    CondenseImage(montage_image);
    if ((i+1) < (int) images_per_page)
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage((ImageInfo *) NULL,montage_image);
        if (montage_image->next == (Image *) NULL)
          {
            DestroyImages(montage_image);
            return((Image *) NULL);
          }
        montage_image=montage_image->next;
        image_list+=tiles_per_page;
        number_images-=tiles_per_page;
      }
  }
  if (texture != (Image *) NULL)
    FreeMemory((char *) texture);
  FreeMemory((char *) master_list);
  while (montage_image->previous != (Image *) NULL)
    montage_image=montage_image->previous;
  return(montage_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o r p h I m a g e s                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MorphImages morphs a set of images.  both the image pixels and size
%  are linearly interpolated to give the appearance of a meta-morphosis from
%  one image to the next.
%
%  The format of the MorphImage routine is:
%
%      morphed_image=MorphImages(images,number_frames)
%
%  A description of each parameter follows:
%
%    o morphed_image: Method MorphImages returns an image sequence that
%      has linearly interpolated pixels and size between two input images.
%
%    o images: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o number_frames:  This unsigned integer reflects the number of in-between
%      images to generate.  The more in-between frames, the smoother
%      the morph.
%
%
*/
Export Image *MorphImages(Image *images,const unsigned int number_frames)
{
#define MorphImageText  "  Morphing image sequence...  "

  double
    alpha,
    beta;

  Image
    *image,
    *morphed_image,
    *morphed_images;

  MonitorHandler
    handler;

  register int
    i,
    j;

  register RunlengthPacket
    *p,
    *q;

  unsigned int
    scene;

  assert(images != (Image *) NULL);
  if (images->next == (Image *) NULL)
    {
      MagickWarning(OptionWarning,"Unable to morph image",
        "image sequence required");
      return((Image *) NULL);
    }
  /*
    Clone first frame in sequence.
  */
  ((Image *) images)->orphan=True;
  morphed_images=CloneImage(images,images->columns,images->rows,True);
  ((Image *) images)->orphan=False;
  if (morphed_images == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to morph image sequence",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Morph image.
  */
  scene=0;
  for (image=images; image->next != (Image *) NULL; image=image->next)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    for (i=0; i < (int) number_frames; i++)
    {
      beta=(double) (i+1.0)/(number_frames+1.0);
      alpha=1.0-beta;
      ((Image *) image)->orphan=True;
      morphed_images->next=ZoomImage(image,
        (unsigned int) (alpha*image->columns+beta*image->next->columns+0.5),
        (unsigned int) (alpha*image->rows+beta*image->next->rows+0.5));
      ((Image *) image)->orphan=False;
      if (morphed_images->next == (Image *) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Unable to morph image sequence",
            "Memory allocation failed");
          break;
        }
      morphed_images->next->previous=morphed_images;
      morphed_images=morphed_images->next;
      image->next->orphan=True;
      morphed_image=ZoomImage(image->next,morphed_images->columns,
        morphed_images->rows);
      image->next->orphan=False;
      if (morphed_image == (Image *) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Unable to morph image sequence",
            "Memory allocation failed");
          break;
        }
      if (!UncondenseImage(morphed_images) || !UncondenseImage(morphed_image))
        {
          MagickWarning(ResourceLimitWarning,"Unable to morph image sequence",
            "Memory allocation failed");
          break;
        }
      morphed_images->class=DirectClass;
      p=morphed_image->pixels;
      q=morphed_images->pixels;
      for (j=0; j < (int) morphed_images->packets; j++)
      {
        q->red=(Quantum) (alpha*q->red+beta*p->red+0.5);
        q->green=(Quantum) (alpha*q->green+beta*p->green+0.5);
        q->blue=(Quantum) (alpha*q->blue+beta*p->blue+0.5);
        q->index=(unsigned short) (alpha*q->index+beta*p->index+0.5);
        p++;
        q++;
      }
      DestroyImage(morphed_image);
      CondenseImage(morphed_images);
    }
    /*
      Clone last frame in sequence.
    */
    image->next->orphan=True;
    morphed_images->next=
      CloneImage(image->next,image->next->columns,image->next->rows,True);
    image->next->orphan=False;
    if (morphed_images->next == (Image *) NULL)
      {
        MagickWarning(ResourceLimitWarning,"Unable to morph image sequence",
          "Memory allocation failed");
        break;
      }
    morphed_images->next->previous=morphed_images;
    morphed_images=morphed_images->next;
    (void) SetMonitorHandler(handler);
    ProgressMonitor(MorphImageText,scene,GetNumberScenes(images));
    scene++;
  }
  while (morphed_images->previous != (Image *) NULL)
    morphed_images=morphed_images->previous;
  if (image->next != (Image *) NULL)
    {
      DestroyImages(morphed_images);
      return((Image *) NULL);
    }
  return(morphed_images);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     N e g a t e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NegateImage negates the colors in the reference image.  The
%  Grayscale option means that only grayscale values within the image are
%  negated.
%
%  The format of the NegateImage routine is:
%
%      NegateImage(image,grayscale)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export void NegateImage(Image *image,const unsigned int grayscale)
{
#define NegateImageText  "  Negating the image colors...  "

  register int
    i;

  register RunlengthPacket
    *p;

  assert(image != (Image *) NULL);
  image->tainted=True;
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Negate DirectClass packets.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        if (grayscale)
          if ((p->red != p->green) || (p->green != p->blue))
            {
              p++;
              continue;
            }
        p->red=(~p->red);
        p->green=(~p->green);
        p->blue=(~p->blue);
        p->index=(~p->index);
        p++;
        if (QuantumTick(i,image->packets))
          ProgressMonitor(NegateImageText,i,image->packets);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Negate PseudoClass packets.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        if (grayscale)
          if ((image->colormap[i].red != image->colormap[i].green) ||
              (image->colormap[i].green != image->colormap[i].blue))
            continue;
        image->colormap[i].red=(~image->colormap[i].red);
        image->colormap[i].green=(~image->colormap[i].green);
        image->colormap[i].blue=(~image->colormap[i].blue);
      }
      SyncImage(image);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     N o r m a l i z e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NormalizeImage normalizes the pixel values to span the full
%  range of color values.  This is a contrast enhancement technique.
%
%  The format of the NormalizeImage routine is:
%
%      NormalizeImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export void NormalizeImage(Image *image)
{
#define NormalizeImageText  "  Normalizing image...  "

  int
    *histogram,
    threshold_intensity;

  Quantum
    gray_value,
    *normalize_map;

  register int
    i,
    intensity;

  register RunlengthPacket
    *p;

  unsigned int
    high,
    low;

  /*
    Allocate histogram and normalize map.
  */
  assert(image != (Image *) NULL);
  histogram=(int *) AllocateMemory((MaxRGB+1)*sizeof(int));
  normalize_map=(Quantum *) AllocateMemory((MaxRGB+1)*sizeof(Quantum));
  if ((histogram == (int *) NULL) || (normalize_map == (Quantum *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Unable to normalize image",
        "Memory allocation failed");
      return;
    }
  /*
    Form histogram.
  */
  for (i=0; i <= MaxRGB; i++)
    histogram[i]=0;
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    gray_value=Intensity(*p);
    histogram[gray_value]+=p->length+1;
    p++;
  }
  /*
    Find the histogram boundaries by locating the 1 percent levels.
  */
  threshold_intensity=(image->columns*image->rows)/100;
  intensity=0;
  for (low=0; low < MaxRGB; low++)
  {
    intensity+=histogram[low];
    if (intensity > threshold_intensity)
      break;
  }
  intensity=0;
  for (high=MaxRGB; high != 0; high--)
  {
    intensity+=histogram[high];
    if (intensity > threshold_intensity)
      break;
  }
  if (low == high)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      intensity=0;
      for (low=0; low < MaxRGB; low++)
      {
        intensity+=histogram[low];
        if (intensity > threshold_intensity)
          break;
      }
      intensity=0;
      for (high=MaxRGB; high != 0; high--)
      {
        intensity+=histogram[high];
        if (intensity > threshold_intensity)
          break;
      }
      if (low == high)
        return;  /* zero span bound */
    }
  /*
    Stretch the histogram to create the normalized image mapping.
  */
  for (i=0; i <= MaxRGB; i++)
    if (i < (int) low)
      normalize_map[i]=0;
    else
      if (i > (int) high)
        normalize_map[i]=MaxRGB;
      else
        normalize_map[i]=(MaxRGB-1)*(i-low)/(high-low);
  /*
    Normalize the image.
  */
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Normalize DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        p->red=normalize_map[p->red];
        p->green=normalize_map[p->green];
        p->blue=normalize_map[p->blue];
        p++;
        if (QuantumTick(i,image->packets))
          ProgressMonitor(NormalizeImageText,i,image->packets);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Normalize PseudoClass image.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        image->colormap[i].red=normalize_map[image->colormap[i].red];
        image->colormap[i].green=normalize_map[image->colormap[i].green];
        image->colormap[i].blue=normalize_map[image->colormap[i].blue];
      }
      SyncImage(image);
      break;
    }
  }
  FreeMemory((char *) normalize_map);
  FreeMemory((char *) histogram);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     O p a g u e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpaqueImage changes the color of an opaque pixel to the pen color.
%
%  The format of the OpaqueImage routine is:
%
%      OpaqueImage(image,opaque_color,pen_color)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o opaque_color,
%      pen_color: A character string that contain an X11 color string.
%
%
*/
Export void OpaqueImage(Image *image,const char *opaque_color,
  const char *pen_color)
{
#define OpaqueImageText  "  Setting opaque color in the image...  "

  ColorPacket
    target;

  register int
    i;

  unsigned int
    status;

  XColor
    target_color;

  /*
    Determine RGB values of the opaque color.
  */
  assert(image != (Image *) NULL);
  status=XQueryColorDatabase(opaque_color,&target_color);
  if (status == False)
    return;
  target.red=XDownScale(target_color.red);
  target.green=XDownScale(target_color.green);
  target.blue=XDownScale(target_color.blue);
  status=XQueryColorDatabase(pen_color,&target_color);
  if (status == False)
    return;
  /*
    Make image color opaque.
  */
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      register RunlengthPacket
        *p;

      /*
        Make DirectClass image opaque.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        if (ColorMatch(*p,target,image->fuzz))
          {
            p->red=XDownScale(target_color.red);
            p->green=XDownScale(target_color.green);
            p->blue=XDownScale(target_color.blue);
          }
        p++;
        if (QuantumTick(i,image->packets))
          ProgressMonitor(OpaqueImageText,i,image->packets);
      }
      break;
    }
    case PseudoClass:
    {
      register ColorPacket
        *p;

      /*
        Make PseudoClass image opaque.
      */
      p=image->colormap;
      for (i=0; i < (int) image->colors; i++)
      {
        if (ColorMatch(*p,target,image->fuzz))
          {
            p->red=XDownScale(target_color.red);
            p->green=XDownScale(target_color.green);
            p->blue=XDownScale(target_color.blue);
          }
        p++;
        if (QuantumTick(i,image->packets))
          ProgressMonitor(OpaqueImageText,i,image->packets);
      }
      SyncImage(image);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   O p e n I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpenImage open a file associated with the image.  A file name of
%  '-' sets the file to stdin for type 'r' and stdout for type 'w'.  If the
%  filename suffix is '.gz' or '.Z', the image is decompressed for type 'r'
%  and compressed for type 'w'.  If the filename prefix is '|', it is piped
%  to or from a system command.
%
%  The format of the OpenImage routine is:
%
%      OpenImage(image_info,image,type)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%    o type: 'r' for reading; 'w' for writing.
%
*/
Export void OpenImage(const ImageInfo *image_info,Image *image,const char *type)
{
  char
    filename[MaxTextExtent];

  register char
    *p;

  assert(image_info != (ImageInfo *) NULL);
  assert(image != (Image *) NULL);
  assert(type != (char *) NULL);
  image->exempt=False;
  if (image_info->file != (FILE *) NULL)
    {
      /*
        Use previously opened filehandle.
      */
      image->file=image_info->file;
      image->exempt=True;
      return;
    }
  (void) strcpy(filename,image->filename);
  p=(char *) NULL;
  if (*filename != '|')
    {
      if ((Extent(filename) > 4) &&
          (Latin1Compare(filename+Extent(filename)-4,".pgp") == 0))
        {
          /*
            Decrypt image file with PGP encryption utilities.
          */
          if (*type == 'r')
            p=GetDelegateCommand(image_info,image,"pgp",(char *) NULL);
        }
      else
        if ((Extent(filename) > 4) &&
            (Latin1Compare(filename+Extent(filename)-4,".bz2") == 0))
          {
            /*
              Uncompress/compress image file with BZIP compress utilities.
            */
            if (*type == 'r')
              p=GetDelegateCommand(image_info,image,"bzip",(char *) NULL);
            else
              p=GetDelegateCommand(image_info,image,(char *) NULL,"bzip");
          }
        else
          if ((Extent(filename) > 3) &&
              (Latin1Compare(filename+Extent(filename)-3,".gz") == 0))
            {
              /*
                Uncompress/compress image file with GNU compress utilities.
              */
              if (*type == 'r')
                p=GetDelegateCommand(image_info,image,"zip",(char *) NULL);
              else
                p=GetDelegateCommand(image_info,image,(char *) NULL,"zip");
            }
          else
            if ((Extent(filename) > 2) &&
                (Latin1Compare(filename+Extent(filename)-2,".Z") == 0))
              {
                /*
                  Uncompress/compress image file with UNIX compress utilities.
                */
                if (*type == 'r')
                  p=GetDelegateCommand(image_info,image,"compress",
                    (char *) NULL);
                else
                  p=GetDelegateCommand(image_info,image,(char *) NULL,
                    "compress");
              }
    }
  if (p != (char *) NULL)
    {
      (void) strcpy(filename,p);
      FreeMemory((char *) p);
    }
  /*
    Open image file.
  */
  image->pipe=False;
  if (Latin1Compare(filename,"-") == 0)
    {
      image->file=(*type == 'r') ? stdin : stdout;
      image->exempt=True;
    }
  else
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
    if (*filename == '|')
      {
        char
          mode[MaxTextExtent];

        /*
          Pipe image to or from a system command.
        */
        if (*type == 'w')
          (void) signal(SIGPIPE,SIG_IGN);
        (void) strncpy(mode,type,1);
        mode[1]='\0';
        image->file=(FILE *) popen(filename+1,mode);
        image->pipe=True;
        image->exempt=True;
      }
    else
#endif
      {
        if (*type == 'w')
          {
            /*
              Form filename for multi-part images.
            */
            FormatString(filename,image->filename,image->scene);
            if (!image_info->adjoin)
              if ((image->previous != (Image *) NULL) ||
                  (image->next != (Image *) NULL))
                {
                  if ((Latin1Compare(filename,image->filename) == 0) ||
                      (strchr(filename,'%') != (char *) NULL))
                    FormatString(filename,"%.1024s.%u",filename,image->scene);
                  if (image->next != (Image *) NULL)
                    (void) strcpy(image->next->magick,image->magick);
                }
            (void) strcpy(image->filename,filename);
          }
#if defined(macintosh)
        if (*type == 'w')
          SetApplicationType(filename,image_info->magick,'8BIM');
#endif
        image->file=(FILE *) fopen(filename,type);
        if (image->file != (FILE *) NULL)
          {
            (void) fseek(image->file,0L,SEEK_END);
            image->filesize=ftell(image->file);
            (void) fseek(image->file,0L,SEEK_SET);
          }
      }
  image->status=False;
  if (*type == 'r')
    {
      image->next=(Image *) NULL;
      image->previous=(Image *) NULL;
    }
  return;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a r s e I m a g e G e o m e t r y                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ParseImageGeometry parses a geometry specification and returns the
%  width, height, x, and y values.  It also returns flags that indicates
%  which of the four values (width, height, xoffset, yoffset) were located
%  in the string, and whether the x and y values are negative.  In addition,
%  there are flags to report any meta characters (%, !, <, and >).
%
%  The format of the ParseImageGeometry routine is:
%
%      flags=ParseImageGeometry(image_geometry,x,y,width,height)
%
%  A description of each parameter follows:
%
%    o flags:  Method ParseImageGeometry returns a bitmask that indicates
%      which of the four values were located in the geometry string.
%
%    o image_geometry:  Specifies a character string representing the geometry
%      specification.
%
%    o x,y:  A pointer to an integer.  The x and y offset as determined by
%      the geometry specification is returned here.
%
%    o width,height:  A pointer to an unsigned integer.  The width and height
%      as determined by the geometry specification is returned here.
%
%
*/
Export int ParseImageGeometry(const char *geometry,int *x,int *y,
  unsigned int *width,unsigned int *height)
{
  int
    delta,
    flags;

  RectangleInfo
    media_info;

  unsigned int
    former_height,
    former_width;

  /*
    Ensure the image geometry is valid.
  */
  assert(x != (int *) NULL);
  assert(y != (int *) NULL);
  assert(width != (unsigned int *) NULL);
  assert(height != (unsigned int *) NULL);
  if ((geometry == (char *) NULL) || (*geometry == '\0'))
    return(NoValue);
  /*
    Parse geometry using XParseGeometry.
  */
  former_width=(*width);
  former_height=(*height);
  flags=GetGeometry(geometry,x,y,width,height);
  if (flags & PercentValue)
    {
      int
        count;

      double
        x_scale,
        y_scale;

      /*
        Geometry is a percentage of the image size.
      */
      x_scale=(*width);
      y_scale=(*height);
      count=sscanf(geometry,"%lfx%lf",&x_scale,&y_scale);
      if (count == 1)
        y_scale=x_scale;
      *width=Max((unsigned int) ((x_scale*former_width)/100.0),1);
      *height=Max((unsigned int) ((y_scale*former_height)/100.0),1);
      former_width=(*width);
      former_height=(*height);
    }
  if (!(flags & AspectValue))
    {
      unsigned long
        scale_factor;

      /*
        Respect aspect ratio of the image.
      */
      scale_factor=UpShift(1);
      if ((former_width*former_height) != 0)
        {
          if (((flags & WidthValue) != 0) && (flags & HeightValue) != 0)
            {
              scale_factor=UpShift(*width)/former_width;
              if (scale_factor > (UpShift(*height)/former_height))
                scale_factor=UpShift(*height)/former_height;
            }
          else
            if ((flags & WidthValue) != 0)
              scale_factor=UpShift(*width)/former_width;
            else
              scale_factor=UpShift(*height)/former_height;
        }
      *width=Max(DownShift(former_width*scale_factor),1);
      *height=Max(DownShift(former_height*scale_factor),1);
    }
  if ((flags & XValue) == 0)
    *width-=(*x) << 1;
  if ((flags & YValue) == 0)
    *height-=(*y) << 1;
  if (flags & GreaterValue)
    {
      if (former_width < *width)
        *width=former_width;
      if (former_height < *height)
        *height=former_height;
    }
  if (flags & LessValue)
    {
      if (former_width > *width)
        *width=former_width;
      if (former_height > *height)
        *height=former_height;
    }
  media_info.width=(*width);
  media_info.height=(*height);
  media_info.x=(*x);
  media_info.y=(*y);
  (void) GetGeometry(geometry,&media_info.x,&media_info.y,&media_info.width,
    &media_info.height);
  if ((flags & XValue) == 0)
    {
      /*
        Center image in the X direction.
      */
      delta=media_info.width-(*width);
      if (delta >= 0)
        *x=delta >> 1;
    }
  else
    if ((flags & XNegative) != 0)
      *x+=media_info.width-(*width);
  if ((flags & YValue) == 0)
    {
      /*
        Center image in the Y direction.
      */
      delta=media_info.height-(*height);
      if (delta >= 0)
        *y=delta >> 1;
    }
  else
    if ((flags & YNegative) != 0)
      *y+=media_info.height-(*height);
  if (flags & GreaterValue)
    {
      if ((*width+((*x) << 1)) > media_info.width)
        *width=media_info.width-((*x) << 1);
      if ((*height+((*y) << 1)) > media_info.height)
        *height=media_info.height-((*y) << 1);
    }
  return(flags);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i n g I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PingImage returns the image size in bytes if it exists and can be
%  read (at %  least up until it reveals it's size).  The width and height of
%  the image is returned as well.  Note, only the first image in a multi-frame
%  image file is pinged.
%
%  The format of the PingImage routine is:
%
%      filesize=PingImage(image_info,columns,rows)
%
%  A description of each parameter follows:
%
%    o filesize: Method PingImage returns the image size in bytes if the
%      image file exists and it size can be determined otherwise 0.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o columns: An unsigned integer pointer that specifies the number of
%      columns in the image.
%
%    o rows: An unsigned integer pointer that specifies the number of rows in
%      the image.
%
%
*/
Export unsigned int PingImage(ImageInfo *image_info,unsigned int *columns,
  unsigned int *rows)
{
  Image
    *image;

  ImageInfo
    *ping_info;

  unsigned int
    filesize;

  *columns=0;
  *rows=0;
  ping_info=CloneImageInfo(image_info);
  ping_info->ping=True;
  ping_info->verbose=False;
  ping_info->subimage=0;
  ping_info->subrange=0;
  image=ReadImage(ping_info);
  DestroyImageInfo(ping_info);
  (void) strcpy(image_info->magick,ping_info->magick);
  if (image == (Image *) NULL)
    return(0);
  if (image_info->verbose)
    DescribeImage(image,stdout,False);
  *columns=image->columns;
  *rows=image->rows;
  filesize=image->filesize;
  DestroyImages(image);
  return(filesize);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     R G B T r a n s f o r m I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RGBTransformImage converts the reference image from RGB to
%  an alternate colorspace.  The transformation matrices are not the standard
%  ones: the weights are rescaled to normalized the range of the transformed
%  values to be [0..MaxRGB].
%
%  The format of the RGBTransformImage routine is:
%
%      RGBTransformImage(image,colorspace)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o colorspace: An unsigned integer value that indicates which colorspace
%      to transform the image.
%
%
*/
Export void RGBTransformImage(Image *image,const ColorspaceType colorspace)
{
#define RGBTransformImageText  "  Transforming image colors...  "
#define X 0
#define Y (MaxRGB+1)
#define Z (MaxRGB+1)*2

  long
    tx,
    ty,
    tz,
    *x,
    *y,
    *z;

  Quantum
    *range_table;

  register int
    blue,
    green,
    i,
    red;

  register Quantum
    *range_limit;

  register RunlengthPacket
    *p;

  assert(image != (Image *) NULL);
  if ((colorspace == RGBColorspace) || (colorspace == TransparentColorspace))
    return;
  if (colorspace == CMYKColorspace)
    {
      Quantum
        black,
        cyan,
        magenta,
        yellow;

      /*
        Convert RGB to CMYK colorspace.
      */
      image->colorspace=CMYKColorspace;
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        cyan=p->red;
        magenta=p->green;
        yellow=p->blue;
        black=MaxRGB-cyan;
        if (magenta < black)
          black=magenta;
        if (yellow < black)
          black=yellow;
        p->red=cyan;
        p->green=magenta;
        p->blue=yellow;
        p->index=black;
        p++;
      }
      return;
    }
  if (colorspace == GRAYColorspace)
    {
      /*
        Return if the image is already gray_scale.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        if ((p->red != p->green) || (p->green != p->blue))
          break;
        p++;
      }
      if (i == (int) image->packets)
        return;
    }
  /*
    Allocate the tables.
  */
  x=(long *) AllocateMemory(3*(MaxRGB+1)*sizeof(long));
  y=(long *) AllocateMemory(3*(MaxRGB+1)*sizeof(long));
  z=(long *) AllocateMemory(3*(MaxRGB+1)*sizeof(long));
  range_table=(Quantum *) AllocateMemory(4*(MaxRGB+1)*sizeof(Quantum));
  if ((x == (long *) NULL) || (y == (long *) NULL) ||
      (z == (long *) NULL) || (range_table == (Quantum *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Unable to transform color space",
        "Memory allocation failed");
      return;
    }
  /*
    Pre-compute conversion tables.
  */
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(Quantum) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  for (i=0; i <= MaxRGB; i++)
    range_table[i+(MaxRGB+1)*3]=MaxRGB;
  range_limit=range_table+(MaxRGB+1);
  tx=0;
  ty=0;
  tz=0;
  switch (colorspace)
  {
    case GRAYColorspace:
    {
      /*
        Initialize GRAY tables:

          G = 0.29900*R+0.58700*G+0.11400*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29900)*i;
        y[i+X]=UpShifted(0.58700)*i;
        z[i+X]=UpShifted(0.11400)*i;
        x[i+Y]=UpShifted(0.29900)*i;
        y[i+Y]=UpShifted(0.58700)*i;
        z[i+Y]=UpShifted(0.11400)*i;
        x[i+Z]=UpShifted(0.29900)*i;
        y[i+Z]=UpShifted(0.58700)*i;
        z[i+Z]=UpShifted(0.11400)*i;
      }
      break;
    }
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          I1 = 0.33333*R+0.33334*G+0.33333*B
          I2 = 0.50000*R+0.00000*G-0.50000*B
          I3 =-0.25000*R+0.50000*G-0.25000*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.33333)*i;
        y[i+X]=UpShifted(0.33334)*i;
        z[i+X]=UpShifted(0.33333)*i;
        x[i+Y]=UpShifted(0.50000)*i;
        y[i+Y]=0;
        z[i+Y]=(-UpShifted(0.50000))*i;
        x[i+Z]=(-UpShifted(0.25000))*i;
        y[i+Z]=UpShifted(0.50000)*i;
        z[i+Z]=(-UpShifted(0.25000))*i;
      }
      break;
    }
    case sRGBColorspace:
    {
      /*
        Initialize sRGB tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B

        sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      ty=UpShifted((unsigned int) UpScale(156));
      tz=UpShifted((unsigned int) UpScale(137));
      for (i=0; i <= (int) (0.018*MaxRGB); i++)
      {
        x[i+X]=(long) (UpShifted(0.29900/1.3584)*0.018*MaxRGB*i);
        y[i+X]=(long) (UpShifted(0.58700/1.3584)*0.018*MaxRGB*i);
        z[i+X]=(long) (UpShifted(0.11400/1.3584)*0.018*MaxRGB*i);
        x[i+Y]=(long) ((-UpShifted(0.29900/2.2179))*0.018*MaxRGB*i);
        y[i+Y]=(long) ((-UpShifted(0.58700/2.2179))*0.018*MaxRGB*i);
        z[i+Y]=(long) (UpShifted(0.88600/2.2179)*0.018*MaxRGB*i);
        x[i+Z]=(long) (UpShifted(0.70100/1.8215)*0.018*MaxRGB*i);
        y[i+Z]=(long) ((-UpShifted(0.58700/1.8215))*0.018*MaxRGB*i);
        z[i+Z]=(long) ((-UpShifted(0.11400/1.8215))*0.018*MaxRGB*i);
      }
      for ( ; i <= MaxRGB; i++)
      {
        x[i+X]=(long) (UpShifted(0.29900/1.3584)*(1.099*i-0.099));
        y[i+X]=(long) (UpShifted(0.58700/1.3584)*(1.099*i-0.099));
        z[i+X]=(long) (UpShifted(0.11400/1.3584)*(1.099*i-0.099));
        x[i+Y]=(long) ((-UpShifted(0.29900/2.2179))*(1.099*i-0.099));
        y[i+Y]=(long) ((-UpShifted(0.58700/2.2179))*(1.099*i-0.099));
        z[i+Y]=(long) (UpShifted(0.88600/2.2179)*(1.099*i-0.099));
        x[i+Z]=(long) (UpShifted(0.70100/1.8215)*(1.099*i-0.099));
        y[i+Z]=(long) ((-UpShifted(0.58700/1.8215))*(1.099*i-0.099));
        z[i+Z]=(long) ((-UpShifted(0.11400/1.8215))*(1.099*i-0.099));
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize CIE XYZ tables:

          X = 0.412453*X+0.357580*Y+0.180423*Z
          Y = 0.212671*X+0.715160*Y+0.072169*Z
          Z = 0.019334*X+0.119193*Y+0.950227*Z
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.412453)*i;
        y[i+X]=UpShifted(0.357580)*i;
        z[i+X]=UpShifted(0.180423)*i;
        x[i+Y]=UpShifted(0.212671)*i;
        y[i+Y]=UpShifted(0.715160)*i;
        z[i+Y]=UpShifted(0.072169)*i;
        x[i+Z]=UpShifted(0.019334)*i;
        y[i+Z]=UpShifted(0.119193)*i;
        z[i+Z]=UpShifted(0.950227)*i;
      }
      break;
    }
    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          Y =  0.299000*R+0.587000*G+0.114000*B
          Cb= -0.168736*R-0.331264*G+0.500000*B
          Cr=  0.500000*R-0.418688*G-0.081312*B

        Cb and Cr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.299000)*i;
        y[i+X]=UpShifted(0.587000)*i;
        z[i+X]=UpShifted(0.114000)*i;
        x[i+Y]=(-UpShifted(0.16873))*i;
        y[i+Y]=(-UpShifted(0.331264))*i;
        z[i+Y]=UpShifted(0.500000)*i;
        x[i+Z]=UpShifted(0.500000)*i;
        y[i+Z]=(-UpShifted(0.418688))*i;
        z[i+Z]=(-UpShifted(0.081312))*i;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Initialize YCC tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      ty=UpShifted((unsigned int) UpScale(156));
      tz=UpShifted((unsigned int) UpScale(137));
      for (i=0; i <= (int) (0.018*MaxRGB); i++)
      {
        x[i+X]=(long) (UpShifted(0.29900/1.3584)*0.018*MaxRGB*i);
        y[i+X]=(long) (UpShifted(0.58700/1.3584)*0.018*MaxRGB*i);
        z[i+X]=(long) (UpShifted(0.11400/1.3584)*0.018*MaxRGB*i);
        x[i+Y]=(long) ((-UpShifted(0.29900/2.2179))*0.018*MaxRGB*i);
        y[i+Y]=(long) ((-UpShifted(0.58700/2.2179))*0.018*MaxRGB*i);
        z[i+Y]=(long) (UpShifted(0.88600/2.2179)*0.018*MaxRGB*i);
        x[i+Z]=(long) (UpShifted(0.70100/1.8215)*0.018*MaxRGB*i);
        y[i+Z]=(long) ((-UpShifted(0.58700/1.8215))*0.018*MaxRGB*i);
        z[i+Z]=(long) ((-UpShifted(0.11400/1.8215))*0.018*MaxRGB*i);
      }
      for ( ; i <= MaxRGB; i++)
      {
        x[i+X]=(long) (UpShifted(0.29900/1.3584)*(1.099*i-0.099));
        y[i+X]=(long) (UpShifted(0.58700/1.3584)*(1.099*i-0.099));
        z[i+X]=(long) (UpShifted(0.11400/1.3584)*(1.099*i-0.099));
        x[i+Y]=(long) ((-UpShifted(0.29900/2.2179))*(1.099*i-0.099));
        y[i+Y]=(long) ((-UpShifted(0.58700/2.2179))*(1.099*i-0.099));
        z[i+Y]=(long) (UpShifted(0.88600/2.2179)*(1.099*i-0.099));
        x[i+Z]=(long) (UpShifted(0.70100/1.8215)*(1.099*i-0.099));
        y[i+Z]=(long) ((-UpShifted(0.58700/1.8215))*(1.099*i-0.099));
        z[i+Z]=(long) ((-UpShifted(0.11400/1.8215))*(1.099*i-0.099));
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          Y = 0.29900*R+0.58700*G+0.11400*B
          I = 0.59600*R-0.27400*G-0.32200*B
          Q = 0.21100*R-0.52300*G+0.31200*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29900)*i;
        y[i+X]=UpShifted(0.58700)*i;
        z[i+X]=UpShifted(0.11400)*i;
        x[i+Y]=UpShifted(0.59600)*i;
        y[i+Y]=(-UpShifted(0.27400))*i;
        z[i+Y]=(-UpShifted(0.32200))*i;
        x[i+Z]=UpShifted(0.21100)*i;
        y[i+Z]=(-UpShifted(0.52300))*i;
        z[i+Z]=UpShifted(0.31200)*i;
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize YPbPr tables:

          Y =  0.299000*R+0.587000*G+0.114000*B
          Pb= -0.168736*R-0.331264*G+0.500000*B
          Pr=  0.500000*R-0.418688*G-0.081312*B

        Pb and Pr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.299000)*i;
        y[i+X]=UpShifted(0.587000)*i;
        z[i+X]=UpShifted(0.114000)*i;
        x[i+Y]=(-UpShifted(0.168736))*i;
        y[i+Y]=(-UpShifted(0.331264))*i;
        z[i+Y]=UpShifted(0.500000)*i;
        x[i+Z]=UpShifted(0.500000)*i;
        y[i+Z]=(-UpShifted(0.418688))*i;
        z[i+Z]=(-UpShifted(0.081312))*i;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          U = -0.14740*R-0.28950*G+0.43690*B
          V =  0.61500*R-0.51500*G-0.10000*B

        U and V, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.  Note that U = 0.493*(B-Y), V = 0.877*(R-Y).
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29900)*i;
        y[i+X]=UpShifted(0.58700)*i;
        z[i+X]=UpShifted(0.11400)*i;
        x[i+Y]=(-UpShifted(0.14740))*i;
        y[i+Y]=(-UpShifted(0.28950))*i;
        z[i+Y]=UpShifted(0.43690)*i;
        x[i+Z]=UpShifted(0.61500)*i;
        y[i+Z]=(-UpShifted(0.51500))*i;
        z[i+Z]=(-UpShifted(0.10000))*i;
      }
      break;
    }
  }
  /*
    Convert from RGB.
  */
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Convert DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        red=p->red;
        green=p->green;
        blue=p->blue;
        p->red=range_limit[DownShift(x[red+X]+y[green+X]+z[blue+X]+tx)];
        p->green=range_limit[DownShift(x[red+Y]+y[green+Y]+z[blue+Y]+ty)];
        p->blue=range_limit[DownShift(x[red+Z]+y[green+Z]+z[blue+Z]+tz)];
        p++;
        if (QuantumTick(i,image->packets))
          ProgressMonitor(RGBTransformImageText,i,image->packets);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        red=image->colormap[i].red;
        green=image->colormap[i].green;
        blue=image->colormap[i].blue;
        image->colormap[i].red=
          range_limit[DownShift(x[red+X]+y[green+X]+z[blue+X]+tx)];
        image->colormap[i].green=
          range_limit[DownShift(x[red+Y]+y[green+Y]+z[blue+Y]+ty)];
        image->colormap[i].blue=
          range_limit[DownShift(x[red+Z]+y[green+Z]+z[blue+Z]+tz)];
      }
      SyncImage(image);
      break;
    }
  }
  /*
    Free allocated memory.
  */
  FreeMemory((char *) range_table);
  FreeMemory((char *) z);
  FreeMemory((char *) y);
  FreeMemory((char *) x);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R o l l I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RollImage rolls an image vertically and horizontally.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the RollImage routine is:
%
%      rolled_image=RollImage(image,x_offset,y_offset)
%
%  A description of each parameter follows:
%
%    o rolled_image: Method RollImage returns a pointer to the image after
%      rolling.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o x_offset: An integer that specifies the number of columns to roll
%      in the horizontal direction.
%
%    o y_offset: An integer that specifies the number of rows to roll in the
%      vertical direction.
%
%
*/
Export Image *RollImage(const Image *image,const int x_offset,
  const int y_offset)
{
#define RollImageText  "  Rolling image...  "

  Image
    *rolled_image;

  int
    y;

  register int
    runlength,
    x;

  register RunlengthPacket
    *p,
    *q;

  PointInfo
    offset;

  /*
    Initialize rolled image attributes.
  */
  assert(image != (Image *) NULL);
  rolled_image=CloneImage(image,image->columns,image->rows,False);
  if (rolled_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to roll image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Roll image.
  */
  offset.x=x_offset % image->columns;
  offset.y=y_offset % image->rows;
  if (offset.x < 0)
    offset.x+=image->columns;
  if (offset.y < 0)
    offset.y+=image->rows;
  p=image->pixels;
  runlength=p->length+1;
  for (y=0; y < (int) image->rows; y++)
  {
    /*
      Transfer scanline.
    */
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      q=rolled_image->pixels+(((int) offset.y+y) % image->rows)*image->columns+
        (((int) offset.x+x) % image->columns);
      *q=(*p);
      q->length=0;
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(RollImageText,y,image->rows);
  }
  return(rolled_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S a m p l e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SampleImage creates a new image that is a scaled size of an
%  existing one using pixel sampling.  It allocates the memory necessary
%  for the new Image structure and returns a pointer to the new image.
%
%  The format of the SampleImage routine is:
%
%      sampled_image=SampleImage(image,columns,rows)
%
%  A description of each parameter follows:
%
%    o sampled_image: Method SampleImage returns a pointer to the image after
%      scaling.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o columns: An integer that specifies the number of columns in the sampled
%      image.
%
%    o rows: An integer that specifies the number of rows in the sampled
%      image.
%
%
*/
Export Image *SampleImage(const Image *image,const unsigned int columns,
  const unsigned int rows)
{
#define SampleImageText  "  Sampling image...  "

  double
    scale_factor;

  Image
    *sampled_image;

  int
    y;

  register int
    i,
    runlength,
    x;

  register RunlengthPacket
    *p,
    *q,
    *s;

  RunlengthPacket
    *scanline;

  unsigned int
    *x_offset,
    *y_offset;

  assert(image != (Image *) NULL);
  if ((columns == 0) || (rows == 0))
    return((Image *) NULL);
  /*
    Initialize sampled image attributes.
  */
  sampled_image=CloneImage(image,columns,rows,False);
  if (sampled_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to sample image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scan line buffer and column offset buffers.
  */
  scanline=(RunlengthPacket *)
    AllocateMemory(image->columns*sizeof(RunlengthPacket));
  x_offset=(unsigned int *)
    AllocateMemory(sampled_image->columns*sizeof(unsigned int));
  y_offset=(unsigned int *)
    AllocateMemory(sampled_image->rows*sizeof(unsigned int));
  if ((scanline == (RunlengthPacket *) NULL) ||
      (x_offset == (unsigned int *) NULL) ||
      (y_offset == (unsigned int *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Unable to sample image",
        "Memory allocation failed");
      DestroyImage(sampled_image);
      return((Image *) NULL);
    }
  /*
    Initialize column pixel offsets.
  */
  scale_factor=(double) image->columns/sampled_image->columns;
  i=0;
  for (x=0; x < (int) sampled_image->columns; x++)
  {
    x_offset[x]=(unsigned int) ((x+1)*scale_factor-i);
    i+=x_offset[x];
  }
  /*
    Initialize row pixel offsets.
  */
  scale_factor=(double) image->rows/sampled_image->rows;
  i=0;
  for (y=0; y < (int) sampled_image->rows; y++)
  {
    y_offset[y]=(unsigned int) ((y+1)*scale_factor-i);
    i+=y_offset[y];
  }
  /*
    Preload first scanline.
  */
  p=image->pixels;
  runlength=p->length+1;
  s=scanline;
  for (x=0; x < (int) image->columns; x++)
  {
    if (runlength != 0)
      runlength--;
    else
      {
        p++;
        runlength=p->length;
      }
    *s=(*p);
    s->length=0;
    s++;
  }
  /*
    Sample each row.
  */
  p=image->pixels;
  runlength=p->length+1;
  q=sampled_image->pixels;
  for (y=0; y < (int) sampled_image->rows; y++)
  {
    for (i=0; i < (int) y_offset[y]; i++)
    {
      /*
        Read a scan line.
      */
      s=scanline;
      for (x=0; x < (int) image->columns; x++)
      {
        if (runlength != 0)
          runlength--;
        else
          {
            p++;
            runlength=p->length;
          }
        *s=(*p);
        s->length=0;
        s++;
      }
    }
    /*
      Sample each column.
    */
    s=scanline;
    for (x=0; x < (int) sampled_image->columns; x++)
    {
      *q=(*s);
      q++;
      s+=x_offset[x];
    }
    if (QuantumTick(y,sampled_image->rows))
      ProgressMonitor(SampleImageText,y,sampled_image->rows);
  }
  FreeMemory((char *) scanline);
  FreeMemory((char *) x_offset);
  FreeMemory((char *) y_offset);
  return(sampled_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S c a l e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ScaleImage creates a new image that is a scaled size of an
%  existing one.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.  To scale a scanline
%  from x pixels to y pixels, each new pixel represents x/y old pixels.  To
%  read x/y pixels, read (x/y rounded up) pixels but only count the required
%  fraction of the last old pixel read in your new pixel.  The remainder
%  of the old pixel will be counted in the next new pixel.
%
%  The scaling algorithm was suggested by rjohnson@shell.com and is adapted
%  from pnmscale(1) of PBMPLUS by Jef Poskanzer.
%
%  The format of the ScaleImage routine is:
%
%      scaled_image=ScaleImage(image,columns,rows)
%
%  A description of each parameter follows:
%
%    o scaled_image: Method ScaleImage returns a pointer to the image after
%      scaling.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o columns: An integer that specifies the number of columns in the scaled
%      image.
%
%    o rows: An integer that specifies the number of rows in the scaled
%      image.
%
%
*/
Export Image *ScaleImage(const Image *image,const unsigned int columns,
  const unsigned int rows)
{
#define ScaleImageText  "  Scaling image...  "

  typedef struct ScaledPacket
  {
    long
      red,
      green,
      blue,
      index;
  } ScaledPacket;

  Image
    *scaled_image;

  int
    next_row,
    number_rows;

  long
    x_scale,
    x_span;

  register int
    runlength,
    x;

  register long
    index,
    packets;

  register RunlengthPacket
    *p,
    *q;

  register ScaledPacket
    *s,
    *t;

  ScaledPacket
    *scaled_scanline,
    *scanline,
    *y_vector,
    *x_vector;

  unsigned int
    y;

  unsigned long
    blue,
    green,
    max_packets,
    red,
    scale_factor;

  assert(image != (Image *) NULL);
  if ((columns == 0) || (rows == 0))
    return((Image *) NULL);
  /*
    Initialize scaled image attributes.
  */
  scale_factor=UpShift(columns*rows)/(image->columns*image->rows);
  max_packets=Max(DownShift(image->packets*scale_factor),1);
  ((Image *) image)->orphan=True;
  scaled_image=CloneImage(image,max_packets,1,False);
  ((Image *) image)->orphan=False;
  if (scaled_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to scale image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  scaled_image->class=DirectClass;
  scaled_image->columns=columns;
  scaled_image->rows=rows;
  packets=0;
  /*
    Allocate memory.
  */
  x_vector=(ScaledPacket *) AllocateMemory(image->columns*sizeof(ScaledPacket));
  scanline=x_vector;
  if (scaled_image->rows != image->rows)
    scanline=(ScaledPacket *)
      AllocateMemory(image->columns*sizeof(ScaledPacket));
  scaled_scanline=(ScaledPacket *)
    AllocateMemory(scaled_image->columns*sizeof(ScaledPacket));
  y_vector=(ScaledPacket *) AllocateMemory(image->columns*sizeof(ScaledPacket));
  if ((x_vector == (ScaledPacket *) NULL) ||
      (scanline == (ScaledPacket *) NULL) ||
      (scaled_scanline == (ScaledPacket *) NULL) ||
      (y_vector == (ScaledPacket *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Unable to scale image",
        "Memory allocation failed");
      DestroyImage(scaled_image);
      return((Image *) NULL);
    }
  /*
    Scale image.
  */
  index=0;
  number_rows=0;
  next_row=True;
  x_scale=UpShift(scaled_image->rows)/image->rows;
  x_span=UpShift(1);
  for (x=0; x < (int) image->columns; x++)
  {
    y_vector[x].red=0;
    y_vector[x].green=0;
    y_vector[x].blue=0;
    y_vector[x].index=0;
  }
  p=image->pixels;
  runlength=p->length+1;
  q=scaled_image->pixels;
  SetRunlengthEncoder(q);
  for (y=0; y < scaled_image->rows; y++)
  {
    if (scaled_image->rows == image->rows)
      for (x=0; x < (int) image->columns; x++)
      {
        /*
          Read a new scanline.
        */
        if (runlength != 0)
          runlength--;
        else
          {
            p++;
            runlength=p->length;
          }
        x_vector[x].red=p->red;
        x_vector[x].green=p->green;
        x_vector[x].blue=p->blue;
        x_vector[x].index=p->index;
      }
    else
      {
        /*
          Scale Y direction.
        */
        while (x_scale < x_span)
        {
          if (next_row && (number_rows < (int) image->rows))
            {
              /*
                Read a new scanline.
              */
              for (x=0; x < (int) image->columns; x++)
              {
                if (runlength != 0)
                  runlength--;
                else
                  {
                    p++;
                    runlength=p->length;
                  }
                x_vector[x].red=p->red;
                x_vector[x].green=p->green;
                x_vector[x].blue=p->blue;
                x_vector[x].index=p->index;
              }
              number_rows++;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            y_vector[x].red+=x_scale*x_vector[x].red;
            y_vector[x].green+=x_scale*x_vector[x].green;
            y_vector[x].blue+=x_scale*x_vector[x].blue;
            y_vector[x].index+=x_scale*x_vector[x].index;
          }
          x_span-=x_scale;
          x_scale=UpShift(scaled_image->rows)/image->rows;
          next_row=True;
        }
        if (next_row && (number_rows < (int) image->rows))
          {
            /*
              Read a new scanline.
            */
            for (x=0; x < (int) image->columns; x++)
            {
              if (runlength != 0)
                runlength--;
              else
                {
                  p++;
                  runlength=p->length;
                }
              x_vector[x].red=p->red;
              x_vector[x].green=p->green;
              x_vector[x].blue=p->blue;
              x_vector[x].index=p->index;
            }
            number_rows++;
            next_row=False;
          }
        s=scanline;
        for (x=0; x < (int) image->columns; x++)
        {
          red=DownShift(y_vector[x].red+x_span*x_vector[x].red);
          green=DownShift(y_vector[x].green+x_span*x_vector[x].green);
          blue=DownShift(y_vector[x].blue+x_span*x_vector[x].blue);
          index=DownShift(y_vector[x].index+x_span*x_vector[x].index);
          s->red=(Quantum) (red > MaxRGB ? MaxRGB : red);
          s->green=(Quantum) (green > MaxRGB ? MaxRGB : green);
          s->blue=(Quantum) (blue > MaxRGB ? MaxRGB : blue);
          s->index=(unsigned short)
            (index > MaxColormapSize ? MaxColormapSize : index);
          s++;
          y_vector[x].red=0;
          y_vector[x].green=0;
          y_vector[x].blue=0;
          y_vector[x].index=0;
        }
        x_scale-=x_span;
        if (x_scale == 0)
          {
            x_scale=UpShift(scaled_image->rows)/image->rows;
            next_row=True;
          }
        x_span=UpShift(1);
      }
    if (scaled_image->columns == image->columns)
      {
        /*
          Transfer scanline to scaled image.
        */
        s=scanline;
        for (x=0; x < (int) scaled_image->columns; x++)
        {
          if ((s->red == q->red) && (s->green == q->green) &&
              (s->blue == q->blue) && (s->index == q->index) &&
              ((int) q->length < MaxRunlength))
            q->length++;
          else
            {
              if (packets != 0)
                q++;
              packets++;
              if (packets == (int) max_packets)
                {
                  max_packets<<=1;
                  scaled_image->pixels=(RunlengthPacket *) ReallocateMemory(
                    (char *) scaled_image->pixels,max_packets*
                    sizeof(RunlengthPacket));
                  if (scaled_image->pixels == (RunlengthPacket *) NULL)
                    {
                      MagickWarning(ResourceLimitWarning,
                        "Unable to scale image","Memory allocation failed");
                      DestroyImage(scaled_image);
                      return((Image *) NULL);
                    }
                  q=scaled_image->pixels+packets-1;
                }
              q->red=(Quantum) s->red;
              q->green=(Quantum) s->green;
              q->blue=(Quantum) s->blue;
              q->index=(unsigned short) s->index;
              q->length=0;
            }
          s++;
        }
      }
    else
      {
        int
          next_column;

        long
          y_scale,
          y_span;

        /*
          Scale X direction.
        */
        red=0;
        green=0;
        blue=0;
        next_column=False;
        y_span=UpShift(1);
        s=scanline;
        t=scaled_scanline;
        for (x=0; x < (int) image->columns; x++)
        {
          y_scale=UpShift(scaled_image->columns)/image->columns;
          while (y_scale >= y_span)
          {
            if (next_column)
              {
                red=0;
                green=0;
                blue=0;
                index=0;
                t++;
              }
            red=DownShift(red+y_span*s->red);
            green=DownShift(green+y_span*s->green);
            blue=DownShift(blue+y_span*s->blue);
            index=DownShift(index+y_span*s->index);
            t->red=(Quantum) (red > MaxRGB ? MaxRGB : red);
            t->green=(Quantum) (green > MaxRGB ? MaxRGB : green);
            t->blue=(Quantum) (blue > MaxRGB ? MaxRGB : blue);
            t->index=(unsigned short)
              (index > MaxColormapSize ? MaxColormapSize : index);
            y_scale-=y_span;
            y_span=UpShift(1);
            next_column=True;
          }
        if (y_scale > 0)
          {
            if (next_column)
              {
                red=0;
                green=0;
                blue=0;
                index=0;
                next_column=False;
                t++;
              }
            red+=y_scale*s->red;
            green+=y_scale*s->green;
            blue+=y_scale*s->blue;
            index+=y_scale*s->index;
            y_span-=y_scale;
          }
        s++;
      }
      if (y_span > 0)
        {
          s--;
          red+=y_span*s->red;
          green+=y_span*s->green;
          blue+=y_span*s->blue;
          index+=y_span*s->index;
        }
      if (!next_column)
        {
          red=DownShift(red);
          green=DownShift(green);
          blue=DownShift(blue);
          index=DownShift(index);
          t->red=(Quantum) (red > MaxRGB ? MaxRGB : red);
          t->green=(Quantum) (green > MaxRGB ? MaxRGB : green);
          t->blue=(Quantum) (blue > MaxRGB ? MaxRGB : blue);
          t->index=(unsigned short) (index > MaxRGB ? MaxRGB : index);
        }
      /*
        Transfer scanline to scaled image.
      */
      t=scaled_scanline;
      for (x=0; x < (int) scaled_image->columns; x++)
      {
        if ((t->red == q->red) && (t->green == q->green) &&
            (t->blue == q->blue) && (t->index == q->index) &&
            ((int) q->length < MaxRunlength))
          q->length++;
        else
          {
            if (packets != 0)
              q++;
            packets++;
            if (packets == (int) max_packets)
              {
                max_packets<<=1;
                scaled_image->pixels=(RunlengthPacket *) ReallocateMemory(
                  (char *) scaled_image->pixels,max_packets*
                  sizeof(RunlengthPacket));
                if (scaled_image->pixels == (RunlengthPacket *) NULL)
                  {
                    MagickWarning(ResourceLimitWarning,"Unable to scale image",
                      "Memory allocation failed");
                    DestroyImage(scaled_image);
                    return((Image *) NULL);
                  }
                q=scaled_image->pixels+packets-1;
              }
            q->red=(Quantum) t->red;
            q->green=(Quantum) t->green;
            q->blue=(Quantum) t->blue;
            q->index=(unsigned short) t->index;
            q->length=0;
          }
        t++;
      }
    }
    if (QuantumTick(y,scaled_image->rows))
      ProgressMonitor(ScaleImageText,y,scaled_image->rows);
  }
  scaled_image->packets=packets;
  scaled_image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
    scaled_image->pixels,scaled_image->packets*sizeof(RunlengthPacket));
  /*
    Free allocated memory.
  */
  FreeMemory((char *) y_vector);
  FreeMemory((char *) scaled_scanline);
  if (scanline != x_vector)
    FreeMemory((char *) scanline);
  FreeMemory((char *) x_vector);
  return(scaled_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   S e t I m a g e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetImage initializes the reference image to the background color.
%
%  The format of the SetImage routine is:
%
%      SetImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Export void SetImage(Image *image)
{
  ColorPacket
    color;

  register int
    i;

  register RunlengthPacket
    *p;

  assert(image != (Image *) NULL);
  color=image->background_color;
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    p->red=color.red;
    p->green=color.green;
    p->blue=color.blue;
    p->length=0;
    p->index=0;
    p++;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetImageInfo initializes the `magick' field of the ImageInfo
%  structure.  It is set to a type of image format based on the prefix or
%  suffix of the filename.  For example, `ps:image' returns PS indicating
%  a Postscript image.  JPEG is returned for this filename: `image.jpg'.
%  The filename prefix has precendence over the suffix.  Use an optional index
%  enclosed in brackets after a file name to specify a desired subimage of a
%  multi-resolution image format like Photo CD (e.g. img0001.pcd[4]).
%
%  The format of the SetImageInfo routine is:
%
%      SetImageInfo(image_info,rectify)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o rectify: an unsigned value other than zero rectifies the attribute for
%      multi-frame support (user may want multi-frame but image format may not
%      support it).
%
%
*/
Export void SetImageInfo(ImageInfo *image_info,const unsigned int rectify)
{
  char
    magick[MaxTextExtent];

  Image
    image;

  register char
    *p,
    *q;

  /*
    Look for 'image.format' in filename.
  */
  assert(image_info != (ImageInfo *) NULL);
  *magick='\0';
  p=image_info->filename+Extent(image_info->filename)-1;
  if (*p == ']')
    for (q=p-1; q > image_info->filename; q--)
    {
      char
        *tile;

      /*
        Look for sub-image specification (e.g. img0001.pcd[4]).
      */
      if (*q != '[')
        continue;
      if (!IsGeometry(q+1))
        break;
      tile=(char *) AllocateMemory((p-q)*sizeof(char));
      if (tile == (char *) NULL)
        break;
      (void) strncpy(tile,q+1,p-q-1);
      tile[p-q-1]='\0';
      *q='\0';
      p=q;
      (void) CloneString(&image_info->tile,tile);
      FreeMemory((char *) tile);
      if (!IsSubimage(image_info->tile,True))
        break;
      /*
        Determine sub-image range.
      */
      image_info->subimage=atoi(image_info->tile);
      image_info->subrange=atoi(image_info->tile);
      (void) sscanf(image_info->tile,"%u-%u",&image_info->subimage,
        &image_info->subrange);
      if (image_info->subrange < image_info->subimage)
        Swap(image_info->subimage,image_info->subrange);
      else
        {
          FreeMemory(image_info->tile);
          image_info->tile=(char *) NULL;
        }
      image_info->subrange-=image_info->subimage-1;
      break;
    }
  while ((*p != '.') && (p > (image_info->filename+1)))
    p--;
  if ((Latin1Compare(p,".gz") == 0) || (Latin1Compare(p,".Z") == 0) ||
      (Latin1Compare(p,".bz2") == 0))
    do
    {
      p--;
    } while ((*p != '.') && (p > (image_info->filename+1)));
  if ((*p == '.') && (Extent(p) < (int) sizeof(magick)))
    {
      /*
        User specified image format.
      */
      (void) strcpy(magick,p+1);
      for (q=magick; *q != '\0'; q++)
        if (*q == '.')
          {
            *q='\0';
            break;
          }
      Latin1Upper(magick);
      /*
        SGI and RGB are ambiguous;  TMP must be set explicitly.
      */
      if (((strncmp(image_info->magick,"SGI",3) != 0) ||
          (Latin1Compare(magick,"RGB") != 0)) &&
          (Latin1Compare(magick,"TMP") != 0))
        (void) strcpy(image_info->magick,magick);
    }
  /*
    Look for explicit 'format:image' in filename.
  */
  image_info->affirm=image_info->file != (FILE *) NULL;
  p=image_info->filename;
  while (isalnum((int) *p))
    p++;
#if defined(vms)
  if (*(p+1) == '[')
    p++;
#endif
  if ((*p == ':') && ((p-image_info->filename) < (int) sizeof(magick)))
    {
      /*
        User specified image format.
      */
      (void) strncpy(magick,image_info->filename,p-image_info->filename);
      magick[p-image_info->filename]='\0';
      Latin1Upper(magick);
#if defined(WIN32)
      if (!ImageFormatConflict(magick))
#endif
        {
          /*
            Strip off image format prefix.
          */
          p++;
          (void) strcpy(image_info->filename,p);
          if (Latin1Compare(magick,"IMPLICIT") != 0)
            {
              (void) strcpy(image_info->magick,magick);
              if (Latin1Compare(magick,"TMP") != 0)
                image_info->affirm=True;
              else
                image_info->temporary=True;
            }
        }
    }
  if (rectify)
    {
      char
        filename[MaxTextExtent];

      MagickInfo
        *magick_info;

      /*
        Rectify multi-image file support.
      */
      FormatString(filename,image_info->filename,0);
      if ((Latin1Compare(filename,image_info->filename) != 0) &&
          (strchr(filename,'%') == (char *) NULL))
        image_info->adjoin=False;
      magick_info=(MagickInfo *) GetMagickInfo(magick);
      if (magick_info != (MagickInfo *) NULL)
        image_info->adjoin&=magick_info->adjoin;
      return;
    }
  if (image_info->affirm)
    return;
  /*
    Determine the image format from the first few bytes of the file.
  */
  (void) strcpy(image.filename,image_info->filename);
  OpenImage(image_info,&image,ReadBinaryType);
  if (image.file == (FILE *) NULL)
    return;
  if (!image.exempt)
    (void) ReadData(magick,1,MaxTextExtent,image.file);
  else
    {
      FILE
        *file;

      register int
        c,
        i;

      /*
        Copy standard input or pipe to temporary file.
      */
      image_info->file=(FILE *) NULL;
      TemporaryFilename(image.filename);
      image_info->temporary=True;
      FormatString(image_info->filename,"%.1024s",image.filename);
      file=fopen(image.filename,WriteBinaryType);
      if (file == (FILE *) NULL)
        {
          MagickWarning(FileOpenWarning,"Unable to write file",image.filename);
          return;
        }
      i=0;
      for (c=fgetc(image.file); c != EOF; c=fgetc(image.file))
      {
        if (i < MaxTextExtent)
          magick[i++]=c;
        (void) putc(c,file);
      }
      (void) fclose(file);
    }
  (void) fclose(image.file);
  magick[MaxTextExtent-1]='\0';
  if (strncmp(magick,"BM",2) == 0)
    (void) strcpy(image_info->magick,"BMP");
  if (strncmp(magick,"IC",2) == 0)
    (void) strcpy(image_info->magick,"BMP");
  if (strncmp(magick,"PI",2) == 0)
    (void) strcpy(image_info->magick,"BMP");
  if (strncmp(magick,"CI",2) == 0)
    (void) strcpy(image_info->magick,"BMP");
  if (strncmp(magick,"CP",2) == 0)
    (void) strcpy(image_info->magick,"BMP");
  if (strncmp(magick,"BEGMF",3) == 0)
    (void) strcpy(image_info->magick,"CGM");
  if (strncmp(magick+128,"DICM",4) == 0)
    (void) strcpy(image_info->magick,"DCM");
  if (strncmp(magick,"\305\320\323\306",4) == 0)
    (void) strcpy(image_info->magick,"EPT");
  if (strncmp(magick,"IT0",3) == 0)
    (void) strcpy(image_info->magick,"FITS");
  if (strncmp(magick,"\261\150\336\72",4) == 0)
    (void) strcpy(image_info->magick,"DCX");
  if (strncmp(magick,"digraph",7) == 0)
    (void) strcpy(image_info->magick,"DOT");
  if (strncmp(magick,"DFAX",4) == 0)
    (void) strcpy(image_info->magick,"FAX");
  if (strncmp(magick,"SIMPLE",6) == 0)
    (void) strcpy(image_info->magick,"FITS");
  if (strncmp(magick,"#FIG",4) == 0)
    (void) strcpy(image_info->magick,"FIG");
  if (strncmp(magick,"GIF8",4) == 0)
    (void) strcpy(image_info->magick,"GIF");
  if (strncmp(magick,"#!/usr/local/bin/gnuplot",24) == 0)
    (void) strcpy(image_info->magick,"GPLT");
  if (strncmp(magick,"\016\003\023\001",4) == 0)
    (void) strcpy(image_info->magick,"HDF");
  if (strncmp(magick,"IN;",3) == 0)
    (void) strcpy(image_info->magick,"HPGL");
  if ((strncmp(magick,"<HTML",5) == 0) ||
      (strncmp(magick,"<html",5) == 0))
    (void) strcpy(image_info->magick,"HTML");
  if (strncmp(magick,"\377\330\377",3) == 0)
    (void) strcpy(image_info->magick,"JPEG");
  if (strncmp(magick,"id=ImageMagick",14) == 0)
    (void) strcpy(image_info->magick,"MIFF");
  if (strncmp(magick,"\212MNG\r\n\032\n",8) == 0)
    (void) strcpy(image_info->magick,"MNG");
  if ((magick[0] == 0x00) && (magick[1] == 0x00))
    if ((magick[2] == 0x01) && (magick[3] == (char) 0xb3))
      (void) strcpy(image_info->magick,"M2V");
  if (strncmp(magick,"PCD_",4) == 0)
    (void) strcpy(image_info->magick,"PCD");
  if (strncmp(magick,"\033E\033",3) == 0)
    (void) strcpy(image_info->magick,"PCL");
  if (strncmp(magick,"\12\2",2) == 0)
    (void) strcpy(image_info->magick,"PCX");
  if (strncmp(magick,"\12\5",2) == 0)
    (void) strcpy(image_info->magick,"PCX");
  if (strncmp(magick,"%PDF-",5) == 0)
    (void) strcpy(image_info->magick,"PDF");
  if ((*magick == 'P') && isdigit((int) (magick[1])))
    (void) strcpy(image_info->magick,"PNM");
  if (strncmp(magick,"\211PNG\r\n\032\n",8) == 0)
    (void) strcpy(image_info->magick,"PNG");
  if (strncmp(magick,"\004%!",3) == 0)
    (void) strcpy(image_info->magick,"PS");
  if (strncmp(magick,"%!",2) == 0)
    (void) strcpy(image_info->magick,"PS");
  if (strncmp(magick,"8BPS",4) == 0)
    (void) strcpy(image_info->magick,"PSD");
  if (strncmp(magick,"SFW95",5) == 0)
    (void) strcpy(image_info->magick,"PWP");
  if (strncmp(magick,"#?RADIANCE",10) == 0)
    (void) strcpy(image_info->magick,"RAD");
  if (strncmp(magick,"\122\314",2) == 0)
    (void) strcpy(image_info->magick,"RLE");
  if (strncmp(magick,"SFW94",5) == 0)
    (void) strcpy(image_info->magick,"SFW");
  if (strncmp(magick,"\001\332",2) == 0)
    (void) strcpy(image_info->magick,"SGI");
  if (strncmp(magick,"\131\246\152\225",4) == 0)
    (void) strcpy(image_info->magick,"SUN");
  if ((magick[0] == 0x4D) && (magick[1] == 0x4D))
    if ((magick[2] == 0x00) && (magick[3] == (char) 0x2A))
      (void) strcpy(image_info->magick,"TIFF");
  if (strncmp(magick,"\111\111\052\000",4) == 0)
    (void) strcpy(image_info->magick,"TIFF");
  if ((strncmp(magick,"LBLSIZE",7) == 0) || (strncmp(magick,"NJPL1I",6) == 0))
    (void) strcpy(image_info->magick,"VICAR");
  if (strncmp(magick,"\253\1",2) == 0)
    (void) strcpy(image_info->magick,"VIFF");
  p=strchr(magick,'#');
  if (p != (char *) NULL)
    if (strncmp(p,"#define",7) == 0)
      (void) strcpy(image_info->magick,"XBM");
  if (strncmp(magick,"/* XPM */",9) == 0)
    (void) strcpy(image_info->magick,"XPM");
  if ((magick[1] == 0x00) && (magick[2] == 0x00))
    if ((magick[5] == 0x00) && (magick[6] == 0x00))
      if ((magick[4] == 0x07) || (magick[7] == 0x07))
        (void) strcpy(image_info->magick,"XWD");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S o r t C o l o r m a p B y I n t e n t s i t y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SortColormapByIntensity sorts the colormap of a PseudoClass image
%  by decreasing color intensity.
%
%  The format of the SortColormapByIntensity routine is:
%
%      SortColormapByIntensity(image)
%
%  A description of each parameter follows:
%
%    o image: A pointer to a Image structure.
%
%
*/

static int IntensityCompare(const void *x,const void *y)
{
  ColorPacket
    *color_1,
    *color_2;

  color_1=(ColorPacket *) x;
  color_2=(ColorPacket *) y;
  return((int) Intensity(*color_2)-(int) Intensity(*color_1));
}

Export void SortColormapByIntensity(Image *image)
{
  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned short
    index;

  unsigned short
    *pixels;

  assert(image != (Image *) NULL);
  if (image->class != PseudoClass)
    return;
  /*
    Allocate memory for pixel indexes.
  */
  pixels=(unsigned short *)
    AllocateMemory(image->colors*sizeof(unsigned short));
  if (pixels == (unsigned short *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to sort colormap",
        "Memory allocation failed");
      return;
    }
  /*
    Assign index values to colormap entries.
  */
  for (i=0; i < (int) image->colors; i++)
    image->colormap[i].index=(unsigned short) i;
  /*
    Sort image colormap by decreasing color popularity.
  */
  qsort((void *) image->colormap,(int) image->colors,sizeof(ColorPacket),
    (int (*)(const void *, const void *)) IntensityCompare);
  /*
    Update image colormap indexes to sorted colormap order.
  */
  for (i=0; i < (int) image->colors; i++)
    pixels[image->colormap[i].index]=(unsigned short) i;
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    index=pixels[p->index];
    p->red=image->colormap[index].red;
    p->green=image->colormap[index].green;
    p->blue=image->colormap[index].blue;
    p->index=index;
    p++;
  }
  FreeMemory((char *) pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t e g a n o I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SteganoImage hides a digital watermark within the image.
%
%  The format of the SteganoImage routine is:
%
%      stegano_image=SteganoImage(image,watermark)
%
%  A description of each parameter follows:
%
%    o stegano_image: Method SteganoImage returns a pointer to the
%      steganographic image with the watermark hidden.  A null image is
%      returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o watermark: The address of a structure of type Image.
%
%
*/
Export Image *SteganoImage(Image *image,Image *watermark)
{
#define EmbedBit(byte) \
{ \
  byte&=(~0x01); \
  byte|=(Intensity(*r) >> shift) & 0x01; \
  r++; \
  if (r >= (watermark->pixels+watermark->packets-1)) \
    { \
      r=watermark->pixels; \
      shift--; \
      if (shift < 0) \
        break; \
    } \
}
#define SteganoImageText  "  Hiding image...  "

  Image
    *stegano_image;

  int
    shift;

  register int
    i;

  register RunlengthPacket
    *p,
    *q,
    *r;

  assert(image != (Image *) NULL);
  assert(watermark != (Image *) NULL);
  if (!UncondenseImage(image))
    return((Image *) NULL);
  if (!UncondenseImage(watermark))
    return((Image *) NULL);
  /*
    Initialize steganographic image attributes.
  */
  stegano_image=CloneImage(image,image->columns,image->rows,True);
  if (stegano_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,
        "Unable to create steganographic image","Memory allocation failed");
      return((Image *) NULL);
    }
  if (stegano_image->class == PseudoClass)
    {
      if (stegano_image->colors > ((MaxRGB+1) >> 1))
        stegano_image->class=DirectClass;
      else
        {
          /*
            Shift colormap to make room for information hiding.
          */
          stegano_image->colors<<=1;
          stegano_image->colormap=(ColorPacket *) ReallocateMemory((char *)
            stegano_image->colormap,stegano_image->colors*sizeof(ColorPacket));
          if (stegano_image->colormap == (ColorPacket *) NULL)
            {
              MagickWarning(ResourceLimitWarning,
                "Unable to create steganographic image",
                "Memory allocation failed");
              DestroyImage(stegano_image);
              return((Image *) NULL);
            }
          for (i=stegano_image->colors-1; i >= 0; i--)
            stegano_image->colormap[i]=stegano_image->colormap[i >> 1];
          q=stegano_image->pixels;
          for (i=0; i < (int) stegano_image->packets; i++)
          {
            q->index<<=1;
            q++;
          }
        }
    }
  /*
    Hide watermark in low-order bits of image.
  */
  shift=QuantumDepth-1;
  p=image->pixels+(image->offset % image->packets);
  q=stegano_image->pixels+(stegano_image->offset % stegano_image->packets);
  r=watermark->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    if (stegano_image->class == PseudoClass)
      EmbedBit(q->index)
    else
      {
        EmbedBit(q->red);
        EmbedBit(q->green);
        EmbedBit(q->blue);
      }
    p++;
    q++;
    if (p >= (image->pixels+image->packets-1))
      {
        p=image->pixels;
        q=stegano_image->pixels;
      }
    if (QuantumTick(i,image->packets))
      ProgressMonitor(SteganoImageText,i,image->packets);
  }
  return(stegano_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t e r e o I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StereoImage combines two images and produces a single image that
%  is the composite of a left and right image of a stereo pair.  The left
%  image is converted to gray scale and written to the red channel of the
%  stereo image.  The right image is converted to gray scale and written to the
%  blue channel of the stereo image.  View the composite image with red-blue
%  glasses to create a stereo effect.
%
%  The format of the StereoImage routine is:
%
%      stereo_image=StereoImage(left_image,right_image)
%
%  A description of each parameter follows:
%
%    o stereo_image: Method StereoImage returns a pointer to the stereo
%      image.  A null image is returned if there is a memory shortage.
%
%    o left_image: The address of a structure of type Image.
%
%    o right_image: The address of a structure of type Image.
%
%
*/
Export Image *StereoImage(Image *left_image,const Image *right_image)
{
#define StereoImageText  "  Stereo image...  "

  Image
    *stereo_image;

  int
    y;

  register int
    left_runlength,
    right_runlength,
    x;

  register RunlengthPacket
    *p,
    *q,
    *r;

  assert(left_image != (Image *) NULL);
  assert(right_image != (Image *) NULL);
  if ((left_image->columns != right_image->columns) ||
      (left_image->rows != right_image->rows))
    {
      MagickWarning(ResourceLimitWarning,"Unable to create stereo image",
        "left and right image sizes differ");
      return((Image *) NULL);
    }
  /*
    Initialize stereo image attributes.
  */
  stereo_image=
    CloneImage(left_image,left_image->columns,left_image->rows,False);
  if (stereo_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to create stereo image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  stereo_image->class=DirectClass;
  /*
    Copy left image to red channel and right image to blue channel.
  */
  p=left_image->pixels;
  left_runlength=p->length+1;
  q=right_image->pixels;
  right_runlength=q->length+1;
  r=stereo_image->pixels;
  for (y=0; y < (int) stereo_image->rows; y++)
  {
    for (x=0; x < (int) stereo_image->columns; x++)
    {
      if (left_runlength != 0)
        left_runlength--;
      else
        {
          p++;
          left_runlength=p->length;
        }
      if (right_runlength != 0)
        right_runlength--;
      else
        {
          q++;
          right_runlength=q->length;
        }
      r->red=Intensity(*p);
      r->green=0;
      r->blue=Intensity(*q);
      r->index=0;
      r->length=0;
      r++;
    }
    if (QuantumTick(y,stereo_image->rows))
      ProgressMonitor(StereoImageText,y,stereo_image->rows);
  }
  return(stereo_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y n c I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncImage initializes the red, green, and blue intensities of each
%  pixel as defined by the colormap index.
%
%  The format of the SyncImage routine is:
%
%      SyncImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
Export void SyncImage(Image *image)
{
  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned short
    index;

  assert(image != (Image *) NULL);
  if (image->class == DirectClass)
    return;
  for (i=0; i < (int) image->colors; i++)
  {
    image->colormap[i].index=0;
    image->colormap[i].flags=0;
  }
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    index=p->index;
    p->red=image->colormap[index].red;
    p->green=image->colormap[index].green;
    p->blue=image->colormap[index].blue;
    p++;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T e x t u r e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TextureImage layers a texture onto the background of an image.
%
%  The format of the TextureImage routine is:
%
%      TextureImage(image,texture)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o texture: This image contains the texture to layer on the background.
%
%
*/
Export void TextureImage(Image *image,Image *texture)
{
#define TextureImageText  "  Appling image texture...  "

  int
    x,
    y;

  assert(image != (Image *) NULL);
  if (texture == (const Image *) NULL)
    return;
  /*
    Tile texture onto the image background.
  */
  for (y=0; y < (int) image->rows; y+=texture->rows)
  {
    for (x=0; x < (int) image->columns; x+=texture->columns)
      CompositeImage(image,ReplaceCompositeOp,texture,x,y);
    if (QuantumTick(y,image->rows))
      ProgressMonitor(TextureImageText,y,image->rows);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T h r e s h o l d I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ThresholdImage thresholds the reference image.
%
%  The format of the ThresholdImage routine is:
%
%      ThresholdImage(image,threshold)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o threshold: A double indicating the threshold value.
%
%
*/
Export void ThresholdImage(Image *image,const double threshold)
{
#define ThresholdImageText  "  Threshold the image...  "

  ColorPacket
    *colormap;

  register int
    i;

  register RunlengthPacket
    *p;

  /*
    Threshold image.
  */
  assert(image != (Image *) NULL);
  colormap=(ColorPacket *) AllocateMemory(2*sizeof(ColorPacket));
  if (colormap == (ColorPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to allocate image",
        "Memory allocation failed");
      return;
    }
  if (image->colormap != (ColorPacket *) NULL)
    FreeMemory((char *) image->colormap);
  image->class=PseudoClass;
  image->colors=2;
  image->colormap=colormap;
  image->colormap[0].red=0;
  image->colormap[0].green=0;
  image->colormap[0].blue=0;
  image->colormap[1].red=MaxRGB;
  image->colormap[1].green=MaxRGB;
  image->colormap[1].blue=MaxRGB;
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    p->index=Intensity(*p) < threshold ? 0 : 1;
    p++;
    if (QuantumTick(i,image->packets))
      ProgressMonitor(ThresholdImageText,i,image->packets);
  }
  SyncImage(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s f o r m I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformImage creates a new image that is a transformed size of
%  of existing one as specified by the crop and image geometries.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  If a crop geometry is specified a subregion of the image is obtained.
%  If the specified image size, as defined by the image and scale geometries,
%  is smaller than the actual image size, the image is first minified to an
%  integral of the specified image size with an antialias digital filter.  The
%  image is then scaled to the exact specified image size with pixel
%  replication.  If the specified image size is greater than the actual image
%  size, the image is first enlarged to an integral of the specified image
%  size with bilinear interpolation.  The image is then scaled to the exact
%  specified image size with pixel replication.
%
%  The format of the TransformImage routine is:
%
%      TransformImage(image,crop_geometry,image_geometry)
%
%  A description of each parameter follows:
%
%    o image: The address of an address of a structure of type Image.  The
%      transformed image is returned as this parameter.
%
%    o crop_geometry: Specifies a pointer to a crop geometry string.
%      This geometry defines a subregion of the image.
%
%    o image_geometry: Specifies a pointer to a image geometry string.
%      The specified width and height of this geometry string are absolute.
%
%
*/
Export void TransformImage(Image **image,const char *crop_geometry,
  const char *image_geometry)
{
  Image
    *transformed_image;

  int
    flags,
    x,
    y;

  unsigned int
    height,
    width;

  assert(image != (Image **) NULL);
  transformed_image=(*image);
  if (crop_geometry != (const char *) NULL)
    {
      Image
        *cropped_image;

      RectangleInfo
        crop_info;

      /*
        Crop image to a user specified size.
      */
      width=transformed_image->columns;
      height=transformed_image->rows;
      crop_info.x=0;
      crop_info.y=0;
      flags=XParseGeometry((char *) crop_geometry,&crop_info.x,&crop_info.y,
        &width,&height);
      if ((flags & WidthValue) == 0)
        width=(unsigned int) ((int) transformed_image->columns-crop_info.x);
      if ((flags & HeightValue) == 0)
        height=(unsigned int) ((int) transformed_image->rows-crop_info.y);
      if ((flags & XNegative) != 0)
        crop_info.x+=transformed_image->columns-width;
      if ((flags & YNegative) != 0)
        crop_info.y+=transformed_image->rows-height;
      if (strchr(crop_geometry,'%') != (char *) NULL)
        {
          /*
            Crop geometry is relative to image size.
          */
          x=0;
          y=0;
          (void) ParseImageGeometry(crop_geometry,&x,&y,&width,&height);
          if (width > transformed_image->columns)
            width=transformed_image->columns;
          if (height > transformed_image->rows)
            height=transformed_image->rows;
          crop_info.x=width >> 1;
          crop_info.y=height >> 1;
          width=transformed_image->columns-width;
          height=transformed_image->rows-height;
          flags|=XValue | YValue;
        }
      crop_info.width=width;
      crop_info.height=height;
      if ((width == 0) || (height == 0) ||
          ((flags & XValue) != 0) || ((flags & YValue) != 0))
        cropped_image=CropImage(transformed_image,&crop_info);
      else
        {
          Image
            *next_image;

          /*
            Crop repeatedly to create uniform subimages.
          */
          next_image=(Image *) NULL;
          cropped_image=(Image *) NULL;
          for (y=0; y < (int) transformed_image->rows; y+=height)
          {
            for (x=0; x < (int) transformed_image->columns; x+=width)
            {
              crop_info.width=width;
              crop_info.height=height;
              crop_info.x=x;
              crop_info.y=y;
              next_image=CropImage(transformed_image,&crop_info);
              if (next_image == (Image *) NULL)
                break;
              if (cropped_image == (Image *) NULL)
                cropped_image=next_image;
              else
                {
                  next_image->previous=cropped_image;
                  cropped_image->next=next_image;
                  cropped_image=cropped_image->next;
                }
            }
            if (next_image == (Image *) NULL)
              break;
          }
        }
      if (cropped_image != (Image *) NULL)
        {
          DestroyImage(transformed_image);
          while (cropped_image->previous != (Image *) NULL)
            cropped_image=cropped_image->previous;
          transformed_image=cropped_image;
        }
    }
  /*
    Scale image to a user specified size.
  */
  width=transformed_image->columns;
  height=transformed_image->rows;
  x=0;
  y=0;
  (void) ParseImageGeometry(image_geometry,&x,&y,&width,&height);
  if ((transformed_image->columns != width) ||
      (transformed_image->rows != height))
    {
      Image
        *zoomed_image;

      /*
        Zoom image.
      */
      zoomed_image=ZoomImage(transformed_image,width,height);
      if (zoomed_image == (Image *) NULL)
        zoomed_image=ScaleImage(transformed_image,width,height);
      if (zoomed_image != (Image *) NULL)
        {
          DestroyImage(transformed_image);
          transformed_image=zoomed_image;
        }
    }
  *image=transformed_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T r a n s f o r m R G B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformRGBImage converts the reference image from an alternate
%  colorspace.  The transformation matrices are not the standard ones:  the
%  weights are rescaled to normalized the range of the transformed values to
%  be [0..MaxRGB].
%
%  The format of the TransformRGBImage routine is:
%
%      TransformRGBImage(image,colorspace)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o colorspace: An unsigned integer value that indicates the colorspace
%      the image is currently in.  On return the image is in the RGB
%      color space.
%
%
*/
Export void TransformRGBImage(Image *image,const ColorspaceType colorspace)
{
#define B (MaxRGB+1)*2
#define G (MaxRGB+1)
#define R 0
#define TransformRGBImageText  "  Transforming image colors...  "

  static const Quantum
    sRGBMap[351] =
    {
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,
       14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
       28,  29,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
       41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,
       55,  56,  57,  58,  59,  60,  61,  62,  63,  65,  66,  67,  68,  69,
       70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,
       84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  95,  96,  97,  98,
       99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
      114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
      128, 129, 130, 131, 132, 133, 135, 136, 137, 138, 139, 140, 141, 142,
      143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156,
      157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
      171, 172, 173, 174, 175, 175, 176, 177, 178, 179, 180, 181, 182, 183,
      184, 185, 186, 187, 187, 188, 189, 190, 191, 192, 193, 194, 194, 195,
      196, 197, 198, 199, 199, 200, 201, 202, 203, 203, 204, 205, 206, 207,
      207, 208, 209, 210, 210, 211, 212, 213, 213, 214, 215, 215, 216, 217,
      218, 218, 219, 220, 220, 221, 222, 222, 223, 223, 224, 225, 225, 226,
      227, 227, 228, 228, 229, 229, 230, 230, 231, 232, 232, 233, 233, 234,
      234, 235, 235, 235, 236, 236, 237, 237, 238, 238, 238, 239, 239, 240,
      240, 240, 241, 241, 242, 242, 242, 243, 243, 243, 243, 244, 244, 244,
      245, 245, 245, 245, 246, 246, 246, 247, 247, 247, 247, 247, 248, 248,
      248, 248, 249, 249, 249, 249, 249, 249, 250, 250, 250, 250, 250, 250,
      251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 254, 254, 254, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255,
      255
    },
    YCCMap[351] =  /* Photo CD information beyond 100% white, Gamma 2.2 */
    {
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,
        14, 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
        28, 29,  30,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,
        43, 45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  56,  57,  58,
        59, 60,  61,  62,  63,  64,  66,  67,  68,  69,  70,  71,  72,  73,
        74, 76,  77,  78,  79,  80,  81,  82,  83,  84,  86,  87,  88,  89,
        90, 91,  92,  93,  94,  95,  97,  98,  99, 100, 101, 102, 103, 104,
      105, 106, 107, 108, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
      120, 121, 122, 123, 124, 125, 126, 127, 129, 130, 131, 132, 133, 134,
      135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148,
      149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162,
      163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
      176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
      190, 191, 192, 193, 193, 194, 195, 196, 197, 198, 199, 200, 201, 201,
      202, 203, 204, 205, 206, 207, 207, 208, 209, 210, 211, 211, 212, 213,
      214, 215, 215, 216, 217, 218, 218, 219, 220, 221, 221, 222, 223, 224,
      224, 225, 226, 226, 227, 228, 228, 229, 230, 230, 231, 232, 232, 233,
      234, 234, 235, 236, 236, 237, 237, 238, 238, 239, 240, 240, 241, 241,
      242, 242, 243, 243, 244, 244, 245, 245, 245, 246, 246, 247, 247, 247,
      248, 248, 248, 249, 249, 249, 249, 250, 250, 250, 250, 251, 251, 251,
      251, 251, 252, 252, 252, 252, 252, 253, 253, 253, 253, 253, 253, 253,
      253, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255
    };

  long
    *blue,
    *green,
    *red;

  Quantum
    *range_table;

  register int
    i,
    x,
    y,
    z;

  register Quantum
    *range_limit;

  register RunlengthPacket
    *p;

  assert(image != (Image *) NULL);
  if ((image->colorspace == CMYKColorspace) && (colorspace == RGBColorspace))
    {
      unsigned int
        black,
        cyan,
        magenta,
        yellow;

      /*
        Transform image from CMYK to RGB.
      */
      image->colorspace=RGBColorspace;
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        cyan=MaxRGB-p->red;
        magenta=MaxRGB-p->green;
        yellow=MaxRGB-p->blue;
        black=MaxRGB-p->index;
        if ((cyan+black) > MaxRGB)
          p->red=0;
        else
          p->red=MaxRGB-(cyan+black);
        if ((magenta+black) > MaxRGB)
          p->green=0;
        else
          p->green=MaxRGB-(magenta+black);
        if ((yellow+black) > MaxRGB)
          p->blue=0;
        else
          p->blue=MaxRGB-(yellow+black);
        p->index=0;
        p++;
      }
      return;
    }
  if ((colorspace == RGBColorspace) || (colorspace == GRAYColorspace) ||
      (colorspace == TransparentColorspace))
    return;
  /*
    Allocate the tables.
  */
  red=(long *) AllocateMemory(3*(MaxRGB+1)*sizeof(long));
  green=(long *) AllocateMemory(3*(MaxRGB+1)*sizeof(long));
  blue=(long *) AllocateMemory(3*(MaxRGB+1)*sizeof(long));
  range_table=(Quantum *) AllocateMemory(4*(MaxRGB+1)*sizeof(Quantum));
  if ((red == (long *) NULL) || (green == (long *) NULL) ||
      (blue == (long *) NULL) || (range_table == (Quantum *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Unable to transform color space",
        "Memory allocation failed");
      return;
    }
  /*
    Initialize tables.
  */
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(Quantum) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  for (i=0; i <= MaxRGB; i++)
    range_table[i+(MaxRGB+1)*3]=MaxRGB;
  range_limit=range_table+(MaxRGB+1);
  switch (colorspace)
  {
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          R = I1+1.00000*I2-0.66668*I3
          G = I1+0.00000*I2+1.33333*I3
          B = I1-1.00000*I2-0.66668*I3

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=UpShifted(1.0000*0.5)*((i << 1)-MaxRGB);
        blue[i+R]=(-UpShifted(0.66668*0.5))*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=0;
        blue[i+G]=UpShifted(1.33333*0.5)*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=(-UpShifted(1.00000*0.5))*((i << 1)-MaxRGB);
        blue[i+B]=(-UpShifted(0.66668*0.5))*((i << 1)-MaxRGB);
      }
      break;
    }
    case sRGBColorspace:
    {
      /*
        Initialize sRGB tables:

          R = Y            +1.032096*C2
          G = Y-0.326904*C1-0.704445*C2
          B = Y+1.685070*C1

        sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.40200)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.88000)*(i-UpScale(137));
        red[i+G]=UpShifted(1.40200)*i;
        green[i+G]=(-(int) (UpShifted(0.19400*2.28900)*(i-UpScale(156))));
        blue[i+G]=(-(int) (UpShifted(0.50900*1.88000)*(i-UpScale(137))));
        red[i+B]=UpShifted(1.40200)*i;
        green[i+B]=UpShifted(2.28900)*(i-UpScale(156));
        blue[i+B]=0;
        range_table[i+(MaxRGB+1)]=(Quantum) UpScale(sRGBMap[DownScale(i)]);
      }
      for ( ; i < (int) UpScale(351); i++)
        range_table[i+(MaxRGB+1)]=(Quantum) UpScale(sRGBMap[DownScale(i)]);
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize CIE XYZ tables:

          R =  3.240479*R-1.537150*G-0.498535*B
          G = -0.969256*R+1.875992*G+0.041556*B
          B =  0.055648*R-0.204043*G+1.057311*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(3.240479)*i;
        green[i+R]=(-UpShifted(1.537150))*i;
        blue[i+R]=(-UpShifted(0.498535))*i;
        red[i+G]=(-UpShifted(0.969256))*i;
        green[i+G]=UpShifted(1.875992)*i;
        blue[i+G]=UpShifted(0.041556)*i;
        red[i+B]=UpShifted(0.055648)*i;
        green[i+B]=(-UpShifted(0.204043))*i;
        blue[i+B]=UpShifted(1.057311)*i;
      }
      break;
    }
    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          R = Y            +1.402000*Cr
          G = Y-0.344136*Cb-0.714136*Cr
          B = Y+1.772000*Cb

        Cb and Cr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.000000)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.402000*0.5)*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.000000)*i;
        green[i+G]=(-UpShifted(0.344136*0.5))*((i << 1)-MaxRGB);
        blue[i+G]=(-UpShifted(0.714136*0.5))*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.000000)*i;
        green[i+B]=UpShifted(1.772000*0.5)*((i << 1)-MaxRGB);
        blue[i+B]=0;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Initialize YCC tables:

          R = Y            +1.340762*C2
          G = Y-0.317038*C1-0.682243*C2
          B = Y+1.632639*C1

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.35840)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.82150)*(i-UpScale(137));
        red[i+G]=UpShifted(1.35840)*i;
        green[i+G]=(-(int) (UpShifted(0.19400*2.21790)*(i-UpScale(156))));
        blue[i+G]=(-(int) (UpShifted(0.50900*1.82150)*(i-UpScale(137))));
        red[i+B]=UpShifted(1.35840)*i;
        green[i+B]=UpShifted(2.21790)*(i-UpScale(156));
        blue[i+B]=0;
        range_table[i+(MaxRGB+1)]=(Quantum) UpScale(YCCMap[DownScale(i)]);
      }
      for ( ; i < (int) UpScale(351); i++)
        range_table[i+(MaxRGB+1)]=(Quantum) UpScale(YCCMap[DownScale(i)]);
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          R = Y+0.95620*I+0.62140*Q
          G = Y-0.27270*I-0.64680*Q
          B = Y-1.10370*I+1.70060*Q

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=UpShifted(0.95620*0.5)*((i << 1)-MaxRGB);
        blue[i+R]=UpShifted(0.62140*0.5)*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=(-UpShifted(0.27270*0.5))*((i << 1)-MaxRGB);
        blue[i+G]=(-UpShifted(0.64680*0.5))*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=(-UpShifted(1.10370*0.5))*((i << 1)-MaxRGB);
        blue[i+B]=UpShifted(1.70060*0.5)*((i << 1)-MaxRGB);
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize YPbPr tables:

          R = Y            +1.402000*C2
          G = Y-0.344136*C1+0.714136*C2
          B = Y+1.772000*C1

        Pb and Pr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.000000)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.402000*0.5)*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.000000)*i;
        green[i+G]=(-UpShifted(0.344136*0.5))*((i << 1)-MaxRGB);
        blue[i+G]=UpShifted(0.714136*0.5)*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.000000)*i;
        green[i+B]=UpShifted(1.772000*0.5)*((i << 1)-MaxRGB);
        blue[i+B]=0;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          R = Y          +1.13980*V
          G = Y-0.39380*U-0.58050*V
          B = Y+2.02790*U

        U and V, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.13980*0.5)*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=(-UpShifted(0.39380*0.5))*((i << 1)-MaxRGB);
        blue[i+G]=(-UpShifted(0.58050*0.5))*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=UpShifted(2.02790*0.5)*((i << 1)-MaxRGB);
        blue[i+B]=0;
      }
      break;
    }
  }
  /*
    Convert to RGB.
  */
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Convert DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        x=p->red;
        y=p->green;
        z=p->blue;
        p->red=range_limit[DownShift(red[x+R]+green[y+R]+blue[z+R])];
        p->green=range_limit[DownShift(red[x+G]+green[y+G]+blue[z+G])];
        p->blue=range_limit[DownShift(red[x+B]+green[y+B]+blue[z+B])];
        p++;
        if (QuantumTick(i,image->packets))
          ProgressMonitor(TransformRGBImageText,i,image->packets);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        x=image->colormap[i].red;
        y=image->colormap[i].green;
        z=image->colormap[i].blue;
        image->colormap[i].red=
          range_limit[DownShift(red[x+R]+green[y+R]+blue[z+R])];
        image->colormap[i].green=
          range_limit[DownShift(red[x+G]+green[y+G]+blue[z+G])];
        image->colormap[i].blue=
          range_limit[DownShift(red[x+B]+green[y+B]+blue[z+B])];
      }
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        x=p->red;
        y=p->green;
        z=p->blue;
        p->red=range_limit[DownShift(red[x+R]+green[y+R]+blue[z+R])];
        p->green=range_limit[DownShift(red[x+G]+green[y+G]+blue[z+G])];
        p->blue=range_limit[DownShift(red[x+B]+green[y+B]+blue[z+B])];
        p++;
      }
      break;
    }
  }
  /*
    Free allocated memory.
  */
  FreeMemory((char *) range_table);
  FreeMemory((char *) blue);
  FreeMemory((char *) green);
  FreeMemory((char *) red);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T r a n s p a r e n t I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransparentImage creates a matte image associated with the
%  image.  All pixel locations are initially set to opaque.  Any pixel
%  that matches the specified color are set to transparent.
%
%  The format of the TransparentImage routine is:
%
%      TransparentImage(image,color)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o color: A character string that contain an X11 color string.
%
%
*/
Export void TransparentImage(Image *image,const char *color)
{
#define TransparentImageText  "  Setting transparent color in the image...  "

  ColorPacket
    target;

  register int
    i;

  register RunlengthPacket
    *p;

  unsigned int
    status;

  XColor
    target_color;

  /*
    Determine RGB values of the transparent color.
  */
  assert(image != (Image *) NULL);
  status=XQueryColorDatabase(color,&target_color);
  if (status == False)
    return;
  target.red=XDownScale(target_color.red);
  target.green=XDownScale(target_color.green);
  target.blue=XDownScale(target_color.blue);
  /*
    Make image color transparent.
  */
  if (!image->matte)
    MatteImage(image);
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    if (ColorMatch(*p,target,image->fuzz))
      p->index=Transparent;
    p++;
    if (QuantumTick(i,image->packets))
      ProgressMonitor(TransparentImageText,i,image->packets);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n c o m p r e s s I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UncondenseImage uncompresses runlength-encoded pixels packets to
%  a rectangular array of pixels.
%
%  The format of the UncondenseImage routine is:
%
%      status=UncondenseImage(image)
%
%  A description of each parameter follows:
%
%    o status: Method UncondenseImage returns True if the image is
%      uncompressed otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int UncondenseImage(Image *image)
{
  int
    length;

  register int
    i,
    j;

  register RunlengthPacket
    *p,
    *q;

  RunlengthPacket
    *uncompressed_pixels;

  assert(image != (Image *) NULL);
  if (image->packets == (image->columns*image->rows))
    return(True);
  /*
    Uncompress runlength-encoded packets.
  */
  uncompressed_pixels=(RunlengthPacket *) ReallocateMemory((char *)
    image->pixels,image->columns*image->rows*sizeof(RunlengthPacket));
  if (uncompressed_pixels == (RunlengthPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to uncompress image",
        "Memory allocation failed");
      return(False);
    }
  p=uncompressed_pixels+(image->packets-1);
  q=uncompressed_pixels+(image->columns*image->rows-1);
  for (i=0; i < (int) image->packets; i++)
  {
    length=p->length;
    for (j=0; j <= length; j++)
    {
      *q=(*p);
      q->length=0;
      q--;
    }
    p--;
  }
  image->packets=image->columns*image->rows;
  image->pixels=uncompressed_pixels;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Z o o m I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ZoomImage creates a new image that is a scaled size of an
%  existing one.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.  The Point filter gives
%  fast pixel replication, Triangle is equivalent to bi-linear interpolation,
%  and Mitchel giver slower, very high-quality results.  See Graphic Gems III
%  for details on this algorithm.
%
%  The filter member of the Image structure specifies which image filter to
%  use. Blur specifies the blur factor where > 1 is blurry, < 1 is sharp.
%
%  The format of the ZoomImage routine is:
%
%      zoomed_image=ZoomImage(image,columns,rows)
%
%  A description of each parameter follows:
%
%    o zoomed_image: Method ZoomImage returns a pointer to the image after
%      scaling.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o columns: An integer that specifies the number of columns in the zoomed
%      image.
%
%    o rows: An integer that specifies the number of rows in the scaled
%      image.
%
%
*/

#define ZoomImageText  "  Zooming image...  "

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static double Box(double x)
{
  if ((x >= -0.5) && (x < 0.5))
    return(1.0);
  return(0.0);
}

static double Bessel(double x)
{
  if (x == 0.0)
    return(M_PI/4.0);
  return(BesselOrderOne(M_PI*x)/(2.0*x));
}

static double Blackman(double x)
{
  return(0.42+0.50*cos(M_PI*x)+0.08*cos(2.0*M_PI*x));
}

static double Catrom(double x)
{
  if (x < 0)
    x=(-x);
  if (x < 1.0)
    return(0.5*(2.0+x*x*(-5.0+x*3.0)));
  if (x < 2.0)
    return(0.5*(4.0+x*(-8.0+x*(5.0-x))));
  return(0.0);
}

static double Cubic(double x)
{
  if (x < 0)
    x=(-x);
  if (x < 1.0)
    return((0.5*x*x*x)-x*x+(2.0/3.0));
  if (x < 2.0)
    {
      x=2.0-x;
      return((1.0/6.0)*x*x*x);
    }
  return(0.0);
}

static double Gaussian(double x)
{
  return(exp(-2.0*x*x)*sqrt(2.0/M_PI));
}

static double Hanning(double x)
{
  return(0.5+0.5*cos(M_PI*x));
}

static double Hamming(double x)
{
  return(0.54+0.46*cos(M_PI*x));
}

static double Hermite(double x)
{
  if (x < 0)
    x=(-x);
  if (x < 1.0)
    return((2.0*x-3.0)*x*x+1.0);
  return(0.0);
}

static double Sinc(double x)
{
  x*=M_PI;
  if (x != 0.0)
    return(sin(x)/x);
  return(1.0);
}

static double Lanczos(double x)
{
  if (x < 0)
    x=(-x);
  if (x < 3.0)
   return(Sinc(x)*Sinc(x/3.0));
  return(0.0);
}

static double Mitchell(double x)
{
  double
    b,
    c;

  b=1.0/3.0;
  c=1.0/3.0;
  if (x < 0)
    x=(-x);
  if (x < 1.0)
    {
      x=((12.0-9.0*b-6.0*c)*(x*x*x))+((-18.0+12.0*b+6.0*c)*x*x)+(6.0-2.0*b);
      return(x/6.0);
    }
 if (x < 2.0)
   {
     x=((-1.0*b-6.0*c)*(x*x*x))+((6.0*b+30.0*c)*x*x)+((-12.0*b-48.0*c)*x)+
       (8.0*b+24.0*c);
     return(x/6.0);
   }
  return(0.0);
}

static double Quadratic(double x)
{
  if (x < 0)
    x=(-x);
  if (x < 0.5)
    return(0.75-x*x);
  if (x < 1.5)
    {
      x-=1.5;
      return(0.5*x*x);
    }
  return(0.0);
}

static double Triangle(double x)
{
  if (x < 0.0)
    x=(-x);
  if (x < 1.0)
    return(1.0-x);
  return(0.0);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

static void HorizontalFilter(Image *source,Image *destination,double x_factor,
  const FilterInfo *filter_info,ContributionInfo *contribution_info,
  const Quantum *range_limit,const unsigned int span,unsigned int *quantum)
{
  double
    blue_weight,
    center,
    green_weight,
    index_weight,
    red_weight,
    scale_factor,
    sum,
    support;

  int
    n,
    x;

  register int
    i,
    j,
    y;

  register RunlengthPacket
    *p,
    *q;

  /*
    Apply filter to zoom horizontally from source to destination.
  */
  scale_factor=source->blur*Max(1.0/x_factor,1.0);
  support=Max(scale_factor*filter_info->support,0.5);
  destination->class=source->class;
  if (support > 0.5)
    destination->class=DirectClass;
  else
    {
      /*
        Reduce to point sampling.
      */
      support=0.5;
      scale_factor=1.0;
    }
  support+=1.e-7;
  for (x=0; x < (int) destination->columns; x++)
  {
    sum=0.0;
    n=0;
    center=(double) x/x_factor;
    for (i=(int) (center-support+0.5); i < (int) (center+support+0.5); i++)
    {
      j=i;
      if (j < 0)
        j=(-j);
      else
        if (j >= (int) source->columns)
          j=(source->columns << 1)-j-1;
      contribution_info[n].pixel=j;
      contribution_info[n].weight=filter_info->function(
        ((double) i-center+0.5)/scale_factor)/scale_factor;
      sum+=contribution_info[n].weight;
      n++;
    }
    if ((sum != 0.0) && (sum != 1.0))
      for (i=0; i < n; i++)
        contribution_info[i].weight/=sum;  /* normalize */
    q=destination->pixels+x;
    for (y=0; y < (int) destination->rows; y++)
    {
      blue_weight=0.0;
      green_weight=0.0;
      red_weight=0.0;
      index_weight=0.0;
      for (i=0; i < n; i++)
      {
        p=source->pixels+(y*source->columns)+contribution_info[i].pixel;
        red_weight+=contribution_info[i].weight*p->red;
        green_weight+=contribution_info[i].weight*p->green;
        blue_weight+=contribution_info[i].weight*p->blue;
        index_weight+=contribution_info[i].weight*p->index;
      }
      q->red=range_limit[(int) (red_weight+0.5)];
      q->green=range_limit[(int) (green_weight+0.5)];
      q->blue=range_limit[(int) (blue_weight+0.5)];
      if (index_weight > Opaque)
        q->index=Opaque;
      else
        if (index_weight < Transparent)
          q->index=Transparent;
        else
          q->index=(unsigned short) (index_weight+0.5);
      q->length=0;
      q+=destination->columns;
    }
    if (QuantumTick(*quantum,span))
      ProgressMonitor(ZoomImageText,*quantum,span);
    (*quantum)++;
  }
}

static void VerticalFilter(Image *source,Image *destination,double y_factor,
  const FilterInfo *filter_info,ContributionInfo *contribution_info,
  const Quantum *range_limit,const unsigned int span,unsigned int *quantum)
{
  double
    blue_weight,
    center,
    green_weight,
    index_weight,
    red_weight,
    scale_factor,
    sum,
    support;

  int
    n,
    y;

  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p,
    *q;

  /*
    Apply filter to zoom vertically from source to destination.
  */
  scale_factor=source->blur*Max(1.0/y_factor,1.0);
  support=Max(scale_factor*filter_info->support,0.5);
  destination->class=source->class;
  if (support > 0.5)
    destination->class=DirectClass;
  else
    {
      /*
        Reduce to point sampling.
      */
      support=0.5;
      scale_factor=1.0;
    }
  support+=1.e-7;
  q=destination->pixels;
  for (y=0; y < (int) destination->rows; y++)
  {
    sum=0.0;
    n=0;
    center=(double) y/y_factor;
    for (i=(int) (center-support+0.5); i < (int) (center+support+0.5); i++)
    {
      j=i;
      if (j < 0)
        j=(-j);
      else
        if (j >= (int) source->rows)
          j=(source->rows << 1)-j-1;
      contribution_info[n].pixel=j;
      contribution_info[n].weight=filter_info->function(
        ((double) i-center+0.5)/scale_factor)/scale_factor;
      sum+=contribution_info[n].weight;
      n++;
    }
    if ((sum != 0.0) && (sum != 1.0))
      for (i=0; i < n; i++)
        contribution_info[i].weight/=sum;  /* normalize */
    for (x=0; x < (int) destination->columns; x++)
    {
      blue_weight=0.0;
      green_weight=0.0;
      red_weight=0.0;
      index_weight=0.0;
      for (i=0; i < n; i++)
      {
        p=source->pixels+(contribution_info[i].pixel*source->columns)+x;
        red_weight+=contribution_info[i].weight*p->red;
        green_weight+=contribution_info[i].weight*p->green;
        blue_weight+=contribution_info[i].weight*p->blue;
        index_weight+=contribution_info[i].weight*p->index;
      }
      q->red=range_limit[(int) (red_weight+0.5)];
      q->green=range_limit[(int) (green_weight+0.5)];
      q->blue=range_limit[(int) (blue_weight+0.5)];
      if (index_weight > Opaque)
        q->index=Opaque;
      else
        if (index_weight < Transparent)
          q->index=Transparent;
        else
          q->index=(unsigned short) (index_weight+0.5);
      q->length=0;
      q++;
    }
    if (QuantumTick(*quantum,span))
      ProgressMonitor(ZoomImageText,*quantum,span);
    (*quantum)++;
  }
}

Export Image *ZoomImage(Image *image,const unsigned int columns,
  const unsigned int rows)
{
  ContributionInfo
    *contribution_info;

  double
    support,
    x_factor,
    y_factor;

  Image
    *source_image,
    *zoomed_image;

  Quantum
    *range_table;

  register int
     i;

  register Quantum
    *range_limit;

  static const FilterInfo
    filters[SincFilter+1] =
    {
      { Box, 0.0 },
      { Box, 0.0 },
      { Box, 0.5 },
      { Triangle, 1.0 },
      { Hermite, 1.0 },
      { Hanning, 1.0 },
      { Hamming, 1.0 },
      { Blackman, 1.0 },
      { Gaussian, 1.25 },
      { Quadratic, 1.5 },
      { Cubic, 2.0 },
      { Catrom, 2.0 },
      { Mitchell, 2.0 },
      { Lanczos, 3.0 },
      { Bessel, 3.2383 },
      { Sinc, 4.0 }
    };

  unsigned int
    quantum,
    span;

  assert(image != (Image *) NULL);
  assert((image->filter >= 0) && (image->filter <= SincFilter));
  if ((columns == 0) || (rows == 0))
    return((Image *) NULL);
  /*
    Image must be uncompressed.
  */
  if (!UncondenseImage(image))
    return((Image *) NULL);
  /*
    Initialize zoomed image attributes.
  */
  zoomed_image=CloneImage(image,columns,rows,False);
  if (zoomed_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to zoom image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  image->orphan=True;
  if (zoomed_image->rows >= image->rows)
    source_image=CloneImage(image,zoomed_image->columns,image->rows,False);
  else
    source_image=CloneImage(image,image->columns,zoomed_image->rows,False);
  image->orphan=False;
  if (source_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to zoom image",
        "Memory allocation failed");
      DestroyImage(zoomed_image);
      return((Image *) NULL);
    }
  /*
    Allocate the range table.
  */
  range_table=(Quantum *) AllocateMemory(3*(MaxRGB+1)*sizeof(Quantum));
  if (range_table == (Quantum *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to zoom image",
        "Memory allocation failed");
      DestroyImage(source_image);
      DestroyImage(zoomed_image);
      return((Image *) NULL);
    }
  /*
    Pre-compute conversion tables.
  */
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(Quantum) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  range_limit=range_table+(MaxRGB+1);
  /*
    Allocate filter info list.
  */
  x_factor=(double) zoomed_image->columns/(double) image->columns;
  y_factor=(double) zoomed_image->rows/(double) image->rows;
  support=Max(filters[image->filter].support/x_factor,
    filters[image->filter].support/y_factor);
  if (support < filters[image->filter].support)
    support=filters[image->filter].support;
  contribution_info=(ContributionInfo *)
    AllocateMemory((int) (support*2+3)*sizeof(ContributionInfo));
  if (contribution_info == (ContributionInfo *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to zoom image",
        "Memory allocation failed");
      FreeMemory((char *) range_table);
      DestroyImage(source_image);
      DestroyImage(zoomed_image);
      return((Image *) NULL);
    }
  /*
    Zoom image.
  */
  quantum=0;
  if (zoomed_image->rows >= image->rows)
    {
      span=source_image->columns+zoomed_image->rows;
      HorizontalFilter(image,source_image,x_factor,&filters[image->filter],
        contribution_info,range_limit,span,&quantum);
      VerticalFilter(source_image,zoomed_image,y_factor,&filters[image->filter],
        contribution_info,range_limit,span,&quantum);
    }
  else
    {
      span=zoomed_image->columns+source_image->columns;
      VerticalFilter(image,source_image,y_factor,&filters[image->filter],
        contribution_info,range_limit,span,&quantum);
      HorizontalFilter(source_image,zoomed_image,x_factor,
        &filters[image->filter],contribution_info,range_limit,span,&quantum);
    }
  /*
    Free allocated memory.
  */
  FreeMemory((char *) contribution_info);
  FreeMemory((char *) range_table);
  DestroyImage(source_image);
  return(zoomed_image);
}
