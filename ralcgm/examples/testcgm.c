/*  Based on GTS-GRAL Example Program GTEST */
/*    Last Modified on  11 Oct 1990  at  13:05:20     */

/*  Include everything for CGM library routines */

#include "cgmlib.h"

char *fontl[3] = { "hardware",
                   "Hershey simplex",
                   "Bezier Times Roman" };

void parse ();

main ( argc, argv )
int argc;
char **argv;
{
   register int i, j;
   Enum type;
   char filename[80];
   Colour col;
   Code aspect[17];
   Enum value[17];
   Point line[200], p, q, r;
   Vector v, w;
   Indexcolour cell[100];

   strcpy(filename, "");

/*  Parse arguments */
   parse(argv, argc, &type, filename);

   printf ( "Output type: %d filename: %s\n", type, filename);

/*  Store points in local encoding */

/*  Open CGM specifying Encoding and file name (blank = stdout)  */

   cgminit ( type, filename);
   cgmverbose = FALSE;

/*                      Start of  Metafile Description
 *---------------------------------------------------------------------
 */

   m_begmf("test");
   m_mfversion ( 1 );
   m_mfdesc("test CGM Library");
   m_vdctype(REAL);
/*
   m_fontlist( 3, fontl );
*/

   m_begmfdefaults();
      aspect[0] = ALL; value[0] = BUNDLED;
      m_asf ( 1, aspect, value );
      switch ( type )
      {
         case CHARACTER:
            m_vdcrealprec ( 16, -10, -16, ALLOWED );
            break;
         default:
            break;
      }
   m_endmfdefaults();

/*                      End of Metafile Description
 *---------------------------------------------------------------------
 *                      First Picture - POLYLINE
 */

   m_begpic("Individual Attributes");
   m_begpicbody();

      aspect[0] = ALL; value[0] = INDIV;
      m_asf ( 1, aspect, value );
/*  Polylines */

      line[0].x = 0.05; line[0].y = 0.05;
      line[1].x = 0.40; line[1].y = 0.05;
      line[2].x = 0.40; line[2].y = 0.40;
      line[3].x = 0.05; line[3].y = 0.40;
      line[4].x = 0.05; line[4].y = 0.05;
      m_linetype ( 1 );
      m_linewidth ( 1.0 );
      col.index = 1;
      m_linecolr ( col );
      m_line ( 5, line );
      for ( i=0; i<5; i++ )
      {
         line[i].x += 0.02; line[i].y += 0.02;
      }
      m_linetype ( 2 );
      col.index = 2;
      m_linecolr ( col );
      m_line ( 5, line );
      for ( i=0; i<5; i++ )
      {
         line[i].x += 0.02; line[i].y += 0.02;
      }
      m_linetype ( 3 );
      col.index = 3;
      m_linecolr ( col );
      m_line ( 5, line );
      for ( i=0; i<5; i++ )
      {
         line[i].x += 0.02; line[i].y += 0.02;
      }
      m_linetype ( 4 );
      col.index = 4;
      m_linecolr ( col );
      m_line ( 5, line );
      line[0].x = 0.15; line[1].x = 0.35;
      for ( i=2; i<175; i+=2 )
      {
         line[i].x = line[0].x;
         line[i+1].x = line[1].x;
      }
      line[0].y = 0.15;
      for ( i=1; i<175; i++ )
      {
         line[i].y = line[i-1].y + 0.001;
      }
      m_linetype ( 1 );
      col.index = 1;
      m_linecolr ( col );
      m_line(175, line);

/* Markers */
      line[0].x = 0.7; line[0].y = 0.75;
      line[1].x = 0.8; line[1].y = 0.75;
      line[2].x = 0.9; line[2].y = 0.75;
      m_markertype ( 1 );
      m_markersize ( 1.0 );
      col.index = 1;
      m_markercolr ( col );
      m_marker (3, line );
      line[0].y = 0.80;
      line[1].y = 0.80;
      line[2].y = 0.80;
      m_markertype ( 2 );
      col.index = 2;
      m_markercolr ( col );
      m_marker (3, line );
      line[0].y = 0.85;
      line[1].y = 0.85;
      line[2].y = 0.85;
      m_markertype ( 3 );
      col.index = 3;
      m_markercolr( col );
      m_marker (3, line );
      line[0].y = 0.90;
      line[1].y = 0.90;
      line[2].y = 0.90;
      m_markertype ( 4 );
      col.index = 4;
      m_markercolr( col );
      m_marker (3, line );
      line[0].y = 0.95;
      line[1].y = 0.95;
      line[2].y = 0.95;
      m_markertype ( 5 );
      col.index = 5;
      m_markercolr( col );
      m_marker (3, line );

/* Text */
      m_charheight( 0.04 );
      v.x = -0.05; v.y = 1.0;
      w.x = 1.0; w.y = 0.05;
      m_charori( v, w );
      col.index = 1;
      m_textcolr( col );
      m_textprec( STRING );
      p.x = 0.05; p.y = 0.60;
      m_text(p, FINAL, "STRING PRECISION");
      m_textprec( CHAR );
      col.index = 2;
      m_textcolr( col );
      p.y = 0.675;
      m_text(p, FINAL, "CHARACTER PRECISION");
      col.index = 3;
      m_textcolr( col );
      m_textprec( STROKE );
      p.y = 0.75;
      m_text(p, FINAL, "STROKE PRECISION");

/* Fill area */
      line[0].x = 0.5;  line[0].y = 0.05;
      line[1].x = 0.5;  line[1].y = 0.2;
      line[2].x = 0.65; line[2].y = 0.05;
      m_intstyle( HOLLOW );
      col.index = 1;
      m_fillcolr( col );
      m_polygon( 3, line );
      line[0].x = 0.75;  line[0].y = 0.05;
      line[1].x = 0.95;  line[1].y = 0.2;
      line[2].x = 0.75;  line[2].y = 0.2;
      line[3].x = 0.95;  line[3].y = 0.05;
      m_intstyle( SOLID );
      col.index = 2;
      m_fillcolr( col );
      m_polygon( 4, line );
      line[0].x = 0.5;   line[0].y = 0.3;
      line[1].x = 0.6;   line[1].y = 0.3;
      line[2].x = 0.6;   line[2].y = 0.35;
      line[3].x = 0.95;  line[3].y = 0.35;
      line[4].x = 0.95;  line[4].y = 0.4;
      line[5].x = 0.7;   line[5].y = 0.4;
      line[6].x = 0.7;   line[6].y = 0.3;
      line[7].x = 0.65;  line[7].y = 0.3;
      line[8].x = 0.65;  line[8].y = 0.4;
      m_intstyle( HATCH );
      m_hatchindex( -3 );
      col.index = 3;
      m_fillcolr( col );
      m_polygon( 9, line );

/* Cell array */
      p.x = 0.55; p.y = 0.45;
      q.x = 0.95; q.y = 0.65;
      r.x = 0.95; r.y = 0.45;
      for ( i=0; i<5*8; i++ ) cell[i] = 1;
      for ( i=1; i<4; i++ )
      {
         cell[i+5] = 0;
         cell[i+10] = 2;
         cell[i+15] = 3;
         cell[i+20] = 0;
         cell[i+25] = 2;
         cell[i+30] = 3;
      }
      m_cellarray(p, q, r, 5, 8, cell, NULL);

   m_endpic();

/*---------------------------------------------------------------------
 *                      Second Picture - All primitives
 */

   m_begpic("Bundled Attributes");
   m_begpicbody();

/*  Polylines */

      line[0].x = 0.05; line[0].y = 0.05;
      line[1].x = 0.40; line[1].y = 0.05;
      line[2].x = 0.40; line[2].y = 0.40;
      line[3].x = 0.05; line[3].y = 0.40;
      line[4].x = 0.05; line[4].y = 0.05;
      m_lineindex ( 1 );
      m_line ( 5, line );
      for ( i=0; i<5; i++ )
      {
         line[i].x += 0.02; line[i].y += 0.02;
      }
      m_lineindex ( 2 );
      m_line ( 5, line );
      for ( i=0; i<5; i++ )
      {
         line[i].x += 0.02; line[i].y += 0.02;
      }
      m_lineindex ( 3 );
      m_line ( 5, line );
      for ( i=0; i<5; i++ )
      {
         line[i].x += 0.02; line[i].y += 0.02;
      }
      m_lineindex ( 4 );
      m_line ( 5, line );
      line[0].x = 0.15; line[1].x = 0.35;
      for ( i=2; i<175; i+=2 )
      {
         line[i].x = line[0].x;
         line[i+1].x = line[1].x;
      }
      line[0].y = 0.15;
      for ( i=1; i<175; i++ )
      {
         line[i].y = line[i-1].y + 0.001;
      }
      m_lineindex ( 1 );
      m_line(175, line);

/* Markers */
      line[0].x = 0.7; line[0].y = 0.75;
      line[1].x = 0.8; line[1].y = 0.75;
      line[2].x = 0.9; line[2].y = 0.75;
      m_markerindex(1);
      m_marker (3, line );
      line[0].y = 0.80;
      line[1].y = 0.80;
      line[2].y = 0.80;
      m_markerindex(2);
      m_marker (3, line );
      line[0].y = 0.85;
      line[1].y = 0.85;
      line[2].y = 0.85;
      m_markerindex(3);
      m_marker (3, line );
      line[0].y = 0.90;
      line[1].y = 0.90;
      line[2].y = 0.90;
      m_markerindex(4);
      m_marker (3, line );
      line[0].y = 0.95;
      line[1].y = 0.95;
      line[2].y = 0.95;
      m_markerindex(5);
      m_marker (3, line );

/* Text */
      m_charheight( 0.04 );
      v.x = -0.05; v.y = 1.0;
      w.x = 1.0; w.y = 0.05;
      m_charori( v, w );
      m_textindex(1);
      p.x = 0.05; p.y = 0.60;
      m_text(p, FINAL, "STRING PRECISION");
      p.y = 0.675;
      m_textindex(2);
      m_text(p, FINAL, "CHARACTER PRECISION");
      p.y = 0.75;
      m_textindex(3);
      m_text(p, FINAL, "STROKE PRECISION");

/* Fill area */
     line[0].x = 0.5;  line[0].y = 0.05;
     line[1].x = 0.5;  line[1].y = 0.2;
     line[2].x = 0.65; line[2].y = 0.05;
     m_fillindex( 1 );
     m_polygon( 3, line );
     line[0].x = 0.75;  line[0].y = 0.05;
     line[1].x = 0.95;  line[1].y = 0.2;
     line[2].x = 0.75;  line[2].y = 0.2;
     line[3].x = 0.95;  line[3].y = 0.05;
     m_fillindex( 2 );
     m_polygon( 4, line );
     line[0].x = 0.5;   line[0].y = 0.3;
     line[1].x = 0.6;   line[1].y = 0.3;
     line[2].x = 0.6;   line[2].y = 0.35;
     line[3].x = 0.95;  line[3].y = 0.35;
     line[4].x = 0.95;  line[4].y = 0.4;
     line[5].x = 0.7;   line[5].y = 0.4;
     line[6].x = 0.7;   line[6].y = 0.3;
     line[7].x = 0.65;  line[7].y = 0.3;
     line[8].x = 0.65;  line[8].y = 0.4;
     m_fillindex( 3 );
     m_polygon( 9, line );

/* Cell array */
     p.x = 0.55; p.y = 0.45;
     q.x = 0.95; q.y = 0.65;
     r.x = 0.95; r.y = 0.45;
     for ( i=0; i<5*8; i++ ) cell[i] = 1;
     for ( i=1; i<4; i++ )
     {
        cell[i+5] = 0;
        cell[i+10] = 2;
        cell[i+15] = 3;
        cell[i+20] = 0;
        cell[i+25] = 2;
        cell[i+30] = 3;
     }
     m_cellarray(p, q, r, 5, 8, cell, NULL);

   m_endpic();
/*--------------------------------------------------------------------
 *                     Close CGM
 */
   m_endmf();

   exit(0);

}

