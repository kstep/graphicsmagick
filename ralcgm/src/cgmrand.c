/*   RAL-CGM Interpreter module:  @(#) cgmrand.c  version 3.3
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
 * Description:  Random file access routines for CGM
 *
 * Maintenance Log:
 *  16 Aug 89 ARM  Original version
 *  26 Sep 89 RTP  Change Character substitution method
 *  25 Jul 90 RTP  Incorporate include files in cgmin.h
 *                 Add routine CGMdefaults to copy structures
 *   7 Aug 90 RTP  Add VDC dependent checks to CGMdefaults
 *  15 Aug 90 NMH  VMS specific #ifdefs added
 *  31 Aug 90 RTP  Introduce Input dependent MF default structs
 *  24 Sep 90 RTP  Put cgmdefaults, cgmfopen, cgmpath &
 *                 cgmialloc etc in file cgmutil.c
 *  27 Nov 90 NMH  Added routine CGMvaxst, Stores start position of
 *                 the first metafile. VMS specific
 *  15 Mar 91 RTP  Change cgmstate to Enum type
 *  14 May 91 RTP  Add ANSI declarations
 *  17 May 91 RTP  Change calloc calls to MALLOC
 *  20 May 91 RTP  Use fgetpos and fsetpos for all machines, remove VAX
 *                 specific code, add routines for non-ANSI compilers
 *                 Use CHARACTER and NATIVE for test instead of 'C' & 'N'
 *  24 May 91 RTP  Change CGMvaxst to CGMstframe to use on all machines
 *                 Move CGMnextbyte to CGMichar
 *  29 May 91 RTP  Add comments and make internal routines static
 *                 Replace CGMOUT() with CGMoutput()
 *  14 Jun 91 RTP  Move external functions to cgmrand.h
 *   2 Aug 91 RTP  Add SCCS id
 *   7 Aug 91 RTP  Tidy up after lint
 *  12 Aug 94 KEVP Enable interpreter to finish by return rather than exit
 *   1 Mar 95 KEVP Allow for possible overrun of opcode by input driver
 *                 when saving frame position
 */

#define CGMRAND_C

#include "cgmin.h"
#include "cgmrand.h"

#ifdef PROTO

/*  Internal routines */

static void CGMfrsave( Code ),
            CGMmfsave( void ),
            CGMpoint( Int frame, Code *rc);

/*  External Routine  */

   extern void CGMoutput( Code c );

#else

/*  CGM Utilities in File  */

   Code CGMframe();    /* Random Access end of frame processimg */
   void CGMstframe();  /* Store disk position of start of first metafile */

   static void CGMfrsave(),  /* Random Access save frame pointer */
               CGMmfsave(),  /* Random access save metafile pointer */
               CGMpoint();   /* Random Access change disk address */

   extern void CGMoutput();

#ifndef HAVE_SETPOS
   static int fgetpos(), fsetpos(); /* ANSI routines */
#endif

#endif

/*  External Variables not declared in CGM.H  */

extern Logical miccharsub, micsubchar[];

/*  Input buffer pointers */

   static Logical nosave_meta=FALSE, eof_meta = FALSE;
                                                        /* Device    */
   static Int cur_frame=1, target_frame=0;              /* control   */
   static Int n_frame=1, max_frame=0;                   /* addresses */
   static Int cur_meta=1, n_meta=1, max_meta=0;
   static struct data_frame  *start_frame=NULL;
   static struct data_frame  *start_meta=NULL;

extern FILE *cgmi;
extern Enum cgminput;

#define DMESS         (void) fprintf(stderr,

/****************************************************** CGMframe *******/

#ifdef PROTO
Code CGMframe( Code code )
#else
Code CGMframe( code )

Code code;
#endif
/*  This routine is the main entry point from the input driver
    which checks for random frame access, stores and changes position
    in the input file and calls the output driver
*/
{
   Int frame;
   Code rc = NONOP;

#ifdef DEBUG
   DMESS "CGMframe: (%d/%d): %04x\n", cur_frame, cur_meta, code );
#endif

   if ( code == EOF)
   {
/* End of File processing */
#ifdef DEBUG
       DMESS "CGMframe: EOF\n" );
#endif
       eof_meta = TRUE;
       switch (cgmstate)
       {
          case PIC_OPEN:
/* If in picture close it */
              rc = ENDPIC;
              break;
          case PIC_CLOSED:
          case MF_CLOSED:
/* If outside picture position at start of last frame */
              cgmpresent = cur_frame;
              cgmnext = cur_frame-1;
              CGMoutput(code);
              if(cgmfinished)
              {   /* quit interpreter */
                  rc = EOF;
                  break;
              }
              if (cgmnext < 1) target_frame = 1;
              else if (cgmnext > cur_frame-1)
                        target_frame = cur_frame-1;
                   else target_frame = cgmnext;
              CGMpoint(target_frame, &rc);
              break;
          default:
/* otherwise signal end of metafile */
              rc = ENDMF;
              break;
       }
       return(rc);
   }

   if ( cgmstate == MF_DESC || cgmstate == MF_ELEMLIST ||
        cgmstate == MF_DEFAULTS || cgmstate == MF_CLOSED )
   {
#ifdef DEBUG
       DMESS "CGMframe: MF Descriptor\n" );
