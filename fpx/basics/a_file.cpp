//  ------------------------------------------------------------------------------------------------
//  MODULE    : Fichier
//  LANGUAGE  : C++
//  AUTHOR    : Adolfo VIDE
//  DATE    : Thursday October 15th, 1992
//  DESCRIPTION : 
//  COMMENT   :
//  SCCSID      : @(#)a_file.cpp  1.1 11:49:53 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #include "a_file.h"
//  ------------------------------------------------------------------------------------------------


//  Includes
//  --------

#ifdef macintosh
#  include  <aliases.h>
#endif

#include "b_error.h"

#ifdef _UNIX
#  include <sys/types.h>
#  include <sys/stat.h>
#  if defined(HAVE_SYS_STATFS_H)
#    /* System V & Linux */
#    include <sys/statfs.h>
#  elif defined(HAVE_SYS_PARAM_H) && defined(HAVE_SYS_MOUNT_H)
     /* FreeBSD 5.0 */
#    include <sys/param.h>
#    include <sys/mount.h>
#  endif
#  include <sys/ioctl.h>
#  include <fcntl.h>
#  include <unistd.h>
#  define O_BINARY 0x0
#endif

#ifdef _WINDOWS
#  include  <io.h>
#  include  <fcntl.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#endif

#include  "debug.h"


//  Constants
//  ---------

// Suffix of temporary files
#define SUFFIXE_FICHIER_TEMPORAIRE  (unsigned char*)"\p.tmp"

OSType signature = (OSType)DEFAULT_SIGNATURE;

//  Types
//  -----

//  Variables
//  ---------

//  ------------------------------------------------------------------------------------------------
#ifdef macintosh
#  pragma segment Fichier
#endif
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Functions intern
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Methods
//  ------------------------------------------------------------------------------------------------

// Open a file. If file is open to write into, try multiple times on Macintosh.
Fichier::Fichier(const FicNom& fNom, mode_Ouverture mode, const obj_Signature &fileSignature)
  : modeOuverture(mode)
{
  InitEverything();
  Ouverture(fNom, mode, fileSignature);
}

Fichier::Fichier(const FicNom& fNom, mode_Ouverture mode)
  : modeOuverture(mode)
{
  InitEverything();
  Ouverture(fNom, mode, signature);
}

Fichier::Fichier(const FicNom& fNom, mode_Ouverture mode, unsigned long tailleBuf, const obj_Signature &fileSignature)
  : modeOuverture(mode)
{
  InitEverything();
  Ouverture(fNom, mode, fileSignature);
  InitCacheBuffer(tailleBuf);
}

Fichier::Fichier(const FicNom& fNom, mode_Ouverture mode, unsigned long tailleBuf)
  : modeOuverture(mode)
{
  InitEverything();
  Ouverture(fNom, mode, signature);
  InitCacheBuffer(tailleBuf);
}

// Create an object with a file descriptor to an already open file.
Fichier::Fichier(int fd, mode_Ouverture mode, const obj_Signature & )
  : modeOuverture(mode)
{
  InitEverything(fd);
}

Fichier::Fichier(int fd, mode_Ouverture mode)
  : modeOuverture(mode)
{
  InitEverything(fd);
}

Fichier::Fichier(int fd, mode_Ouverture mode, unsigned long tailleBuf, const obj_Signature & )
  : modeOuverture(mode)
{
  InitEverything(fd);
  InitCacheBuffer(tailleBuf);
}

Fichier::Fichier(int fd, mode_Ouverture mode, unsigned long tailleBuf)
  : modeOuverture(mode)
{
  InitEverything(fd);
  InitCacheBuffer(tailleBuf);
}

void Fichier::DoNotCloseFileWhenDelete()
{
  closeWhenDelete = FALSE;
}

void Fichier::CloseFileWhenDelete()
{
  closeWhenDelete = TRUE;
}

// Open a file. If file is open to write into, try multiple times on Macintosh.
void Fichier::Ouverture(const FicNom& fNom, mode_Ouverture mode, OSType fileSignature)

