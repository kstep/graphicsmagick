//  ------------------------------------------------------------------------------------------------
//  MODULE    : FicNom
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : October 15, 1992
//  DESCRIPTION : File name class
//  COMMENTAIRE :
//      SCCSID          : @(#)filename.cpp  1.5 12:54:10 02 Jun 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
#include "filename.h"
//  ------------------------------------------------------------------------------------------------


//  Includes
//  --------

#ifdef macintosh
#  ifdef USE_UNIVERS_HDR
#    include  <LowMem.h>
#  else
#    include  <SysEqu.h>
#  endif
#  include  <Finder.h>
#  include  <Errors.h>  
#  include  "fileutil.h"
#endif // macintosh

#ifndef macintosh
#  include  "a_file.h"
#endif
  
#ifdef _UNIX
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/statfs.h>
#  include <stdio.h>
#  include <unistd.h>
#endif

#ifdef _WINDOWS
#  include <windows.h>
#  include <stdio.h>
#endif

#include  "b_memory.h"  
#include  "b_error.h"


//  Constants
//  ---------

#define kOneKilobyte 1024
static const short  unknownFolderMessageID[] = {203,1};
  
  
//  Types
//  -----

//  Variables
//  ---------

#ifdef macintosh
ptr_MemoDirVol  enLecture = 0;
ptr_MemoDirVol  enEcriture = 0;
#endif

Boolean FicNom::showErrorDialog = TRUE;
FicNom_SearchFunc FicNom::searchFunction = NULL;        

//  ------------------------------------------------------------------------------------------------
#ifdef macintosh
#  pragma segment Fichier
#endif
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Fonctions internes
//  ------------------------------------------------------------------------------------------------

// Truncate a long to fit into an unsigned char
inline unsigned char TruncateToUchar(long x)
{
  return  (unsigned char) ( (x < 0) ? 0 : (x > 255 ? 255 : x) );
}

#ifdef macintosh
inline void SetSFCurFolder(long dirID, short vRefNum)
{
  // System 7.5 forces the default directory when using GetFile
  // we desactivate that mecanism
  long response;
  OSErr err = Gestalt(gestaltSystemVersion, &response);
  if (err == noErr) {
    response &= 0x0000FFFF;
    if (response >= 0x0750)
      *(unsigned char*)0xB21 &= ~0x04;
  } 
  
#ifdef USE_UNIVERS_HDR
  LMSetCurDirStore(dirID);
  LMSetSFSaveDisk(-vRefNum);
#else
  *((long*)CurDirStore) = dirID;
  *((short*)SFSaveDisk) = -vRefNum;
#endif
}

static OSErr TrouveIndVolume(short whichVol,StringPtr volName,short* volRefNum)

{
  /*Return the name and vRefNum of volume specified by whichVol.*/
  HVolumeParam  volPB;
  OSErr     error;
     
  volPB.ioNamePtr = volName; /*make sure it returns the name*/
  volPB.ioVRefNum = 0;       /*0 means use ioVolIndex*/
  volPB.ioVolIndex = whichVol; /*use this to determine the volume*/
  error = PBHGetVInfoSync( (HParmBlkPtr) &volPB ); /*do it*/
  if (error == noErr)   /*if no error occurred */
    *volRefNum = volPB.ioVRefNum; /*return the volume reference*/
  /*other information is available from this record; see the FILE*/
  /*Manager's description of PBHGetVInfo for more details...*/
        
  return(error);   /*always return error code*/
} /* GetIndVolume */


static OSErr ChercheFichierDsTsVolumes(FicNom& leFichier, FicNom* tableauDeFichier, long* nbReponse)
{
  FSSpec* tableau;
  Str255 nomVolume;
  short j, i = 1;
  long  nbFichiersTrouves = 0;
  long taille;
  short volume;
  OSErr err = noErr;
  Boolean sortie = FALSE;

  FastAllocArray(tableau,FSSpec,*nbReponse);
  
  sortie = TrouveIndVolume(i++ , nomVolume, &volume) != noErr;
  while (!sortie) {
    taille = *nbReponse;
    err = leFichier.ChercheDsVolume(volume, tableau, &taille);
    if ((taille > 0) && (nbFichiersTrouves < *nbReponse)) {
      for (j = 0; j < taille; j++) {
        if (nbFichiersTrouves + j < *nbReponse)
          tableauDeFichier[nbFichiersTrouves + j] = FicNom(tableau[j]);
      }
      nbFichiersTrouves += taille;
    }
    sortie = TrouveIndVolume(i++ , nomVolume, &volume) != noErr;
  }
  
  FastDeleteArray(tableau,FSSpec);
  
  *nbReponse = nbFichiersTrouves;
  
  return err;
}
#endif

