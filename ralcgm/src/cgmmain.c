/*   RAL-CGM Interpreter module:  @(#) cgmmain.c  version 3.6
 *
 * Copyright (C) Rutherford Appleton Laboratory 1990, All Rights Reserved.
 *
 * This source may be copied, distributed, altered or used, but not sold
 * for profit or incorporated into a product except under licence from
 * the author.
 *
 * It is not in the public domain.
 *
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the author should be marked as
 * such.
 *
 * Original Author:
 *    R T Platon @ Rutherford Appleton Laboratory (rtp@uk.ac.rl.ib)
 *
 * Description: Main RAL-CGM Interpreter Routine
 *
 *
 * Arguments:     -x   Options - Valid parameters are:
 *                     Drivers:
 *                        b - Binary output
 *                        t - Clear text output
 *                        c - Character output
 *                        n - Native character output (IBM only)
 *                     Kept for compatibility:
 *                        i - Iris Graphics Library
 *                        p - Postscript
 *                        x - X11
 *
 *                        d xxx - Device Output
 *                          xxx =
 *                            Ps   : PostScript
 *                            Epsf : Encapsulated PostScript
 *                            Cps  : Colour PostScript
 *                            Igl  : Iris Graphics Library
 *                            Xw   : X-Windows release 11
 *                            Tek  : Tektronix 42xx
 *                            Hpgl : HPGL
 *                            Vga  : PC VGA
 *                            Win:  MiscroSoft Windows Graphics Device Interface or Metafile
 *                     To be added later
 *                            8514 : PC 8514 monitor
 *
 *                     Options:
 *                        e - remove error count limit check
 *                        f - Errors to file
 *                        v - Use verbose output mode
 *
 *                        l xxx - Use 'xxx' Profile
 *                          xxx = GKS
 *                                CALS
 *                                OLDBIN
 *                                TCP Use alternative translate table(IBM only)
 *                        g xxx - Use 'xxx' GDPs (Only GKS at present)
 *
 *                        o xxx - Driver dependent options
 *
 *                     X-Windows only:
 *                        a xxx - X windows display address
 *
 *                in    Input file name (Unix format)
 *
 *                out   Optional Output file name (Unix format)
 *                err   Optional error file name (Unix format)
 *
 * Maintenance Log:
 *   3 Oct 88 RTP  Original version stabilised
 *  17 Nov 88 RTP  Allow monitor to stdout
 *                 Use typedefs
 *  18 Nov 88 RTP  Add native character encoding
 *   9 Jan 89 RTP  Automatically check input encoding
 *  22 Feb 89 RTP  Remove rewind but hold first 2 bytes
 *  19 Apr 89 RTP  Add Iris mode switches
 *  11 Oct 89 RTP  Add ANSI Prototyping
 *                 Split Header files
 *                 Remove Comarea (arr) & use pointers
 *   4 Jun 90 RTP  Add Postscript driver '-p' option
 *  14 Jun 90 RTP  Add Dynamic buffers
 *  25 Jul 90 RTP  Incorporate include files in cgminit.h
 *   7 Aug 90 RTP  Add error handling Routine
 *  15 Aug 90 NMH  VMS specific ifdefs added
 *  18 Aug 90 PLP  CGMpath added
 *  24 Aug 90 CDO  Name changed to cgmmain.c
 *   4 Sep 90 RTP  Add RAL-GKS GDP option -g ral
 *                 and no error count limit -e
 *                 Change Iris option to -i
 *                 Add routine CGMfopen
 *  24 Sep 90 RTP  Move CGMpath and CGMfopen to cgminput.c
 *  22 Nov 90 RTP  Add string variable for '-g' option
 *  12 Dec 90 NMH  Added different input filename parsing for VMS
 *                 so that it can take files from any directory
 *  20 Dec 90 RTP  Change ungetc to rewind for C/370 on CMS
 *   8 Mar 91 RTP  Add -d option to specify device
 *                 Add Iris options to -i option
 *                 Add TEK, VGA, 8514, HPGL options
 *  11 Mar 91 RTP  Add routine CGMgetarg to decode an argument string
 *  14 Mar 91 RTP  Add profile option '-f'
 *  12 Apr 91 RTP  Change initial file names to ""
 *  14 May 91 RTP  Add ANSI declarations
 *  17 May 91 RTP  Change position of NATIVE driver so that it is only
 *                 used for EBCDIC machines
 *                 Use size_t and MALLOC macro
 *  22 May 91 RTP  Allow input redirection of Character & Binary CGMs
 *  29 May 91 RTP  Add CGMmatch routine for string matching
 *                 Add -o option for driver dependent options
 *  10 Jun 91 RTP  Check options after parsing so that they can be
 *                 entered before the driver
 *                 Create driver names structure
 *  13 Jun 91 RTP  Add local code include statement
 *                 Add call to set driver options
 *  21 Jun 91 RTP  Add CGMenv for checking getenvment variables
 *   2 Aug 91 RTP  Add SCCS id
 *                 Make dev_options a global variable so that CGMgetenv
 *                   works when the driver is set on the command line
 *  21 Aug 91 RTP  Change names of Device dependent environment variables
 *                   to PSOPTIONS, IGLOPTIONS etc and move checkto after
 *                   arguement parsing
 *  22 Aug 91 RTP  Add flag NOSTDIN and NOSTDOUT to prevent using stdin
 *                   and stdout on some machines
 *  13 Sep 91 RTP  Change Device options to CGMPSOPT etc
 *   8 Oct 91 RTP  Add EXTLGTH to fnlength calculation & FREE filenames
 *  17 Jul 92 RTP  Add different levels for verbose reporting
 *                 Add option -gOLDBIN for incorrect ralcgm binary CGMs
 *   2 Nov 92 KEVP Add FILE argument to CGMOtek
 *  10 Nov 92 KEVP Add device dependent options to TEK4200
 *   3 Dec 92 RTP  Change Monitor file to Error file
 *                 Add 'Z' option for CMS to use telnet translate table
 *   8 Dec 92 RTP  Change -goldbin to -loldbin
 *   9 Dec 92 RTP  Change handling of driver options to action user options
 *                 after default options
 *  10 Feb 93 RTP  Make FREE conditional to avoid malloc problem with stdin
 *  15 Jun 94 KEVP Delay announcement of CGM version till error file is defined
 *  23 Jun 94 KEVP Change default ClearText output CGM filetype from TXT to CLT.
 *                             Force ClearText output onto file, ifdef WINDOWS
 *   9 Aug 94 KEVP Add output driver for Windows MetaFile on Graphics Device Interface
 *  13 Feb 95 KEVP Allow default GDP option to remain DEFRALGKSGDP when the
 *                 corresponding environment variable is absent
 *  14 Feb 95 KEVP Make sure PostScript always writes to file, if NOSTDOUT.
 *                 Tidy up verbose output for NOSTDOUT
 *  15 Feb 95 KEVP For MS Windows, put in variable to indicate, whether the
 *                 error file is open, so that error messages can be diverted.
 *  22 Mar 95 KEVP Add quiet output option
 */