{

#ifdef macintosh
  OSErr   err       = noErr;
  Boolean targetIsFolder  = FALSE;
  Boolean wasAliased    = FALSE;
  FSSpec myFSSpec;

  // Check that the name doesn't begin with a period 
  // File name starting with a period (.) are forbidden (see Inside Macintosh : Files, p.2-173)
  erreurIO = fNom.CheckNameWithPeriod(FALSE);

  // Try to resolve an alias
  if ((mode != mode_Ecrasement) && (mode != mode_Reset)) {
    fNom.CopyToFSSpec (myFSSpec);
    err = ResolveAliasFile (&myFSSpec, TRUE, &targetIsFolder, &wasAliased);
    if (erreurIO == noErr)
      erreurIO = err;
  }
  
  // If 'fNom' is an alias on a file
  if (!targetIsFolder && wasAliased) 
    {
      vraiNom = nomActuel = FicNom(myFSSpec); 
    } 
  else 
    // else, handle the case where 'fNom.nom' contains an access path
    {
      OSErr myErr = FSMakeFSSpec(fNom.volume, fNom.directory, Chaine(fNom.nom), &myFSSpec);
      if (myErr == noErr)
        vraiNom = nomActuel = FicNom(myFSSpec);
      else
        vraiNom = nomActuel = fNom;
    }
  
  // Force the file's type if we are going to create the file
  if ((mode == mode_Ecrasement) || (mode == mode_Reset))
    vraiNom.type = nomActuel.type = fNom.type;

  if (erreurIO == noErr) {
    switch (mode) {
    case mode_Lecture:
      erreurIO = HOpen(vraiNom.volume,vraiNom.directory,vraiNom.nom,fsRdPerm,&refNum);
      break;
        
    case mode_Ecrasement:
      do
        {
          // Check if the file exists already :
          erreurIO = HOpen(vraiNom.volume,vraiNom.directory,vraiNom.nom,fsRdWrPerm,&refNum);
          //
          // If the file exists and can be erased => give another name to the new file
          if (erreurIO == noErr) {
            FSClose(refNum);
            nomActuel.nom  = Chaine(TickCount());
            nomActuel.nom += SUFFIXE_FICHIER_TEMPORAIRE;
            nomActuel.type = 'TMP ';
          }
          // If the file is locked => end
          else if (erreurIO != fnfErr)  
            break;
            
          // Create the new file
          HCreate(nomActuel.volume,nomActuel.directory,nomActuel.nom,fileSignature,nomActuel.type);
          erreurIO = HOpen(nomActuel.volume,nomActuel.directory,nomActuel.nom,fsRdWrPerm,&refNum);
        }
      while ( (erreurIO != noErr) && SignaleErreurEcriture() );
      break;
        
    case mode_Modification:
      erreurIO = HOpen(vraiNom.volume,vraiNom.directory,vraiNom.nom,fsRdWrPerm,&refNum);
      break;
        
    case mode_Reset:
      do
        {
          erreurIO = HOpen(vraiNom.volume,vraiNom.directory,vraiNom.nom,fsRdWrPerm,&refNum);
          
          // If file doesn't exist, create it
          if (erreurIO == fnfErr)
            {
              HCreate(vraiNom.volume,vraiNom.directory,vraiNom.nom,fileSignature,vraiNom.type);
              erreurIO = HOpen(vraiNom.volume,vraiNom.directory,vraiNom.nom,fsRdWrPerm,&refNum);
            }
          // If file exists and is open, empty it
          else if (erreurIO == noErr)
            {
              erreurIO = SetEOF(refNum, 0);
            }
        }
      while ( (erreurIO != noErr) && SignaleErreurEcriture() );
      break;
    }
  }
  
  myFSSpec.vRefNum = 0;

#else
  // Convert Pascal string to C string
  int len = ((unsigned char *)fNom.nom)[0];
  cname[len] = '\0';
  int i;
  for (i = len-1; i >= 0; i--)
    cname[i] = ((unsigned char *)fNom.nom)[i+1];

  // remove access path from name
  long index = 0;
  while (cname[i] != '\0') {
    if (cname[i] == ':')
      index = i;
    i++;
  }
  if (index != 0) {
    i = 0;
    while (cname[i+index+1] != '\0') {
      cname[i] = cname[i+index+1];
      i++;
    }
    cname[i] = '\0';
  }
    
  errno = noErr;

  switch (mode) {
  case mode_Lecture:
    handle = open (cname, O_RDONLY | O_BINARY);
    erreurIO = (OSErr) errno;
    break;
  case mode_Ecrasement:
  case mode_Reset:
    handle = open (cname, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0666);
    break;
  case mode_Modification:
    handle = open (cname, O_RDWR | O_BINARY);
    break;
  }
  vraiNom = fNom;
  erreurIO = (OSErr)(handle > 0 ? 0 : errno);
#endif

  if (erreurIO)
    SignaleErreur();
  fatalError = (erreurIO != noErr);
}

