/*   RAL-CGM Interpreter module:  @(#) genbez.c  version 3.2
 *
 * Program to generate the binary version of the Bezier text database.
 *
 *    Author:   C D Osland
 *
 *    Date :    April 1989
 *
 *    Modification Log:
 *    ================
 *
 *     1/09/90 CDS  Changed to use new data structures defined in cgmbez.h
 *    03/09/90 CDS  Changed all file opens to machine independant CGMfopen
 *                  with filenames supplied by CGMpath.
 *    25/04/91 CDS  Changed frees to #defed FREE
 *    16/05/91 CDS  Moved reference adjustment to here from cgmbez.c
 *    21/06/91 CDS  Changed so that only filename is put into directory file
 *                  instead of full pathname and extension.
 *    12/09/91 CDS  Changed to use new utility header file cgmubez.
 *    30/09/91 CDS  Added checks to all the callocs.
 *    16/06/95 RTP  Added variable cgmquiet.
 *
 */

#include "cgmmach.h"
#include "cgmpar.h"
#define GENBEZ_C
#include "cgmubez.h"
#define CGMINIT_H
#include "cgmerr.h"
#include "cgmfile.h"

static char *func="genbez";

Logical cgmerrcount=FALSE, cgmquiet=FALSE;

BezDir  Dir;
BezFont Bfont;