#include "cgminit.h"

#ifdef PROTO

/*  Functions in this module  */

   void  CGMoutput ( Code c );
   void  CGMocode ( Code c );
   static Logical CGMgetarg( char*, int, char**, char*, Logical );
   static Logical CGMmatch( char*, char*, int );
   static void CGMgetenv ( void );

/*  Functions called from this module  */

   extern int CGMerror ( char*, int, Enum, char*);
   extern void CGMprofile ( Code );

/*   Input encodings */

   extern void CGMIchar( void );    /*   Character encoding  */
   extern void CGMItext( void );    /*   Clear text encoding */
   extern void CGMIbin( void );     /*   Binary encoding     */

/*   Output drivers  */

   extern void CGMOtext(FILE *, Code, long *, float *, char *);
   extern void CGMOchar(FILE *, Code, long *, float *, char *);
   extern void CGMObin(FILE *, Code, long *, float *, char *);
   extern void CGMstframe( void );

#ifdef POSTSCRIPT
   extern void CGMOps(FILE *, Code, long *, float *, char *);
   extern void PSoptions ( char * );
#endif
#ifdef IGL
   extern void CGMOigl( Code, long *, float *, char * );
   extern void IGLoptions ( char * );
#endif
#ifdef XW
   extern void CGMOXw( Code, long *, float *, char * );
   extern void Xw_options ( char * );
#endif
#ifdef HPGL
   extern void CGMOhpgl( FILE *, Code, long *, float *, char * );
   extern void HPGLoptions ( char * );
#endif
#ifdef TEK4200
   extern void CGMOtek( FILE*, Code, long *, float *, char * );
   extern void TEKoptions ( char * );
#endif
#ifdef WIN
    extern void CGMOwin ( Code, long*, float*, char* );
    extern void WINoptions( char*);
#endif

   extern FILE *CGMfopen ( char*, char, Enum );

#else /* non ANSI */
   void CGMoutput();          /* Output function with profiling */
   void CGMocode();           /* Output a single code */

   static Logical CGMgetarg();
   static Logical CGMmatch();
   static void CGMgetenv();

   extern int CGMerror ();

   extern void CGMIchar();    /*   Character encoding  */
   extern void CGMItext();    /*   Clear text encoding */
   extern void CGMIbin();     /*   Binary encoding     */

   extern void CGMOtext();    /*   Clear text encoding */
   extern void CGMOchar();    /*   Character encoding  */
   extern void CGMObin();     /*   Binary encoding     */

#ifdef POSTSCRIPT
   extern void CGMOps();      /*   Postscript Output */
   extern void PSoptions();   /*   PostScript options */
#endif
#ifdef IGL
   extern void CGMOigl();     /*   Iris Graphics Library  */
   extern void IGLoptions();  /*   Iris options */
