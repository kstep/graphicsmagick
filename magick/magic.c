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
%                    M   M   AAA    GGGG  IIIII   CCCC                        %
%                    MM MM  A   A  G        I    C                            %
%                    M M M  AAAAA  G GGG    I    C                            %
%                    M   M  A   A  G   G    I    C                            %
%                    M   M  A   A   GGGG  IIIII   CCCC                        %
%                                                                             %
%                                                                             %
%                    GraphicsMagick Image Magic Methods                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Bob Friesenhahn                                %
%                                 July 2000                                   %
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
#include "blob.h"
#include "magic.h"
#include "utility.h"

/*
  Define declarations.
*/
#define MagicFilename  "magic.mgk"

/*
  Declare color map.
*/
static char
  *MagicMap = (char *)
    "<?xml version=\"1.0\"?>"
    "<magicmap>"
    "  <magic stealth=\"True\" />"
    "</magicmap>";

/*
  Static declarations.
*/
static SemaphoreInfo
  *magic_semaphore = (SemaphoreInfo *) NULL;

static MagicInfo
  *magic_list = (MagicInfo *) NULL;

/*
  Forward declarations.
*/
static unsigned int
  ReadConfigureFile(const char *,const unsigned long,ExceptionInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y M a g i c I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyMagicInfo deallocates memory associated with the magic list.
%
%  The format of the DestroyMagicInfo method is:
%
%      DestroyMagicInfo(void)
%
%
*/
MagickExport void DestroyMagicInfo(void)
{
  MagicInfo
    *magic_info;

  register MagicInfo
    *p;

  AcquireSemaphoreInfo(&magic_semaphore);
  for (p=magic_list; p != (MagicInfo *) NULL; )
  {
    magic_info=p;
    p=p->next;
    if (magic_info->path != (char *) NULL)
      LiberateMemory((void **) &magic_info->path);
    if (magic_info->name != (char *) NULL)
      LiberateMemory((void **) &magic_info->name);
    if (magic_info->target != (char *) NULL)
      LiberateMemory((void **) &magic_info->target);
    if (magic_info->magic != (unsigned char *) NULL)
      LiberateMemory((void **) &magic_info->magic);
    LiberateMemory((void **) &magic_info);
  }
  magic_list=(MagicInfo *) NULL;
  DestroySemaphoreInfo(&magic_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t M a g i c I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetMagicInfo searches the magic list for the specified name and if
%  found returns attributes for that magic.
%
%  The format of the GetMagicInfo method is:
%
%      const MagicInfo *GetMagicInfo(const unsigned char *magic,
%        const size_t length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o magic_info: Method GetMagicInfo searches the magic list for any image
%      format tag that matches the specified image signature and if found
%      returns attributes for that image format.
%
%    o magic: A binary string generally representing the first few characters
%      of the image file or blob.
%
%    o length: The length of the binary signature.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const MagicInfo *GetMagicInfo(const unsigned char *magic,
  const size_t length,ExceptionInfo *exception)
{
  register MagicInfo
    *p;

  if (magic_list == (MagicInfo *) NULL)
    {
      AcquireSemaphoreInfo(&magic_semaphore);
      if (magic_list == (MagicInfo *) NULL)
        (void) ReadConfigureFile(MagicFilename,0,exception);
      LiberateSemaphoreInfo(&magic_semaphore);
    }
  if ((magic == (const unsigned char *) NULL) || (length == 0))
    return((const MagicInfo *) magic_list);
  /*
    Search for requested magic.
  */
  AcquireSemaphoreInfo(&magic_semaphore);
  for (p=magic_list; p != (MagicInfo *) NULL; p=p->next)
    if (memcmp(magic+p->offset,p->magic,p->length) == 0)
      break;
  if (p != (MagicInfo *) NULL)
    if (p != magic_list)
      {
        /*
          Self-adjusting list.
        */
        if (p->previous != (MagicInfo *) NULL)
          p->previous->next=p->next;
        if (p->next != (MagicInfo *) NULL)
          p->next->previous=p->previous;
        p->previous=(MagicInfo *) NULL;
        p->next=magic_list;
        magic_list->previous=p;
        magic_list=p;
      }
  LiberateSemaphoreInfo(&magic_semaphore);
  return((const MagicInfo *) p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M a g i c I n f o                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListMagicInfo lists the magic info to a file.
%
%  The format of the ListMagicInfo method is:
%
%      unsigned int ListMagicInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListMagicInfo(FILE *file,ExceptionInfo *exception)
{
  register const MagicInfo
    *p;

  register long
    i;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) GetMagicInfo((unsigned char *) NULL,0,exception);
  AcquireSemaphoreInfo(&magic_semaphore);
  for (p=magic_list; p != (MagicInfo *) NULL; p=p->next)
  {
    if ((p->previous == (MagicInfo *) NULL) ||
        (LocaleCompare(p->path,p->previous->path) != 0))
      {
        if (p->previous != (MagicInfo *) NULL)
          (void) fprintf(file,"\n");
        if (p->path != (char *) NULL)
          (void) fprintf(file,"Path: %.1024s\n\n",p->path);
        (void) fprintf(file,"Name      Offset Target\n");
        (void) fprintf(file,"-------------------------------------------------"
          "------------------------------\n");
      }
    if (p->stealth)
      continue;
    (void) fprintf(file,"%.1024s",p->name);
    for (i=(long) strlen(p->name); i <= 9; i++)
      (void) fprintf(file," ");
    (void) fprintf(file,"%6ld ",p->offset);
    if (p->target != (char *) NULL)
      (void) fprintf(file,"%.1024s",p->target);
    (void) fprintf(file,"\n");
  }
  (void) fflush(file);
  LiberateSemaphoreInfo(&magic_semaphore);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r e F i l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadConfigureFile reads the color configuration file which maps
%  color strings with a particular image format.
%
%  The format of the ReadConfigureFile method is:
%
%      unsigned int ReadConfigureFile(const char *basename,
%        const unsigned long depth,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method ReadConfigureFile returns True if at least one color
%      is defined otherwise False.
%
%    o basename:  The color configuration filename.
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
    Read the magic configure file.
  */
  (void) strcpy(path,basename);
  if (depth == 0)
    xml=(char *) GetConfigureBlob(basename,path,&length,exception);
  else
    xml=(char *) FileToBlob(basename,&length,exception);
  if (xml == (char *) NULL)
    xml=AllocateString(MagicMap);
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
              if (magic_list != (MagicInfo *) NULL)
                while (magic_list->next != (MagicInfo *) NULL)
                  magic_list=magic_list->next;
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<magic") == 0)
      {
        MagicInfo
          *magic_info;

        /*
          Allocate memory for the magic list.
        */
        magic_info=(MagicInfo *) AcquireMemory(sizeof(MagicInfo));
        if (magic_info == (MagicInfo *) NULL)
          MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
            "UnableToAllocateMagicInfo");
        (void) memset(magic_info,0,sizeof(MagicInfo));
        magic_info->path=AcquireString(path);
        magic_info->signature=MagickSignature;
        if (magic_list == (MagicInfo *) NULL)
          {
            magic_list=magic_info;
            continue;
          }
        magic_list->next=magic_info;
        magic_info->previous=magic_list;
        magic_list=magic_list->next;
        continue;
      }
    if (magic_list == (MagicInfo *) NULL)
      continue;
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword)
    {
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            magic_list->name=AcquireString(token);
            break;
          }
        break;
      }
      case 'O':
      case 'o':
      {
        if (LocaleCompare((char *) keyword,"offset") == 0)
          {
            magic_list->offset=atol(token);
            break;
          }
        break;
      }
      case 'S':
      case 's':
      {
        if (LocaleCompare((char *) keyword,"stealth") == 0)
          {
            magic_list->stealth=LocaleCompare(token,"True") == 0;
            break;
          }
        break;
      }
      case 'T':
      case 't':
      {
        if (LocaleCompare((char *) keyword,"target") == 0)
          {
            const char
              *p;

            register unsigned char
              *q;

            magic_list->target=AcquireString(token);
            magic_list->magic=(unsigned char *) AllocateString(token);
            q=magic_list->magic;
            for (p=magic_list->target; *p != '\0'; )
            {
              if (*p == '\\')
                {
                  p++;
                  if (isdigit((int) *p))
                    {
                      char
                        *end;

                      *q++=(unsigned char) strtol(p,&end,8);
                      p+=(end-p);
                      magic_list->length++;
                      continue;
                    }
                  switch (*p)
                  {
                    case 'b': *q='\b'; break;
                    case 'f': *q='\f'; break;
                    case 'n': *q='\n'; break;
                    case 'r': *q='\r'; break;
                    case 't': *q='\t'; break;
                    case 'v': *q='\v'; break;
                    case 'a': *q='a'; break;
                    case '?': *q='\?'; break;
                    default: *q=(*p); break;
                  }
                  p++;
                  q++;
                  magic_list->length++;
                  continue;
                }
              *q++=(*p++);
              magic_list->length++;
            }
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
  if (magic_list == (MagicInfo *) NULL)
    return(False);
  while (magic_list->previous != (MagicInfo *) NULL)
    magic_list=magic_list->previous;
  return(True);
}
