//  ------------------------------------------------------------------------------------------------
//  MODULE      : Fichier
//  LANGUAGE    : C++
//  AUTHOR      : Adolfo VIDE
//  DATE        : Thursday October 15th, 1992
//  DESCRIPTION :
//  COMMENT   :
//  SCCSID      : @(#)a_file.h  1.1 11:49:53 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
#ifndef Fichier_h
#define Fichier_h
#ifndef ApplCommun_h
#include    "common_a.h"
#endif
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifdef macintosh
#  include    <StandardFile.h>
#endif

#include    "filename.h"

//  Constants
//  ---------

enum mode_Ouverture {
  mode_Lecture,
  mode_Ecrasement,    //  if existing       => Reset file, but there is a backup if error
  mode_Modification,  //  if non existing   => Error
  mode_Reset          //  if existing       => Reset file and no backup
};

#ifdef applec
#  define DEFAULT_SIGNATURE "????"
#else
#  define DEFAULT_SIGNATURE "\?\?\?\?"
#endif

//  Global Functions
//  ________________

uint16          shortswap(uint16);  // Swap for Big/Little Endian
uint32          longswap(uint32);   // Swap for Big/Little Endian
#  define int32swap   longswap
#  define int16swap   shortswap

//  Classes Declarations
//  --------------------

class Fichier;
typedef Fichier* ptr_Fichier;
typedef Fichier& ref_Fichier;

class obj_Signature;
typedef obj_Signature* ptr_Signature;
typedef obj_Signature& ref_Signature;

//  Classes Definitions
//  -------------------

class Fichier {
public:

#ifdef macintosh
  friend void SauvegardeMemosDirsVols(ref_Fichier fic);
  friend void ChargementMemosDirsVols(ref_Fichier fic);
#endif
        // To be cleaned up one day...
  friend class obj_Brosse;
  friend class obj_Trace;
  friend class obj_EltTrace;
  friend class obj_AnimTrace;
  friend class obj_EltAnimTrace;

  // Creators with a file name
  Fichier(const FicNom& fNom, mode_Ouverture mode);
  Fichier(const FicNom& fNom, mode_Ouverture mode, unsigned long tailleBuf);
  Fichier(const FicNom& fNom, mode_Ouverture mode, const obj_Signature &fileSignature);
  Fichier(const FicNom& fNom, mode_Ouverture mode,
          unsigned long tailleBuf, const obj_Signature &fileSignature);
  // Creators with a file descriptor
  Fichier(int fd, mode_Ouverture mode);
  Fichier(int fd, mode_Ouverture mode, unsigned long tailleBuf);
  Fichier(int fd, mode_Ouverture mode, const obj_Signature &fileSignature);
  Fichier(int fd, mode_Ouverture mode,
          unsigned long tailleBuf, const obj_Signature &fileSignature);

  ~Fichier();
  FicNom          GetFileName() const;
  // Sorry, I couldn't find anything else to be able
  // to access the resource fork of a file being
  // overwritten (GetFileName() above returns the wrong
  // file name). Don't use it if you don't need it. (XS)
  FicNom      GetInternalFileName (void) const;

  void      DoNotCloseFileWhenDelete();   // To avoid closing the file when deleting the IVUE
  void      CloseFileWhenDelete();      // File will be closed when deleting (default mode)

protected:  // Before using those functions, please think portability

  // Pointers to unspecified objects
  // -------------------------------
  Boolean         Ecriture(ptr buffer, long nbOctets, long offset);
  Boolean         Lecture(ptr buffer, long nbOctets, long offset);
  Boolean         Ecriture(ptr buffer, long nbOctets);
  Boolean         Lecture(ptr buffer, long nbOctets);

public:

  // Int32s
  // ------

#ifdef DEFINE_INT32_FUNCTIONS
  Boolean         Ecriture(int32 ceLong);
  Boolean         Lecture(int32* ceLong);
  Boolean         EcritureOffset(int32 ceLong, long offset);
  Boolean         LectureOffset(int32* ceLong, long offset);
#endif