// Init the private fields correctly.
void Fichier::InitEverything(int fd)
{
  bufferIO    = NULL;
  erreurIO    = noErr;
  CloseFileWhenDelete();
  
  maxBuf      = 0;
  modifie     = FALSE;
  offsetCourant = 0;
  offsetDebut   = 0;
  offsetFin   = 0;
  offsetEndOfFile = 0;

  temporary   = FALSE;
  fatalError    = FALSE;
  
#ifdef macintosh
  refNum = fd;
#else
  handle = fd;
#endif

}

void Fichier::AllocateCacheBuffer(unsigned long tailleBuf)
{
  if (!erreurIO) {
    bufferIO = NULL;
    while (!bufferIO) {
      maxBuf = (tailleBuf + 511) & ~511;
#ifdef macintosh
      bufferIO = NewPtr(maxBuf);
#else
      bufferIO = (char*)malloc(maxBuf);
#endif
      tailleBuf >>= 1;
      if (tailleBuf == 0)
        break;
    }
    offsetDebut = 0;
    offsetFin = 0;
  }
}

void Fichier::InitCacheBuffer(unsigned long tailleBuf)
{
  long  offset;

  if (!erreurIO) {
    AllocateCacheBuffer(tailleBuf);
    offsetCourant = 0;
#ifdef macintosh
    erreurIO = GetEOF(refNum,&offset);
    assert(erreurIO == noErr);
#else
    // Skip ahead to find out length of file.
    errno = noErr;
    offset = lseek (handle, 0L, SEEK_END);
    erreurIO = (OSErr) errno;
    lseek (handle, 0L, SEEK_SET);
#endif
    if (offset > 0)
      offsetEndOfFile = offset;
    else
      offsetEndOfFile = 0;
    modifie = FALSE;
  }
}

// Write buffer on disk.
Boolean Fichier::Flush()
{
  if ( bufferIO && (!fatalError) )
    {
      do
        ValideTampon();
      while ( (erreurIO != noErr) && SignaleErreurEcriture() );
      if (erreurIO)
        SignaleErreur();
      fatalError = (erreurIO != noErr);
    }
  
  return fatalError;
}

Fichier::~Fichier()
{
  // If file with buffer, write the buffer on disk and delete it
  if (bufferIO) 
    {
      Flush();
#ifdef macintosh
      DisposePtr(bufferIO);
#else
      free(bufferIO);
#endif
    }
  
  // Close the file
#ifdef macintosh
  if (closeWhenDelete)
    FSClose(refNum);
  FlushVol(0,nomActuel.volume);
#else
  if (closeWhenDelete)
    close (handle);
#endif
  
#ifdef macintosh
  // If file corrupted
  if (fatalError)
    {
      if ( (modeOuverture == mode_Ecrasement) || (modeOuverture == mode_Reset) )
        {
          HDelete(nomActuel.volume,nomActuel.directory,nomActuel.nom);
          SignaleFichierDetruit();
        }
    }
  else if (temporary)
    {
      HDelete (nomActuel.volume, nomActuel.directory, nomActuel.nom);
    }
  // Else : if the file was already existing and if we created a new one with a 
  //        temporary name, we store the right signature and we erase the old one 
  else if (vraiNom != nomActuel)
    {
      FInfo fileInfos;
      HGetFInfo(nomActuel.volume,nomActuel.directory,nomActuel.nom, &fileInfos);
      fileInfos.fdType = vraiNom.type;
      HSetFInfo(nomActuel.volume,nomActuel.directory,nomActuel.nom, &fileInfos);

      HDelete(vraiNom.volume,vraiNom.directory,vraiNom.nom);
      HRename(nomActuel.volume,nomActuel.directory,nomActuel.nom, vraiNom.nom);
    }
#else
  if (temporary)
    {
      remove (&cname[0]);
    }
#endif
}