main()
{
    FILE *generator, *directory;
    char *dirname, *genname;

    cgmerr = stderr;

    /*****  Open up the directory to all the text versions of the
            Bezier font files.  Whatever name.d this directory is,
            the binary font file will be called name.b  */

    /***** Open generator file */
    genname = CGMpath("fontgen",bezext);
    generator = CGMfopen(genname, F_READ, CLEAR_TEXT);
    if( generator == NULL)
    { /* Exit on the spot if open failed */
        CGMerror(func, ERR_OPENFILE, FATAL, genname);
        (void)FREE(genname);
        exit(0);
    }

    /***** Open directory file */
    dirname = CGMpath("fontdir",bezext);
    directory = CGMfopen(dirname, F_WRITE, BINARY);
    if ( directory == NULL)
    { /* Exit on the spot if open failed */
        CGMerror(func, ERR_OPENFILE, FATAL, dirname);
        (void)FREE(dirname);
        exit(0);
    }

    B_char=(BezChar *)calloc((int)MaxChars,sizeof(BezChar));
    if (B_char == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);
    B_def=(BezDef *)calloc((int)MaxDefs,sizeof(BezDef));
    if (B_def == NULL)
       CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR);

    do
    {
        int count, ic, fontcode;
        char srcname[80], *fsrcname, fontname[80], comment[80], *binname;
        int charid, loc, more, line;
        float va, vb, vc, vd, ve;
        FILE *source, *binary;

        /*****  Read in the information about the next font from the
                generator file.  This will be its number, the filename
                in which the source data of the font is held and finally
                its "official" font name, inside double quotes.           */

        fscanf(generator,"%d %s",&fontcode,srcname);
        if (fontcode == 0) break;
        fgets(fontname,80,generator);
        for (ic=0;ic<strlen(fontname);ic++)
            if (fontname[ic+2] == '"')
            {
                fontname[ic] = '\000';
                break;
            }
            else
                fontname[ic]=fontname[ic+2];

#ifdef DEBUG
        fprintf(stderr, "Font %d in file %s is %s\n",
                fontcode,srcname,fontname);
#endif

        /*****  Open up the source file.                                  */

        fsrcname = CGMpath(srcname,srcext);
        source = CGMfopen(fsrcname,F_READ,CLEAR_TEXT);
        if (source == NULL)
        {
            printf("Font data file %s could not be opened\n",fsrcname);
            continue;
        }
#ifdef DEBUG
        else
            fprintf(stderr, "Input font file %s opened OK\n",fsrcname);
#endif

        /*****  Open up the binary file  */

        binname = CGMpath(srcname,binext);
        binary = CGMfopen(binname,F_WRITE,BINARY);
        if (binary == NULL)
        {
            printf("Output file %s could not be opened\n",binname);
            exit(0);
        }
#ifdef DEBUG
        fprintf(stderr, "Output font file %s opened OK\n",binname);
#endif

        /*****  Read in the three comment lines and then the header line
                from the source file.                                    */

        for (ic=0;ic<3;ic++)
        {
            fgets(comment,80,source);
            comment[strlen(comment)-1] = comment[strlen(comment)];
#ifdef DEBUG
            fprintf(stderr, "Comment ignored: %s\n",comment);
#endif
        }

        count = fscanf(source,"%f%f%f%f%f", &va, &vb, &vc, &vd, &ve);
        if (count != 5)
        {
            printf("Too few values on font header\n");
            exit(0);
        }

        while (getc(source) != '\n')
           ;

#ifdef DEBUG
        fprintf(stderr, "Top %g cap %g bottom %g width %g shear %g\n",
                va,vb,vc,vd,ve);
        fprintf(stderr, "Characters processed:");
#endif

        /*****  Initialize counters for reading in the character
                descriptions.                                     */

        charid = -1;
        loc = 0;
        more = 1;
        line = 3;

        for (ic=0;ic<MaxChars;ic++)
            B_char[ic].location = 0;

        do
        {
            int flag;
            float a,b,c,xref,yref;

            fscanf(source,"%d",&flag);
            line++;

            switch (flag)

            {
            case 9:     /*  new character  */
                fscanf(source,"%f%f",&a,&b);
                while (fgetc(source) != '\n')
                    ;
                fscanf(source,"%*d%f",&c);
                while (fgetc(source) != '\n')
                        ;
                line++;

                B_char[++charid].location = loc;
                B_char[charid].numcomp  = 0;
                B_char[charid].width  = c;

                xref = a;
                yref = b;

#ifdef DEBUG
                fprintf(stderr, " %d",charid+31);
#endif
                break;

            case 0: case 1: case 2: case 4:
                fscanf(source,"%f%f",&a,&b);
                while (fgetc(source) != '\n')
                  ;
                B_def[loc].x      = a - xref;
                B_def[loc].y      = b - yref;
                B_def[loc++].flag = flag;
                B_char[charid].numcomp++;
                if (loc > MaxDefs)
                {
                    printf("Character %d causes array overflow\n",
                            charid+31);
                    exit(0);
                }
                if (flag == 2)
                {
                    if (B_char[charid].numcomp > MaxComp)
                    {
                        printf("Character %d ; too many (%d) components\n",
                                 charid+31,MaxComp+1);
                        exit(0);
                    }
                }
                break;

            case -1:     /*  end of font  */
                B_char[charid].location = loc;
                more = 0;
#ifdef DEBUG
                fprintf(stderr, "\n");
#endif
                break;

            default:
                printf("Unknown BEZIER flag %d at line %d\n",flag,line);
                break;

            }

        } while (more);

        /*****  Store the font metric parameters in the font header.  */

        Bfont.top    = va;
        Bfont.cap    = vb;
        Bfont.bottom = vc;
        Bfont.width  = vd;
        Bfont.shear  = ve;
        Bfont.height = vb;
        Bfont.number = fontcode;
        Bfont.charids = charid;
        Bfont.defs = loc;

        /*****  Copy the information into the directory block and the
                 header block that goes in the binary font file.           */

        Dir.number = fontcode;
        strcpy(Dir.name,fontname);
        strcpy(Dir.filename,srcname);

#ifdef DEBUG
        fprintf(stderr, "Will store %d chars %d defs\n",charid,loc);
#endif

        fwrite ((char *) &Bfont,   sizeof(Bfont),       1,binary);
        fwrite ((char *) B_char, sizeof(*B_char), charid,binary);
        fwrite ((char *) B_def,  sizeof(*B_def),     loc,binary);

        fwrite ((char *) &Dir,    sizeof(Dir),     1,directory);

        fclose(source);
        FREE(fsrcname);

        fclose(binary);
        printf("Binary file %s created\n",srcname);
        FREE(binname);

#ifdef DEBUG
        fprintf(stderr,
                "Bezier font %d (%s) stored with %d characters, %d points\n",
                Dir.number, Dir.name, Bfont.charids, Bfont.defs);
#endif

    } while (1);

#ifdef DEBUG
    fprintf(stderr, "All data files read in\n");
#endif

    Dir.number = -1;
    fwrite ((char *) &Dir, sizeof(Dir), 1, directory);

    /* close directory file */
    fclose(directory);
    /* FREE pathname - no longer needed */
    (void)FREE(dirname);

    /* close generator file */
    fclose(generator);
    /* FREE pathname - no longer needed */
    (void)FREE(genname);

    FREE(B_char);
    FREE(B_def);

    printf("Bezier data files created\n");
    exit(0);
}