//  ------------------------------------------------------------------------------------------------
//  Methodes
//  ------------------------------------------------------------------------------------------------

FicNom::FicNom(const FicNom& fic)
{
  volume    = fic.volume;
  directory   = fic.directory;
  nom     = fic.nom;
  type    = fic.type;
}

FicNom::FicNom(const ChaineFic& fileName, OSType fileType, short fileVolume, long fileDirectory)
{
  volume    = fileVolume;
  directory   = fileDirectory;
  nom     = fileName;
  type    = fileType;
}

FicNom::FicNom(const char* fileName, OSType fileType)
{
#ifdef macintosh
  nom     = Chaine(CStringHolder(fileName));
#else
  nom     = Chaine(CStringHolder(fileName));
#endif
  type    = fileType;
  volume    = 0;
  directory   = 0;
}

#ifdef macintosh
FicNom::FicNom(const FSSpec& specs)
{
  FSSpec tmp = specs;             // we need a non const variable <= ChaineFic(str63)
  FInfo  fndrInfo;
  
  volume    = tmp.vRefNum;
  directory   = tmp.parID;
  nom     = tmp.name;
  FSpGetFInfo(&tmp,&fndrInfo);
  type    = fndrInfo.fdType;
}

#endif

FicNom::FicNom(const FicNom& fic, short newVolume)
{
  volume    = newVolume;
  directory   = fic.directory;
  nom     = fic.nom;
  type    = fic.type;
}


OSErr FicNom::Detruit() const
{
  OSErr err;
#ifdef macintosh
  err = HDelete(volume, directory, nom);
#else
  char cname[256];

  int len = nom[0]; // FIXME: Sun Forte 6.0 fails
  cname[len] = '\0';
  for (int i = len-1; i >= 0; i--)
    cname[i] = nom[i+1]; // FIXME: Sun Forte 6.0 fails
  if ((err = unlink (cname)) != 0)
    perror ("Unlink failed\n");
#endif
  return err;
}

OSErr FicNom::Search()
{
  if (searchFunction)
    return (*searchFunction)(this);
  else
    return fnfErr;
}

#ifdef macintosh
OSErr FicNom::GiveNumberOfHierarchicalLevels(long* nbNiveauxPtr) const
{
  CInfoPBRec  lePB;
  Str255    dirNom;
  OSErr   err = noErr;
  
  // Update the Parameter Block
  lePB.dirInfo.ioNamePtr = dirNom;
  lePB.dirInfo.ioVRefNum = this->volume;
  lePB.dirInfo.ioDrParID = this->directory;
  lePB.dirInfo.ioFDirIndex = -1;
  
  // Count number of hierarchical levels
  *nbNiveauxPtr = 1;
  if (this->directory != fsRtParID) {
    do {
      lePB.dirInfo.ioDrDirID = lePB.dirInfo.ioDrParID;
      err = PBGetCatInfoSync(&lePB);
      (*nbNiveauxPtr)++;
    } while ((lePB.dirInfo.ioDrDirID != fsRtDirID) && (err == noErr));
  }
  return err;
}

