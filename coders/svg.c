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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"
#if defined(HasXML)
#ifdef WIN32
#include <win32config.h>
#endif
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/parserInternals.h>
#if defined(HAVE_LIBXML_XML_ERROR_H)
#include <libxml/xml-error.h>
#elif defined(HAVE_LIBXML_XMLERROR_H)
#include <libxml/xmlerror.h>
#endif
#endif

#if defined(HasAUTOTRACE)
/*
  Forward declarations.
*/
static unsigned int
  WriteSVGImage(const ImageInfo *,Image *);
#endif

/*
  Typedef declaractions.
*/
#if defined(HasXML)
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
    verbose;

  ExceptionInfo
    *exception;

  AffineInfo
    affine;

  int
    width,
    height;

  char
    *size,
    *page,
    *title,
    *description,
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
%      unsigned int IsSVG(const unsigned char *magick,
%        const unsigned int length)
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
static unsigned int IsSVG(const unsigned char *magick,
  const unsigned int length)
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
    *p;

  double
    value;

  register int
    n;

  assert(string != (const char *) NULL);
  n=svg_info->n;
  p=(char *) string;
  value=strtod(p,&p);
  if (*p == '%')
    {
      double
        scale;

      scale=ExpandAffine(&svg_info->affine);
      return(value*svg_info->width/svg_info->scale[n]/scale/100.0);
    }
  if (LocaleNCompare(p,"cm",2) == 0)
    return(72.0/2.54*value/svg_info->scale[n]);
  if (LocaleNCompare(p,"em",2) == 0)
    return(svg_info->pointsize*value/svg_info->scale[n]);
  if (LocaleNCompare(p,"ex",2) == 0)
    return(svg_info->pointsize*value/svg_info->scale[n]/2.0);
  if (LocaleNCompare(p,"in",2) == 0)
    return(72.0*value/svg_info->scale[n]);
  if (LocaleNCompare(p,"mm",2) == 0)
    return(72.0/25.4*value/svg_info->scale[n]);
  if (LocaleNCompare(p,"pc",2) == 0)
    return(72.0/6.0*value/svg_info->scale[n]);
  if (LocaleNCompare(p,"pt",2) == 0)
    return(value/svg_info->scale[n]);
  if (LocaleNCompare(p,"px",2) == 0)
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
  {
    if (*p == ':')
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
  if (svg_info->verbose)
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
  if (svg_info->verbose)
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
  if (svg_info->verbose)
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
  if (svg_info->verbose)
    {
      (void) fprintf(stdout,"  SAX.internalSubset(%s",name);
      if (external_id != NULL)
        (void) fprintf(stdout,"  , %s",external_id);
      if (system_id != NULL)
        (void) fprintf(stdout,"  , %s",system_id);
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
  if (svg_info->verbose)
    {
      (void) fprintf(stdout,"  SAX.resolveEntity(");
      if (public_id != NULL)
        (void) fprintf(stdout,"  %s",(char *) public_id);
      else
        (void) fprintf(stdout,"   ");
      if (system_id != NULL)
        (void) fprintf(stdout,"  , %s)\n",(char *) system_id);
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
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.SVGGetEntity(%s)\n",name);
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
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.getParameterEntity(%s)\n",name);
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
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.entityDecl(%s, %d, %s, %s, %s)\n",name,type,
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

void SVGAttributeDeclaration(void *context,const xmlChar *element,
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
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.attributeDecl(%s, %s, %d, %d, %s, ...)\n",
      element,name,type,value,default_value);
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
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.elementDecl(%s, %d, ...)\n",name,type);
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
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.notationDecl(%s, %s, %s)\n",(char *) name,
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
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.unparsedEntityDecl(%s, %s, %s, %s)\n",
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
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.setDocumentLocator()\n");
}

static void SVGStartDocument(void *context)
{
  register int
    i;

  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    Called when the document start being processed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
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
  register int
    i;

  SVGInfo
    *svg_info;

  /*
    Called when the document end has been detected.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
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
    **tokens;

  const char
    *keyword,
    *value;

  int
    number_tokens;

  SVGInfo
    *svg_info;

  register int
    i,
    j,
    k;

  /*
    Called when an opening tag has been processed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.startElement(%s",(char *) name);
  svg_info->n++;
  ReacquireMemory((void **) &svg_info->scale,(svg_info->n+1)*sizeof(double));
  if (svg_info->scale == (double *) NULL)
    MagickError(ResourceLimitError,"Unable to convert SVG image",
      "Memory allocation failed");
  svg_info->scale[svg_info->n]=svg_info->scale[svg_info->n-1];
  if (LocaleCompare((char *) name,"svg") != 0)
    (void) fprintf(svg_info->file,"push graphic-context\n");
  color=AllocateString("none");
  font_family=(char *) NULL;
  font_style=(char *) NULL;
  font_weight=(char *) NULL;
  if (attributes != (const xmlChar **) NULL)
    for (i=0; (attributes[i] != (const xmlChar *) NULL); i+=2)
    {
      keyword=(const char *) attributes[i];
      value=(const char *) attributes[i+1];
      if (svg_info->verbose)
        {
          (void) fprintf(stdout,"  , %s='",keyword);
          (void) fprintf(stdout,"  %s'",value);
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
              (void) fprintf(svg_info->file,"fill-opacity %g\n",atof(value)*
                (strchr(value,'%') != (char *) NULL ? 1.0 : 100.0));
              break;
            }
          if (LocaleCompare(keyword,"font-family") == 0)
            {
              font_family=AllocateString(value);
              break;
            }
          if (LocaleCompare(keyword,"font-style") == 0)
            {
              font_style=AllocateString(value);
              *font_style=toupper((int) *font_style);
              break;
            }
          if (LocaleCompare(keyword,"font-size") == 0)
            {
              svg_info->pointsize=GetUserSpaceCoordinateValue(svg_info,value);
              (void) fprintf(svg_info->file,"pointsize %g\n",
                svg_info->pointsize);
              break;
            }
          if (LocaleCompare(keyword,"font-weight") == 0)
            {
              font_weight=AllocateString(value);
              *font_weight=toupper((int) *font_weight);
              break;
            }
          break;
        }
        case 'H':
        case 'h':
        {
          if (LocaleCompare(keyword,"height") == 0)
            {
              svg_info->bounds.height=
                GetUserSpaceCoordinateValue(svg_info,value);
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
              svg_info->element.major=
                GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"minor") == 0)
            {
              svg_info->element.minor=
                GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        case 'O':
        case 'o':
        {
          if (LocaleCompare(keyword,"opacity") == 0)
            {
              (void) fprintf(svg_info->file,"opacity %g\n",atof(value)*
                (strchr(value,'%') != (char *) NULL ? 1.0 : 100.0));
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
              svg_info->element.major=
                GetUserSpaceCoordinateValue(svg_info,value);
              svg_info->element.minor=
                GetUserSpaceCoordinateValue(svg_info,value);
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
              (void) fprintf(svg_info->file,"stroke-opacity %g\n",atof(value)*
                (strchr(value,'%') != (char *) NULL ? 1.0 : 100.0));
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
              if (svg_info->verbose)
                (void) fprintf(stdout,"  \n");
              tokens=GetStyleTokens(value,&number_tokens);
              for (j=0; j < (number_tokens-1); j+=2)
              {
                keyword=(char *) tokens[j];
                value=(char *) tokens[j+1];
                if (svg_info->verbose)
                  (void) fprintf(stdout,"    %s: %s\n",keyword,value);
                switch (*keyword)
                {
                  case 'C':
                  case 'c':
                  {
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
                        (void) fprintf(svg_info->file,"fill-opacity %g\n",
                          atof(value)*(strchr(value,'%') != (char *) NULL ?
                          1.0 : 100.0));
                        break;
                      }
                    if (LocaleCompare(keyword,"font-family") == 0)
                      {
                        font_family=AllocateString(value);
                        break;
                      }
                    if (LocaleCompare(keyword,"font-style") == 0)
                      {
                        font_style=AllocateString(value);
                        *font_style=toupper((int) *font_style);
                        break;
                      }
                    if (LocaleCompare(keyword,"font-size") == 0)
                      {
                        svg_info->pointsize=
                          GetUserSpaceCoordinateValue(svg_info,value);
                        (void) fprintf(svg_info->file,"pointsize %g\n",
                          svg_info->pointsize);
                        break;
                      }
                    if (LocaleCompare(keyword,"font-weight") == 0)
                      {
                        font_weight=AllocateString(value);
                        *font_weight=toupper((int) *font_weight);
                        break;
                      }
                    break;
                  }
                  case 'O':
                  case 'o':
                  {
                    if (LocaleCompare(keyword,"opacity") == 0)
                      {
                        (void) fprintf(svg_info->file,"opacity %g\n",
                          atof(value)*(strchr(value,'%') != (char *) NULL ?
                          1.0 : 100.0));
                        break;
                      }
                    break;
                  }
                  case 'S':
                  case 's':
                  {
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
                        (void) fprintf(svg_info->file,"stroke-opacity %g\n",
                          atof(value)*(strchr(value,'%') != (char *) NULL ?
                          1.0 : 100.0));
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
              AffineInfo
                affine,
                current,
                transform;

              IdentityAffine(&transform);
              if (svg_info->verbose)
                (void) fprintf(stdout,"  \n");
              tokens=GetTransformTokens(value,&number_tokens);
              for (j=0; j < (number_tokens-1); j+=2)
              {
                keyword=(char *) tokens[j];
                value=(char *) tokens[j+1];
                if (svg_info->verbose)
                  (void) fprintf(stdout,"    %s: %s\n",keyword,value);
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
                        affine.sx=strtod(p,&p);
                        if (*p ==',')
                          p++;
                        affine.rx=strtod(p,&p);
                        if (*p ==',')
                          p++;
                        affine.ry=strtod(p,&p);
                        if (*p ==',')
                          p++;
                        affine.sy=strtod(p,&p);
                        if (*p ==',')
                          p++;
                        affine.tx=strtod(p,&p);
                        if (*p ==',')
                          p++;
                        affine.ty=strtod(p,&p);
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
                          GetUserSpaceCoordinateValue(svg_info,value),
                          360.0)));
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
              svg_info->view_box.x=strtod(p,&p);
              if (*p == ',');
                p++;
              svg_info->view_box.y=strtod(p,&p);
              if (*p == ',');
                p++;
              svg_info->view_box.width=strtod(p,&p);
              if (*p == ',');
                p++;
              svg_info->view_box.height=strtod(p,&p);
              if (*p == ',');
                p++;
              break;
            }
          break;
        }
        case 'W':
        case 'w':
        {
          if (LocaleCompare(keyword,"width") == 0)
            {
              svg_info->bounds.width=
                GetUserSpaceCoordinateValue(svg_info,value);
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
          (void) fprintf(svg_info->file,"font %s",font_family);
          if ((font_style != (char *) NULL) && (font_weight != (char *) NULL))
            (void) fprintf(svg_info->file,"-%s-%s",font_weight,
              font_style);
          else
            if (font_style != (char *) NULL)
              (void) fprintf(svg_info->file,"-%s",font_style);
            else
              if (font_weight != (char *) NULL)
                (void) fprintf(svg_info->file,"-%s",font_weight);
          (void) fprintf(svg_info->file,"\n");
        }
      if (font_family != (char *) NULL)
        LiberateMemory((void **) &font_family);
      if (font_style != (char *) NULL)
        LiberateMemory((void **) &font_style);
      if (font_weight != (char *) NULL)
        LiberateMemory((void **) &font_weight);
    }
  if (svg_info->verbose)
    (void) fprintf(stdout,"  )\n");
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
          page.width=svg_info->bounds.width;
          page.height=svg_info->bounds.height;
          page.x=0;
          page.y=0;
          geometry=(char *) NULL;
          if (svg_info->size != (char *) NULL)
            geometry=PostscriptGeometry(svg_info->size);
          if (svg_info->page != (char *) NULL)
            geometry=PostscriptGeometry(svg_info->page);
          if (geometry != (char *) NULL)
            {
              p=strchr(geometry,'>');
              if (p != (char *) NULL)
                *p='\0';
              (void) ParseImageGeometry(geometry,&page.x,&page.y,
                &page.width,&page.height);
              DestroyPostscriptGeometry(geometry);
            }
          if (svg_info->affine.sx != 1.0)
            page.width=(unsigned int)
              ceil(ExpandAffine(&svg_info->affine)*page.width-0.5);
          if (svg_info->affine.sy != 0.0)
            page.height=(unsigned int)
              ceil(ExpandAffine(&svg_info->affine)*page.height-0.5);
          (void) fprintf(svg_info->file,"viewbox %g %g %g %g\n",
            svg_info->view_box.x,svg_info->view_box.y,svg_info->view_box.width,
            svg_info->view_box.height);
          (void) fprintf(svg_info->file,"affine %g 0.0 0.0 %g 0.0 0.0\n",
            (double) page.width/svg_info->view_box.width,
            (double) page.height/svg_info->view_box.height);
          svg_info->width=page.width;
          svg_info->height=page.height;
        }
    }
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
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.endElement(%s)\n",(char *) name);
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
          break;
        }
      break;
    }
    case 'D':
    case 'd':
    {
      if (LocaleCompare((char *) name,"desc") == 0)
        {
          CloneString(&svg_info->description,svg_info->text);
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
          break;
        }
      break;
    }
    case 'I':
    case 'i':
    {
      if (LocaleCompare((char *) name,"image") == 0)
        {
          (void) fprintf(svg_info->file,"image %g,%g %g,%g %s\n",
            svg_info->bounds.x,svg_info->bounds.y,svg_info->bounds.width,
            svg_info->bounds.height,svg_info->url);
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
          break;
        }
      if (LocaleCompare((char *) name,"polygon") == 0)
        {
          (void) fprintf(svg_info->file,"polygon %s\n",svg_info->vertices);
          break;
        }
      if (LocaleCompare((char *) name,"polyline") == 0)
        {
          (void) fprintf(svg_info->file,"polyline %s\n",svg_info->vertices);
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
          break;
        }
      break;
    }
    case 'T':
    case 't':
    {
      if (LocaleCompare((char *) name,"text") == 0)
        {
          if (strchr(svg_info->text,'\'') != (char *) NULL)
            {
              (void) fprintf(svg_info->file,"text %g,%g \"%s\"\n",
                svg_info->bounds.x,svg_info->bounds.y,svg_info->text);
              break;
            }
          (void) fprintf(svg_info->file,"text %g,%g '%s'\n",svg_info->bounds.x,
            svg_info->bounds.y,svg_info->text);
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
  if (LocaleCompare((char *) name,"svg") != 0)
    (void) fprintf(svg_info->file,"pop graphic-context\n");
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
  if (svg_info->verbose)
    {
      (void) fprintf(stdout,"  SAX.characters(");
      for (i=0; (i < length) && (i < 30); i++)
        (void) fprintf(stdout,"  %c",c[i]);
      (void) fprintf(stdout,"  , %d)\n",length);
    }
  if (svg_info->text != (char *) NULL)
    ReacquireMemory((void **) &svg_info->text,Extent(svg_info->text)+length+1);
  else
    {
      svg_info->text=(char *) AcquireMemory(length+1);
      if (svg_info->text != (char *) NULL)
        *svg_info->text='\0';
    }
  if (svg_info->text == (char *) NULL)
    return;
  p=svg_info->text+Extent(svg_info->text);
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
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.reference(%s)\n",name);
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
  if (svg_info->verbose)
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
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.processingInstruction(%s, %s)\n",(char *) target,
      (char *) data);
}

static void SVGComment(void *context,const xmlChar *value)
{
  SVGInfo
    *svg_info;

  /*
    A comment has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.comment(%s)\n",value);
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
  if (svg_info->verbose)
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
  if (svg_info->verbose)
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
  if (svg_info->verbose)
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
  if (svg_info->verbose)
    {
      (void) fprintf(stdout,"  SAX.InternalSubset(%s",name);
      if (external_id != NULL)
        (void) fprintf(stdout,"  , %s",external_id);
      if (system_id != NULL)
        (void) fprintf(stdout,"  , %s",system_id);
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
    SAXHandlerStruct =
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
    buffer[MaxTextExtent],
    filename[MaxTextExtent],
    geometry[MaxTextExtent];

  FILE
    *file;

  Image
    *image;

  ImageInfo
    *clone_info;

  int
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
  TemporaryFilename(filename);
  file=fopen(filename,"w");
  if (file == (FILE *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Parse SVG file.
  */
  memset(&svg_info,0,sizeof(SVGInfo));
  svg_info.file=file;
  svg_info.verbose=image_info->verbose;
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
  if (svg_info.verbose)
    (void) fprintf(stdout,"begin SAX\n");
  xmlSubstituteEntitiesDefault(1);
  SAXHandler=(&SAXHandlerStruct);
  svg_info.parser=xmlCreatePushParserCtxt(SAXHandler,&svg_info,(char *) NULL,0,
    image->filename);
  while (GetStringBlob(image,buffer) != (char *) NULL)
  {
    n=Extent(buffer);
    if (n == 0)
      continue;
    status=xmlParseChunk(svg_info.parser,buffer,n,False);
    if (status != 0)
      break;
    (void) xmlParseChunk(svg_info.parser," ",1,False);
  }
  /* (void) xmlParseChunk(svg_info.parser," ",1,True); */
  xmlFreeParserCtxt(svg_info.parser);
  if (svg_info.verbose)
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
            (void) SetImageAttribute(image,"Comment",svg_info.comment);
          if (svg_info.description != (char *) NULL)
            (void) SetImageAttribute(image,"Description",svg_info.description);
          if (svg_info.title != (char *) NULL)
            (void) SetImageAttribute(image,"Title",svg_info.title);
        }
    }
  /*
    Free resources.
  */
  (void) remove(filename);
  if (svg_info.title != (char *) NULL)
    LiberateMemory((void **) &svg_info.title);
  if (svg_info.description != (char *) NULL)
    LiberateMemory((void **) &svg_info.description);
  if (svg_info.comment != (char *) NULL)
    LiberateMemory((void **) &svg_info.comment);
  return(image);
}
#else
static Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateWarning,
    "SVG library is not available",image_info->filename);
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
#if defined(HasAUTOTRACE)
  entry->encoder=WriteSVGImage;
