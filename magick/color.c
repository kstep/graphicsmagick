/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                       CCCC   OOO   L       OOO   RRRR                       %
%                      C      O   O  L      O   O  R   R                      %
%                      C      O   O  L      O   O  RRRR                       %
%                      C      O   O  L      O   O  R R                        %
%                       CCCC   OOO   LLLLL   OOO   R  R                       %
%                                                                             %
%                                                                             %
%                  Methods to Count the Colors in an Image                    %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
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
  Define declarations.
*/
#define ColorFilename  "colors.mgk"

/*
  Declare color map.
*/
static char
  *ColorMap =
    "<?xml version=\"1.0\"?>"
    "<colormap>"
    "  <color name=\"none\" red=\"0\" green=\"0\" blue=\"0\" opacity=\"255\" />"
    "  <color name=\"white\" red=\"255\" green=\"255\" blue=\"255\" />"
    "  <color name=\"red\" red=\"255\" green=\"0\" blue=\"0\" />"
    "  <color name=\"green\" red=\"0\" green=\"255\" blue=\"0\" />"
    "  <color name=\"gray\" red=\"190\" green=\"190\" blue=\"190\" />"
    "  <color name=\"blue\" red=\"0\" green=\"0\" blue=\"255\" />"
    "  <color name=\"cyan\" red=\"0\" green=\"255\" blue=\"255\" />"
    "  <color name=\"magenta\" red=\"255\" green=\"0\" blue=\"255\" />"
    "  <color name=\"yellow\" red=\"255\" green=\"255\" blue=\"0\" />"
    "  <color name=\"black\" red=\"0\" green=\"0\" blue=\"0\" />"
    "</colormap>";

/*
  Structures.
*/
typedef struct _ColorPacket
{
  Quantum
    red,
    green,
    blue;

  unsigned short
    index;

  unsigned long
    count;
} ColorPacket;

typedef struct _NodeInfo
{
  struct _NodeInfo
    *child[8];

  ColorPacket
    *list;

  unsigned long
    number_unique;

  unsigned char
    level;
} NodeInfo;

typedef struct _Nodes
{
  NodeInfo
    nodes[NodesInAList];

  struct _Nodes
    *next;
} Nodes;

typedef struct _CubeInfo
{
  NodeInfo
    *root;

  unsigned long
    progress,
    colors,
    free_nodes;

  NodeInfo
    *node_info;

  Nodes
    *node_queue;
} CubeInfo;

/*
  Static declarations.
*/
static ColorInfo
  *color_list = (ColorInfo *) NULL;

static SemaphoreInfo
  *color_semaphore = (SemaphoreInfo *) NULL;

/*
  Forward declarations.
*/
static NodeInfo
  *GetNodeInfo(CubeInfo *,const unsigned int);

static unsigned int
  ReadConfigurationFile(const char *,ExceptionInfo *);

