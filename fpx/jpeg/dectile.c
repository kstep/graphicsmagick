/* -*-C-*-
******************************************************************************
*
* File:         dectile.c
* RCS:          $Header$
* Description:  performs JPEG decoding.
* Author:       Gregory S. Yovanof
* Created:      Sat Jul 15 11:04:22 1995
* Modified:     Sat Jul 15 11:04:22 1995
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
#include <string.h>
#include "jpegconf.h"
#include "decoder.h"
#include "dbuffer.h"
#include "dectile.h"
#include "djpeg.h"
#include "dparser.h"
#include "fpxmem.h"

static int warning_type = 0;

/*
 * Recoverable Error.  Error found in the data stream, and it is recoverable.
 */
void warning(int type)
{
  if (warning_type != WARNING_EOF) warning_type = type;
}

#ifdef DEBUG
/*
 * Programming Error (i.e. something wrong with the program).
 */
void debug(message)
char *message;
{
  fprintf(stderr,"**** SYSTEM ERROR: %s\n", message);
  exit(1);
}
#endif


#ifdef PRINT_INFO
void Print_Marker(int marker)
{
  switch (marker) {
  case MARKER_SOF0:
    printf("SOF\n");
    break;
  case MARKER_SOF1:
  case MARKER_SOF2:
  case MARKER_SOF3:
  case MARKER_SOF5:
  case MARKER_SOF6:
  case MARKER_SOF7:
  case MARKER_SOF8:
  case MARKER_SOF9:
  case MARKER_SOFA:
  case MARKER_SOFB:
  case MARKER_SOFD:
  case MARKER_SOFE:
  case MARKER_SOFF:
    printf("SOF%X\n", marker-MARKER_SOF0);
    break;
  case MARKER_DHT:
    printf("DHT\n");
    break;
  case MARKER_DAC:
    printf("DAC\n");
    break;
  case MARKER_RST0:
  case MARKER_RST1:
  case MARKER_RST2:
  case MARKER_RST3:
  case MARKER_RST4:
  case MARKER_RST5:
  case MARKER_RST6:
  case MARKER_RST7:
    printf("RST%d\n", marker-MARKER_RST0);
    break;
  case MARKER_SOI:
    printf("SOI\n");
    break;
  case MARKER_EOI:
    printf("EOI\n");
    break;
  case MARKER_SOS:
    printf("SOS\n");
    break;
  case MARKER_DQT:
    printf("DQT\n");
    break;
  case MARKER_DNL:
    printf("DNL\n");
    break;
  case MARKER_DRI:
    printf("DRI\n");
    break;
  case MARKER_DHP:
    printf("DHP\n");
    break;
  case MARKER_EXP:
    printf("EXP\n");
    break;
  case MARKER_APP:
    printf("APP\n");
    break;
  case MARKER_JPG:
    printf("JPG\n");
    break;
  case MARKER_COM:
    printf("COM\n");
    break;
  case MARKER_END_FILE: /* not a marker, simply means EOF is reached */
    break;
  case MARKER_NONE:
  default:
    printf("Unknown marker '0x%X'\n", marker);
    break;
  }
}
#endif


