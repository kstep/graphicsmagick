/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            SSSSS  V   V   GGGG                              %
%                            SS     V   V  G                                  %
%                             SSS   V   V  G GG                               %
%                               SS   V V   G   G                              %
%                            SSSSS    V     GGG                               %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                              Bill Radcliffe                                 %
%                                March 2000                                   %
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
#if defined(HasXML)
#ifdef WIN32
#include <win32config.h>
#endif
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlerror.h>
#endif

#if defined(HasAUTOTRACE)
#include "types.h"
#include "image-header.h"
#include "fit.h"
#include "output.h"
#include "pxl-outline.h"
#include "atquantize.h"
#include "thin-image.h"

char
  *version_string = "AutoTrace version 0.24a";
#endif

/*
  Typedef declaractions.
*/
#if defined(HasXML)
typedef struct _BoundingBox
{
  double
    width,
    height,
    x,
    y;
} BoundingBox;

typedef struct _ElementInfo
{
  double
    cx,
    cy,
    major,
    minor,
    angle;
} ElementInfo;

typedef struct _SVGInfo
{
  FILE
    *file;

  unsigned int
    debug;

  ExceptionInfo
    *exception;

  AffineMatrix
    affine;

  int
    width,
    height;

  char
    *size,
    *page,
    *title,
    *comment;

  int
    n;

  double
    *scale,
    pointsize;

  ElementInfo
    element;

  SegmentInfo
    segment;

  BoundingBox
    bounds,
    view_box;

  PointInfo
    radius;

  char
    *text,
    *vertices,
    *url,
    *entities;

  xmlParserCtxtPtr
    parser;

  xmlDocPtr
    document;
} SVGInfo;
#endif

/*
  Forward declarations.
*/
static unsigned int
  WriteSVGImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s S V G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSVG returns True if the image format type, identified by the
%  magick string, is SVG.
%
%  The format of the IsSVG method is:
%
%      unsigned int IsSVG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsSVG returns True if the image format type is SVG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsSVG(const unsigned char *magick,const size_t length)
{
  if (length < 5)
    return(False);
  if (LocaleNCompare((char *) magick,"<?xml",5) == 0)
    return(True);
  return(False);
}

#if defined(HasXML)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S V G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSVGImage reads a Scalable Vector Gaphics file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadSVGImage method is:
%
%      Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSVGImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static double GetUserSpaceCoordinateValue(const SVGInfo *svg_info,
  const char *string)
{
  char
    *p,
    token[MaxTextExtent];

  double
    value;

  register int
    n;

  assert(string != (const char *) NULL);
  n=svg_info->n;
  p=(char *) string;
  GetToken(p,&p,token);
  value=atof(token);
  if (strchr(token,'%') != (char *) NULL)
    {
      double
        scale;

      scale=ExpandAffine(&svg_info->affine);
      return(value*svg_info->width/svg_info->scale[n]/scale/100.0);
    }
  GetToken(p,&p,token);
  if (LocaleNCompare(token,"cm",2) == 0)
    return(72.0/2.54*value/svg_info->scale[n]);
  if (LocaleNCompare(token,"em",2) == 0)
    return(svg_info->pointsize*value/svg_info->scale[n]);
  if (LocaleNCompare(token,"ex",2) == 0)
    return(svg_info->pointsize*value/svg_info->scale[n]/2.0);
  if (LocaleNCompare(token,"in",2) == 0)
    return(72.0*value/svg_info->scale[n]);
  if (LocaleNCompare(token,"mm",2) == 0)
    return(72.0/25.4*value/svg_info->scale[n]);
  if (LocaleNCompare(token,"pc",2) == 0)
    return(72.0/6.0*value/svg_info->scale[n]);
  if (LocaleNCompare(token,"pt",2) == 0)
    return(value/svg_info->scale[n]);
  if (LocaleNCompare(token,"px",2) == 0)
    return(value/svg_info->scale[n]);
  return(value);
}

