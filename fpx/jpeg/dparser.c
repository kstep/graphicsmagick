/* -*-C-*-
******************************************************************************
*
* File:         dparser.c
* RCS:          $Header$
* Description:  Parses JPEG file into internal data structures
* Author:       Hugh P. Nguyen
* Created:      Fri Dec  4 11:04:22 1992
* Modified:     Fri Dec  4 11:04:53 1992
* Language:     C
* Package:      Hewlett-Packard JPEG Encoder/Decoder
*
* Copyright (c) 1999 Digital Imaging Group, Inc.
* For conditions of distribution and use, see copyright notice
* in Flashpix.h
*
******************************************************************************
*/

#define WINDLL

#include <stdlib.h>
#include <stdio.h>
#include "jpegconf.h"
#include "decoder.h"
#include "dbuffer.h"
#include "dectile.h"
#include "djpeg.h"
#include "dparser.h"
#include "winograd.h"
#include "chen_dct.h"
#include "dhuff.h" 
#include "fpxmem.h"

/*************************** link list utilities ****************************/
/*
 * Return NULL if memory can not be allocated.
 */
TABLE_LIST *New_Table_Element(int table_type)
  /* 0: HUFFMAN_TABLE, 1: QUANT_TABLE, -1: NONE! (KW) */
{
    TABLE_LIST *tlist;
    int check;
    
    if ((tlist = (TABLE_LIST *) FPX_malloc(sizeof(TABLE_LIST))) != NULL) {
        tlist->next = NULL;
        if (table_type == 1) {
            tlist->table = (void *) FPX_malloc(sizeof(QUANT_TABLE));
            check = 1;
        } else if (table_type == -1) {
          tlist->table = NULL;
          check = 0;
        } else {
            tlist->table = (void *) FPX_malloc(sizeof(DHUFF_TABLE));
            check = 1;
        }
        if ((tlist->table == NULL) && (check)) {
            FPX_free(tlist);
            tlist = NULL;
        }
    }
    return(tlist);
}

TABLE_LIST *Add_Element( TABLE_LIST *elem,  TABLE_LIST *table )
{
    TABLE_LIST *t;
    
    if (table == NULL) {
        return(elem);
    } else {
        for (t = table; t->next != NULL; t = t->next);
        t->next = elem;
        elem->next = NULL;
        return(table);
    }
}

void Free_Table_List(TABLE_LIST *tlist)
{
    TABLE_LIST *t;
    
    while (tlist != NULL) {
        t = tlist;
        tlist = tlist->next;
        FPX_free(t->table);
        FPX_free(t);
    }
}

unsigned int Get_Segment_Length(DB_STATE *db_state)
{
    unsigned char *p;
    int error_code;
    
    if ((p = DB_Get_Data(db_state, 2, &error_code)) == NULL) {
        return(0);
    } else {
        return(((unsigned int)p[0] << 8) + p[1]);
    }
}


/***************************  exported functions ****************************/

int DP_Get_Next_Marker(DB_STATE *db_state)
{
    unsigned char *p;
    int error_code;

    if (DB_Skip_To_Next_Marker(db_state)) {
        return(MARKER_END_FILE);
    } else {
        if ((p = DB_Get_Data(db_state, 1, &error_code)) == NULL) {
            return(MARKER_END_FILE);
        } else {
            return(CONVERT_TO_MARKER(*p));
        }
    }
}

/*
 * Return 0 if successful, otherwise one of ERROR_ types
 */
int DP_Skip(DB_STATE *db_state)
{
    unsigned char *p;
    unsigned int segment_size;
    int error_code;
    
    if ((p = DB_Get_Data(db_state, 2, &error_code)) == NULL) {
        return(error_code);
    }
    segment_size = ((unsigned int) p[0] << 8) + p[1];
    if (segment_size < 2) {
        return(ERROR_DATA);
    } else if (DB_Get_Data(db_state, segment_size - 2, &error_code) == NULL) {
        return(error_code);
    } else {
        return(0);
    }
}

unsigned char *DP_Parse_APP(
  DB_STATE *db_state,
  unsigned int *length,
  int *error_code)
{
    unsigned int len;
    
    if ((len = Get_Segment_Length(db_state)) < 2) {
        *error_code = ERROR_DATA;
        return(NULL);
    }
    *length = len - 2;
    return(DB_Get_Data(db_state, len-2, error_code));
}

/*
 * Return non negative number if successful, -1 if error
 */
