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

// Magick++ combined error/warning callback function
namespace Magick
{
  extern "C" {
  void LastErrorHandler( const unsigned int error_,
			 const char *message_ ,
			 const char *qualifier_);
  }
}

// Initialize static members
Magick::LastError*  Magick::LastError::_instance = 0;

// Constructor
Magick::LastError::LastError( void )
  : _isError(false),
    _error(0),
    _syserror(0),
    _message(),
    _qualifier()
{
}

// Construct with parameters
Magick::LastError::LastError( int error_, const std::string message_,
			      const std::string qualifier_ )
  : _isError(true),
    _error(error_),
    _syserror(errno),
    _message(message_),
    _qualifier(qualifier_)
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
      MagickLib::SetWarningHandler( LastErrorHandler );
    }
  return _instance;
}

// Clear out existing error info
void Magick::LastError::clear ( void )
{
  _isError   = false;
  _error     = 0;
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

  switch ( _error )
    {
      // Warnings
    case MagickLib::ResourceLimitWarning :
      clear();
      throw WarningResourceLimit( message );
    case MagickLib::XServerWarning :
      clear();
      throw WarningXServer( message );
    case MagickLib::OptionWarning :
      clear();
      throw WarningOption( message );
    case MagickLib::DelegateWarning :
      clear();
      throw WarningDelegate( message );
    case MagickLib::MissingDelegateWarning :
      clear();
      throw WarningMissingDelegate( message );
    case MagickLib::CorruptImageWarning :
      clear();
      throw WarningCorruptImage( message );
    case MagickLib::FileOpenWarning :
      clear();
      throw WarningFileOpen( message );
      // Errors
    case MagickLib::ResourceLimitError :
      clear();
      throw ErrorResourceLimit( message );
    case MagickLib::XServerError :
      clear();
      throw ErrorXServer( message );
    case MagickLib::OptionError :
      clear();
      throw ErrorOption( message );
    case MagickLib::DelegateError :
      clear();
      throw ErrorDelegate( message );
    case MagickLib::MissingDelegateError :
      clear();
      throw ErrorMissingDelegate( message );
    case MagickLib::CorruptImageError :
      clear();
      throw ErrorCorruptImage( message );
    case MagickLib::FileOpenError :
      clear();
      throw ErrorFileOpen( message );
    case MagickLib::UndefinedError :
    default :
      clear();
      throw ErrorUndefined( message );
    }
}

//
// Magick++ combined error/warning callback function
//
void Magick::LastErrorHandler( const unsigned int error_,
			       const char *message_ ,
			       const char *qualifier_)
{
  LastError* errPtr = LastError::instance();
  errPtr->isError( true );
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