#endif
#ifdef XW
   extern void CGMOXw();      /*   Xwindows Graphics  */
#endif
#ifdef HPGL
   extern void CGMOhpgl();    /*   HPGL */
   extern void HPGLoptions(); /*   HPGL options */
#endif
#ifdef TEK4200
   extern void CGMOtek();     /*   Tektronix 4200 */
   extern void TEKoptions();
#endif
#ifdef WIN
   extern void CGMOwin();    /* Windows MetaFile and Graphics Device Interface */
   extern void WINoptions();     /* Set WIN Options */
#endif

   extern FILE *CGMfopen ();  /*   File open routine */
   extern void CGMstframe();  /*   Initiate random frame access */
#endif /* ANSI */

#ifdef XW
   extern char terminal[];    /*   Display variable */
#endif
#ifdef WIN
   extern HDC hdcOut;   /* Device Context Handle for MS Windows Driver */
#endif

/*  Input and Output files */

char *cgmroot, *cgmin,
     *cgmofile, *cgmefile;

FILE *cgmi, *cgmo;

/*  Character variable for error messages */

static char dev_options[80]="", user_options[80]="";
static char mess[80];

#ifdef _DEBUG_MALLOC_INC
int dbfile;
unsigned long malloc_size, malloc_ptr1, malloc_ptr2;
#endif

#ifdef WIN
Logical errfileopen = FALSE;
#endif

/*  Define standard message format */

#define OUTMESS      (void) fprintf( (FILE *) cgmerr,

/******************************************************** main *********/
#ifdef PROTO
main ( int argc, char **argv)
#else
main (argc, argv)
int argc;
char **argv;
#endif