  // Arrays of Int32s
  // ----------------
  //  Modified by GDN -- Visual C++ 2.0 gives error with const int32,
  //  because the Ecriture functions actually modify the array contents with byte
  //  and word swaps before writing, then change them back.
  //  If not for this, these functions could take const (indeed it compiles on Mac)
  Boolean         EcritureTableau( int32 *array, int32 t);
  Boolean         LectureTableau(int32 *array, int32 t);
  // Added by GDN
  Boolean         LectureTableauOffset(int32 *array,int32 t, long offset);

  // Arrays of char
  // --------------
  Boolean         EcritureTableau(const unsigned char *array,int32 nshort);
  Boolean         LectureTableau(unsigned char *array,int32 nshort);
  Boolean         LectureTableauOffset(unsigned char *array,int32 nshort, long offset);

  // Shorts
  // ------
  Boolean         Ecriture(short ceShort);
  Boolean         Lecture(short* ceShort);

  // Arrays of shorts
  // ----------------
  //  Modified by GDN -- Visual C++ 2.0 gives error with const short
  Boolean         EcritureTableau( short *array, int32 t);
  Boolean         LectureTableau(short *array,int32 nshort);

#ifdef SHOULDNOT
  // Floats
  // ------
  Boolean         Ecriture(float ceFloat);
  Boolean         Lecture(float* ceFloat);

  // Doubles
  // -------
  Boolean         Ecriture(double ceDouble);
  Boolean         Lecture(double* ceDouble);
#endif

  // Strings
  // -------
  Boolean         Ecriture(const Chaine& string);
  Boolean         Lecture(Chaine* string);

  // Chaine63
  // ---------
  Boolean         Ecriture(const Chaine63& string);
  Boolean         Lecture(Chaine63* string);

  // Boolean
  // -------
  Boolean         Ecriture(Boolean x);
  Boolean         Lecture(Boolean* x);

  // RGBColor
  // -------
  Boolean         Ecriture(RGBColor x);
  Boolean         Lecture(RGBColor* x);

  // Array of RGBColor
  // ------------------
  Boolean         EcritureTableau(const RGBColor *array,int32 nshort);
  Boolean         LectureTableau(RGBColor *array,int32 nshort);

  // Text
  // ----
  Boolean         LectureTexte(unsigned char* text, long nb);
  Boolean         EcritureTexte(const Chaine& string);
  Boolean         EcritureTexteOffset(const Chaine& string, long offset);

  // Goodies
  // -------
  OSErr           Erreur() const;             // Result of last action on the file
  Boolean         FatalError() const;         // == true if a non corrected error occurred

  void            DeleteOnClose (Boolean flag);

  long            PositionCourante();
  void            SetPosInFile(long offset); // change the position in the file

  long            GetEndOfFile();
  OSErr           SetEndOfFile(long logEOF);
  Boolean         Flush();                    // Write buffer on disk.
  // This function is called by ~Fichier()
  OSErr           ResizeBuffer(unsigned long tailleBuf); // to resize the buffer

protected:

private:
  void            ValideTampon();
  void            LectureBufferisee(ptr buffer, long nbOctets);
  void            EcritureBufferisee(ptr buffer, long nbOctets);

  void            Ouverture(const FicNom& fNom, mode_Ouverture mode, OSType fileSignature);
  void            InitEverything(int fd = 0);
  void            InitCacheBuffer(unsigned long tailleBuf);
  void            AllocateCacheBuffer(unsigned long tailleBuf);

protected:

  virtual Boolean         SignaleErreurEcriture();    // Called first when an error occured while writing.
  // Fichier::SignaleErreurEcriture() asks to user
  // if he wants to try again (returns true to try again)
  virtual void            SignaleErreur();            // Called when an error occurred.
  // Fichier::SignaleErreur() does nothing.
  virtual void            SignaleFichierDetruit();    // Called after deleting a corrupted file.

  char*           bufferIO;
  OSErr           erreurIO;
  unsigned long   maxBuf;
  Boolean         modifie;
  unsigned long   offsetCourant;
  unsigned long   offsetDebut;
  unsigned long   offsetFin;
  unsigned long   offsetEndOfFile;

  const   mode_Ouverture  modeOuverture;              // Open mode

  Boolean         temporary;                  // True if file is to be deleted when closed
  Boolean         fatalError;                 // == true if a non corrected error occurred
  // since file is open
#ifdef macintosh
  short           refNum;           // File reference number
  FicNom          vraiNom,                    // Name asked by the user
    nomActuel;                  // Name used at that time
#else

