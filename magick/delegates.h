/*
  Delegate define definitions.
*/
#ifndef _DELEGATES_H
#define _DELEGATES_H

#if !defined(BrowseDelegate)
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#define BrowseDelegate  "netscape"
#else
#if defined(vms)
#define BrowseDelegate  "mosaic"
#endif
#if defined(macintosh)
#define BrowseDelegate  "netscape"
#endif
#if defined(WIN32)
#define BrowseDelegate  "cmd /C start"
#endif
#endif
#endif
#define BrowseCommand  BrowseDelegate " %s &"
#define Win95BrowseCommand  "start %s &"
#if !defined(BZIPDelegate)
#define BZIPDelegate  "bzip2"
#endif
#define BunzipCommand  "|" BZIPDelegate " -cd %s"
#define BzipCommand  "|" BZIPDelegate " -f > %s"
#if !defined(CGMDecodeDelegate)
#define CGMDecodeDelegate  "ralcgm"
#endif
#define CGMCommand  CGMDecodeDelegate " -d ps %s %s %s"
#if !defined(EditorDelegate)
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#define EditorDelegate  "xterm -e vi"
#else
#if defined(vms)
#define EditorDelegate  "cre/term/wait edit/tpu"
#endif
#if defined(macintosh)
#define EditorDelegate  ""
#endif
#if defined(WIN32)
#define EditorDelegate  "notepad"
#endif
#endif
#endif
#define EditorCommand  EditorDelegate EditorOptions " %s"
#if !defined(FIGDecodeDelegate)
#define FIGDecodeDelegate  "fig2dev"
#endif
#define FIGCommand  FIGDecodeDelegate " -L ps %s %s"
#if !defined(HTMLDecodeDelegate)
#define HTMLDecodeDelegate  "html2ps"
#endif
#define HTMLCommand  HTMLDecodeDelegate " -o %s %s"
#if !defined(HPGLDecodeDelegate)
#define HPGLDecodeDelegate  "hp2xx"
#endif
#define HPGLCommand  HPGLDecodeDelegate " -q -m eps -f %s %s"
#if !defined(LaunchDelegate)
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#define LaunchDelegate  "gimp"
#else
#if defined(vms)
#define LaunchDelegate  "gimp"
#endif
#if defined(macintosh)
#define LaunchDelegate  "gimp"
#endif
#if defined(WIN32)
#define LaunchDelegate  "mspaint"
#endif
#endif
#endif
#define LauncherCommand  LaunchDelegate " %s"
#if !defined(LPDelegate)
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#define LPDelegate  "lp"
#else
#if defined(vms)
#define LPDelegate  "print/delete"
#endif
#if defined(macintosh)
#define LPDelegate  "print"
#endif
#if defined(WIN32)
#define LPDelegate  "print"
#endif
#endif
#endif
#define LPCommand  LPDelegate " %s"
#if !defined(LZWDecodeDelegate)
#define LZWDecodeDelegate  "uncompress"
#endif
#define LZWDecodeCommand  "|" LZWDecodeDelegate " -c %s"
#if !defined(LZWEncodeDelegate)
#define LZWEncodeDelegate  "compress"
#endif
#define LZWEncodeCommand  "|" LZWEncodeDelegate " -c > %s"
#if !defined(MPEGDecodeDelegate)
#define MPEGDecodeDelegate  "mpeg2decode"
#endif
#define MPEGDecodeCommand  MPEGDecodeDelegate " -q -b %s -f -r -o3 %s"
#if !defined(MPEGEncodeDelegate)
#define MPEGEncodeDelegate  "mpeg2encode"
#endif
#define MPEGEncodeCommand  MPEGEncodeDelegate " %s %s"
#if !defined(PGPDecodeDelegate)
#define PGPDecodeDelegate  "pgpv"
#endif
#if !defined(PGPEncodeDelegate)
#define PGPEncodeDelegate  "pgpe"
#endif
#define PGPDecodeCommand  "|" PGPDecodeDelegate " -fq %s"
#if !defined(PSDelegate)
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#define PSDelegate  "gs"
#else
#if defined(vms)
#define PSDelegate  "gs"
#endif
#if defined(macintosh)
#define PSDelegate  ""
#endif
#if defined(WIN32)
#define PSDelegate  "gswin32"
#endif
#endif
#endif
#define PSCommand  PSDelegate " -sDEVICE=%s -q -dNOPAUSE -dSAFER " \
  "-dTextAlphaBits=%u -dGraphicsAlphaBits=%u -g%s -r%s %s " \
  "-sOutputFile=\"%s\" -- \"%s\" -c quit"
#if !defined(RADDecodeDelegate)
#define RADDecodeDelegate  "ra_ppm"
#endif
#define RADCommand  RADDecodeDelegate " -g 1.0 %s %s"
#if !defined(RLEEncodeDelegate)
#define RLEEncodeDelegate  "rawtorle"
#endif
#define RLECommand  RLEEncodeDelegate " -w %u -h %u -n %u -o %s %s"
#if !defined(SCANDecodeDelegate)
#define SCANDecodeDelegate  "scanimage"
#endif
#define SCANCommand  SCANDecodeDelegate " -d %s > %s"
#if !defined(ShowImageDelegate)
#define ShowImageDelegate  "display"
#endif
#define ShowImageCommand  ShowImageDelegate \
  " %s -immutable -window_group 0x%lx -title \"%s of %s\" tmp:%s"
#if !defined(WWWDecodeDelegate)
#define WWWDecodeDelegate  "GET"
#endif
#define WWWCommand  WWWDecodeDelegate " %s:%s > %s 2>&1"
#if !defined(ZIPDelegate)
#define ZIPDelegate  "gzip"
#endif
#define ZIPDecodeCommand  "|" ZIPDelegate " -cdfq %s"
#define ZIPEncodeCommand  "|" ZIPDelegate " -cf > %s"

#endif