{
   char *func = "CGMmain", s[80];
   int n;
   register long i;
   register Code c, chr1, chr2;
   Logical infilename = FALSE,
           outfilename = FALSE,
#ifdef NOSTDOUT
           errfilename = TRUE;
#else
           errfilename = FALSE;
#endif

   Logical Options = FALSE;

   Logical outsuffix = FALSE;

/*  Define default error stream  */

   cgmerr = stderr;

/*  Add local initialisation code if needed */

#ifdef LOCALCODE
#include "cgmlocal.c"
#endif

/*   Create memory for standard files */

   cgmroot  = (char *) MALLOC( MAXFLSIZE, sizeof(char) );
   cgmin    = (char *) MALLOC( MAXFLSIZE, sizeof(char) );
   cgmofile = (char *) MALLOC( MAXFLSIZE, sizeof(char) );
   cgmefile = (char *) MALLOC( MAXFLSIZE, sizeof(char) );

/* Set up default filenames */
   strcpy( cgmin,  "-");
   strcpy( cgmofile,  "-");
   strcpy( cgmefile,  "-");

/*   check Environment Variables */

   CGMgetenv();

/*   check if filename entered on command line   */

   if ( argc > 1 )
   {
      argv++;  /*  ignore command name  */

/*  Cycle through arguments */
      for (n = 1 ; n < argc ; n++)
      {
         char *arg = *argv++;
         int len = strlen(arg)-1;

         if ( arg[0] == '-' && len )
         {
            char *pa = arg + 1;

/*  parse option codes */

            while ( len )
            {
               c  = toupper( *pa );
               pa++; len--;
               switch ( c )
               {
                  case 'E':  /* Error count ignore */
                     cgmerrcount = 0;
                     break;

                  case 'F':  /* Error file */
                     errfilename = TRUE;
                     break;

                  case 'V':  /* Verbose output */
                     cgmverbose = !cgmverbose;
                     if(cgmverbose) cgmquiet = FALSE;
                     break;

                  case 'Q':  /* Quiet output */
                     cgmquiet = !cgmquiet;
                     if(cgmquiet) cgmverbose = FALSE;
                     break;

                  case '1':  /* more Verbose output */
                     if ( cgmverbose ) cgmverbose++;
                     break;

                  case 'L':  /* use set profile */
                     if( CGMgetarg( pa, len, argv, s, TRUE) )
                     {
                       n++; argv++;
                     }
                     len = 0;
                     cgmralbin = CGMmatch(s,"OLDBIN",3);
#ifdef EBCDIC
                     if( CGMmatch(s,"TCPIP",3) )
                     {
                        cgmebcdic[0x5e] = 0x5f; /* Change 'tilde' */
                        cgmebcdic[0xde] = 0x5f;
                        cgmebcdic[0x7e] = 0xa1; /* Change 'carat' */
                        cgmebcdic[0xfe] = 0xa1;
                        cgmascii[0x5f] = 0x5e;
                     }
                     break;
#endif

#ifdef CGMPROFILE
/* Note this is for future development */
                     if( CGMmatch(s,"CALS",4) ) cgmprof = PROF_CALS;
                     else if( CGMmatch(s,"GKS",3) ) cgmprof = PROF_GKS;
                     else if( CGMmatch(s,"IVDC",2) ) cgmprof = PROF_INTVDC;
#endif
                     break;

                  case 'G':  /* set local GDPs  */
                     if( CGMgetarg( pa, len, argv, s, TRUE) )
                     {
                       n++; argv++;
                     }
                     len = 0;
                     cgmralgks = CGMmatch(s,"RALGKS",3);
                     break;

                  case 'B':
                     cgmdriver = BINARY;
                     break;
                  case 'N':
#ifdef EBCDIC
                     cgmdriver = NATIVE;
                     break;
#endif
                  case 'C':
                     cgmdriver = CHARACTER;
                     break;
                  case 'T':
                     cgmdriver = CLEAR_TEXT;
                     break;

                  case 'D':  /* Device driver */
                  {
                     const struct cgmdrivers *drive;
                     if (CGMgetarg( pa, len, argv, s, TRUE) )
                     {
                       n++; argv++;
                     }
                     len = 0;
                     cgmdriver = NONE;

                     for( drive=drivers; drive->type; drive++ )
                        if( CGMmatch(s, (char*)drive->name, drive->chrs) )
                        {
                           cgmdriver = drive->type;
                           break;
                        }

                     if ( cgmdriver == NONE )
                        exit ( CGMerror(func, ERR_NODRIVER, FATAL, s) );
                     break;
                  }

/*  Old driver options - kept for compatability */

#ifdef POSTSCRIPT
                  case 'P':
                     cgmdriver = POSTSCRIPT;
                     break;
#endif
#ifdef IGL
                  case 'I':
                     cgmdriver = IGL;
                     break;
#endif
#ifdef XW
                  case 'X':
                     cgmdriver = XW;
                     break;

/* Kept for compatability */
                  case 'A':   /*  look for terminal address   */
                     if( CGMgetarg( pa, len, argv, terminal, FALSE) )
                     {
                       n++; argv++;
                     }
                     len = 0;
                     break;
#endif
                  case 'O':   /* Driver dependent options */
                     if( CGMgetarg( pa, len, argv, s, FALSE) )
                     {
                       n++; argv++;
                     }
                     strcat ( user_options, s );
                     if ( strlen(user_options) ) Options = TRUE;
                     len = 0;
                     break;

                  default:   /*  Invalid option - ignore */
                     (void) sprintf(mess, "(-%c)", c);
                     (void) CGMerror( func, ERR_INVARG, WARNING, mess);
                     break;
               }
            }
         }
         else  /* end of option parsing */
         {

/*  at least one file name is given  */

            if ( ! infilename )
            {

/*  work out Input file name if present  */
#ifdef VMS
               int j;

               for ( j = strlen(arg)-1 ; (j > -1 ); j--)
                 if(arg[j]!=']')
                 {
                   if (arg[j]=='.')
                   {
                      i=j;
                      break;
                   }
                 }
                 else
                 {
                    i=strlen(arg);
                    break;
                 }

               if (j== -1) i = strlen(arg);
#else
               for ( i = 0; ( arg[i] != '.' ) &&
                            (i < strlen(arg) ); i++);
#endif
               infilename = TRUE;

               strcpy(cgmin, arg);
               strncpy(cgmroot, arg, (int) i);
               cgmroot[i] = '\0';

/* set default filename to stdout */

               strcpy(cgmofile, "-");


/* If no suffix and not stdin assume suffix 'cgm' */

               if ( i == strlen(arg) && arg[0] != '-' )
               {
                  strcat(cgmin, FILESEP);
                  strcat(cgmin, "cgm");
               }
            }
            else if ( ! outfilename )
            {

/*  Output file name given  */

               outfilename = TRUE;

/* Check if filename has a suffix */
               for (i=0; arg[i] != '\0'; i++)
                  if ( arg[i] == '.' )
                  {
                     outsuffix = TRUE;
#ifdef CMS
/*  Convert to CMS format ie blank separators */
                     arg[i] = ' ';
#endif
                  }
               if ( arg[0] == '-' ) outsuffix = TRUE;
               strcpy(cgmofile, arg);
               if ( ! outsuffix ) strcpy(cgmroot,arg);
            }
            else
            {

/*  Error file name given  */

               if ( arg[0] != '-' ) errfilename = TRUE;

#ifdef CMS     /*  Convert to CMS format ie blank separators */
               for (i=0; arg[i] != '\0'; i++)
                  if ( arg[i] == '.' )
                  {
                     arg[i] = ' ';
                  }
#endif
               strcpy(cgmefile, arg);
            }
         }
      }
   }

#ifdef DEBUG
   cgmquiet = FALSE;   /* No quiet output, if debugging */
#endif

/*  make sure a driver has been specified */

   if ( cgmdriver == NONE )
      exit ( CGMerror(func, ERR_NODRIVER, FATAL, NULLSTR) );

/* Now check driver dependent options */

#ifdef IGL
   if ( cgmdriver == IGL )
   {
      char *env;

      env = (char *)getenv( "CGMIGLOPT" );
      if( env != NULL ) strcpy(dev_options,env);
      if ( Options ) strcat ( dev_options, user_options );
      IGLoptions( dev_options );
   }
#endif /* IGL */

#ifdef XW
   if ( cgmdriver == XW )
   {
      char *env;

      env = (char *)getenv( "CGMXWOPT" );
      if( env != NULL ) strcpy(dev_options,env);
      if ( Options ) strcat ( dev_options, user_options );
      Xw_options( dev_options );
   }
#endif /* XW */

#ifdef POSTSCRIPT
  if ( cgmdriver == POSTSCRIPT )
  {
     char *env;

     env = (char *)getenv( "CGMPSOPT" );
     if( env != NULL ) strcpy(dev_options,env);
     if ( Options ) strcat ( dev_options, user_options );
     PSoptions( dev_options );
  }
#endif  /* POSTSCRIPT */

#ifdef HPGL
  if ( cgmdriver == HPGL )
  {
     char *env;

     env = (char *)getenv( "CGMHPGLOPT" );
     if( env != NULL ) strcpy(dev_options,env);
     if ( Options ) strcat ( dev_options, user_options );
     HPGLoptions( dev_options );
  }
#endif /* IGL */

#ifdef TEK4200
  if ( cgmdriver == TEK4200 )
  {
     char *env;

     env = (char *)getenv( "CGMTEKOPT" );
     if( env != NULL ) strcpy(dev_options,env);
     if ( Options ) strcat ( dev_options, user_options );
     TEKoptions( dev_options );
  }
#endif  /* TEK4200 */

#ifdef VGA
  if ( cgmdriver == VGA )
  {
     char *env;

     env = (char *)getenv( "CGMVGAOPT" );
     if( env != NULL ) strcpy(dev_options,env);
     if ( Options ) strcat ( dev_options, user_options );
     VGAoptions( dev_options );
  }
#endif  /* VGA */

#ifdef WIN
  if ( cgmdriver == WIN )
  {
     char *env;

     env = (char *)getenv( "CGMWINOPT" );
     if( env != NULL ) strcpy(dev_options,env);
     if ( Options ) strcat ( dev_options, user_options );
     WINoptions( dev_options );
  }
#endif  /* WIN */

/*  stdin, stdout and/or stderr are used unset filename */

   if ( !strcmp(cgmin, "-") )
   {
      infilename = FALSE;
      strcpy(cgmin, "stdin");
   }
   if ( !strcmp(cgmofile, "-") )
   {
      outfilename = FALSE;
      strcpy(cgmofile, "stdout");
   }
   if ( !strcmp(cgmefile, "-") )
   {
      if ( errfilename == TRUE )
      {
         strcpy(cgmefile, cgmroot);
         strcat(cgmefile, FILESEP);
         strcat(cgmefile, "err");
      }
      else
      {
         strcpy(cgmefile, "stderr");
      }
   }

/* Open Input file  */

#ifdef DEBUG
   OUTMESS "Output driver: %d Options: %s\n", cgmdriver, dev_options);
   OUTMESS "Input file: '%s'\n", cgmin);