  int           handle;           // File descriptor
  FicNom          vraiNom;                    // Name asked by the user
  char            cname[255];         // UNIX path name to the file
#endif
  Boolean     closeWhenDelete;      // Flag indicating if we have to close the handle
  // when deleting the object (for IVUE Toolkit)
};



class obj_Signature {
public:
  obj_Signature(OSType macType);
  operator OSType() const;

private:
  OSType          realSignature;
};


//  Functions 'inline'
//  ------------------

// INLINE METHODS OF Fichier
inline FicNom Fichier::GetFileName() const { return vraiNom; }

inline FicNom Fichier::GetInternalFileName (void) const
{
#ifdef macintosh
  return nomActuel;
#else
  return vraiNom;
#endif
}

inline OSErr Fichier::Erreur() const { return erreurIO; }

inline void
Fichier::DeleteOnClose (Boolean flag)
{
  temporary = flag;
}

inline Boolean Fichier::FatalError() const
{
#ifdef macintosh
  return fatalError;
#else
  return FALSE;
#endif
}



#ifdef DEFINE_INT32_FUNCTIONS

inline uint32   longswap( uint32 x )
{
  uint32 a, b;
  a = ( x & 0xffff0000 ) >> 16;
  b = x & 0x0000ffff;
  return ( ((uint32)shortswap((uint16)b) << 16) | (uint32)shortswap((uint16)a) );
}

inline Boolean Fichier::Ecriture(int32 ceLong)
{
#ifdef __bigEndian
  return Ecriture((ptr)&ceLong,sizeof(ceLong));
#else
  int32 x = int32swap(ceLong);
  return Ecriture((ptr)&x,sizeof(x));
#endif
}


inline Boolean Fichier::Lecture(int32* ceLong)
{
#ifdef __bigEndian
  return Lecture((ptr) ceLong,sizeof(*ceLong));
#else
  Boolean ret;
  int32 x;

  ret = Lecture((ptr) &x,sizeof(x));
  *ceLong = (int32)(int32swap (x));

  return ret;
#endif
}


inline Boolean Fichier::EcritureOffset(int32 ceLong, long offset)
{
  int32 x;
#ifdef __bigEndian
  x = ceLong;
#else
  x = int32swap(ceLong);
#endif
  return Ecriture((ptr)&x,sizeof(x), offset);
}


inline Boolean Fichier::LectureOffset(int32* ceLong, long offset)
{
  Boolean ret;
  int32 x;
  ret = Lecture((ptr) &x,sizeof(x), offset);
#ifdef __bigEndian
  *ceLong = (long)x;
#else
  *ceLong = (long)(int32swap (x));
#endif
  return ret;
}

#endif  // DEFINE_INT32_FUNCTIONS


// Added by GDN
inline uint16   shortswap( uint16 x )
{
  uint16 a, b;
  a = ( x & 0xff00 ) >> 8;
  b = x & 0x00ff;
  return ( (b << 8) | a );
}

inline Boolean Fichier::Ecriture(short ceShort)
{
  int16 x;
#ifdef __bigEndian
  x = ceShort;
#else
  x = int16swap(ceShort);
#endif
  return Ecriture((ptr)&x,sizeof(short));
}


inline Boolean Fichier::Lecture(short* ceShort)
{
  Boolean ret;
  int16 x;
  ret = Lecture((ptr) &x,sizeof(x));
#ifdef __bigEndian
  *ceShort = (short)x;
#else
  *ceShort = (short)(int16swap (x));
#endif
  return ret;
}


#ifdef SHOULDNOT
inline Boolean Fichier::Ecriture(float ceFloat) { return Ecriture((ptr)&ceFloat,sizeof(float)); }
inline Boolean Fichier::Lecture(float* ceFloat) { return Lecture((ptr)ceFloat,sizeof(float)); }
inline Boolean Fichier::Ecriture(double ceDouble) { return Ecriture((ptr)&ceDouble,sizeof(double)); }
inline Boolean Fichier::Lecture(double* ceDouble) { return Lecture((ptr)ceDouble,sizeof(double)); }
#endif

inline Boolean Fichier::LectureTableau(unsigned char *array, int32 nshort)
{
  return Lecture((ptr)array, nshort * sizeof(*array));
}

