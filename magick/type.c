/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        TTTTT  Y   Y  PPPP   EEEEE                           %
%                          T     Y Y   P   P  E                               %
%                          T      Y    PPPP   EEE                             %
%                          T      Y    P      E                               %
%                          T      Y    P      EEEEE                           %
%                                                                             %
%                                                                             %
%                     GraphicsMagick Image Type Methods                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 May 2001                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#if defined(WIN32) || defined(__CYGWIN__)
# include "nt_feature.h"
#endif
#include "blob.h"
#include "log.h"
#include "render.h"
#include "utility.h"

/*
  Define declarations.
*/
#define TypeFilename  "type.mgk"

/*
  Declare type map.
*/
static char
  *TypeMap = (char *)
    "<?xml version=\"1.0\"?>"
    "<typemap>"
    "  <type stealth=\"True\" />"
    "</typemap>";

/*
  Static declarations.
*/
static SemaphoreInfo
  *type_semaphore = (SemaphoreInfo *) NULL;

static TypeInfo
  *type_list = (TypeInfo *) NULL;

/*
  Forward declarations.
*/
static void
  *GetTypeBlob(const char *filename,char *path,size_t *length,
    ExceptionInfo *exception);

static unsigned int
  ReadConfigureFile(const char *,const unsigned long,ExceptionInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y T y p e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyTypeInfo deallocates memory associated with the font list.
%
%  The format of the DestroyTypeInfo method is:
%
%      DestroyTypeInfo(void)
%
%
*/
MagickExport void DestroyTypeInfo(void)
{
  register TypeInfo
    *p;

  TypeInfo
    *type_info;

  AcquireSemaphoreInfo(&type_semaphore);
  for (p=type_list; p != (TypeInfo *) NULL; )
  {
    type_info=p;
    p=p->next;
    if (type_info->path != (char *) NULL)
      LiberateMemory((void **) &type_info->path);
    if (type_info->name != (char *) NULL)
      LiberateMemory((void **) &type_info->name);
    if (type_info->description != (char *) NULL)
      LiberateMemory((void **) &type_info->description);
    if (type_info->family != (char *) NULL)
      LiberateMemory((void **) &type_info->family);
    if (type_info->encoding != (char *) NULL)
      LiberateMemory((void **) &type_info->encoding);
    if (type_info->foundry != (char *) NULL)
      LiberateMemory((void **) &type_info->foundry);
    if (type_info->format != (char *) NULL)
      LiberateMemory((void **) &type_info->format);
    if (type_info->metrics != (char *) NULL)
      LiberateMemory((void **) &type_info->metrics);
    if (type_info->glyphs != (char *) NULL)
      LiberateMemory((void **) &type_info->glyphs);
    LiberateMemory((void **) &type_info);
  }
  type_list=(TypeInfo *) NULL;
  DestroySemaphoreInfo(&type_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t T y p e B l o b                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetTypeBlob() returns the specified font file as a blob.
%
%  The format of the GetTypeBlob method is:
%
%      void *GetTypeBlob(const char *filename,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: The font file name.
%
%    o path: return the full path information of the font file.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#if !defined(UseInstalledImageMagick) && defined(POSIX)
static void ChopPathComponents(char *path,const unsigned long components)
{
  long
    count;

  register char
    *p;

  if (*path == '\0')
    return;
  p=path+strlen(path);
  if (*p == *DirectorySeparator)
    *p='\0';
  for (count=0; (count < (long) components) && (p > path); p--)
    if (*p == *DirectorySeparator)
      {
        *p='\0';
        count++;
      }
}
#endif

static void *GetTypeBlob(const char *filename,char *path,size_t *length,
  ExceptionInfo *exception)
{
  assert(filename != (const char *) NULL);
  assert(path != (char *) NULL);
  assert(length != (size_t *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  (void) strncpy(path,filename,MaxTextExtent-1);
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Searching for type file \"%s\" ...",filename);
  if (getenv("MAGICK_FONT_PATH") != (char *) NULL)
    {
      /*
        Search MAGICK_FONT_PATH.
      */
      FormatString(path,"%.1024s%s%.1024s",getenv("MAGICK_FONT_PATH"),
        DirectorySeparator,filename);
      if (IsAccessible(path))
        return(FileToBlob(path,length,exception));
    }
#if defined(UseInstalledImageMagick)
#if defined(WIN32)
  {
    char
      *key_value;

    /*
      Locate file via registry key.
    */
    key_value=NTRegistryKeyLookup("ConfigurePath");
    if (key_value != (char *) NULL)
      {
        FormatString(path,"%.1024s%s%.1024s",key_value,DirectorySeparator,
          filename);
        if (!IsAccessible(path))
          ThrowException(exception,ConfigureError,"UnableToAccessFontFile",
            path);
        return(FileToBlob(path,length,exception));
      }
  }
#endif
#if defined(MagickLibPath)
  /*
    Search hard coded paths.
  */
  FormatString(path,"%.1024s%.1024s",MagickLibPath,filename);
  if (!IsAccessible(path))
    ThrowException(exception,ConfigureError,"UnableToAccessFontFile",path);
  return(FileToBlob(path,length,exception));
#endif
#else
  if (*SetClientPath((char *) NULL) != '\0')
    {
#if defined(POSIX)
      char
        prefix[MaxTextExtent];

      /*
        Search based on executable directory if directory is known.
      */
      (void) strncpy(prefix,SetClientPath((char *) NULL),MaxTextExtent-1);
      ChopPathComponents(prefix,1);
      FormatString(path,"%.1024s/lib/%s/%.1024s",prefix,MagickLibSubdir,
        filename);
#else
      FormatString(path,"%.1024s%s%.1024s",SetClientPath((char *) NULL),
        DirectorySeparator,filename);
#endif
      if (IsAccessible(path))
        return(FileToBlob(path,length,exception));
    }
  /*
    Search current directory.
  */
  if (IsAccessible(path))
    return(FileToBlob(path,length,exception));
#if defined(WIN32)
  return(NTResourceToBlob(filename));
#endif
#endif
  ThrowException(exception,ConfigureError,"UnableToAccessFontFile",path);
  return((void *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t T y p e I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetTypeInfo searches the type list for the specified name and if
%  found returns attributes for that type.
%
%  The format of the GetTypeInfo method is:
%
%      const TypeInfo *GetTypeInfo(const char *name,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o type_info: Method GetTypeInfo searches the type list for the specified
%      name and if found returns attributes for that type.
%
%    o name: The type name.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const TypeInfo *GetTypeInfo(const char *name,
  ExceptionInfo *exception)
{
  register TypeInfo
    *p;

  if (type_list == (TypeInfo *) NULL)
    {
      AcquireSemaphoreInfo(&type_semaphore);
      if (type_list == (TypeInfo *) NULL)
        {
          (void) ReadConfigureFile(TypeFilename,0,exception);
#if defined(WIN32) || defined(__CYGWIN__)
          {
            TypeInfo
              *type_info;

            type_info=NTGetTypeList();
            if (type_info != (TypeInfo *) NULL)
              {
                if (type_list == (TypeInfo *) NULL)
                  type_list=type_info;
                else
                  {
                    while (type_list->next != (TypeInfo *) NULL)
                      type_list=type_list->next;
                    type_list->next=type_info;
                    type_info->previous=type_list;
                    while (type_list->previous != (TypeInfo *) NULL)
                      type_list=type_list->previous;
                  }
              }
          }
#endif
        }
      LiberateSemaphoreInfo(&type_semaphore);
    }
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    return((const TypeInfo *) type_list);
  /*
    Search for requested type.
  */
  AcquireSemaphoreInfo(&type_semaphore);
  for (p=type_list; p != (TypeInfo *) NULL; p=p->next)
    if ((p->name != (char *) NULL) && (LocaleCompare(p->name,name) == 0))
      break;
  if (p != (TypeInfo *) NULL)
    if (p != type_list)
      {
        /*
          Self-adjusting list.
        */
        if (p->previous != (TypeInfo *) NULL)
          p->previous->next=p->next;
        if (p->next != (TypeInfo *) NULL)
          p->next->previous=p->previous;
        p->previous=(TypeInfo *) NULL;
        p->next=type_list;
        type_list->previous=p;
        type_list=p;
      }
  LiberateSemaphoreInfo(&type_semaphore);
  return((const TypeInfo *) p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t T y p e I n f o B y F a m i l y                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetTypeInfoByFamily searches the type list for the specified family
%  and if found returns attributes for that type.
%
%  Type substitution and scoring algorithm contributed by Bob Friesenhahn.
%
%  The format of the GetTypeInfoByFamily method is:
%
%      const TypeInfo *GetTypeInfoByFamily(const char *family,
%        const StyleType style,const StretchType stretch,
%        const unsigned long weight,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o type_info: Method GetTypeInfo searches the type list for the specified
%      name and if found returns attributes for that type.
%
%    o family: The type family.
%
%    o style: The type style.
%
%    o stretch: The type stretch.
%
%    o weight: The type weight.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const TypeInfo *GetTypeInfoByFamily(const char *family,
  const StyleType style,const StretchType stretch,const unsigned long weight,
  ExceptionInfo *exception)
{
  typedef struct _Fontmap
  {
    char
      *name,
      *substitute;
  } Fontmap;

  const TypeInfo
    *type_info;

  long
    range;

  register const TypeInfo
    *p;

  register long
    i;

  static Fontmap
    fontmap[] =
    {
      { (char *) "fixed", (char *) "courier" },
      { (char *) "modern",(char *) "courier" },
      { (char *) "monotype corsiva", (char *) "courier" },
      { (char *) "news gothic", (char *) "helvetica" },
      { (char *) "system", (char *) "courier" },
      { (char *) "terminal", (char *) "courier" },
      { (char *) "wingdings", (char *) "symbol" },
      { (char *) NULL, (char *) NULL }
    };

  unsigned long
    max_score,
    score;

  /*
    Check for an exact type match.
  */
  (void) GetTypeInfo("*",exception);
  if (type_list == (TypeInfo *) NULL)
    return((TypeInfo *) NULL);
  for (p=type_list; p != (TypeInfo *) NULL; p=p->next)
  {
    if (p->family == (char *) NULL)
      continue;
    if (family == (const char *) NULL)
      {
        if ((LocaleCompare(p->family,"arial") != 0) &&
            (LocaleCompare(p->family,"helvetica") != 0))
          continue;
      }
    else
      if (LocaleCompare(p->family,family) != 0)
        continue;
    if ((style != AnyStyle) && (p->style != style))
      continue;
    if ((stretch != AnyStretch) && (p->stretch != stretch))
      continue;
    if ((weight != 0) && (p->weight != weight))
      continue;
    return((TypeInfo *) p);
  }
  /*
    Check for types in the same family.
  */
  max_score=0;
  type_info=(TypeInfo *) NULL;
  for (p=type_list; p != (TypeInfo *) NULL; p=p->next)
  {
    if (p->family == (char *) NULL)
      continue;
    if (family == (const char *) NULL)
      {
        if ((LocaleCompare(p->family,"arial") != 0) &&
            (LocaleCompare(p->family,"helvetica") != 0))
          continue;
      }
    else
      if (LocaleCompare(p->family,family) != 0)
        continue;
    score=0;
    if ((style == AnyStyle) || (p->style == style))
      score+=32;
    else
      if (((style == ItalicStyle) || (style == ObliqueStyle)) &&
          ((p->style == ItalicStyle) || (p->style == ObliqueStyle)))
        score+=25;
    if (weight == 0)
      score+=16;
    else
      score+=(16*(800-((long) Max(Min(weight,900),p->weight)-
        (long) Min(Min(weight,900),p->weight))))/800;
    if (stretch == AnyStretch)
      score+=8;
    else
      {
        range=(long) UltraExpandedStretch-(long) NormalStretch;
        score+=(8*(range-((long) Max(stretch,p->stretch)-
          (long) Min(stretch,p->stretch))))/range;
      }
    if (score > max_score)
      {
        max_score=score;
        type_info=p;
      }
  }
  if (type_info != (TypeInfo *) NULL)
    return((TypeInfo *) type_info);
  /*
    Check for table-based substitution match.
  */
  for (i=0; fontmap[i].name != (char *) NULL; i++)
  {
    if (family == (const char *) NULL)
      {
        if ((LocaleCompare(fontmap[i].name,"arial") != 0) &&
            (LocaleCompare(fontmap[i].name,"helvetica") != 0))
          continue;
      }
    else
      if (LocaleCompare(fontmap[i].name,family) != 0)
        continue;
    type_info=GetTypeInfoByFamily(fontmap[i].substitute,style,stretch,weight,
      exception);
    break;
  }
  if (type_info != (TypeInfo *) NULL)
    {
      ThrowException(exception,TypeError,"FontSubstitutionRequired",
        type_info->family);
      return((TypeInfo *) type_info);
    }
  if (family != (const char *) NULL)
    type_info=GetTypeInfoByFamily((const char *) NULL,style,stretch,weight,
      exception);
  return((TypeInfo *) type_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t T y p e l i s t                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetTypeList() returns any types that match the specified pattern.
%
%  The format of the GetTypeList function is:
%
%      char **GetTypeList(const char *pattern,unsigned long *number_types)
%
%  A description of each parameter follows:
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_types:  This integer returns the number of types in the list.
%
%
*/
MagickExport char **GetTypeList(const char *pattern,unsigned long *number_types)
{
  char
    **typelist;

  ExceptionInfo
    exception;

  register const TypeInfo
    *p;

  register long
    i;

  /*
    Allocate type list.
  */
  assert(pattern != (char *) NULL);
  assert(number_types != (unsigned long *) NULL);
  *number_types=0;
  GetExceptionInfo(&exception);
  p=GetTypeInfo("*",&exception);
  DestroyExceptionInfo(&exception);
  if (p == (const TypeInfo *) NULL)
    return((char **) NULL);
  i=0;
  for (p=type_list; p != (const TypeInfo *) NULL; p=p->next)
    i++;
  typelist=(char **) AcquireMemory(i*sizeof(char *));
  if (typelist == (char **) NULL)
    return((char **) NULL);
  i=0;
  for (p=type_list; p != (const TypeInfo *) NULL; p=p->next)
  {
    if (p->stealth)
      continue;
    if (GlobExpression(p->name,pattern))
      typelist[i++]=AllocateString(p->name);
  }
  *number_types=i;
  return(typelist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t T y p e I n f o                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListTypeInfo lists the fonts to a file.
%
%  The format of the ListTypeInfo method is:
%
%      unsigned int ListTypeInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static const char *StretchTypeToString(StretchType stretch)
{
  switch(stretch)
  {
    case NormalStretch: return("normal");
    case UltraCondensedStretch: return("ultra-condensed");
    case ExtraCondensedStretch: return("extra-condensed");
    case CondensedStretch: return("condensed");
    case SemiCondensedStretch: return("semi-condensed");
    case SemiExpandedStretch: return("semi-expanded");
    case ExpandedStretch: return("expanded");
    case ExtraExpandedStretch: return("extra-expanded");
    case UltraExpandedStretch: return("ultra-expanded");
    case AnyStretch: return("any");
    default: break;
  }
  return("unknown");
}

static const char *StyleTypeToString(StyleType style)
{
  switch(style)
  {
    case NormalStyle: return("normal");
    case ItalicStyle: return("italic");
    case ObliqueStyle: return("oblique");
    case AnyStyle: return("any");
    default:  break;
  }
  return("unknown");
}

MagickExport unsigned int ListTypeInfo(FILE *file,ExceptionInfo *exception)
{
  char
    weight[MaxTextExtent];

  const char
    *family,
    *name,
    *stretch,
    *style;

  register const TypeInfo
    *p;

  if (file == (FILE *) NULL)
    file=stdout;
  (void) GetTypeInfo("*",exception);
  AcquireSemaphoreInfo(&type_semaphore);
  for (p=type_list; p != (const TypeInfo *) NULL; p=p->next)
  {
    if ((p->previous == (TypeInfo *) NULL) ||
        (LocaleCompare(p->path,p->previous->path) != 0))
      {
        if (p->previous != (TypeInfo *) NULL)
          (void) fprintf(file,"\n");
        if (p->path != (char *) NULL)
          (void) fprintf(file,"Path: %.1024s\n\n",p->path);
        (void) fprintf(file,"%-32.32s %-23.23s %-7.7s %-8s %-3s\n",
          "Name","Family","Style","Stretch","Weight");
        (void) fprintf(file,"--------------------------------------------------"
          "------------------------------\n");
      }
    if (p->stealth)
      continue;
    name="unknown";
    if (p->name != (char *) NULL)
      name=p->name;
    family="unknown";
    if (p->family != (char *) NULL)
      family=p->family;
    style=StyleTypeToString(p->style);
    stretch=StretchTypeToString(p->stretch);
    FormatString(weight,"%lu",p->weight);
    (void) fprintf(file,"%-32.32s %-23.23s %-7.7s %-9s %-3s\n",
      name,family,style,stretch,weight);
  }
  (void) fflush(file);
  LiberateSemaphoreInfo(&type_semaphore);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r e F i l e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadConfigureFile() reads the type configuration file which provides
%  a mapping between type attributes and font files.
%
%  The format of the ReadConfigureFile method is:
%
%      unsigned int ReadConfigureFile(const char *basename,
%        const unsigned long depth,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: ReadConfigureFile() returns True if at least one entry
%      is read, otherwise False is returned.
%
%    o basename:  The type configuration filename.
%
%    o depth: depth of <include /> statements.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int ReadConfigureFile(const char *basename,
  const unsigned long depth,ExceptionInfo *exception)
{
  char
    keyword[MaxTextExtent],
    path[MaxTextExtent],
    *q,
    *token,
    *xml;

  size_t
    length;

  /*
    Read the type configure file.
  */
  (void) strcpy(path,basename);
  if (depth == 0)
    xml=(char *) GetTypeBlob(basename,path,&length,exception);
  else
    xml=(char *) FileToBlob(basename,&length,exception);
  if (xml == (char *) NULL)
    xml=AllocateString(TypeMap);
  token=AllocateString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetToken(q,&q,token);
    if (*token == '\0')
      break;
    (void) strncpy(keyword,token,MaxTextExtent-1);
    if (LocaleNCompare(keyword,"<!--",4) == 0)
      {
        /*
          Comment element.
        */
        while ((LocaleNCompare(q,"->",2) != 0) && (*q != '\0'))
          GetToken(q,&q,token);
        continue;
      }
    if (LocaleCompare(keyword,"<include") == 0)
      {
        /*
          Include element.
        */
        while ((*token != '>') && (*q != '\0'))
        {
          (void) strncpy(keyword,token,MaxTextExtent-1);
          GetToken(q,&q,token);
          if (*token != '=')
            continue;
          GetToken(q,&q,token);
          if (LocaleCompare(keyword,"file") == 0)
            {
              if (depth > 200)
                ThrowException(exception,ConfigureError,
                  "IncludeElementNestedTooDeeply",path);
              else
                {
                  char
                    filename[MaxTextExtent];

                  GetPathComponent(path,HeadPath,filename);
                  if (*filename != '\0')
                    (void) strcat(filename,DirectorySeparator);
                  (void) strncat(filename,token,MaxTextExtent-
                    strlen(filename)-1);
                  (void) ReadConfigureFile(filename,depth+1,exception);
                }
              if (type_list != (TypeInfo *) NULL)
                while (type_list->next != (TypeInfo *) NULL)
                  type_list=type_list->next;
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<type") == 0)
      {
        TypeInfo
          *type_info;

        /*
          Allocate memory for the type list.
        */
        type_info=(TypeInfo *) AcquireMemory(sizeof(TypeInfo));
        if (type_info == (TypeInfo *) NULL)
          MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
            "UnableToAllocateTypeInfo");
        (void) memset(type_info,0,sizeof(TypeInfo));
        type_info->path=AcquireString(path);
        type_info->signature=MagickSignature;
        if (type_list == (TypeInfo *) NULL)
          {
            type_list=type_info;
            continue;
          }
        type_list->next=type_info;
        type_info->previous=type_list;
        type_list=type_list->next;
        continue;
      }
    if (type_list == (TypeInfo *) NULL)
      continue;
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword)
    {
      case 'E':
      case 'e':
      {
        if (LocaleCompare((char *) keyword,"encoding") == 0)
          {
            type_list->encoding=AcquireString(token);
            break;
          }
        break;
      }
      case 'F':
      case 'f':
      {
        if (LocaleCompare((char *) keyword,"family") == 0)
          {
            type_list->family=AcquireString(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"format") == 0)
          {
            type_list->format=AcquireString(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"foundry") == 0)
          {
            type_list->foundry=AcquireString(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"fullname") == 0)
          {
            type_list->description=AcquireString(token);
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        if (LocaleCompare((char *) keyword,"glyphs") == 0)
          {
            char
              *glyphs;

            glyphs=(char *) NULL;
            CloneString(&glyphs,token);
#if defined(WIN32)
            if (strchr(glyphs,'@') != (char *) NULL)
              {
                char
                  path[MaxTextExtent];

                NTGhostscriptFonts(path,MaxTextExtent-2);
                (void) strcat(path,DirectorySeparator);
                SubstituteString(&glyphs,"@ghostscript_font_dir@",path);
              }
#endif
            type_list->glyphs=glyphs;
            break;
          }
        break;
      }
      case 'M':
      case 'm':
      {
        if (LocaleCompare((char *) keyword,"metrics") == 0)
          {
            char
              *metrics;

            metrics=(char *) NULL;
            CloneString(&metrics,token);
#if defined(WIN32)
            if (strchr(metrics,'@') != (char *) NULL)
              {
                char
                  path[MaxTextExtent];

                NTGhostscriptFonts(path,MaxTextExtent-2);
                (void) strcat(path,DirectorySeparator);
                SubstituteString(&metrics,"@ghostscript_font_dir@",path);
              }
#endif
            type_list->metrics=metrics;
            break;
          }
        break;
      }
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            type_list->name=AcquireString(token);
            break;
          }
        break;
      }
      case 'S':
      case 's':
      {
        if (LocaleCompare((char *) keyword,"stealth") == 0)
          {
            type_list->stealth=LocaleCompare(token,"True") == 0;
            break;
          }
        if (LocaleCompare((char *) keyword,"stretch") == 0)
          {
            if (LocaleCompare(token,"condensed") == 0)
              type_list->stretch=CondensedStretch;
            if (LocaleCompare(token,"expanded") == 0)
              type_list->stretch=ExpandedStretch;
            if (LocaleCompare(token,"extra-condensed") == 0)
              type_list->stretch=ExtraCondensedStretch;
            if (LocaleCompare(token,"extra-expanded") == 0)
              type_list->stretch=ExtraExpandedStretch;
            if (LocaleCompare(token,"normal") == 0)
              type_list->stretch=NormalStretch;
            if (LocaleCompare(token,"semi-condensed") == 0)
              type_list->stretch=SemiCondensedStretch;
            if (LocaleCompare(token,"semi-expanded") == 0)
              type_list->stretch=SemiExpandedStretch;
            if (LocaleCompare(token,"ultra-condensed") == 0)
              type_list->stretch=UltraCondensedStretch;
            if (LocaleCompare(token,"ultra-expanded") == 0)
              type_list->stretch=UltraExpandedStretch;
            break;
          }
        if (LocaleCompare((char *) keyword,"style") == 0)
          {
            if (LocaleCompare(token,"italic") == 0)
              type_list->style=ItalicStyle;
            if (LocaleCompare(token,"normal") == 0)
              type_list->style=NormalStyle;
            if (LocaleCompare(token,"oblique") == 0)
              type_list->style=ObliqueStyle;
            break;
          }
        break;
      }
      case 'W':
      case 'w':
      {
        if (LocaleCompare((char *) keyword,"weight") == 0)
          {
            type_list->weight=atol(token);
            if (LocaleCompare(token,"bold") == 0)
              type_list->weight=700;
            if (LocaleCompare(token,"normal") == 0)
              type_list->weight=400;
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
  if (type_list == (TypeInfo *) NULL)
    return(False);
  while (type_list->previous != (TypeInfo *) NULL)
    type_list=type_list->previous;
  return(True);
}
