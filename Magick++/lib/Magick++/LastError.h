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
    ~LastError( void );
    
    // Obtain instance of object
    static LastError*      instance ( void );
    
    // Test to see if error or warning is set
    bool                   isError( void ) const;
    
    // Clear out existing error info
    void                   clear ( void );
    
    // Error code
    void                   error ( MagickLib::ErrorType error_ );
    MagickLib::ErrorType   error ( void ) const;

    // Warning code
    void                   warning ( MagickLib::WarningType warning_ );
    MagickLib::WarningType warning ( void ) const;
    
    // System errno
    void                   syserror ( int syserror_ );
    int                    syserror ( void ) const;
    
    // Error message
    void                   message ( std::string message_ );
    std::string            message ( void ) const;
    
    // Error qualifier
    void                   qualifier ( std::string qualifier_ );
    std::string            qualifier ( void ) const;
    
    // Throw exception corresponding to error (if any)
    void                   throwException( void );
    
  private:
    
    // Don't support copy constructor
    LastError ( const LastError& original_ );
    
    // Don't support assignment
    LastError& operator = ( const LastError& original_ );
    
    MagickLib::ErrorType   _error;
    MagickLib::WarningType _warning;
    int                    _syserror;
    std::string            _message;
    std::string            _qualifier;
    
    static LastError*      _instance;
  };
} // namespace Magick

//
// Inlines
//

// Test to see if object contains error
inline bool Magick::LastError::isError( void ) const
{
  return ( _error   != MagickLib::UndefinedError ||
	   _warning != MagickLib::UndefinedWarning );
}

// Error code
inline void Magick::LastError::error ( MagickLib::ErrorType error_ )
{
  _error = error_;
}
inline MagickLib::ErrorType Magick::LastError::error ( void ) const
{
  return _error;
}

// Warning code
inline void Magick::LastError::warning ( MagickLib::WarningType warning_ )
{
  _warning = warning_;
}
inline MagickLib::WarningType Magick::LastError::warning ( void ) const
{
  return _warning;
}

// System error
inline void Magick::LastError::syserror ( int syserror_ )
{
  _syserror = syserror_;
}
inline int Magick::LastError::syserror ( void ) const
{
  return _syserror;
}
  
// Error message
inline void Magick::LastError::message ( std::string message_ )
{
  _message = message_;
}
inline std::string Magick::LastError::message ( void ) const
{
  return _message;
}

// Error qualifier
inline void  Magick::LastError::qualifier ( std::string qualifier_ )
{
  _qualifier = qualifier_;
}
inline std::string Magick::LastError::qualifier ( void ) const
{
  return _qualifier;
}


#endif // LastError_header
