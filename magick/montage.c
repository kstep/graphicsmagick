/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              M   M   OOO   N   N  TTTTT   AAA    GGGG  EEEEE                %
%              MM MM  O   O  NN  N    T    A   A  G      E                    %
%              M M M  O   O  N N N    T    AAAAA  G  GG  EEE                  %
%              M   M  O   O  N  NN    T    A   A  G   G  E                    %
%              M   M   OOO   N   N    T    A   A   GGG   EEEEE                %
%                                                                             %
%                                                                             %
%              ImageMagick Methods to Create Image Thumbnails                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e M o n t a g e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneMontageInfo() makes a copy of the given montage info structure.  If
%  NULL is specified, a new image info structure is created initialized to
%  default values.
%
%  The format of the CloneMontageInfo method is:
%
%      MontageInfo *CloneMontageInfo(const ImageInfo *image_info,
%        const MontageInfo *montage_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o montage_info: The montage info.
%
%
*/
MagickExport MontageInfo *CloneMontageInfo(const ImageInfo *image_info,
  const MontageInfo *montage_info)
{
  MontageInfo
    *clone_info;

  clone_info=(MontageInfo *) AcquireMemory(sizeof(MontageInfo));
  if (clone_info == (MontageInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to clone montage info",
      "Memory allocation failed");
  if (montage_info == (MontageInfo *) NULL)
    {
      GetMontageInfo(image_info,clone_info);
      return(clone_info);
    }
  *clone_info=(*montage_info);
  if (montage_info->geometry != (char *) NULL)
    clone_info->geometry=AllocateString(montage_info->geometry);
  if (montage_info->tile != (char *) NULL)
    clone_info->tile=AllocateString(montage_info->tile);
  if (montage_info->frame != (char *) NULL)
    clone_info->frame=AllocateString(montage_info->frame);
  if (montage_info->texture != (char *) NULL)
    clone_info->texture=AllocateString(montage_info->texture);
  if (montage_info->font != (char *) NULL)
    clone_info->font=AllocateString(montage_info->font);
  return(clone_info);
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
%  DestroyMontageInfo() deallocates memory associated with montage_info.
%
%  The format of the DestroyMontageInfo method is:
%
%      void DestroyMontageInfo(MontageInfo *montage_info)
%
%  A description of each parameter follows:
%
%    o montage_info: Specifies a pointer to an MontageInfo structure.
%
%
*/
MagickExport void DestroyMontageInfo(MontageInfo *montage_info)
{
  assert(montage_info != (MontageInfo *) NULL);
  assert(montage_info->signature == MagickSignature);
  if (montage_info->geometry != (char *) NULL)
    LiberateMemory((void **) &montage_info->geometry);
  if (montage_info->tile != (char *) NULL)
    LiberateMemory((void **) &montage_info->tile);
  if (montage_info->title != (char *) NULL)
    LiberateMemory((void **) &montage_info->title);
  if (montage_info->frame != (char *) NULL)
    LiberateMemory((void **) &montage_info->frame);
  if (montage_info->texture != (char *) NULL)
    LiberateMemory((void **) &montage_info->texture);
  if (montage_info->font != (char *) NULL)
    LiberateMemory((void **) &montage_info->font);
  LiberateMemory((void **) &montage_info);
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
%  GetMontageInfo() initializes montage_info to default values.
%
%  The format of the GetMontageInfo method is:
%
%      void GetMontageInfo(const ImageInfo *image_info,
%        MontageInfo *montage_info)
%
%  A description of each parameter follows:
%
%    o image_info: a structure of type ImageInfo.
%
%    o montage_info: Specifies a pointer to a MontageInfo structure.
%
%
*/
MagickExport void GetMontageInfo(const ImageInfo *image_info,
  MontageInfo *montage_info)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(montage_info != (MontageInfo *) NULL);
  (void) memset(montage_info,0,sizeof(MontageInfo));
  (void) strncpy(montage_info->filename,image_info->filename,MaxTextExtent-1);
  montage_info->geometry=AllocateString(DefaultTileGeometry);
  montage_info->gravity=CenterGravity;
  montage_info->tile=AllocateString("6x4");
  if (image_info->font != (char *) NULL)
    montage_info->font=AllocateString(image_info->font);
  montage_info->pointsize=image_info->pointsize;
  montage_info->fill.opacity=OpaqueOpacity;
  montage_info->stroke.opacity=TransparentOpacity;
  montage_info->background_color=image_info->background_color;
  montage_info->border_color=image_info->border_color;
  montage_info->matte_color=image_info->matte_color;
  montage_info->signature=MagickSignature;
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
%  Montageimages() is a layout manager that lets you tile one or more
%  thumbnails across an image canvas.
%
%  The format of the MontageImages method is:
%
%      Image *MontageImages(const Image *images,const MontageInfo *montage_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: Specifies a pointer to an array of Image structures.
%
%    o montage_info: Specifies a pointer to a MontageInfo structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int SceneCompare(const void *x,const void *y)
{
  Image
    **image_1,
    **image_2;

  image_1=(Image **) x;
  image_2=(Image **) y;
  return((int) ((*image_1)->scene-(*image_2)->scene));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport Image *MontageImages(const Image *images,
  const MontageInfo *montage_info,ExceptionInfo *exception)
{
#define MontageImageText  "  Create visual image directory...  "
#define TileImageText  "  Create image tiles...  "

  char
    tile_geometry[MaxTextExtent],
    *title;

  const ImageAttribute
    *attribute;

  DrawInfo
    *draw_info;

  FrameInfo
    frame_info;

  Image
    *image,
    **image_list,
    **master_list,
    *montage,
    *texture,
    *tile_image,
    *zoom_image;

  ImageInfo
    *image_info;

  int
    flags;

  long
    x,
    x_offset,
    y,
    y_offset;

  MonitorHandler
    handler;

  register long
    i;

  register PixelPacket
    *q;

  RectangleInfo
    bounds,
    geometry,
    tile_info;

  size_t
    number_images;

  TypeMetric
    metrics;

  unsigned int
    concatenate;

  unsigned long
    bevel_width,
    border_width,
    count,
    height,
    images_per_page,
    max_height,
    number_lines,
    tile,
    tiles,
    tiles_per_column,
    tiles_per_page,
    tiles_per_row,
    title_offset,
    total_tiles,
    width;

  /*
    Create image tiles.
  */
  assert(images != (Image *) NULL);
  assert(images->signature == MagickSignature);
  assert(montage_info != (MontageInfo *) NULL);
  assert(montage_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  number_images=GetImageListSize(images);
  master_list=ImageListToArray(images,exception);
  if (master_list == (Image **) NULL)
    ThrowImageException(ResourceLimitWarning,"Unable to create montage",
      "Memory allocation failed")
  image_list=master_list;
  for (i=0; i < (long) number_images; i++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    image=image_list[i];
    SetGeometry(image,&geometry);
    flags=ParseImageGeometry(montage_info->geometry,&geometry.x,&geometry.y,
      &geometry.width,&geometry.height);
    zoom_image=ZoomImage(image,geometry.width,geometry.height,exception);
    if (zoom_image == (Image *) NULL)
      break;
    DestroyImage(image);
    image_list[i]=zoom_image;
    (void) SetMonitorHandler(handler);
    MagickMonitor(TileImageText,i,number_images);
  }
  if (i < (long) number_images)
    {
      for (i=0; i < (long) number_images; i++)
        DestroyImage(master_list[i]);
      LiberateMemory((void **) &master_list);
      return((Image *) NULL);
    }
  /*
    Sort image list by increasing tile number.
  */
  for (i=0; i < (long) number_images; i++)
    if (image_list[i]->scene == 0)
      break;
  if (i == (long) number_images)
    qsort((void *) image_list,number_images,sizeof(Image *),SceneCompare);
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
      x=0;
      y=0;
      tiles_per_column=number_images;
      flags=GetGeometry(montage_info->tile,&x,&y,&tiles_per_row,
        &tiles_per_column);
    }
  /*
    Determine tile sizes.
  */
  concatenate=False;
  SetGeometry(image_list[0],&tile_info);
  tile_info.x=(long) montage_info->border_width;
  tile_info.y=(long) montage_info->border_width;
  if (montage_info->geometry != (char *) NULL)
    {
      /*
        Initialize tile geometry.
      */
      flags=GetGeometry(montage_info->geometry,&tile_info.x,&tile_info.y,
        &tile_info.width,&tile_info.height);
      if ((tile_info.x == 0) && (tile_info.y == 0))
        concatenate=!((flags & WidthValue) || (flags & HeightValue));
    }
  border_width=montage_info->border_width;
  bevel_width=0;
  if (montage_info->frame != (char *) NULL)
    {
      char
        absolute_geometry[MaxTextExtent];

      (void) memset(&frame_info,0,sizeof(FrameInfo));
      frame_info.width=tile_info.width;
      frame_info.height=tile_info.height;
      FormatString(absolute_geometry,"%.1024s!",montage_info->frame);
      flags=ParseImageGeometry(absolute_geometry,&frame_info.outer_bevel,
        &frame_info.inner_bevel,&frame_info.width,&frame_info.height);
      if ((flags & HeightValue) == 0)
        frame_info.height=frame_info.width;
      if ((flags & XValue) == 0)
        frame_info.outer_bevel=(long) (frame_info.width >> 2)+1;
      if ((flags & YValue) == 0)
        frame_info.inner_bevel=frame_info.outer_bevel;
      frame_info.x=(long) frame_info.width;
      frame_info.y=(long) frame_info.height;
      bevel_width=Max(frame_info.inner_bevel,frame_info.outer_bevel);
      border_width=Max(frame_info.width,frame_info.height);
    }
  for (i=1; i < (long) number_images; i++)
  {
    if (image_list[i]->columns > tile_info.width)
      tile_info.width=image_list[i]->columns;
    if (image_list[i]->rows > tile_info.height)
      tile_info.height=image_list[i]->rows;
  }
  /*
    Initialize draw attributes.
  */
  image_info=CloneImageInfo((ImageInfo *) NULL);
  image_info->background_color=montage_info->background_color;
  image_info->border_color=montage_info->border_color;
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  if (montage_info->font != (char *) NULL)
    (void) CloneString(&draw_info->font,montage_info->font);
  draw_info->pointsize=montage_info->pointsize;
  draw_info->gravity=NorthGravity;
  draw_info->stroke=montage_info->stroke;
  draw_info->fill=montage_info->fill;
  draw_info->text=AllocateString("");
  GetTypeMetrics(image_list[0],draw_info,&metrics);
  texture=(Image *) NULL;
  if (montage_info->texture != (char *) NULL)
    {
      (void) strncpy(image_info->filename,montage_info->texture,
        MaxTextExtent-1);
      texture=ReadImage(image_info,exception);
    }
  /*
    Determine the number of lines in an next label.
  */
  title=TranslateText(image_info,image_list[0],montage_info->title);
  title_offset=0;
  if (montage_info->title != (char *) NULL)
    title_offset=2*metrics.height*MultilineCensus(title)+2*tile_info.y;
  number_lines=0;
  for (i=0; i < (long) number_images; i++)
  {
    attribute=GetImageAttribute(image_list[i],"label");
    if (attribute == (ImageAttribute *) NULL)
      continue;
    if (MultilineCensus(attribute->value) > number_lines)
      number_lines=MultilineCensus(attribute->value);
  }
  /*
    Allocate next structure.
  */
  tile_image=AllocateImage(NULL);
  montage=AllocateImage(image_info);
  montage->scene=1;
  images_per_page=(number_images-1)/(tiles_per_row*tiles_per_column)+1;
  tiles=0;
  total_tiles=number_images;
  for (i=0; i < (long) images_per_page; i++)
  {
    /*
      Determine bounding box.
    */
    tiles_per_page=Min(number_images,tiles_per_row*tiles_per_column);
    x_offset=0;
    y_offset=(long) title_offset;
    max_height=0;
    bounds.width=0;
    bounds.height=0;
    for (tile=0; tile < tiles_per_page; tile++)
    {
      width=concatenate ? image_list[tile]->columns : tile_info.width;
      x_offset+=width+(tile_info.x+border_width)*2;
      if (x_offset > (long) bounds.width)
        bounds.width=x_offset;
      if (image_list[tile]->rows > max_height)
        max_height=image_list[tile]->rows;
      if (((tile+1) == tiles_per_page) || (((tile+1) % tiles_per_row) == 0))
        {
          x_offset=0;
          height=concatenate ? max_height : tile_info.height;
          y_offset+=height+(tile_info.y+border_width)*2+(metrics.height+4)*
            number_lines+(montage_info->shadow ? 4 : 0)+(concatenate ? 0 : 2);
          if (y_offset > (long) bounds.height)
            bounds.height=y_offset;
          max_height=0;
        }
    }
    bounds.height-=(concatenate ? 0 : 2);
    /*
      Initialize montage image.
    */
    (void) strncpy(montage->filename,montage_info->filename,MaxTextExtent-1);
    montage->columns=bounds.width;
    montage->rows=bounds.height;
    SetImage(montage,OpaqueOpacity);
    /*
      Set montage geometry.
    */
    montage->montage=AllocateString((char *) NULL);
    count=1;
    for (tile=0; tile < tiles_per_page; tile++)
      count+=strlen(image_list[tile]->filename)+1;
    montage->directory=(char *) AcquireMemory(count);
    if ((montage->montage == (char *) NULL) ||
        (montage->directory == (char *) NULL))
      ThrowImageException(ResourceLimitWarning,"Unable to create montage",
        "Memory allocation failed");
    x_offset=0;
    y_offset=(long) title_offset;
    FormatString(montage->montage,"%ldx%ld%+ld%+ld",
      (long) (tile_info.width+(tile_info.x+border_width)*2),
      (long) (tile_info.height+(tile_info.y+border_width)*2+(metrics.height+4)*
      number_lines+(montage_info->shadow ? 4 : 0)),x_offset,y_offset);
    *montage->directory='\0';
    for (tile=0; tile < tiles_per_page; tile++)
    {
      (void) strncat(montage->directory,image_list[tile]->filename,
        MaxTextExtent-strlen(montage->directory)-1);
      (void) strcat(montage->directory,"\n");
    }
    handler=SetMonitorHandler((MonitorHandler) NULL);
    if (texture != (Image *) NULL)
      TextureImage(montage,texture);
    if (montage_info->title != (char *) NULL)
      {
        char
          geometry[MaxTextExtent];

        /*
          Annotate composite image with title.
        */
        FormatString(geometry,"%lux%lu%+ld%+ld",montage->columns,
          (unsigned long) (2*metrics.height),0,(long) (metrics.height+
          tile_info.y+4+metrics.ascent));
        (void) CloneString(&draw_info->geometry,geometry);
        (void) CloneString(&draw_info->text,title);
        (void) AnnotateImage(montage,draw_info);
      }
    (void) SetMonitorHandler(handler);
    /*
      Copy tile to the composite.
    */
    x_offset=tile_info.x;
    y_offset=(long) title_offset+tile_info.y;
    max_height=0;
    for (tile=0; tile < tiles_per_page; tile++)
    {
      /*
        Copy this tile to the composite.
      */
      handler=SetMonitorHandler((MonitorHandler) NULL);
      image=CloneImage(image_list[tile],0,0,True,exception);
      width=concatenate ? image->columns : tile_info.width;
      if (image->rows > max_height)
        max_height=image->rows;
      height=concatenate ? max_height : tile_info.height;
      if (border_width != 0)
        {
          Image
            *border_image;

          RectangleInfo
            border_info;

          /*
            Put a border around the image.
          */
          border_info.width=border_width;
          border_info.height=border_width;
          if (montage_info->frame != (char *) NULL)
            {
              border_info.width=(width-image->columns+1)/2;
              border_info.height=(height-image->rows+1)/2;
            }
          border_image=BorderImage(image,&border_info,exception);
          if (border_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=border_image;
            }
        }
      /*
        Gravitate as specified by the tile gravity.
      */
      tile_image->columns=width;
      tile_image->rows=height;
      tile_image->gravity=montage_info->gravity;
      if (image->gravity != ForgetGravity)
        tile_image->gravity=image->gravity;
      FormatString(tile_geometry,"%lux%lu+0+0",image->columns,image->rows);
      flags=GetImageGeometry(tile_image,tile_geometry,False,&geometry);
      x=geometry.x+border_width;
      y=geometry.y+border_width;
      if ((montage_info->frame != (char *) NULL) && (bevel_width != 0))
        {
          FrameInfo
            tile_info;

          Image
            *frame_image;

          /*
            Put an ornamental border around this tile.
          */
          tile_info=frame_info;
          tile_info.width=width+2*frame_info.width;
          tile_info.height=height+2*frame_info.height;
          attribute=GetImageAttribute(image,"label");
          if (attribute != (const ImageAttribute *) NULL)
            tile_info.height+=(metrics.height+4)*
              MultilineCensus(attribute->value);
          frame_image=FrameImage(image,&tile_info,exception);
          if (frame_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=frame_image;
            }
          x=0;
          y=0;
        }
      if (LocaleCompare(image->magick,"NULL") != 0)
        {
          /*
            Composite background with tile.
          */
          (void) CompositeImage(montage,image->compose,image,x_offset+x,
            y_offset+y);
          if (montage_info->shadow)
            {
              register long
                columns,
                rows;

              /*
                Put a shadow under the tile to show depth.
              */
              for (rows=0; rows < ((long) image->rows-4); rows++)
              {
                q=GetImagePixels(montage,(long) (x+x_offset+image->columns),
                  (long) (y+y_offset+rows+4),Min(tile_info.x,4),1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (columns=0; columns < Min(tile_info.x,4); columns++)
                {
                  Modulate(0.0,0.0,-25.0+4*columns,&q->red,&q->green,&q->blue);
                  q++;
                }
                if (!SyncImagePixels(montage))
                  break;
              }
              for (rows=0; rows < Min(tile_info.y,4); rows++)
              {
                q=GetImagePixels(montage,x+x_offset+4,y+y_offset+
                  (long) image->rows+rows,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (columns=0; columns < (long) image->columns; columns++)
                {
                  Modulate(0.0,0.0,-25.0+4*rows,&q->red,&q->green,&q->blue);
                  q++;
                }
                if (!SyncImagePixels(montage))
                  break;
              }
            }
          attribute=GetImageAttribute(image,"label");
          if (attribute != (const ImageAttribute *) NULL)
            {
              char
                geometry[MaxTextExtent];

              /*
                Annotate composite tile with label.
              */
              FormatString(geometry,"%lux%lu%+ld%+ld",
                (montage_info->frame ? image->columns : width)-2*border_width,
                (unsigned long) (metrics.height+4)*
                MultilineCensus(attribute->value),x_offset+border_width,
                (montage_info->frame ? y_offset+height+border_width+2 :
                y_offset+tile_info.height+border_width+(montage_info->shadow ?
                4 : 0))+(long) metrics.height);
              (void) CloneString(&draw_info->geometry,geometry);
              (void) CloneString(&draw_info->text,attribute->value);
              (void) AnnotateImage(montage,draw_info);
            }
        }
      x_offset+=width+(tile_info.x+border_width)*2;
      if (((tile+1) == tiles_per_page) || (((tile+1) % tiles_per_row) == 0))
        {
          x_offset=tile_info.x;
          y_offset+=height+(tile_info.y+border_width)*2+(metrics.height+4)*
            number_lines+(montage_info->shadow ? 4 : 0);
          max_height=0;
        }
      DestroyImage(image);
      (void) SetMonitorHandler(handler);
      MagickMonitor(MontageImageText,tiles,total_tiles);
      tiles++;
    }
    if ((i+1) < (long) images_per_page)
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,montage);
        if (montage->next == (Image *) NULL)
          {
            DestroyImageList(montage);
            return((Image *) NULL);
          }
        montage=montage->next;
        image_list+=tiles_per_page;
        number_images-=tiles_per_page;
      }
  }
  DestroyImage(tile_image);
  if (texture != (Image *) NULL)
    LiberateMemory((void **) &texture);
  for (i=0; i < (long) number_images; i++)
    DestroyImage(master_list[i]);
  LiberateMemory((void **) &master_list);
  DestroyDrawInfo(draw_info);
  DestroyImageInfo(image_info);
  while (montage->previous != (Image *) NULL)
    montage=montage->previous;
  return(montage);
}
