//  ------------------------------------------------------------------------------------------------
//  MODULE    : CompresseurQuickTime
//  LANGUAGE  : C++
//  CREATOR   : Laurent SABORET
//  DATE    : Friday, October 1st 1993
//  DESCRIPTION : QuickTime compressor/decompressor
//  COMMENT   : Today, works only on Mac (compression/decompression) and SGI (decompression only)
//  SCCSID      : @(#)cpqtime.h 1.1 11:46:39 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef CompresseurQuickTime_h
  #define CompresseurQuickTime_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifdef macintosh
  #include  <Components.h>
  #include  <QuickTimeComponents.h>
    #include    <QDOffscreen.h>
#endif

  #include  "fpxcompress.h"

//  Constants
//  ---------

//  Classes declarations 
//  --------------------

  class obj_CompresseurQuickTime;
  typedef obj_CompresseurQuickTime* ptr_CompresseurQuickTime;
  typedef obj_CompresseurQuickTime& ref_CompresseurQuickTime;

//  Classes definitions
//  -------------------

  // AfficheDialogueCompression()'s answers
  typedef enum { CQT_OK,                // User pushed 'OK' button
           CQT_Annuler,             // User pushed 'Cancel' button
           CQT_SansCompression } CQT_Reponse; // User choosed 'Raw' compressor and 'Millions of colors'
  
  // A obj_CompresseurQuickTime object handles ALL compressors/decompressors of QuickTime
  // If QuickTime isn't installed, CompresseurPresent() will answer 'false'.
  class obj_CompresseurQuickTime : public obj_Compresseur
  {
  public:
  
                    obj_CompresseurQuickTime();
    virtual             ~obj_CompresseurQuickTime();
    
        // Open standard QuickTime dialog to get compression options
        CQT_Reponse       AfficheDialogueCompression();
    
        // Compress an image. Return false on error.
    virtual Boolean         Compresse(Ptr uncompressedDatas, short width, short height,
                          Ptr* compressedDatasPtr, long* compressedDatasSizePtr);
                      
        // Uncompress an image. Return false on error.
    virtual Boolean         Decompresse(Ptr uncompressedDatas, short width, short height,
                          Ptr compressedDatas, long compressedDatasSize);
                        
        // Purge data used by the compressor
    virtual Boolean         Purge();
    
        // Save and load the compressor's settings for the batch and the preferences
        // THESE FUNCTIONS MUST RESPECT BOTH ASCENDANT AND DESCENDANT COMPATIBILITY
        //
        // Rq : return any empty obj_AnyData if QuickTime is not present
    virtual obj_AnyData       GetCompressionSettings();
        // Rq : return false if QuickTime is not present
    virtual Boolean         SetCompressionSettings(const obj_AnyData& settings);
        
  private:
  
#ifdef macintosh
        ComponentInstance   componentInstance;    // MacOS object which handles QuickTime
        
        GWorldPtr       compressionGWorld;    // GWorld that contains the compressed image
        long          compressionGWorldWidth; //          ""
        long          compressionGWorldHeight;//          ""
        
        Ptr           compressionBuffer;    // Buffer with compressed datas returned 
        long          compressionBufferSize;  //  by Compresse() in *compressedDatasPtr

        ImageDescriptionHandle  compressionDescriptor;  // ImageDescription handle needed to uncompress
#endif
  };

//  Functions 'inline'
//  ------------------

//  Functions 'extern'
//  ------------------

//  Variables 'extern'
//  ------------------

//  ------------------------------------------------------------------------------------------------
  #endif // CompresseurQuickTime_h
//  ------------------------------------------------------------------------------------------------