// Write a buffer on disk. Do not try again. Do not set 'fatalError'.
void Fichier::ValideTampon()
{
  erreurIO = noErr;

  if (bufferIO && modifie) 
    {
#ifdef macintosh
      erreurIO = SetFPos(refNum,fsFromStart,offsetDebut);
#else
      errno = noErr;
      lseek( handle, offsetDebut, SEEK_SET );
      erreurIO = errno;
#endif
      //    if (erreurIO) {
      // VISU2 "Seek error when writing : offset = %d, error = %d, try = %d\n", offsetDebut, erreurIO, nbErr FIN
      //    }
    
      if (!erreurIO) 
        {
          long nb = offsetFin - offsetDebut;
          if (nb) {
#ifdef macintosh
            erreurIO = FSWrite(refNum,&nb,bufferIO);
#else
            errno = noErr;
            nb = write (handle, bufferIO, nb );
            erreurIO = errno;
#endif
            if (!erreurIO) {
              modifie = FALSE;
            }
            //        else {
            // VISU2 "Error when writing : offset = %d, size = %d, error = %d, try = %d\n", offsetDebut, nb, erreurIO, nbErr FIN
            //        }
          }
        }
    }
}

// Write a buffer on disk. Try multiple times on Macintosh.
Boolean Fichier::Ecriture(ptr buffer, long nbOctets, long offset)
{
  const long  nbBytesToWrite = nbOctets;        // Backup
  
  assert(nbOctets > 0);
  
  if (!fatalError)
    {
      do
        {
          nbOctets = nbBytesToWrite;
      
          if (bufferIO) {
            offsetCourant = offset;
            EcritureBufferisee(buffer, nbOctets);
          } else {
#ifdef macintosh
            erreurIO = SetFPos(refNum,fsFromStart,offset);
            if (erreurIO == eofErr)
              if (!(erreurIO = SetEOF(refNum,offset)))
                erreurIO = SetFPos(refNum,fsFromStart,offset);
            if (!erreurIO)
              erreurIO = FSWrite(refNum,&nbOctets,buffer);
#else
            off_t pos;
            long nWritten;
            erreurIO = noErr;
            errno = noErr;
            pos = lseek (handle, offset, SEEK_SET);
            if (pos == offset) {
              nWritten = write (handle, buffer, nbOctets);
              if (nWritten != nbOctets)
                erreurIO = (OSErr) errno;
            }
            else
              erreurIO = (OSErr) errno;
#endif
          }
        }
      while ( (erreurIO != noErr) && SignaleErreurEcriture() );
    
      if (erreurIO)
        SignaleErreur();
      fatalError = (erreurIO != noErr);
    }
  return fatalError;
}

// Write a buffer on disk. Try multiple times on Macintosh.
Boolean Fichier::Ecriture(ptr buffer, long nbOctets)
{
  const long  currentOffset  = PositionCourante();  // Backups
  const long  nbBytesToWrite = nbOctets;        //
  
  assert(nbOctets > 0);
  
  if (!fatalError)
    {
      // Try to write
      if (bufferIO)
        EcritureBufferisee(buffer,nbOctets);
      else {
#ifdef macintosh
        erreurIO = FSWrite(refNum,&nbOctets,buffer);
#else
        long nWritten;
        erreurIO = noErr;
        errno = noErr;
        nWritten = write (handle, buffer, nbOctets);
        if (nWritten != nbOctets)
          erreurIO = (OSErr) errno;
#endif
      } 
      // On error, try again while user wants to
      if (erreurIO)
        Ecriture(buffer, nbBytesToWrite, currentOffset);
      
      assert(fatalError == (erreurIO != noErr));
    }
  return fatalError;
}

long Fichier::PositionCourante()
{
  long  offset;

  if (bufferIO)
    offset = offsetCourant;
  else {
#ifdef macintosh
    erreurIO = GetFPos(refNum,&offset);
#else
    errno = noErr;
    offset = lseek (handle, 0L, SEEK_CUR);
    erreurIO = (OSErr) errno;
#endif
  }
  return offset;
}

long Fichier::GetEndOfFile()
{
  long  offset;

  if (bufferIO)
    offset = offsetEndOfFile;
  else {
#ifdef macintosh
    erreurIO = GetEOF(refNum,&offset);
#else
    long  oldoffset;
    oldoffset = lseek (handle, 0L, SEEK_CUR);
    errno = noErr;
    offset = lseek (handle, 0L, SEEK_END);
    erreurIO = (OSErr) errno;
    lseek (handle, oldoffset, SEEK_SET);
#endif
  }

  return offset;
}