OSErr FicNom::ConstructTruncPathName(short width, TruncCode truncWhere, Chaine * chaineText) const
{
  long    nbNiveaux;
  CInfoPBRec  lePB;
  OSErr   err = noErr;
  Str255    dirNom;
  Chaine    cheminAcces;
  long    i;
  Chaine*   hierarchieAcces;
  char*   textBuffer;
  short   textWidth;
  
  // If the file or folder exists
  if ( ((err = Exist()) == noErr) &&
       ((err = GiveNumberOfHierarchicalLevels(&nbNiveaux)) == noErr) ) {
    // Allocate an array to store level's names
    AllocArray(hierarchieAcces,Chaine,nbNiveaux);
      
    // Update the Parameter Block
    lePB.dirInfo.ioNamePtr = dirNom;
    lePB.dirInfo.ioVRefNum = volume;
    lePB.dirInfo.ioDrParID = directory;
    lePB.dirInfo.ioFDirIndex = -1;
      
    // Get name of each level
    hierarchieAcces[0] =  nom; 
    textWidth = hierarchieAcces[0].Longueur() + 1;
    for (i = 1; i<nbNiveaux; i++) {
      lePB.dirInfo.ioDrDirID = lePB.dirInfo.ioDrParID;
      err = PBGetCatInfoSync(&lePB);
      hierarchieAcces[i] = dirNom; 
      textWidth += hierarchieAcces[i].Longueur() + 1;
    }
  } else {
    // If an error occured when searching the file
    nbNiveaux = 1;
    AllocArray(hierarchieAcces,Chaine,1);
    Chaine unknownFolder;
    GetIndString(unknownFolder, unknownFolderMessageID[0],unknownFolderMessageID[1]);
    *hierarchieAcces = unknownFolder;
    textWidth = hierarchieAcces->Longueur() + 1;
  }
    
  // Allocate the text buffer 
  textBuffer = new char [textWidth];
  if (textBuffer==NULL) {
    return memFullErr;
  }
  char* currText = textBuffer;
  *currText++ = ' ';
  
  // Built the complete path name
  for (i=(nbNiveaux-1); i>=0; i--) {
    BlockMove (hierarchieAcces[i].Texte(), currText, hierarchieAcces[i].Longueur());
    currText += hierarchieAcces[i].Longueur();
    if (i)
      *currText++ = ':';
  }

  // Trunc the text
  TruncText (width, textBuffer, &textWidth, truncWhere);
  *textBuffer = (unsigned char)(textWidth-1);
  
  // Build the complete chain
  BlockMove (textBuffer, chaineText, textWidth);

  // Clean memory of intermediate buffers
  DeleteArray(hierarchieAcces,Chaine);
  delete [] textBuffer;
  
  return err;
}

#endif // #ifdef macintosh


OSErr FicNom::Exist() const
{
  OSErr   err = noErr;
#ifdef macintosh
  FSSpec myFSSpec;

  err = FSMakeFSSpec(this->volume, this->directory, Chaine(this->nom), &myFSSpec);
#else
  ptr_Fichier file = new Fichier(*this, mode_Lecture);
  if (file==NULL) {
    return memFullErr;
  }
  err = file->Erreur();
  delete file;
#endif

  return err;
}


// File name starting with a period (.) are forbidden (see Inside Macintosh : Files, p.2-173)
OSErr FicNom::CheckNameWithPeriod(Boolean withDialog) const
{
  OSErr err = noErr;
  if (*(nom.Texte()) == '.')
    err = badFidErr;
  return err;
}

// To know if there is at least size KBytes on the disk
// If not, write an error message (on Macintosh)
Boolean FicNom::EnoughFreeSpace(long sizeInKBytes) const
{
  Boolean enoughSpace = TRUE;
  
  if (PlaceDisponible() < sizeInKBytes) {
    enoughSpace = FALSE;
  }
  
  return enoughSpace;
}

// Give the free space in *this's volume (in KBytes)
long FicNom::PlaceDisponible() const
{
#ifdef macintosh
  HParamBlockRec  myHPB;
  OSErr     myErr;
  long      sizeInKBytes;
  
  myHPB.volumeParam.ioNamePtr  = nil;
  myHPB.volumeParam.ioVRefNum  = volume;
  myHPB.volumeParam.ioVolIndex = 0;
  
  myErr = PBHGetVInfoSync(&myHPB);
  
  if (myErr == noErr)
    {
      sizeInKBytes = (((float)(myHPB.volumeParam.ioVFrBlk)/1024.0) * (float)(myHPB.volumeParam.ioVAlBlkSiz));
    }
  else
    {
      sizeInKBytes = 0;
    }
  
  return sizeInKBytes;
#elif WIN32
  DWORD nSecPerCluster, nBytePerSec, nFreeCluster, nCluster;
  if (GetDiskFreeSpace( NULL, &nSecPerCluster, &nBytePerSec, &nFreeCluster, &nCluster ))
    return nFreeCluster * nSecPerCluster * nBytePerSec / 1024;
  else    // Function failed, don't know why
    return 0;
#else
  struct statfs buf;
  int errNum=0, fstyp;
  /* Get file system stats */
  fstyp=0;
#ifdef __SVR4 // FIXME: statfs is not a portable API
  errNum = statfs ( ".", &buf, sizeof( struct statfs ), fstyp);
#else
  errNum = statfs ( ".", &buf);
#endif

  return (buf.f_bsize*buf.f_bfree)/kOneKilobyte;
#endif
}

