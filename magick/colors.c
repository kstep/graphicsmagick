/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  CCCC   OOO   L       OOO   RRRR    SSSSS                   %
%                 C      O   O  L      O   O  R   R   SS                      %
%                 C      O   O  L      O   O  RRRR     SSS                    %
%                 C      O   O  L      O   O  R R        SS                   %
%                  CCCC   OOO   LLLLL   OOO   R  R    SSSSS                   %
%                                                                             %
%                                                                             %
%                  Methods to Count the Colors in an Image                    %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1992                                      %
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
#include "Colorlist.h"

/*
  Structures.
*/
typedef struct _NodeInfo
{
  unsigned char
    level;

  unsigned long
    number_unique;

  ColorPacket
    *list;

  struct _NodeInfo
    *child[8];
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

  unsigned int
    progress,
    colors;

  unsigned int
    free_nodes;

  NodeInfo
    *node_info;

  Nodes
    *node_list;
} CubeInfo;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  D e s t r o y L i s t                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyList traverses the color cube tree and free the list of
%  unique colors.
%
%  The format of the DestroyList routine is:
%
+      DestroyList(node_info)
%
%  A description of each parameter follows.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void DestroyList(const NodeInfo *node_info)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      DestroyList(node_info->child[id]);
  if (node_info->level == MaxTreeDepth)
    if (node_info->list != (ColorPacket *) NULL)
      FreeMemory((char *) node_info->list);
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
%  The format of the Histogram routine is:
%
%      Histogram(color_cube,node_info,file)
%
%  A description of each parameter follows.
%
%    o color_cube: A pointer to the CubeInfo structure.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void Histogram(CubeInfo *color_cube,const NodeInfo *node_info,FILE *file)
{
#define HistogramImageText  "  Computing image histogram...  "

  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      Histogram(color_cube,node_info->child[id],file);
  if (node_info->level == MaxTreeDepth)
    {
      char
        name[MaxTextExtent];

      register ColorPacket
        *p;

      register int
        i;

      p=node_info->list;
      for (i=0; i < (int) node_info->number_unique; i++)
      {
        (void) fprintf(file,"%10lu: (%3d,%3d,%3d)  #%02x%02x%02x",
          p->count,p->red,p->green,p->blue,(unsigned int) p->red,
          (unsigned int) p->green,(unsigned int) p->blue);
        (void) fprintf(file,"  ");
        (void) QueryColorName(p,name);
        (void) fprintf(file,"%.1024s",name);
        (void) fprintf(file,"\n");
      }
      if (QuantumTick(color_cube->progress,color_cube->colors))
        ProgressMonitor(HistogramImageText,color_cube->progress,
          color_cube->colors);
      color_cube->progress++;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  I n i t i a l i z e N o d e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeNode allocates memory for a new node in the color cube
%  tree and presets all fields to zero.
%
%  The format of the InitializeNode routine is:
%
+      node_info=InitializeNode(color_cube,level)
%
%  A description of each parameter follows.
%
%    o color_cube: A pointer to the CubeInfo structure.
%
%    o level: Specifies the level in the classification the node resides.
%
%
*/
static NodeInfo *InitializeNode(CubeInfo *color_cube,const unsigned int level)
{
  register int
    i;

  NodeInfo
    *node_info;

  if (color_cube->free_nodes == 0)
    {
      Nodes
        *nodes;

      /*
        Allocate a new nodes of nodes.
      */
      nodes=(Nodes *) AllocateMemory(sizeof(Nodes));
      if (nodes == (Nodes *) NULL)
        return((NodeInfo *) NULL);
      nodes->next=color_cube->node_list;
      color_cube->node_list=nodes;
      color_cube->node_info=nodes->nodes;
      color_cube->free_nodes=NodesInAList;
    }
  color_cube->free_nodes--;
  node_info=color_cube->node_info++;
  for (i=0; i < 8; i++)
    node_info->child[i]=(NodeInfo *) NULL;
  node_info->level=level;
  node_info->number_unique=0;
  node_info->list=(ColorPacket *) NULL;
  return(node_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s P s e u d o C l a s s                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPseudoClass returns True if the image is PseudoClass and has 256
%  unique colors or less.  If the image is DirectClass and has less 256 colors
%  or less, the image is demoted to PseudoClass.
%
%  The format of the IsPseudoClass routine is:
%
%      status=IsPseudoClass(image)
%
%  A description of each parameter follows.
%
%    o status:  Method IsPseudoClass returns True is the image is
%      PseudoClass or has 256 color or less.
%
%    o image: The address of a byte (8 bits) array of run-length
%      encoded pixel data of your source image.  The sum of the
%      run-length counts in the source image must be equal to or exceed
%      the number of pixels.
%
%
*/
Export unsigned int IsPseudoClass(Image *image)
{
  CubeInfo
    color_cube;

  Nodes
    *nodes;

  register int
    i,
    j;

  register NodeInfo
    *node_info;

  register RunlengthPacket
    *p;

  register unsigned int
    index,
    level;

  unsigned int
    id;

  assert(image != (Image *) NULL);
  if ((image->class == PseudoClass) && (image->colors <= 256))
    return(True);
  if (image->matte)
    return(False);
  /*
    Initialize color description tree.
  */
  color_cube.node_list=(Nodes *) NULL;
  color_cube.progress=0;
  color_cube.colors=0;
  color_cube.free_nodes=0;
  color_cube.root=InitializeNode(&color_cube,0);
  if (color_cube.root == (NodeInfo *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to count colors",
        "Memory allocation failed");
      return(False);
    }
  if (image->packets == (image->columns*image->rows))
    CondenseImage(image);
  p=image->pixels;
  for (i=0; (i < (int) image->packets) && (color_cube.colors <= 256); i++)
  {
    /*
      Start at the root and proceed level by level.
    */
    node_info=color_cube.root;
    index=MaxTreeDepth-1;
    for (level=1; level <= MaxTreeDepth; level++)
    {
      id=(((Quantum) DownScale(p->red) >> index) & 0x01) << 2 |
         (((Quantum) DownScale(p->green) >> index) & 0x01) << 1 |
         (((Quantum) DownScale(p->blue) >> index) & 0x01);
      if (node_info->child[id] == (NodeInfo *) NULL)
        {
          node_info->child[id]=InitializeNode(&color_cube,level);
          if (node_info->child[id] == (NodeInfo *) NULL)
            {
              MagickWarning(ResourceLimitWarning,"Unable to count colors",
                "Memory allocation failed");
              return(False);
            }
        }
      node_info=node_info->child[id];
      index--;
      if (level != MaxTreeDepth)
        continue;
      for (j=0; j < (int) node_info->number_unique; j++)
         if ((p->red == node_info->list[j].red) &&
             (p->green == node_info->list[j].green) &&
             (p->blue == node_info->list[j].blue))
           break;
      if (j < (int) node_info->number_unique)
        {
          node_info->list[j].count+=p->length+1;
          continue;
        }
      if (node_info->number_unique == 0)
        node_info->list=(ColorPacket *) AllocateMemory(sizeof(ColorPacket));
      else
        node_info->list=(ColorPacket *)
          ReallocateMemory(node_info->list,(j+1)*sizeof(ColorPacket));
      if (node_info->list == (ColorPacket *) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Unable to count colors",
            "Memory allocation failed");
          return(False);
        }
      node_info->list[j].red=p->red;
      node_info->list[j].green=p->green;
      node_info->list[j].blue=p->blue;
      node_info->list[j].count=1;
      node_info->number_unique++;
      color_cube.colors++;
    }
    p++;
  }
  /*
    Release color cube tree storage.
  */
  DestroyList(color_cube.root);
  do
  {
    nodes=color_cube.node_list->next;
    FreeMemory((char *) color_cube.node_list);
    color_cube.node_list=nodes;
  }
  while (color_cube.node_list != (Nodes *) NULL);
  if (color_cube.colors <= 256)
    {
      QuantizeInfo
        quantize_info;

      /*
        Demote DirectClass to PseudoClass.
      */
      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=256;
      (void) QuantizeImage(&quantize_info,image);
      SyncImage(image);
    }
  return((image->class == PseudoClass) && (image->colors <= 256));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  Q u e r y C o l o r N a m e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method QueryColorName returns the name of the color that is closest to the
%  supplied color in RGB space.
%
%  The format of the QueryColorName routine is:
%
%      distance=QueryColorName(color,name)
%
%  A description of each parameter follows.
%
%    o distance: Method QueryColorName returns the distance-squared in RGB
%      space as well as the color name that is at a minimum distance.
%
%    o color: This is a pointer to a ColorPacket structure that contains the
%      color we are searching for.
%
%    o name: The name of the color that is closest to the supplied color is
%      returned in this character buffer.
%
%
*/
Export unsigned int QueryColorName(const ColorPacket *color,char *name)
{
  double
    min_distance;

  long
    mean;

  register const XColorlist
    *p;

  register double
    distance_squared;

  register int
    distance;

  *name='\0';
  min_distance=0;
  for (p=Colorlist; p->name != (char *) NULL; p++)
  {
    mean=(DownScale(color->red)+(int) p->red)/2;
    distance=DownScale(color->red)-(int) p->red;
    distance_squared=(2.0*256.0+mean)*distance*distance/256.0;
    distance=DownScale(color->green)-(int) p->green;
    distance_squared+=4.0*distance*distance;
    distance=DownScale(color->blue)-(int) p->blue;
    distance_squared+=(3.0*256.0-1.0-mean)*distance*distance/256.0;
    if ((p == Colorlist) || (distance_squared < min_distance))
      {
        min_distance=distance_squared;
        (void) strcpy(name,p->name);
      }
  }
  if (min_distance != 0.0)
    FormatString(name,HexColorFormat,(unsigned int) color->red,
      (unsigned int) color->green,(unsigned int) color->blue);
  return((unsigned int) min_distance);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  N u m b e r C o l o r s                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NumberColors returns the number of unique colors in an image.
%
%  The format of the NumberColors routine is:
%
%      NumberColors(image,file)
%
%  A description of each parameter follows.
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
%
*/
Export void NumberColors(Image *image,FILE *file)
{
#define NumberColorsImageText  "  Computing image colors...  "

  CubeInfo
    color_cube;

  NodeInfo
    *node_info;

  Nodes
    *nodes;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned int
    id,
    index,
    level;

  /*
    Initialize color description tree.
  */
  assert(image != (Image *) NULL);
  image->total_colors=0;
  color_cube.node_list=(Nodes *) NULL;
  color_cube.colors=0;
  color_cube.free_nodes=0;
  color_cube.root=InitializeNode(&color_cube,0);
  if (color_cube.root == (NodeInfo *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to count colors",
        "Memory allocation failed");
      return;
    }
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    /*
      Start at the root and proceed level by level.
    */
    node_info=color_cube.root;
    index=MaxTreeDepth-1;
    for (level=1; level <= MaxTreeDepth; level++)
    {
      id=(((Quantum) DownScale(p->red) >> index) & 0x01) << 2 |
         (((Quantum) DownScale(p->green) >> index) & 0x01) << 1 |
         (((Quantum) DownScale(p->blue) >> index) & 0x01);
      if (node_info->child[id] == (NodeInfo *) NULL)
        {
          node_info->child[id]=InitializeNode(&color_cube,level);
          if (node_info->child[id] == (NodeInfo *) NULL)
            {
              MagickWarning(ResourceLimitWarning,"Unable to count colors",
                "Memory allocation failed");
              return;
            }
        }
      node_info=node_info->child[id];
      index--;
      if (level != MaxTreeDepth)
        continue;
      for (j=0; j < (int) node_info->number_unique; j++)
         if ((p->red == node_info->list[j].red) &&
             (p->green == node_info->list[j].green) &&
             (p->blue == node_info->list[j].blue))
           break;
      if (j < (int) node_info->number_unique)
        {
          node_info->list[j].count+=p->length+1;
          continue;
        }
      if (node_info->number_unique == 0)
        node_info->list=(ColorPacket *) AllocateMemory(sizeof(ColorPacket));
      else
        node_info->list=(ColorPacket *)
          ReallocateMemory(node_info->list,(j+1)*sizeof(ColorPacket));
      if (node_info->list == (ColorPacket *) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Unable to count colors",
            "Memory allocation failed");
          return;
        }
      node_info->list[j].red=p->red;
      node_info->list[j].green=p->green;
      node_info->list[j].blue=p->blue;
      node_info->list[j].count=p->length+1;
      node_info->number_unique++;
      color_cube.colors++;
    }
    p++;
    if (QuantumTick(i,image->packets))
      ProgressMonitor(NumberColorsImageText,i,image->packets);
  }
  if (file != (FILE *) NULL)
    {
      Histogram(&color_cube,color_cube.root,file);
      (void) fflush(file);
    }
  /*
    Release color cube tree storage.
  */
  DestroyList(color_cube.root);
  do
  {
    nodes=color_cube.node_list->next;
    FreeMemory((char *) color_cube.node_list);
    color_cube.node_list=nodes;
  }
  while (color_cube.node_list != (Nodes *) NULL);
  image->total_colors=color_cube.colors;
}