OSErr  Fichier::SetEndOfFile(long logEOF)
{
  char c = 'z';
  erreurIO = noErr;
  
#ifdef macintosh
  erreurIO = SetEOF(refNum,logEOF);
#else
  errno = noErr;
  lseek (handle, logEOF, SEEK_SET );
  errno = write( handle, &c, 1 );
  erreurIO = (OSErr) errno;
#endif
  return erreurIO;
}

// change the current position in the file
void Fichier::SetPosInFile(long offset)
{
  if (bufferIO) {
    offsetCourant = offset;
  } else {
#ifdef macintosh
    erreurIO = SetFPos(refNum,fsFromStart,offset);
#else
    off_t pos;
    erreurIO = noErr;
    errno = noErr;
    pos = lseek (handle, offset, SEEK_SET);
    if (pos != offset)
      erreurIO = errno;
#endif
  }
}

Boolean Fichier::Lecture(ptr buffer, long nbOctets, long offset)
{
  // change the current position in the file
  SetPosInFile(offset);
  // read the file
  return Lecture(buffer, nbOctets);
}

Boolean Fichier::Lecture(ptr buffer, long nbOctets)
{
  long nbOctetsToRead;
  long currentPosition = PositionCourante();
  Boolean tryToRead =  TRUE;    // test to retry three time to read a bloc if its not correct the first one
  short i = 0;
  while (tryToRead) {
    nbOctetsToRead = nbOctets;
    if (!erreurIO) {
      if (bufferIO)
        LectureBufferisee(buffer,nbOctetsToRead);
      else {
#ifdef macintosh
        erreurIO = FSRead(refNum,&nbOctetsToRead,buffer);
#else
        long nRead;
        erreurIO = noErr;
        errno = noErr;
        nRead = read (handle, buffer, nbOctetsToRead);
        if (nRead != nbOctetsToRead)
          erreurIO = errno;
#endif
      }
    }
    if (erreurIO && (i<2)) {
      SetPosInFile(currentPosition);
      tryToRead = TRUE;
    } else 
      tryToRead = FALSE;
    i++;
  }
  if (erreurIO) {
    SignaleErreur();
    return TRUE;
  } else {
    return FALSE;
  }
}

void Fichier::LectureBufferisee(ptr buffer, long nbOctets)
{
  //--- If the block to be read is entirely in the IO buffer,     ---//
  //--- we just copy the required amount of memory.       ---//
  
  if ((offsetCourant >= offsetDebut) && (offsetCourant + nbOctets <= offsetFin)) {
    BlockMove(bufferIO + offsetCourant - offsetDebut,buffer,nbOctets);
    offsetCourant += nbOctets;
  } else {

    //--- Else ---//

    // If the old IO buffer has been modified, we write it on the disk.
    Flush();
    
    // jump to the current offset in the file
    if (!erreurIO) {
#ifdef macintosh
      erreurIO = SetFPos(refNum,fsFromStart,offsetCourant);
#else
      errno = noErr;
      lseek( handle, offsetCourant, SEEK_SET );
      if( offsetEndOfFile <= offsetCourant )
        erreurIO = eofErr;
      else
        erreurIO = errno; // was noErr
#endif
    } else
      return;

    if (!erreurIO) {
    
      // If the block to be read is small enough to be contained in the IO buffer,
      // we read a complete IO buffer and we copy the required amount of memory.
      
      if ((unsigned long)nbOctets < maxBuf) {
        long nb = maxBuf;
        //        unsigned long nb = maxBuf;
#ifdef macintosh  
        erreurIO = FSRead(refNum,&nb,bufferIO);
        if (erreurIO == eofErr)
          erreurIO = noErr;
#else
        errno = noErr;
        nb = read( handle, bufferIO, nb );
        erreurIO = errno;
        if ((unsigned long)nb < maxBuf)
          erreurIO = noErr;
#endif
        if (!erreurIO) {
          offsetDebut = offsetCourant;
          offsetFin = offsetDebut + nb;
          if (nb < nbOctets) {
            nbOctets = nb;
            erreurIO = eofErr;
          }
          BlockMove(bufferIO,buffer,nbOctets);
          offsetCourant += nbOctets;
        }
  
        // If the block to be read is too big to be contained in the IO buffer,
        // we read it directly and keep the old IO buffer unchanged.
  
      } else {
#ifdef macintosh
        erreurIO = FSRead(refNum,&nbOctets,buffer);
#else
        long oldOctets = nbOctets;
        errno = noErr;
        nbOctets = read( handle, buffer, nbOctets );
        if (oldOctets != nbOctets)
          erreurIO = eofErr;
        else
          erreurIO = errno;
#endif
        offsetCourant += nbOctets;
      }
    }
  }
}

