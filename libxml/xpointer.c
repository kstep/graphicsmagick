/*
 * xpointer.c : Code to handle XML Pointer
 *
 * World Wide Web Consortium Working Draft 03-March-1998 
 * http://www.w3.org/TR/1998/WD-xptr-19980303
 *
 * See Copyright for the status of this software.
 *
 * Daniel.Veillard@w3.org
 */

#ifdef WIN32
#include "win32config.h"
#else
#include "config.h"
#endif

#include <stdio.h>
#include <libxml/xpointer.h>
#include <libxml/xmlmemory.h>
#include <libxml/parserInternals.h>

/* TODO : XPointer ... using XPath and adding functions and axis */
