//  ----------------------------------------------------------------------------
//  MODULE    : Compressor
//  LANGUAGE  : C++
//  AUTHOR    : Adolfo VIDE
//  DATE    : Tuesday, September 15th 1992
//  DESCRIPTION : 
//  COMMENT   :
//  SCCSID      : @(#)compress.h  1.1 11:46:37 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef Compresseur_h
  #define Compresseur_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef IVue_h
  #include  "ri_sys.h"
#endif

//  Classes declarations 
//  --------------------

  class obj_Compresseur;
  typedef obj_Compresseur* ptr_Compresseur;
  typedef obj_Compresseur& ref_Compresseur;
  
  class obj_TousLesCodecs;
  typedef obj_TousLesCodecs* ptr_TousLesCodecs;
  typedef obj_TousLesCodecs& ref_TousLesCodecs;

  class obj_ServerParam;
  typedef obj_ServerParam* ptr_ServerParam;
  typedef obj_ServerParam& ref_ServerParam;

//  Classes definitions
//  -------------------

  class obj_Compresseur : public PToolkitObject
  {
  public:
  
                obj_Compresseur() { compresseurPresent = decompresseurPresent = TRUE; compresseurLocked = FALSE; }
    virtual         ~obj_Compresseur();
    
        // Compress an image. Return false on error.
    virtual Boolean     Compresse(Ptr uncompressedData, short width, short height,
                      Ptr* pcompressedData, long* sizeCompressed);
                      
        // Uncompress an image. Return false on error.
    virtual Boolean     Decompresse(Ptr uncompressedData, short width, short height,
                      Ptr compressedData, long sizeCompressed);
    
        void      Lock();     // Flag compressor as in use
        void      UnLock();   // Flag compressor as free
    
        // Purge data used by the compressor
    virtual Boolean     Purge();

        short     CompresseurPresent()   { return compresseurPresent; }
        short     DecompresseurPresent() { return decompresseurPresent; }
        
  protected:
        short     compresseurPresent, decompresseurPresent;
        Boolean     compresseurLocked;
        long      compressionSubtype; 
        
  };


  // One object of this class knows all the compressors/decompressors installed on this machine
  class obj_TousLesCodecs : public PToolkitObject
  {
  public:
  
        // Initialize the array of compressors lesCodecs[] for this machine
                obj_TousLesCodecs();
    virtual         ~obj_TousLesCodecs();
    
        // To consult the array of compressors
        ptr_Compresseur operator [](TLC_IdCodec numero) { return lesCodecs[numero]; }

        // Purge data used by the compressors
        Boolean     Purge();

  private:
        // Array of compressors installed on this machine
        obj_Compresseur **lesCodecs;
  };

//  Global variables
//  ----------------

  extern ptr_TousLesCodecs tousLesCodecs;
  #define          allCodecs    (*tousLesCodecs)

//  Functions 'inline'
//  ------------------

inline void obj_Compresseur::Lock() { compresseurLocked = TRUE; }

inline void obj_Compresseur::UnLock() { compresseurLocked = FALSE; }

//  Functions 'extern'
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // Compresseur_h
//  ----------------------------------------------------------------------------