int Get_Huffman_Tables(DB_STATE *db_state,
DHUFF_TABLE *huffman_tables_dc[4],
DHUFF_TABLE *huffman_tables_ac[4],
int *total_ntables)
{
  int i, error_code;
  TABLE_LIST *tlist, *t;
  DHUFF_TABLE *table;
  int ntables;

  if ((tlist = DP_Parse_DHT(db_state, &ntables, &error_code)) == NULL) {
    return(error_code);
  }

  *total_ntables += ntables;

  for (t = tlist, i = 0; i < ntables; i++, t = t->next) {
    table = (DHUFF_TABLE *) t->table;
    if ((table->ident != 0) && (table->ident != 1)) {
      /* Free only tables that are not assigned */
      for (; i < ntables; i++, t = t->next) {
        if(t->table)          /* CHG_JPEG_MEM_FIX - added check & clear */
          FPX_free(t->table);
        t->table = NULL;
      }
      DP_Free_Table_Links(tlist);
      return(ERROR_DATA);
    }
    if (table->huff_class) {             /* AC table */
      if (huffman_tables_ac[table->ident] != NULL) {
        FPX_free(huffman_tables_ac[table->ident]);
        huffman_tables_ac[table->ident] = NULL;
      }
      huffman_tables_ac[table->ident] = table;
    } else {                        /* DC table */
      if (huffman_tables_dc[table->ident] != NULL) {
        FPX_free(huffman_tables_dc[table->ident]);
        huffman_tables_dc[table->ident] = NULL;
      }
      huffman_tables_dc[table->ident] = table;
    }

#ifdef PRINT_INFO
    printf("    Class %d - Ident %d\n",table->huff_class,
        table->ident);
#endif

    /* Dump Huffman table & trees */
#ifdef PRINT_INFO_MORE
    {
      int j, k;
      HUFFMAN_TREE  *huff_tree;
      HUFFMAN_ELEM  *huff_elem;
      int *huff_val;

      huff_val  = table->huffval;
      huff_elem = table->huffelem;
      for (j = 0; j < 256; j++) {
        if (huff_elem[j].codelen) {
          printf("%3d -> %3d (%d bits)\n", j,
              huff_elem[j].value, huff_elem[j].codelen);
        } else {
          printf("%3d TREE:\n", j);
          huff_tree = huff_elem[j].hufftree;
          for (k = 0; k < 8; k++) {
            if (huff_tree->maxcode[k] != -1) {
              int l, ind;
              ind = huff_tree->valptr[k] + huff_tree->mincode[k];
              for (l = huff_tree->mincode[k];
                  l <= huff_tree->maxcode[k]; l++) {
                printf("    %3d -> %3d\n", l,
                    huff_val[ind++]);
              }
            }
          }
        }
      }
    }
#endif
  }
  DP_Free_Table_Links(tlist);
  return(0);
}

int Get_Quantization_Tables(DB_STATE *db_state,
QUANT_TABLE *quant_tables[4],
int *numb_quant_tables,
int method)
{
  int i, ntables, error_code;
  TABLE_LIST *tlist, *t;
  QUANT_TABLE *table;

  if ((tlist =DP_Parse_DQT(db_state,&ntables,method,&error_code))==NULL){
    return(error_code);
  }

  for (t = tlist, i = 0; i < ntables; i++, t = t->next) {
    table = (QUANT_TABLE *) t->table;
    if ((table->ident != 0) && (table->ident != 1)
        && (table->ident != 2) && (table->ident != 3)) {
      /* Free only tables that are not assigned */
      for (; i < ntables; i++, t = t->next) {
        FPX_free(t->table);
        t->table = NULL;      /* CHG_JPEG_MEM_FIX - added clear */
      }
      DP_Free_Table_Links(tlist);
      return(ERROR_DATA);
    }
    if (quant_tables[table->ident] != NULL) {
      FPX_free(quant_tables[table->ident]);
    }
    quant_tables[table->ident] = table;

    *numb_quant_tables = table->ident+1;

#ifdef PRINT_INFO
    if (table->precision==0) printf("    Precision 8-bit - ");
    else printf("    Precision 16-bit - ");
    printf("tabIdent %d\n", table->ident);
#endif

    /* Dump Quantization table */
#ifdef PRINT_INFO_MORE
    {
      int *data, j, k, n;

      data = table->elements;
      if (table->precision == 0) {
        for (j = 0; j < 8; j++) {
          printf("    ");
          for (k = 0; k < 8; k++) {
            printf("%3d ", *data++);
          }
          printf("\n");
        }
      } else {
        for (j = 0; j < 8; j++) {
          printf("    ");
          for (k = 0; k < 8; k++) {
            n = (*data++) << 8;
            n += *data++;
            printf("%8d ", n);
          }
          printf("\n");
        }
      }
    }
#endif
  }
  DP_Free_Table_Links(tlist);
  return(0);
}

int Get_Frame(DB_STATE *db_state,
FRAME **ret_frame)
{
  FRAME *frame;
  int error_code;
  /*    FRAME_COMPONENT *comp; */

  if ((frame = DP_Parse_SOF(db_state, &error_code)) == NULL) {
    return(error_code);
  }

#ifdef PRINT_INFO
  printf("    Precision %d - Y %d - X %d - #comps %d\n",
      frame->precision,frame->height,frame->width,frame->ncomps);
  for (comp=frame->comps,i=0;i<frame->ncomps;i++,comp++){
    printf("    CompId %d - HSamp %d - VSamp %d - TabSel %d\n",
        comp->ident, comp->hsampling, comp->vsampling,
        comp->quant_sel);
  }
#endif
  *ret_frame = frame;
  return(0);
}