// Write a buffer on disk. Do not try again. Do not set 'fatalError'.
void Fichier::EcritureBufferisee(ptr buffer, long nbOctets)
{
  erreurIO = noErr;

  //--- If the block to be written is entirely in the IO buffer,  ---//
  //--- we just copy the required amount of memory.       ---//
  
  if ((offsetCourant >= offsetDebut) && (offsetCourant + nbOctets <= offsetFin)) {
    BlockMove(buffer,bufferIO + offsetCourant - offsetDebut,nbOctets);
    offsetCourant += nbOctets;
    if (offsetCourant > offsetEndOfFile)
      offsetEndOfFile = offsetCourant;
    modifie = TRUE;

    //--- If the block to be written can be contained in the biggest IO buffer         ---//
    //--- we copy the required amount of memory and we increase the size of the IO buffer. ---//
  
  } else if ((offsetCourant >= offsetDebut) && (offsetCourant + nbOctets <= offsetDebut + maxBuf)
             && (offsetFin >= offsetEndOfFile)) {
    BlockMove(buffer,bufferIO + offsetCourant - offsetDebut,nbOctets);
    offsetCourant += nbOctets;
    offsetFin = offsetCourant;
    if (offsetCourant > offsetEndOfFile)
      offsetEndOfFile = offsetCourant;
    modifie = TRUE;
  } else {

    //--- Else ---//

    // If the old IO buffer has been modified, we write it on the disk.

    ValideTampon();
    
    // If we are at the end of the file, we increase the file size.

    if (!erreurIO) {
#ifdef macintosh
      erreurIO = SetFPos(refNum,fsFromStart,offsetCourant);
      if (erreurIO == eofErr)
        if (!(erreurIO = SetEOF(refNum,offsetCourant)))
          erreurIO = SetFPos(refNum,fsFromStart,offsetCourant);
#else
      errno = noErr;
      lseek( handle, offsetCourant, SEEK_SET );
      erreurIO = errno;
#endif
    }

    // If the bloc to be written is small enough to be contained in the IO buffer,
    // we read a complete IO buffer and we copy the required amount of memory.
    
    if (!erreurIO) {
      if ((unsigned long)nbOctets < maxBuf) {
        long nb = maxBuf;
#ifdef macintosh
        erreurIO = FSRead(refNum,&nb,bufferIO);
        if( erreurIO == eofErr )
          erreurIO = noErr;
#else
        errno = noErr;
        nb = read(handle, bufferIO, nb );
        erreurIO = errno;
        if ((unsigned long)nb < maxBuf)
          erreurIO = noErr;
#endif
        if (!erreurIO) {
          offsetDebut = offsetCourant;
          if (nb < nbOctets)
            nb = nbOctets;
          offsetFin = offsetDebut + nb;
          BlockMove(buffer,bufferIO,nbOctets);
          offsetCourant += nbOctets;
          if (offsetCourant > offsetEndOfFile)
            offsetEndOfFile = offsetCourant;
          modifie = TRUE;
        }
        
        // If the bloc to be written os too big to be contained in the IO buffer,
        // we read it directly and keep the old IO buffer unchanged.
  
      } else {
#ifdef macintosh
        erreurIO = FSWrite(refNum,&nbOctets,buffer);
#else
        errno = noErr;
        long oldOctets = nbOctets;
        nbOctets = write( handle, buffer, nbOctets );
        erreurIO = errno;
        if(oldOctets != nbOctets)
          erreurIO = eofErr;
        else
          erreurIO = errno;
#endif
        if (!erreurIO) {
          offsetCourant += nbOctets;
          if (offsetCourant > offsetEndOfFile)
            offsetEndOfFile = offsetCourant;
        }
      }
    }
  }
}

OSErr Fichier::ResizeBuffer(unsigned long tailleBuf)
{
  if (bufferIO) {
    // If the old IO buffer has been modified, we write it on the disk.
    ValideTampon();

    if (!erreurIO) {
#ifdef macintosh
      DisposePtr(bufferIO);
#else
      free(bufferIO);
#endif
    }
  }
  if (!erreurIO)
    AllocateCacheBuffer(tailleBuf);
  
  return erreurIO;
}