long DP_Parse_DRI(
  DB_STATE *db_state,
  int *error_code)
{
    unsigned char *p;
    unsigned int len;
    
    if ((len = Get_Segment_Length(db_state)) < 2) {
        *error_code = ERROR_DATA;
        return(-1);
    }
    if ((p = DB_Get_Data(db_state, len-2, error_code)) == NULL) {
        return(-1);
    } else {
        return(((unsigned int) *p << 8) + ((unsigned int) p[1]));
    }
}

/*
 * Return pointer to newly created frame, or NULL if not successful
 */
FRAME *DP_Parse_SOF(
  DB_STATE *db_state,
  int *error_code )
{
    int length, i, maxv, maxh;
    unsigned int len;
    unsigned char *p;
    FRAME *frame;
    FRAME_COMPONENT *comp;
    
    if ((len = Get_Segment_Length(db_state)) < 2) {
        *error_code = ERROR_DATA;
        return(NULL);
    }
    
    if ((p = DB_Get_Data(db_state, len-2, error_code)) == NULL) return(NULL);
    
    if ((frame = (FRAME *) FPX_malloc(sizeof(FRAME))) == NULL) {
        *error_code = ERROR_MEM;
        return(NULL);
    }
    frame->precision = *p++;
    frame->height  = *p++ << 8;
    frame->height += *p++;
    frame->width   = *p++ << 8;
    frame->width  += *p++;
    frame->ncomps  = *p++;

    if (frame->height == 0) {
        FPX_free(frame);
        *error_code = ERROR_DNL;
        return(NULL);
    }

    if ((frame->comps = (FRAME_COMPONENT *) FPX_calloc(frame->ncomps,
        sizeof(FRAME_COMPONENT))) == NULL) {
        FPX_free(frame);
        *error_code = ERROR_MEM;
        return(NULL);
    }

    /* Parse components and compute max hsampling and vsampling */
    maxh = maxv = 0;
    for (comp = frame->comps, i = 0; i < frame->ncomps; i++, comp++) {
        comp->ident = *p++;
        comp->hsampling = ((int) *p) >> 4;
        comp->vsampling = *p++ & 0xf;
        comp->quant_sel = *p++;
        if (maxh < comp->hsampling) maxh = comp->hsampling;
        if (maxv < comp->vsampling) maxv = comp->vsampling;
    }

    /* Compute component width and height */
    for (comp = frame->comps, i = 0; i < frame->ncomps; i++, comp++) {
        comp->width = (frame->width *comp->hsampling+maxh-1) / maxh;
        comp->height= (frame->height*comp->vsampling+maxv-1) / maxv;
    }

    /* Compute number of MCU */
    length = maxh * 8;
    frame->horMCU =  (frame->width + (length-1)) / length;
    length = maxv * 8;
    frame->totalMCU= (long)frame->horMCU*((frame->height+(length-1))/length);

    return(frame);
}

/*
 * Parse DQT segment into a list of QUANT_TABLEs.
 * Return NULL if error.
 */
TABLE_LIST *DP_Parse_DQT(
  DB_STATE *db_state,
  int *ntables,       /* number of tables */
  int type,           /* 0 = Chen, 1 = Lee, 2 = Winograd, 
                 3 = Feig, 4 = Pruned Winograd */
  int *error_code)
{
    int i, j;
    unsigned int len;
    unsigned char *p;
    TABLE_LIST *tlist = NULL, *t;
    QUANT_TABLE *table;
    
    if ((len = Get_Segment_Length(db_state)) < 2) {
        *error_code = ERROR_DATA;
        return(NULL);
    }
    if ((p = DB_Get_Data(db_state, len-2, error_code)) == NULL) return(NULL);
    *ntables = (len-2) / 65;
    
    /* Allocate Memory for all tables */
    for (i = 0; i < *ntables; i++) {
        if ((t = New_Table_Element(1)) == NULL) {
            Free_Table_List(tlist);
            *error_code = ERROR_MEM;
            return(NULL);
        } else {
            table = (QUANT_TABLE *) t->table;
            table->precision = ((int) *p) >> 4;
            table->ident     = *p++ & 0xf;
            for (j = 0; j < 64; j++) table->elements[j] = *p++;
            if (type == 0) {    /* Chen */
                Fill_Chen_Quant_Table(table->elements, table->elements);
            } else {            /* Winograd or Pruned Winograd */
                Fill_Winograd_Quant_Table2(table->elements, table->elements);
            }
            for (j = 64; j < 80; j++) {
                table->elements[j] = 0;
            }
            tlist = Add_Element(t, tlist);
        }
    }
    return(tlist);
}