// Get the size of the file (in KBytes)
OSErr FicNom::GetSize(long* sizeInKBytes)
{
  OSErr err = noErr;
  
#ifdef macintosh
  HParamBlockRec  myHPB;
  
  myHPB.fileParam.ioNamePtr  = (StringPtr)(&(nom));
  myHPB.fileParam.ioVRefNum  = volume;
  myHPB.fileParam.ioDirID = directory;
  myHPB.fileParam.ioFDirIndex = 0;
  
  err = PBHGetFInfoSync(&myHPB);
  
  if (err == noErr)
    {
      *sizeInKBytes = (((float)(myHPB.fileParam.ioFlPyLen)/1024.) + (float)(myHPB.fileParam.ioFlRPyLen)/1024.) + 1;
    }
  
  return err;
#else
  return -1;
#endif
}

OSErr FicNom::Empty(Boolean* empty)
{
  OSErr err = noErr;
  *empty = FALSE;
  
#ifdef macintosh
  long sizeInBytes;
  HParamBlockRec  myHPB;
  myHPB.fileParam.ioNamePtr  = (StringPtr)(&(nom));
  myHPB.fileParam.ioVRefNum  = volume;
  myHPB.fileParam.ioDirID = directory;
  myHPB.fileParam.ioFDirIndex = 0;
  
  err = PBHGetFInfoSync(&myHPB);
  
  if (err == noErr)
    {
      sizeInBytes = myHPB.fileParam.ioFlRPyLen;
      if (!sizeInBytes)
        *empty = TRUE;
      else 
        *empty = FALSE;
      
    }
  return err;
#else
  return -1;
#endif  
}

#ifdef macintosh
MemoDirVol::MemoDirVol(const FicNom& fic, ptr_MemoDirVol avant)
{
  type = fic.type;
  directory = fic.directory;
  volume = fic.volume;
  suivant = avant;
}

ptr_MemoDirVol FicNom::Recherche(ptr_MemoDirVol memo)
{
  ptr_MemoDirVol x;
  
  for (x = memo; x; x = x->suivant) {
    if (type == x->type)
      break;
  }
  return x;
}

void FicNom::DirectoryDeSauvegarde()
{
  ptr_MemoDirVol trouve = Recherche(enEcriture);
  
  if (trouve) {
    SetSFCurFolder(trouve->directory, trouve->volume);
  }
}

void FicNom::DirectoryDeChargement()
{
  ptr_MemoDirVol trouve = Recherche(enLecture);
  
  if (trouve) {
    SetSFCurFolder(trouve->directory, trouve->volume);
  }
}

void FicNom::SetToDirectoryChargement()
{
  ptr_MemoDirVol trouve = Recherche(enLecture);
  
  if (trouve) {
    directory = trouve->directory;
    volume    = trouve->volume;
  }
}

void FicNom::SetToDirectorySauvegarde()
{
  ptr_MemoDirVol trouve = Recherche(enEcriture);
  
  if (trouve) {
    directory = trouve->directory;
    volume    = trouve->volume;
  }
}

Boolean FicNom::ChangeFolder(const FicNom& folder)
{
  OSErr err = noErr;
  
  if (folder.nom.Longueur() == 0) {
    volume = folder.volume;
    directory = folder.directory;
  }
  else {
    // Initialize and clear the parameter blocks needed by PBGetCatInfoSync
    CInfoPBPtr lePB = (CInfoPBPtr)NewPtrClear(sizeof(CInfoPBRec));
    
    if (lePB) {
      // Position inside the given preference folder
      lePB->dirInfo.ioNamePtr   = (StringPtr)(&(folder.nom));
      lePB->dirInfo.ioVRefNum   = folder.volume;
      lePB->dirInfo.ioDrDirID   = folder.directory;
      lePB->dirInfo.ioFDirIndex = 0;
      err = PBGetCatInfoSync(lePB);
      
      if (err == noErr) {
        // Set current volume and directory
        volume    = lePB->dirInfo.ioVRefNum;
        directory = lePB->dirInfo.ioDrDirID;
      }
      
      DisposePtr((Ptr)lePB);
    }
    else
      err = memFullErr;
  }
  return (err == noErr ? TRUE : FALSE);
}

void FicNom::SetDefaultFolder()
{
  SetSFCurFolder(directory, volume);
}

void FicNom::MemorisationDeSauvegarde()
{
  ptr_MemoDirVol trouve = Recherche(enEcriture);
  
  if (!trouve) {
    trouve = new MemoDirVol(*this,enEcriture);
    if (trouve==NULL) {
      return;
    }
    enEcriture = trouve;
  } else {
    trouve->directory = directory;
    trouve->volume = volume;
  }
}

