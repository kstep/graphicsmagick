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
%  Usage: conjure [options ...] script.msl [imagefiles...]
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
typedef struct _MSLGroupInfo
{
	unsigned long
		numImages;	/* how many images are in this group */
} MSLGroupInfo;

typedef struct _MSLInfo
{
  ExceptionInfo
    exception;

  long
    n,
	nGroups;

  ImageInfo
    **image_info;

  Image
    **attributes,
    **image;

  MSLGroupInfo
	  *group_info;

  unsigned int
    debug;

  xmlParserCtxtPtr
    parser;

  xmlDocPtr
    document;
} MSLInfo;

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

static int MSLIsStandalone(void *context)
{
  MSLInfo
    *msl_info;

  /*
    Is this document tagged standalone?
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.MSLIsStandalone()\n");
  return(msl_info->document->standalone == 1);
}

static int MSLHasInternalSubset(void *context)
{
  MSLInfo
    *msl_info;

  /*
    Does this document has an internal subset?
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.MSLHasInternalSubset()\n");
  return(msl_info->document->intSubset != NULL);
}

static int MSLHasExternalSubset(void *context)
{
  MSLInfo
    *msl_info;

  /*
    Does this document has an external subset?
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.MSLHasExternalSubset()\n");
  return(msl_info->document->extSubset != NULL);
}

static void MSLInternalSubset(void *context,const xmlChar *name,
  const xmlChar *external_id,const xmlChar *system_id)
{
  MSLInfo
    *msl_info;

  /*
    Does this document has an internal subset?
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    {
      (void) fprintf(stdout,"  SAX.internalSubset(%.1024s",name);
      if (external_id != NULL)
        (void) fprintf(stdout,"  , %.1024s",external_id);
      if (system_id != NULL)
        (void) fprintf(stdout,"  , %.1024s",system_id);
      (void) fprintf(stdout,"  \n");
    }
  (void) xmlCreateIntSubset(msl_info->document,name,external_id,system_id);
}

static xmlParserInputPtr MSLResolveEntity(void *context,
  const xmlChar *public_id,const xmlChar *system_id)
{
  MSLInfo
    *msl_info;

  xmlParserInputPtr
    stream;

  /*
    Special entity resolver, better left to the parser, it has more
    context than the application layer.  The default behaviour is to
    not resolve the entities, in that case the ENTITY_REF nodes are
    built in the structure (and the parameter values).
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
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
    public_id,msl_info->parser);
  return(stream);
}

static xmlEntityPtr MSLGetEntity(void *context,const xmlChar *name)
{
  MSLInfo
    *msl_info;

  /*
    Get an entity by name.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.MSLGetEntity(%.1024s)\n",name);
  return(xmlGetDocEntity(msl_info->document,name));
}

static xmlEntityPtr MSLGetParameterEntity(void *context,const xmlChar *name)
{
  MSLInfo
    *msl_info;

  /*
    Get a parameter entity by name.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.getParameterEntity(%.1024s)\n",name);
  return(xmlGetParameterEntity(msl_info->document,name));
}

static void MSLEntityDeclaration(void *context,const xmlChar *name,int type,
  const xmlChar *public_id,const xmlChar *system_id,xmlChar *content)
{
  MSLInfo
    *msl_info;

  /*
    An entity definition has been parsed.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,
      "  SAX.entityDecl(%.1024s, %d, %.1024s, %.1024s, %.1024s)\n",name,type,
      public_id ? (char *) public_id : "none",
      system_id ? (char *) system_id : "none",content);
  if (msl_info->parser->inSubset == 1)
    (void) xmlAddDocEntity(msl_info->document,name,type,public_id,system_id,
      content);
  else
    if (msl_info->parser->inSubset == 2)
      (void) xmlAddDtdEntity(msl_info->document,name,type,public_id,system_id,
        content);
}

static void MSLAttributeDeclaration(void *context,const xmlChar *element,
  const xmlChar *name,int type,int value,const xmlChar *default_value,
  xmlEnumerationPtr tree)
{
  MSLInfo
    *msl_info;

  xmlChar
    *fullname,
    *prefix;

  xmlParserCtxtPtr
    parser;

  /*
    An attribute definition has been parsed.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,
      "  SAX.attributeDecl(%.1024s, %.1024s, %d, %d, %.1024s, ...)\n",element,
      name,type,value,default_value);
  fullname=(xmlChar *) NULL;
  prefix=(xmlChar *) NULL;
  parser=msl_info->parser;
  fullname=(xmlChar *) xmlSplitQName(parser,name,&prefix);
  if (parser->inSubset == 1)
    (void) xmlAddAttributeDecl(&parser->vctxt,msl_info->document->intSubset,
      element,fullname,prefix,(xmlAttributeType) type,
      (xmlAttributeDefault) value,default_value,tree);
  else
    if (parser->inSubset == 2)
      (void) xmlAddAttributeDecl(&parser->vctxt,msl_info->document->extSubset,
        element,fullname,prefix,(xmlAttributeType) type,
        (xmlAttributeDefault) value,default_value,tree);
  if (prefix != (xmlChar *) NULL)
    xmlFree(prefix);
  if (fullname != (xmlChar *) NULL)
    xmlFree(fullname);
}

static void MSLElementDeclaration(void *context,const xmlChar *name,int type,
  xmlElementContentPtr content)
{
  MSLInfo
    *msl_info;

  xmlParserCtxtPtr
    parser;

  /*
    An element definition has been parsed.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.elementDecl(%.1024s, %d, ...)\n",name,type);
  parser=msl_info->parser;
  if (parser->inSubset == 1)
    (void) xmlAddElementDecl(&parser->vctxt,msl_info->document->intSubset,
      name,(xmlElementTypeVal) type,content);
  else
    if (parser->inSubset == 2)
      (void) xmlAddElementDecl(&parser->vctxt,msl_info->document->extSubset,
        name,(xmlElementTypeVal) type,content);
}

static void MSLNotationDeclaration(void *context,const xmlChar *name,
  const xmlChar *public_id,const xmlChar *system_id)
{
  MSLInfo
    *msl_info;

  xmlParserCtxtPtr
    parser;

  /*
    What to do when a notation declaration has been parsed.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,
      "  SAX.notationDecl(%.1024s, %.1024s, %.1024s)\n",(char *) name,
      public_id ? (char *) public_id : "none",
      system_id ? (char *) system_id : "none");
  parser=msl_info->parser;
  if (parser->inSubset == 1)
    (void) xmlAddNotationDecl(&parser->vctxt,msl_info->document->intSubset,
      name,public_id,system_id);
  else
    if (parser->inSubset == 2)
      (void) xmlAddNotationDecl(&parser->vctxt,msl_info->document->intSubset,
        name,public_id,system_id);
}

static void MSLUnparsedEntityDeclaration(void *context,const xmlChar *name,
  const xmlChar *public_id,const xmlChar *system_id,const xmlChar *notation)
{
  MSLInfo
    *msl_info;

  /*
    What to do when an unparsed entity declaration is parsed.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,
      "  SAX.unparsedEntityDecl(%.1024s, %.1024s, %.1024s, %.1024s)\n",
      (char *) name,public_id ? (char *) public_id : "none",
      system_id ? (char *) system_id : "none",(char *) notation);
  (void) xmlAddDocEntity(msl_info->document,name,
    XML_EXTERNAL_GENERAL_UNPARSED_ENTITY,public_id,system_id,notation);

}

static void MSLSetDocumentLocator(void *context,xmlSAXLocatorPtr location)
{
  MSLInfo
    *msl_info;

  /*
    Receive the document locator at startup, actually xmlDefaultSAXLocator.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.setDocumentLocator()\n");
}

static void MSLStartDocument(void *context)
{
  MSLInfo
    *msl_info;

  xmlParserCtxtPtr
    parser;

  /*
    Called when the document start being processed.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.startDocument()\n");
  parser=msl_info->parser;
  msl_info->document=xmlNewDoc(parser->version);
  if (msl_info->document == (xmlDocPtr) NULL)
    return;
  if (parser->encoding == NULL)
    msl_info->document->encoding=NULL;
  else
    msl_info->document->encoding=xmlStrdup(parser->encoding);
  msl_info->document->standalone=parser->standalone;
}

static void MSLEndDocument(void *context)
{
  MSLInfo
    *msl_info;

  /*
    Called when the document end has been detected.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.endDocument()\n");
}

static void MSLStartElement(void *context,const xmlChar *name,
  const xmlChar **attributes)
{
  char
    key[MaxTextExtent],
    *value = NULL;

  const char
    *keyword;

  ExceptionInfo
    exception;

  long
    n,
    x,
    y;

  MSLInfo
    *msl_info;

  register int
    i, j;

  unsigned long
    height,
    width;

  /*
    Called when an opening tag has been processed.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.startElement(%.1024s",(char *) name);
  n=msl_info->n;
  switch (*name)
  {
    case 'B':
    case 'b':
		{
			if (LocaleCompare(name, "blur") == 0)
			{
				double	radius = 0.0,
						sigma = 1.0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  /*
				NOTE: blur can have no attributes, since we use all the defaults!
			  */
			  if (attributes != (const xmlChar **) NULL)
			  {
				  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
				  {
					keyword=(const char *) attributes[i++];
					CloneString(&value,TranslateText(msl_info->image_info[n],
					  msl_info->attributes[n],attributes[i]));
					switch (*keyword)
					{
					  case 'R':
					  case 'r':
						{
							if (LocaleCompare(keyword, "radius") == 0)
							{
							  radius = atof( value );
							  break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
						}
					  case 'S':
					  case 's':
					  {
							if (LocaleCompare(keyword,"sigma") == 0)
							{
								sigma = atoi( value );
								break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
					  }
					  default:
					  {
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
					  }
					}
				  }
				}

				/*
				  blur image.
				*/
			  {
				Image
				  *newImage;

				newImage=BlurImage(msl_info->image[n],radius,sigma,&msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }
			}
		  else if (LocaleCompare(name,"border") == 0)
			{
			  /* init the values */
				width = height = 6;	/* this is the value that Magick++ uses */
				x = y = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  /*
				NOTE: border can have no attributes, since we use all the defaults!
			  */
			  if (attributes != (const xmlChar **) NULL)
			  {
				  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
				  {
					keyword=(const char *) attributes[i++];
					CloneString(&value,TranslateText(msl_info->image_info[n],
					  msl_info->attributes[n],attributes[i]));
					switch (*keyword)
					{
					  case 'F':
					  case'f':
						{
							if (LocaleCompare(keyword, "fill") == 0)
							{
							  (void) QueryColorDatabase(value,
										&msl_info->image[n]->border_color);
							  break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
						}
					  case 'G':
					  case 'g':
					  {
						if (LocaleCompare(keyword,"geometry") == 0)
						  {
							(void) ParseImageGeometry(value,&x,&y,&width,&height);
							break;
						  }
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					  case 'H':
					  case 'h':
					  {
						if (LocaleCompare(keyword,"height") == 0)
						  {
							height = atoi( value );
							break;
						  }
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					  case 'W':
					  case 'w':
					  {
						if (LocaleCompare(keyword,"width") == 0)
						  {
							width = atoi( value );
							break;
						  }
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					  default:
					  {
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					}
				  }
				}

				/*
				  border image.
				*/
			  {
				Image
				  *border_image;
				RectangleInfo
					rectInfo;

				rectInfo.height = height;
				rectInfo.width = width;
				rectInfo.x = x;
				rectInfo.y = y;

				border_image=BorderImage(msl_info->image[n],&rectInfo, &msl_info->image[n]->exception);
				if (border_image == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=border_image;
				break;
			  }

			  break;
			}
		  ThrowException(&msl_info->exception,OptionError,
			"Unrecognized element",(const char *) name);
		}
	case 'C':
	case 'c':
		{
			if (LocaleCompare(name, "charcoal") == 0)
			{
				double	radius = 0.0,
						sigma = 1.0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  /*
				NOTE: charcoal can have no attributes, since we use all the defaults!
			  */
			  if (attributes != (const xmlChar **) NULL)
			  {
				  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
				  {
					keyword=(const char *) attributes[i++];
					CloneString(&value,TranslateText(msl_info->image_info[n],
					  msl_info->attributes[n],attributes[i]));
					switch (*keyword)
					{
					  case 'R':
					  case 'r':
						{
							if (LocaleCompare(keyword, "radius") == 0)
							{
							  radius = atof( value );
							  break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
						}
					  case 'S':
					  case 's':
					  {
							if (LocaleCompare(keyword,"sigma") == 0)
							{
								sigma = atoi( value );
								break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
					  }
					  default:
					  {
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
					  }
					}
				  }
				}

				/*
				  charcoal image.
				*/
			  {
				Image
				  *newImage;

				newImage=CharcoalImage(msl_info->image[n],radius,sigma,&msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }
			}
		  else if (LocaleCompare(name,"chop") == 0)
			{
			  /* init the values */
				width=msl_info->image[n]->columns;
				height=msl_info->image[n]->rows;
				x = y = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'G':
				  case 'g':
				  {
					if (LocaleCompare(keyword,"geometry") == 0)
					  {
						(void) ParseImageGeometry(value,&x,&y,&width,&height);
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'H':
				  case 'h':
				  {
					if (LocaleCompare(keyword,"height") == 0)
					  {
						height = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'W':
				  case 'w':
				  {
					if (LocaleCompare(keyword,"width") == 0)
					  {
						width = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'X':
				  case 'x':
				  {
					if (LocaleCompare(keyword,"x") == 0)
					  {
						x = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'Y':
				  case 'y':
				  {
					if (LocaleCompare(keyword,"y") == 0)
					  {
						y = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				Image
				  *newImage;
				RectangleInfo
					rectInfo;

				rectInfo.height = height;
				rectInfo.width = width;
				rectInfo.x = x;
				rectInfo.y = y;

				newImage=ChopImage(msl_info->image[n],&rectInfo, &msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }

			  break;
			}
			else if (LocaleCompare(name, "composite") == 0 )
			{
				Image *
					srcImage = (Image*)NULL;
				CompositeOperator
					compositeOp = OverCompositeOp;
				x = y = 0;

				if (msl_info->image[n] == (Image *) NULL)
				{
					ThrowException(&msl_info->exception,OptionWarning,
									"no images defined",name);
					break;
				}
				if (attributes == (const xmlChar **) NULL)
					break;
				for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
				{
					keyword=(const char *) attributes[i++];
					CloneString(&value,TranslateText(msl_info->image_info[n],
									msl_info->attributes[n],attributes[i]));
					switch (*keyword)
					{
						case 'I':
						case 'i':
						{
							if (LocaleCompare(keyword,"image") == 0)
							{
								for (j=0; j<msl_info->n;j++)
								{
									ImageAttribute *
										theAttr = GetImageAttribute(msl_info->attributes[j], "id");
									if (theAttr && LocaleCompare(theAttr->value, value) == 0)
									{
										srcImage = msl_info->image[j];
										break;
									}
								}
								break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
											"Unrecognized attribute",keyword);
							break;
						}
						case 'X':
						case 'x':
						{
							if (LocaleCompare(keyword,"x") == 0)
							{
								x = atoi( value );
								break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
											"Unrecognized attribute",keyword);
							break;
						}
						case 'Y':
						case 'y':
						{
							if (LocaleCompare(keyword,"y") == 0)
							{
								y = atoi( value );
								break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
											"Unrecognized attribute",keyword);
							break;
						}
						default:
						{
							ThrowException(&msl_info->exception,OptionWarning,
											"Unrecognized attribute",keyword);
							break;
						}
					}

					/*
						process image.
					*/
					if (srcImage != (Image*)NULL)
					{
						unsigned int
								result;
						
						result = CompositeImage(msl_info->image[n], compositeOp, srcImage, x, y);
						break;
					} else 
						ThrowException(&msl_info->exception,OptionWarning,
										"no composite image defined",name);

				}
				break;
			}
		  else if (LocaleCompare(name,"crop") == 0)
			{
			  /* init the values */
				width=msl_info->image[n]->columns;
				height=msl_info->image[n]->rows;
				x = y = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'G':
				  case 'g':
				  {
					if (LocaleCompare(keyword,"geometry") == 0)
					  {
						(void) ParseImageGeometry(value,&x,&y,&width,&height);
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'H':
				  case 'h':
				  {
					if (LocaleCompare(keyword,"height") == 0)
					  {
						height = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'W':
				  case 'w':
				  {
					if (LocaleCompare(keyword,"width") == 0)
					  {
						width = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'X':
				  case 'x':
				  {
					if (LocaleCompare(keyword,"x") == 0)
					  {
						x = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'Y':
				  case 'y':
				  {
					if (LocaleCompare(keyword,"y") == 0)
					  {
						y = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				Image
				  *newImage;
				RectangleInfo
					rectInfo;

				rectInfo.height = height;
				rectInfo.width = width;
				rectInfo.x = x;
				rectInfo.y = y;

				newImage=CropImage(msl_info->image[n],&rectInfo, &msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }

			  break;
			}
		  ThrowException(&msl_info->exception,OptionError,
			"Unrecognized element",(const char *) name);
		}
	case 'D':
	case 'd':
		{
			if (LocaleCompare(name, "despeckle") == 0)
			{
				if (msl_info->image[n] == (Image *) NULL)
				{
					ThrowException(&msl_info->exception,OptionWarning,
									"no images defined",name);
					break;
				}
				
				/* no attributes here */

				/* process the image */
				{
					Image
					  *newImage;

					newImage=DespeckleImage(msl_info->image[n],&msl_info->image[n]->exception);
					if (newImage == (Image *) NULL)
					  break;
					DestroyImage(msl_info->image[n]);
					msl_info->image[n]=newImage;
					break;
				}
			}
			ThrowException(&msl_info->exception,OptionError,
							"Unrecognized element",(const char *) name);
		}
	case 'E':
	case 'e':
		{
			if (LocaleCompare(name, "emboss") == 0)
			{
				double	radius = 0.0,
						sigma = 1.0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  /*
				NOTE: emboss can have no attributes, since we use all the defaults!
			  */
			  if (attributes != (const xmlChar **) NULL)
			  {
				  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
				  {
					keyword=(const char *) attributes[i++];
					CloneString(&value,TranslateText(msl_info->image_info[n],
					  msl_info->attributes[n],attributes[i]));
					switch (*keyword)
					{
					  case 'R':
					  case 'r':
						{
							if (LocaleCompare(keyword, "radius") == 0)
							{
							  radius = atof( value );
							  break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
						}
					  case 'S':
					  case 's':
					  {
							if (LocaleCompare(keyword,"sigma") == 0)
							{
								sigma = atoi( value );
								break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
					  }
					  default:
					  {
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
					  }
					}
				  }
				}

				/*
				  emboss image.
				*/
			  {
				Image
				  *newImage;

				newImage=EmbossImage(msl_info->image[n],radius,sigma,&msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }
			}
			else if (LocaleCompare(name, "enhance") == 0)
			{
				if (msl_info->image[n] == (Image *) NULL)
				{
					ThrowException(&msl_info->exception,OptionWarning,
									"no images defined",name);
					break;
				}
				
				/* no attributes here */

				/* process the image */
				{
					Image
					  *newImage;

					newImage=EnhanceImage(msl_info->image[n],&msl_info->image[n]->exception);
					if (newImage == (Image *) NULL)
					  break;
					DestroyImage(msl_info->image[n]);
					msl_info->image[n]=newImage;
					break;
				}
			}
			else if (LocaleCompare(name, "equalize") == 0)
			{
				if (msl_info->image[n] == (Image *) NULL)
				{
					ThrowException(&msl_info->exception,OptionWarning,
									"no images defined",name);
					break;
				}
				
				/* no attributes here */

				/* process the image */
				{
					EqualizeImage(msl_info->image[n]);	/* why doesn't this take an exception or return the changed image */
					break;
				}
			}
			ThrowException(&msl_info->exception,OptionError,
							"Unrecognized element",(const char *) name);
		}
	case 'F':
	case 'f':
		{
			if (LocaleCompare(name, "flip") == 0)
			{
				if (msl_info->image[n] == (Image *) NULL)
				{
					ThrowException(&msl_info->exception,OptionWarning,
									"no images defined",name);
					break;
				}
				
				/* no attributes here */

				/* process the image */
				{
					Image
					  *newImage;

					newImage=FlipImage(msl_info->image[n],&msl_info->image[n]->exception);
					if (newImage == (Image *) NULL)
					  break;
					DestroyImage(msl_info->image[n]);
					msl_info->image[n]=newImage;
					break;
				}
			}
			else if (LocaleCompare(name, "flop") == 0)
			{
				if (msl_info->image[n] == (Image *) NULL)
				{
					ThrowException(&msl_info->exception,OptionWarning,
									"no images defined",name);
					break;
				}
				
				/* no attributes here */

				/* process the image */
				{
					Image
					  *newImage;

					newImage=FlopImage(msl_info->image[n],&msl_info->image[n]->exception);
					if (newImage == (Image *) NULL)
					  break;
					DestroyImage(msl_info->image[n]);
					msl_info->image[n]=newImage;
					break;
				}
			}
			else if (LocaleCompare(name,"frame") == 0)
			{
			  /* init the values */
				width = height = 25;	/* these are the values that Magick++ uses */
				x = y = 6;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  /*
				NOTE: frame can have no attributes, since we use all the defaults!
			  */
			  if (attributes != (const xmlChar **) NULL)
			  {
				  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
				  {
					keyword=(const char *) attributes[i++];
					CloneString(&value,TranslateText(msl_info->image_info[n],
					  msl_info->attributes[n],attributes[i]));
					switch (*keyword)
					{
					  case 'F':
					  case 'f':
						{
							if (LocaleCompare(keyword, "fill") == 0)
							{
							  (void) QueryColorDatabase(value,
										&msl_info->image[n]->border_color);
							  break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
						}
					  case 'G':
					  case 'g':
					  {
						if (LocaleCompare(keyword,"geometry") == 0)
						  {
							(void) ParseImageGeometry(value,&x,&y,&width,&height);
							break;
						  }
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					  case 'H':
					  case 'h':
					  {
						if (LocaleCompare(keyword,"height") == 0)
						  {
							height = atoi( value );
							break;
						  }
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					  case 'I':
					  case 'i':
					  {
						if (LocaleCompare(keyword,"inner") == 0)
						  {
							y = atoi( value );
							break;
						  }
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					  case 'O':
					  case 'o':
					  {
						if (LocaleCompare(keyword,"outer") == 0)
						  {
							x = atoi( value );
							break;
						  }
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					  case 'W':
					  case 'w':
					  {
						if (LocaleCompare(keyword,"width") == 0)
						  {
							width = atoi( value );
							break;
						  }
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					  case 'X':
					  case 'x':
					  {
						if (LocaleCompare(keyword,"x") == 0)
						  {
							x = atoi( value );
							break;
						  }
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					  case 'Y':
					  case 'y':
					  {
						if (LocaleCompare(keyword,"y") == 0)
						  {
							y = atoi( value );
							break;
						  }
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					  default:
					  {
						ThrowException(&msl_info->exception,OptionWarning,
						  "Unrecognized attribute",keyword);
						break;
					  }
					}
				  }
				}

				/*
				  frame image.
				*/
			  {
				Image
				  *newImage;
				FrameInfo
					frameInfo;

				frameInfo.x = width;
				frameInfo.y = height;
				frameInfo.width = msl_info->image[n]->columns + ( frameInfo.x << 1 );
				frameInfo.height = msl_info->image[n]->rows + ( frameInfo.y << 1 );
				frameInfo.outer_bevel = x;
				frameInfo.inner_bevel = y;

				newImage=FrameImage(msl_info->image[n],&frameInfo, &msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }

			  break;
			}
			ThrowException(&msl_info->exception,OptionError,
							"Unrecognized element",(const char *) name);
		}
    case 'G':
    case 'g':
    {
      if (LocaleCompare(name,"get") == 0)
        {
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowException(&msl_info->exception,OptionWarning,
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
                    FormatString(value,"%ld",msl_info->image[n]->rows);
                    (void) SetImageAttribute(msl_info->attributes[n],key,value);
                    break;
                  }
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
              }
              case 'W':
              case 'w':
              {
                if (LocaleCompare(keyword,"width") == 0)
                  {
                    FormatString(value,"%ld",msl_info->image[n]->columns);
                    (void) SetImageAttribute(msl_info->attributes[n],key,value);
                    break;
                  }
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
              }
              default:
              {
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }
          break;
        }
	  else if (LocaleCompare(name, "group") == 0) 
	  {
			msl_info->nGroups++;
			ReacquireMemory((void**) &msl_info->group_info, (msl_info->nGroups+1)*sizeof(MSLGroupInfo));
			break;
	  }
      ThrowException(&msl_info->exception,OptionError,
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
          msl_info->n=n;
          ReacquireMemory((void **) &msl_info->image_info,
            (n+1)*sizeof(ImageInfo *));
          ReacquireMemory((void **) &msl_info->attributes,
            (n+1)*sizeof(Image *));
          ReacquireMemory((void **) &msl_info->image,(n+1)*sizeof(Image *));
          if ((msl_info->image_info == (ImageInfo **) NULL) ||
              (msl_info->attributes == (Image **) NULL) ||
              (msl_info->image == (Image **) NULL))
            ThrowException(&msl_info->exception,ResourceLimitError,
              "Unable to allocate image","Memory allocation failed");
          msl_info->image_info[n]=CloneImageInfo(msl_info->image_info[n-1]);
          msl_info->attributes[n]=AllocateImage(msl_info->image_info[n]);
          msl_info->image[n]=(Image *) NULL;
          if ((msl_info->image_info[n] == (ImageInfo *) NULL) ||
              (msl_info->attributes[n] == (Image *) NULL))
            ThrowException(&msl_info->exception,ResourceLimitError,
              "Unable to allocate image","Memory allocation failed");
		  msl_info->group_info[msl_info->nGroups-1].numImages++;
          attribute=GetImageAttribute(msl_info->attributes[n-1],(char *) NULL);
          while (attribute != (const ImageAttribute *) NULL)
          {
            (void) SetImageAttribute(msl_info->attributes[n],attribute->key,
              attribute->value);
            attribute=attribute->next;
          }
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            CloneString(&value,TranslateText(msl_info->image_info[n],
              msl_info->attributes[n],attributes[i]));
            switch (*keyword)
            {
			case 'B':
			case 'b':
				{
					if (LocaleCompare(keyword,"background") == 0)
					{
						(void) QueryColorDatabase(value,
											&msl_info->image_info[n]->background_color);
						break;
					}
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				}

			case 'C':
			case 'c':
				{
					if (LocaleCompare(keyword,"color") == 0)
					{
						Image
						  *next_image;

						(void) strcpy(msl_info->image_info[n]->filename,"xc:");
						(void) strcat(msl_info->image_info[n]->filename,value);
						next_image=ReadImage(msl_info->image_info[n],&exception);
						if (exception.severity != UndefinedException)
						  MagickWarning(exception.severity,exception.reason,
							exception.description);
						if (next_image == (Image *) NULL)
						  continue;
						if (msl_info->image[n] == (Image *) NULL)
						  msl_info->image[n]=next_image;
						else
						  {
							register Image
							  *p;

							/*
							  Link image into image list.
							*/
							p=msl_info->image[n];
							for ( ; p->next != (Image *) NULL; p=p->next);
							next_image->previous=p;
							p->next=next_image;
						  }
						break;
					}
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				}

			case 'I':
			case 'i':
				{
					if (LocaleCompare(keyword,"id") == 0)
					{
						(void) SetImageAttribute(msl_info->attributes[n],keyword,NULL);	/* make sure to clear it! */
						(void) SetImageAttribute(msl_info->attributes[n],keyword,value);
						break;
					}
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				}
              case 'S':
              case 's':
              {
                if (LocaleCompare(keyword,"size") == 0)
                {
					CloneString(&msl_info->image_info[n]->size,value);
					break;
				}
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
              default:
              {
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }
          break;
        }
      ThrowException(&msl_info->exception,OptionError,
        "Unrecognized element",(const char *) name);
    }
	case 'M':
	case 'm':
		{
			if (LocaleCompare(name, "magnify") == 0)
			{
				if (msl_info->image[n] == (Image *) NULL)
				{
					ThrowException(&msl_info->exception,OptionWarning,
									"no images defined",name);
					break;
				}
				
				/* no attributes here */

				/* process the image */
				{
					Image
					  *newImage;

					newImage=MagnifyImage(msl_info->image[n],&msl_info->image[n]->exception);
					if (newImage == (Image *) NULL)
					  break;
					DestroyImage(msl_info->image[n]);
					msl_info->image[n]=newImage;
					break;
				}
			}
			else if (LocaleCompare(name, "minify") == 0)
			{
				if (msl_info->image[n] == (Image *) NULL)
				{
					ThrowException(&msl_info->exception,OptionWarning,
									"no images defined",name);
					break;
				}
				
				/* no attributes here */

				/* process the image */
				{
					Image
					  *newImage;

					newImage=MinifyImage(msl_info->image[n],&msl_info->image[n]->exception);
					if (newImage == (Image *) NULL)
					  break;
					DestroyImage(msl_info->image[n]);
					msl_info->image[n]=newImage;
					break;
				}
			} 
			else if (LocaleCompare(name, "msl") == 0 )
			{
				/* 
					This is our base element.
						at the moment we don't do anything special
						but someday we might!
				*/
				break;
			}
			ThrowException(&msl_info->exception,OptionError,
							"Unrecognized element",(const char *) name);
		}
	case 'N':
	case 'n':
		{
			if (LocaleCompare(name, "normalize") == 0)
			{
				if (msl_info->image[n] == (Image *) NULL)
				{
					ThrowException(&msl_info->exception,OptionWarning,
									"no images defined",name);
					break;
				}
				
				/* no attributes here */

				/* process the image */
				{
					NormalizeImage(msl_info->image[n]);
					break;
				}
			}
			ThrowException(&msl_info->exception,OptionError,
							"Unrecognized element",(const char *) name);
		}
    case 'P':
    case 'p':
    {
      if (LocaleCompare(name,"print") == 0)
        {
		  /* print does not require an image to be present! */

          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            CloneString(&value,TranslateText(msl_info->image_info[n],
              msl_info->attributes[n],attributes[i]));
            switch (*keyword)
            {
              case 'O':
              case 'o':
              {
                if (LocaleCompare(keyword,"output") == 0)
                {
					(void) fprintf(stdout,"%s",value);
					break;
				}
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
				break;
              }
              default:
              {
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }
          break;
        }
      ThrowException(&msl_info->exception,OptionError,
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
            CloneString(&value,TranslateText(msl_info->image_info[n],
              msl_info->attributes[n],attributes[i]));
            switch (*keyword)
            {
              case 'F':
              case 'f':
              {
                if (LocaleCompare(keyword,"filename") == 0)
                  {
                    Image
                      *next_image;

                    (void) strcpy(msl_info->image_info[n]->filename,value);
                    next_image=ReadImage(msl_info->image_info[n],&exception);
                    if (exception.severity != UndefinedException)
                      MagickWarning(exception.severity,exception.reason,
                        exception.description);
                    if (next_image == (Image *) NULL)
                      continue;
                    if (msl_info->image[n] == (Image *) NULL)
                      msl_info->image[n]=next_image;
                    else
                      {
                        register Image
                          *p;

                        /*
                          Link image into image list.
                        */
                        p=msl_info->image[n];
                        for ( ; p->next != (Image *) NULL; p=p->next);
                        next_image->previous=p;
                        p->next=next_image;
                      }
                    break;
                  }
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
              case 'S':
              case 's':
              {
                if (LocaleCompare(keyword,"size") == 0)
                  {
                    msl_info->image_info[n]->size=AllocateString(value);
                    break;
                  }
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
              default:
              {
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }
          break;
        }
      if (LocaleCompare(name,"resize") == 0)
        {
		  /* init the values */
			width=msl_info->image[n]->columns;
			height=msl_info->image[n]->rows;
			x=0;
			y=0;

          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowException(&msl_info->exception,OptionWarning,
                "no images defined",name);
              break;
            }
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            CloneString(&value,TranslateText(msl_info->image_info[n],
              msl_info->attributes[n],attributes[i]));
            switch (*keyword)
            {
			  case 'B':
			  case 'b':
				  {
					if (LocaleCompare(keyword,"blur") == 0)
					  {
						msl_info->image[n]->blur = atof( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
			  case 'F':
			  case 'f':
				  {
					if (LocaleCompare(keyword, "filter") == 0)
					{
						FilterTypes	newFilter = UndefinedFilter;	/* default */
						if (LocaleCompare(value, "Point") == 0 )
							newFilter = PointFilter;
						else if (LocaleCompare(value, "Box") == 0 )
							newFilter = BoxFilter;
						else if (LocaleCompare(value, "Triangle") == 0 )
							newFilter = TriangleFilter;
						else if (LocaleCompare(value, "Hermite") == 0 )
							newFilter = HermiteFilter;
						else if (LocaleCompare(value, "Hanning") == 0 )
							newFilter = HanningFilter;
						else if (LocaleCompare(value, "Hamming") == 0 )
							newFilter = HammingFilter;
						else if (LocaleCompare(value, "Blackman") == 0 )
							newFilter = BlackmanFilter;
						else if (LocaleCompare(value, "Gaussian") == 0 )
							newFilter = GaussianFilter;
						else if (LocaleCompare(value, "Quadratic") == 0 )
							newFilter = QuadraticFilter;
						else if (LocaleCompare(value, "Cubic") == 0 )
							newFilter = CubicFilter;
						else if (LocaleCompare(value, "Catrom") == 0 )
							newFilter = CatromFilter;
						else if (LocaleCompare(value, "Mitchell") == 0 )
							newFilter = MitchellFilter;
						else if (LocaleCompare(value, "Lanczos") == 0 )
							newFilter = LanczosFilter;
						else if (LocaleCompare(value, "Bessel") == 0 )
							newFilter = BesselFilter;
						else if (LocaleCompare(value, "Sinc") == 0 )
							newFilter = SincFilter;

						msl_info->image[n]->filter = newFilter;
						break;
					}
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
				  }
              case 'G':
              case 'g':
              {
                if (LocaleCompare(keyword,"geometry") == 0)
                  {
                    (void) ParseImageGeometry(value,&x,&y,&width,&height);
                    break;
                  }
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
              case 'H':
              case 'h':
              {
                if (LocaleCompare(keyword,"height") == 0)
                  {
					height = atoi( value );
					break;
                  }
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
              case 'W':
              case 'w':
              {
                if (LocaleCompare(keyword,"width") == 0)
                  {
					width = atoi( value );
					break;
                  }
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
              default:
              {
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }

            /*
              Resize image.
            */
          {
            Image
              *resize_image;

            if ((width == msl_info->image[n]->columns) &&
                (height == msl_info->image[n]->rows))
              break;
            resize_image=ZoomImage(msl_info->image[n],width,height,
              &msl_info->image[n]->exception);
            if (resize_image == (Image *) NULL)
              break;
            DestroyImage(msl_info->image[n]);
            msl_info->image[n]=resize_image;
            break;
          }

          break;
        }
		  else if (LocaleCompare(name,"roll") == 0)
			{
			  /* init the values */
				width=msl_info->image[n]->columns;
				height=msl_info->image[n]->rows;
				x = y = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'G':
				  case 'g':
				  {
					if (LocaleCompare(keyword,"geometry") == 0)
					  {
						(void) ParseImageGeometry(value,&x,&y,&width,&height);
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'X':
				  case 'x':
				  {
					if (LocaleCompare(keyword,"x") == 0)
					  {
						x = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'Y':
				  case 'y':
				  {
					if (LocaleCompare(keyword,"y") == 0)
					  {
						y = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				Image
				  *newImage;

				newImage=RollImage(msl_info->image[n], x, y, &msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }

			  break;
			}
		  else if (LocaleCompare(name,"rotate") == 0)
			{
			  /* init the values */
			  double	degrees = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'D':
				  case 'd':
				  {
					if (LocaleCompare(keyword,"degrees") == 0)
					  {
						degrees = atof( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				Image
				  *newImage;

				newImage=RotateImage(msl_info->image[n], degrees, &msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }

			  break;
			}
		  ThrowException(&msl_info->exception,OptionError,
			"Unrecognized element",(const char *) name);
    }
	case 'S':
	case 's':
		{
		  if (LocaleCompare(name,"sample") == 0)
			{
			  /* init the values */
				width=msl_info->image[n]->columns;
				height=msl_info->image[n]->rows;
				x = y = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'G':
				  case 'g':
				  {
					if (LocaleCompare(keyword,"geometry") == 0)
					  {
						(void) ParseImageGeometry(value,&x,&y,&width,&height);
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'H':
				  case 'h':
				  {
					if (LocaleCompare(keyword,"height") == 0)
					  {
						height = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'W':
				  case 'w':
				  {
					if (LocaleCompare(keyword,"width") == 0)
					  {
						width = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				Image
				  *newImage;

				newImage=SampleImage(msl_info->image[n], width, height, &msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }

			  break;
			}
		  else if (LocaleCompare(name,"scale") == 0)
			{
			  /* init the values */
				width=msl_info->image[n]->columns;
				height=msl_info->image[n]->rows;
				x = y = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'G':
				  case 'g':
				  {
					if (LocaleCompare(keyword,"geometry") == 0)
					  {
						(void) ParseImageGeometry(value,&x,&y,&width,&height);
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'H':
				  case 'h':
				  {
					if (LocaleCompare(keyword,"height") == 0)
					  {
						height = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'W':
				  case 'w':
				  {
					if (LocaleCompare(keyword,"width") == 0)
					  {
						width = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				Image
				  *newImage;

				newImage=ScaleImage(msl_info->image[n], width, height, &msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }

			  break;
			}
			else if (LocaleCompare(name, "sharpen") == 0)
			{
				double	radius = 0.0,
						sigma = 1.0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  /*
				NOTE: sharpen can have no attributes, since we use all the defaults!
			  */
			  if (attributes != (const xmlChar **) NULL)
			  {
				  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
				  {
					keyword=(const char *) attributes[i++];
					CloneString(&value,TranslateText(msl_info->image_info[n],
					  msl_info->attributes[n],attributes[i]));
					switch (*keyword)
					{
					  case 'R':
					  case 'r':
						{
							if (LocaleCompare(keyword, "radius") == 0)
							{
							  radius = atof( value );
							  break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
						}
					  case 'S':
					  case 's':
					  {
							if (LocaleCompare(keyword,"sigma") == 0)
							{
								sigma = atoi( value );
								break;
							}
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
					  }
					  default:
					  {
							ThrowException(&msl_info->exception,OptionWarning,
							  "Unrecognized attribute",keyword);
							break;
					  }
					}
				  }
				}

				/*
				  sharpen image.
				*/
			  {
				Image
				  *newImage;

				newImage=SharpenImage(msl_info->image[n],radius,sigma,&msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }
			}
		  else if (LocaleCompare(name,"shave") == 0)
			{
			  /* init the values */
				width = height = 0;
				x = y = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'G':
				  case 'g':
				  {
					if (LocaleCompare(keyword,"geometry") == 0)
					  {
						(void) ParseImageGeometry(value,&x,&y,&width,&height);
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'H':
				  case 'h':
				  {
					if (LocaleCompare(keyword,"height") == 0)
					  {
						height = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'W':
				  case 'w':
				  {
					if (LocaleCompare(keyword,"width") == 0)
					  {
						width = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				Image
				  *newImage;
				RectangleInfo
					rectInfo;

				rectInfo.height = height;
				rectInfo.width = width;
				rectInfo.x = x;
				rectInfo.y = y;


				newImage=ShaveImage(msl_info->image[n], &rectInfo, &msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }

			  break;
			}
		  else if (LocaleCompare(name,"shear") == 0)
			{
			  /* init the values */
				width=msl_info->image[n]->columns;
				height=msl_info->image[n]->rows;
				x = y = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'X':
				  case 'x':
				  {
					if (LocaleCompare(keyword,"x") == 0)
					  {
						x = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  case 'Y':
				  case 'y':
				  {
					if (LocaleCompare(keyword,"y") == 0)
					  {
						y = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				Image
				  *newImage;

				newImage=ShearImage(msl_info->image[n], x, y, &msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }

			  break;
			}
		  else if (LocaleCompare(name,"solarize") == 0)
			{
			  /* init the values */
			  int	threshold = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'T':
				  case 't':
				  {
					if (LocaleCompare(keyword,"threshold") == 0)
					  {
						threshold = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				SolarizeImage(msl_info->image[n], threshold);
				break;
			  }
			}
		  else if (LocaleCompare(name,"spread") == 0)
			{
			  /* init the values */
			  unsigned int	radius = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'R':
				  case 'r':
				  {
					if (LocaleCompare(keyword,"radius") == 0)
					  {
						radius = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				Image
				  *newImage;

				newImage=SpreadImage(msl_info->image[n], radius, &msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }
			}
		  else if (LocaleCompare(name,"swirl") == 0)
			{
			  /* init the values */
			  double	degrees = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'D':
				  case 'd':
				  {
					if (LocaleCompare(keyword,"degrees") == 0)
					  {
						degrees = atof( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				Image
				  *newImage;

				newImage=SwirlImage(msl_info->image[n], degrees, &msl_info->image[n]->exception);
				if (newImage == (Image *) NULL)
				  break;
				DestroyImage(msl_info->image[n]);
				msl_info->image[n]=newImage;
				break;
			  }

			  break;
			}
			ThrowException(&msl_info->exception,OptionError,
							"Unrecognized element",(const char *) name);
		}
	case 'T':
	case 't':
		{
			if (LocaleCompare(name,"threshold") == 0)
			{
			  /* init the values */
			  int	threshold = 0;

			  if (msl_info->image[n] == (Image *) NULL)
				{
				  ThrowException(&msl_info->exception,OptionWarning,
					"no images defined",name);
				  break;
				}
			  if (attributes == (const xmlChar **) NULL)
				break;
			  for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
			  {
				keyword=(const char *) attributes[i++];
				CloneString(&value,TranslateText(msl_info->image_info[n],
				  msl_info->attributes[n],attributes[i]));
				switch (*keyword)
				{
				  case 'T':
				  case 't':
				  {
					if (LocaleCompare(keyword,"threshold") == 0)
					  {
						threshold = atoi( value );
						break;
					  }
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				  default:
				  {
					ThrowException(&msl_info->exception,OptionWarning,
					  "Unrecognized attribute",keyword);
					break;
				  }
				}
			  }

				/*
				  process image.
				*/
			  {
				ThresholdImage(msl_info->image[n], threshold);
				break;
			  }
			}
			else if (LocaleCompare(name, "trim") == 0)
			{
				if (msl_info->image[n] == (Image *) NULL)
				{
					ThrowException(&msl_info->exception,OptionWarning,
									"no images defined",name);
					break;
				}
				
				/* no attributes here */

				/* process the image */
				{
					Image
					  *newImage;
					RectangleInfo
						rectInfo;

					/* all zeros on a crop == trim edges! */
					rectInfo.height = rectInfo.width = 0;
					rectInfo.x =  rectInfo.y = 0;

					newImage=CropImage(msl_info->image[n],&rectInfo, &msl_info->image[n]->exception);
					if (newImage == (Image *) NULL)
					  break;
					DestroyImage(msl_info->image[n]);
					msl_info->image[n]=newImage;
					break;
				}
			}
			ThrowException(&msl_info->exception,OptionError,
							"Unrecognized element",(const char *) name);
		}
    case 'W':
    case 'w':
    {
      if (LocaleCompare(name,"write") == 0)
        {
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowException(&msl_info->exception,OptionWarning,
                "no images defined",name);
              break;
            }
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            CloneString(&value,TranslateText(msl_info->image_info[n],
              msl_info->attributes[n],attributes[i]));
            switch (*keyword)
            {
              case 'F':
              case 'f':
              {
                if (LocaleCompare(keyword,"filename") == 0)
                  {
                    (void) strncpy(msl_info->image[n]->filename,value,
                      MaxTextExtent);
                    (void) WriteImage(msl_info->image_info[n],
                      msl_info->image[n]);
                    break;
                  }
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
              }
              default:
              {
                ThrowException(&msl_info->exception,OptionWarning,
                  "Unrecognized attribute",keyword);
                break;
              }
            }
          }
          break;
        }
      ThrowException(&msl_info->exception,OptionError,
        "Unrecognized element",(const char *) name);
    }
    default:
    {
      ThrowException(&msl_info->exception,OptionError,"Unrecognized element",
        (const char *) name);
      break;
    }
  }
  if ( value != NULL )
	LiberateMemory((void **) &value);
  if (msl_info->debug)
    (void) fprintf(stdout,"  )\n");
}

static void MSLEndElement(void *context,const xmlChar *name)
{
	MSLInfo
		*msl_info;

	/*
		Called when the end of an element has been detected.
	*/
	msl_info=(MSLInfo *) context;
	if (msl_info->debug)
		(void) fprintf(stdout,"  SAX.endElement(%.1024s)\n",(char *) name);
	switch (*name)
	{
		case 'G':
		case 'g':
		{
			if (LocaleCompare(name, "group") == 0 )
			{
				if (msl_info->group_info[msl_info->nGroups-1].numImages > 0 ) 
				{
					int	i = msl_info->group_info[msl_info->nGroups-1].numImages;
					while ( i-- ) 
					{
						if (msl_info->image[msl_info->n] != (Image *) NULL)
							DestroyImage(msl_info->image[msl_info->n]);
						DestroyImage(msl_info->attributes[msl_info->n]);
						DestroyImageInfo(msl_info->image_info[msl_info->n]);
						msl_info->n--;
					}
				}
				msl_info->nGroups--;
			}
		}
		break;

		case 'I':
		case 'i':
		{
			if (LocaleCompare(name, "image") == 0)
			{
				/*
					only dispose of images when they aren't in a group
				*/
				if ( msl_info->nGroups == 0 )
				{
					if (msl_info->image[msl_info->n] != (Image *) NULL)
						DestroyImage(msl_info->image[msl_info->n]);
					DestroyImage(msl_info->attributes[msl_info->n]);
					DestroyImageInfo(msl_info->image_info[msl_info->n]);
					msl_info->n--;
				}
			}
		}
		break;

		case 'M':
		case 'm':
		{
			if (LocaleCompare(name, "msl") == 0 )
			{
				/* 
					This is our base element.
						at the moment we don't do anything special
						but someday we might!
				*/
			}
		}
		break;

		default:
		break;
	}
}

static void MSLCharacters(void *context,const xmlChar *c,int length)
{
  register int
    i;

  MSLInfo
    *msl_info;

  /*
    Receiving some characters from the parser.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    {
      (void) fprintf(stdout,"  SAX.characters(");
      for (i=0; (i < length) && (i < 30); i++)
        (void) fprintf(stdout,"  %c",c[i]);
      (void) fprintf(stdout,"  , %d)\n",length);
    }
}

static void MSLReference(void *context,const xmlChar *name)
{
  MSLInfo
    *msl_info;

  xmlParserCtxtPtr
    parser;

  /*
    Called when an entity reference is detected.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.reference(%.1024s)\n",name);
  parser=msl_info->parser;
  if (*name == '#')
    (void) xmlAddChild(parser->node,xmlNewCharRef(msl_info->document,name));
  else
    (void) xmlAddChild(parser->node,xmlNewReference(msl_info->document,name));
}

static void MSLIgnorableWhitespace(void *context,const xmlChar *c,int length)
{
  MSLInfo
    *msl_info;

  /*
    Receiving some ignorable whitespaces from the parser.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.ignorableWhitespace(%.30s, %d)\n",(char *) c,
      length);
}

static void MSLProcessingInstructions(void *context,const xmlChar *target,
  const xmlChar *data)
{
  MSLInfo
    *msl_info;

  /*
    A processing instruction has been parsed.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.processingInstruction(%.1024s, %.1024s)\n",
      (char *) target,(char *) data);
}

static void MSLComment(void *context,const xmlChar *value)
{
  MSLInfo
    *msl_info;

  /*
    A comment has been parsed.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.comment(%.1024s)\n",value);
}

static void MSLWarning(void *context,const char *format,...)
{
  char
    reason[MaxTextExtent];

  MSLInfo
    *msl_info;

  va_list
    operands;

  /**
    Display and format a warning messages, gives file, line, position and
    extra parameters.
  */
  va_start(operands,format);
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    {
      (void) fprintf(stdout,"  SAX.warning: ");
      (void) vfprintf(stdout,format,operands);
    }
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  ThrowException(&msl_info->exception,DelegateWarning,reason,(char *) NULL);
  va_end(operands);
}

static void MSLError(void *context,const char *format,...)
{
  char
    reason[MaxTextExtent];

  MSLInfo
    *msl_info;

  va_list
    operands;

  /*
    Display and format a error formats, gives file, line, position and
    extra parameters.
  */
  va_start(operands,format);
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    {
      (void) fprintf(stdout,"  SAX.error: ");
      (void) vfprintf(stdout,format,operands);
    }
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  ThrowException(&msl_info->exception,DelegateError,reason,(char *) NULL);
  va_end(operands);
}

static void MSLCDataBlock(void *context,const xmlChar *value,int length)
{
  MSLInfo
    *msl_info;

   xmlNodePtr
     child;

  xmlParserCtxtPtr
    parser;

  /*
    Called when a pcdata block has been parsed.
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    (void) fprintf(stdout,"  SAX.pcdata(%.1024s, %d)\n",(char *) value,length);
  parser=msl_info->parser;
  child=xmlGetLastChild(parser->node);
  if ((child != (xmlNodePtr) NULL) && (child->type == XML_CDATA_SECTION_NODE))
    {
      xmlTextConcat(child,value,length);
      return;
    }
  (void) xmlAddChild(parser->node,xmlNewCDataBlock(parser->myDoc,value,length));
}

static void MSLExternalSubset(void *context,const xmlChar *name,
  const xmlChar *external_id,const xmlChar *system_id)
{
  MSLInfo
    *msl_info;

  xmlParserCtxt
    parser_context;

  xmlParserCtxtPtr
    parser;

  xmlParserInputPtr
    input;

  /*
    Does this document has an enternal subset?
  */
  msl_info=(MSLInfo *) context;
  if (msl_info->debug)
    {
      (void) fprintf(stdout,"  SAX.InternalSubset(%.1024s",name);
      if (external_id != NULL)
        (void) fprintf(stdout,"  , %.1024s",external_id);
      if (system_id != NULL)
        (void) fprintf(stdout,"  , %.1024s",system_id);
      (void) fprintf(stdout,"\n");
    }
  parser=msl_info->parser;
  if (((external_id == NULL) && (system_id == NULL)) ||
      (!parser->validate || !parser->wellFormed || !msl_info->document))
    return;
  input=MSLResolveEntity(context,external_id,system_id);
  if (input == NULL)
    return;
  (void) xmlNewDtd(msl_info->document,name,external_id,system_id);
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
      "you can specify any name and it's value to later read back from your script.",
	  "	examples: -geometry '100x100' -backcolor 'blue' -foo 'bar'",
      (char *) NULL
    };

  const char
    **p;

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf("Usage: %.1024s [options ...] msl_script_file [ image_files... ] \n",
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
      MSLInternalSubset,
      MSLIsStandalone,
      MSLHasInternalSubset,
      MSLHasExternalSubset,
      MSLResolveEntity,
      MSLGetEntity,
      MSLEntityDeclaration,
      MSLNotationDeclaration,
      MSLAttributeDeclaration,
      MSLElementDeclaration,
      MSLUnparsedEntityDeclaration,
      MSLSetDocumentLocator,
      MSLStartDocument,
      MSLEndDocument,
      MSLStartElement,
      MSLEndElement,
      MSLReference,
      MSLCharacters,
      MSLIgnorableWhitespace,
      MSLProcessingInstructions,
      MSLComment,
      MSLWarning,
      MSLError,
      MSLError,
      MSLGetParameterEntity,
      MSLCDataBlock,
      MSLExternalSubset
    };

  char
    message[MaxTextExtent];

  long
    n;

  register int
    i;

  MSLInfo
    msl_info;

  unsigned int
    status,
	done = False;

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
  (void) memset(&msl_info,0,sizeof(MSLInfo));
  GetExceptionInfo(&msl_info.exception);
  msl_info.image_info=(ImageInfo **) AcquireMemory(sizeof(ImageInfo *));
  msl_info.image=(Image **) AcquireMemory(sizeof(Image *));
  msl_info.attributes=(Image **) AcquireMemory(sizeof(Image *));
  msl_info.group_info=(MSLGroupInfo *) AcquireMemory(sizeof(MSLGroupInfo));
  if ((msl_info.image_info == (ImageInfo **) NULL) ||
      (msl_info.image == (Image **) NULL) ||
      (msl_info.attributes == (Image **) NULL) ||
	  (msl_info.group_info == (MSLGroupInfo *) NULL))
    MagickError(ResourceLimitError,"Unable to allocate image",
      "Memory allocation failed");
  msl_info.image_info[0]=CloneImageInfo((ImageInfo *) NULL);
  msl_info.attributes[0]=AllocateImage(msl_info.image_info[0]);
  msl_info.image[0]=AllocateImage(msl_info.image_info[0]);
  msl_info.group_info[0].numImages = 0;
  if ((msl_info.image_info[0] == (ImageInfo *) NULL) ||
      (msl_info.attributes[0] == (Image *) NULL) ||
      (msl_info.image[0] == (Image *) NULL))
    MagickError(ResourceLimitError,"Unable to allocate image",
      "Memory allocation failed");

#if 0
  for (i=1; i < (argc-1); i+=2)
  {
    status=SetImageAttribute(msl_info.attributes[0],argv[i]+1,argv[i+1]);
    if (status == False)
      MagickError(OptionError,"Unable to persist key/value pair",argv[i]);
  }
  if (i != (argc-1))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  (void) strcpy(msl_info.image[0]->filename,argv[argc-1]);
#endif

	i = 1;
	while ( *argv[i] == '-' )	/* find all the key/value pairs */
	{
		status=SetImageAttribute(msl_info.attributes[0],argv[i]+1,argv[i+1]);
		if (status == False)
		  MagickError(OptionError,"Unable to persist key/value pair",argv[i]);	
		i+=2;
	}
	if (i == argc)	/* no more parameters :( */
		MagickError(OptionError,"Missing an image file name",(char *) NULL);
	(void) strcpy(msl_info.image[0]->filename,argv[i++]);

	/* at this point, the rest of the arguments should be filenames!! */

	while (!done)
	{
		/* setup the filename attribute based on the passed names */
		if (i < argc) {
			status=SetImageAttribute(msl_info.attributes[0],"filename",NULL);	/* clear it out first */
			status=SetImageAttribute(msl_info.attributes[0],"filename",argv[i]);
			if (status == False)
			  MagickError(OptionError,"Unable to persist key/value pair",argv[i]);	
			i++;
		}

		/*
			Open image file.
		*/
		status=OpenBlob(*msl_info.image_info,*msl_info.image,ReadBinaryType,
						&msl_info.exception);
		if (status == False)
			MagickError(FileOpenWarning,"Unable to open file", msl_info.image[0]->filename);

		/*
			Parse MSL file.
		*/
		(void) xmlSubstituteEntitiesDefault(1);
		SAXHandler=(&SAXModules);
		msl_info.parser=xmlCreatePushParserCtxt(SAXHandler,&msl_info,(char *) NULL,0, msl_info.image[0]->filename);
		while (ReadBlobString(msl_info.image[0],message) != (char *) NULL)
		{
			n=(long) strlen(message);
			if (n == 0)
				continue;
			status=xmlParseChunk(msl_info.parser,message,(int) n,False);
			if (status != 0)
				break;

			(void) xmlParseChunk(msl_info.parser," ",1,False);
			if (msl_info.exception.severity != UndefinedException)
			{
				MagickError(msl_info.exception.severity,msl_info.exception.reason, msl_info.exception.description);
			break;
			}
		}
		(void) xmlParseChunk(msl_info.parser," ",1,True);
		xmlFreeParserCtxt(msl_info.parser);
		if (msl_info.debug)
			(void) fprintf(stdout,"end SAX\n");
		xmlCleanupParser();

		if (i >= argc)
			done = True;
	}

	DestroyImage(*msl_info.image);
	DestroyMagick();
	LiberateMemory(&msl_info.group_info);
	LiberateMemory((void **) &argv);
	Exit(0);
	return(False);
}
