/*   RAL-CGM Interpreter module:  @(#) genher.c  version 3.2
 *
 * Program to generate the binary version of the Hershey text database.
 *
 *     Author:    C D Osland
 *
 *     Date:      April 1989
 *
 *     Modification log:
 *     ================
 *
 *     Sept 30, 89  CDO  Modified to produce separate files for each font
 *                       instead of one vast one with small indexes into it.
 *
 *     Oct  23, 90  CDS  Almost totaly re-written!
 *     Apr   4, 91  CDS  Fixed bug in memory allocation (lines 80+83)
 *
 *     Apr  16, 91  CDS  Changed to produce character not defined message
 *                       only once for each missing character.
 *                       Ifdefed out unwanted informational messages.
 *     Apr  25, 91  CDS  Changed frees to #defed FREE.
 *     May  20, 91  CDS  Added character refernce handling, which has been
 *                       removed form cgmher.
 *     June 21, 91  CDS  Changed so that only filename is put into directory
 *                       file instead of full pathname and extension.
 *     Sept 12, 91  CDS  Changed to use new utility header file cgmuher.h.
 *     Sept 30, 91  CDS  Changed charpoint to long, to solve probs on machines
 *                       with 16 bit ints.
 *                       Added checks to all the callocs!
 *     June 16, 95  RTP  Added variable cgmquiet.
 */

#include "cgmmach.h"
#include "cgmpar.h"
#define GENHER_C
#include "cgmuher.h"
#define CGMINIT_H
#include "cgmerr.h"
#include "cgmfile.h"

#define maxpoints 60000       /*  maximum number of points  */
#define maxfpoints 5000       /*  maximum points in one font  */
#define maxcpoints   50       /*  maximum points in one character */

long   *charloc;              /*  points to each character definition  */
int   *charlen;               /*  lengths of each character definition  */

HerDef chardef[maxpoints];    /*  definitions of all characters  */
HerDef fontdef[maxfpoints];   /*  definitions of chars in a font  */

static char *func = "genher";
Logical cgmerrcount=FALSE, cgmquiet=FALSE;

main()