static char **GetStyleTokens(const char *text,int *number_tokens)
{
  char
    **tokens;

  register const char
    *p,
    *q;

  register int
    i;

  *number_tokens=0;
  if (text == (const char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=text; *p != '\0'; p++)
    if (*p == ':')
      (*number_tokens)+=2;
  tokens=(char **) AcquireMemory((*number_tokens+2)*sizeof(char *));
  if (tokens == (char **) NULL)
    MagickError(ResourceLimitError,"Unable to convert string to tokens",
      "Memory allocation failed");
  /*
    Convert string to an ASCII list.
  */
  i=0;
  p=text;
  for (q=p; *q != '\0'; q++)
  {
    if ((*q != ':') && (*q != ';') && (*q != '\0'))
      continue;
    tokens[i]=AllocateString(p);
    (void) strncpy(tokens[i],p,q-p);
    tokens[i][q-p]='\0';
    Strip(tokens[i++]);
    p=q+1;
  }
  tokens[i]=AllocateString(p);
  (void) strncpy(tokens[i],p,q-p);
  tokens[i][q-p]='\0';
  Strip(tokens[i++]);
  tokens[i]=(char *) NULL;
  return(tokens);
}

static char **GetTransformTokens(const char *text,int *number_tokens)
{
  char
    **tokens;

  register const char
    *p,
    *q;

  register int
    i;

  *number_tokens=0;
  if (text == (const char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=text; *p != '\0'; p++)
  {
    if (*p == '(')
      (*number_tokens)+=2;
  }
  tokens=(char **) AcquireMemory((*number_tokens+2)*sizeof(char *));
  if (tokens == (char **) NULL)
    MagickError(ResourceLimitError,"Unable to convert string to tokens",
      "Memory allocation failed");
  /*
    Convert string to an ASCII list.
  */
  i=0;
  p=text;
  for (q=p; *q != '\0'; q++)
  {
    if ((*q != '(') && (*q != ')') && (*q != '\0'))
      continue;
    tokens[i]=AllocateString(p);
    (void) strncpy(tokens[i],p,q-p);
    tokens[i][q-p]='\0';
    Strip(tokens[i++]);
    p=q+1;
  }
  tokens[i]=AllocateString(p);
  (void) strncpy(tokens[i],p,q-p);
  tokens[i][q-p]='\0';
  Strip(tokens[i++]);
  tokens[i]=(char *) NULL;
  return(tokens);
}

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int SVGIsStandalone(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Is this document tagged standalone?
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.SVGIsStandalone()\n");
  return(svg_info->document->standalone == 1);
}

static int SVGHasInternalSubset(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Does this document has an internal subset?
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.SVGHasInternalSubset()\n");
  return(svg_info->document->intSubset != NULL);
}

static int SVGHasExternalSubset(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Does this document has an external subset?
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.SVGHasExternalSubset()\n");
  return(svg_info->document->extSubset != NULL);
}

static void SVGInternalSubset(void *context,const xmlChar *name,
  const xmlChar *external_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  /*
    Does this document has an internal subset?
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    {
      (void) fprintf(stdout,"  SAX.internalSubset(%.1024s",name);
      if (external_id != NULL)
        (void) fprintf(stdout,"  , %.1024s",external_id);
      if (system_id != NULL)
        (void) fprintf(stdout,"  , %.1024s",system_id);
      (void) fprintf(stdout,"  \n");
    }
  (void) xmlCreateIntSubset(svg_info->document,name,external_id,system_id);
}

static xmlParserInputPtr SVGResolveEntity(void *context,
  const xmlChar *public_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  xmlParserInputPtr
    stream;

  /*
    Special entity resolver, better left to the parser, it has more
    context than the application layer.  The default behaviour is to
    not resolve the entities, in that case the ENTITY_REF nodes are
    built in the structure (and the parameter values).
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    {
      (void) fprintf(stdout,"  SAX.resolveEntity(");
      if (public_id != NULL)
        (void) fprintf(stdout,"  %.1024s",(char *) public_id);
      else
        (void) fprintf(stdout,"   ");
      if (system_id != NULL)
        (void) fprintf(stdout,"  , %.1024s)\n",(char *) system_id);
      else
        (void) fprintf(stdout,"  , )\n");
    }
  stream=xmlLoadExternalEntity((const char *) system_id,(const char *)
    public_id,svg_info->parser);
  return(stream);
}

static xmlEntityPtr SVGGetEntity(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  /*
    Get an entity by name.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.SVGGetEntity(%.1024s)\n",name);
  return(xmlGetDocEntity(svg_info->document,name));
}

static xmlEntityPtr SVGGetParameterEntity(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  /*
    Get a parameter entity by name.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.getParameterEntity(%.1024s)\n",name);
  return(xmlGetParameterEntity(svg_info->document,name));
}

static void SVGEntityDeclaration(void *context,const xmlChar *name,int type,
  const xmlChar *public_id,const xmlChar *system_id,xmlChar *content)
{
  SVGInfo
    *svg_info;

  /*
    An entity definition has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,
      "  SAX.entityDecl(%.1024s, %d, %.1024s, %.1024s, %.1024s)\n",name,type,
      public_id ? (char *) public_id : "none",
      system_id ? (char *) system_id : "none",content);
  if (svg_info->parser->inSubset == 1)
    (void) xmlAddDocEntity(svg_info->document,name,type,public_id,system_id,
      content);
  else
    if (svg_info->parser->inSubset == 2)
      (void) xmlAddDtdEntity(svg_info->document,name,type,public_id,system_id,
        content);
}

static void SVGAttributeDeclaration(void *context,const xmlChar *element,
  const xmlChar *name,int type,int value,const xmlChar *default_value,
  xmlEnumerationPtr tree)
{
  SVGInfo
    *svg_info;

  xmlChar
    *fullname,
    *prefix;

  xmlParserCtxtPtr
    parser;

  /*
    An attribute definition has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,
      "  SAX.attributeDecl(%.1024s, %.1024s, %d, %d, %.1024s, ...)\n",element,
      name,type,value,default_value);
  fullname=(xmlChar *) NULL;
  prefix=(xmlChar *) NULL;
  parser=svg_info->parser;
  fullname=(xmlChar *) xmlSplitQName(parser,name,&prefix);
  if (parser->inSubset == 1)
    (void) xmlAddAttributeDecl(&parser->vctxt,svg_info->document->intSubset,
      element,fullname,prefix,(xmlAttributeType) type,
      (xmlAttributeDefault) value,default_value,tree);
  else
    if (parser->inSubset == 2)
      (void) xmlAddAttributeDecl(&parser->vctxt,svg_info->document->extSubset,
        element,fullname,prefix,(xmlAttributeType) type,
        (xmlAttributeDefault) value,default_value,tree);
  if (prefix != (xmlChar *) NULL)
    xmlFree(prefix);
  if (fullname != (xmlChar *) NULL)
    xmlFree(fullname);
}

static void SVGElementDeclaration(void *context,const xmlChar *name,int type,
  xmlElementContentPtr content)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    An element definition has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.elementDecl(%.1024s, %d, ...)\n",name,type);
  parser=svg_info->parser;
  if (parser->inSubset == 1)
    (void) xmlAddElementDecl(&parser->vctxt,svg_info->document->intSubset,
      name,(xmlElementTypeVal) type,content);
  else
    if (parser->inSubset == 2)
      (void) xmlAddElementDecl(&parser->vctxt,svg_info->document->extSubset,
        name,(xmlElementTypeVal) type,content);
}

static void SVGNotationDeclaration(void *context,const xmlChar *name,
  const xmlChar *public_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    What to do when a notation declaration has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,
      "  SAX.notationDecl(%.1024s, %.1024s, %.1024s)\n",(char *) name,
      public_id ? (char *) public_id : "none",
      system_id ? (char *) system_id : "none");
  parser=svg_info->parser;
  if (parser->inSubset == 1)
    (void) xmlAddNotationDecl(&parser->vctxt,svg_info->document->intSubset,
      name,public_id,system_id);
  else
    if (parser->inSubset == 2)
      (void) xmlAddNotationDecl(&parser->vctxt,svg_info->document->intSubset,
        name,public_id,system_id);
}

static void SVGUnparsedEntityDeclaration(void *context,const xmlChar *name,
  const xmlChar *public_id,const xmlChar *system_id,const xmlChar *notation)
{
  SVGInfo
    *svg_info;

  /*
    What to do when an unparsed entity declaration is parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,
      "  SAX.unparsedEntityDecl(%.1024s, %.1024s, %.1024s, %.1024s)\n",
      (char *) name,public_id ? (char *) public_id : "none",
      system_id ? (char *) system_id : "none",(char *) notation);
  xmlAddDocEntity(svg_info->document,name,XML_EXTERNAL_GENERAL_UNPARSED_ENTITY,
    public_id,system_id,notation);

}

static void SVGSetDocumentLocator(void *context,xmlSAXLocatorPtr location)
{
  SVGInfo
    *svg_info;

  /*
    Receive the document locator at startup, actually xmlDefaultSAXLocator.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.setDocumentLocator()\n");
}

static void SVGStartDocument(void *context)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    Called when the document start being processed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.startDocument()\n");
  GetExceptionInfo(svg_info->exception);
  parser=svg_info->parser;
  svg_info->document=xmlNewDoc(parser->version);
  if (svg_info->document == (xmlDocPtr) NULL)
    return;
  if (parser->encoding == NULL)
    svg_info->document->encoding=NULL;
  else
    svg_info->document->encoding=xmlStrdup(parser->encoding);
  svg_info->document->standalone=parser->standalone;
}

static void SVGEndDocument(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Called when the document end has been detected.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.endDocument()\n");
  if (svg_info->scale != (double *) NULL)
    LiberateMemory((void **) &svg_info->scale);
  if (svg_info->text != (char *) NULL)
    LiberateMemory((void **) &svg_info->text);
  if (svg_info->vertices != (char *) NULL)
    LiberateMemory((void **) &svg_info->vertices);
  if (svg_info->url != (char *) NULL)
    LiberateMemory((void **) &svg_info->url);
}

static void SVGStartElement(void *context,const xmlChar *name,
  const xmlChar **attributes)
{
  char
    *color,
    *font_family,
    *font_style,
    *font_weight,
    *p,
    token[MaxTextExtent],
    **tokens,
    *units;

  const char
    *keyword,
    *value;

  int
    number_tokens;

  SVGInfo
    *svg_info;

  register int
    i,
    j;

  /*
    Called when an opening tag has been processed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.startElement(%.1024s",(char *) name);
  value=(const char *) NULL;
  if (attributes != (const xmlChar **) NULL)
    for (i=0; (attributes[i] != (const xmlChar *) NULL); i+=2)
    {
      keyword=(const char *) attributes[i];
      value=(const char *) attributes[i+1];
      if (LocaleCompare(keyword,"id") == 0)
        break;
    }
  switch (*name)
  {
    case 'C':
    case 'c':
    {
      if (LocaleCompare((char *) name,"circle") == 0)
        {
          (void) fprintf(svg_info->file,"push graphic-context\n");
          break;
        }
      if (LocaleCompare((char *) name,"clipPath") == 0)
        {
          (void) fprintf(svg_info->file,"push clip-path %s\n",value);
          break;
        }
      break;
    }
    case 'D':
    case 'd':
    {
      if (LocaleCompare((char *) name,"defs") == 0)
        {
          (void) fprintf(svg_info->file,"push defs\n");
          break;
        }
      break;
    }
    case 'E':
    case 'e':
    {
      if (LocaleCompare((char *) name,"ellipse") == 0)
        {
          (void) fprintf(svg_info->file,"push graphic-context\n");
          break;
        }
      break;
    }
    case 'G':
    case 'g':
    {
      if (LocaleCompare((char *) name,"g") == 0)
        {
          (void) fprintf(svg_info->file,"push graphic-context\n");
          break;
        }
      break;
    }
    case 'I':
    case 'i':
    {
      if (LocaleCompare((char *) name,"image") == 0)
        {
          (void) fprintf(svg_info->file,"push graphic-context\n");
          break;
        }
      break;
    }
    case 'L':
    case 'l':
    {
      if (LocaleCompare((char *) name,"line") == 0)
        {
          (void) fprintf(svg_info->file,"push graphic-context\n");
          break;
        }
      if (LocaleCompare((char *) name,"linearGradient") == 0)
        {
          (void) fprintf(svg_info->file,"push linear-gradient %s\n",value);
          break;
        }
      break;
    }
    case 'P':
    case 'p':
    {
      if (LocaleCompare((char *) name,"path") == 0)
        {
          (void) fprintf(svg_info->file,"push graphic-context\n");
          break;
        }
      if (LocaleCompare((char *) name,"polygon") == 0)
        {
          (void) fprintf(svg_info->file,"push graphic-context\n");
          break;
        }
      if (LocaleCompare((char *) name,"polyline") == 0)
        {
          (void) fprintf(svg_info->file,"push graphic-context\n");
          break;
        }
      break;
    }
    case 'R':
    case 'r':
    {
      if (LocaleCompare((char *) name,"rect") == 0)
        {
          (void) fprintf(svg_info->file,"push graphic-context\n");
          break;
        }
      break;
    }
    case 'S':
    case 's':
    {
      if (LocaleCompare((char *) name,"svg") == 0)
        {
          (void) fprintf(svg_info->file,"push graphic-context\n");
          break;
        }
      break;
    }
    case 'T':
    case 't':
    {
      if (LocaleCompare((char *) name,"text") == 0)
        {
          (void) fprintf(svg_info->file,"push graphic-context\n");
          break;
        }
      break;
    }
    default:
      break;
  }
  svg_info->n++;
  ReacquireMemory((void **) &svg_info->scale,(svg_info->n+1)*sizeof(double));
  if (svg_info->scale == (double *) NULL)
    MagickError(ResourceLimitError,"Unable to convert SVG image",
      "Memory allocation failed");
  svg_info->scale[svg_info->n]=svg_info->scale[svg_info->n-1];
  color=AllocateString("none");
  font_family=(char *) NULL;
  font_style=(char *) NULL;
  font_weight=(char *) NULL;
  units=AllocateString("userSpaceOnUse");
  if (attributes != (const xmlChar **) NULL)
    for (i=0; (attributes[i] != (const xmlChar *) NULL); i+=2)
    {
      keyword=(const char *) attributes[i];
      value=(const char *) attributes[i+1];
      if (svg_info->debug)
        {
          (void) fprintf(stdout,"  , %.1024s='",keyword);
          (void) fprintf(stdout,"  %.1024s'",value);
        }
      switch (*keyword)
      {
        case 'A':
        case 'a':
        {
          if (LocaleCompare(keyword,"angle") == 0)
            {
              (void) fprintf(svg_info->file,"angle %g\n",
                GetUserSpaceCoordinateValue(svg_info,value));
              break;
            }
          break;
        }
        case 'C':
        case 'c':
        {
          if (LocaleCompare(keyword,"clip-path") == 0)
            {
              (void) fprintf(svg_info->file,"clip-path %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"clip-rule") == 0)
            {
              (void) fprintf(svg_info->file,"clip-rule %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"clipPathUnits") == 0)
            {
              CloneString(&units,value);
              (void) fprintf(svg_info->file,"clip-units %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"color") == 0)
            {
              CloneString(&color,value);
              break;
            }
          if (LocaleCompare(keyword,"cx") == 0)
            {
              svg_info->element.cx=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"cy") == 0)
            {
              svg_info->element.cy=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        case 'D':
        case 'd':
        {
          if (LocaleCompare(keyword,"d") == 0)
            {
              CloneString(&svg_info->vertices,value);
              break;
            }
          break;
        }
        case 'F':
        case 'f':
        {
          if (LocaleCompare(keyword,"fill") == 0)
            {
              if (LocaleCompare(value,"currentColor") == 0)
                {
                  (void) fprintf(svg_info->file,"fill %s\n",color);
                  break;
                }
              (void) fprintf(svg_info->file,"fill %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"fillcolor") == 0)
            {
              (void) fprintf(svg_info->file,"fill %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"fill-rule") == 0)
            {
              (void) fprintf(svg_info->file,"fill-rule %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"fill-opacity") == 0)
            {
              (void) fprintf(svg_info->file,"fill-opacity %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"font-family") == 0)
            {
              font_family=AllocateString(value);
              break;
            }
          if (LocaleCompare(keyword,"font-style") == 0)
            {
              if (LocaleCompare(value,"Normal") == 0)
                break;
              font_style=AllocateString(value);
              *font_style=toupper((int) *font_style);
              break;
            }
          if (LocaleCompare(keyword,"font-size") == 0)
            {
              svg_info->pointsize=GetUserSpaceCoordinateValue(svg_info,value);
              (void) fprintf(svg_info->file,"font-size %g\n",svg_info->pointsize);
              break;
            }
          if (LocaleCompare(keyword,"font-weight") == 0)
            {
              if (LocaleCompare(value,"Normal") == 0)
                break;
              font_weight=AllocateString(value);
              *font_weight=toupper((int) *font_weight);
              break;
            }
          break;
        }
        case 'G':
        case 'g':
        {
          if (LocaleCompare(keyword,"gradientUnits") == 0)
            {
              CloneString(&units,value);
              (void) fprintf(svg_info->file,"gradient-units %s\n",value);
              break;
            }
          break;
        }
        case 'H':
        case 'h':
        {
          if (LocaleCompare(keyword,"height") == 0)
            {
              svg_info->bounds.height=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"href") == 0)
            {
              CloneString(&svg_info->url,value);
              break;
            }
          break;
        }
        case 'M':
        case 'm':
        {
          if (LocaleCompare(keyword,"major") == 0)
            {
              svg_info->element.major=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"minor") == 0)
            {
              svg_info->element.minor=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        case 'O':
        case 'o':
        {
          if (LocaleCompare(keyword,"offset") == 0)
            {
              (void) fprintf(svg_info->file,"offset %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"opacity") == 0)
            {
              (void) fprintf(svg_info->file,"opacity %s\n",value);
              break;
            }
          break;
        }
        case 'P':
        case 'p':
        {
          if (LocaleCompare(keyword,"path") == 0)
            {
              CloneString(&svg_info->url,value);
              break;
            }
          if (LocaleCompare(keyword,"points") == 0)
            {
              CloneString(&svg_info->vertices,value);
              break;
            }
          break;
        }
        case 'R':
        case 'r':
        {
          if (LocaleCompare(keyword,"r") == 0)
            {
              svg_info->element.major=GetUserSpaceCoordinateValue(svg_info,value);
              svg_info->element.minor=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"rx") == 0)
            {
              if (LocaleCompare((char *) name,"ellipse") == 0)
                svg_info->element.major=
                  GetUserSpaceCoordinateValue(svg_info,value);
              else
                svg_info->radius.x=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"ry") == 0)
            {
              if (LocaleCompare((char *) name,"ellipse") == 0)
                svg_info->element.minor=
                  GetUserSpaceCoordinateValue(svg_info,value);
              else
                svg_info->radius.y=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        case 'S':
        case 's':
        {
          if (LocaleCompare(keyword,"stop-color") == 0)
            {
              (void) fprintf(svg_info->file,"stop-color %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke") == 0)
            {
              if (LocaleCompare(value,"currentColor") == 0)
                {
                  (void) fprintf(svg_info->file,"stroke %s\n",color);
                  break;
                }
              (void) fprintf(svg_info->file,"stroke %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-antialiasing") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-antialias %d\n",
                LocaleCompare(value,"true") == 0);
              break;
            }
          if (LocaleCompare(keyword,"stroke-dasharray") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-dasharray %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-dashoffset") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-dashoffset %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-linecap") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-linecap %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-linejoin") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-linejoin %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-miterlimit") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-miterlimit %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-opacity") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-opacity %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-width") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-width %g\n",
                GetUserSpaceCoordinateValue(svg_info,value));
              break;
            }
          if (LocaleCompare(keyword,"style") == 0)
            {
              if (svg_info->debug)
                (void) fprintf(stdout,"  \n");
              tokens=GetStyleTokens(value,&number_tokens);
              for (j=0; j < (number_tokens-1); j+=2)
              {
                keyword=(char *) tokens[j];
                value=(char *) tokens[j+1];
                if (svg_info->debug)
                  (void) fprintf(stdout,"    %.1024s: %.1024s\n",keyword,value);
                switch (*keyword)
                {
                  case 'C':
                  case 'c':
                  {
                     if (LocaleCompare(keyword,"clip-path") == 0)
                       {
                         (void) fprintf(svg_info->file,"clip-path %s\n",value);
                         break;
                       }
                    if (LocaleCompare(keyword,"clip-rule") == 0)
                      {
                        (void) fprintf(svg_info->file,"clip-rule %s\n",value);
                        break;
                      }
                     if (LocaleCompare(keyword,"clipPathUnits") == 0)
                       {
                         CloneString(&units,value);
                        (void) fprintf(svg_info->file,"clip-units %s\n",value);
                         break;
                       }
                    if (LocaleCompare(keyword,"color") == 0)
                      {
                        CloneString(&color,value);
                        break;
                      }
                    break;
                  }
                  case 'F':
                  case 'f':
                  {
                    if (LocaleCompare(keyword,"fill") == 0)
                      {
                         if (LocaleCompare(value,"currentColor") == 0)
                           {
                             (void) fprintf(svg_info->file,"fill %s\n",color);
                             break;
                           }
                        (void) fprintf(svg_info->file,"fill %s\n",value);
                        break;
                      }
                    if (LocaleCompare(keyword,"fillcolor") == 0)
                      {
                        (void) fprintf(svg_info->file,"fill %s\n",value);
                        break;
                      }
                    if (LocaleCompare(keyword,"fill-rule") == 0)
                      {
                        (void) fprintf(svg_info->file,"fill-rule %s\n",value);
                        break;
                      }
                    if (LocaleCompare(keyword,"fill-opacity") == 0)
                      {
                        (void) fprintf(svg_info->file,"fill-opacity %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"font-family") == 0)
                      {
                        font_family=AllocateString(value);
                        break;
                      }
                    if (LocaleCompare(keyword,"font-style") == 0)
                      {
                        if (LocaleCompare(value,"Normal") == 0)
                          break;
                        font_style=AllocateString(value);
                        *font_style=toupper((int) *font_style);
                        break;
                      }
                    if (LocaleCompare(keyword,"font-size") == 0)
                      {
                        svg_info->pointsize=
                          GetUserSpaceCoordinateValue(svg_info,value);
                        (void) fprintf(svg_info->file,"font-size %g\n",
                          svg_info->pointsize);
                        break;
                      }
                    if (LocaleCompare(keyword,"font-weight") == 0)
                      {
                        if (LocaleCompare(value,"Normal") == 0)
                          break;
                        font_weight=AllocateString(value);
                        *font_weight=toupper((int) *font_weight);
                        break;
                      }
                    break;
                  }
                  case 'O':
                  case 'o':
                  {
                    if (LocaleCompare(keyword,"offset") == 0)
                      {
                        (void) fprintf(svg_info->file,"offset %g\n",
                          GetUserSpaceCoordinateValue(svg_info,value));
                        break;
                      }
                    if (LocaleCompare(keyword,"opacity") == 0)
                      {
                        (void) fprintf(svg_info->file,"opacity %s\n",value);
                        break;
                      }
                    break;
                  }
                  case 'S':
                  case 's':
                  {
                    if (LocaleCompare(keyword,"stop-color") == 0)
                      {
                        (void) fprintf(svg_info->file,"stop-color %s\n",value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke") == 0)
                      {
                         if (LocaleCompare(value,"currentColor") == 0)
                           {
                             (void) fprintf(svg_info->file,"stroke %s\n",color);
                             break;
                           }
                        (void) fprintf(svg_info->file,"stroke %s\n",value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-antialiasing") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-antialias %d\n",
                          LocaleCompare(value,"true") == 0);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-dasharray") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-dasharray %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-dashoffset") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-dashoffset %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-linecap") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-linecap %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-linejoin") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-linejoin %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-miterlimit") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-miterlimit %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-opacity") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-opacity %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-width") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-width %g\n",
                          GetUserSpaceCoordinateValue(svg_info,value));
                        break;
                      }
                    break;
                  }
                  case 't':
                  case 'T':
                  {
                    if (LocaleCompare(keyword,"text-align") == 0)
                      {
                        if (LocaleCompare(value,"center") == 0)
                          (void) fprintf(svg_info->file,"gravity North\n");
                        if (LocaleCompare(value,"left") == 0)
                          (void) fprintf(svg_info->file,"gravity NorthWest\n");
                        if (LocaleCompare(value,"right") == 0)
                          (void) fprintf(svg_info->file,"gravity NorthEast\n");
                        break;
                      }
                    if (LocaleCompare(keyword,"text-decoration") == 0)
                      {
                        if (LocaleCompare(value,"underline") == 0)
                          (void) fprintf(svg_info->file,"decorate underline\n");
                        if (LocaleCompare(value,"line-through") == 0)
                          (void) fprintf(svg_info->file,
                            "decorate line-through\n");
                        if (LocaleCompare(value,"overline") == 0)
                          (void) fprintf(svg_info->file,"decorate overline\n");
                        break;
                      }
                    if (LocaleCompare(keyword,"text-antialiasing") == 0)
                      {
                        (void) fprintf(svg_info->file,"text-antialias %d\n",
                          LocaleCompare(value,"true") == 0);
                        break;
                      }
                    break;
                  }
                  default:
                    break;
                }
              }
              for (j=0; j < number_tokens; j++)
                LiberateMemory((void **) &tokens[j]);
              LiberateMemory((void **) &tokens);
              break;
            }
          break;
        }
        case 'T':
        case 't':
        {
          if (LocaleCompare(keyword,"text-align") == 0)
            {
              if (LocaleCompare(value,"center") == 0)
                (void) fprintf(svg_info->file,"gravity North\n");
              if (LocaleCompare(value,"left") == 0)
                (void) fprintf(svg_info->file,"gravity NorthWest\n");
              if (LocaleCompare(value,"right") == 0)
                (void) fprintf(svg_info->file,"gravity NorthEast\n");
              break;
            }
          if (LocaleCompare(keyword,"text-decoration") == 0)
            {
              if (LocaleCompare(value,"underline") == 0)
                (void) fprintf(svg_info->file,"decorate underline\n");
              if (LocaleCompare(value,"line-through") == 0)
                (void) fprintf(svg_info->file,"decorate line-through\n");
              if (LocaleCompare(value,"overline") == 0)
                (void) fprintf(svg_info->file,"decorate overline\n");
              break;
            }
          if (LocaleCompare(keyword,"text-antialiasing") == 0)
            {
              (void) fprintf(svg_info->file,"text-antialias %d\n",
                LocaleCompare(value,"true") == 0);
              break;
            }
          if (LocaleCompare(keyword,"transform") == 0)
            {
              AffineMatrix
                affine,
                current,
                transform;

              IdentityAffine(&transform);
              if (svg_info->debug)
                (void) fprintf(stdout,"  \n");
              tokens=GetTransformTokens(value,&number_tokens);
              for (j=0; j < (number_tokens-1); j+=2)
              {
                keyword=(char *) tokens[j];
                value=(char *) tokens[j+1];
                if (svg_info->debug)
                  (void) fprintf(stdout,"    %.1024s: %.1024s\n",keyword,value);
                current=transform;
                IdentityAffine(&affine);
                switch (*keyword)
                {
                  case 'M':
                  case 'm':
                  {
                    if (LocaleCompare(keyword,"matrix") == 0)
                      {
                        p=(char *) value;
                        GetToken(p,&p,token);
                        affine.sx=atof(value);
                        GetToken(p,&p,token);
                        if (*token == ',')
                          GetToken(p,&p,token);
                        affine.rx=atof(token);
                        GetToken(p,&p,token);
                        if (*token == ',')
                          GetToken(p,&p,token);
                        affine.ry=atof(token);
                        GetToken(p,&p,token);
                        if (*token == ',')
                          GetToken(p,&p,token);
                        affine.sy=atof(token);
                        GetToken(p,&p,token);
                        if (*token == ',')
                          GetToken(p,&p,token);
                        affine.tx=atof(token);
                        GetToken(p,&p,token);
                        if (*token == ',')
                          GetToken(p,&p,token);
                        affine.ty=atof(token);
                        break;
                      }
                    break;
                  }
                  case 'R':
                  case 'r':
                  {
                    if (LocaleCompare(keyword,"rotate") == 0)
                      {
                        double
                          angle;

                        angle=GetUserSpaceCoordinateValue(svg_info,value);
                        affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                        affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                        affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                        affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                        break;
                      }
                    break;
                  }
                  case 'S':
                  case 's':
                  {
                    if (LocaleCompare(keyword,"scale") == 0)
                      {
                        for (p=(char *) value; *p != '\0'; p++)
                          if (isspace((int) (*p)) || (*p == ','))
                            break;
                        affine.sx=GetUserSpaceCoordinateValue(svg_info,value);
                        affine.sy=affine.sx;
                        if (*p != '\0')
                          affine.sy=GetUserSpaceCoordinateValue(svg_info,p+1);
                        svg_info->scale[svg_info->n]=ExpandAffine(&affine);
                        break;
                      }
                    if (LocaleCompare(keyword,"skewX") == 0)
                      {
                        affine.sx=svg_info->affine.sx;
                        affine.ry=tan(DegreesToRadians(fmod(
                          GetUserSpaceCoordinateValue(svg_info,value),360.0)));
                        affine.sy=svg_info->affine.sy;
                        break;
                      }
                    if (LocaleCompare(keyword,"skewY") == 0)
                      {
                        affine.sx=svg_info->affine.sx;
                        affine.rx=tan(DegreesToRadians(fmod(
                          GetUserSpaceCoordinateValue(svg_info,value),
                          360.0)));
                        affine.sy=svg_info->affine.sy;
                        break;
                      }
                    break;
                  }
                  case 'T':
                  case 't':
                  {
                    if (LocaleCompare(keyword,"translate") == 0)
                      {
                        for (p=(char *) value; *p != '\0'; p++)
                          if (isspace((int) (*p)) || (*p == ','))
                            break;
                        affine.tx=GetUserSpaceCoordinateValue(svg_info,value);
                        affine.ty=affine.tx;
                        if (*p != '\0')
                          affine.ty=GetUserSpaceCoordinateValue(svg_info,p+1);
                        break;
                      }
                    break;
                  }
                  default:
                    break;
                }
                transform.sx=current.sx*affine.sx+current.ry*affine.rx;
                transform.rx=current.rx*affine.sx+current.sy*affine.rx;
                transform.ry=current.sx*affine.ry+current.ry*affine.sy;
                transform.sy=current.rx*affine.ry+current.sy*affine.sy;
                transform.tx=current.sx*affine.tx+current.ry*affine.ty+
                  current.tx;
                transform.ty=current.rx*affine.tx+current.sy*affine.ty+
                  current.ty;
              }
              (void) fprintf(svg_info->file,"affine %g %g %g %g %g %g\n",
                transform.sx,transform.rx,transform.ry,transform.sy,
                transform.tx,transform.ty);
              for (j=0; j < number_tokens; j++)
                LiberateMemory((void **) &tokens[j]);
              LiberateMemory((void **) &tokens);
              break;
            }
          break;
        }
        case 'V':
        case 'v':
        {
          if (LocaleCompare(keyword,"verts") == 0)
            {
              CloneString(&svg_info->vertices,value);
              break;
            }
          if (LocaleCompare(keyword,"viewBox") == 0)
            {
              p=(char *) value;
              GetToken(p,&p,token);
              svg_info->view_box.x=atof(token);
              GetToken(p,&p,token);
              if (*token == ',')
                GetToken(p,&p,token);
              svg_info->view_box.y=atof(token);
              GetToken(p,&p,token);
              if (*token == ',')
                GetToken(p,&p,token);
              svg_info->view_box.width=atof(token);
              GetToken(p,&p,token);
              if (*token == ',')
                GetToken(p,&p,token);
              svg_info->view_box.height=atof(token);
              break;
            }
          break;
        }
        case 'W':
        case 'w':
        {
          if (LocaleCompare(keyword,"width") == 0)
            {
              svg_info->bounds.width=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        case 'X':
        case 'x':
        {
          if (LocaleCompare(keyword,"x") == 0)
            {
              svg_info->bounds.x=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"xlink:href") == 0)
            {
              CloneString(&svg_info->url,value);
              break;
            }
          if (LocaleCompare(keyword,"x1") == 0)
            {
              svg_info->segment.x1=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"x2") == 0)
            {
              svg_info->segment.x2=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        case 'Y':
        case 'y':
        {
          if (LocaleCompare(keyword,"y") == 0)
            {
              svg_info->bounds.y=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"y1") == 0)
            {
              svg_info->segment.y1=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"y2") == 0)
            {
              svg_info->segment.y2=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        default:
          break;
      }
      if (font_family != (char *) NULL)
        {
          (void) fprintf(svg_info->file,"font '%s'",font_family);
          if ((font_weight != (char *) NULL) && (font_style != (char *) NULL))
            (void) fprintf(svg_info->file,"-%s%s",font_weight,font_style);
          else
            if (font_weight != (char *) NULL)
              (void) fprintf(svg_info->file,"-%s",font_weight);
            else
              if (font_style != (char *) NULL)
                (void) fprintf(svg_info->file,"-%s",font_style);
          (void) fprintf(svg_info->file,"\n");
        }
      if (font_family != (char *) NULL)
        LiberateMemory((void **) &font_family);
      if (font_style != (char *) NULL)
        LiberateMemory((void **) &font_style);
      if (font_weight != (char *) NULL)
        LiberateMemory((void **) &font_weight);
    }
  if (LocaleCompare((char *) name,"svg") == 0)
    {
      if (attributes != (const xmlChar **) NULL)
        {
          char
            *geometry,
            *p;

          RectangleInfo
            page;

          if ((svg_info->view_box.width == 0.0) ||
              (svg_info->view_box.height == 0.0))
            svg_info->view_box=svg_info->bounds;
          page.width=(unsigned int) svg_info->bounds.width;
          page.height=(unsigned int) svg_info->bounds.height;
          page.x=0;
          page.y=0;
          geometry=(char *) NULL;
          if (svg_info->page != (char *) NULL)
            geometry=PostscriptGeometry(svg_info->page);
          else
            if (svg_info->size != (char *) NULL)
              geometry=PostscriptGeometry(svg_info->size);
          if (geometry != (char *) NULL)
            {
              p=strchr(geometry,'>');
              if (p != (char *) NULL)
                *p='\0';
              (void) ParseImageGeometry(geometry,&page.x,&page.y,
                &page.width,&page.height);
              LiberateMemory((void **) &geometry);
            }
          if (svg_info->affine.sx != 1.0)
            page.width=(unsigned int)
              ceil(ExpandAffine(&svg_info->affine)*page.width-0.5);
          if (svg_info->affine.sy != 0.0)
            page.height=(unsigned int)
              ceil(ExpandAffine(&svg_info->affine)*page.height-0.5);
          (void) fprintf(svg_info->file,"viewbox 0 0 %g %g\n",
            svg_info->view_box.width,svg_info->view_box.height);
          (void) fprintf(svg_info->file,"affine %g 0.0 0.0 %g %g %g\n",
            (double) page.width/svg_info->view_box.width,
            (double) page.height/svg_info->view_box.height,
	    -svg_info->view_box.x,-svg_info->view_box.y);
          svg_info->width=page.width;
          svg_info->height=page.height;
        }
    }
  if (svg_info->debug)
    (void) fprintf(stdout,"  )\n");
  LiberateMemory((void **) &units);
  if (color != (char *) NULL)
    LiberateMemory((void **) &color);
}

static void SVGEndElement(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  /*
    Called when the end of an element has been detected.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.endElement(%.1024s)\n",(char *) name);
  if (svg_info->text != (char *) NULL)
    Strip(svg_info->text);
  switch (*name)
  {
    case 'C':
    case 'c':
    {
      if (LocaleCompare((char *) name,"circle") == 0)
        {
          (void) fprintf(svg_info->file,"circle %g,%g %g,%g\n",
            svg_info->element.cx,svg_info->element.cy,svg_info->element.cx,
            svg_info->element.cy+svg_info->element.minor);
          (void) fprintf(svg_info->file,"pop graphic-context\n");
          break;
        }
      if (LocaleCompare((char *) name,"clipPath") == 0)
        {
          (void) fprintf(svg_info->file,"pop clip-path\n");
          break;
        }
      break;
    }
    case 'D':
    case 'd':
    {
      if (LocaleCompare((char *) name,"defs") == 0)
        {
          (void) fprintf(svg_info->file,"pop defs\n");
          break;
        }
      if (LocaleCompare((char *) name,"desc") == 0)
        {
          register char
            *p;

          (void) fputc('#',svg_info->file);
          for (p=svg_info->text; *p != '\0'; p++)
          {
            (void) fputc(*p,svg_info->file);
            if (*p == '\n')
              (void) fputc('#',svg_info->file);
          }
          (void) fputc('\n',svg_info->file);
          break;
        }
      break;
    }
    case 'E':
    case 'e':
    {
      if (LocaleCompare((char *) name,"ellipse") == 0)
        {
          double
            angle;

          angle=svg_info->element.angle;
          (void) fprintf(svg_info->file,"ellipse %g,%g %g,%g 0,360\n",
            svg_info->element.cx,svg_info->element.cy,
            angle == 0.0 ? svg_info->element.major : svg_info->element.minor,
            angle == 0.0 ? svg_info->element.minor : svg_info->element.major);
          (void) fprintf(svg_info->file,"pop graphic-context\n");
          break;
        }
      break;
    }
    case 'G':
    case 'g':
    {
      if (LocaleCompare((char *) name,"g") == 0)
        {
          (void) fprintf(svg_info->file,"pop graphic-context\n");
          break;
        }
      break;
    }
    case 'I':
    case 'i':
    {
      if (LocaleCompare((char *) name,"image") == 0)
        {
          (void) fprintf(svg_info->file,"image Copy %g,%g %g,%g '%s'\n",
            svg_info->bounds.x,svg_info->bounds.y,svg_info->bounds.width,
            svg_info->bounds.height,svg_info->url);
          (void) fprintf(svg_info->file,"pop graphic-context\n");
          break;
        }
      break;
    }
    case 'L':
    case 'l':
    {
      if (LocaleCompare((char *) name,"line") == 0)
        {
          (void) fprintf(svg_info->file,"line %g,%g %g,%g\n",
            svg_info->segment.x1,svg_info->segment.y1,svg_info->segment.x2,
            svg_info->segment.y2);
          (void) fprintf(svg_info->file,"pop graphic-context\n");
          break;
        }
      if (LocaleCompare((char *) name,"linearGradient") == 0)
        {
          (void) fprintf(svg_info->file,"gradient %g,%g %g,%g\n",
            svg_info->segment.x1,svg_info->segment.y1,svg_info->segment.x2,
            svg_info->segment.y2);
          (void) fprintf(svg_info->file,"pop linear-gradient\n");
          break;
        }
      break;
    }
    case 'P':
    case 'p':
    {
      if (LocaleCompare((char *) name,"path") == 0)
        {
          (void) fprintf(svg_info->file,"path '%s'\n",svg_info->vertices);
          (void) fprintf(svg_info->file,"pop graphic-context\n");
          break;
        }
      if (LocaleCompare((char *) name,"polygon") == 0)
        {
          (void) fprintf(svg_info->file,"polygon %s\n",svg_info->vertices);
          (void) fprintf(svg_info->file,"pop graphic-context\n");
          break;
        }
      if (LocaleCompare((char *) name,"polyline") == 0)
        {
          (void) fprintf(svg_info->file,"polyline %s\n",svg_info->vertices);
          (void) fprintf(svg_info->file,"pop graphic-context\n");
          break;
        }
      break;
    }
    case 'R':
    case 'r':
    {
      if (LocaleCompare((char *) name,"rect") == 0)
        {
          if ((svg_info->radius.x == 0.0) && (svg_info->radius.y == 0.0))
            {
              (void) fprintf(svg_info->file,"rectangle %g,%g %g,%g\n",
                svg_info->bounds.x,svg_info->bounds.y,
                svg_info->bounds.x+svg_info->bounds.width,
                svg_info->bounds.y+svg_info->bounds.height);
              (void) fprintf(svg_info->file,"pop graphic-context\n");
              break;
            }
          if (svg_info->radius.x == 0.0)
            svg_info->radius.x=svg_info->radius.y;
          if (svg_info->radius.y == 0.0)
            svg_info->radius.y=svg_info->radius.x;
          (void) fprintf(svg_info->file,"roundRectangle %g,%g %g,%g %g,%g\n",
            svg_info->bounds.x,svg_info->bounds.y,
            svg_info->bounds.x+svg_info->bounds.width,
            svg_info->bounds.y+svg_info->bounds.height,
            svg_info->radius.x,svg_info->radius.y);
          svg_info->radius.x=0.0;
          svg_info->radius.y=0.0;
          (void) fprintf(svg_info->file,"pop graphic-context\n");
          break;
        }
      break;
    }
    case 'S':
    case 's':
    {
      if (LocaleCompare((char *) name,"svg") == 0)
        {
          (void) fprintf(svg_info->file,"pop graphic-context\n");
          break;
        }
      break;
    }
    case 'T':
    case 't':
    {
      if (LocaleCompare((char *) name,"text") == 0)
        {
          if (strlen(svg_info->text) == 0)
            break;
          if (strchr(svg_info->text,'\'') != (char *) NULL)
            {
              (void) fprintf(svg_info->file,"text %g,%g \"%s\"\n",
                svg_info->bounds.x,svg_info->bounds.y,svg_info->text);
              (void) fprintf(svg_info->file,"pop graphic-context\n");
              break;
            }
          (void) fprintf(svg_info->file,"text %g,%g '%s'\n",svg_info->bounds.x,
            svg_info->bounds.y,svg_info->text);
          (void) fprintf(svg_info->file,"pop graphic-context\n");
          break;
        }
      if (LocaleCompare((char *) name,"title") == 0)
        {
          CloneString(&svg_info->title,svg_info->text);
          break;
        }
      break;
    }
    default:
      break;
  }
  if (svg_info->text != (char *) NULL)
    *svg_info->text='\0';
  svg_info->n--;
}

static void SVGCharacters(void *context,const xmlChar *c,int length)
{
  register char
    *p;

  register int
    i;

  SVGInfo
    *svg_info;

  /*
    Receiving some characters from the parser.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    {
      (void) fprintf(stdout,"  SAX.characters(");
      for (i=0; (i < length) && (i < 30); i++)
        (void) fprintf(stdout,"  %c",c[i]);
      (void) fprintf(stdout,"  , %d)\n",length);
    }
  if (svg_info->text != (char *) NULL)
    ReacquireMemory((void **) &svg_info->text,strlen(svg_info->text)+length+1);
  else
    {
      svg_info->text=(char *) AcquireMemory(length+1);
      if (svg_info->text != (char *) NULL)
        *svg_info->text='\0';
    }
  if (svg_info->text == (char *) NULL)
    return;
  p=svg_info->text+strlen(svg_info->text);
  for (i=0; i < length; i++)
    *p++=c[i];
  *p='\0';
}

static void SVGReference(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    Called when an entity reference is detected.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.reference(%.1024s)\n",name);
  parser=svg_info->parser;
  if (*name == '#')
    xmlAddChild(parser->node,xmlNewCharRef(svg_info->document,name));
  else
    xmlAddChild(parser->node,xmlNewReference(svg_info->document,name));
}

static void SVGIgnorableWhitespace(void *context,const xmlChar *c,int length)
{
  SVGInfo
    *svg_info;

  /*
    Receiving some ignorable whitespaces from the parser.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.ignorableWhitespace(%.30s, %d)\n",(char *) c,
      length);
}

static void SVGProcessingInstructions(void *context,const xmlChar *target,
  const xmlChar *data)
{
  SVGInfo
    *svg_info;

  /*
    A processing instruction has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.processingInstruction(%.1024s, %.1024s)\n",
      (char *) target,(char *) data);
}

static void SVGComment(void *context,const xmlChar *value)
{
  SVGInfo
    *svg_info;

  /*
    A comment has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.comment(%.1024s)\n",value);
  CloneString(&svg_info->comment,(char *) value);
}

static void SVGWarning(void *context,const char *format,...)
{
  char
    reason[MaxTextExtent];

  SVGInfo
    *svg_info;

  va_list
    operands;

  /**
    Display and format a warning messages, gives file, line, position and
    extra parameters.
  */
  va_start(operands,format);
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    {
      (void) fprintf(stdout,"  SAX.warning: ");
      vfprintf(stdout,format,operands);
    }
  svg_info->exception->severity=DelegateWarning;
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  CloneString(&svg_info->exception->reason,reason);
  va_end(operands);
}

static void SVGError(void *context,const char *format,...)
{
  char
    reason[MaxTextExtent];

  SVGInfo
    *svg_info;

  va_list
    operands;

  /*
    Display and format a error formats, gives file, line, position and
    extra parameters.
  */
  va_start(operands,format);
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    {
      (void) fprintf(stdout,"  SAX.error: ");
      vfprintf(stdout,format,operands);
    }
  svg_info->exception->severity=DelegateError;
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  CloneString(&svg_info->exception->reason,reason);
  va_end(operands);
}

static void SVGCDataBlock(void *context,const xmlChar *value,int length)
{
  SVGInfo
    *svg_info;

   xmlNodePtr
     child;

  xmlParserCtxtPtr
    parser;

  /*
    Called when a pcdata block has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.pcdata(%.1024s, %d)\n",(char *) value,length);
  parser=svg_info->parser;
  child=xmlGetLastChild(parser->node);
  if ((child != (xmlNodePtr) NULL) && (child->type == XML_CDATA_SECTION_NODE))
    {
      xmlTextConcat(child,value,length);
      return;
    }
  xmlAddChild(parser->node,xmlNewCDataBlock(parser->myDoc,value,length));
}

static void SVGExternalSubset(void *context,const xmlChar *name,
  const xmlChar *external_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  xmlParserCtxt
    parser_context;

  xmlParserCtxtPtr
    parser;

  xmlParserInputPtr
    input;

  /*
    Does this document has an enternal subset?
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    {
      (void) fprintf(stdout,"  SAX.InternalSubset(%.1024s",name);
      if (external_id != NULL)
        (void) fprintf(stdout,"  , %.1024s",external_id);
      if (system_id != NULL)
        (void) fprintf(stdout,"  , %.1024s",system_id);
      (void) fprintf(stdout,"\n");
    }
  parser=svg_info->parser;
  if (((external_id == NULL) && (system_id == NULL)) ||
      (!parser->validate || !parser->wellFormed || !svg_info->document))
    return;
  input=SVGResolveEntity(context,external_id,system_id);
  if (input == NULL)
    return;
  xmlNewDtd(svg_info->document,name,external_id,system_id);
  parser_context=(*parser);
  parser->inputTab=(xmlParserInputPtr *) xmlMalloc(5*sizeof(xmlParserInputPtr));
  if (parser->inputTab == (xmlParserInputPtr *) NULL)
    {
      parser->errNo=XML_ERR_NO_MEMORY;
      parser->input=parser_context.input;
      parser->inputNr=parser_context.inputNr;
      parser->inputMax=parser_context.inputMax;
      parser->inputTab=parser_context.inputTab;
      return;
  }
  parser->inputNr=0;
  parser->inputMax=5;
  parser->input=NULL;
  xmlPushInput(parser,input);
  xmlSwitchEncoding(parser,xmlDetectCharEncoding(parser->input->cur,4));
  if (input->filename == (char *) NULL)
    input->filename=(char *) xmlStrdup(system_id);
  input->line=1;
  input->col=1;
  input->base=parser->input->cur;
  input->cur=parser->input->cur;
  input->free=NULL;
  xmlParseExternalSubset(parser,external_id,system_id);
  while (parser->inputNr > 1)
    xmlPopInput(parser);
  xmlFreeInputStream(parser->input);
  xmlFree(parser->inputTab);
  parser->input=parser_context.input;
  parser->inputNr=parser_context.inputNr;
  parser->inputMax=parser_context.inputMax;
  parser->inputTab=parser_context.inputTab;
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

static Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  xmlSAXHandler
    SAXModules =
    {
      SVGInternalSubset,
      SVGIsStandalone,
      SVGHasInternalSubset,
      SVGHasExternalSubset,
      SVGResolveEntity,
      SVGGetEntity,
      SVGEntityDeclaration,
      SVGNotationDeclaration,
      SVGAttributeDeclaration,
      SVGElementDeclaration,
      SVGUnparsedEntityDeclaration,
      SVGSetDocumentLocator,
      SVGStartDocument,
      SVGEndDocument,
      SVGStartElement,
      SVGEndElement,
      SVGReference,
      SVGCharacters,
      SVGIgnorableWhitespace,
      SVGProcessingInstructions,
      SVGComment,
      SVGWarning,
      SVGError,
      SVGError,
      SVGGetParameterEntity,
      SVGCDataBlock,
      SVGExternalSubset
    };

  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    message[MaxTextExtent];

  FILE
    *file;

  Image
    *image;

  ImageInfo
    *clone_info;

  size_t
    n;

  SVGInfo
    svg_info;

  unsigned int
    status;

  xmlSAXHandlerPtr
    SAXHandler;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Open draw file.
  */
  UniqueImageFilename(image,filename);
  file=fopen(filename,"w");
  if (file == (FILE *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Parse SVG file.
  */
  memset(&svg_info,0,sizeof(SVGInfo));
  svg_info.file=file;
  svg_info.debug=image_info->debug;
  svg_info.exception=exception;
  svg_info.scale=(double *) AcquireMemory(sizeof(double));
  if (svg_info.scale == (double *) NULL)
    MagickError(ResourceLimitError,"Unable to convert SVG image",
      "Memory allocation failed");
  svg_info.scale[0]=1.0;
  IdentityAffine(&svg_info.affine);
  svg_info.affine.sx=
    image->x_resolution == 0.0 ? 1.0 : image->x_resolution/72.0;
  svg_info.affine.sy=
    image->y_resolution == 0.0 ? 1.0 : image->y_resolution/72.0;
  svg_info.bounds.width=image->columns;
  svg_info.bounds.height=image->rows;
  if (image_info->size != (char *) NULL)
    CloneString(&svg_info.size,image_info->size);
  if (image_info->page != (char *) NULL)
    CloneString(&svg_info.page,image_info->page);
  if (svg_info.debug)
    (void) fprintf(stdout,"begin SAX\n");
  xmlSubstituteEntitiesDefault(1);
  SAXHandler=(&SAXModules);
  svg_info.parser=xmlCreatePushParserCtxt(SAXHandler,&svg_info,(char *) NULL,0,
    image->filename);
  while (ReadBlobString(image,message) != (char *) NULL)
  {
    n=strlen(message);
    if (n == 0)
      continue;
    status=xmlParseChunk(svg_info.parser,message,(int) n,False);
    if (status != 0)
      break;
    (void) xmlParseChunk(svg_info.parser," ",1,False);
  }
  (void) xmlParseChunk(svg_info.parser," ",1,True);
  xmlFreeParserCtxt(svg_info.parser);
  if (svg_info.debug)
    (void) fprintf(stdout,"end SAX\n");
  xmlCleanupParser();
  (void) fclose(file);
  CloseBlob(image);
  DestroyImage(image);
  image=(Image *) NULL;
  if (exception->severity == UndefinedException)
    {
      /*
        Draw image.
      */
      clone_info=CloneImageInfo(image_info);
      RewindBlob(clone_info->blob);
      FormatString(geometry,"%dx%d",svg_info.width,svg_info.height);
      CloneString(&clone_info->size,geometry);
      FormatString(clone_info->filename,"mvg:%.1024s",filename);
      if (clone_info->density != (char *) NULL)
        LiberateMemory((void **) &clone_info->density);
      image=ReadImage(clone_info,exception);
      DestroyImageInfo(clone_info);
      if (image != (Image *) NULL)
        {
          (void) strcpy(image->filename,image_info->filename);
          if (svg_info.comment != (char *) NULL)
            (void) SetImageAttribute(image,"comment",svg_info.comment);
          if (svg_info.title != (char *) NULL)
            (void) SetImageAttribute(image,"title",svg_info.title);
        }
    }
  /*
    Free resources.
  */
  (void) remove(filename);
  if (svg_info.title != (char *) NULL)
    LiberateMemory((void **) &svg_info.title);
  if (svg_info.comment != (char *) NULL)
    LiberateMemory((void **) &svg_info.comment);
  return(image);
}
#else
static Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateWarning,
    "XML library is not available",image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r S V G I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSVGImage adds attributes for the SVG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSVGImage method is:
%
%      RegisterSVGImage(void)
%
*/
ModuleExport void RegisterSVGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("SVG");
  entry->magick=IsSVG;
  entry->decoder=ReadSVGImage;
  entry->encoder=WriteSVGImage;
  entry->description=AllocateString("Scalable Vector Gaphics");
  entry->module=AllocateString("SVG");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("XML");
  entry->magick=IsSVG;
  entry->decoder=ReadSVGImage;
  entry->description=AllocateString("Scalable Vector Gaphics");
  entry->module=AllocateString("SVG");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r S V G I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterSVGImage removes format registrations made by the
%  SVG module from the list of supported formats.
%
%  The format of the UnregisterSVGImage method is:
%
%      UnregisterSVGImage(void)
%
*/
ModuleExport void UnregisterSVGImage(void)
{
  UnregisterMagickInfo("SVG");
  UnregisterMagickInfo("XML");
}

#if defined(HasAUTOTRACE)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e S V G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteSVGImage writes a image in the SVG - XML based W3C standard
%  format.
%
%  The format of the WriteSVGImage method is:
%
%      unsigned int WriteSVGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteSVGImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteSVGImage(const ImageInfo *image_info,Image *image)
{
  FILE
    *output_file;

  fitting_opts_type
    fit_info;

  image_header_type
    image_header;

  bitmap_type
    bitmap;

  ImageType
    image_type;

  int
    j;

  pixel_outline_list_type
    pixels;

  PixelPacket
    p;

  PixelPacket
    *pixel;

  QuantizeObj
    *quantize_info;

  spline_list_array_type
    splines;

  output_write
    output_writer;

  register int
    i;

  size_t
    number_pixels;

  unsigned int
    number_planes,
    point,
    thin;

  thin=False;
  quantize_info=(QuantizeObj *) NULL;
  pixel=(&p);
  fit_info=new_fitting_opts();
  output_writer=output_get_handler("svg");
  if (output_writer == NULL)
    ThrowWriterException(FileOpenWarning,"Unable to write svg format",image);
  image_type=GetImageType(image);
  number_planes=3;
  if ((image_type == BilevelType) || (image_type == GrayscaleType))
    number_planes=1;
  bitmap.np=number_planes;
  bitmap.dimensions.width=image->columns;
  bitmap.dimensions.height=image->rows;
  number_pixels=image->columns*image->rows;
  bitmap.bitmap=(unsigned char *) AcquireMemory(number_planes*number_pixels);
  if (bitmap.bitmap == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
      image);
  point=0;
  for (j=0; j < image->rows; j++)
  {
    for (i=0; i < image->columns; i++)
    {
      p=GetOnePixel(image,i,j);
      bitmap.bitmap[point++]=pixel->red;
      if (number_planes == 3)
        {
          bitmap.bitmap[point++]=pixel->green;
          bitmap.bitmap[point++]=pixel->blue;
        }
    }
  }
  image_header.width=DIMENSIONS_WIDTH(bitmap.dimensions);
  image_header.height=DIMENSIONS_HEIGHT(bitmap.dimensions);
  if ((fit_info.color_count > 0) && (BITMAP_PLANES(bitmap) == 3))
    quantize(bitmap.bitmap,bitmap.bitmap,DIMENSIONS_WIDTH(bitmap.dimensions),
      DIMENSIONS_HEIGHT(bitmap.dimensions),fit_info.color_count,
      fit_info.bgColor,&quantize_info);
  if (thin)
    thin_image(&bitmap);
  pixels=find_outline_pixels (bitmap);
  LiberateMemory((void **) &(bitmap.bitmap));
  splines=fitted_splines(pixels,&fit_info);
  output_file=fopen(image->filename,"w");
  if (output_file == (FILE *) NULL)
    ThrowWriterException(FileOpenWarning,"Unable to open the output file",
      image);
  output_writer(output_file,image->filename,0,0,image_header.width,
    image_header.height,splines);
  return(True);
}
#else

static void AffineToTransform(Image *image,AffineMatrix *affine)
{
  char
    transform[MaxTextExtent];

  if ((fabs(affine->tx) < MagickEpsilon) && (fabs(affine->ty) < MagickEpsilon))
    {
      if ((fabs(affine->rx) < MagickEpsilon) &&
          (fabs(affine->ry) < MagickEpsilon))
        {
          if ((fabs(affine->sx-1.0) < MagickEpsilon) &&
              (fabs(affine->sy-1.0) < MagickEpsilon))
            return;
          FormatString(transform,"\" transform=\"scale(%g,%g)",
            affine->sx,affine->sy);
          (void) WriteBlobString(image,transform);
          return;
        }
      else
        {
          if ((fabs(affine->sx-affine->sy) < MagickEpsilon) &&
              (fabs(affine->rx+affine->ry) < MagickEpsilon) &&
              (fabs(affine->sx*affine->sx+affine->rx*affine->rx-1.0) <
               2*MagickEpsilon))
            {
              double
                theta;

              theta=(180.0/MagickPI)*atan2(affine->rx,affine->sx);
              FormatString(transform,"\" transform=\"rotate(%g)",theta);
              (void) WriteBlobString(image,transform);
              return;
            }
        }
    }
  else
    {
      if ((fabs(affine->sx-1.0) < MagickEpsilon) &&
          (fabs(affine->rx) < MagickEpsilon) &&
          (fabs(affine->ry) < MagickEpsilon) &&
          (fabs(affine->sy-1.0) < MagickEpsilon))
        {
          FormatString(transform,"\" transform=\"translate(%g,%g)",
            affine->tx,affine->ty);
          (void) WriteBlobString(image,transform);
          return;
        }
    }
  FormatString(transform,"\"\n  transform=\"matrix(%g,%g,%g,%g,%g,%g)",
    affine->sx,affine->rx,affine->ry,affine->sy,affine->tx,affine->ty);
  (void) WriteBlobString(image,transform);
}

static unsigned int WriteSVGImage(const ImageInfo *image_info,Image *image)
{
#define BezierQuantum  200

  AffineMatrix
    affine;

  char
    keyword[MaxTextExtent],
    message[MaxTextExtent],
    *p,
    *q,
    *token;

  ImageAttribute
    *attribute;

  int
    n;

  PointInfo
    point;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  register int
    x;

  register size_t
    i;

  size_t
    j,
    length;

  unsigned int
    active,
    status;

  unsigned long
    number_points;

  /*
    Open output image file.
  */
  attribute=GetImageAttribute(image,"[MVG]");
  if ((attribute == (ImageAttribute *) NULL) ||
      (attribute->value == (char *) NULL))
    ThrowWriterException(DelegateWarning,"no image vector graphics",image);
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  /*
    Write SVG header.
  */
  (void) WriteBlobString(image,"<?xml version=\"1.0\" standalone=\"no\"?>\n");
  (void) WriteBlobString(image,
    "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20000802//EN\"\n");
  (void) WriteBlobString(image,
    "  \"http://www.w3.org/TR/2000/CR-SVG-20000802/DTD/svg-20000802.dtd\">\n");
  (void) FormatString(message,"<svg width=\"%u\" height=\"%u\">\n",
    image->columns,image->rows);
  (void) WriteBlobString(image,message);
  /*
    Allocate primitive info memory.
  */
  number_points=2047;
  primitive_info=(PrimitiveInfo *)
    AcquireMemory(number_points*sizeof(PrimitiveInfo));
  if (primitive_info == (PrimitiveInfo *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  token=AllocateString(attribute->value);
  active=False;
  n=0;
  status=True;
  for (q=attribute->value; *q != '\0'; )
  {
    /*
      Interpret graphic primitive.
    */
    GetToken(q,&q,keyword);
    if (*keyword == '\0')
      break;
    if (*keyword == '#')
      {
        /*
          Comment.
        */
        if (active)
          (void) WriteBlobString(image,"\">\n");
        active=False;
        WriteBlobString(image,"<desc>");
        for (q++; (*q != '\n') && (*q != '\0'); q++)
          switch (*q)
          {
            case '<': WriteBlobString(image,"&lt;"); break;
            case '>': WriteBlobString(image,"&gt;"); break;
            case '&': WriteBlobString(image,"&amp;"); break;
            default: WriteBlobByte(image,*q); break;
          }
        WriteBlobString(image,"</desc>\n");
        continue;
      }
    primitive_type=UndefinedPrimitive;
    switch (*keyword)
    {
      case ';':
        break;
      case 'a':
      case 'A':
      {
        if (LocaleCompare("affine",keyword) == 0)
          {
            GetToken(q,&q,token);
            affine.sx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.rx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.ry=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.sy=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.tx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.ty=atof(token);
            AffineToTransform(image,&affine);
            break;
          }
        if (LocaleCompare("angle",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"rotate(%.1024s) ",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("arc",keyword) == 0)
          {
            primitive_type=ArcPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'b':
      case 'B':
      {
        if (LocaleCompare("bezier",keyword) == 0)
          {
            primitive_type=BezierPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'c':
      case 'C':
      {
        if (LocaleCompare("clip-path",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"clip-path:url(#%.1024s);",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("clip-rule",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"clip-rule:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("clip-units",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"clipPathUnits=%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("circle",keyword) == 0)
          {
            primitive_type=CirclePrimitive;
            break;
          }
        if (LocaleCompare("color",keyword) == 0)
          {
            primitive_type=ColorPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'd':
      case 'D':
      {
        if (LocaleCompare("decorate",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"text-decoration:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        status=False;
        break;
      }
      case 'e':
      case 'E':
      {
        if (LocaleCompare("ellipse",keyword) == 0)
          {
            primitive_type=EllipsePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'f':
      case 'F':
      {
        if (LocaleCompare("fill",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"fill:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("fill-rule",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"fill-rule:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("fill-opacity",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"fill-opacity:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("font",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"font-family:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("font-size",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"font-size:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        status=False;
        break;
      }
      case 'g':
      case 'G':
      {
        if (LocaleCompare("gradient-units",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"gradientUnits=%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("gravity",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("NorthWest",token) == 0)
              WriteBlobString(image,"text-align:left;");
            if (LocaleCompare("NorthEast",token) == 0)
              WriteBlobString(image,"text-align:right;");
            if (LocaleCompare("North",token) == 0)
              WriteBlobString(image,"text-align:center;");
            break;
          }
        status=False;
        break;
      }
      case 'i':
      case 'I':
      {
        if (LocaleCompare("image",keyword) == 0)
          {
            GetToken(q,&q,token);
            primitive_type=ImagePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'l':
      case 'L':
      {
        if (LocaleCompare("line",keyword) == 0)
          {
            primitive_type=LinePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'm':
      case 'M':
      {
        if (LocaleCompare("matte",keyword) == 0)
          {
            primitive_type=MattePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'o':
      case 'O':
      {
        if (LocaleCompare("opacity",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"opacity %.1024s ",token);
            WriteBlobString(image,message);
            break;
          }
        status=False;
        break;
      }
      case 'p':
      case 'P':
      {
        if (LocaleCompare("path",keyword) == 0)
          {
            primitive_type=PathPrimitive;
            break;
          }
        if (LocaleCompare("point",keyword) == 0)
          {
            primitive_type=PointPrimitive;
            break;
          }
        if (LocaleCompare("polyline",keyword) == 0)
          {
            primitive_type=PolylinePrimitive;
            break;
          }
        if (LocaleCompare("polygon",keyword) == 0)
          {
            primitive_type=PolygonPrimitive;
            break;
          }
        if (LocaleCompare("pop",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("clip-path",token) == 0)
              {
                (void) WriteBlobString(image,"</clipPath>\n");
                break;
              }
            if (LocaleCompare("defs",token) == 0)
              {
                (void) WriteBlobString(image,"</defs>\n");
                break;
              }
            if (LocaleCompare("graphic-context",token) == 0)
              {
                n--;
                if (n < 0)
                  ThrowWriterException(CorruptImageWarning,
                    "unbalanced graphic context push/pop",image);
              }
            if (LocaleCompare("linear-gradient",token) == 0)
              {
                (void) WriteBlobString(image,"</linearGradient>\n");
                break;
              }
            (void) WriteBlobString(image,"</g>\n");
            break;
          }
        if (LocaleCompare("push",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("clip-path",token) == 0)
              {
                GetToken(q,&q,token);
                FormatString(message,"<clipPath id=\"%s\">\n",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("defs",token) == 0)
              {
                (void) WriteBlobString(image,"<defs>\n");
                break;
              }
            if (LocaleCompare("graphic-context",token) == 0)
              {
                n++;
                if (active)
                  (void) WriteBlobString(image,"\">\n");
                (void) WriteBlobString(image,"<g style=\"");
                active=True;
              }
            if (LocaleCompare("linear-gradient",token) == 0)
              {
                GetToken(q,&q,token);
                FormatString(message,"<linearGradient id=\"%s\">\n",token);
                (void) WriteBlobString(image,message);
                break;
              }
            break;
          }
        status=False;
        break;
      }
      case 'r':
      case 'R':
      {
        if (LocaleCompare("rectangle",keyword) == 0)
          {
            primitive_type=RectanglePrimitive;
            break;
          }
        if (LocaleCompare("roundRectangle",keyword) == 0)
          {
            primitive_type=RoundRectanglePrimitive;
            break;
          }
        if (LocaleCompare("rotate",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"rotate(%.1024s) ",token);
            WriteBlobString(image,message);
            break;
          }
        status=False;
        break;
      }
      case 's':
      case 'S':
      {
        if (LocaleCompare("scale",keyword) == 0)
          {
            GetToken(q,&q,token);
            affine.sx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.sy=atof(token);
            FormatString(message,"scale(%g,%g) ",affine.sx,affine.sy);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("skewX",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"skewX(%.1024s) ",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("skewY",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"skewY(%.1024s) ",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"stroke:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-antialias",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"stroke-antialias:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-dasharray",keyword) == 0)
          {
            if (IsGeometry(q))
              {
                size_t
                  k;

                p=q;
                GetToken(p,&p,token);
                for (k=0; IsGeometry(token); k++)
                  GetToken(p,&p,token);
                WriteBlobString(image,"stroke-dasharray:");
                for (j=0; j < k; j++)
                {
                  GetToken(q,&q,token);
                  FormatString(message,"%.1024s ",token);
                  WriteBlobString(image,message);
                }
                WriteBlobString(image,";");
                break;
              }
            GetToken(q,&q,token);
            FormatString(message,"stroke-dasharray:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-dashoffset",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"stroke-dashoffset:%.1024;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-linecap",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"stroke-linecap:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-linejoin",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"stroke-linejoin:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-miterlimit",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"stroke-miterlimit:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-opacity",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"stroke-opacity:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("stroke-width",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"stroke-width:%.1024s;",token);
            WriteBlobString(image,message);
            continue;
          }
        status=False;
        break;
      }
      case 't':
      case 'T':
      {
        if (LocaleCompare("text",keyword) == 0)
          {
            primitive_type=TextPrimitive;
            break;
          }
        if (LocaleCompare("text-antialias",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(message,"text-antialias:%.1024s;",token);
            WriteBlobString(image,message);
            break;
          }
        if (LocaleCompare("translate",keyword) == 0)
          {
            GetToken(q,&q,token);
            affine.tx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.ty=atof(token);
            FormatString(message,"translate(%g,%g) ",affine.tx,affine.ty);
            WriteBlobString(image,message);
            break;
          }
        status=False;
        break;
      }
      case 'v':
      case 'V':
      {
        if (LocaleCompare("viewbox",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            GetToken(q,&q,token);
            break;
          }
        status=False;
        break;
      }
      default:
      {
        status=False;
        break;
      }
    }
    if (status == False)
      break;
    if (primitive_type == UndefinedPrimitive)
      continue;
    /*
      Parse the primitive attributes.
    */
    i=0;
    j=0;
    for (x=0; *q != '\0'; x++)
    {
      /*
        Define points.
      */
      if (!IsGeometry(q))
        break;
      GetToken(q,&q,token);
      point.x=atof(token);
      GetToken(q,&q,token);
      if (*token == ',')
        GetToken(q,&q,token);
      point.y=atof(token);
      GetToken(q,(char **) NULL,token);
      if (*token == ',')
        GetToken(q,&q,token);
      primitive_info[i].primitive=primitive_type;
      primitive_info[i].point=point;
      primitive_info[i].coordinates=0;
      primitive_info[i].method=FloodfillMethod;
      i++;
      if (i < (number_points-6*BezierQuantum-360))
        continue;
      number_points+=6*BezierQuantum+360;
      ReacquireMemory((void **) &primitive_info,
        number_points*sizeof(PrimitiveInfo));
      if (primitive_info == (PrimitiveInfo *) NULL)
        {
          ThrowException(&image->exception,ResourceLimitWarning,
            "Unable to draw image","Memory allocation failed");
          break;
        }
    }
    primitive_info[j].primitive=primitive_type;
    primitive_info[j].coordinates=x;
    primitive_info[j].method=FloodfillMethod;
    primitive_info[j].text=(char *) NULL;
    if (active)
      (void) WriteBlobString(image,"\">\n");
    active=False;
    switch (primitive_type)
    {
      case PointPrimitive:
      default:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        break;
      }
      case LinePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        (void) FormatString(message,
          "  <line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"/>\n",
          primitive_info[j].point.x,primitive_info[j].point.y,
          primitive_info[j+1].point.x,primitive_info[j+1].point.y);
        (void) WriteBlobString(image,message);
        break;
      }
      case RectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        (void) FormatString(message,
          "  <rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\"/>\n",
          primitive_info[j].point.x,primitive_info[j].point.y,
          primitive_info[j+1].point.x-primitive_info[j].point.x,
          primitive_info[j+1].point.y-primitive_info[j].point.y);
        (void) WriteBlobString(image,message);
        break;
      }
      case RoundRectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=False;
            break;
          }
        (void) FormatString(message,"  <rect x=\"%g\" y=\"%g\" "
          "width=\"%g\" height=\"%g\" rx=\"%g\" ry=\"%g\"/>\n",
          primitive_info[j].point.x,primitive_info[j].point.y,
          primitive_info[j+1].point.x-primitive_info[j].point.x,
          primitive_info[j+1].point.y-primitive_info[j].point.y,
          primitive_info[j+2].point.x,primitive_info[j+2].point.y);
        (void) WriteBlobString(image,message);
        break;
      }
      case ArcPrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=False;
            break;
          }
        break;
      }
      case EllipsePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=False;
            break;
          }
        (void) FormatString(message,
          "  <ellipse cx=\"%g\" cy=\"%g\" rx=\"%g\" ry=\"%g\"/>\n",
          primitive_info[j].point.x,primitive_info[j].point.y,
          primitive_info[j+1].point.x,primitive_info[j+1].point.y);
        (void) WriteBlobString(image,message);
        break;
      }
      case CirclePrimitive:
      {
        double
          alpha,
          beta;

        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        alpha=primitive_info[j+1].point.x-primitive_info[j].point.x;
        beta=primitive_info[j+1].point.y-primitive_info[j].point.y;
        (void) FormatString(message,"  <circle cx=\"%g\" cy=\"%g\" r=\"%g\"/>\n",
          primitive_info[j].point.x,primitive_info[j].point.y,
          sqrt(alpha*alpha+beta*beta));
        (void) WriteBlobString(image,message);
        break;
      }
      case PolylinePrimitive:
      {
        if (primitive_info[j].coordinates < 2)
          {
            status=False;
            break;
          }
        (void) strcpy(message,"  <polyline points=\"");
        (void) WriteBlobString(image,message);
        length=strlen(message);
        for ( ; j < i; j++)
        {
          FormatString(message,"%g,%g ",primitive_info[j].point.x,
            primitive_info[j].point.y);
          length+=strlen(message);
          if (length >= 80)
            {
              (void) WriteBlobString(image,"\n    ");
              length=strlen(message)+5;
            }
          (void) WriteBlobString(image,message);
        }
        (void) WriteBlobString(image,"\"/>\n");
        break;
      }
      case PolygonPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=False;
            break;
          }
        primitive_info[i]=primitive_info[j];
        primitive_info[i].coordinates=0;
        primitive_info[j].coordinates++;
        i++;
        (void) strcpy(message,"  <polygon points=\"");
        (void) WriteBlobString(image,message);
        length=strlen(message);
        for ( ; j < i; j++)
        {
          FormatString(message,"%g,%g ",primitive_info[j].point.x,
            primitive_info[j].point.y);
          length+=strlen(message);
          if (length >= 80)
            {
              (void) WriteBlobString(image,"\n    ");
              length=strlen(message)+5;
            }
          (void) WriteBlobString(image,message);
        }
        (void) WriteBlobString(image,"\"/>\n");
        break;
      }
      case BezierPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=False;
            break;
          }
        break;
      }
      case PathPrimitive:
      {
        int
          number_attributes;

        GetToken(q,&q,token);
        number_attributes=1;
        for (p=token; *p != '\0'; p++)
          if (isalpha((int) *p))
            number_attributes++;
        if (i > (number_points-6*BezierQuantum*number_attributes-1))
          {
            number_points+=6*BezierQuantum*number_attributes;
            ReacquireMemory((void **) &primitive_info,
              number_points*sizeof(PrimitiveInfo));
            if (primitive_info == (PrimitiveInfo *) NULL)
              {
                ThrowException(&image->exception,ResourceLimitWarning,
                  "Unable to draw image","Memory allocation failed");
                break;
              }
          }
        (void) WriteBlobString(image,"  <path d=\"");
        (void) WriteBlobString(image,token);
        (void) WriteBlobString(image,"\"/>\n");
        break;
      }
      case ColorPrimitive:
      case MattePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        GetToken(q,&q,token);
        if (LocaleCompare("point",token) == 0)
          primitive_info[j].method=PointMethod;
        if (LocaleCompare("replace",token) == 0)
          primitive_info[j].method=ReplaceMethod;
        if (LocaleCompare("floodfill",token) == 0)
          primitive_info[j].method=FloodfillMethod;
        if (LocaleCompare("filltoborder",token) == 0)
          primitive_info[j].method=FillToBorderMethod;
        if (LocaleCompare("reset",token) == 0)
          primitive_info[j].method=ResetMethod;
        break;
      }
      case TextPrimitive:
      {
        register char
          *p;

        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        GetToken(q,&q,token);
        (void) FormatString(message,"  <text x=\"%g\" y=\"%g\">",
          primitive_info[j].point.x,primitive_info[j].point.y);
        (void) WriteBlobString(image,message);
        for (p=token; *p != '\0'; p++)
          switch (*p)
          {
            case '<': WriteBlobString(image,"&lt;"); break;
            case '>': WriteBlobString(image,"&gt;"); break;
            case '&': WriteBlobString(image,"&amp;"); break;
            default: WriteBlobByte(image,*p); break;
          }
        (void) WriteBlobString(image,"</text>\n");
        break;
      }
      case ImagePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        GetToken(q,&q,token);
        (void) FormatString(message,"  <image x=\"%g\" y=\"%g\" "
          "width=\"%g\" height=\"%g\" xlink:href=\"%.1024s\"/>\n",
          primitive_info[j].point.x,primitive_info[j].point.y,
          primitive_info[j+1].point.x,primitive_info[j+1].point.y,token);
        (void) WriteBlobString(image,message);
        break;
      }
    }
    if (primitive_info == (PrimitiveInfo *) NULL)
      break;
    primitive_info[i].primitive=UndefinedPrimitive;
    if (status == False)
      break;
  }
  (void) WriteBlobString(image,"</svg>\n");
  /*
    Free resources.
  */
  LiberateMemory((void **) &token);
  if (primitive_info != (PrimitiveInfo *) NULL)
    LiberateMemory((void **) &primitive_info);
  CloseBlob(image);
  return(status);
}
#endif
