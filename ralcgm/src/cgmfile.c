/*   RAL-CGM Interpreter module:  @(#) cgmfile.c  version 3.3
 *
 * Copyright (C) Rutherford Appleton Laboratory 1990, All Rights Reserved.
 *
 * This source may be copied, distributed, altered or used, but not sold
 * for profit or incorporated into a product except under licence from
 * the author.
 * It is not in the public domain.
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the author should be marked as
 * such.
 *
 * R T Platon @ Rutherford Appleton Laboratory (rtp@uk.ac.rl.ib)
 *
 * Description:  System independent file handling Utilities
 *
 * Maintenance Log:
 *   4 Oct 90 RTP  Original version created from cgminput.c to separate
 *                   file handling routines.
 *  13 Nov 90 PLP  Introduced environment variable CGMDATA.
 *  14 Dec 90 RTP  Replace 'sqrt' and 'fabs' by macros SQRT and FABS
 *   7 Jan 91 RTP  Use MAXFLSIZE for 'tmp' char array
 *  10 Jan 91 RTP  Add C370 options for CMS, increase fmode to 80 chars
 *  12 Apr 91 AMR  Added CGMmodfextn, CGMmodfname and CGMgetfname. Moved from
 *                 the PostScript driver.
 *  14 Apr 91 RTP  Remove PATHDELIM definition, put in cgmmach.h/mc_*.h
 *  14 May 91 RTP  Add ANSI declarations
 *  17 May 91 RTP  Cast malloc arguments to (size_t)
 *  20 May 91 RTP  Change free() calls to Macro FREE
 *   6 Jun 91 RTP  Add 1 to size for malloc for filename
 *  19 Jun 91 RTP  Add header file cgmatt.h to include function declarations
 *  29 Jul 91 NMH  VMS Binary cgms now output in 512 byte fixed length records
 *  30 Jul 91 NMH  Force stream mode access on input and binary output for VMS
 *   2 Aug 91 RTP  Add SCCS id
 *   7 Aug 91 RTP  Tidy up after lint
 *  23 Sep 91 NMH  Added "bls=512" for VMS binary output.
 *   3 Mar 92 RTP  Add IBM RS6000
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *   24 Jul 94 KEVP In CGMfopen put in binary option for BORLANDC
 */

#define CGMFILE_C

#include "cgmmach.h"
#include "cgmpar.h"
#include "cgmerr.h"
#include "cgmfile.h"

/********************************************************* CGMpath ***/

#ifdef PROTO
char *CGMpath ( char *filename, char *extension )
#else
char *CGMpath ( filename, extension )
char *filename, *extension;
#endif

/*
 *    CGMpath(): Function to build up a  full pathname in a (hopefully)
 *               system independent way.
 *
 *               IN: Base name part of the filename (i.e. no extension).
 *                   File extension part of the filename.
 *               OUT: None.
 *               RETURN VALUE: Pointer to the fully-built pathname.
 *
 *               Author: PLP
 *
 * ALGORITHM
 * ---------
 *
 * Allocate memory needed to store the entire pathname, which consists of
 * the data directory name (macro DATADIR defined in mach.h), filename's
 * base name, file separator (macro FILESEP defined in mach.h) and the
 * file extension.
 *
 * NB: If malloc fails we exit on the spot.
 *
 * LOCALS
 * ------
 * func - function identifier to pass down to the error handler.
 * ptr  - pointer to the fully built pathname.
 * tmp  - temporary string.
 *
 */