TABLE_LIST *DP_Parse_DHT(
  DB_STATE *db_state,
  int *ntables,       /* number of tables */
  int *error_code )
{
    int sum, i;
    long length;
    unsigned int len;
    unsigned char *p;
    TABLE_LIST *tlist = NULL, *t;
    DHUFF_TABLE *table;
    
    if ((len = Get_Segment_Length(db_state)) < 2) {
        *error_code = ERROR_DATA;
        return(NULL);
    }
    
    *ntables = 0;
    if ((p = DB_Get_Data(db_state, len-2, error_code)) == NULL) return(NULL);
    
    length = len-2;
    while (length > 0L) {
        if ((t = New_Table_Element(0)) == NULL) { /* KW:  -1 now indicates no allocation! */
            Free_Table_List(tlist);
            *error_code = ERROR_MEM;
            return(NULL);
        } else {
            if ((table = Build_Huffman_Table((*p & 0xf0) >> 4, *p & 0x0f,
                p+1, p+17)) == NULL) {
                Free_Table_List(tlist);
                FPX_free(t->table);
                FPX_free(t);
                *error_code = ERROR_MEM;
                return(NULL);
            } else {
                FPX_free(t->table);
                t->table = (void *) table;
                tlist = Add_Element(t, tlist);
                (*ntables)++;
                for (p++, i = sum = 0; i < 16; i++) {
                    sum += *p++;
                }
                p += sum;
                length -= sum + 17;
            }
        }
    }
    return(tlist);
}

SCAN *DP_Parse_SOS(
  DB_STATE *db_state,
  FRAME *frame,
  DHUFF_TABLE *dc_huffman_tables[],
  DHUFF_TABLE *ac_huffman_tables[],
  QUANT_TABLE *quant_tables[],
  int *error_code)
{
    int i, j;
    unsigned int len;
    unsigned char *p;
    SCAN *scan;
    SCAN_COMPONENT *comp;
    int comp_sel, dctab_sel, actab_sel;
    
    if ((len = Get_Segment_Length(db_state)) < 2) {
        *error_code = ERROR_DATA;
        return(NULL);
    }
    
    if ((p = DB_Get_Data(db_state, len-2, error_code)) == NULL) return(NULL);

    if ((scan = (SCAN *) FPX_malloc(sizeof(SCAN))) == NULL) {
        *error_code = ERROR_MEM;
        return(NULL);
    }

    scan->ncomps     = *p++;
    if ((scan->comps = (SCAN_COMPONENT *) FPX_calloc(scan->ncomps,
        sizeof(SCAN_COMPONENT))) == NULL) {
        FPX_free(scan);
        *error_code = ERROR_MEM;
        return(NULL);
    }
    for (comp = scan->comps, i = 0; i < scan->ncomps; i++, comp++) {
        comp_sel  = *p++;
        dctab_sel = ((int) *p) >> 4;
        actab_sel = *p++ & 0xf;
        if ((dctab_sel < 0) || (dctab_sel > 1) ||
            (actab_sel < 0) || (actab_sel > 1)) {
            FPX_free(scan->comps);
            FPX_free(scan);
            *error_code = ERROR_DATA;
            return(NULL);
        }

        for (j = 0; j < frame->ncomps; j++) { /* Search component ident */
            if (comp_sel == frame->comps[j].ident) {
                break;
            }
        }
        if ((j >= frame->ncomps) ||             /* Search Failed */
            (frame->comps[j].quant_sel < 0) ||  /* or out of range */
            (frame->comps[j].quant_sel > 1)) {
            FPX_free(scan->comps);
            FPX_free(scan);
            *error_code = ERROR_DATA;
            return(NULL);
        } else {
            comp->comp = j;
            comp->hsampling = frame->comps[j].hsampling;
            comp->vsampling = frame->comps[j].vsampling;
            comp->dc_table = dc_huffman_tables[dctab_sel];
            comp->ac_table = ac_huffman_tables[actab_sel];
            comp->quant_table = quant_tables[frame->comps[j].quant_sel];
        }
    }

    scan->start_spec = *p++;
    scan->end_spec   = *p++;
    scan->approx_high= ((int) *p) >> 4;
    scan->approx_low = *p++ & 0xf;

    return(scan);
}


void DP_Free_Frame(FRAME *frame)
{
    if (frame != NULL) {
        FPX_free(frame->comps);
        FPX_free(frame);
    }
}


void DP_Free_Scan(SCAN *scan)
{
    if (scan != NULL) {
        FPX_free(scan->comps);
        FPX_free(scan);
    }
}


void DP_Free_Table_Links(TABLE_LIST *tlist)
{
    TABLE_LIST *t;
    
    while (tlist != NULL) {
        t = tlist;
        tlist = tlist->next;
        FPX_free(t);
    }
}