#endif
/*  If in metafile descriptor save file position */
      CGMfrsave(code);

/* if ENDMF then save metafile pointer */
      if (code == ENDMF)
         if (nosave_meta) nosave_meta = FALSE;
         else
         {
            CGMmfsave();
            cur_meta++;
         }

/* Output Element which may change 'cgmnext' */
      cgmpresent = cur_frame;
      cgmnext = target_frame;
      CGMoutput(code);
      target_frame = (cgmnext<1 ? 1: cgmnext);
      cgmabort = 0;

      return (rc) ;
   }

   if ( code == ENDPIC )
   {
#ifdef DEBUG
       DMESS "CGMframe: End of Picture\n" );
#endif

/*  Save frame at end of picture */
       CGMfrsave(code);
       if (cur_frame == target_frame)
       {
           cgmnext = cur_frame+1;
           cgmpresent = cur_frame;
           CGMoutput(code);
           target_frame = (cgmnext<1 ? 1: cgmnext);

/* If reading from stdin then issue error message if positioning backward */
           if (!cgmrandom)
           {
              if (target_frame <= cur_frame)
                  (void) CGMerror ( "CGMframe", ERR_NOBACKSP, ERROR, NULLSTR);
              target_frame = cur_frame + 1;
           }
           cgmabort = 0;
/* If target is next frame then return */
           if (target_frame == cur_frame + 1)
           {
              cur_frame++;
              return(rc);
           }
/* Else point at target */
           if (target_frame <= n_frame) frame = target_frame;
           else frame = n_frame;
           CGMpoint(frame, &rc);
       }
       else cur_frame++;
   }
   else  /* For all other codes ie inside a Picture */
   {
/* First check if picture is aborted */
      if ( cgmabort ) return(rc);
/* Then if new target point to it */
      if ( target_frame != cur_frame )
         if (cur_frame == n_frame) return(rc);
         else
         {
            if (target_frame <= n_frame) frame = target_frame;
            else frame = n_frame;
            CGMpoint(frame, &rc);
            cur_frame = frame - 1;
            rc = ENDPIC;
         }

      cgmpresent = cur_frame;
      CGMoutput(code);

/* If Picture aborted point to next frame */
      if (cgmabort)
      {
         if ((cur_frame == n_frame) || !cgmrandom) return(rc);
         frame = cur_frame+1;
         CGMpoint(frame, &rc);
         cur_frame = frame-1;

         rc = ENDPIC;
      }
   }
   return (rc);
}

/****************************************************** CGMpoint **/

#ifdef PROTO
static void CGMpoint( Int frame, Code *rc )
#else
static void CGMpoint( frame, rc )
Int frame;
Code *rc;
#endif
/*  Function to move file to 'frame' */
{
    struct data_frame *p;

#ifdef DEBUG
    DMESS "CGMpoint (%d/%d): %d\n", cur_frame, cur_meta, frame);
#endif

/* Trying to point past the end of file, issue warning  */
    if (eof_meta && (frame >= n_frame) && !cgmabort)
    {
       target_frame = frame = n_frame-1;
       CGMerror ( "CGMpoint", ERR_SEEKEOF, ERROR, NULLSTR);
    }

/* set p to data frame structure */
    p = &start_frame[frame-1];
/* If different metafile get BEGMF pointer */
    if (p->index.meta != cur_meta)
    {
       cur_meta = p->index.meta;
       p = &start_meta[cur_meta-1];
       cur_frame = p->index.frame;
/* if picture is closed then send an ENDMF to driver */
       if ( cgmstate == PIC_CLOSED )
       {
          *rc = ENDMF;
          nosave_meta = TRUE;
       }
    }
    else cur_frame = frame;

/* Move to position set by 'p' */
    fsetpos(cgmi,&(p->disk_addr));
#ifdef DEBUG
    DMESS "Now points to (%d/%d): %d\n", cur_frame, cur_meta, frame);
    DMESS "Set pointer to:" );
#ifndef HAVE_SETPOS
    DMESS " %4d\n", p->disk_addr );
#else
#ifdef C370
    DMESS " %4d %4d\n", p->disk_addr.__fpos_elem[1],
                        p->disk_addr.__fpos_elem[41] );
#endif
#endif
#endif
/* Unset EOF flag if neccessary */
    cgmEOF = FALSE;

}

/****************************************************** CGMfrsave **/