inline Boolean Fichier::LectureTableauOffset(unsigned char *array, int32 nshort, long offset)
{
  return Lecture((ptr)array, nshort * sizeof(*array), offset);
}

inline Boolean Fichier::LectureTableau(short *array, int32 nshort)
{
  Boolean ret;
  ret = Lecture((ptr)array, nshort * sizeof(short));
#ifndef __bigEndian
  if (!ret) {
    for (int32 i = 0; i < nshort; i++)
      array[i] = shortswap (array[i]);
  }
#endif
  return ret;
}


inline Boolean Fichier::LectureTableau(int32 *array, int32 t)
{
  Boolean ret;
  ret = Lecture((ptr)array, t * sizeof(int32));
#ifndef __bigEndian
  if (!ret) {
    for (int32 i = 0; i < t; i++)
      array[i] = int32swap (array[i]);
  }
#endif
  return ret;
}

// Added by GDN
inline Boolean Fichier::LectureTableauOffset(int32 *array, int32 t, long offset)
{
  Boolean ret;
  ret = Lecture((ptr)array, t * sizeof(*array), offset);
#ifndef __bigEndian
  if (!ret) {
    for (int32 i = 0; i < t; i++)
      array[i] = int32swap (array[i]);
  }
#endif
  return ret;
}


inline Boolean Fichier::LectureTableau(RGBColor *array, int32 t)
{
  Boolean ret = FALSE;
  for (int32 i = 0; i < t; i++)
    ret = ret || Lecture(&(array[i]));
  return ret;
}


inline Boolean Fichier::EcritureTableau(const unsigned char *array, int32 nshort)
                            { return Ecriture((ptr)array, nshort * sizeof(*array)); }

// It is not a bit too long for an inline method ???
inline Boolean Fichier::EcritureTableau(short *array, int32 nshort)
{
  Boolean ret;

#ifndef __bigEndian
  int32 i;
  for (i = 0; i < nshort; i++)
    array[i] = shortswap (array[i]);
#endif

  ret = Ecriture((ptr)array, nshort * sizeof(short));

#ifndef __bigEndian
  for (i = 0; i < nshort; i++)
    array[i] = shortswap (array[i]);
#endif

  return ret;
}



//  Modified by GDN -- Visual C++ 2.0 gives error with const int32
inline Boolean Fichier::EcritureTableau( int32 *array, int32 t)
{
  Boolean ret;
#ifndef __bigEndian
  int32 i;
  for (i = 0; i < t; i++)
    array[i] = int32swap (array[i]);
#endif

  ret = Ecriture((ptr)array, t * sizeof(int32));

#ifndef __bigEndian
  for (i = 0; i < t; i++)
    array[i] = int32swap (array[i]);
#endif

  return ret;
}


inline Boolean Fichier::EcritureTableau(const RGBColor *array, int32 t)
{
  Boolean ret = FALSE;
  for (int32 i = 0; i < t; i++) ret = ret || Ecriture(array[i]);
  return ret;
}

inline Boolean Fichier::Ecriture(Boolean x)
{
  return Ecriture(&x,sizeof(Boolean));
}

inline Boolean Fichier::Lecture(Boolean* x)
{
  return Lecture(x,sizeof(Boolean));
}

inline Boolean Fichier::LectureTexte(unsigned char* text, long nb)
{
  return Lecture(text, nb);
}

inline Boolean Fichier::EcritureTexte(const Chaine& string)
{
  return Ecriture(string.Texte(), string.Longueur());
}

inline Boolean Fichier::EcritureTexteOffset(const Chaine& string, long offset)
{
  return Ecriture(string.Texte(),string.Longueur(),offset);
}


// INLINE METHODS OF obj_Signature
inline obj_Signature::obj_Signature(OSType macType) { realSignature = macType; }
inline obj_Signature::operator OSType() const { return realSignature; }


//  Functions 'extern'
//  ------------------

#ifdef macintosh
extern void SauvegardeMemosDirsVols(ref_Fichier fic);
extern void ChargementMemosDirsVols(ref_Fichier fic);
#endif

//  Variables 'extern'
//  ------------------

extern OSType signature;

//  ------------------------------------------------------------------------------------------------
#endif // Fichier_h
//  ------------------------------------------------------------------------------------------------