void FicNom::MemorisationDeChargement()
{
  ptr_MemoDirVol trouve = Recherche(enLecture);
  
  if (!trouve) {
    trouve = new MemoDirVol(*this,enLecture);
    if (trouve==NULL) {
      return;
    }
    enLecture = trouve;
  } else {
    trouve->directory = directory;
    trouve->volume = volume;
  }
}

OSErr FicNom::ChercheDsVolume (short volume, FSSpec* tableauDeFSSpec, long* nbReponse)
{
  FicNom intermFile (*this, volume);
  return intermFile.SearchWithCriteria (tableauDeFSSpec, nbReponse, searchOnName);
}

/* Search file routine on FicNom criteria. This routine hides PBCatSearch() problems. */
/* see Inside Macintosh : Files p.2-204 for PBCatSearch documentation         */
/* see Inside Macintosh : Files p.2-40 for search bit setting constants         */

OSErr FicNom::SearchWithCriteria (FSSpec* tableauDeFSSpec, long* nbReponse, searchCriteria what)
{
  short bufferSize = kOneKilobyte * 16;
  OSErr myError = noErr;
  
  /* Initialize and clear the cat search parameter block (csBlockPtr)         */
  CSParamPtr csBlockPtr = (CSParamPtr) NewPtrClear(sizeof(CSParam));
  
  if (csBlockPtr) {

    /* Initialize and clear the two other parameter blocks needed by PBCatSearch  */
    csBlockPtr->ioSearchInfo1 = (CInfoPBPtr)NewPtrClear(sizeof(CInfoPBRec));
    csBlockPtr->ioSearchInfo2 = (CInfoPBPtr)NewPtrClear(sizeof(CInfoPBRec));

    /* Set the match pointer to the output array                  */
    csBlockPtr->ioMatchPtr = tableauDeFSSpec;
    csBlockPtr->ioReqMatchCount = *nbReponse;
      
    /* Allocate optimization buffer : PBCatSearch will work even if there is no   */
    /* optimization buffer.                             */
    csBlockPtr->ioOptBufSize = bufferSize;
    while (csBlockPtr->ioOptBufSize) {        
      csBlockPtr->ioOptBuffer = NewPtr(csBlockPtr->ioOptBufSize);
      if (csBlockPtr->ioOptBuffer != NULL)
        break;
      csBlockPtr->ioOptBufSize -= kOneKilobyte;
    }

    /* No timeout                                 */
    csBlockPtr->ioSearchTime = 0;

    /* Test if needed buffers has been allocated before launching PBCatSearch   */
    if (csBlockPtr->ioSearchInfo1 && csBlockPtr->ioSearchInfo2) {
      /* Set the search criteria :                        */
      /* The volume reference number to search                  */
      csBlockPtr->ioVRefNum = volume;
      switch (what) {
      case searchOnName:      /* Search files with the same name      */
        csBlockPtr->ioSearchInfo1->hFileInfo.ioNamePtr = nom;
        csBlockPtr->ioSearchBits += fsSBFullName;
        break;
      case searchOnType:      /* Search files with the same type      */
        csBlockPtr->ioSearchInfo1->hFileInfo.ioNamePtr = nil;
        csBlockPtr->ioSearchInfo1->hFileInfo.ioFlFndrInfo.fdType = type;
        csBlockPtr->ioSearchInfo2->hFileInfo.ioFlFndrInfo.fdType = 0xFFFFFFFF;
        csBlockPtr->ioSearchBits += fsSBFlFndrInfo;
        break;
      default:          /* Will search all files in volume      */
        break;
      }
      /* To do because we are searching for files only              */
      csBlockPtr->ioSearchInfo2->hFileInfo.ioNamePtr = nil;
      csBlockPtr->ioSearchInfo2->hFileInfo.ioFlAttrib = 0x10;
      csBlockPtr->ioSearchBits += fsSBFlAttrib;
      
      /* Launch the search routine itself+ (at last+)               */
      myError = PBCatSearchSync (csBlockPtr);
      if (myError == eofErr)  /* PBCatSearch has searched the entire volume   */
        myError = noErr;
      *nbReponse = csBlockPtr->ioActMatchCount;
    } else
      myError = memFullErr;
  } else
    myError = memFullErr;
  
  /* Free allocated buffers before leaving                      */
  if (csBlockPtr) {
    if (csBlockPtr->ioSearchInfo1)
      DisposePtr((Ptr)csBlockPtr->ioSearchInfo1);
    if (csBlockPtr->ioSearchInfo2)
      DisposePtr((Ptr)csBlockPtr->ioSearchInfo2);
    if (csBlockPtr->ioOptBuffer)
      DisposePtr((Ptr)csBlockPtr->ioOptBuffer);
    DisposePtr((Ptr)csBlockPtr);
  }
    
  return myError;
}

