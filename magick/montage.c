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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

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
%  Method CloneMontageInfo makes a duplicate of the given montage info, or if
%  montage info is NULL, a new one.
%
%  The format of the CloneMontageInfo method is:
%
%      MontageInfo *CloneMontageInfo(const ImageInfo *image_info,
%        const MontageInfo *montage_info)
%
%  A description of each parameter follows:
%
%    o cloned_info: Method CloneMontageInfo returns a duplicate of the given
%      annotate info, or if annotate info is NULL a new one.
%
%    o image_info: a structure of type ImageInfo.
%
%    o montage_info: a structure of type MontageInfo.
%
%
*/
MagickExport MontageInfo *CloneMontageInfo(const ImageInfo *image_info,
  const MontageInfo *montage_info)
{
  MontageInfo
    *cloned_info;

  cloned_info=(MontageInfo *) AllocateMemory(sizeof(MontageInfo));
  if (cloned_info == (MontageInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to clone montage info",
      "Memory allocation failed");
  if (montage_info == (MontageInfo *) NULL)
    {
      GetMontageInfo(image_info,cloned_info);
      return(cloned_info);
    }
  *cloned_info=(*montage_info);
  if (montage_info->geometry != (char *) NULL)
    cloned_info->geometry=AllocateString(montage_info->geometry);
  if (montage_info->tile != (char *) NULL)
    cloned_info->tile=AllocateString(montage_info->tile);
  if (montage_info->frame != (char *) NULL)
    cloned_info->frame=AllocateString(montage_info->frame);
  if (montage_info->texture != (char *) NULL)
    cloned_info->texture=AllocateString(montage_info->texture);
  if (montage_info->font != (char *) NULL)
    cloned_info->font=AllocateString(montage_info->font);
  return(cloned_info);
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
  if (montage_info->geometry != (char *) NULL)
    FreeMemory((void **) &montage_info->geometry);
  if (montage_info->tile != (char *) NULL)
    FreeMemory((void **) &montage_info->tile);
  if (montage_info->title != (char *) NULL)
    FreeMemory((void **) &montage_info->title);
  if (montage_info->frame != (char *) NULL)
    FreeMemory((void **) &montage_info->frame);
  if (montage_info->texture != (char *) NULL)
    FreeMemory((void **) &montage_info->texture);
  if (montage_info->font != (char *) NULL)
    FreeMemory((void **) &montage_info->font);
  FreeMemory((void **) &montage_info);
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
  assert(montage_info != (MontageInfo *) NULL);
  (void) strcpy(montage_info->filename,image_info->filename);
  montage_info->geometry=AllocateString(DefaultTileGeometry);
  montage_info->tile=AllocateString("6x4");
  montage_info->title=(char *) NULL;
  montage_info->frame=(char *) NULL;
  montage_info->texture=(char *) NULL;
  montage_info->font=AllocateString(image_info->font);
  montage_info->pointsize=image_info->pointsize;
  montage_info->border_width=0;
  montage_info->gravity=CenterGravity;
  montage_info->shadow=False;
  montage_info->compose=ReplaceCompositeOp;
  montage_info->fill=image_info->fill;
  montage_info->stroke=image_info->stroke;
  montage_info->background_color=image_info->background_color;
  montage_info->border_color=image_info->border_color;
  montage_info->matte_color=image_info->matte_color;
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
%  The format of the MontageImages method is:
%
%      Image *MontageImages(Image *image,const MontageInfo *montage_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to an array of Image structures.
%
%    o montage_info: Specifies a pointer to a MontageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static void FormatLabel(ImageInfo *image_info,char *label,
  const unsigned int width,unsigned int *font_height)
{
  ExceptionInfo
    error;

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
    (void) strncat(image_info->filename+6,p,(int) (q-p+1));
    image=ReadImage(image_info,&error);
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

MagickExport Image *MontageImages(Image *image,const MontageInfo *montage_info,
  ExceptionInfo *exception)
{
#define MontageImageText  "  Creating visual next directory...  "
#define TileImageText  "  Creating next tiles...  "

  AnnotateInfo
    *annotate_info;

  char
    geometry[MaxTextExtent];

  FrameInfo
    frame_info;

  Image
    **next_list,
    **master_list,
    *montage_next,
    *texture,
    *tiled_next;

  ImageAttribute
    *attribute;

  ImageInfo
    *clone_info;

  int
    x,
    x_offset,
    y,
    y_offset;

  MonitorHandler
    handler;

  register Image
    *next;

  register int
    i;

  register PixelPacket
    *q;

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
    image_per_page,
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

  /*
    Create next tiles.
  */
  assert(image != (Image *) NULL);
  assert(montage_info != (MontageInfo *) NULL);
  next_list=ListToGroupImage(image,&number_images);
  master_list=next_list;
  for (tile=0; tile < number_images; tile++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    width=next_list[tile]->columns;
    height=next_list[tile]->rows;
    x=0;
    y=0;
    (void) ParseImageGeometry(montage_info->geometry,&x,&y,&width,&height);
    next_list[tile]->orphan=True;
    tiled_next=ZoomImage(next_list[tile],width,height,exception);
    if (tiled_next == (Image *) NULL)
      {
        for (i=0; i < (int) tile; i++)
          DestroyImage(next_list[i]);
        (void) SetMonitorHandler(handler);
        return((Image *) NULL);
      }
    next_list[tile]=tiled_next;
    (void) SetMonitorHandler(handler);
    ProgressMonitor(TileImageText,tile,number_images);
  }
  /*
    Sort next_list by increasing tile number.
  */
  for (tile=0; tile < number_images; tile++)
    if (next_list[tile]->scene == 0)
      break;
  if (tile == number_images)
    qsort((void *) next_list,number_images,sizeof(Image *),
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
      (void) ParseGeometry(montage_info->tile,&x,&y,&tiles_per_row,
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
      flags=ParseGeometry(montage_info->frame,&frame_info.outer_bevel,
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
  tile_info.width=next_list[0]->columns;
  tile_info.height=next_list[0]->rows;
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
    if (next_list[tile]->columns > tile_info.width)
      tile_info.width=next_list[tile]->columns;
    if (next_list[tile]->rows > tile_info.height)
      tile_info.height=next_list[tile]->rows;
  }
  /*
    Initialize annotate info.
  */
  clone_info=CloneImageInfo((ImageInfo *) NULL);
  (void) CloneString(&clone_info->font,montage_info->font);
  clone_info->pointsize=montage_info->pointsize;
  clone_info->stroke=montage_info->stroke;
  clone_info->fill=montage_info->fill;
  clone_info->background_color=montage_info->background_color;
  clone_info->border_color=montage_info->border_color;
  annotate_info=CloneAnnotateInfo(clone_info,(AnnotateInfo *) NULL);
  annotate_info->gravity=NorthGravity;
  texture=(Image *) NULL;
  if (montage_info->texture != (char *) NULL)
    {
      (void) strcpy(clone_info->filename,montage_info->texture);
      texture=ReadImage(clone_info,exception);
    }
  /*
    Initialize font info.
  */
  font_height=annotate_info->bounds.height;
  FormatLabel(clone_info,montage_info->title,((tile_info.width+
    2*border_width)*Min(number_images,tiles_per_column))/2,
    &font_height);
  for (tile=0; tile < number_images; tile++)
  {
    attribute=GetImageAttribute(next_list[tile],"Label");
    if (attribute == (ImageAttribute *) NULL)
      continue;
    FormatLabel(clone_info,attribute->value,tile_info.width+2*border_width,
      &font_height);
  }
  /*
    Determine the number of lines in an next label.
  */
  title_offset=0;
  if (montage_info->title != (char *) NULL)
    title_offset=((font_height*MultilineCensus(montage_info->title)) << 1)+
      (tile_info.y << 1);
  number_lines=0;
  for (tile=0; tile < number_images; tile++)
  {
    attribute=GetImageAttribute(next_list[tile],"Label");
    if (attribute == (ImageAttribute *) NULL)
      continue;
    if (MultilineCensus(attribute->value) > (int) number_lines)
      number_lines=MultilineCensus(attribute->value);
  }
  /*
    Allocate next structure.
  */
  montage_next=AllocateImage(clone_info);
  montage_next->scene=1;
  image_per_page=(number_images-1)/(tiles_per_row*tiles_per_column)+1;
  tiles=0;
  total_tiles=number_images;
  for (i=0; i < (int) image_per_page; i++)
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
      width=concatenate ? next_list[tile]->columns : tile_info.width;
      x_offset+=width+(tile_info.x+border_width)*2;
      if (x_offset > (int) bounding_box.width)
        bounding_box.width=x_offset;
      if (next_list[tile]->rows > max_height)
        max_height=next_list[tile]->rows;
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
    (void) strcpy(montage_next->filename,montage_info->filename);
    montage_next->columns=bounding_box.width;
    montage_next->rows=bounding_box.height;
    SetImage(montage_next,OpaqueOpacity);
    /*
      Set montage geometry.
    */
    montage_next->montage=(char *) AllocateMemory(MaxTextExtent);
    count=1;
    for (tile=0; tile < tiles_per_page; tile++)
      count+=Extent(next_list[tile]->filename)+1;
    montage_next->directory=(char *) AllocateMemory(count);
    if ((montage_next->montage == (char *) NULL) ||
        (montage_next->directory == (char *) NULL))
      {
        DestroyImages(montage_next);
        ThrowImageException(ResourceLimitWarning,"Unable to montage next_list",
          "Memory allocation failed");
      }
    x_offset=0;
    y_offset=title_offset;
    FormatString(montage_next->montage,"%dx%d%+d%+d",
      (int) (tile_info.width+(tile_info.x+border_width)*2),
      (int) (tile_info.height+(tile_info.y+border_width)*2+(font_height+4)*
      number_lines+(montage_info->shadow ? 4 : 0)),x_offset,y_offset);
    *montage_next->directory='\0';
    for (tile=0; tile < tiles_per_page; tile++)
    {
      (void) strcat(montage_next->directory,next_list[tile]->filename);
      (void) strcat(montage_next->directory,"\n");
    }
    handler=SetMonitorHandler((MonitorHandler) NULL);
    if (texture != (Image *) NULL)
      TextureImage(montage_next,texture);
    if (montage_info->title != (char *) NULL)
      {
        /*
          Annotate composite next with title.
        */
        FormatString(geometry,"%ux%u%+d%+d",montage_next->columns,
          font_height << 1,0,font_height+tile_info.y+4);
        (void) CloneString(&annotate_info->geometry,geometry);
        (void) CloneString(&annotate_info->text,montage_info->title);
        AnnotateImage(montage_next,annotate_info);
      }
    (void) SetMonitorHandler(handler);
    /*
      Copy tile next to the composite next.
    */
    x_offset=tile_info.x;
    y_offset=title_offset+tile_info.y;
    max_height=0;
    for (tile=0; tile < tiles_per_page; tile++)
    {
      /*
        Copy this tile to the composite next.
      */
      handler=SetMonitorHandler((MonitorHandler) NULL);
      next=next_list[tile];
      width=concatenate ? next->columns : tile_info.width;
      if (next->rows > max_height)
        max_height=next->rows;
      height=concatenate ? max_height : tile_info.height;
      if (border_width != 0)
        {
          Image
            *bordered_next;

          RectangleInfo
            border_info;

          /*
            Put a border around the next.
          */
          border_info.width=border_width;
          border_info.height=border_width;
          if (montage_info->frame != (char *) NULL)
            {
              border_info.width=(width-next->columns+1)/2;
              border_info.height=(height-next->rows+1)/2;
            }
          next->orphan=True;
          bordered_next=BorderImage(next,&border_info,exception);
          if (bordered_next != (Image *) NULL)
            {
              DestroyImage(next);
              next=bordered_next;
            }
        }
      /*
        Gravitate next as specified by the tile gravity.
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
          x=((width+2*border_width)-(int) next->columns)/2;
          y=0;
          break;
        }
        case NorthEastGravity:
        {
          x=(width+2*border_width)-(int) next->columns;
          y=0;
          break;
        }
        case WestGravity:
        {
          x=0;
          y=((height+2*border_width)-(int) next->rows)/2;
          break;
        }
        case ForgetGravity:
        case StaticGravity:
        case CenterGravity:
        default:
        {
          x=((width+2*border_width)-(int) next->columns)/2;
          y=((height+2*border_width)-(int) next->rows)/2;
          break;
        }
        case EastGravity:
        {
          x=(width+2*border_width)-(int) next->columns;
          y=((height+2*border_width)-(int) next->rows)/2;
          break;
        }
        case SouthWestGravity:
        {
          x=0;
          y=(height+2*border_width)-(int) next->rows;
          break;
        }
        case SouthGravity:
        {
          x=((width+2*border_width)-(int) next->columns)/2;
          y=(height+2*border_width)-(int) next->rows;
          break;
        }
        case SouthEastGravity:
        {
          x=(width+2*border_width)-(int) next->columns;
          y=(height+2*border_width)-(int) next->rows;
          break;
        }
      }
      if ((montage_info->frame != (char *) NULL) && (bevel_width != 0))
        {
          FrameInfo
            tile_info;

          Image
            *framed_next;

          /*
            Put an ornamental border around this tile.
          */
          tile_info=frame_info;
          tile_info.width=width+2*frame_info.width;
          tile_info.height=height+2*frame_info.height;
          attribute=GetImageAttribute(next,"Label");
          if (attribute != (ImageAttribute *) NULL)
            tile_info.height+=(font_height+4)*MultilineCensus(attribute->value);
          next->orphan=True;
          framed_next=FrameImage(next,&tile_info,exception);
          if (framed_next != (Image *) NULL)
            {
              DestroyImage(next);
              next=framed_next;
            }
          x=0;
          y=0;
        }
      if (LocaleCompare(next->magick,"NULL") != 0)
        {
          /*
            Composite background next with tile next.
          */
          if (next->matte)
            CompositeImage(montage_next,montage_info->compose,next,x_offset+x,
              y_offset+y);
          else
            CompositeImage(montage_next,ReplaceCompositeOp,next,x_offset+x,
              y_offset+y);
          montage_next->matte=False;
          if (montage_info->shadow)
            {
              register int
                columns,
                rows;

              /*
                Put a shadow under the tile to show depth.
              */
              for (rows=0; rows < ((int) next->rows-4); rows++)
              {
                q=GetImagePixels(montage_next,x+x_offset+next->columns,
                  y_offset+y+rows+4,Min(tile_info.x,4),1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (columns=0; columns < Min(tile_info.x,4); columns++)
                {
                  Modulate(0.0,0.0,-25.0+4*columns,&q->red,&q->green,&q->blue);
                  q++;
                }
                if (!SyncImagePixels(montage_next))
                  break;
              }
              for (rows=0; rows < Min(tile_info.y,4); rows++)
              {
                q=GetImagePixels(montage_next,x+x_offset+4,y_offset+y+
                  next->rows+rows,next->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (columns=0; columns < (int) next->columns; columns++)
                {
                  Modulate(0.0,0.0,-25.0+4*rows,&q->red,&q->green,&q->blue);
                  q++;
                }
                if (!SyncImagePixels(montage_next))
                  break;
              }
            }
          attribute=GetImageAttribute(next,"Label");
          if (attribute != (ImageAttribute *) NULL)
            {
              /*
                Annotate composite next tile with label.
              */
              FormatString(geometry,"%ux%u%+d%+d",
                (montage_info->frame ? next->columns : width)-
                2*border_width,font_height,(int) (x_offset+border_width),
                (int) (montage_info->frame ? y_offset+height+
                2*border_width-bevel_width-2 : y_offset+tile_info.height+
                2*border_width+(montage_info->shadow ? 4 : 0)));
              (void) CloneString(&annotate_info->geometry,geometry);
              (void) CloneString(&annotate_info->text,attribute->value);
              AnnotateImage(montage_next,annotate_info);
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
      DestroyImage(next);
      (void) SetMonitorHandler(handler);
      ProgressMonitor(MontageImageText,tiles,total_tiles);
      tiles++;
    }
    if ((i+1) < (int) image_per_page)
      {
        /*
          Allocate next next structure.
        */
        AllocateNextImage(clone_info,montage_next);
        if (montage_next->next == (Image *) NULL)
          {
            DestroyImages(montage_next);
            return((Image *) NULL);
          }
        montage_next=montage_next->next;
        next_list+=tiles_per_page;
        number_images-=tiles_per_page;
      }
  }
  if (texture != (Image *) NULL)
    FreeMemory((void **) &texture);
  FreeMemory((void **) &master_list);
  DestroyAnnotateInfo(annotate_info);
  DestroyImageInfo(clone_info);
  while (montage_next->previous != (Image *) NULL)
    montage_next=montage_next->previous;
  return(montage_next);
}
