/*
  ImageMagick defines.
*/
#ifndef _DEFINES_H
#define _DEFINES_H

/*
  Review these platform specific definitions.
*/
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#if !defined(ApplicationDefaults)
#define ApplicationDefaults  "/usr/X11R6/lib/X11/app-defaults/"
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
#if !defined(MagickConfigurePath)
#define MagickConfigurePath  "/usr/local/share/ImageMagick/"
#endif
#if !defined(MagickModulesPath)
#define MagickModulesPath  "/usr/local/lib/ImageMagick/modules/"
#endif
#define PreferencesDefaults  "~/."
#define ProcessPendingEvents(text)
#define ReadCommandlLine(argc,argv)
#if !defined(X11ConfigurePath)
#define X11ConfigurePath  "/usr/X11R6/lib/X11/"
#endif
#define SetNotifyHandlers
#define TemporaryTemplate  "magick"
#else

#if defined(vms)
#define ApplicationDefaults  "decw$system_defaults:"
#define DirectorySeparator  ""
#define DirectoryListSeparator  ';'
#define EditorOptions  ""
#define Exit  exit
#define IsBasenameSeparator(c)  (((c) == ']') || ((c) == ':') || ((c) == '/'))
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL))
#define j1(x)  x
#define MagickConfigurePath  "sys$login:"
#define MagickModulesPath  "sys$login:"
#define PreferencesDefaults  "decw$user_defaults:"
#define ProcessPendingEvents(text)
#define ReadCommandlLine(argc,argv)
#if !defined(X11ConfigurePath)
#define X11ConfigurePath  "sys$common:[sysmgr]rgb.dat"
#endif
#define SetNotifyHandlers
#endif

#if defined(macintosh)
#define ApplicationDefaults  "/usr/lib/X11/app-defaults/"
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
#define MagickConfigurePath  ""
#define MagickModulesPath  ""
#define PreferencesDefaults  "~/."
#define ReadCommandlLine(argc,argv)  argc=ccommand(argv); puts(MagickVersion);
#if !defined(X11ConfigurePath)
#define X11ConfigurePath  "../xlib/lib/X11/"
#endif
#define SetNotifyHandlers \
  SetErrorHandler(MACErrorHandler); \
  SetWarningHandler(MACWarningHandler)
#endif

#if defined(WIN32)
#define ApplicationDefaults  "c:\\ImageMagick\\"
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
#define MagickConfigurePath  "c:\\ImageMagick\\"
#define MagickModulesPath  "c:\\ImageMagick\\"
#define PreferencesDefaults  "~/."
#define ProcessPendingEvents(text)
#define ReadCommandlLine(argc,argv)
#if !defined(X11ConfigurePath)
#define X11ConfigurePath  "c:\\ImageMagick\\"
#endif
#define SetNotifyHandlers \
  SetErrorHandler(NTErrorHandler); \
  SetWarningHandler(NTWarningHandler)
#undef sleep
#define sleep(seconds)  Sleep(seconds*1000)
#define HAVE_TIFFCONF_H
#endif

#endif

/*
  Define declarations.
*/
#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))
#define CloseImage(image)  CloseBlob(image)
#define ColorMatch(color,target,distance) \
  (((distance) == 0) ? \
   (((color).red == (target).red) && \
    ((color).green == (target).green) && \
    ((color).blue == (target).blue)) : \
   ((((double) (color).red-(double) (target).red)* \
      ((double) (color).red-(double) (target).red))+ \
     (((double) (color).green-(double) (target).green)* \
      ((double) (color).green-(double) (target).green))+ \
     (((double) (color).blue-(double) (target).blue)* \
      ((double) (color).blue-(double) (target).blue))) <= \
      (double) (distance*distance))
#define Extent(string)  ((int) strlen(string))
#define False  0
#define DegreesToRadians(x) (M_PI*(x)/180.0)
#define Intensity(color)  \
  ((0.299*(color).red+0.587*(color).green+(0.114+MagickEpsilon)*(color).blue))
#define IsFaxImage(color) \
  (IsMonochromeImage(image) && ((image)->columns <= 2560))
#define IsGray(color)  \
  (((color).red == (color).green) && ((color).green == (color).blue))
#define MagickEpsilon  1.0e-12
#define Max(x,y)  (((x) > (y)) ? (x) : (y))
#define MaxApplicationProfiles  16
#define Min(x,y)  (((x) < (y)) ? (x) : (y))
#if !defined(M_PI)
#define M_PI  3.14159265358979323846
#endif
#define OpenImage(image_info,image,type)  OpenBlob(image,image,type)
#define QuantumTick(i,span) \
  (((~((span)-i-1) & ((span)-i-2))+1) == ((span)-i-1))
#define RadiansToDegrees(x) (180.0*(x)/M_PI)
#define RenderPostscriptText  "  Rendering postscript...  "
#define ScaleColor5to8(x)  ((x) << 3)
#define ScaleColor6to8(x)  ((x) << 2)
#define Swap(x,y) ((x)^=(y), (y)^=(x), (x)^=(y))
#define True  1

/*
  Define system symbols if not already defined.
*/
#if !defined(STDIN_FILENO)
#define STDIN_FILENO  0x00
#endif

#if !defined(O_BINARY)
#define O_BINARY  0x00
#endif

#if !defined(MAP_FAILED)
#define MAP_FAILED      ((void *) -1)
#endif

#endif