int Get_Scan(DB_STATE *db_state,
FRAME *frame,
DHUFF_TABLE *huffman_tables_dc[4],
DHUFF_TABLE *huffman_tables_ac[4],
QUANT_TABLE *quant_tables[4],
SCAN **ret_scan)
{
  SCAN *scan;
  int i, error_code;

  if (frame == NULL) {
    return(ERROR_NO_FRAME);
  }

  if ((scan = DP_Parse_SOS(db_state, frame, huffman_tables_dc,
      huffman_tables_ac, quant_tables, &error_code))==NULL){
    return(error_code);
  }

  /* Check if huffman and quant. tables are ALREADY defined */
  for (i = 0; i < scan->ncomps; i++) {
    if ((scan->comps[i].dc_table == NULL) ||
        (scan->comps[i].ac_table == NULL)) {
      return(ERROR_NO_HUF);
    }
    if (scan->comps[i].quant_table == NULL) {
      return(ERROR_NO_QUAN);
    }
  }

#ifdef PRINT_INFO
  printf("    ncomps=%d, SS=%d, Se=%d, Ah=%d, Al=%d\n",
      scan->ncomps, scan->start_spec, scan->end_spec,
      scan->approx_high, scan->approx_low);
#endif
  *ret_scan = scan;
  return(0);
}

void Free_All_Memory(DB_STATE *db_state,
FRAME **frame,
DHUFF_TABLE *huffman_tables_dc[4],
DHUFF_TABLE *huffman_tables_ac[4],
QUANT_TABLE *quant_tables[4],
DECODER_STRUCT *decoder)
{
  int i, tc;
  int ntab;

  ntab = decoder->numb_huffman_pair_tables;
  if ((ntab < 0) || (ntab > 4)) ntab =2;
  for (i = 0; i < ntab; i++) {
    if (huffman_tables_dc[i] != NULL) {
      for (tc = 0; tc < 256; tc++) {
        if (huffman_tables_dc[i]->huffelem[tc].hufftree) {
          FPX_free(huffman_tables_dc[i]->huffelem[tc].hufftree);
          huffman_tables_dc[i]->huffelem[tc].hufftree=NULL;
          }
      }
      FPX_free(huffman_tables_dc[i]);
      huffman_tables_dc[i] = NULL;
    }
    if (huffman_tables_ac[i] != NULL) {
      for (tc = 0; tc < 256; tc++) {
        if (huffman_tables_ac[i]->huffelem[tc].hufftree) {
          FPX_free(huffman_tables_ac[i]->huffelem[tc].hufftree);
          huffman_tables_ac[i]->huffelem[tc].hufftree=NULL;
          }
      }
      FPX_free(huffman_tables_ac[i]);
      huffman_tables_ac[i] = NULL;
    }
  }

  ntab = decoder->numb_quant_tables;
  if ((ntab < 0) || (ntab > 4)) ntab =2;
  for (i = 0; i < ntab; i++) {
    if (quant_tables[i] != NULL) {
      FPX_free(quant_tables[i]);
      quant_tables[i] = NULL;
    }
  }
}


/*
 * Return 0 if successful, otherwise one of ERROR_ types defined in djpeg.h
 *
 * This function should be called twice. In the first time, it decodes until
 * it sees the Frame Header when it returns the image info (such as width,
 * height).  During the 2nd time, it decodes the rest. 
 */