#endif

   if ( infilename )
   {
      if ( (cgmi = CGMfopen(cgmin, F_READ, CLEAR_TEXT)) == NULL)
      {
         (void) sprintf( mess, "(Input file: %s)", cgmin);
         exit ( CGMerror(func, ERR_OPENFILE, FATAL, mess) );
      }
/*  can use random frame access if disk file input */
      cgmrandom = TRUE;
/*  Initialise Random Access structure */
      CGMstframe();
   }
   else
   {
#ifdef NOSTDIN
         exit ( CGMerror(func, ERR_NOFILE, FATAL, NULLSTR) );
#else
      cgmi = stdin;
      cgmterm = TRUE;
#endif
   }

/* Determine type of encoding, get first 2 bytes then reopen file */

   chr1 = fgetc( cgmi );
   chr2 = fgetc( cgmi );

   switch ( (chr1<<8 | chr2) & 0xfff8 )
   {
      case 0x3020:   /*  ASCII Character  */
         cgminput = CHARACTER;
#ifdef BINFILE
/*  Reopen the file as binary */
         fclose(cgmi);
         cgmi = CGMfopen(cgmin, F_READ, CHARACTER);
#endif
         break;


#ifdef EBCDIC
      case 0xf040:  /*  Native character input (EBCDIC)  */
         cgminput = NATIVE;
         break;
#endif

      case 0x0020:   /*  Binary  */
      case 0x0028:
      case 0x0030:
      case 0x0038:
         cgminput = BINARY;
#ifdef BINFILE
         fclose(cgmi);
         cgmi = CGMfopen(cgmin, F_READ, BINARY);
#endif
         break;

      default:
         cgminput = CLEAR_TEXT;   /*  Clear text encoding */
         break;
   }

   if ( cgmterm )
   {
#ifdef CMS
      if ( cgminput == CHARACTER || cgminput == BINARY )
         exit ( CGMerror(func, ERR_REDIRECT, FATAL, NULLSTR) );
#else
/*  unget 2 characters if reading from stdin. MAY NOT ALWAYS WORK */
      ungetc( chr2, cgmi ); ungetc( chr1, cgmi );
#endif
   }
   else
   {
      rewind ( cgmi );
   }

   if( !cgmquiet)
   {
      if ( errfilename )
      {
         if ( (cgmerr = CGMfopen(cgmefile, F_WRITE, CLEAR_TEXT)) == NULL)
         {
            (void) sprintf( mess, "(Error file: %s)", cgmefile);
            exit ( CGMerror(func, ERR_OPENFILE, FATAL, mess) );
         }
#ifdef WIN
         errfileopen = TRUE;
#endif
      }
/* Announce CGM Version */

      OUTMESS "%s\n", cgmversion);
   }


   if ( cgmverbose && infilename )
   {
      OUTMESS " Input file (" );
      switch ( cgminput )
      {
         case BINARY:
            OUTMESS "Binary Encoding" );
            break;
         case CHARACTER:
            OUTMESS "Character Encoding" );
            break;
