// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Implementation of MagickError (ImageMagick error handler)
//

#define MAGICK_IMPLEMENTATION

#include <string>

#include <errno.h>

#include <Magick++/Include.h>

using namespace std;

#include <Magick++/LastError.h>
#include <Magick++/Exception.h>

// Magick++ error/warning callback functions
namespace Magick
{
  extern "C" {
  void LastErrorHandler( const MagickLib::ErrorType error_,
			 const char *message_ ,
			 const char *qualifier_ );
  void LastWarningHandler( const MagickLib::WarningType warning_,
			   const char *message_ ,
			   const char *qualifier_ );
  }
}

// Initialize static members
Magick::LastError*  Magick::LastError::_instance = 0;

// Constructor
Magick::LastError::LastError( void )
  : _error(MagickLib::UndefinedError),
    _warning(MagickLib::UndefinedWarning),
    _syserror(0),
    _message(),
    _qualifier()
{
}

// Destructor
Magick::LastError::~LastError( void )
{
  // Nothing to do
}

// Obtain instance of object
Magick::LastError* Magick::LastError::instance ( void )
{
  if ( _instance == 0 )
    {
      // Assuming that new throws a bad_alloc exception here if
      // allocation error occurs
      _instance = new LastError();

      // Register error callback function with ImageMagick
      MagickLib::SetErrorHandler( LastErrorHandler );
      MagickLib::SetWarningHandler( LastWarningHandler );
    }
  return _instance;
}

// Clear out existing error info
void Magick::LastError::clear ( void )
{
  _error     = MagickLib::UndefinedError;
  _warning   = MagickLib::UndefinedWarning;
  _syserror  = 0;
  _message.erase();
  _qualifier.erase();
}

// Throw exception corresponding to error (if any)
// Clears error info before throw.
void Magick::LastError::throwException( void )
{
  if ( !isError() )
    return;

  // Format error message ImageMagick-style
  std::string message = MagickLib::SetClientName((char *) NULL);
  if ( message.length() > 0 )
    message += std::string(": ");

  if ( _message.length() > 0 )
    message +=  _message;

  if ( _qualifier.length() > 0 )
    message += " (" + _qualifier + ")";

  // FIXME: For some reason this all of a sudden became necessary on a SVR4 box!
#undef strerror
  if ( syserror() )
    message += std::string(" [") + strerror(syserror()) + std::string("]");

  if ( warning() != MagickLib::UndefinedWarning )
    {
      MagickLib::WarningType warningVal = warning();
      clear();
      
      switch ( warningVal )
	{
	  // Warnings
	case MagickLib::ResourceLimitWarning :
	  throw WarningResourceLimit( message );
	case MagickLib::XServerWarning :
	  throw WarningXServer( message );
	case MagickLib::OptionWarning :
	  throw WarningOption( message );
	case MagickLib::DelegateWarning :
	  throw WarningDelegate( message );
	case MagickLib::MissingDelegateWarning :
	  throw WarningMissingDelegate( message );
	case MagickLib::CorruptImageWarning :
	  throw WarningCorruptImage( message );
	case MagickLib::FileOpenWarning :
	  throw WarningFileOpen( message );
	case MagickLib::BlobWarning :
	  throw WarningBlob ( message );
	case MagickLib::CacheWarning :
	  throw WarningCache ( message );
	case MagickLib::UndefinedWarning :
	default :
	  throw WarningUndefined( message );
	}
    }
  else if ( error() != MagickLib::UndefinedError )
    {
      MagickLib::ErrorType errorVal = error();
      clear();
      
      switch ( errorVal )
	{
	  // Errors
	case MagickLib::ResourceLimitError :
	  throw ErrorResourceLimit( message );
	case MagickLib::XServerError :
	  throw ErrorXServer( message );
	case MagickLib::OptionError :
	  throw ErrorOption( message );
	case MagickLib::DelegateError :
	  throw ErrorDelegate( message );
	case MagickLib::MissingDelegateError :
	  throw ErrorMissingDelegate( message );
	case MagickLib::CorruptImageError :
	  throw ErrorCorruptImage( message );
	case MagickLib::FileOpenError :
	  throw ErrorFileOpen( message );
	case MagickLib::BlobError :
	  throw ErrorBlob ( message );
	case MagickLib::CacheWarning :
	  throw ErrorCache ( message );
	case MagickLib::UndefinedError :
	default :
	  throw ErrorUndefined( message );
	}
    }
}

//
// Magick++ error callback function
//
void Magick::LastErrorHandler( const MagickLib::ErrorType error_,
			       const char *message_ ,
			       const char *qualifier_)
{
  LastError* errPtr = LastError::instance();
  errPtr->error( error_ );
  errPtr->syserror( errno );
  if ( message_ )
    errPtr->message( message_ );
  else
    errPtr->message( "" );
  if ( qualifier_ )
    errPtr->qualifier( qualifier_ );
  else
    errPtr->qualifier( "" );

  // Clear out system errno now that it has been collected.
  errno = 0;
}

//
// Magick++ warning callback function
//
void Magick::LastWarningHandler( const MagickLib::WarningType warning_,
				 const char *message_ ,
				 const char *qualifier_)
{
  LastError* errPtr = LastError::instance();
  errPtr->warning( warning_ );
  errPtr->syserror( errno );
  if ( message_ )
    errPtr->message( message_ );
  else
    errPtr->message( "" );
  if ( qualifier_ )
    errPtr->qualifier( qualifier_ );
  else
    errPtr->qualifier( "" );

  // Clear out system errno now that it has been collected.
  errno = 0;
}
