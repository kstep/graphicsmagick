//  ------------------------------------------------------------------------------------------------
//  MODULE    : Debug
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : Vendredi 25 Septembre 1992
//  DESCRIPTION : 
//  COMMENTAIRE :
//  SCCSID      : @(#)debug.cpp 1.1 11:49:59 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include "debug.h"
  #include "mac_comp.h"

//  Declarations Internes
//  ---------------------

//  Constantes et macros
//  --------------------

//  Nouveaux types
//  --------------

#ifndef NDEBUG
  struct dbg_FunctionNode
  {
    dbg_FunctionNode *      nextNode;
    char *              funcName;
  };
#endif

//  Variables globales
//  ------------------

  FILE* debug = NULL;
  short debugVolRefNum = -1;

//  Variables locales
//  -----------------

#ifndef NDEBUG
  long        debugNestCounter = 0;
  long        debugNestInList = 0;
  dbg_FunctionNode *  debugList = NULL;
#endif


//  ================================================================================================
//  Fonctions internes
//  ------------------------------------------------------------------------------------------------

//  ================================================================================================
//  Fonctions externes
//  ------------------------------------------------------------------------------------------------

void DebugInit(char* nom)

{
  
  debug = fopen(nom,"w");
  
#ifdef macintosh
  Str255  bidon;
  GetVol(bidon, &debugVolRefNum);
#endif
}

void DebugEnd()

{
  if (debug != NULL)
  {
    fclose(debug);
    debug = NULL;
    debugVolRefNum = -1;
  }
}


#ifndef NDEBUG
void dbg_EnterFunction(char * name)
{
  if (debug == NULL) DebugInit("debug.tmp");
  if(debug == NULL)
    return;
  dbg_PrintIndent();
  fprintf(debug,">>%s\n",name);
  
  // adding the node if possible
  dbg_FunctionNode * newNode = new dbg_FunctionNode;
  if (newNode) {
    newNode->nextNode = debugList;
    newNode->funcName = name;
    debugList = newNode;
    debugNestInList++;
  }
  
  debugNestCounter++;
}

  
void dbg_ExitFunction()
{
  if (debug == NULL) DebugInit("debug.tmp");
  if(debug == NULL)
    return;
  
  if (debugNestCounter > debugNestInList) {
    debugNestCounter--;
    dbg_PrintIndent();
    fprintf(debug,"<<\n");
  } else {
    debugNestCounter--;
    debugNestInList--;
    dbg_FunctionNode * oldNode = debugList;
    // on enleve le noeud de la liste
    debugList = oldNode->nextNode;
    dbg_PrintIndent();
    fprintf(debug,"<<%s\n",oldNode->funcName);
    delete oldNode;
  }
}


void dbg_PrintIndent()
{
  long actCount = debugNestCounter;
  while (actCount) {
    fprintf(debug,"  ");
    actCount--;
  }
}
#endif