#ifdef EBCDIC
         case NATIVE:
            OUTMESS "Character Encoding - EBCDIC" );
            break;
#endif
         case CLEAR_TEXT:
         default:
            OUTMESS "Clear Text Encoding" );
            break;
      }
      OUTMESS "): %s\n", cgmin);
   }

#ifdef DEBUG
   OUTMESS "Output file: '%s'\n", cgmofile);
#endif

/*  Work out Output Driver  */

   switch (cgmdriver)
   {
#ifdef EBCDIC
        case NATIVE:   /* Native character Output (EBCDIC) */
#endif

        case CHARACTER:   /* Character Output   */
            if ( !outsuffix )
            {
#ifdef NOSTDOUT
               outfilename = TRUE;
#else
               if ( outfilename )
#endif
               {
                  strcpy(cgmofile, cgmroot);
                  strcat(cgmofile, FILESEP);
                  strcat(cgmofile, "chr");
               }
            }
            break;

        case BINARY:   /* Binary Output */
            if ( !outsuffix )
            {
#ifdef NOSTDOUT
               outfilename = TRUE;
#else
               if ( outfilename )
#endif
               {
                  strcpy(cgmofile, cgmroot);
                  strcat(cgmofile, FILESEP);
                  strcat(cgmofile, "bin");
               }
            }
            break;

#ifdef IGL
        case IGL: /* Iris Graphics Library */
            break;
#endif
#ifdef XW
        case XW:   /* Xwindows*/
            break;
#endif

#ifdef TEK4200
        case TEK4200:   /*  Tektronix 42xx */
            if ( !outsuffix && outfilename )
            {
               strcpy(cgmofile, cgmroot);
               strcat(cgmofile, FILESEP);
               strcat(cgmofile, "tek");
            }
            break;
#endif

#ifdef VGA
        case VGA:   /*  VGA Output  */
            break;
#endif

#ifdef IBM8514
        case IBM8514:   /*  8514 Output  */
            break;
#endif

#ifdef POSTSCRIPT
        case POSTSCRIPT:   /*  Postscript Output  */
#ifdef NOSTDOUT
            outfilename = TRUE;
            if ( !outsuffix)
#else
            if ( !outsuffix && outfilename )
#endif
            {
               strcpy(cgmofile, cgmroot);
               strcat(cgmofile, FILESEP);
               strcat(cgmofile, "ps");
            }
            break;
#endif

#ifdef HPGL
        case HPGL:   /*  HPGL Output  */
            if ( !outsuffix && outfilename )
            {
               strcpy(cgmofile, cgmroot);
               strcat(cgmofile, FILESEP);
               strcat(cgmofile, "hpgl");
            }
            break;
#endif

#ifdef WIN
        case WIN:   /*  WIN Output  */
            if ( !outsuffix && outfilename )
            {
               strcpy(cgmofile, cgmroot);
               strcat(cgmofile, FILESEP);
               strcat(cgmofile, "wmf");
            }
            break;
#endif

        case CLEAR_TEXT:   /*  Clear Text Output  */
#ifdef NOSTDOUT
            outfilename = TRUE;
            if( !outsuffix)
#else
            if ( !outsuffix && outfilename )
#endif
            {
               strcpy(cgmofile, cgmroot);
               strcat(cgmofile, FILESEP);
               strcat(cgmofile, "clt");
            }
            break;

        default:    /*  Unknown Driver  */
            (void) sprintf( mess, "(%d)", cgmdriver );
            exit ( CGMerror(func, ERR_NOCODE, FATAL, mess) );
   }

   if ( outfilename )
   {
      switch ( cgmdriver )
      {
         case BINARY:
            cgmo = CGMfopen(cgmofile, F_WRITE, BINARY);
            break;

#ifdef TEK4200
         case TEK4200:
            cgmo = CGMfopen(cgmofile, F_WRITE, BINARY);
            break;
#endif
#ifdef WIN
          case WIN:
            hdcOut = CreateMetaFile(cgmofile);
            if(hdcOut != NULL) cgmo =(FILE*) NULL+1;
            else cgmo = NULL;
            break;
#endif

         case CHARACTER:
            cgmo = CGMfopen(cgmofile, F_WRITE, CHARACTER);
            break;

         default:
            cgmo = CGMfopen(cgmofile, F_WRITE, CLEAR_TEXT);
      }
      if (cgmo == NULL)
      {
         (void) sprintf( mess, "(Output file: %s)", cgmofile);
         exit ( CGMerror(func, ERR_OPENFILE, FATAL, mess) );
      }
   }
   else cgmo = stdout;

   if ( cgmverbose )
   {
      OUTMESS " Output file (");
      switch ( cgmdriver )
      {
         case BINARY:
            OUTMESS "Binary Encoding" );
            break;
         case CHARACTER:
            OUTMESS "Character Encoding" );
            break;