Boolean Fichier::Ecriture(RGBColor x)
{
  return Ecriture((int16)x.red)
    || Ecriture((int16)x.green)
    || Ecriture((int16)x.blue);
}

Boolean Fichier::Lecture(RGBColor* x)
{
  return Lecture((int16 *)&x->red)
    || Lecture((int16 *)&x->green)
    || Lecture((int16 *)&x->blue);
}

Boolean Fichier::Ecriture(const Chaine& cetteChaine)
{
  return Ecriture((Ptr)&cetteChaine,cetteChaine.Longueur()+1);
}

Boolean Fichier::Lecture(Chaine* cetteChaine)
{
  unsigned char* pStr = *cetteChaine;
  
  if (!Lecture(pStr,sizeof(unsigned char)))
    if (*pStr)
      return Lecture(pStr+1,*pStr);
    else
      return FALSE;
  else
    return TRUE;
}


Boolean Fichier::Ecriture(const Chaine63& cetteChaine)
{
  return Ecriture((Ptr)&cetteChaine,cetteChaine.Longueur()+1);
}

Boolean Fichier::Lecture(Chaine63* cetteChaine)
{
  unsigned char* pStr = *cetteChaine;
  
  if (!Lecture(pStr,sizeof(unsigned char)))
    if (*pStr)
      return Lecture(pStr+1,*pStr);
    else
      return FALSE;
  else
    return TRUE;
}


void Fichier::SignaleErreur()
{
}

Boolean Fichier::SignaleErreurEcriture()
{
  return FALSE;
}

void Fichier::SignaleFichierDetruit()
{
}

//  ------------------------------------------------------------------------------------------------
//  Functions 'extern'
//  ------------------------------------------------------------------------------------------------

#ifdef macintosh
void SauvegardeMemosDirsVols(ref_Fichier fic)
{
  int32     i;
  ptr_MemoDirVol  aDetruire;
  
  for (i=1;enLecture;i++) {
    fic.Ecriture(i);
    //    fic.Ecriture(enLecture,sizeof(MemoDirVol));
    fic.Ecriture(int32(enLecture->type));
    fic.Ecriture(int32(enLecture->directory));
    fic.Ecriture(enLecture->volume);
    fic.Ecriture(int32(enLecture->suivant));
    
    aDetruire = enLecture;
    enLecture = enLecture->suivant;
    delete aDetruire;
  }
  fic.Ecriture(0L);
  
  for (i=1;enEcriture;i++) {
    fic.Ecriture(i);
    //    fic.Ecriture(enEcriture,sizeof(MemoDirVol));
    fic.Ecriture(int32(enEcriture->type));
    fic.Ecriture(int32(enEcriture->directory));
    fic.Ecriture(enEcriture->volume);
    fic.Ecriture(int32(enEcriture->suivant));
    
    aDetruire = enEcriture;
    enEcriture = enEcriture->suivant;
    delete aDetruire;
  }
  fic.Ecriture(0L);
}

void ChargementMemosDirsVols(ref_Fichier fic)
{
  int32     i, tmp;
  ptr_MemoDirVol  nouveau;
  
  do {
    fic.Lecture(&i);
    if (i) {
      nouveau = new MemoDirVol();
      //  fic.Lecture(nouveau,sizeof(MemoDirVol));
      fic.Lecture(&tmp);
      nouveau->type = OSType(tmp);
      fic.Lecture(&tmp);
      nouveau->directory = long(tmp);
      fic.Lecture(&(nouveau->volume));
      fic.Lecture(&tmp);
      
      nouveau->suivant = enLecture;
      enLecture = nouveau;
    }
  } while (i);

  do {
    fic.Lecture(&i);
    if (i) {
      nouveau = new MemoDirVol();
      //      fic.Lecture(nouveau,sizeof(MemoDirVol));
      fic.Lecture(&tmp);
      nouveau->type = OSType(tmp);
      fic.Lecture(&tmp);
      nouveau->directory = long(tmp);
      fic.Lecture(&(nouveau->volume));
      fic.Lecture(&tmp);
      
      nouveau->suivant = enEcriture;
      enEcriture = nouveau;
    }
  } while (i);
}

#endif