/* Search file routine in a given folder. This routine hides PBGetCatInfoSync() problems*/
/* see Inside Macintosh : Files for PBGetCatInfo documentation            */
/* see Lettre des DÄveloppeurs Apple ní23/DÄcembre 1992, p.10             */

OSErr FicNom::SearchInFolder (FSSpec* tableauDeFSSpec, long* nbReponse, ptr_FicNom sourceFolder, searchCriteria what, Boolean into)
{
  OSErr   err = noErr;
  short   index = 1;
  ChaineFic dirNom;
  short   currVol;
  long    currDir;
  long    maxReponse = *nbReponse;

  *nbReponse = 0;
  
  // Initialize and clear the parameter blocks needed by PBGetCatInfoSync
  CInfoPBPtr lePB = (CInfoPBPtr)NewPtrClear(sizeof(CInfoPBRec));

  if (lePB) {
    lePB->dirInfo.ioNamePtr   = (StringPtr)(&sourceFolder->nom);
    lePB->dirInfo.ioVRefNum   = sourceFolder->volume;
    lePB->dirInfo.ioDrDirID   = sourceFolder->directory;
    lePB->dirInfo.ioFDirIndex = 0;

    // Position inside the given folder (default option)
    if (into)
      err = PBGetCatInfoSync(lePB);
    
    if (err == noErr) {
      // Set current volume and directory
      currVol = lePB->dirInfo.ioVRefNum;
      currDir = lePB->dirInfo.ioDrDirID;
      // Initialize volume and pointer to string name
      lePB->dirInfo.ioNamePtr = (StringPtr)(&dirNom);
      lePB->dirInfo.ioVRefNum = currVol;
    }
    
    // Loop while the folder is not empty
    while (err == noErr) {
      lePB->dirInfo.ioDrDirID   = currDir;
      lePB->dirInfo.ioFDirIndex = index;
      err = PBGetCatInfoSync(lePB);
      if (err == noErr) {
        // Test if this is not a folder
        if ( (  !(lePB->hFileInfo.ioFlAttrib & 0x10) &&
                (((what == searchOnType) && (lePB->hFileInfo.ioFlFndrInfo.fdType == type)) ||
                 ((what == searchOnName) && (!IdenticalString(lePB->hFileInfo.ioNamePtr,(ConstStr255Param)(&nom),0)))) )
             ||
             (  (lePB->hFileInfo.ioFlAttrib & 0x10) && (what == searchSubFolder) ) ){
          tableauDeFSSpec->vRefNum = currVol;
          tableauDeFSSpec->parID   = currDir;
          BlockMove (&dirNom, &(tableauDeFSSpec->name), dirNom.Longueur()+1);
          tableauDeFSSpec++;
          (*nbReponse)++;
          if (*nbReponse == maxReponse)
            break;
        }
      }
      index++;
    }
    // Reset to noErr
    err = noErr;
  } else
    err = memFullErr;
    
  if (lePB)
    DisposePtr((Ptr)lePB);
  
  return err;
}

OSErr FicNom::SearchSameTypeInFolder(FicNom* tableauDeFichier, long* nbReponse)
{
  FSSpec* tableau;
  OSErr err = noErr;

  FastAllocArray(tableau,FSSpec,*nbReponse);

  if (tableau) {
    err = SearchInFolder (tableau, nbReponse, this, searchOnType, FALSE);
    if ((err == noErr) && (*nbReponse > 1)) {
      FicNom convNom;
      long i, j;
      for (i = 0, j = 0; i < *nbReponse; i++) {
        convNom = FicNom(tableau[i]);
        if (!(convNom == *this))
          tableauDeFichier[j++] = FicNom(tableau[i]);
      }
    }
    (*nbReponse)--;
    FastDeleteArray(tableau,FSSpec);
  } else
    err = memFullErr;
  
  return err;
}