#ifdef CMS
         case NATIVE:
            OUTMESS "Character Encoding - EBCDIC" );
            break;
#endif
#ifdef POSTSCRIPT
         case POSTSCRIPT:
            OUTMESS "Postscript file" );
            break;
#endif

#ifdef IGL
         case IGL:
            OUTMESS "Iris screen" );
            break;
#endif
#ifdef TEK4200
         case TEK4200:
            OUTMESS "Tektronics screen" );
            break;
#endif
#ifdef HPGL
         case HPGL:
            OUTMESS "HPGL file" );
            break;
#endif
#ifdef VGA
         case VGA:
            OUTMESS "VGA Screen" );
            break;
#endif
#ifdef IBM8514
         case IBM8514:
            OUTMESS "8514 Screen" );
            break;
#endif
#ifdef WIN
         case WIN:
            if(outfilename) OUTMESS "Windows MetaFile");
            else OUTMESS "Windows Graphics Device Interface");
            break;
#endif
         case CLEAR_TEXT:
         default:
            OUTMESS "Clear Text Encoding" );
               break;
      }
#ifdef NOSTDOUT
      if(outfilename)
        OUTMESS ") %s\n",cgmofile);
      else
        OUTMESS ") graphics output\n");
#else
      OUTMESS "): %s\n", cgmofile);
#endif
   }

   if ( cgmverbose )
         OUTMESS " Error file: %s\n\n",  cgmefile);

           /*  Define initial buffers for dynamic allocation */

#ifdef VAR_BUFFER
   pint = (long *) MALLOC ( ARRAY_MAX, sizeof(long) );
   pimax = pint + ARRAY_MAX;
   preal= (float *) MALLOC ( ARRAY_MAX, sizeof(float) );
   prmax = preal + ARRAY_MAX;
   str = (char *) MALLOC ( STRING_MAX, sizeof(char) );
   strmax = str + STRING_MAX;
   if ( pint == NULL || preal == NULL || str == NULL )
   {
      exit ( CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR) );
   }
#ifdef DEBUG
   OUTMESS "Buffers allocated\n  int:  %x-%x (%d)\n",
                                   pint, pimax, pimax - pint);
   OUTMESS " real:  %x-%x (%d)\n", preal, prmax, prmax - preal);
   OUTMESS " char:  %x-%x (%d)\n", str, strmax, strmax - str );
#endif
#endif

/* Free filenames */

    FREE(cgmin);
    FREE(cgmefile);
    FREE(cgmroot);

#ifdef _DEBUG_MALLOC_INC
   dbfile = open("malloc.log",O_WRONLY);
   malloc_size = malloc_inuse( &malloc_ptr1 );
   malloc_dump(dbfile);
#endif

           /*  Run Interpreter  */

   if ( cgmverbose ) OUTMESS "Interpreter started\n");
   switch ( cgminput )
   {
#ifdef EBCDIC
      case NATIVE:  /*  Native character input (EBCDIC)  */
         cgminnative = TRUE;
#endif
      case CHARACTER:
         CGMIchar( );
         break;

      case CLEAR_TEXT:
         CGMItext( );
         break;

      case BINARY:
         CGMIbin( );
         break;

      default:
         exit ( CGMerror(func, ERR_NOCODE, FATAL, NULLSTR) );
   }

   if(cgmverbose && cgmfinished) OUTMESS "Interpreter Finished OK\n");
#ifdef WINDOWS
   return;
#else
   exit (0);
#endif
}

/********************************************************* CGMgetenv ***/
#ifdef PROTO
static void CGMgetenv( void )
#else
static void CGMgetenv()
#endif

