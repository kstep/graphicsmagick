/*	Clip.h	Release : 2.2	Date : 12/09/93	by sk	*/
/*-----------------------------------------------
 *	Clip.h
 *
 *	Headerfile zum Clipping nach Liang-Barsky
 */

#ifndef _Dt_CLIP_H
#define _Dt_CLIP_H

#ifdef __cplusplus
extern "C" {
#endif

#define CLIP_DRAW 1
#define CLIP_NODRAW 0

/*  Der Rœckgabewert der Funktion ist entweder CLIP_DRAW, dann
 *  muﬁ etwas gezeichnet werden und die Pointer auf die
 *  Koordinaten zeigen auf die korrigierten Werte, 
 *	oder CLIP_NODRAW , dann sind die Pointerinhalte undefiniert,
 *  weil nichts zu zeichnen ist
 */

extern short DtClipLine(
	double xmin, double ymin,	/* Linke untere Ecke des Clip-Bereichs */
	double xmax, double ymax,	/* Rechte obere Ecke des Clip-Bereichs */
	double *x1, double *y1,		/* Pointer auf Startpunkt der Linie */
	double *x2, double *y2);	/* Pointer auf Endpunkt der Linie */

extern short DtClipFilledBox(
	double xmin, double ymin,	/* Linke untere Ecke des Clip-Bereichs */
	double xmax, double ymax,	/* Rechte obere Ecke des Clip-Bereichs */
	double *x1, double *y1,		/* Pointer auf Startpunkt der Box */
	double *x2, double *y2);	/* Pointer auf Endpunkt der Box */

#ifdef __cplusplus
}
#endif

#endif