OSErr FicNom::SearchSameType(FicNom* tableauDeFichier, long* nbReponse, ptr_FicNom sourceFolder)
{
  FSSpec* tableau;
  OSErr err = noErr;

  FastAllocArray(tableau,FSSpec,*nbReponse);

  if (tableau) {
    if (sourceFolder)
      err = SearchInFolder (tableau, nbReponse, sourceFolder);
    else
      err = SearchWithCriteria (tableau, nbReponse, searchOnType);
    if ((err == noErr) && *nbReponse) {
      for (long i = 0; i < *nbReponse; i++)
        tableauDeFichier[i] = FicNom(tableau[i]);
    }
    FastDeleteArray(tableau,FSSpec);
  } else
    err = memFullErr;
  
  return err;
}

OSErr FicNom::ChercheDsRepertoire(FicNom* tableauDeFichier, long* nbReponse, ptr_FicNom sourceFolder)
{
  FSSpec* tableau;
  long j;
  OSErr err = noErr;
  
  FastAllocArray(tableau,FSSpec,*nbReponse);

  if (tableau) {
    err = SearchInFolder (tableau, nbReponse, sourceFolder, searchOnName);
    if (err == noErr) {
      for (j = 0; j < *nbReponse; j++)
        tableauDeFichier[j] = FicNom(tableau[j]);
    }
    FastDeleteArray(tableau,FSSpec);
  } else
    err = memFullErr;
  
  return err;
}

OSErr FicNom::ChercheDsVolumesLocaux(FicNom* tableauDeFichier, long* nbReponse)
{
  FSSpec* tableau;
  Str255 nomVolume;
  short j, i = 1;
  long  nbFichiersTrouves = 0;
  long taille;
  short volume;
  OSErr err = noErr;
  Boolean sortie = FALSE;
  HParamBlockRec paramBlock;
  GetVolParmsInfoBuffer infoBuffer;
  
  FastAllocArray(tableau,FSSpec,*nbReponse);

  if (tableau) {
    sortie = TrouveIndVolume(i++ , nomVolume, &volume) != noErr;
    while (!sortie) {
      taille = *nbReponse;
      paramBlock.ioParam.ioNamePtr = NULL;
      paramBlock.ioParam.ioVRefNum = volume;
      paramBlock.ioParam.ioBuffer = (Ptr) &infoBuffer;    
      paramBlock.ioParam.ioReqCount = sizeof(infoBuffer);
      err = PBHGetVolParmsSync(&paramBlock);
      if (((infoBuffer.vMAttrib & (1 << bHasExtFSVol)) == 0) && 
          ((infoBuffer.vMAttrib & (1 << bHasCatSearch)) != 0)) {
        err = this->ChercheDsVolume(volume, tableau, &taille);
        if (taille > 0) {
          for (j = 0; j < taille; j++) {
            if (nbFichiersTrouves + j < *nbReponse)
              tableauDeFichier[nbFichiersTrouves + j] = FicNom(tableau[j]);
            else
              break;
          }
          nbFichiersTrouves += j;
        }
      }
      if (nbFichiersTrouves < *nbReponse)
        sortie = TrouveIndVolume(i++ , nomVolume, &volume) != noErr;
      else
        sortie = TRUE;
    }
    FastDeleteArray(tableau,FSSpec);
    *nbReponse = nbFichiersTrouves;
  } else
    err = memFullErr;
  
  return err;
}

OSErr FicNom::ChercheDsVolumesDistants(FicNom* tableauDeFichier, long* nbReponse)
{
  FSSpec* tableau;
  Str255 nomVolume;
  short j, i = 1;
  long  nbFichiersTrouves = 0;
  long taille;
  short volume;
  OSErr err = noErr;
  Boolean sortie = FALSE;
  HParamBlockRec paramBlock;
  GetVolParmsInfoBuffer infoBuffer;
  
  FastAllocArray(tableau,FSSpec,*nbReponse);

  if (tableau) {
    sortie = TrouveIndVolume(i++ , nomVolume, &volume) != noErr;
    while (!sortie) {
      taille = *nbReponse;
      paramBlock.ioParam.ioNamePtr = NULL;
      paramBlock.ioParam.ioVRefNum = volume;
      paramBlock.ioParam.ioBuffer = (Ptr) &infoBuffer;    
      paramBlock.ioParam.ioReqCount = sizeof(infoBuffer);
      err = PBHGetVolParmsSync(&paramBlock);
      if (((infoBuffer.vMAttrib & (1 << bHasExtFSVol)) != 0) &&
          ((infoBuffer.vMAttrib & (1 << bHasCatSearch)) != 0)) {
        err = this->ChercheDsVolume(volume, tableau, &taille);
        if (taille > 0) {
          for (j = 0; j < taille; j++) {
            if (nbFichiersTrouves + j < *nbReponse)
              tableauDeFichier[nbFichiersTrouves + j] = FicNom(tableau[j]);
            else
              break;
          }
          nbFichiersTrouves += j;
        }
      }
      if (nbFichiersTrouves < *nbReponse)
        sortie = TrouveIndVolume(i++ , nomVolume, &volume) != noErr;
      else
        sortie = TRUE;
    }
    FastDeleteArray(tableau,FSSpec);
    *nbReponse = nbFichiersTrouves;
  } else
    err = memFullErr;
  
  return err;
}

