/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                CCCC   OOO   N   N  JJJJJ  U   U  RRRR   EEEEE               %
%               C      O   O  NN  N    J    U   U  R   R  E                   %
%               C      O   O  N N N    J    U   U  RRRR   EEE                 %
%               C      O   O  N  NN  J J    U   U  R R    E                   %
%                CCCC   OOO   N   N  JJJ     UUU   R  R   EEEEE               %
%                                                                             %
%                     Interpret Magick Scripting Language.                    %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               December 2001                                 %
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
%  Conjure is an interpreter of the Magick Scripting Language.
%
%  The conjure program syntax is:
%
%  Usage: conjure [options ...] script.msl
%
%
*/

/*
  Include declarations.
*/
#include "magick/magick.h"
#include "magick/define.h"
#ifdef WIN32
#include <win32config.h>
#endif
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlerror.h>

/*
  Typedef declaractions.
*/
typedef struct _SVGInfo
{
  ExceptionInfo
    exception;

  long
    n;

  ImageInfo
    **image_info;

  Image
    **attributes,
    **image;

  unsigned int
    debug;

  xmlParserCtxtPtr
    parser;

  xmlDocPtr
    document;
} SVGInfo;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n j u r e                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

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
  (void) xmlAddDocEntity(svg_info->document,name,
    XML_EXTERNAL_GENERAL_UNPARSED_ENTITY,public_id,system_id,notation);

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
}