#endif
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
}

#if defined(HasAUTOTRACE)
/* autotrace -- convert bitmaps to splines. */
#include "types.h"
#include "image-header.h"
#include "fit.h"
#include "output.h"
#include "pxl-outline.h"
#include "atquantize.h"
#include "thin-image.h" 

char *version_string = "AutoTrace version 0.24a";
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

  bool
    thin;

  fitting_opts_type
    fitting_opts;

  image_header_type
    image_header;

  pixel_outline_list_type
    pixels;

  spline_list_array_type
    splines;

  bitmap_type
    bitmap;

  ImageType
    image_type;

  PixelPacket
    p;

  PixelPacket
    *pixel;

  QuantizeObj
    *myQuant;

  output_write
    output_writer;

  unsigned int
    i,
    j,
    point,
    np;

  thin = false;
  myQuant = NULL;
  pixel=&p;

  fitting_opts = new_fitting_opts ();

  output_writer = output_get_handler("svg");
  if (output_writer == NULL)
    ThrowWriterException(FileOpenWarning,"Unable to write svg format",image);

  image_type=GetImageType(image);
  if(image_type == BilevelType || image_type == GrayscaleType)
    np=1;
  else
    np=3;

  bitmap.np=np;
  bitmap.dimensions.width=image->columns;
  bitmap.dimensions.height=image->rows;
  bitmap.bitmap=(unsigned char*)AcquireMemory(np*image->columns*image->rows);
  for(j=0,point=0;j<image->rows;j++)
  {
    for(i=0;i<image->columns;i++)
    {
      p=GetOnePixel(image,i,j);
      bitmap.bitmap[point++]=pixel->red; /* if gray: red=green=blue */
      if (np==3)
      {
        bitmap.bitmap[point++]=pixel->green;
        bitmap.bitmap[point++]=pixel->blue;
      }
    }
  }
  image_header.width = DIMENSIONS_WIDTH (bitmap.dimensions);
  image_header.height = DIMENSIONS_HEIGHT (bitmap.dimensions);

  if (fitting_opts.color_count > 0 && BITMAP_PLANES(bitmap)== 3)
    quantize (bitmap.bitmap, bitmap.bitmap, DIMENSIONS_WIDTH (bitmap.dimensions),
      DIMENSIONS_HEIGHT (bitmap.dimensions), fitting_opts.color_count,
      fitting_opts.bgColor, &myQuant);
  if (thin)
    thin_image (&bitmap); 
 
  pixels = find_outline_pixels (bitmap);
  LiberateMemory((void **) &(bitmap.bitmap));
  splines = fitted_splines (pixels, &fitting_opts);

  output_file = fopen(image->filename, "w");
  if (output_file == (FILE *) NULL)
    ThrowWriterException(FileOpenWarning,"Unable to open the output file",image);
  output_writer (output_file, image->filename,
		0, 0, image_header.width, image_header.height, splines);
  
  return(True);
}
#endif