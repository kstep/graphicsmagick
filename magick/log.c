/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                             L       OOO    GGGG                             %
%                             L      O   O  G                                 %
%                             L      O   O  G GG                              %
%                             L      O   O  G   G                             %
%                             LLLLL   OOO    GGG                              %
%                                                                             %
%                                                                             %
%                          Log GraphicsMagick Events                          %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                September 2002                               %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of GraphicsMagick,     %
%  and to permit persons to whom GraphicsMagick is furnished to do so,        %
%  subject to the following conditions:                                       %
%                                                                             %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.                   %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of                 %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,            %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick       %
%  or the use or other dealings in GraphicsMagick.                            %
%                                                                             %
%  Except as contained in this notice, the name of the GraphicsMagick         %
%  Group shall not be used in advertising or otherwise to promote the         %
%  sale, use or other dealings in GraphicsMagick without prior written        %
%  authorization from the GraphicsMagick Group.                               %
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
#include "utility.h"

/*
  Define declarations.
*/
#define LogFilename  "log.mgk"

/*
  Typedef declarations.
*/
typedef struct _LogInfo
{
  const char
    *path,
    *filename;

  unsigned long
    generations,
    limit;

  const char
    *format;

  FILE
    *file;

  unsigned long
    events,
    generation,
    count;

  TimerInfo
    timer;
} LogInfo;

/*
  Declare color map.
*/
static char
  *MagickLog = (char *)
    "<?xml version=\"1.0\"?>"
    "<magicklog>"
    "  <log events=\"None\" />"
    "  <log output=\"stdout\" />"
    "  <log filename=\"Magick-%d.log\" />"
    "  <log generations=\"3\" />"
    "  <log limit=\"2000\" />"
    "  <log format=\"%t %r %u %p %m/%f/%l/%d:\n  %e\" />"
    "</magicklog>";

/*
  Static declarations.
*/
static unsigned int
  log_initialize = True;

static LogInfo
  *log_info = (LogInfo *) NULL;

static SemaphoreInfo
  *log_semaphore = (SemaphoreInfo *) NULL;

/*
  Forward declarations.
*/
static unsigned int
  IsLogAccessible(const char *),
  ReadConfigureFile(const char *,const unsigned long,ExceptionInfo *);