void parse( argv, argc, type, filename)
int argc;
char **argv, *filename;
Enum *type;
{
   int i;
   char *arg, *ext = "cgm", option;

   argv++;
   *type = CLEAR_TEXT;

   for (i = 1; i< argc; i++)
   {
      arg = *argv++;
      if ( arg[0] == '-' )
      {
         option = toupper(arg[1]);
         switch ( option )
         {
/* Options */
           case 'E':
              cgmerrcount = !cgmerrcount;
              break;
           case 'G':
              cgmralgks = !cgmralgks;
              break;
           case 'V':
              cgmverbose = !cgmverbose;
              break;

/*  Drivers */
           case 'C':
              strcpy(ext,"chr");
              *type = CHARACTER;
              break;
           case 'N':
              strcpy(ext,"chr");
              *type = NATIVE;
              break;
           case 'B':
              strcpy(ext,"bin");
              *type = BINARY;
              break;
#ifdef IGL
           case 'I':
              *type = IGL;
              break;
#endif
#ifdef POSTSCRIPT
           case 'P':
              strcpy(ext,"ps");
              *type = POSTSCRIPT;
              break;
#endif
           case 'T':
              strcpy(ext,"txt");
              *type = CLEAR_TEXT;
              break;
#ifdef XW
           case 'X':
              *type = XW;
              break;
#endif
           default:
              break;
         }
      }
      else strcpy( filename, &arg[0] );
   }
   if ( filename[0] != '\0' )
   {
      strcat(filename,FILESEP);
      strcat(filename,ext);
   }

   return;
}