unsigned long FicNom::DateModification()
{
  OSErr err;
  HParamBlockRec  paramBlock;
  
  paramBlock.fileParam.ioNamePtr = this->nom;
  paramBlock.fileParam.ioVRefNum = this->volume;
  paramBlock.fileParam.ioDirID   = this->directory;
  paramBlock.fileParam.ioFDirIndex = 0;
  
  err = PBHGetFInfoSync(&paramBlock);
  
  return paramBlock.fileParam.ioFlMdDat;
}

unsigned long FicNom::DateCreation()
{
  OSErr err;
  HParamBlockRec  paramBlock;
  
  paramBlock.fileParam.ioNamePtr = this->nom;
  paramBlock.fileParam.ioVRefNum = this->volume;
  paramBlock.fileParam.ioDirID   = this->directory;
  paramBlock.fileParam.ioFDirIndex = 0;
  
  err = PBHGetFInfoSync(&paramBlock);
  
  return paramBlock.fileParam.ioFlCrDat;
}

void FicNom::GetNewLocation()
{
  //  This directory always exists because the program is running
  FicNom livePicture;
  FicNom file;
  OSErr err = GetProgramName(livePicture);
  this->directory = livePicture.directory; 
  this->volume = livePicture.volume; 
  
}
#endif

OSErr FicNom::Copy(const FicNom& newFileName)
{
  OSErr err;
  
#ifdef macintosh
  err = FileCopy(this->volume, this->directory, this->nom, 
                 newFileName.volume, newFileName.directory, NULL , newFileName.nom,
                 NULL, 0, TRUE);
#else
  err = -1;
#endif
  
  return err;
}



OSErr FicNom::Rename(const FicNom& newFileName)
{
  OSErr err;
#ifdef macintosh
  FSSpec mySpec;
  CopyToFSSpec (mySpec);

  err = FSpRenameCompat(&mySpec,newFileName.nom);
#else
  err = -1;
#endif
  return err;
}

OSErr FicNom::Move(const FicNom& newFileName)
{
  OSErr err;
#ifdef macintosh
  if (volume != newFileName.volume)
    return (diffVolErr);
  err = HMoveRenameCompat(volume,directory, nom,
                          newFileName.directory,NULL,newFileName.nom);
#else
  err = -1;
#endif
  return err;
}

//  ------------------------------------------------------------------------------------------------
//  Fonctions externes
//  ------------------------------------------------------------------------------------------------

#ifdef macintosh
  OSErr GetProgramName (ref_FicNom programName)
{
  static short refNum = 0;

  OSErr err = TRUE;

  // Get the pointer to the program file : 
  // Must be done only once in the pragram life at the very beginning
  if (!refNum)
    refNum = CurResFile();
    
  // Initialize and clear the parameter blocks needed by PBGetFCBInfoSync
  FCBPBPtr lePB = (FCBPBPtr)NewPtrClear(sizeof(FCBPBRec));

  if (lePB) {
    // Set the flags to get the current program name
    lePB->ioCompletion  = NULL;
    lePB->ioNamePtr     = (StringPtr)(&(programName.nom));
    lePB->ioVRefNum     = 0;
    lePB->ioRefNum    = refNum;
    lePB->ioFCBIndx   = 0;
    
    // Get the information
    err = PBGetFCBInfoSync (lePB);
    
    if (err == noErr) {
      // Set current volume and directory
      programName.volume    = lePB->ioFCBVRefNum;
      programName.directory = lePB->ioFCBParID;
    }
    
    // Release the parameter block
    DisposePtr((Ptr)lePB);
        
  } else
    err = memFullErr;
  
  return err;
}
#endif