static void SVGStartElement(void *context,const xmlChar *name,
  const xmlChar **attributes)
{
  char
    key[MaxTextExtent],
    *value;

  const char
    *keyword;

  ExceptionInfo
    exception;

  long
    n,
    x,
    y;

  SVGInfo
    *svg_info;

  register int
    i;

  unsigned long
    height,
    width;

  /*
    Called when an opening tag has been processed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->debug)
    (void) fprintf(stdout,"  SAX.startElement(%.1024s",(char *) name);
  n=svg_info->n;
  switch (*name)
  {
    case 'G':
    case 'g':
    {
      if (LocaleCompare(name,"get") == 0)
        {
          if (svg_info->image[n] == (Image *) NULL)
            {
              ThrowException(&svg_info->exception,OptionWarning,
                "no images defined",name);
              break;
            }
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            CloneString(&value,attributes[i]);
            (void) strncpy(key,value,MaxTextExtent-1);
            switch (*keyword)
            {
              case 'H':
              case 'h':
              {
                if (LocaleCompare(keyword,"height") == 0)
                  {
                    FormatString(value,"%ld",svg_info->image[n]->rows);
                    (void) SetImageAttribute(svg_info->attributes[n],key,value);
                    break;
                  }
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
              }
              case 'W':
              case 'w':
              {
                if (LocaleCompare(keyword,"width") == 0)
                  {
                    FormatString(value,"%ld",svg_info->image[n]->columns);
                    (void) SetImageAttribute(svg_info->attributes[n],key,value);
                    break;
                  }
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
              }
              default:
              {
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }
          break;
        }
      ThrowException(&svg_info->exception,OptionError,
        "Unrecognized element",(const char *) name);
    }
    case 'I':
    case 'i':
    {
      if (LocaleCompare(name,"image") == 0)
        {
          const ImageAttribute
            *attribute;

          n++;
          svg_info->n=n;
          ReacquireMemory((void **) &svg_info->image_info,
            (n+1)*sizeof(ImageInfo *));
          ReacquireMemory((void **) &svg_info->attributes,
            (n+1)*sizeof(Image *));
          ReacquireMemory((void **) &svg_info->image,(n+1)*sizeof(Image *));
          if ((svg_info->image_info == (ImageInfo **) NULL) ||
              (svg_info->attributes == (Image **) NULL) ||
              (svg_info->image == (Image **) NULL))
            ThrowException(&svg_info->exception,ResourceLimitError,
              "Unable to allocate image","Memory allocation failed");
          svg_info->image_info[n]=CloneImageInfo(svg_info->image_info[n-1]);
          svg_info->attributes[n]=AllocateImage(svg_info->image_info[n]);
          svg_info->image[n]=(Image *) NULL;
          if ((svg_info->image_info[n] == (ImageInfo *) NULL) ||
              (svg_info->attributes[n] == (Image *) NULL))
            ThrowException(&svg_info->exception,ResourceLimitError,
              "Unable to allocate image","Memory allocation failed");
          attribute=GetImageAttribute(svg_info->attributes[n-1],(char *) NULL);
          while (attribute != (const ImageAttribute *) NULL)
          {
            (void) SetImageAttribute(svg_info->attributes[n],attribute->key,
              attribute->value);
            attribute=attribute->next;
          }
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            CloneString(&value,TranslateText(svg_info->image_info[n],
              svg_info->attributes[n],attributes[i]));
            switch (*keyword)
            {
              case 'S':
              case 's':
              {
                CloneString(&svg_info->image_info[n]->size,value);
                break;
              }
              default:
              {
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }
          break;
        }
      ThrowException(&svg_info->exception,OptionError,
        "Unrecognized element",(const char *) name);
    }
    case 'P':
    case 'p':
    {
      if (LocaleCompare(name,"print") == 0)
        {
          if (svg_info->image[n] == (Image *) NULL)
            {
              ThrowException(&svg_info->exception,OptionWarning,
                "no images defined",name);
              break;
            }
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            CloneString(&value,TranslateText(svg_info->image_info[n],
              svg_info->attributes[n],attributes[i]));
            switch (*keyword)
            {
              case 'O':
              case 'o':
              {
                (void) fprintf(stdout,"%s",value);
                break;
              }
              default:
              {
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }
          break;
        }
      ThrowException(&svg_info->exception,OptionError,
        "Unrecognized element",(const char *) name);
    }
    case 'R':
    case 'r':
    {
      if (LocaleCompare(name,"read") == 0)
        {
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            CloneString(&value,TranslateText(svg_info->image_info[n],
              svg_info->attributes[n],attributes[i]));
            switch (*keyword)
            {
              case 'F':
              case 'f':
              {
                if (LocaleCompare(keyword,"filename") == 0)
                  {
                    Image
                      *next_image;

                    (void) strcpy(svg_info->image_info[n]->filename,value);
                    next_image=ReadImage(svg_info->image_info[n],&exception);
                    if (exception.severity != UndefinedException)
                      MagickWarning(exception.severity,exception.reason,
                        exception.description);
                    if (next_image == (Image *) NULL)
                      continue;
                    if (svg_info->image[n] == (Image *) NULL)
                      svg_info->image[n]=next_image;
                    else
                      {
                        register Image
                          *p;

                        /*
                          Link image into image list.
                        */
                        p=svg_info->image[n];
                        for ( ; p->next != (Image *) NULL; p=p->next);
                        next_image->previous=p;
                        p->next=next_image;
                      }
                    break;
                  }
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
              case 'S':
              case 's':
              {
                if (LocaleCompare(keyword,"size") == 0)
                  {
                    svg_info->image_info[n]->size=AllocateString(value);
                    break;
                  }
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
              default:
              {
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }
          break;
        }
      if (LocaleCompare(name,"resize") == 0)
        {
          if (svg_info->image[n] == (Image *) NULL)
            {
              ThrowException(&svg_info->exception,OptionWarning,
                "no images defined",name);
              break;
            }
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            CloneString(&value,TranslateText(svg_info->image_info[n],
              svg_info->attributes[n],attributes[i]));
            switch (*keyword)
            {
              case 'G':
              case 'g':
              {
                if (LocaleCompare(keyword,"geometry") == 0)
                  {
                    Image
                      *resize_image;

                    /*
                      Resize image.
                    */
                    width=svg_info->image[n]->columns;
                    height=svg_info->image[n]->rows;
                    x=0;
                    y=0;
                    (void) ParseImageGeometry(value,&x,&y,&width,&height);
                    if ((width == svg_info->image[n]->columns) &&
                        (height == svg_info->image[n]->rows))
                      break;
                    resize_image=ZoomImage(svg_info->image[n],width,height,
                      &svg_info->image[n]->exception);
                    if (resize_image == (Image *) NULL)
                      break;
                    DestroyImage(svg_info->image[n]);
                    svg_info->image[n]=resize_image;
                    break;
                  }
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
              default:
              {
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }
          break;
        }
      ThrowException(&svg_info->exception,OptionError,
        "Unrecognized element",(const char *) name);
    }
    case 'W':
    case 'w':
    {
      if (LocaleCompare(name,"write") == 0)
        {
          if (svg_info->image[n] == (Image *) NULL)
            {
              ThrowException(&svg_info->exception,OptionWarning,
                "no images defined",name);
              break;
            }
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            CloneString(&value,TranslateText(svg_info->image_info[n],
              svg_info->attributes[n],attributes[i]));
            switch (*keyword)
            {
              case 'F':
              case 'f':
              {
                if (LocaleCompare(keyword,"filename") == 0)
                  {
                    (void) strncpy(svg_info->image[n]->filename,value,
                      MaxTextExtent);
                    (void) WriteImage(svg_info->image_info[n],
                      svg_info->image[n]);
                    break;
                  }
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
              }
              default:
              {
                ThrowException(&svg_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }
          break;
        }
      ThrowException(&svg_info->exception,OptionError,
        "Unrecognized element",(const char *) name);
    }
    default:
    {
      ThrowException(&svg_info->exception,OptionError,"Unrecognized element",
        (const char *) name);
      break;
    }
  }
  LiberateMemory((void **) &value);
  if (svg_info->debug)
    (void) fprintf(stdout,"  )\n");
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
  switch (*name)
  {
    case 'I':
    case 'i':
    {
      if (svg_info->image[svg_info->n] != (Image *) NULL)
        DestroyImage(svg_info->image[svg_info->n]);
      DestroyImage(svg_info->attributes[svg_info->n]);
      DestroyImageInfo(svg_info->image_info[svg_info->n]);
      svg_info->n--;
    }
    default:
      break;
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
  if (svg_info->debug)
    {
      (void) fprintf(stdout,"  SAX.characters(");
      for (i=0; (i < length) && (i < 30); i++)
        (void) fprintf(stdout,"  %c",c[i]);
      (void) fprintf(stdout,"  , %d)\n",length);
    }
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
    (void) xmlAddChild(parser->node,xmlNewCharRef(svg_info->document,name));
  else
    (void) xmlAddChild(parser->node,xmlNewReference(svg_info->document,name));
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
      (void) vfprintf(stdout,format,operands);
    }
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  ThrowException(&svg_info->exception,DelegateWarning,reason,(char *) NULL);
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
      (void) vfprintf(stdout,format,operands);
    }
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  ThrowException(&svg_info->exception,DelegateError,reason,(char *) NULL);
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
  (void) xmlAddChild(parser->node,xmlNewCDataBlock(parser->myDoc,value,length));
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
  (void) xmlNewDtd(svg_info->document,name,external_id,system_id);
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
  (void) xmlSwitchEncoding(parser,xmlDetectCharEncoding(parser->input->cur,4));
  if (input->filename == (char *) NULL)
    input->filename=(char *) xmlStrdup(system_id);
  input->line=1;
  input->col=1;
  input->base=parser->input->cur;
  input->cur=parser->input->cur;
  input->free=NULL;
  xmlParseExternalSubset(parser,external_id,system_id);
  while (parser->inputNr > 1)
    (void) xmlPopInput(parser);
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

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U s a g e                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure Usage displays the program usage;
%
%  The format of the Usage method is:
%
%      void Usage()
%
*/
static void Usage(void)
{
  static const char
    *options[]=
    {
      "-adjoin              join images into a single multi-image file",
      (char *) NULL
    };

  const char
    **p;

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf("Usage: %.1024s [options ...] file [ files... ] output_file\n",
    SetClientName((char *) NULL));
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  Exit(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a i n                                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
int main(int argc,char **argv)
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
    message[MaxTextExtent];

  long
    n;

  register int
    i;

  SVGInfo
    svg_info;

  unsigned int
    status;

  xmlSAXHandlerPtr
    SAXHandler;

  /*
    Initialize command line arguments.
  */
  ReadCommandlLine(argc,&argv);
  InitializeMagick(*argv);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickError(ResourceLimitError,"Memory allocation failed",(char *) NULL);
  if (argc < 2)
    Usage();
  /*
    Parse command-line arguments.
  */
  (void) memset(&svg_info,0,sizeof(SVGInfo));
  GetExceptionInfo(&svg_info.exception);
  svg_info.image_info=(ImageInfo **) AcquireMemory(sizeof(ImageInfo *));
  svg_info.image=(Image **) AcquireMemory(sizeof(Image *));
  svg_info.attributes=(Image **) AcquireMemory(sizeof(Image *));
  if ((svg_info.image_info == (ImageInfo **) NULL) ||
      (svg_info.image == (Image **) NULL) ||
      (svg_info.attributes == (Image **) NULL))
    MagickError(ResourceLimitError,"Unable to allocate image",
      "Memory allocation failed");
  svg_info.image_info[0]=CloneImageInfo((ImageInfo *) NULL);
  svg_info.attributes[0]=AllocateImage(svg_info.image_info[0]);
  svg_info.image[0]=AllocateImage(svg_info.image_info[0]);
  if ((svg_info.image_info[0] == (ImageInfo *) NULL) ||
      (svg_info.attributes[0] == (Image *) NULL) ||
      (svg_info.image[0] == (Image *) NULL))
    MagickError(ResourceLimitError,"Unable to allocate image",
      "Memory allocation failed");
  for (i=1; i < (argc-1); i+=2)
  {
    status=SetImageAttribute(svg_info.attributes[0],argv[i]+1,argv[i+1]);
    if (status == False)
      MagickError(OptionError,"Unable to persist key/value pair",argv[i]);
  }
  if (i != (argc-1))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  (void) strcpy(svg_info.image[0]->filename,argv[argc-1]);
  /*
    Open image file.
  */
  status=OpenBlob(*svg_info.image_info,*svg_info.image,ReadBinaryType,
    &svg_info.exception);
  if (status == False)
    MagickError(FileOpenWarning,"Unable to open file",
      svg_info.image[0]->filename);
  /*
    Parse SVG file.
  */
  (void) xmlSubstituteEntitiesDefault(1);
  SAXHandler=(&SAXModules);
  svg_info.parser=xmlCreatePushParserCtxt(SAXHandler,&svg_info,(char *) NULL,0,
    svg_info.image[0]->filename);
  while (ReadBlobString(svg_info.image[0],message) != (char *) NULL)
  {
    n=(long) strlen(message);
    if (n == 0)
      continue;
    status=xmlParseChunk(svg_info.parser,message,(int) n,False);
    if (status != 0)
      break;
    (void) xmlParseChunk(svg_info.parser," ",1,False);
    if (svg_info.exception.severity != UndefinedException)
      {
        MagickError(svg_info.exception.severity,svg_info.exception.reason,
          svg_info.exception.description);
        break;
      }
  }
  (void) xmlParseChunk(svg_info.parser," ",1,True);
  xmlFreeParserCtxt(svg_info.parser);
  if (svg_info.debug)
    (void) fprintf(stdout,"end SAX\n");
  xmlCleanupParser();
  DestroyImage(*svg_info.image);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(0);
  return(False);
}