{
   char *env;

   env = (char *)getenv( "CGMOUTPUT" );
   if ( env != NULL )
   {
      const struct cgmdrivers *drive;
      for( drive=drivers; drive->type; drive++ )
         if( CGMmatch(env, (char*)drive->name, drive->chrs) )
         {
            cgmdriver = drive->type;
            break;
         }
   }

   env = (char *)getenv( "CGMVERBOSE" );
   cgmverbose = ( env != NULL && CGMmatch(env, "ON", 2) ? TRUE : FALSE);

   env = (char *)getenv( "CGMERRCOUNT" );
   if( env != NULL ) cgmerrcount = atoi( env );

   env = (char *)getenv( "CGMGDP" );
   if( env != NULL)
      cgmralgks = (CGMmatch(env, "RALGKS", 3) ? TRUE : FALSE);

   env = (char *)getenv( "CGMPROFILE" );
   if ( env != NULL )
   {
       cgmralbin = CGMmatch(env,"OLDBIN",3);
   }

   return;
}
/********************************************************* CGMgetarg ***/

#ifdef PROTO
static Logical CGMgetarg ( char *pa, int len,
                           char **argv, char *s, Logical upcase )
#else
static Logical CGMgetarg ( pa, len, argv, s, upcase )
char *pa, **argv, s[];
int len;
Logical upcase;
#endif

{
  register int i;
  Logical newarg = FALSE;

  if ( len )    /* parameter follows option */
  {
     if ( upcase )
        for ( i = 0; i <= strlen(pa); i++)
           *(s+i) = toupper(*(pa+i));
     else
        strcpy(s, pa);
  }
  else  /* space after option */
  {
     newarg = TRUE;
     if ( upcase )
        for ( i = 0; i <= strlen(*argv); i++)
           *(s+i) = toupper(*(*argv+i));
     else
        strcpy(s, *argv);
  }
#ifdef DEBUG
  OUTMESS "arg: %s\n", s);
#endif

  return newarg;

}

/********************************************************* CGMmatch ****/
#ifdef PROTO
static Logical CGMmatch( char *str1, char *str2, int n )
#else
static Logical CGMmatch( str1, str2, n )
char *str1, *str2;
int n;
#endif
{
   if ( strlen(str1) < n || strlen(str1) > strlen(str2) ) return FALSE;

   if ( strlen(str1) > n ) n = strlen( str1 );
   while (n--)
   {
      if ( toupper(*str1) != toupper(*str2) ) return FALSE;
      str1++; str2++;
   }

   return TRUE;
}


#ifdef PROTO
void CGMoutput ( Code c )
#else
void CGMoutput ( c )

Code c;
#endif

{
   struct defaults saveset;

#ifdef DEBUG
   OUTMESS "Output code: 0x%x\n", c );
#endif

   if ( cgmprof )       /* use a profile */
   {
      saveset = cur;    /*  Save common settings */
      (void) CGMprofile(c);
      cur = saveset;    /*  Restore settings */
   }
   else CGMocode( c );

   return;
}

/********************************************************* CGMocode ****/

#ifdef PROTO
void CGMocode ( Code c )
#else
void CGMocode ( c )

Code c;
#endif

{
   switch ( cgmdriver )
   {
#ifdef EBCDIC
      case NATIVE:
#endif
      case CHARACTER:
         CGMOchar(cgmo, c, pint, preal, str );
         break;

      case CLEAR_TEXT:
         CGMOtext(cgmo, c, pint, preal, str );
         break;

      case BINARY:
         CGMObin(cgmo, c, pint, preal, str );
         break;

#ifdef POSTSCRIPT
      case POSTSCRIPT:
         CGMOps(cgmo, c, pint, preal, str );
         break;
#endif

#ifdef IGL
      case IGL:
         CGMOigl(c, pint, preal, str );
         break;
#endif
#ifdef XW
      case XW:
         CGMOXw( c, pint, preal, str );
         break;
#endif
#ifdef VGA
      case VGA:
         CGMOvga(c, pint, preal, str );
         break;
#endif
#ifdef IBM8514
      case IBM8514:
         CGMO8514(c, pint, preal, str );
         break;
#endif
#ifdef HPGL
      case HPGL:
         CGMOhpgl(cgmo, c, pint, preal, str );
         break;
#endif
#ifdef TEK4200
      case TEK4200:
         CGMOtek(cgmo, c, pint, preal, str );
         break;
#endif
#ifdef WIN
       case WIN:
      CGMOwin (c, pint, preal, str);
      break;
#endif
      default:
         exit ( CGMerror("CGMoutput", ERR_NOCODE, FATAL, NULLSTR) );
   }

#ifdef _DEBUG_MALLOC_INC
   if ( c == ENDPIC )
   {
      malloc_size = malloc_inuse(&malloc_ptr2);
      printf("Malloc Size: %ld\n", malloc_size);
      malloc_list(dbfile, malloc_ptr1, malloc_ptr2);
      malloc_ptr1 = malloc_ptr2;
   }
#endif

   return;
}