static void
  *LogToBlob(const char *,size_t *,ExceptionInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y L o g I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyLogInfo deallocates memory associated with the log list.
%
%  The format of the DestroyLogInfo method is:
%
%      DestroyLogInfo(void)
%
%
*/
MagickExport void DestroyLogInfo(void)
{
  AcquireSemaphoreInfo(&log_semaphore);
  if (log_info == (LogInfo *) NULL)
    {
      DestroySemaphoreInfo(&log_semaphore);
      return;
    }
  if (log_info->file != (FILE *) NULL)
    if ((log_info->file != stdout) && (log_info->file != stderr))
      {
        (void) fprintf(log_info->file,"</log>\n");
        (void) fclose(log_info->file);
      }
  if (log_info->filename != (char *) NULL)
    LiberateMemory((void **) &log_info->filename);
  if (log_info->path != (char *) NULL)
    LiberateMemory((void **) &log_info->path);
  if (log_info->format != (char *) NULL)
    LiberateMemory((void **) &log_info->format);
  LiberateMemory((void **) &log_info);
  DestroySemaphoreInfo(&log_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  G e t L o g B l o b                                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetLogBlob() returns the specified configure file as a blob.
%
%  The format of the GetLogBlob method is:
%
%      void *GetLogBlob(const char *filename,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: The configure file name.
%
%    o path: return the full path information of the configure file.
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

static void *GetLogBlob(const char *filename,char *path,size_t *length,
  ExceptionInfo *exception)
{
  assert(filename != (const char *) NULL);
  assert(path != (char *) NULL);
  assert(length != (size_t *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  (void) strncpy(path,filename,MaxTextExtent-1);
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
        return(LogToBlob(path,length,exception));
      }
  }
#endif
#if defined(MagickLibPath)
  /*
    Search hard coded paths.
  */
  FormatString(path,"%.1024s%.1024s",MagickLibPath,filename);
  return(LogToBlob(path,length,exception));
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
      if (IsLogAccessible(path))
        return(LogToBlob(path,length,exception));
    }
  if (getenv("MAGICK_HOME") != (char *) NULL)
    {
      /*
        Search MAGICK_HOME.
      */
#if defined(POSIX)
      FormatString(path,"%.1024s/lib/%s/%.1024s",getenv("MAGICK_HOME"),
        MagickLibSubdir,filename);
#else
      FormatString(path,"%.1024s%s%.1024s",getenv("MAGICK_HOME"),
        DirectorySeparator,filename);
#endif
      if (IsLogAccessible(path))
        return(LogToBlob(path,length,exception));
    }
  if (getenv("HOME") != (char *) NULL)
    {
      /*
        Search $HOME/.magick.
      */
      FormatString(path,"%.1024s%s%s%.1024s",getenv("HOME"),
        *getenv("HOME") == '/' ? "/.magick" : "",DirectorySeparator,filename);
      if (IsLogAccessible(path))
        return(LogToBlob(path,length,exception));
    }
  /*
    Search current directory.
  */
  (void) strncpy(path,filename,MaxTextExtent-1);
  if (IsLogAccessible(path))
    return(LogToBlob(path,length,exception));
#if defined(WIN32)
  return(NTResourceToBlob(filename));
#endif
#endif
  return((void *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s E v e n t L o g g i n g                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsEventLogging() returns True if logging of events is enabled otherwise
%  False.
%
%  The format of the IsEventLogging method is:
%
%      unsigned int IsEventLogging(void)
%
%
*/
MagickExport unsigned int IsEventLogging(void)
{
  unsigned int
    status;

  if (log_info == (LogInfo *) NULL)
    {
      AcquireSemaphoreInfo(&log_semaphore);
      if (log_initialize == True)
      {
        log_initialize=False;
        if (log_info == (LogInfo *) NULL)
          {
            ExceptionInfo
              exception;

            GetExceptionInfo(&exception);
            (void) ReadConfigureFile(LogFilename,0,&exception);
            DestroyExceptionInfo(&exception);
          }
      }
      LiberateSemaphoreInfo(&log_semaphore);
    }
  if (log_info == (LogInfo *) NULL)
    return(False);
  status=log_info->events != NoEvents;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  I s L o g A c c e s s i b l e                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsLogAccessible() returns True if the file as defined by filename is
%  accessible.
%
%  The format of the IsLogAccessible method is:
%
%      unsigned int IsLogAccessible(const char *filename)
%
%  A description of each parameter follows.
%
%    o status:  IsLogAccessible() returns True is the file as defined by
%      filename is accessible, otherwise False is returned.
%
%    o filename:  Specifies a pointer to an array of characters.  The unique
%      file name is returned in this array.
%
%
*/
static unsigned int IsLogAccessible(const char *filename)
{
  int
    status;

  struct stat
    file_info;

  if ((filename == (const char *) NULL) || (*filename == '\0'))
    return(False);
  status=stat(filename,&file_info);
  if (status != 0)
    return(False);
  return(S_ISREG(file_info.st_mode));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   L o g T o B l o b                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LogToBlob() returns the contents of a file as a blob.  It returns the
%  file as a blob and its length.  If an error occurs, NULL is returned.
%
%  The format of the LogToBlob method is:
%
%      void *LogToBlob(const char *filename,size_t *length,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blob:  LogToBlob() returns the contents of a file as a blob.  If
%      an error occurs NULL is returned.
%
%    o filename: The filename.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static void *LogToBlob(const char *filename,size_t *length,
  ExceptionInfo *exception)
{
  int
    file;

  ExtendedSignedIntegralType
    offset;

  unsigned char
    *blob;

  void
    *map;

  assert(filename != (const char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  SetExceptionInfo(exception,UndefinedException);
  file=open(filename,O_RDONLY | O_BINARY,0777);
  if (file == -1)
    return((void *) NULL);
  offset=MagickSeek(file,0,SEEK_END);
  if ((offset < 0) || (offset != (size_t) offset))
    {
      (void) close(file);
      return((void *) NULL);
    }
  *length=(size_t) offset;
  blob=(unsigned char *) AcquireMemory(*length+1);
  if (blob == (unsigned char *) NULL)
    {
      (void) close(file);
      return((void *) NULL);
    }
  map=MapBlob(file,ReadMode,0,*length);
  if (map != (void *) NULL)
    {
      (void) memcpy(blob,map,*length);
      UnmapBlob(map,*length);
    }
  else
    {
      ExtendedSignedIntegralType
        count;

      register size_t
        i;

      count=0;
      for (i=0; i < *length; i+=count)
      {
        count=read(file,blob+i,*length-i);
        if (count <= 0)
          break;
      }
      if (i < *length)
        {
          (void) close(file);
          LiberateMemory((void **) &blob);
          return((void *) NULL);
        }
    }
  blob[*length]='\0';
  (void) close(file);
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o g M a g i c k E v e n t                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LogMagickEvent() logs an event as determined by the log configuration file.
%  If an error occurs, False is returned otherwise True.
%
%  The format of the LogMagickEvent method is:
%
%      unsigned int LogMagickEvent(const LogEventType type,const char *module,
%        const char *function,const unsigned long line,const char *format,...)
%
%  A description of each parameter follows:
%
%    o type: The event type.
%
%    o filename: The source module filename.
%
%    o function: The function name.
%
%    o line: The line number of the source module.
%
%    o format: The output format.
%
%
*/
MagickExport unsigned int LogMagickEvent(const LogEventType type,
  const char *module,const char *function,const unsigned long line,
  const char *format,...)
{
  char
    *domain,
    event[MaxTextExtent],
    timestamp[MaxTextExtent];

  double
    elapsed_time,
    user_time;

  register const char
    *p;

  struct tm
    *time_meridian;

  time_t
    seconds;

  va_list
    operands;

  if (!IsEventLogging())
    return(False);
  if (!(log_info->events & type))
    return(True);
  AcquireSemaphoreInfo(&log_semaphore);
  switch (type)
  {
    case ConfigureEvent: domain=(char *) "Configure"; break;
    case AnnotateEvent: domain=(char *) "Annotate"; break;
    case RenderEvent: domain=(char *) "Render"; break;
    case TransformEvent: domain=(char *) "Transform"; break;
    case LocaleEvent: domain=(char *) "Locale"; break;
    case CoderEvent: domain=(char *) "Coder"; break;
    case X11Event: domain=(char *) "X11"; break;
    case CacheEvent: domain=(char *) "Cache"; break;
    case BlobEvent: domain=(char *) "Blob"; break;
    case DeprecateEvent: domain=(char *) "Deprecate"; break;
    case UserEvent: domain=(char *) "User"; break;
    default: domain=(char *) "UnknownEvent"; break;
  }
  va_start(operands,format);
#if defined(HAVE_VSNPRINTF)
  (void) vsnprintf(event,MaxTextExtent,format,operands);
#else
#  if defined(HAVE_VSPRINTF)
  (void) vsprintf(event,format,operands);
#  else
#    error Neither vsnprintf or vsprintf is available.
#  endif
#endif
  va_end(operands);
  seconds=time((time_t *) NULL);
  time_meridian=localtime(&seconds);
  elapsed_time=GetElapsedTime(&log_info->timer);
  user_time=GetUserTime(&log_info->timer);
  if ((log_info->file != stdout) && (log_info->file != stderr))
    {
      /*
        Log to a file in the XML format.
      */
      log_info->count++;
      if (log_info->count == log_info->limit)
        {
          (void) fprintf(log_info->file,"</log>\n");
          (void) fclose(log_info->file);
          log_info->file=(FILE *) NULL;
        }
      if (log_info->file == (FILE *) NULL)
        {
          char
            filename[MaxTextExtent];

          FormatString(filename,log_info->filename,
            log_info->generation % log_info->generations);
          log_info->file=fopen(filename,"w");
          if (log_info->file == (FILE *) NULL)
            return(False);
          log_info->generation++;
          (void) fprintf(log_info->file,"<?xml version=\"1.0\"?>\n");
          (void) fprintf(log_info->file,"<log>\n");
        }
      FormatString(timestamp,"%04d%02d%02d%02d%02d%02d",time_meridian->tm_year+
        1900,time_meridian->tm_mon+1,time_meridian->tm_mday,
        time_meridian->tm_hour,time_meridian->tm_min,time_meridian->tm_sec);
      (void) fprintf(log_info->file,"<record>\n");
      (void) fprintf(log_info->file,"  <timestamp>%.1024s</timestamp>\n",
        timestamp);
      (void) fprintf(log_info->file,
        "  <elapsed-time>%ld:%02ld</elapsed-time>\n",
        (long) (elapsed_time/60.0),(long) ceil(fmod(elapsed_time,60.0)));
      (void) fprintf(log_info->file,"  <user-time>%0.3f</user-time>\n",
        user_time);
      (void) fprintf(log_info->file,"  <pid>%ld</pid>\n",(long) getpid());
      (void) fprintf(log_info->file,"  <module>%.1024s</module>\n",module);
      (void) fprintf(log_info->file,"  <function>%.1024s</function>\n",
        function);
      (void) fprintf(log_info->file,"  <line>%lu</line>\n",line);
      (void) fprintf(log_info->file,"  <domain>%.1024s</domain>\n",domain);
      (void) fprintf(log_info->file,"  <event>%.1024s</event>\n",event);
      (void) fprintf(log_info->file,"</record>\n");
      (void) fflush(log_info->file);
      ContinueTimer((TimerInfo *) &log_info->timer);
      LiberateSemaphoreInfo(&log_semaphore);
      return(True);
    }
  /*
    Log to stdout in a "human readable" format.
  */
  for (p=log_info->format; *p != '\0'; p++)
  {
    /*
      Process formatting characters in text.
    */
    if ((*p == '\\') && (*(p+1) == 'r'))
      {
        (void) fprintf(stdout,"\r");
        p++;
        continue;
      }
    if ((*p == '\\') && (*(p+1) == 'n'))
      {
        (void) fprintf(stdout,"\n");
        p++;
        continue;
      }
    if (*p != '%')
      {
        (void) fprintf(stdout,"%c",*p);
        continue;
      }
    p++;
    switch (*p)
    {
      case 'd':
      {
        (void) fprintf(stdout,"%.1024s",domain);
        break;
      }
      case 'e':
      {
        (void) fprintf(stdout,"%.1024s",event);
        break;
      }
      case 'f':
      {
        (void) fprintf(stdout,"%.1024s",function);
        break;
      }
      case 'l':
      {
        (void) fprintf(stdout,"%lu",line);
        break;
      }
      case 'm':
      {
        register const char
          *p;

        for (p=module+strlen(module)-1; p > module; p--)
          if (*p == *DirectorySeparator)
            {
              p++;
              break;
            }
        (void) fprintf(stdout,"%.1024s",p);
        break;
      }
      case 'p':
      {
        (void) fprintf(stdout,"%ld",(long) getpid());
        break;
      }
      case 'r':
      {
        (void) fprintf(stdout,"%ld:%02ld",(long) (elapsed_time/60.0),
          (long) ceil(fmod(elapsed_time,60.0)));
        break;
      }
      case 't':
      {
        (void) fprintf(stdout,"%02d:%02d:%02d",time_meridian->tm_hour,
          time_meridian->tm_min,time_meridian->tm_sec);
        break;
      }
      case 'u':
      {
        (void) fprintf(stdout,"%0.3fu",user_time);
        break;
      }
      default:
      {
        (void) fprintf(stdout,"%%");
        (void) fprintf(stdout,"%c",*p);
        break;
      }
    }
  }
  (void) fprintf(stdout,"\n");
  ContinueTimer((TimerInfo *) &log_info->timer);
  LiberateSemaphoreInfo(&log_semaphore);
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
%  ReadConfigureFile() reads the log configuration file.
%
%  The format of the ReadConfigureFile method is:
%
%      unsigned int ReadConfigureFile(const char *basename,
%        const unsigned long depth,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: ReadConfigureFile() returns True if at least one log entry
%      is defined otherwise False.
%
%    o basename:  The log configuration filename.
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
    length=0;

  /*
    Read the log configure file.
  */
  (void) strcpy(path,basename);
  if (depth == 0)
    xml=(char *) GetLogBlob(basename,path,&length,exception);
  else
    xml=(char *) LogToBlob(basename,&length,exception);
  if (xml == (char *) NULL)
    xml=AllocateString(MagickLog);
  token=AllocateString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret Coder.
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
                (void) fprintf(stderr,"%.1024s: <include /> nested too deeply",
                  path);
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
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<magicklog>") == 0)
      {
        /*
          Allocate memory for the log list.
        */
        log_info=(LogInfo *) AcquireMemory(sizeof(LogInfo));
        if (log_info == (LogInfo *) NULL)
          MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
            "UnableToAllocateLogInfo");
        (void) memset((void *) log_info,0,sizeof(LogInfo));
        log_info->path=AcquireString(path);
        GetTimerInfo((TimerInfo *) &log_info->timer);
        continue;
      }
    if (log_info == (LogInfo *) NULL)
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
        if (LocaleCompare((char *) keyword,"events") == 0)
          {
            if (GlobExpression(token,"*[Aa]ll*"))
              log_info->events=AllEvents;
            if (GlobExpression(token,"*[Aa]nnotate*"))
              log_info->events|=AnnotateEvent;
            if (GlobExpression(token,"*[Bb]lob*"))
              log_info->events|=BlobEvent;
            if (GlobExpression(token,"*[Cc]oder*"))
              log_info->events|=CoderEvent;
            if (GlobExpression(token,"*[Cc]onfigure*"))
              log_info->events|=ConfigureEvent;
            if (GlobExpression(token,"*[Dd]precate*"))
              log_info->events|=DeprecateEvent;
            if (GlobExpression(token,"*[Ll]ocale*"))
              log_info->events|=LocaleEvent;
            if (GlobExpression(token,"*[Nn]one*"))
              log_info->events=NoEvents;
            if (GlobExpression(token,"*[Rr]ender*"))
              log_info->events|=RenderEvent;
            if (GlobExpression(token,"*[Tt]ransform*"))
              log_info->events|=TransformEvent;
            if (GlobExpression(token,"*[U]ser*"))
              log_info->events|=UserEvent;
            if (GlobExpression(token,"*[X]11*"))
              log_info->events|=X11Event;
            break;
          }
        break;
      }
      case 'F':
      case 'f':
      {
        if (LocaleCompare((char *) keyword,"filename") == 0)
          {
            log_info->filename=AcquireString(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"format") == 0)
          {
            log_info->format=AcquireString(token);
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        if (LocaleCompare((char *) keyword,"generations") == 0)
          {
            log_info->generations=atol(token);
            break;
          }
        break;
      }
      case 'L':
      case 'l':
      {
        if (LocaleCompare((char *) keyword,"limit") == 0)
          {
            log_info->limit=atol(token);
            break;
          }
        break;
      }
      case 'O':
      case 'o':
      {
        if (LocaleCompare((char *) keyword,"output") == 0)
          {
            if (LocaleCompare(token,"stderr") == 0)
              {
                log_info->file=stderr;
                break;
              }
            if (LocaleCompare(token,"stdout") == 0)
              {
                log_info->file=stdout;
                break;
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
  if (log_info == (LogInfo *) NULL)
    return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t L o g E v e n t M a s k                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetLogEventMask() accepts a list that determines which events to log.  All
%  other events are ignored.  By default, no logging is enabled.  This method
%  returns the previous log event mask.
%
%  The format of the AcquireString method is:
%
%      unsigned long SetLogEventMask(const char *events)
%
%  A description of each parameter follows:
%
%    o events: log these events.
%
%
*/
MagickExport unsigned long SetLogEventMask(const char *events)
{
  AcquireSemaphoreInfo(&log_semaphore);
  if (log_info == (LogInfo *) NULL)
    {
      ExceptionInfo
        exception;

      GetExceptionInfo(&exception);
      (void) ReadConfigureFile(LogFilename,0,&exception);
      DestroyExceptionInfo(&exception);
    }
  if (log_info == (LogInfo *) NULL)
    {
      LiberateSemaphoreInfo(&log_semaphore);
      return(NoEvents);
    }
  log_info->events=NoEvents;
  if (events == (const char *) NULL)
    {
      LiberateSemaphoreInfo(&log_semaphore);
      return(log_info->events);
    }
  if (GlobExpression(events,"*[Aa]ll*"))
    log_info->events=AllEvents;
  if (GlobExpression(events,"*[Aa]nnotate*"))
    log_info->events|=AnnotateEvent;
  if (GlobExpression(events,"*[Bb]lob*"))
    log_info->events|=BlobEvent;
  if (GlobExpression(events,"*[Cc]ache*"))
    log_info->events|=CacheEvent;
  if (GlobExpression(events,"*[Cc]oder*"))
    log_info->events|=CoderEvent;
  if (GlobExpression(events,"*[Cc]onfigure*"))
    log_info->events|=ConfigureEvent;
  if (GlobExpression(events,"*[Dd]eprecate*"))
    log_info->events|=DeprecateEvent;
  if (GlobExpression(events,"*[Ll]ocale*"))
    log_info->events|=LocaleEvent;
  if (GlobExpression(events,"*[Nn]one*"))
    log_info->events=NoEvents;
  if (GlobExpression(events,"*[Rr]ender*"))
    log_info->events|=RenderEvent;
  if (GlobExpression(events,"*[Tt]ransform*"))
    log_info->events|=TransformEvent;
  if (GlobExpression(events,"*[U]ser*"))
    log_info->events|=UserEvent;
  if (GlobExpression(events,"*[X]11*"))
    log_info->events|=X11Event;
  LiberateSemaphoreInfo(&log_semaphore);
  return(log_info->events);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t L o g F o r m a t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetLogFormat() sets the format for the "human readable" log record.
%
%  The format of the LogMagickFormat method is:
%
%      SetLogFormat(const char *format)
%
%  A description of each parameter follows:
%
%    o format: The log record format.
%
%
*/
MagickExport void SetLogFormat(const char *format)
{
  AcquireSemaphoreInfo(&log_semaphore);
  if (log_info->format != (char *) NULL)
    LiberateMemory((void **) &log_info->format);
  log_info->format=AcquireString(format);
  LiberateSemaphoreInfo(&log_semaphore);
}
