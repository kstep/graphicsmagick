// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Definition of LastError (ImageMagick error handler)
//
// This singleton class serves to collect the last error/warning
// reported by the ImageMagick error callback.
//
// This class is a Magick++ implementation class and is not intended
// for use by end-users.
// 

#if !defined(LastError_header)
#define LastError_header

#include <string>

const std::string MagickNullString = "";

namespace Magick
{
  class LastError
  {
    
  public:
    LastError( void );
    LastError( int error_,
		     const std::string message_,
		     const std::string qualifier_ = MagickNullString );
    ~LastError( void );
    
    // Obtain instance of object
    static LastError* instance ( void );
    
    // Test to see if object contains error
    void                 isError( bool isError_ );
    bool                 isError( void ) const;
    
    // Clear out existing error info
    void                 clear ( void );
    
    // Error code
    void                 error ( int error_ );
    int                  error ( void ) const;
    
    // System errno
    void                 syserror ( int syserror_ );
    int                  syserror ( void ) const;
    
    // Error message
    void                 message ( std::string message_ );
    std::string          message ( void ) const;
    
    // Error qualifier
    void                 qualifier ( std::string qualifier_ );
    std::string          qualifier ( void ) const;
    
    // Throw exception corresponding to error (if any)
    void                 throwException( void ) const;
    
  private:
    
    // Don't support copy constructor
    LastError ( const LastError& original_ );
    
    // Don't support assignment
    LastError& operator = ( const LastError& original_ );
    
    bool                 _isError;
    int                  _error;
    int                  _syserror;
    std::string          _message;
    std::string          _qualifier;
    
    static LastError* _instance;
  };
} // namespace Magick
#endif // LastError_header