{
    int last, code, i;
    long charpoint = 0l;
#ifdef DEBUG
    int total = 0, chars = 0;
#endif
    char *genname, *dirname, *fsrcname, srcname[80];
    HerFont Hfont;
    HerDir Dir;
    FILE *generator, *directory, *source;

    cgmerr = stderr;

    /***** Open generator file */
    genname = CGMpath("fontgen",herext);

    generator = CGMfopen(genname, F_READ, CLEAR_TEXT);
    if( generator == NULL)
    { /* Exit on the spot if open failed */
        CGMerror(func, ERR_OPENFILE, FATAL, genname);
        (void)FREE(genname);
        exit(0);
    }

    /***** Open directory file */
    dirname = CGMpath("fontdir",herext);
    directory = CGMfopen(dirname, F_WRITE, BINARY);
    if ( directory == NULL)
    { /* Exit on the spot if open failed */
        CGMerror(func, ERR_OPENFILE, FATAL, dirname);
        (void)FREE(dirname);
        exit(0);
    }

    /*****  Read in the name of the main source file and the highest
            character number that is expected in this file.           */
    fscanf(generator, "%d%s", &last, srcname);

    /*****  Allocate memory for the hershey database  */
    charloc=(long *)calloc(last+1, sizeof(int *));
    if (charloc == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);
    for(i=0; i<last; charloc[i++] = -1)
        ;

    charlen=(int *)calloc(last+1, sizeof(int *));
    if (charlen == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);

    /*****  Open up this source file.  */
    fsrcname = CGMpath(srcname, srcext);
    source = CGMfopen(fsrcname, F_READ, CLEAR_TEXT);
    if (source == NULL)
    { /* Exit on the spot if open failed */
        CGMerror(func, ERR_OPENFILE, FATAL, fsrcname);
        (void)FREE(fsrcname);
        exit(0);
    }
#ifdef DEBUG
    else
        fprintf(stderr, "Input font file %s opened OK\n",fsrcname);
#endif

    /*****  Since the source file has a sparce set of character definitions,
            render all characters missing until they are read in.            */

    /*****  Now read all the characters in from the source file,
            keeping them in the big arrays.                       */

    do
    {
        int count = 0;
        short x, y;

        fscanf(source,"%d",&code);
        charloc[code] = charpoint;

        do
        {
            fscanf(source,"%hd %hd",&x,&y);

            chardef[charpoint].x = (char)x;
            chardef[charpoint++].y = (char)y;
            if (charpoint > maxpoints)
            {
                /**** !!!! Should change to cgmerr call ****/
                fprintf(stderr, "More than %d points in database\n",maxpoints);
                exit(0);
            }
            count++;
        } while (x != -64 || y != -64);

        charlen[code] = count;
#ifdef DEBUG
        total += count;
        chars++;
#endif
    } while (code < last);

    fclose(source);
#ifdef DEBUG
    fprintf(stderr, "Hershey source read: %d chars %d points\n",chars,total);
#endif

     /*****  Now read in each of the 'fin' files in turn, creating the
             pointer arrays required and writing out the font definition
             after these.                                                 */

    do
    {
        int ic, charcode, fontcode, fontpoint;
        int avwidth;
        SignChar xref, yref;
        char *finname, *binname, fontname[80], fname[80];
        FILE *finfile, *binary;

        /*****  Scan the generator file for the next 'fin' file
                name and attributes.                             */

        fscanf(generator,"%d%s",&fontcode,fname);
        if (fontcode == -1) break;
        fgets(fontname,80,generator);
        for (ic=0;ic<strlen(fontname);ic++)
            if (fontname[ic+2] == '"')
            {
                fontname[ic] = '\000';
                break;
            }
            else
                fontname[ic] = fontname[ic+2];

#ifdef DEBUG
        fprintf(stderr, "Font %d in file %s is %s\n",fontcode,fname,fontname);
#endif

        /*****  Open up the 'fin' file which holds the list of character
                numbers constituting the font.  */

        finname = CGMpath(fname, srcext);
        finfile = CGMfopen(finname,F_READ,CLEAR_TEXT);
        if (finfile == NULL)
        {   /* Give up on this font if we cant open the finfile */
            CGMerror(func, ERR_OPENFILE, ERROR, finname);
            (void)FREE(finname);
            continue;
        }

        /*****  Generate the name of the binary file  */
        binname = CGMpath(fname, binext);

        /*****  Open up the binary file.  */
        binary = CGMfopen(binname, F_WRITE,BINARY);
        if (binary == NULL)
        {   /***** Give up on this font if we cant open the binary */
            CGMerror(func, ERR_OPENFILE, ERROR, binname);
            (void)FREE(binname);
            /*****  close and FREE finfile as well  */
            fclose(finfile);
            (void)FREE(finname);
            continue;
        }

        avwidth = fontpoint = 0;
        H_char=(HerChar *)calloc(sizeof(*H_char),MaxChars);
        if (H_char == NULL)
           CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);

#ifdef DEBUG
            fprintf(stderr,"Finished character ");
#endif

        for (ic=0;ic<MaxChars;ic++)
        {
            int i;

            fscanf(finfile,"%d",&charcode);
            while (getc(finfile) != '\n')
                ;

            /***** If the character is not present in the main Hershey
                   data file then produce an information message,
                   but flag it so it isnt repeated.                    */
            if (charloc[charcode]<0)
            {
                if (charloc[charcode] == -2) continue;

                /**** !!!! Should change to cgmerr call ****/
                fprintf(stderr,
                        "Warning: Hershey character definition %d missing.\n",
                        charcode);
                charloc[charcode] = -2;
                continue;
            }

            if ((charcode>last)||(charcode<0))
            {
                /**** !!!! Should change to cgmerr call ****/
                fprintf(stderr, "Character %d, Hershey %d, out of range\n",
                       ic+32,charcode);
                continue;
            }

            charpoint = charloc[charcode];
            if ( (fontpoint+charlen[charcode]) > maxfpoints)
            {
                /**** !!!! Should change to cgmerr call ****/
                fprintf(stderr, "More than %d points in one font\n",maxfpoints);
                exit(0);
            }

            H_char[ic].location=fontpoint;
            H_char[ic].numcomp=charlen[charcode];

            xref=chardef[charpoint].x;
            yref=9;

            H_char[ic].width=(unsigned char)(chardef[charpoint].y
                                             -chardef[charpoint].x);
            charpoint++;
            H_char[ic].numcomp=charlen[charcode];
            H_char[ic].centre=0;
            avwidth += H_char[ic].width;

            for (i=1;i<charlen[charcode];i++)
            {
                if (chardef[charpoint].x == -64)
                    fontdef[fontpoint++] = chardef[charpoint++];
                else
                {
                    fontdef[fontpoint].x = chardef[charpoint].x - xref;
                    fontdef[fontpoint++].y = yref - chardef[charpoint++].y;
                }
            }

#ifdef DEBUG
            fprintf(stderr,"%d ",ic+32);
#endif
        }

        printf("Binary file %s created\n",fname);

        Dir.number = fontcode;
        strcpy(Dir.name,fontname);
        strcpy(Dir.filename,fname);
#ifdef DEBUG
        fprintf(stderr,"Dir entry\n Code %d\n Fontname %s\n Fontfile %s\n\n",
                Dir.number,Dir.name,Dir.filename);
#endif

        fwrite ((char *) &Dir, sizeof(Dir), 1, directory);

        Hfont.number = fontcode;
        Hfont.defs = fontpoint;
        Hfont.charids = MaxChars;
        Hfont.top = 14;
        Hfont.cap = 12;
        Hfont.bottom = 2;
        Hfont.height = 21;
        Hfont.width = avwidth / MaxChars;

        ic=fwrite ((char *) &Hfont, sizeof(Hfont), 1, binary);
        ic=fwrite ((char *) H_char, sizeof(*H_char), MaxChars, binary);
        ic=fwrite ((char *) fontdef, sizeof(fontdef[0]), fontpoint, binary);
        FREE(H_char);

        fclose(finfile);
        (void)FREE(finname);
        fclose(binary);
        (void)FREE(binname);
    } while (1);

    printf("Hershey data files created\n");

    /*****  Write out a dummy header to mark end of directory  */
    Dir.number = -1;
    fwrite ((char *) &Dir, sizeof(Dir), 1, directory);

    fclose(directory);
    (void)FREE(dirname);

#ifdef DEBUG
    fprintf(stderr, "Directory file closed\n");
#endif

    exit(0);
}
