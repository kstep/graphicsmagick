/*
 *  jbgtopbm - JBIG to Portable Bitmap converter
 *
 *  Markus Kuhn -- mkuhn@acm.org
 *
 *  $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "jbig.h"

#define BUFSIZE 8192

const char usage_msg[] = "JBIGtoPBM converter " JBG_VERSION " -- "
"reads a bi-level image entity (BIE) as input file\n\n"
"usage: %s [<options>] [<input-file> | -  [<output-file>]]\n\n"
"options:\n\n"
"  -x number\tif possible decode only up to a resolution layer not\n"
"\t\twider than the given number of pixels\n"
"  -y number\tif possible decode only up to a resolution layer not\n"
"\t\thigher than the given number of pixels\n"
"  -b\t\tuse binary code for multiple bitplanes (default: Gray code)\n"
"  -d\t\tdiagnose BIE, print content of header and exit\n"
"  -p number\tdecode only one single bit plane (0 = first plane)\n\n";

char *progname;                  /* global pointer to argv[0] */


/*
 * Print usage message and abort
 */
static void usage(void)
{
  fprintf(stderr, usage_msg, progname);
  exit(1);
}


/*
 * Call-back routine for merged image output
 */
void write_it(unsigned char *data, size_t len, void *file)
{
  fwrite(data, len, 1, (FILE *) file);
}


/*
 *
 */
void diagnose_bie(FILE *f)
{
  unsigned char bih[20];
  int len;
  unsigned long xd, yd, l0;

  len = fread(bih, 1, 20, f);
  if (len < 20) {
    printf("Input file is %d < 20 bytes long and does therefore not "
	   "contain an intact BIE header!\n", len);
    return;
  }

  printf("Decomposition of BIH:\n\n  DL = %d\n  D  = %d\n  P  = %d\n"
	 "  -  = %d\n  XD = %lu\n  YD = %lu\n  L0 = %lu\n  MX = %d\n"
	 "  MY = %d\n",
	 bih[0], bih[1], bih[2], bih[3],
	 xd = ((unsigned long) bih[ 4] << 24) | ((unsigned long)bih[ 5] << 16)|
	 ((unsigned long) bih[ 6] <<  8) | ((unsigned long) bih[ 7]),
	 yd = ((unsigned long) bih[ 8] << 24) | ((unsigned long)bih[ 9] << 16)|
	 ((unsigned long) bih[10] <<  8) | ((unsigned long) bih[11]),
	 l0 = ((unsigned long) bih[12] << 24) | ((unsigned long)bih[13] << 16)|
	 ((unsigned long) bih[14] <<  8) | ((unsigned long) bih[15]),
	 bih[16], bih[17]);
  printf("  order   = %d %s%s%s%s%s\n", bih[18],
	 bih[18] & JBG_HITOLO ? " HITOLO" : "",
	 bih[18] & JBG_SEQ ? " SEQ" : "",
	 bih[18] & JBG_ILEAVE ? " ILEAVE" : "",
	 bih[18] & JBG_SMID ? " SMID" : "",
	 bih[18] & 0xf0 ? " other" : "");
  printf("  options = %d %s%s%s%s%s%s%s%s\n", bih[18],
	 bih[19] & JBG_LRLTWO ? " LRLTWO" : "",
	 bih[19] & JBG_VLENGTH ? " VLENGTH" : "",
	 bih[19] & JBG_TPDON ? " TPDON" : "",
	 bih[19] & JBG_TPBON ? " TPBON" : "",
	 bih[19] & JBG_DPON ? " DPON" : "",
	 bih[19] & JBG_DPPRIV ? " DPPRIV" : "",
	 bih[19] & JBG_DPLAST ? " DPLAST" : "",
	 bih[19] & 0x80 ? " other" : "");
  printf("\n  %lu stripes, %d layers, %d planes\n\n",
	 ((yd >> bih[1]) +  ((((1UL << bih[1]) - 1) & xd) != 0) + l0 - 1) / l0,
	 bih[1] - bih[0], bih[2]);

  return;
}


