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

/*
  Typedef declaractions.
*/
#if defined(HasXML)
#include <libxml/parser.h>
#include <libxml/xml-error.h>
typedef struct _ElementInfo
{
  double
    cx,
    cy,
    major,
    minor,
    angle;
} ElementInfo;

typedef struct _GraphicContext
{
  char
    *fill,
    *stroke,
    *font;

  unsigned int
    antialias;

  GravityType
    gravity;

  DecorationType
    decorate;

  double
    linewidth,
    pointsize,
    opacity,
    angle,
    affine[6];
} GraphicContext;

typedef struct _SVGInfo
{
  FILE
    *file;

  unsigned int
    verbose;

  ExceptionInfo
    *exception;

  int
    width,
    height;

  char
    *comment;

  int
    n;

  GraphicContext
    *graphic_context;

  ElementInfo
    element;

  SegmentInfo
    segment;

  BoundingBox
    page;

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

static char **StringToTokens(const char *text,int *number_tokens)
{
  char
    **tokens;

  register char
    *p,
    *q;

  register int
    i;

  *number_tokens=0;
  if (text == (char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=(char *) text; *p != '\0'; )
  {
    while (isspace((int) (*p)))
      p++;
    (*number_tokens)++;
    if (*p == '"')
      for (p++; (*p != '"') && (*p != '\0'); p++);
    if (*p == '\'')
      for (p++; (*p != '\'') && (*p != '\0'); p++);
    if ((LocaleNCompare(p,"rgb(",4) == 0) || (*p == '('))
      for (p++; (*p != ')') && (*p != '\0'); p++);
    while (!isspace((int) (*p)) && (*p != '(') && (*p != '\0'))
    {
      p++;
      if (!isspace((int) *p) && ((*(p-1) == ':') || (*(p-1) == ';')))
        (*number_tokens)++;
    }
  }
  tokens=(char **) AllocateMemory((*number_tokens+1)*sizeof(char *));
  if (tokens == (char **) NULL)
    MagickError(ResourceLimitError,"Unable to convert string to tokens",
      "Memory allocation failed");
  /*
    Convert string to an ASCII list.
  */
  p=(char *) text;
  for (i=0; i < *number_tokens; i++)
  {
    while (isspace((int) (*p)))
      p++;
    q=p;
    if (*q == '"')
      {
        p++;
        for (q++; (*q != '"') && (*q != '\0'); q++);
      }
    else
      if (*q == '\'')
        {
          for (q++; (*q != '\'') && (*q != '\0'); q++);
          q++;
        }
      else
        if ((LocaleNCompare(q,"rgb(",4) == 0) || (*q == '('))
          {
            for (q++; (*q != ')') && (*q != '\0'); q++);
            q++;
          }
        else
          while (!isspace((int) (*q)) && (*q != '(') && (*q != '\0'))
          {
            q++;
            if (!isspace((int) *q) && ((*(q-1) == ':') || (*(q-1) == ';')))
              break;
          }
    tokens[i]=(char *) AllocateMemory(q-p+1);
    if (tokens[i] == (char *) NULL)
      MagickError(ResourceLimitError,"Unable to convert string to tokens",
        "Memory allocation failed");
    (void) strncpy(tokens[i],p,q-p);
    tokens[i][q-p]='\0';
    if ((q > (p+1)) && (tokens[i][q-p-1] == ';'))
      tokens[i][q-p-1]='\0';
    p=q;
    if ((*(q-1) == ':') || (*(q-1) == ';') || (*q == '('))
      continue;
    while (!isspace((int) (*p)) && (*p != '\0'))
      p++;
  }
  tokens[i]=(char *) NULL;
  return(tokens);
}

static double UnitOfMeasure(const char *value)
{
  assert(value != (const char *) NULL);
  if (Extent(value) < 3)
    return(1.0);
  if (LocaleCompare(value+strlen(value)-2,"cm") == 0)
    return(72.0/2.54);
  if (LocaleCompare(value+strlen(value)-2,"in") == 0)
    return(72.0);
  if (LocaleCompare(value+strlen(value)-2,"pt") == 0)
    return(1.0);
  return(1.0);
}

static int SVGIsStandalone(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Is this document tagged standalone?
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"SAX.SVGIsStandalone()\n");
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
    (void) fprintf(stdout,"SAX.SVGHasInternalSubset()\n");
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
    (void) fprintf(stdout,"SAX.SVGHasExternalSubset()\n");
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
      (void) fprintf(stdout,"SAX.internalSubset(%s",name);
      if (external_id != NULL)
        (void) fprintf(stdout,", %s",external_id);
      if (system_id != NULL)
        (void) fprintf(stdout,", %s",system_id);
      (void) fprintf(stdout,"\n");
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
      (void) fprintf(stdout,"SAX.resolveEntity(");
      if (public_id != NULL)
        (void) fprintf(stdout,"%s",(char *) public_id);
      else
        (void) fprintf(stdout," ");
      if (system_id != NULL)
        (void) fprintf(stdout,", %s)\n",(char *) system_id);
      else
        (void) fprintf(stdout,", )\n");
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
    (void) fprintf(stdout,"SAX.SVGGetEntity(%s)\n",name);
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
    (void) fprintf(stdout,"SAX.getParameterEntity(%s)\n",name);
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
    (void) fprintf(stdout,"SAX.entityDecl(%s, %d, %s, %s, %s)\n",name,type,
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
    (void) fprintf(stdout,"SAX.attributeDecl(%s, %s, %d, %d, %s, ...)\n",
      element,name,type,value,default_value);
  fullname=NULL;
  prefix=NULL;
  parser=svg_info->parser;
  fullname=(xmlChar *) xmlSplitQName(parser,name,&prefix);
  if (parser->inSubset == 1)
    (void) xmlAddAttributeDecl(&parser->vctxt,svg_info->document->intSubset,
      element,fullname,prefix,type,value,default_value,tree);
  else
    if (parser->inSubset == 2)
      (void) xmlAddAttributeDecl(&parser->vctxt,svg_info->document->extSubset,
        element,fullname,prefix,type,value,default_value,tree);
  if (prefix != NULL)
    xmlFree(prefix);
  if (fullname != NULL)
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
    (void) fprintf(stdout,"SAX.elementDecl(%s, %d, ...)\n",name,type);
  parser=svg_info->parser;
  if (parser->inSubset == 1)
    (void) xmlAddElementDecl(&parser->vctxt,svg_info->document->intSubset,
      name,type,content);
  else
    if (parser->inSubset == 2)
      (void) xmlAddElementDecl(&parser->vctxt,svg_info->document->extSubset,
        name,type,content);
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
    (void) fprintf(stdout,"SAX.notationDecl(%s, %s, %s)\n",(char *) name,
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
    (void) fprintf(stdout,"SAX.unparsedEntityDecl(%s, %s, %s, %s)\n",
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
    (void) fprintf(stdout,"SAX.setDocumentLocator()\n");
}

static void SVGStartDocument(void *context)
{
  register int
    i;

  SVGInfo
    *svg_info;

  /*
    Called when the document start being processed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"SAX.startDocument()\n");
  svg_info->width=640;
  svg_info->height=480;
  svg_info->graphic_context=(GraphicContext *)
    AllocateMemory(sizeof(GraphicContext));
  if (svg_info->graphic_context == (GraphicContext *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate memory",(char *) NULL);
  svg_info->graphic_context[0].fill=AllocateString("none");
  svg_info->graphic_context[0].stroke=AllocateString("none");
  svg_info->graphic_context[0].font=AllocateString("none");
  svg_info->graphic_context[0].antialias=True;
  svg_info->graphic_context[0].gravity=NorthWestGravity;
  svg_info->graphic_context[0].decorate=NoDecoration;
  svg_info->graphic_context[0].linewidth=1.0;
  svg_info->graphic_context[0].pointsize=12.0;
  svg_info->graphic_context[0].opacity=100.0;
  for (i=0; i < 6; i++)
    svg_info->graphic_context[0].affine[i]=(i == 0) || (i == 3) ? 1.0 : 0.0;
  GetExceptionInfo(svg_info->exception);
  svg_info->document=xmlNewDoc(svg_info->parser->version);
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
    (void) fprintf(stdout,"SAX.endDocument()\n");
  for (i=0; i <= svg_info->n; i++)
  {
    FreeMemory((void **) &svg_info->graphic_context[i].fill);
    FreeMemory((void **) &svg_info->graphic_context[i].stroke);
    FreeMemory((void **) &svg_info->graphic_context[i].font);
  }
  FreeMemory((void **) &svg_info->graphic_context);
  if (svg_info->text != (char *) NULL)
    FreeMemory((void **) &svg_info->text);
  if (svg_info->vertices != (char *) NULL)
    FreeMemory((void **) &svg_info->vertices);
  if (svg_info->url != (char *) NULL)
    FreeMemory((void **) &svg_info->url);
}

static void SVGStartElement(void *context,const xmlChar *name,
  const xmlChar **attributes)
{
  char
    *keyword,
    *p,
    **tokens;

  const char
    *value;

  double
    affine[6],
    current[6];

  int
    number_tokens;

  register GraphicContext
    *q;

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
    (void) fprintf(stdout,"SAX.startElement(%s",(char *) name);
  svg_info->n++;
  ReallocateMemory((void **) &svg_info->graphic_context,
    (svg_info->n+1)*sizeof(GraphicContext));
  if (svg_info->graphic_context == (GraphicContext *) NULL)
    return;
  q=svg_info->graphic_context+svg_info->n;
  *q=(*(q-1));
  q->fill=AllocateString((q-1)->fill);
  q->stroke=AllocateString((q-1)->stroke);
  q->font=AllocateString((q-1)->font);
  if (attributes != (const xmlChar **) NULL)
    for (i=0; (attributes[i] != (const xmlChar *) NULL); i+=2)
    {
      keyword=(char *) attributes[i];
      value=(char *) attributes[i+1];
      if (svg_info->verbose)
        {
          (void) fprintf(stdout,", %s='",keyword);
          (void) fprintf(stdout,"%s'",value);
        }
      if (LocaleCompare(keyword,"angle") == 0)
        {
          svg_info->element.angle=atof(value);
          continue;
        }
      if (LocaleCompare(keyword,"cx") == 0)
        {
          svg_info->element.cx=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"cy") == 0)
        {
          svg_info->element.cy=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"d") == 0)
        {
          CloneString(&svg_info->vertices,value);
          continue;
        }
      if (LocaleCompare(keyword,"height") == 0)
        {
          svg_info->page.height=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"href") == 0)
        {
          CloneString(&svg_info->url,value);
          continue;
        }
      if (LocaleCompare(keyword,"major") == 0)
        {
          svg_info->element.major=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"minor") == 0)
        {
          svg_info->element.minor=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"path") == 0)
        {
          CloneString(&svg_info->url,value);
          continue;
        }
      if (LocaleCompare(keyword,"points") == 0)
        {
          CloneString(&svg_info->vertices,value);
          continue;
        }
      if (LocaleCompare(keyword,"r") == 0)
        {
          svg_info->element.major=atof(value)*UnitOfMeasure(value);
          svg_info->element.minor=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"rx") == 0)
        {
          svg_info->element.major=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"ry") == 0)
        {
          svg_info->element.minor=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"style") == 0)
        {
          if (svg_info->verbose)
            (void) fprintf(stdout,"\n");
          tokens=StringToTokens(value,&number_tokens);
          for (j=0; j < (number_tokens-1); j+=2)
          {
            keyword=(char *) tokens[j];
            value=(char *) tokens[j+1];
            if (svg_info->verbose)
              (void) fprintf(stdout,"  %s %s\n",keyword,value);
            if (LocaleCompare(keyword,"fill:") == 0)
              {
                (void) CloneString(&q->fill,value);
                continue;
              }
            if (LocaleCompare(keyword,"fillcolor:") == 0)
              {
                (void) CloneString(&q->fill,value);
                continue;
              }
            if (LocaleCompare(keyword,"font-family:") == 0)
              {
                (void) CloneString(&q->font,value);
                continue;
              }
            if (LocaleCompare(keyword,"fill-opacity:") == 0)
              {
                q->opacity=atof(value);
                if (strchr(value,'%') == (char *) NULL)
                  q->opacity*=100.0;
                continue;
              }
            if (LocaleCompare(keyword,"font-size:") == 0)
              {
                q->pointsize=atof(value)*UnitOfMeasure(value);
                continue;
              }
            if (LocaleCompare(keyword,"opacity:") == 0)
              {
                q->opacity=atof(value);
                if (strchr(value,'%') == (char *) NULL)
                  q->opacity*=100.0;
                continue;
              }
            if (LocaleCompare(keyword,"stroke:") == 0)
              {
                (void) CloneString(&q->stroke,value);
                continue;
              }
            if (LocaleCompare(keyword,"stroke-antialiasing:") == 0)
              {
                q->antialias=LocaleCompare(value,"true") == 0;
                continue;
              }
            if (LocaleCompare(keyword,"stroke-opacity:") == 0)
              {
                q->opacity=atof(value);
                continue;
              }
            if (LocaleCompare(keyword,"stroke-width:") == 0)
              {
                q->linewidth=atof(value)*UnitOfMeasure(value);
                continue;
              }
            if (LocaleCompare(keyword,"text-align:") == 0)
              {
                if (LocaleCompare(value,"center") == 0)
                  q->gravity=NorthGravity;
                if (LocaleCompare(value,"left") == 0)
                  q->gravity=NorthWestGravity;
                if (LocaleCompare(value,"right") == 0)
                  q->gravity=NorthEastGravity;
                continue;
              }
            if (LocaleCompare(keyword,"text-decoration:") == 0)
              {
                if (LocaleCompare(value,"underline") == 0)
                  q->decorate=UnderlineDecoration;
                if (LocaleCompare(value,"line-through") == 0)
                  q->decorate=LineThroughDecoration;
                if (LocaleCompare(value,"overline") == 0)
                  q->decorate=OverlineDecoration;
                continue;
              }
            if (LocaleCompare(keyword,"text-antialiasing:") == 0)
              {
                q->antialias=LocaleCompare(value,"true");
                continue;
              }
          }
          for (j=0; j < number_tokens; j++)
            FreeMemory((void **) &tokens[j]);
          FreeMemory((void **) &tokens);
          continue;
        }
      if (LocaleCompare(keyword,"transform") == 0)
        {
          if (svg_info->verbose)
            (void) fprintf(stdout,"\n");
          tokens=StringToTokens(value,&number_tokens);
          for (j=0; j < (number_tokens-1); j+=2)
          {
            keyword=(char *) tokens[j];
            value=(char *) tokens[j+1];
            if (svg_info->verbose)
              (void) fprintf(stdout,"  %s %s\n",keyword,value);
            for (k=0; k < 6; k++)
            {
              current[k]=q->affine[k];
              affine[k]=(k == 0) || (k == 3) ? 1.0 : 0.0;
            }
            if (LocaleCompare(keyword,"matrix") == 0)
              {
                p=(char *) (value+1);
                for (k=0; k < 6; k++)
                {
                  affine[k]=strtod(p,&p);
                  if (*p ==',')
                    p++;
                }
              }
            if (LocaleCompare(keyword,"rotate") == 0)
              {
                double
                  angle;

                angle=atof(value+1);
                affine[0]=cos(DegreesToRadians(fmod(angle,360.0)));
                affine[1]=(-sin(DegreesToRadians(fmod(angle,360.0))));
                affine[2]=sin(DegreesToRadians(fmod(angle,360.0)));
                affine[3]=cos(DegreesToRadians(fmod(angle,360.0)));
              }
            if (LocaleCompare(keyword,"scale") == 0)
              {
                p=(char *) (value+1);
                affine[0]=strtod(p,&p);
                affine[3]=affine[0];
              }
            if (LocaleCompare(keyword,"skewX") == 0)
              {
                affine[0]=1.0;
                affine[2]=
                  tan(DegreesToRadians(fmod(atof(value+1),360.0)));
                affine[3]=1.0;
              }
            if (LocaleCompare(keyword,"skewY") == 0)
              {
                affine[0]=1.0;
                affine[1]=
                  tan(DegreesToRadians(fmod(atof(value+1),360.0)));
                affine[3]=1.0;
              }
            if (LocaleCompare(keyword,"translate") == 0)
              {
                affine[0]=1.0;
                affine[3]=1.0;
                k=sscanf(value+1,"%lf%lf",&affine[4],&affine[5]);
                k=sscanf(keyword+1,"%lf,%lf",&affine[4],&affine[5]);
                if (k == 1)
                  affine[5]=affine[4];
              }
            q->affine[0]=current[0]*affine[0]+current[2]*affine[1];
            q->affine[1]=current[1]*affine[0]+current[3]*affine[1];
            q->affine[2]=current[0]*affine[2]+current[2]*affine[3];
            q->affine[3]=current[1]*affine[2]+current[3]*affine[3];
            q->affine[4]=current[0]*affine[4]+current[2]*affine[5]+current[4];
            q->affine[5]=current[1]*affine[4]+current[3]*affine[5]+current[5];
          }
          for (j=0; j < number_tokens; j++)
            FreeMemory((void **) &tokens[j]);
          FreeMemory((void **) &tokens);
          continue;
        }
      if (LocaleCompare(keyword,"verts") == 0)
        {
          CloneString(&svg_info->vertices,value);
          continue;
        }
      if (LocaleCompare(keyword,"viewBox") == 0)
        {
          p=(char *) value;
          svg_info->page.x=strtod(p,&p);
          if (*p == ',');
            p++;
          svg_info->page.y=strtod(p,&p);
          if (*p == ',');
            p++;
          svg_info->page.width=strtod(p,&p);
          if (*p == ',');
            p++;
          svg_info->page.height=strtod(p,&p);
          if (*p == ',');
            p++;
          continue;
        }
      if (LocaleCompare(keyword,"width") == 0)
        {
          svg_info->page.width=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"x") == 0)
        {
          svg_info->page.x=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"verts") == 0)
        {
          CloneString(&svg_info->vertices,value);
          continue;
        }
      if (LocaleCompare(keyword,"xlink:href") == 0)
        {
          CloneString(&svg_info->url,value);
          continue;
        }
      if (LocaleCompare(keyword,"x1") == 0)
        {
          svg_info->segment.x1=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"x2") == 0)
        {
          svg_info->segment.x2=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"y") == 0)
        {
          svg_info->page.y=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"y1") == 0)
        {
          svg_info->segment.y1=atof(value)*UnitOfMeasure(value);
          continue;
        }
      if (LocaleCompare(keyword,"y2") == 0)
        {
          svg_info->segment.y2=atof(value)*UnitOfMeasure(value);
          continue;
        }
    }
  if (svg_info->verbose)
    (void) fprintf(stdout,")\n");
  if (LocaleCompare((char *) name,"svg") == 0)
    {
      if (attributes != (const xmlChar **) NULL)
        for (i=0; (attributes[i] != (const xmlChar *) NULL); i+=2)
        {
          keyword=(char *) attributes[i];
          value=(char *) attributes[i+1];
          if (LocaleCompare(keyword,"height") == 0)
            svg_info->height=svg_info->page.height;
          if (LocaleCompare(keyword,"width") == 0)
            svg_info->width=svg_info->page.width;
          if (LocaleCompare(keyword,"viewBox") == 0)
            {
              svg_info->height=svg_info->page.height;
              svg_info->width=svg_info->page.width;
            }
        }
    }
}

static void SVGEndElement(void *context,const xmlChar *name)
{
  double
    angle;

  register GraphicContext
    *p;

  register int
    i;

  SVGInfo
    *svg_info;

  /*
    Called when the end of an element has been detected.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"SAX.endElement(%s)\n",(char *) name);
  p=svg_info->graphic_context+svg_info->n;
  (void) fprintf(svg_info->file,"antialias %d\n",p->antialias);
  switch (p->gravity)
  {
    case NorthEastGravity:
    {
      (void) fprintf(svg_info->file,"gravity NorthEast\n");
      break;
    }
    case NorthGravity:
    {
      (void) fprintf(svg_info->file,"gravity North\n");
      break;
    }
    case NorthWestGravity:
    {
      (void) fprintf(svg_info->file,"gravity NorthWest\n");
      break;
    }
  }
  switch (p->decorate)
  {
    case NoDecoration:
    {
      (void) fprintf(svg_info->file,"decorate none\n");
      break;
    }
    case UnderlineDecoration:
    {
      (void) fprintf(svg_info->file,"decorate underline\n");
      break;
    }
    case LineThroughDecoration:
    {
      (void) fprintf(svg_info->file,"decorate line-through\n");
      break;
    }
    case OverlineDecoration:
    {
      (void) fprintf(svg_info->file,"decorate overline\n");
      break;
    }
  }
  (void) fprintf(svg_info->file,"linewidth %g\n",p->linewidth);
  (void) fprintf(svg_info->file,"pointsize %g\n",p->pointsize);
  (void) fprintf(svg_info->file,"opacity %g\n",p->opacity);
  (void) fprintf(svg_info->file,"fill %s\n",p->fill);
  (void) fprintf(svg_info->file,"stroke %s\n",p->stroke);
  if (LocaleCompare(p->font,"none") != 0)
    (void) fprintf(svg_info->file,"font %s\n",p->font);
  (void) fprintf(svg_info->file,"angle %f\n",p->angle);
  (void) fprintf(svg_info->file,"affine ");
  for (i=0; i < 6; i++)
    (void) fprintf(svg_info->file,"%g ",p->affine[i]);
  (void) fprintf(svg_info->file,"\n");
  FreeMemory((void **) &p->fill);
  FreeMemory((void **) &p->stroke);
  FreeMemory((void **) &p->font);
  svg_info->n--;
  if (LocaleCompare((char *) name,"circle") == 0)
    {
      (void) fprintf(svg_info->file,"circle %g,%g %g,%g\n",
        svg_info->element.cx,svg_info->element.cy,svg_info->element.cx,
        svg_info->element.cy+svg_info->element.minor);
      return;
    }
  if (LocaleCompare((char *) name,"ellipse") == 0)
    {
      angle=svg_info->element.angle;
      (void) fprintf(svg_info->file,"ellipse %g,%g %g,%g 0,360\n",
        svg_info->element.cx,svg_info->element.cy,
        angle == 0.0 ? svg_info->element.major : svg_info->element.minor,
        angle == 0.0 ? svg_info->element.minor : svg_info->element.major);
      return;
    }
  if (LocaleCompare((char *) name,"image") == 0)
    {
      (void) fprintf(svg_info->file,"image %f,%f %s\n",
        svg_info->page.x,svg_info->page.y,svg_info->url);
      return;
    }
  if (LocaleCompare((char *) name,"line") == 0)
    {
      (void) fprintf(svg_info->file,"line %g,%g %g,%g\n",
        svg_info->segment.x1,svg_info->segment.y1,svg_info->segment.x2,
        svg_info->segment.y2);
      return;
    }
  if (LocaleCompare((char *) name,"path") == 0)
    {
      (void) fprintf(svg_info->file,"path '%s'\n",svg_info->vertices);
      return;
    }
  if (LocaleCompare((char *) name,"polygon") == 0)
    {
      (void) fprintf(svg_info->file,"polygon %s\n",svg_info->vertices);
      return;
    }
  if (LocaleCompare((char *) name,"polyline") == 0)
    {
      (void) fprintf(svg_info->file,"polyline %s\n",svg_info->vertices);
      return;
    }
  if (LocaleCompare((char *) name,"rect") == 0)
    {
      if (svg_info->element.major == 0.0)
        (void) fprintf(svg_info->file,"rectangle %g,%g %g,%g\n",
          svg_info->page.x,svg_info->page.y,svg_info->page.x+
          svg_info->page.width,svg_info->page.y+svg_info->page.height);
      else
        (void) fprintf(svg_info->file,"roundRectangle %g,%g %g,%g %g,%g\n",
          svg_info->page.x+svg_info->page.width/2.0,svg_info->page.y+
          svg_info->page.height/2.0,svg_info->page.width,svg_info->page.height,
          svg_info->element.major/2.0,svg_info->element.minor/2.0);
      return;
    }
  if (LocaleCompare((char *) name,"text") == 0)
    {
      (void) fprintf(svg_info->file,"text %g,%g '%s'\n",svg_info->page.x,
        svg_info->page.y,svg_info->text);
      return;
    }
}

static void SVGCharacters(void *context,const xmlChar *c,int length)
{
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
      (void) fprintf(stdout,"SAX.characters(");
      for (i=0; (i < length) && (i < 30); i++)
        (void) fprintf(stdout,"%c",c[i]);
      (void) fprintf(stdout,", %d)\n",length);
    }
  if (svg_info->text == (char *) NULL)
    svg_info->text=(char *) AllocateMemory(length+1);
  else
    ReallocateMemory((void **) &svg_info->text,length+1);
  if (svg_info->text == (char *) NULL)
    return;
  for (i=0; i < length; i++)
    svg_info->text[i]=c[i];
  svg_info->text[i]='\0';
  Strip(svg_info->text);
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
    (void) fprintf(stdout,"SAX.reference(%s)\n",name);
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
    (void) fprintf(stdout,"SAX.ignorableWhitespace(%.30s, %d)\n",(char *) c,
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
    (void) fprintf(stdout,"SAX.processingInstruction(%s, %s)\n",(char *) target,
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
    (void) fprintf(stdout,"SAX.comment(%s)\n",value);
  CloneString(&svg_info->comment,(char *) value);
}

static void SVGWarning(void *context,const char *format,...)
{
  char
    message[MaxTextExtent];

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
      (void) fprintf(stdout,"SAX.warning: ");
      vfprintf(stdout,format,operands);
    }
  svg_info->exception->severity=DelegateWarning;
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(message,format,operands);
#else
  (void) vsnprintf(message,MaxTextExtent,format,operands);
#endif
  CloneString(&svg_info->exception->message,message);
  va_end(operands);
}

static void SVGError(void *context,const char *format,...)
{
  char
    message[MaxTextExtent];

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
      (void) fprintf(stdout,"SAX.error: ");
      vfprintf(stdout,format,operands);
    }
  svg_info->exception->severity=DelegateError;
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(message,format,operands);
#else
  (void) vsnprintf(message,MaxTextExtent,format,operands);
#endif
  CloneString(&svg_info->exception->message,message);
  va_end(operands);
}

static void SVGCDataBlock(void *context,const xmlChar *value,int length)
{
  SVGInfo
    *svg_info;

  /*
    Called when a pcdata block has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stderr, "SAX.pcdata(%.20s, %d)\n",(char *) value,length);
}

static void SVGExternalSubset(void *context,const xmlChar *name,
  const xmlChar *external_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  xmlParserCtxt
    wenus;

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
      (void) fprintf(stdout,"SAX.InternalSubset(%s",name);
      if (external_id != NULL)
        (void) fprintf(stdout,", %s",external_id);
      if (system_id != NULL)
        (void) fprintf(stdout,", %s",system_id);
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
  wenus=(*parser);
  parser->inputTab=(xmlParserInputPtr *) xmlMalloc(5*sizeof(xmlParserInputPtr));
  if (parser->inputTab == NULL)
    {
      parser->errNo=XML_ERR_NO_MEMORY;
      parser->input=wenus.input;
      parser->inputNr=wenus.inputNr;
      parser->inputMax=wenus.inputMax;
      parser->inputTab=wenus.inputTab;
      parser->charset=wenus.charset;
      return;
  }
  parser->inputNr=0;
  parser->inputMax=5;
  parser->input=NULL;
  xmlPushInput(parser,input);
  xmlSwitchEncoding(parser,xmlDetectCharEncoding(parser->input->cur,4));
  if (input->filename == NULL)
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
  parser->input=wenus.input;
  parser->inputNr=wenus.inputNr;
  parser->inputMax=wenus.inputMax;
  parser->inputTab=wenus.inputTab;
  parser->charset=wenus.charset;
}

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

  register int
    i;

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
  xmlSubstituteEntitiesDefault(1);
  SAXHandler=(&SAXHandlerStruct);
  n=ReadBlob(image,4,buffer);
  if (n > 0)
    {
      svg_info.parser=xmlCreatePushParserCtxt(SAXHandler,&svg_info,buffer,n,
        image->filename);
      while ((n=ReadBlob(image,3,buffer)) > 0)
        xmlParseChunk(svg_info.parser,buffer,n,0);
    }
  n=xmlParseChunk(svg_info.parser,buffer,0,1);
  xmlFreeParserCtxt(svg_info.parser);
  xmlCleanupParser();
  (void) fclose(file);
  CloseBlob(image);
  DestroyImage(image);
  /*
    Draw image.
  */
  clone_info=CloneImageInfo(image_info);
  FormatString(geometry,"%ux%u",svg_info.width,svg_info.height);
  CloneString(&clone_info->size,geometry);
  FormatString(clone_info->filename,"mvg:%.1024s",filename);
  image=ReadImage(clone_info,exception);
  (void) remove(filename);
  DestroyImageInfo(clone_info);
  if (image != (Image *) NULL)
    {
      (void) strcpy(image->filename,image_info->filename);
      if (svg_info.comment != (char *) NULL)
        (void) SetImageAttribute(image,"Comment",svg_info.comment);
    }
  /*
    Free resources.
  */
  if (svg_info.comment != (char *) NULL)
    FreeMemory((void **) &svg_info.comment);
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
