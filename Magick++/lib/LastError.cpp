// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Implementation of MagickError (ImageMagick error handler)
//

#define MAGICK_IMPLEMENTATION

#include <string>

#include <errno.h>

#include "Magick++/Include.h"

using namespace std;

#include "Magick++/LastError.h"
#include "Magick++/Exception.h"

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

    // Destruction guard class.  Calls doDelete() if program exits
    class LastErrorGuard
    {
	public:
	LastErrorGuard( void )
	{
	}
	~LastErrorGuard( void )
	{
	    _singleton.doDelete();
	}
	private:
	LastError _singleton;
    };
}

// Initialize LastError static members
Magick::LastErrorInst* Magick::LastError::_instance = 0;     // LastErrorInst pointer
unsigned               Magick::LastError::_refCount = 0;     // Reference counter
bool                   Magick::LastError::_doDelete = false; // Object is to be deleted

// Allocate static guard class in this translation unit
// Static guard class is destroyed when program exits.
static Magick::LastErrorGuard LastErrorGuard_inst;

// Constructor
Magick::LastErrorInst::LastErrorInst( void )
  : _error(MagickLib::UndefinedError),
    _warning(MagickLib::UndefinedWarning),
    _syserror(0),
    _message(),
    _qualifier()
{
    // Register error callback function with ImageMagick
    MagickLib::SetErrorHandler( LastErrorHandler );
    MagickLib::SetWarningHandler( LastWarningHandler );
}

// Destructor
Magick::LastErrorInst::~LastErrorInst( void )
{
  // Nothing to do
}

//
// Singleton implementation
//

// Test to see if object contains error
bool Magick::LastErrorInst::isError( void ) const
{
  return ( _error   != MagickLib::UndefinedError ||
	   _warning != MagickLib::UndefinedWarning );
}

// Clear out existing error info
void Magick::LastErrorInst::clear ( void )
{
  _error     = MagickLib::UndefinedError;
  _warning   = MagickLib::UndefinedWarning;
  _syserror  = 0;
  _message.erase();
  _qualifier.erase();
}

// Error code
void Magick::LastErrorInst::error ( MagickLib::ErrorType error_ )
{
  _error = error_;
}
MagickLib::ErrorType Magick::LastErrorInst::error ( void ) const
{
  return _error;
}

// Warning code
void Magick::LastErrorInst::warning ( MagickLib::WarningType warning_ )
{
  _warning = warning_;
}
MagickLib::WarningType Magick::LastErrorInst::warning ( void ) const
{
  return _warning;
}

// System error
void Magick::LastErrorInst::syserror ( int syserror_ )
{
  _syserror = syserror_;
}
int Magick::LastErrorInst::syserror ( void ) const
{
  return _syserror;
}
  
// Error message
void Magick::LastErrorInst::message ( std::string message_ )
{
  _message = message_;
}
std::string Magick::LastErrorInst::message ( void ) const
{
  return _message;
}

// Error qualifier
void  Magick::LastErrorInst::qualifier ( std::string qualifier_ )
{
  _qualifier = qualifier_;
}
std::string Magick::LastErrorInst::qualifier ( void ) const
{
  return _qualifier;
}

// Throw exception corresponding to error (if any)
// Clears error info before throw.
void Magick::LastErrorInst::throwException( void )
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

// Default constructor
Magick::LastError::LastError( void )
{
    if ( _instance == 0 )
	{
	    _instance = new LastErrorInst;
	    ++_refCount;
	}
}

// Destructor
Magick::LastError::~LastError( void )
{
    if ( --_refCount == 0 && _doDelete )
	delete _instance;
}

// Assignment operator
Magick::LastError& Magick::LastError::operator=(const Magick::LastError& value_)
{
    // Nothing to assign.
    return *this;
}

// Copy constructor
Magick::LastError::LastError( const Magick::LastError& value_ )
{
    // Increase reference count
    ++value_._refCount;
}

// Indicate that destruction is desired
void Magick::LastError::doDelete()
{
    _doDelete = true;
    if ( _refCount == 0 )
	delete _instance;
}


//
// Magick++ error callback function
//
void Magick::LastErrorHandler( const MagickLib::ErrorType error_,
			       const char *message_ ,
			       const char *qualifier_)
{
    LastError err;
    err.error( error_ );
    err.syserror( errno );
    if ( message_ )
	err.message( message_ );
    else
	err.message( "" );
    if ( qualifier_ )
	err.qualifier( qualifier_ );
    else
	err.qualifier( "" );

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
    LastError err;
    err.warning( warning_ );
    err.syserror( errno );
    if ( message_ )
	err.message( message_ );
    else
	err.message( "" );
    if ( qualifier_ )
	err.qualifier( qualifier_ );
    else
	err.qualifier( "" );

    // Clear out system errno now that it has been collected.
    errno = 0;
}