int DE_Decode(DB_STATE *db_state,
void *decoder,
int method,  /* 0: Chen DCT, 1 = Winograd, 2: Pruned Winograd */
int gray_scale,/* if TRUE, color JPEG image will be decoded as gray scale*/
int pass_num, /* which pass 
       0 = decode entropy-only stream,
       1 = 1st pass, decode header-only (up to SOF), or 
       2 = 2nd, decode header up to SOS. 
       3 = decode Scan(s) (SOS-EOI). */
int *image_w,
int *image_h,
int *bytes_per_pixel,
int *hdim,
int *vdim,
int interleave)
{
  int marker, i = 0, ii;
  static int error_code, dct_method, image_found, eoi_found, is_gray_scale;
  static int interleave_flag;
  static unsigned int restart_interv; /* restart interval, 0 if disabled */
  FRAME *frame;
  DHUFF_TABLE *de_huffman_tables_dc[4], *de_huffman_tables_ac[4];
  QUANT_TABLE   *de_quant_tables[4];

  int hsampling[4], vsampling[4];
  DECODER_STRUCT *decoderPtr = (DECODER_STRUCT *) decoder;
  int numb_huffman_tables;
  int numb_quant_tables;

  for (i=0; i< 4; i++) {
    de_huffman_tables_dc[i] = NULL;
    de_huffman_tables_ac[i] = NULL;
    de_quant_tables[i] = NULL;
  }

  if (pass_num == 0) {
    /* decode only entropy stream, header information has been set before */
    /* Initialize static variables */
    warning_type = 0;
    restart_interv = 0;
    image_found = 0;
    eoi_found = 0;
    dct_method = method;
    error_code = 0;
    is_gray_scale = gray_scale;
    interleave_flag = interleave;

    if (DB_Begin(db_state)) return(error_code=ERROR_MEM);

    for (i=0;i<4;i++) {
      de_huffman_tables_dc[i]= decoderPtr->huffman_table_dc[i];
      de_huffman_tables_ac[i]= decoderPtr->huffman_table_ac[i];
    }
    for (i=0;i<4;i++) {
      de_quant_tables[i] = decoderPtr->quant_table[i];
    }
    frame = decoderPtr->frame;

  } else if (pass_num == 1 || pass_num == 2) {
    /* Initialize static variables */
    warning_type = 0;
    frame = NULL;
    restart_interv = 0;
    image_found = 0;
    eoi_found = 0;
    dct_method = method;
    error_code = 0;
    is_gray_scale = gray_scale;
    interleave_flag = interleave;

    if (DB_Begin(db_state)) return(error_code=ERROR_MEM);
    numb_huffman_tables = 0;

    if ((marker=DP_Get_Next_Marker(db_state)) != MARKER_SOI) {
      Free_All_Memory(db_state, &frame, de_huffman_tables_dc,de_huffman_tables_ac,
          de_quant_tables, decoderPtr);
      return(error_code=ERROR_NOT_JPG);
    } else {
#ifdef PRINT_INFO
      Print_Marker(marker);
#endif
    }
  } else if ( pass_num == 3 ) {
    for (i=0;i<4;i++) {
      de_huffman_tables_dc[i]=de_huffman_tables_ac[i]=
          decoderPtr->huffman_table_dc[i]=decoderPtr->huffman_table_ac[i]=NULL;
    }
    for (i=0;i<4;i++) {
      de_quant_tables[i] = decoderPtr->quant_table[i] = NULL;
    }

    if (error_code) return(error_code);  /* error found in 1st pass */

    frame = decoderPtr->frame;
      for (i = 0; i < frame->ncomps; i++) {
        *(hdim+i) = hsampling[i] = frame->comps[i].hsampling;
        *(vdim+i) = vsampling[i] = frame->comps[i].vsampling;
      }
      *image_w = frame->width;
      *image_h = frame->height;
      *bytes_per_pixel = (is_gray_scale) ? 1 : frame->ncomps;

      if (DB_Write_Begin(db_state,*bytes_per_pixel, frame->horMCU,
          *image_w, *image_h, hsampling,vsampling,decoderPtr)) {
        return(error_code=ERROR_MEM);
      }

    if (DB_Allocate_Output_Buffers(db_state, decoderPtr)) {
        Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
            de_huffman_tables_ac, de_quant_tables, 
            decoderPtr);
        return(error_code=ERROR_MEM);
      }

  } else { /* default case ... should never go here */
    if (error_code) return(error_code);
  }

  while ((marker = DP_Get_Next_Marker(db_state)) != MARKER_END_FILE) {
Switch0:
#ifdef PRINT_INFO
    Print_Marker(marker);
#endif
    switch (marker) {
    case MARKER_SOF0:
      {
        if (pass_num == 1) {   /* pass_num 1 exits at SOF or EOI */
          db_state->db_byte_count -=2;

          /* assign huffman tables */
          decoderPtr->numb_huffman_pair_tables = numb_huffman_tables >> 1;
          for (ii=0; ii < 4; ii++) {
            decoderPtr->huffman_table_dc[ii] =  de_huffman_tables_dc[ii];
            decoderPtr->huffman_table_ac[ii] =  de_huffman_tables_ac[ii];
          }

          return(error_code=0);
        }
        if (frame != NULL)
        {   /* reassign db_state buffers from decoderPtr */
          for (i = 0; i < frame->ncomps; i++) {
            *(hdim+i) = hsampling[i] = frame->comps[i].hsampling;
            *(vdim+i) = vsampling[i] = frame->comps[i].vsampling;
          }
          *image_w = frame->width;
          *image_h = frame->height;
          *bytes_per_pixel = (is_gray_scale) ? 1 : frame->ncomps;
          if (DB_Write_Begin(db_state,*bytes_per_pixel, frame->horMCU,
              *image_w, *image_h, hsampling,vsampling,decoderPtr)) {
            Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
                de_huffman_tables_ac, de_quant_tables, decoderPtr);
            return(error_code=ERROR_MEM);
          }
          if (DB_Allocate_Output_Buffers(db_state, decoderPtr)) {
            Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
                de_huffman_tables_ac, de_quant_tables, 
                decoderPtr);
            return(error_code=ERROR_MEM);
          }
          break; /* this is the case for 2nd tile on */
        } /* if frame is not null */

        error_code = Get_Frame(db_state, &frame);
        if (error_code) {
          Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
              de_huffman_tables_ac, de_quant_tables, decoderPtr);
          return(error_code);
        }

        /* Only support 1, 2, 3 or 4 component JPEG images */
        if ( (frame->ncomps != 1) && (frame->ncomps !=2) && 
            (frame->ncomps != 3) && (frame->ncomps !=4) ) {
          Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
              de_huffman_tables_ac, de_quant_tables, decoderPtr);
          return(error_code=ERROR_COMP);
        }

        if (pass_num == 0 || pass_num ==2) {
          /* Save FRAME information into the DECODER_STRUCT */
          decoderPtr->frame = frame;

          for (i = 0; i < frame->ncomps; i++) {
            *(hdim+i) = hsampling[i] = frame->comps[i].hsampling;
            *(vdim+i) = vsampling[i] = frame->comps[i].vsampling;
          }
          *image_w = frame->width;
          *image_h = frame->height;
          *bytes_per_pixel = (is_gray_scale) ? 1 : frame->ncomps;
          if (DB_Write_Begin(db_state,*bytes_per_pixel, frame->horMCU,
              *image_w, *image_h, hsampling,vsampling,decoderPtr)) {
            Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
                de_huffman_tables_ac, de_quant_tables, decoderPtr);
            return(error_code=ERROR_MEM);
          }
          if (DB_Allocate_Output_Buffers(db_state, decoderPtr)) {
            Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
                de_huffman_tables_ac, de_quant_tables, 
                decoderPtr);
            return(error_code=ERROR_MEM);
          }

        }
        break;
      }
    case MARKER_SOF1:
    case MARKER_SOF2:
    case MARKER_SOF3:
    case MARKER_SOF5:
    case MARKER_SOF6:
    case MARKER_SOF7:
    case MARKER_SOF8:
    case MARKER_SOF9:
    case MARKER_SOFA:
    case MARKER_SOFB:
    case MARKER_SOFD:
    case MARKER_SOFE:
    case MARKER_SOFF:
      {
        Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
            de_huffman_tables_ac, de_quant_tables, decoderPtr);
        return(error_code=ERROR_SOF);
        break;
      }
    case MARKER_DHT:
      {
        if (pass_num == 0) break;
        error_code = Get_Huffman_Tables(db_state, de_huffman_tables_dc,
            de_huffman_tables_ac,&numb_huffman_tables);
        if (error_code) {
          Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
              de_huffman_tables_ac, de_quant_tables, decoderPtr);
          return(error_code);
        }
        break;
      }
    case MARKER_RST0:
    case MARKER_RST1:
    case MARKER_RST2:
    case MARKER_RST3:
    case MARKER_RST4:
    case MARKER_RST5:
    case MARKER_RST6:
    case MARKER_RST7:
      {
        break;
      }
    case MARKER_SOI:
      {
        restart_interv = 0;
        break;
      }
    case MARKER_EOI:
      {
#ifdef NOCODE
        if (pass_num == 3) {
          db_state->db_byte_count -=2;
          return(error_code=0);
        }
#endif /* NOCODE GSY 8/2/96 */
        eoi_found = 1;
        if (image_found) {
          goto Finished;
        }
        else if (pass_num == 1) {    /* assign huffman tables */
          decoderPtr->numb_huffman_pair_tables = numb_huffman_tables >> 1; 
          for (ii=0; ii < 4; ii++) {
            decoderPtr->huffman_table_dc[ii] =  de_huffman_tables_dc[ii];
            decoderPtr->huffman_table_ac[ii] =  de_huffman_tables_ac[ii];
          }
          goto Finished;
        } else if ((frame == NULL) &&
            ((marker = DP_Get_Next_Marker(db_state)) == MARKER_SOI)) {
          eoi_found = 0;
          goto Switch0;
        } else {
          Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
              de_huffman_tables_ac, de_quant_tables, decoderPtr);
          return(error_code=ERROR_NO_IMAGE);
        }
        break;
      }
    case MARKER_SOS:
      {
        SCAN *scan;
        if (pass_num == 2) {   /* pass_num 2 exits at EOI */
          /* assign huffman tables */
          decoderPtr->numb_huffman_pair_tables = numb_huffman_tables >> 1;
          for (ii=0; ii < 4; ii++) {
            decoderPtr->huffman_table_dc[ii] =  de_huffman_tables_dc[ii];
            decoderPtr->huffman_table_ac[ii] =  de_huffman_tables_ac[ii];
          }
        }

        error_code = Get_Scan(db_state, frame, de_huffman_tables_dc,
            de_huffman_tables_ac, de_quant_tables, &scan);
        if (error_code) {
          Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
              de_huffman_tables_ac, de_quant_tables, decoderPtr);
          return(error_code);
        }
        scan->restart_interv = restart_interv;
        scan->gray_scale = is_gray_scale;

        marker = Decode_Scan(db_state, frame, scan, dct_method, interleave_flag);
        DP_Free_Scan(scan);
        image_found = 1;
        if (marker) goto Switch0;
        break;
      }
    case MARKER_DQT:
      {
        error_code = Get_Quantization_Tables(db_state, de_quant_tables,
            &numb_quant_tables, dct_method);
        if (error_code) {
          Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
              de_huffman_tables_ac, de_quant_tables, decoderPtr);
          return(error_code);
        }
        decoderPtr->numb_quant_tables = numb_quant_tables;
        for (ii=0; ii < numb_quant_tables; ii++) {
          decoderPtr->quant_table[ii] = de_quant_tables[ii];
        }
        break;
      }
    case MARKER_DRI:
      {
        long ret;

        ret = DP_Parse_DRI(db_state,&error_code);
        if (ret < 0) {
          Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
              de_huffman_tables_ac, de_quant_tables, decoderPtr);
          return(error_code);
        } else {
          restart_interv = (unsigned int)ret;
        }
        break;
      }
    case MARKER_APP:
      {
        unsigned int len;
        unsigned char *data;

        if (pass_num == 0) break;
        data = DP_Parse_APP(db_state, &len, &error_code);
        if (data == NULL) {
          Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
              de_huffman_tables_ac, de_quant_tables, decoderPtr);
          return(error_code);
        } else {
#ifdef PRINT_INFO
          printf("    ");
          for (; len > 0; len--) {
            printf("%d ", *data++);
          }
          printf("\n");
#endif
        }
        break;
      }
    case MARKER_END_FILE:
      {
        goto Finished;
        break;
      }
    case MARKER_DNL:
    case MARKER_DHP:
    case MARKER_EXP:
    case MARKER_JPG:
    case MARKER_COM:
    case MARKER_DAC:
    case MARKER_NONE:
    default:
      {
#ifdef PRINT_INFO
        printf("    IGNORED\n");
#endif
        error_code = DP_Skip(db_state);
        if (error_code) {
          Free_All_Memory(db_state, &frame, de_huffman_tables_dc,
              de_huffman_tables_ac, de_quant_tables, decoderPtr);
          return(error_code);
        }
        break;
      }
    }
  }

Finished:
  /* data and header */
  /*if (pass_num == 2) 
    Free_All_Memory(db_state, &frame, de_huffman_tables_dc, de_huffman_tables_ac,
        de_quant_tables, decoderPtr);*/

#ifdef WARNING
  if (!image_found)    return(error_code=ERROR_EOF);
  else if (!eoi_found) return(error_code=WARNING_EOF);
  else return(error_code=warning_type);
#else
  return(error_code=0);
#endif
}