{
   char *ptr, *func="CGMpath";
   char tmp[MAXFLSIZE+1];
   static char *datapath=(char *)NULL;

   /* Check input arguments - issue warning if illegal length */
   if ((strlen(filename)  > NAMELGTH || !strlen(filename)) ||
       (strlen(extension) > EXTLGTH  || !strlen(extension)))
   {
      (void) sprintf(tmp, "%s%s%s", filename, FILESEP, extension);
      (void) CGMerror(func, ERR_NAME2LONG, WARNING, tmp );
   }

   if (datapath == (char *)NULL)
   {
      datapath = (char *)getenv ("CGMDATA");
      if (datapath == (char *)NULL) datapath = DATADIR;
   }
   /* Get enough memory for the entire filename */
   ptr = (char *)malloc( (size_t)( strlen(datapath) + strlen(filename)
                          + strlen(FILESEP) + strlen(extension) + 1) );
   if( ptr == NULL )
      /* Exit on the spot if malloc failed */
      exit ( CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
   else
   {
      /* Build up the four part filename */
      (void)sprintf(ptr, "%s%s%s%s", datapath, filename, FILESEP, extension);
   }
   return ptr;
}
/********************************************************* CGMfopen ****/

#ifdef PROTO
FILE *CGMfopen ( char *filename, char mode, Enum type )
#else
FILE *CGMfopen ( filename, mode, type )
char *filename, mode;
Enum type;
#endif

/*
 *    CGMfopen(): Function to open a file in a system independent way.
 *
 *               IN: file name
 *                   mode ( F_READ or F_WRITE )
 *                   type ( BINARY, CHARACTER  or CLEAR_TEXT )
 *               OUT: None.
 *               RETURN: Return code from fopen (*FILE)
 *
 *               Author: RTP
 *
 */

{
   char fmode[80], *fname;
#if LFILEDEF > 0
   char *func="CGMfopen";
#endif
   FILE *file;

#if LFILEDEF > 0
   fname = (char *)malloc( (size_t) (strlen(filename) + LFILEDEF) );
   if( fname == NULL)
      /* Exit on the spot if malloc failed */
      exit ( CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
   strcpy(fname, filename);
#else
   fname = filename;
#endif
   sprintf(fmode, "%c", mode);

#ifdef CMS
   if ( type == BINARY || type == CHARACTER )
#ifdef IBMC
      strcat(fmode, "b");
#endif
#ifdef C370
   {
      strcat(fmode, "b");
      if ( mode == F_WRITE )
         strcat( fmode, ", recfm=f, lrecl=80" );
   }
   else
      if ( mode == F_WRITE )
         strcat( fmode, ", recfm=v, lrecl=132" );
#endif
#ifdef WATC
   {
      if ( mode == F_READ )
         strcat( fname, " (bin" );
      else
         strcat( fname, " (bin recfm f lrecl 80" );
   }
   else
      if ( mode == F_WRITE )
         strcat( fname, " (recfm v lrecl 132" );
#endif
#endif

#ifdef MSOFTC
   if ( type == BINARY )
      strcat(fmode, "b");
#endif

#ifdef BORLANDC
   if ( type == BINARY )
      strcat(fmode, "b");
#endif


#ifdef RS6000
   if ( type == BINARY )
      strcat(fmode, "b");
#endif

#ifdef VMS
   if ( (type == CLEAR_TEXT || type == CHARACTER) && mode == F_WRITE )
      file = fopen(fname, fmode, "rfm=var", "rat=cr");
   else if ( (type == BINARY) && mode == F_WRITE )
      file = fopen(fname, fmode, "rfm=fix", "mrs=512", "bls=512", "ctx=stm");
   else
      file = fopen(fname, fmode );
#else
      file = fopen(fname, fmode);
#endif

#ifdef DEBUG
   (void) fprintf(stderr, "Open file: '%s' '%s' %x\n",
                  fname, fmode, file );
#endif
#if LFILEDEF > 0
   FREE ( fname );
#endif

   return file;
}

/***********************************************************************
* CGMmodfextn()  : modify the extension of a filename.
*
* Input Arguments:
*     extension  : new extension
*
* Input/Output Arguments:
*     filename   : address of the filename to modify
*
* Return value   :
*     None
*
* Algorithm      :
*     This routine is system dependant!
*     Uses PATHDELIM to determine where the extension is within the filename,
*       then replaces it with the new extension.
*     Not possible to use FILESEP, defined in mach.h, in tests
*       due to double quotes rather than apostrophes being used, so assign it
*       at the beginning, then use the first character when comparing.
*     For cms there may be a filemode following the extension, therefore
*       a copy is used to search, while constructing the original again.
*
* Maintenance Log:
*  25 Jul 91 AMR  Fixed a bug in CMS part, where original filename was being
*                 reset before being copied!!
*
* Bugs           :
*     Doesn't check that the resulting filename is valid!
************************************************************************/
#ifdef PROTO
void CGMmodfextn( char *fn , char *extn )
#else
void CGMmodfextn( fn , extn )
char *fn, *extn;
#endif
{
  char *cp1, *filesep = FILESEP;

#ifdef CMS
  char *fn2, *func = "CGMmodfextn";

  /*
    Create a copy of the string, and set original to NULL ready to start
    reconstruction.
  */

  fn2 = (char *)malloc( (size_t) (sizeof(char) * (strlen(fn) + 1)) );
  if ( fn2 == NULL )
    exit((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
  strcpy( fn2, fn);
  cp1 = fn2;
  *fn = '\0';

  /*
   Search for space between filename and extension
  */
  while ( *cp1 != '\0' )
  {
    if ( *cp1 == *filesep )
      break;
    cp1++;
  }

  /*
   Copy up to the space into fn, then add the default separator and new extn.
  */
  (void)strncat(fn, fn2, cp1 - fn2 );
  (void)strcat(fn, FILESEP);
  (void)strcat(fn, extn);

  /*
   Carry on searching in case there's a filemode
  */
  if ( *cp1 != '\0' )
  {
    cp1++;
    while ( *cp1 != '\0' )
    {
      /*
       Another separator found, so there must be and extension,
       add the rest of the string to fn.
      */
      if ( *cp1 == *filesep )
      {
        (void)strcat(fn, cp1);
        break;
      }
      cp1++;
    }
  }
  FREE(fn2);

#else /* CMS */
  /*
   Look through the filename backwards until either ...
     the beginning of the extension is found, or
     the beginning of the filename is found, i.e. no extension, by
       looking for the pathname delimter or the beginning of the string.
  */

#ifdef VMS
  if ((cp1 = (char *)getenv(fn)) != NULL)
    fn = cp1;
#endif

  for ( cp1 = fn + strlen( fn ); cp1 > fn; cp1-- )
    if ( *cp1 == PATHDELIM )
    {
      cp1 = fn;        /* no extension, so pretend to have finished the FOR */
      break;
    }
    else
      if ( *cp1 == *filesep ) break;

  /*
   If extension found, remove it by delimiting the string after the separator.
   else, append the separator to the end of the filename.
  */
  if (cp1 != fn)
    *++cp1 = '\0';
  else
    (void)strcat(fn,FILESEP);

  /* Add the new extension to the filename */
  (void)strcat(fn, extn);

#endif /* CMS */

return;

}


/***********************************************************************
* CGMmodfname() : modify the extension of a filename.
*
* Input Arguments:
*     newname   : new filename
*
* Input/Output Arguments:
*     fn        : address of the filename to modify
*
* Return value  :
*     None
*
* Algorithm     :
*     This routine is system dependant!
*     Not possible to use MACRO's such as FILESEP, defined in mach.h, in tests
*       due to double quotes rather than apostrophes being used, so assigns it
*       to a string, and uses the first character in order to do comparisons.
*     Uses a copy of the string to search on, and re-constructs the original
*       from it, and the new filename.
*
* Maintenance Log:
*  25 Jul 91 AMR  Added free for fn2.
*
* Bugs          :
*     Doesn't check that the re-constructed filename is valid.
************************************************************************/
#ifdef PROTO
void CGMmodfname( char *fn , char *newname )
#else
void CGMmodfname( fn , newname )
char *fn, *newname;
#endif
{
  char *cp1, *fn2, *filesep = FILESEP, *func="CGMmodfname";
  char *extnptr;
  int extnflag = 0;

  fn2 = (char*)malloc( (size_t) (sizeof(char) * (strlen(fn) + 1)) );

  if ( fn2 == NULL )
    exit((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));

  (void)strcpy(fn2,fn);

#ifdef CMS
  *fn = '\0';
  cp1 = fn2;
  (void)strcat(fn,newname);

  /*
   Skip over filename by searching for filename/extension separator.
  */
  while ( *cp1 != '\0' )
  {
    if ( *cp1 == *filesep ) break;
    cp1++;
  }
  /*
   Then append the rest of the filename after the new part.
  */
  (void)strcat(fn,cp1);

#else /* Not CMS */

#ifdef VMS
  if ((cp1 = (char *)getenv(fn)) != NULL)
    (void)strcpy(fn2,(fn = cp1));
#endif

  for ( cp1 = fn2 + strlen(fn2); cp1 >= fn2; cp1--)

    /*
     Search backwards through the string, setting a pointer at the extension
     delimeter, and stopping when the beginning of the filename is reached
     by looking for the pathname delimeter or the beginning of the string.
    */
    if ( (*cp1 == *filesep) && (extnflag == 0) )
    {
      extnptr = cp1;
      extnflag = 1;
    }
    else
      if ( *cp1 == PATHDELIM )
        break;
  /*
   truncate the file at the end of the pathname, then add then new filename
   and then the old extension
  */
  *(fn + (++cp1 - fn2)) = '\0';
  (void)strcat( fn, newname);
  (void)strcat( fn, extnptr);

#endif

FREE(fn2);

return;

}

/***********************************************************************
* CGMgetfname() : locate the end of the pathname part of a given file, and
*                 return the length of the filename base.
* Input Arguments:
*     fname     : the complete filename (path, filename base and extension)
*                 to search
*
* Input/Output Arguments:
*     fnptr     : address pointer to end of pathname.
*
* Return value:
*     fnlen     : the length of the base part of the filename.
*
* Algorithm     :
*     This routine is system dependant!
*     Not possible to use MACRO's such as FILESEP, defined in mach.h, in tests
*       due to double quotes rather than apostrophes being used, so assigns it
*       to a string, and uses the first character in order to do comparisons.
*
************************************************************************/
#ifdef PROTO
unsigned short CGMgetfname( char *fname, char **fnptr)
#else
unsigned short CGMgetfname( fname, fnptr )
char *fname, **fnptr;
#endif
{
  char *filesep = FILESEP;
  char *fsptr, *cptr;
  unsigned short fnlen, extnflag = 0;

#ifdef CMS
  char *fnend, *func = "CGMgetfname";

  *fnptr = fname;
  fnend = fname + strlen(fname);

  /*
   Search for the filename/filetype separator.
  */
  for ( cptr = fname; cptr < fnend; cptr++)
    if (*cptr == *filesep)
      break;

  fnlen = cptr - fname;

#else /* Not CMS */

#ifdef VMS
  if ((cptr = (char *)getenv(fname)) != NULL)
    fname = cptr;
#endif

  /*
   Search for the LAST separator in the filename, and the pathname/filename
   delimiter.
  */
  for ( cptr = fname + strlen(fname); cptr >= fname; cptr--)

    if ( (*cptr == *filesep) && (extnflag == 0) )
    {
      fsptr = cptr;
      extnflag = 1;
    }
    else
      if ( *cptr == PATHDELIM )
        break;

  *fnptr = ++cptr;
  fnlen = (extnflag == 1 ? (fsptr - *fnptr)
                         : fname + strlen(fname) - *fnptr);

#endif

return (fnlen);
}
