/*
  ImageMagick defines.
*/
#ifndef _DEFINES_H
#define _DEFINES_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Review these machine specific definitions.
*/
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#define ApplicationDefaults  "/usr/X11R6/lib/X11/app-defaults/"
#if !defined(DelegatePath)
#define DelegatePath  "/usr/local/share/ImageMagick/"
#endif
#define DirectorySeparator  "/"
#define DirectoryListSeparator  ':'
#define EditorOptions  " -title \"Edit Image Comment\" -e vi"
#define Exit  exit
#define IsBasenameSeparator(c)  ((c) == '/')
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL) || \
   (strchr(text,'[') != (char *) NULL) || \
   (strchr(text,']') != (char *) NULL))
#define PreferencesDefaults  "~/."
#define ProcessPendingEvents(text)
#define ReadCommandlLine(argc,argv)
#if !defined(RGBColorDatabase)
#define RGBColorDatabase  "/usr/X11R6/lib/X11/rgb.txt"
#endif
#define SetNotifyHandlers
#define TemporaryTemplate  "magick"
#else
#if defined(vms)
#define ApplicationDefaults  "decw$system_defaults:"
#define DelegatePath  "sys$login:"
#define DirectorySeparator  ""
#define DirectoryListSeparator  ';'
#define EditorOptions  ""
#define Exit(errno)  errno ? exit(-1) : exit(0)
#define IsBasenameSeparator(c)  ((c) == ']')
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL))
#define j1(x)  x
#define PreferencesDefaults  "decw$user_defaults:"
#define ProcessPendingEvents(text)
#define ReadCommandlLine(argc,argv)
#if !defined(RGBColorDatabase)
#define RGBColorDatabase  "sys$common:[sysmgr]decw$rgb.dat"
#endif
#define SetNotifyHandlers
#endif
#if defined(macintosh)
#define ApplicationDefaults  "/usr/lib/X11/app-defaults/"
#define DelegatePath  ""
#define DirectorySeparator  ":"
#define DirectoryListSeparator  ';'
#define EditorOptions ""
#define IsBasenameSeparator(c)  ((c) == ':')
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL) || \
   (strchr(text,'[') != (char *) NULL) || \
   (strchr(text,']') != (char *) NULL))
#define j1(x)  x
#define PreferencesDefaults  "~/."
#define ReadCommandlLine(argc,argv)  argc=ccommand(argv); puts(MagickVersion);
#if !defined(RGBColorDatabase)
#define RGBColorDatabase  "../xlib/lib/X11/rgb.txt"
#endif
#define SetNotifyHandlers \
  SetErrorHandler(MACErrorHandler); \
  SetWarningHandler(MACWarningHandler)
#endif
#if defined(WIN32)
#define ApplicationDefaults  "c:\\ImageMagick\\"
#define DelegatePath  "c:\\ImageMagick\\"
#define DirectorySeparator  "\\"
#define DirectoryListSeparator  ';'
#define EditorOptions ""
#undef isatty
#define isatty(filedes)  1
#define IsBasenameSeparator(c)  (((c) == '/') || ((c) == '\\'))
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL) || \
   (strchr(text,'[') != (char *) NULL) || \
   (strchr(text,']') != (char *) NULL))
#define j1(x)  x
#define PreferencesDefaults  "~/."
#define ProcessPendingEvents(text)
#define ReadCommandlLine(argc,argv)
#if !defined(RGBColorDatabase)
#define RGBColorDatabase  "c:\\ImageMagick\\rgb.txt"
#endif
#define SetNotifyHandlers \
  SetErrorHandler(NTErrorHandler); \
  SetWarningHandler(NTWarningHandler)
#undef sleep
#define sleep(seconds)  Sleep(seconds*1000)
#endif
#endif

/*
  Define declarations.
*/
#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))
#define ColorMatch(color,target,distance) \
  (((((int) (color).red-(int) (target).red)* \
     ((int) (color).red-(int) (target).red))+ \
    (((int) (color).green-(int) (target).green)* \
     ((int) (color).green-(int) (target).green))+ \
    (((int) (color).blue-(int) (target).blue)* \
     ((int) (color).blue-(int) (target).blue))) <= (distance*distance))
#define DownShift(x) (((int) ((x)+(1L << 13))) >> 14)
#define Extent(string)  ((int) strlen(string))
#define False  0
#define DegreesToRadians(x) ((x)*M_PI/180.0)
#define Intensity(color)  \
  ((unsigned int) ((color).red*77+(color).green*150+(color).blue*29) >> 8)
#define IsFaxImage(color)  \
  (IsMonochromeImage(image) && ((image)->columns <= 2560))
#define IsGray(color)  \
  (((color).red == (color).green) && ((color).green == (color).blue))
#define Max(x,y)  (((x) > (y)) ? (x) : (y))
#define Min(x,y)  (((x) < (y)) ? (x) : (y))
#if !defined(M_PI)
#define M_PI  3.14159265358979323846
#endif
#define Opaque  MaxRGB
#define PixelOffset(image,x,y) \
  ((image)->pixels+(((int) (y))*(image)->columns+((int) (x))))
#define QuantumTick(i,span) \
  (((~((span)-i-1) & ((span)-i-2))+1) == ((span)-i-1))
#define RadiansToDegrees(x) ((x)*180/M_PI)
#define ReadQuantum(quantum,p)  \
{  \
  if (image->depth <= 8) \
    quantum=UpScale(*p++); \
  else \
    { \
      value=(*p++) << 8;  \
      value|=(*p++);  \
      quantum=value >> (image->depth-QuantumDepth); \
    } \
}
#define ReadQuantumFile(quantum)  \
{  \
  if (image->depth <= 8) \
    quantum=UpScale(fgetc(image->file)); \
  else \
    quantum=MSBFirstReadShort(image->file) >> (image->depth-QuantumDepth); \
}
#define Swap(x,y) ((x)^=(y), (y)^=(x), (x)^=(y))
#if !defined(STDIN_FILENO)
#define STDIN_FILENO  0
#endif
#define TextWidth(font,text,length)  XTextWidth(font,(char *) (text),length)
#define Transparent  0
#define True  1
#define UpShift(x) ((int) (x) << 14)
#define UpShifted(x) ((int) ((x)*(1L << 14)+0.5))
#define WriteQuantum(quantum,q)  \
{  \
  if (image->depth <= 8) \
    *q++=DownScale(quantum); \
  else \
    { \
      value=(quantum); \
      if ((QuantumDepth-image->depth) > 0) \
        value*=257; \
      *q++=value >> 8; \
      *q++=value; \
    } \
}
#define WriteQuantumFile(quantum)  \
{  \
  if (image->depth <= 8) \
    (void) fputc(DownScale(quantum),image->file); \
  else \
    if ((QuantumDepth-image->depth) > 0) \
      MSBFirstWriteShort((quantum)*257,image->file); \
    else \
      MSBFirstWriteShort(quantum,image->file); \
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