int main (int argc, char **argv)
{
  FILE *fin = stdin, *fout = stdout;
  const char *fnin = "<stdin>", *fnout = "<stdout>";
  int i, j, result;
  int all_args = 0, files = 0;
  struct jbg_dec_state s;
  char *buffer;
  unsigned char *p;
  size_t len, cnt;
  unsigned long xmax = 4294967295UL, ymax = 4294967295UL;
  int plane = -1, use_graycode = 1, diagnose = 0;

  buffer = malloc(BUFSIZE);
  if (!buffer) {
    printf("Sorry, not enough memory available!\n");
    exit(1);
  }

  /* parse command line arguments */
  progname = argv[0];
  for (i = 1; i < argc; i++) {
    if (!all_args && argv[i][0] == '-')
      if (argv[i][1] == '\0' && files == 0)
        ++files;
      else
        for (j = 1; j > 0 && argv[i][j]; j++)
          switch(tolower(argv[i][j])) {
          case '-' :
            all_args = 1;
            break;
	  case 'b':
	    use_graycode = 0;
            break;
	  case 'd':
	    diagnose = 1;
            break;
          case 'x':
            if (++i >= argc) usage();
            xmax = atol(argv[i]);
            j = -1;
            break;
          case 'y':
            if (++i >= argc) usage();
            ymax = atol(argv[i]);
            j = -1;
            break;
          case 'p':
            if (++i >= argc) usage();
            plane = atoi(argv[i]);
            j = -1;
            break;
          default:
            usage();
          }
    else
      switch (files++) {
      case 0:
        if (argv[i][0] != '-' || argv[i][1] != '\0') {
          fnin = argv[i];
          fin = fopen(fnin, "rb");
          if (!fin) {
            fprintf(stderr, "Can't open input file '%s", fnin);
            perror("'");
            exit(1);
          }
        }
	if (diagnose) {
	  diagnose_bie(fin);
	  exit(0);
	}
        break;
      case 1:
        fnout = argv[i];
        fout = fopen(fnout, "wb");
        if (!fout) {
          fprintf(stderr, "Can't open input file '%s", fnout);
          perror("'");
          exit(1);
        }
        break;
      default:
        usage();
      }
  }

  /* send input file to decoder */
  jbg_dec_init(&s);
  jbg_dec_maxsize(&s, xmax, ymax);
  result = JBG_EAGAIN;
  do {
    len = fread(buffer, 1, BUFSIZE, fin);
    if (!len) break;
    cnt = 0;
    p = (unsigned char *) buffer;
    while (len > 0 && (result == JBG_EAGAIN || result == JBG_EOK)) {
      result = jbg_dec_in(&s, p, len, &cnt);
      p += cnt;
      len -= cnt;
    }
  } while (result == JBG_EAGAIN || result == JBG_EOK);
  if (ferror(fin)) {
    fprintf(stderr, "Problem while reading input file '%s", fnin);
    perror("'");
    if (fout != stdout) {
      fclose(fout);
      remove(fnout);
    }
    exit(1);
  }
  if (result != JBG_EOK && result != JBG_EOK_INTR) {
    fprintf(stderr, "Problem with input file '%s': %s\n",
	    fnin, jbg_strerror(result, JBG_EN));
    if (fout != stdout) {
      fclose(fout);
      remove(fnout);
    }
    exit(1);
  }
  if (plane >= 0 && jbg_dec_getplanes(&s) <= plane) {
    fprintf(stderr, "Image has only %d planes!\n", jbg_dec_getplanes(&s));
    if (fout != stdout) {
      fclose(fout);
      remove(fnout);
    }
    exit(1);
  }

  if (jbg_dec_getplanes(&s) == 1 || plane >= 0) {
    /* write PBM output file */
    fprintf(fout, "P4\n%ld %ld\n", jbg_dec_getwidth(&s),
	    jbg_dec_getheight(&s));
    fwrite(jbg_dec_getimage(&s, plane < 0 ? 0 : plane), 1,
	   jbg_dec_getsize(&s), fout);
  } else {
    /* write PGM output file */
    fprintf(fout, "P5\n%ld %ld\n%d\n", jbg_dec_getwidth(&s),
	    jbg_dec_getheight(&s), (1<<jbg_dec_getplanes(&s)) - 1);
    jbg_dec_merge_planes(&s, use_graycode, write_it, fout);
  }

  /* check for file errors and close fout */
  if (ferror(fout) || fclose(fout)) {
    fprintf(stderr, "Problem while writing output file '%s", fnout);
    perror("'");
    exit(1);
  }

  jbg_dec_free(&s);

  return 0;
}