static void
  DestroyColorList(const NodeInfo *),
  Histogram(const Image *,CubeInfo *,const NodeInfo *,FILE *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C o l o r I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyColorInfo deallocates memory associated with the color list.
%
%  The format of the DestroyColorInfo method is:
%
%      DestroyColorInfo(void)
%
%
*/
MagickExport void DestroyColorInfo(void)
{
  register ColorInfo
    *p;

  AcquireSemaphoreInfo(&color_semaphore);
  for (p=color_list; p != (ColorInfo *) NULL; )
  {
    if (p->filename != (char *) NULL)
      LiberateMemory((void **) &p->filename);
    if (p->name != (char *) NULL)
      LiberateMemory((void **) &p->name);
    color_list=p;
    p=p->next;
    LiberateMemory((void **) &color_list);
  }
  color_list=(ColorInfo *) NULL;
  DestroySemaphoreInfo(color_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C u b e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyCubeInfo deallocates memory associated with an image.
%
%  The format of the DestroyCubeInfo method is:
%
%      DestroyCubeInfo(CubeInfo *cube_info)
%
%  A description of each parameter follows:
%
%    o cube_info: The address of a structure of type CubeInfo.
%
%
*/
static void DestroyCubeInfo(CubeInfo *cube_info)
{
  register Nodes
    *nodes;

  /*
    Release color cube tree storage.
  */
  DestroyColorList(cube_info->root);
  do
  {
    nodes=cube_info->node_queue->next;
    LiberateMemory((void **) &cube_info->node_queue);
    cube_info->node_queue=nodes;
  } while (cube_info->node_queue != (Nodes *) NULL);
  LiberateMemory((void **) &cube_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  D e s t r o y C o l o r L i s t                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyColorList traverses the color cube tree and frees the list of
%  unique colors.
%
%  The format of the DestroyColorList method is:
%
%      void DestroyColorList(const NodeInfo *node_info)
%
%  A description of each parameter follows.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void DestroyColorList(const NodeInfo *node_info)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      DestroyColorList(node_info->child[id]);
  if (node_info->list != (ColorPacket *) NULL)
    LiberateMemory((void **) &node_info->list);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C o l o r I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorInfo searches the color list for the specified name and if
%  found returns attributes for that color.
%
%  The format of the GetColorInfo method is:
%
%      PixelPacket *GetColorInfo(const char *name,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o color_info: Method GetColorInfo searches the color list for the
%      specified name and if found returns attributes for that color.
%
%    o name: The color name.
%
%    o compliance: Define the required color standard.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport ColorInfo *GetColorInfo(const char *name,ExceptionInfo *exception)
{
  char
    colorname[MaxTextExtent];

  register ColorInfo
    *p;

  register char
    *q;

  AcquireSemaphoreInfo(&color_semaphore);
  if (color_list == (ColorInfo *) NULL)
    (void) ReadConfigurationFile(ColorFilename,exception);
  LiberateSemaphoreInfo(&color_semaphore);
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    return(color_list);
  /*
    Search for named color.
  */
  FormatString(colorname,"%.1024s",name);
  for (q=colorname; *q != '\0'; q++)
  {
    if (*q != ' ')
      continue;
    (void) strcpy(q,q+1);
    q--;
  }
  for (p=color_list; p != (ColorInfo *) NULL; p=p->next)
    if (LocaleCompare(colorname,p->name) == 0)
      break;
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C o l o r l i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorList returns any colors that match the specified pattern
%  and color standard.
%
%  The format of the GetColorList function is:
%
%      filelist=GetColorList(const char *pattern,int number_colors)
%
%  A description of each parameter follows:
%
%    o filelist: Method GetColorList returns a list of colors that match the
%      specified pattern and color standard.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_colors:  This integer returns the number of colors in the list.
%
%
*/
MagickExport char **GetColorList(const char *pattern,int *number_colors)
{
  char
    **colorlist;

  ExceptionInfo
    exception;

  register ColorInfo
    *p;

  register int
    i;

  /*
    Allocate color list.
  */
  assert(pattern != (char *) NULL);
  assert(number_colors != (int *) NULL);
  *number_colors=0;
  GetExceptionInfo(&exception);
  p=GetColorInfo("*",&exception);
  if (p == (ColorInfo *) NULL)
    return((char **) NULL);
  i=0;
  for (p=color_list; p != (ColorInfo *) NULL; p=p->next)
    i++;
  colorlist=(char **) AcquireMemory(i*sizeof(char *));
  if (colorlist == (char **) NULL)
    return((char **) NULL);
  i=0;
  for (p=color_list; p != (ColorInfo *) NULL; p=p->next)
    if (GlobExpression(p->name,pattern))
      colorlist[i++]=AllocateString(p->name);
  *number_colors=i;
  return(colorlist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C u b e I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCubeInfo initialize the CubeInfo data structure.
%
%  The format of the GetCubeInfo method is:
%
%      cube_info=GetCubeInfo()
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%
*/
static CubeInfo *GetCubeInfo(void)
{
  CubeInfo
    *cube_info;

  /*
    Initialize tree to describe color cube.
  */
  cube_info=(CubeInfo *) AcquireMemory(sizeof(CubeInfo));
  if (cube_info == (CubeInfo *) NULL)
    return((CubeInfo *) NULL);
  (void) memset(cube_info,0,sizeof(CubeInfo));
  /*
    Initialize root node.
  */
  cube_info->root=GetNodeInfo(cube_info,0);
  if (cube_info->root == (NodeInfo *) NULL)
    return((CubeInfo *) NULL);
  return(cube_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  G e t N o d e I n f o                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNodeInfo allocates memory for a new node in the color cube tree
%  and presets all fields to zero.
%
%  The format of the GetNodeInfo method is:
%
%      node_info=GetNodeInfo(cube_info,level)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the CubeInfo structure.
%
%    o level: Specifies the level in the storage_class the node resides.
%
%
*/
static NodeInfo *GetNodeInfo(CubeInfo *cube_info,const unsigned int level)
{
  NodeInfo
    *node_info;

  if (cube_info->free_nodes == 0)
    {
      Nodes
        *nodes;

      /*
        Allocate a new nodes of nodes.
      */
      nodes=(Nodes *) AcquireMemory(sizeof(Nodes));
      if (nodes == (Nodes *) NULL)
        return((NodeInfo *) NULL);
      nodes->next=cube_info->node_queue;
      cube_info->node_queue=nodes;
      cube_info->node_info=nodes->nodes;
      cube_info->free_nodes=NodesInAList;
    }
  cube_info->free_nodes--;
  node_info=cube_info->node_info++;
  (void) memset(node_info,0,sizeof(NodeInfo));
  node_info->level=level;
  return(node_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t N u m b e r C o l o r s                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNumberColors returns the number of unique colors in an image.
%
%  The format of the GetNumberColors method is:
%
%      unsigned long GetNumberColors(const Image *image,FILE *file,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o number_colors: Method GetNumberColors returns the number of unique
%      colors in the specified image.
%
%    o image: The address of a byte (8 bits) array of run-length
%      encoded pixel data of your source image.  The sum of the
%      run-length counts in the source image must be equal to or exceed
%      the number of pixels.
%
%    o file:  An pointer to a FILE.  If it is non-null a list of unique pixel
%      field values and the number of times each occurs in the image is
%      written to the file.
%
%
*/
MagickExport unsigned long GetNumberColors(const Image *image,FILE *file,
  ExceptionInfo *exception)
{
#define ComputeImageColorsText  "  Compute image colors...  "

  CubeInfo
    *cube_info;

  long
    y;

  NodeInfo
    *node_info;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register unsigned int
    id,
    index,
    level;

  unsigned long
    number_colors;

  /*
    Initialize color description tree.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  cube_info=GetCubeInfo();
  if (cube_info == (CubeInfo *) NULL)
    {
      ThrowException(exception,ResourceLimitWarning,
        "unable to determine the number of image colors",
        "Memory allocation failed");
      return(0);
    }
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      return(False);
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Start at the root and proceed level by level.
      */
      node_info=cube_info->root;
      index=MaxTreeDepth-1;
      for (level=1; level <= MaxTreeDepth; level++)
      {
        id=(((Quantum) DownScale(p->red) >> index) & 0x01) << 2 |
           (((Quantum) DownScale(p->green) >> index) & 0x01) << 1 |
           (((Quantum) DownScale(p->blue) >> index) & 0x01);
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            node_info->child[id]=GetNodeInfo(cube_info,level);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                ThrowException(exception,ResourceLimitWarning,
                  "unable to determine the number of image colors",
                  "memory allocation failed");
                return(0);
              }
          }
        node_info=node_info->child[id];
        index--;
        if (level != MaxTreeDepth)
          continue;
        for (i=0; i < (long) node_info->number_unique; i++)
           if (ColorMatch(*p,node_info->list[i],0))
             break;
        if (i < (long) node_info->number_unique)
          {
            node_info->list[i].count++;
            continue;
          }
        if (node_info->number_unique == 0)
          node_info->list=(ColorPacket *) AcquireMemory(sizeof(ColorPacket));
        else
          ReacquireMemory((void **) &node_info->list,
            (i+1)*sizeof(ColorPacket));
        if (node_info->list == (ColorPacket *) NULL)
          {
            ThrowException(exception,ResourceLimitWarning,
              "unable to determine the number of image colors",
              "memory allocation failed");
            return(0);
          }
        node_info->list[i].red=p->red;
        node_info->list[i].green=p->green;
        node_info->list[i].blue=p->blue;
        node_info->list[i].count=1;
        node_info->number_unique++;
        cube_info->colors++;
      }
      p++;
    }
    if (QuantumTick(y,image->rows))
      MagickMonitor(ComputeImageColorsText,y,image->rows);
  }
  if (file != (FILE *) NULL)
    {
      (void) fputs("\n",file);
      Histogram(image,cube_info,cube_info->root,file);
      (void) fflush(file);
    }
  number_colors=cube_info->colors;
  DestroyCubeInfo(cube_info);
  return(number_colors);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  H i s t o g r a m                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Histogram traverses the color cube tree and produces a list of
%  unique pixel field values and the number of times each occurs in the image.
%
%  The format of the Histogram method is:
%
%      void Histogram(Image *image,CubeInfo *cube_info,
%        const NodeInfo *node_info,FILE *file)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the CubeInfo structure.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void Histogram(const Image *image,CubeInfo *cube_info,
  const NodeInfo *node_info,FILE *file)
{
#define HistogramImageText  "  Compute image histogram...  "

  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      Histogram(image,cube_info,node_info->child[id],file);
  if (node_info->level == MaxTreeDepth)
    {
      char
        name[MaxTextExtent];

      PixelPacket
        color;

      register ColorPacket
        *p;

      register long
        i;

      p=node_info->list;
      for (i=0; i < (long) node_info->number_unique; i++)
      {
        (void) fprintf(file,"%10lu: (%5d,%5d,%5d)  ",p->count,
          p->red,p->green,p->blue);
        (void) fprintf(file,"  ");
        color.red=p->red;
        color.green=p->green;
        color.blue=p->blue;
        (void) QueryColorname(image,&color,AllCompliance,name);
        (void) fprintf(file,"%.1024s",name);
        (void) fprintf(file,"\n");
        p++;
      }
      if (QuantumTick(cube_info->progress,cube_info->colors))
        MagickMonitor(HistogramImageText,cube_info->progress,
          cube_info->colors);
      cube_info->progress++;
    }
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
%  Method IsGrayImage returns True if the image is grayscale otherwise False
%  is returned.
%
%  The format of the IsGrayImage method is:
%
%      unsigned int IsGrayImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method IsGrayImage returns True if the image is grayscale
%      otherwise False is returned.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsGrayImage(const Image *image,ExceptionInfo *exception)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  switch (image->storage_class)
  {
    case DirectClass:
    case UndefinedClass:
    {
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          return(False);
        for (x=0; x < (long) image->columns; x++)
        {
          if (!IsGray(*p))
            return(False);
          p++;
        }
      }
      break;
    }
    case PseudoClass:
    {
      for (i=0; i < (long) image->colors; i++)
        if (!IsGray(image->colormap[i]))
          return(False);
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   I s M o n o c h r o m e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMonochromeImage returns True if the image is monochrome otherwise
%  False is returned.  If the image is DirectClass and grayscale (including
%  monochrome), it is demoted to PseudoClass.
%
%  The format of the IsMonochromeImage method is:
%
%      unsigned int IsMonochromeImage(const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method IsMonochromeImage returns True if the image is
%      monochrome otherwise False is returned.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsMonochromeImage(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  switch (image->storage_class)
  {
    case DirectClass:
    case UndefinedClass:
    {
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          return(False);
        for (x=0; x < (long) image->columns; x++)
        {
          if (!IsGray(*p))
            return(False);
          if ((p->red != 0) && (p->red != MaxRGB))
            return(False);
          p++;
        }
      }
      break;
    }
    case PseudoClass:
    {
      for (i=0; i < (long) image->colors; i++)
      {
        if (!IsGray(image->colormap[i]))
          return(False);
        if ((image->colormap[i].red != 0) && (image->colormap[i].red != MaxRGB))
          return(False);
      }
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s O p a q u e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsOpaqueImage returns False if the image has one or more pixels that
%  are transparent otherwise True is returned.
%
%  The format of the IsOpaqueImage method is:
%
%      unsigned int IsOpaqueImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method IsOpaqueImage returns False if the image has one or more
%      pixels that are transparent otherwise True is returned.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsOpaqueImage(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  /*
    Determine if image is grayscale.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!image->matte)
    return(True);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      return(True);
    for (x=0; x < (long) image->columns; x++)
    {
      if (p->opacity != OpaqueOpacity)
        return(False);
      p++;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s P a l e t t e I m a g e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPaletteImage returns True if the image is PseudoClass and has 256
%  unique colors or less.
%
%  The format of the IsPaletteImage method is:
%
%      unsigned int IsPaletteImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o status:  Method IsPaletteImage returns True is the image is
%      PseudoClass or has 256 color or less.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsPaletteImage(const Image *image,
  ExceptionInfo *exception)
{
  CubeInfo
    *cube_info;

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register NodeInfo
    *node_info;

  register long
    i;

  unsigned int
    id,
    index,
    level;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((image->storage_class == PseudoClass) && (image->colors <= 256))
    return(True);
  /*
    Initialize color description tree.
  */
  cube_info=GetCubeInfo();
  if (cube_info == (CubeInfo *) NULL)
    {
      ThrowException(exception,ResourceLimitWarning,
        "Unable to determine image class","Memory allocation failed");
      return(False);
    }
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      return(False);
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Start at the root and proceed level by level.
      */
      node_info=cube_info->root;
      index=MaxTreeDepth-1;
      for (level=1; level < MaxTreeDepth; level++)
      {
        id=((DownScale(p->red) >> index) & 0x01) << 2 |
           ((DownScale(p->green) >> index) & 0x01) << 1 |
           ((DownScale(p->blue) >> index) & 0x01);
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            node_info->child[id]=GetNodeInfo(cube_info,level);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                ThrowException(exception,ResourceLimitWarning,
                  "Unable to determine image class","Memory allocation failed");
                return(False);
              }
          }
        node_info=node_info->child[id];
        index--;
      }
      for (i=0; i < (long) node_info->number_unique; i++)
        if (ColorMatch(*p,node_info->list[i],0))
          break;
      if (i == (long) node_info->number_unique)
        {
          /*
            Add this unique color to the color list.
          */
          if (node_info->number_unique == 0)
            node_info->list=(ColorPacket *) AcquireMemory(sizeof(ColorPacket));
          else
            ReacquireMemory((void **) &node_info->list,
              (i+1)*sizeof(ColorPacket));
          if (node_info->list == (ColorPacket *) NULL)
            {
              ThrowException(exception,ResourceLimitWarning,
                "Unable to determine image class","Memory allocation failed");
              return(False);
            }
          node_info->list[i].red=p->red;
          node_info->list[i].green=p->green;
          node_info->list[i].blue=p->blue;
          node_info->list[i].index=cube_info->colors++;
          node_info->number_unique++;
          if (cube_info->colors > 256)
            {
              DestroyCubeInfo(cube_info);
              return(False);
            }
        }
      p++;
    }
  }
  DestroyCubeInfo(cube_info);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t C o l o r I n f o                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListColorInfo lists the color info to a file.
%
%  The format of the ListColorInfo method is:
%
%      unsigned int ListColorInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListColorInfo(FILE *file,ExceptionInfo *exception)
{
  register ColorInfo
    *p;

  register long
    i;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) fprintf(file,"ImageMagick understands these color strings:\n");
  p=GetColorInfo("*",exception);
  if (p == (ColorInfo *) NULL)
    return(False);
  if (p->filename != (char *) NULL)
    (void) fprintf(file,"\nFilename: %.1024s\n\n",p->filename);
  (void) fprintf(file,
    "Name                   Color                   Compliance\n");
  (void) fprintf(file,"-------------------------------------------------------"
    "------------------------\n");
  for ( ; p != (ColorInfo *) NULL; p=p->next)
  {
    (void) fprintf(file,"%.1024s",p->name);
    for (i=strlen(p->name); i <= 22; i++)
      (void) fprintf(file," ");
    if (p->color.opacity == OpaqueOpacity)
      (void) fprintf(file,"%5d,%5d,%5d       ",p->color.red,p->color.green,
        p->color.blue);
    else
      (void) fprintf(file,"%5d,%5d,%5d,%5d ",p->color.red,p->color.green,
        p->color.blue,p->color.opacity);
    if (p->compliance == AllCompliance)
      (void) fprintf(file,"SVG, X11");
    else
      if (p->compliance == X11Compliance)
        (void) fprintf(file,"X11");
      else
        (void) fprintf(file,"SVG");
    (void) fprintf(file,"\n");
  }
  (void) fflush(file);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u e r y C o l o r D a t a b a s e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method QueryColorDatabase looks up a RGB values for a given color name.
%
%  The format of the QueryColorDatabase method is:
%
%      unsigned int QueryColorDatabase(const char *name,PixelPacket *color)
%
%  A description of each parameter follows:
%
%    o status:  Method QueryColorDatabase returns True if the RGB values
%      of the target color is defined, otherwise False is returned.
%
%    o name: Specifies the color to lookup in the X color database.
%
%    o color: A pointer to an PixelPacket structure.  The RGB value of the
%      target color is returned as this value.
%
%
*/
MagickExport unsigned int QueryColorDatabase(const char *name,
  PixelPacket *color)
{
  ExceptionInfo
    exception;

  int
    blue,
    green,
    opacity,
    red;

  register long
    i;

  register ColorInfo
    *p;

  /*
    Initialize color return value.
  */
  assert(color != (PixelPacket *) NULL);
  (void) memset(color,0,sizeof(PixelPacket));
  color->opacity=TransparentOpacity;
  if ((name == (char *) NULL) || (*name == '\0'))
    name=BackgroundColor;
  while (isspace((int) (*name)))
    name++;
  if (*name == '#')
    {
      char
        c;

      long
        n;

      green=0;
      blue=0;
      opacity=(-1);
      name++;
      n=strlen(name);
      if ((n == 3) || (n == 6) || (n == 9) || (n == 12))
        {
          /*
            Parse RGB specification.
          */
          n/=3;
          do
          {
            red=green;
            green=blue;
            blue=0;
            for (i=n-1; i >= 0; i--)
            {
              c=(*name++);
              blue<<=4;
              if ((c >= '0') && (c <= '9'))
                blue|=c-'0';
              else
                if ((c >= 'A') && (c <= 'F'))
                  blue|=c-('A'-10);
                else
                  if ((c >= 'a') && (c <= 'f'))
                    blue|=c-('a'-10);
                  else
                    return(False);
            }
          } while (*name != '\0');
        }
      else
        if ((n != 4) && (n != 8) && (n != 16))
          return(False);
        else
          {
            /*
              Parse RGBA specification.
            */
            n/=4;
            do
            {
              red=green;
              green=blue;
              blue=opacity;
              opacity=0;
              for (i=n-1; i >= 0; i--)
              {
                c=(*name++);
                opacity<<=4;
                if ((c >= '0') && (c <= '9'))
                  opacity|=c-'0';
                else
                  if ((c >= 'A') && (c <= 'F'))
                    opacity|=c-('A'-10);
                  else
                    if ((c >= 'a') && (c <= 'f'))
                      opacity|=c-('a'-10);
                    else
                      return(False);
              }
            } while (*name != '\0');
          }
      n<<=2;
      color->red=((unsigned long) (MaxRGB*red)/((1 << n)-1));
      color->green=((unsigned long) (MaxRGB*green)/((1 << n)-1));
      color->blue=((unsigned long) (MaxRGB*blue)/((1 << n)-1));
      color->opacity=OpaqueOpacity;
      if (opacity >= 0)
        color->opacity=((unsigned long) (MaxRGB*opacity)/((1 << n)-1));
      return(True);
    }
  if (LocaleNCompare(name,"rgb(",4) == 0)
    {
      (void) sscanf(name,"%*[^(](%d%*[ ,]%d%*[ ,]%d",&red,&green,&blue);
      color->red=UpScale(red);
      color->green=UpScale(green);
      color->blue=UpScale(blue);
      color->opacity=OpaqueOpacity;
      return(True);
    }
  if (LocaleNCompare(name,"rgba(",5) == 0)
    {
      (void) sscanf(name,"%*[^(](%d%*[ ,]%d%*[ ,]%d%*[ ,]%d",&red,&green,
        &blue,&opacity);
      color->red=UpScale(red);
      color->green=UpScale(green);
      color->blue=UpScale(blue);
      color->opacity=(Quantum) opacity;
      return(True);
    }
  GetExceptionInfo(&exception);
  p=GetColorInfo(name,&exception);
  if (p == (ColorInfo *) NULL)
    return(False);
  *color=p->color;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  Q u e r y C o l o r n a m e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method QueryColorname returns the name of the color that is closest to the
%  supplied color in RGB space.
%
%  The format of the QueryColorname method is:
%
%      unsigned int QueryColorname(const Image *image,const PixelPacket *color,
%        ComplianceType compliance,char *name)
%
%  A description of each parameter follows.
%
%    o status: Method QueryColorname returns True if the color is matched
%      exactly, otherwise False.
%
%    o image: The image.
%
%    o color: This is a pointer to a PixelPacket structure that contains the
%      color we are searching for.
%
%    o name: The name of the color that is closest to the supplied color is
%      returned in this character buffer.
%
%
*/
MagickExport unsigned int QueryColorname(const Image *image,
  const PixelPacket *color,const ComplianceType compliance,char *name)
{

  ExceptionInfo
    exception;

  register ColorInfo
    *p;

  *name='\0';
  GetExceptionInfo(&exception);
  p=GetColorInfo("*",&exception);
  if (p != (ColorInfo *) NULL)
    {
      double
        distance,
        distance_squared;

      for (p=color_list; p != (ColorInfo *) NULL; p=p->next)
      {
        if ((p->compliance != AllCompliance) && (p->compliance != compliance))
          continue;
        distance=color->red-(int) p->color.red;
        distance_squared=distance*distance;
        distance=color->green-(int) p->color.green;
        distance_squared+=distance*distance;
        distance=color->blue-(int) p->color.blue;
        distance_squared+=distance*distance;
        distance=color->opacity-(int) p->color.opacity;
        distance_squared+=distance*distance;
        if (distance_squared != 0.0)
          continue;
        (void) strncpy(name,p->name,MaxTextExtent-1);
        return(True);
      }
    }
  if (color->opacity == OpaqueOpacity)
    {
      if (image->depth <= 8)
        FormatString(name,"#%02x%02x%02x",DownScale(color->red),
          DownScale(color->green),DownScale(color->blue));
      else
        FormatString(name,"#%04x%04x%04x",(unsigned int) color->red,
          (unsigned int) color->green,(unsigned int) color->blue);
      return(False);
    }
  if (image->depth <= 8)
    FormatString(name,"#%02x%02x%02x%02x",DownScale(color->red),
      DownScale(color->green),DownScale(color->blue),
      DownScale(color->opacity));
  else
    FormatString(name,"#%04x%04x%04x%04x",(unsigned int) color->red,
      (unsigned int) color->green,(unsigned int) color->blue,
      (unsigned int) color->opacity);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r a t i o n F i l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadConfigurationFile reads the color configuration file which maps
%  color strings with a particular image format.
%
%  The format of the ReadConfigurationFile method is:
%
%      unsigned int ReadConfigurationFile(const char *basename,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method ReadConfigurationFile returns True if at least one color
%      is defined otherwise False.
%
%    o basename:  The color configuration filename.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int ReadConfigurationFile(const char *basename,
  ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    keyword[MaxTextExtent],
    *path,
    *q,
    *token,
    *xml;

  size_t
    length;

  /*
    Read the color configuration file.
  */
  FormatString(filename,"%.1024s",basename);
  path=GetMagickConfigurePath(basename);
  if (path != (char *) NULL)
    {
      FormatString(filename,"%.1024s",path);
      LiberateMemory((void **) &path);
    }
  xml=(char *) FileToBlob(filename,&length,exception);
  if (xml == (char *) NULL)
    xml=AllocateString(ColorMap);
  token=AllocateString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetToken(q,&q,token);
    if (*token == '\0')
      break;
    FormatString(keyword,"%.1024s",token);
    if (LocaleCompare(keyword,"<!") == 0)
      {
        /*
          Comment.
        */
        while ((*token != '>') && (*q != '\0'))
          GetToken(q,&q,token);
        continue;
      }
    if (LocaleCompare(keyword,"<color") == 0)
      {
        ColorInfo
          *color_info;

        /*
          Allocate memory for the color list.
        */
        color_info=(ColorInfo *) AcquireMemory(sizeof(ColorInfo));
        if (color_info == (ColorInfo *) NULL)
          MagickError(ResourceLimitError,"Unable to allocate colors",
            "Memory allocation failed");
        (void) memset(color_info,0,sizeof(ColorInfo));
        if (color_list == (ColorInfo *) NULL)
          {
            color_info->filename=AllocateString(filename);
            color_list=color_info;
            continue;
          }
        color_list->next=color_info;
        color_info->previous=color_list;
        color_list=color_list->next;
        continue;
      }
    if (color_list == (ColorInfo *) NULL)
      continue;
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword)
    {
      case 'B':
      case 'b':
      {
        if (LocaleCompare((char *) keyword,"blue") == 0)
          {
            color_list->color.blue=UpScale(atol(token));
            break;
          }
        break;
      }
      case 'C':
      case 'c':
      {
        if (LocaleCompare((char *) keyword,"compliance") == 0)
          {
            if (GlobExpression(token,"*SVG*") && GlobExpression(token,"*X11*"))
              color_list->compliance=AllCompliance;
            else
              if (GlobExpression(token,"SVG"))
                color_list->compliance=SVGCompliance;
              else
                color_list->compliance=X11Compliance;
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        if (LocaleCompare((char *) keyword,"green") == 0)
          {
            color_list->color.green=UpScale(atol(token));
            break;
          }
        break;
      }
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            color_list->name=AllocateString(token);
            break;
          }
        break;
      }
      case 'O':
      case 'o':
      {
        if (LocaleCompare((char *) keyword,"opacity") == 0)
          {
            color_list->color.opacity=UpScale(atol(token));
            break;
          }
        break;
      }
      case 'R':
      case 'r':
      {
        if (LocaleCompare((char *) keyword,"red") == 0)
          {
            color_list->color.red=UpScale(atol(token));
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  LiberateMemory((void **) &token);
  LiberateMemory((void **) &xml);
  if (color_list == (ColorInfo *) NULL)
    return(False);
  while (color_list->previous != (ColorInfo *) NULL)
    color_list=color_list->previous;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   V a l i d a t e C o l o r m a p I n d e x                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ValidateColormapIndex validates the colormap index.  If the index
%  does not range from 0 to the number of colors in the colormap an exception
%  is issued and 0 is returned.
%
%  The format of the ValidateColormapIndex method is:
%
%      IndexPacket ValidateColormapIndex(Image *image,const unsigned int index)
%
%  A description of each parameter follows:
%
%    o index: Method ValidateColormapIndex returns colormap index if it is
%      valid other an exception is issued and 0 is returned.
%
%    o image: The image.
%
%    o index: This integer is the colormap index.
%
%
*/
MagickExport IndexPacket ValidateColormapIndex(Image *image,
  const unsigned long index)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (index < image->colors)
    return((IndexPacket) index);
  ThrowException(&image->exception,CorruptImageWarning,
    "invalid colormap index",image->filename);
  return(0);
}