#ifdef PROTO
static void CGMfrsave( Code code)
#else
static void CGMfrsave( code )
Code code;
#endif
/*  Save frame pointer either in MF descriptor or at ENDPIC
    so that pointer is set to next BEGPICBODY or ENDMF */
{
   fpos_t disk_addr;
   struct data_frame *p;
   Int i;

#ifdef DEBUG
   DMESS "CGMfrsave (%d/%d): %04x\n", cur_frame, cur_meta, code );
#endif

/* If using stdin ignore as we can't position backwards */
   if (!cgmrandom) return;

/* ignore if not at last frame - we have already recorded it */
   if (cur_frame != n_frame) return;

/* increment last frame counter at ENDPIC */
   if (code == ENDPIC) n_frame++;

/* Get current file position */
   fgetpos(cgmi,&disk_addr);

/* Correct any overrun */
   if(cgmoverrun)
   {
      disk_addr -= cgmoverrun;
      cgmoverrun = 0;
   }

#ifdef DEBUG
    DMESS "Frame %d/%d pointer:", n_frame, cur_meta );
#ifndef HAVE_SETPOS
    DMESS " %4d\n", disk_addr );
#else
#ifdef C370
    DMESS " %4d %4d\n", disk_addr.__fpos_elem[1], disk_addr.__fpos_elem[4] );
#endif
#endif
#endif

/* If first time initialise start frame structure */
   if (start_frame == NULL)
   {
      max_frame = NFRAME;
      start_frame=(struct data_frame *)
                         MALLOC( max_frame, sizeof(struct data_frame) );
   }

/* if we have reached the maximum get more space */
   if (n_frame == max_frame)
   {
      p = start_frame;
      max_frame += NFRAME;
      start_frame=(struct data_frame *)
                         MALLOC( max_frame, sizeof(struct data_frame) );
      for (i=0; i<n_frame; i++) start_frame[i] = p[i];
      FREE( p );
    }

/* Save file pointer and current metafile number */
    start_frame[n_frame-1].disk_addr = disk_addr;
    start_frame[n_frame-1].index.meta = cur_meta;

    return;
}

/****************************************************** CGMmfsave **/

#ifdef PROTO
static void CGMmfsave( void )
#else
static void CGMmfsave()
#endif
/*  Save metafile pointer at ENDMF */
{
   struct data_frame *pm;
   Int i;

#ifdef DEBUG
   DMESS "CGMmfsave: %d (%d)\n", cur_meta, n_meta );
#endif

/* If using stdin ignore as we can't position backwards */
   if (!cgmrandom) return;

/* ignore if not at last frame - we have already recorded it */
   if (cur_meta != n_meta) return;

/* initialisation was made by CGMstframe */
   if (start_meta == NULL)
   {
      CGMerror("CGMframe", ERR_RANDOM, RC_FATAL, NULLSTR );
   }

/* Increase memory if structure has reached maximum */
   if (n_meta == max_meta)
   {
      pm = start_meta;
      max_meta += NMETA;
      start_meta=(struct data_frame *)
                         MALLOC( max_meta, sizeof(struct data_frame) );
      for (i=0; i<n_meta; i++) start_meta[i] = pm[i];
      FREE( pm );
    }

/* Store pointer to BEGPICBODY */
    start_meta[n_meta] = start_frame[n_frame-1];
/* Set index to curent frame */
    start_meta[n_meta].index.frame = cur_frame;
    n_meta++;

   return;
}

/****************************************************** CGMstframe *****/

#ifdef PROTO
void CGMstframe( void )
#else
void CGMstframe( )
#endif
/* Initialise Metafile data_frame at start of file */
{
   fpos_t start_pos;

/* Don't bother if reading from stdin */
   if (!cgmrandom) return;

/* start_meta should always be NULL, but check for double call */
   if (start_meta == NULL)
   {
      max_meta = NMETA;
      start_meta = (struct data_frame *)
                         MALLOC( max_meta, sizeof(struct data_frame) );
/* Use start_pos as the struct is machine dependent */
      fgetpos(cgmi,&start_pos);
      start_meta[0].disk_addr = start_pos;
      start_meta[0].cur_ptr  =  start_meta[0].end_ptr = NULL;
      start_meta[0].index.frame = 1;
   }

#ifdef DEBUG
    DMESS "Initial frame pointer:" );
#ifndef HAVE_SETPOS
    DMESS " %4d\n", start_pos );
#else
#ifdef C370
    DMESS " %4d %4d\n", start_pos.__fpos_elem[1], start_pos.__fpos_elem[4] );
#endif
#endif
#endif

   return;
}

#ifndef HAVE_SETPOS /*  Define routines for fsetpos and fgetpos for old C */

/****************************************************** fsetpos ********/
static int fsetpos ( file, bptr )
FILE *file;
fpos_t *bptr;
{
    return fseek(file,*bptr,0);
}

/****************************************************** fgetpos ********/
static int fgetpos ( file, bptr )
FILE *file;
fpos_t *bptr;
{
    *bptr = ftell(file);
    return 0;
}

#endif
