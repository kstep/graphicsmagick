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

    // Format and throw exception
    void throwException( LastErrorBase &lastError_ );

    //
    // Singleton class for single-threaded use
    //
    class LastErrorST : public LastErrorBase
    {
	// Accessed only via LastError
	friend class LastError;

	private:

	LastErrorST( void );
	~LastErrorST( void );

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
	LastErrorST ( const LastErrorST& original_ );
    
	// Don't support assignment
	LastErrorST& operator = ( const LastErrorST& original_ );
    
	MagickLib::ErrorType   _error;
	MagickLib::WarningType _warning;
	int                    _syserror;
	std::string            _message;
	std::string            _qualifier;
    };

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
Magick::LastErrorBase* Magick::LastError::_instance = 0;     // LastErrorST pointer
unsigned               Magick::LastError::_refCount = 0;     // Reference counter
bool                   Magick::LastError::_doDelete = false; // Object is to be deleted

// Allocate static guard class in this translation unit
// Static guard class is destroyed when program exits.
static Magick::LastErrorGuard LastErrorGuard_inst;

// Constructor
Magick::LastErrorST::LastErrorST( void )
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
Magick::LastErrorST::~LastErrorST( void )
{
  // Nothing to do
}

//
// LastErrorST (Single Thread) implementation
//

// Test to see if object contains error
bool Magick::LastErrorST::isError( void ) const
{
  return ( _error   != MagickLib::UndefinedError ||
	   _warning != MagickLib::UndefinedWarning );
}

// Clear out existing error info
void Magick::LastErrorST::clear ( void )
{
  _error     = MagickLib::UndefinedError;
  _warning   = MagickLib::UndefinedWarning;
  _syserror  = 0;
  _message.erase();
  _qualifier.erase();
}

// Error code
void Magick::LastErrorST::error ( MagickLib::ErrorType error_ )
{
  _error = error_;
}
MagickLib::ErrorType Magick::LastErrorST::error ( void ) const
{
  return _error;
}

// Warning code
void Magick::LastErrorST::warning ( MagickLib::WarningType warning_ )
{
  _warning = warning_;
}
MagickLib::WarningType Magick::LastErrorST::warning ( void ) const
{
  return _warning;
}

// System error
void Magick::LastErrorST::syserror ( int syserror_ )
{
  _syserror = syserror_;
}
int Magick::LastErrorST::syserror ( void ) const
{
  return _syserror;
}
  
// Error message
void Magick::LastErrorST::message ( std::string message_ )
{
  _message = message_;
}
std::string Magick::LastErrorST::message ( void ) const
{
  return _message;
}

// Error qualifier
void  Magick::LastErrorST::qualifier ( std::string qualifier_ )
{
  _qualifier = qualifier_;
}
std::string Magick::LastErrorST::qualifier ( void ) const
{
  return _qualifier;
}

// Throw exception corresponding to error (if any)
// Clears error info before throw.
void Magick::LastErrorST::throwException( void )
{
  if ( !isError() )
    return;

  Magick::throwException( *this );
}

//
// LastError (interface class) implementation
//

// Default constructor
Magick::LastError::LastError( void )
{
    if ( _instance == 0 )
	{
	    _instance = new LastErrorST;
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

// Format and throw exception
void Magick::throwException( Magick::LastErrorBase &lastError_ )
{
  // Format error message ImageMagick-style
  std::string message = MagickLib::SetClientName((char *) NULL);
  if ( lastError_.message().length() > 0 )
    message += std::string(": ");

  if ( lastError_.message().length() > 0 )
    message += lastError_.message();

  if ( lastError_.qualifier().length() > 0 )
    message += " (" + lastError_.qualifier() + ")";

  if ( lastError_.syserror() )
    message += std::string(" [") + strerror(lastError_.syserror()) + std::string("]");

  if ( lastError_.warning() != MagickLib::UndefinedWarning )
    {
      MagickLib::WarningType warningVal = lastError_.warning();
      lastError_.clear();
      
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
  else if ( lastError_.error() != MagickLib::UndefinedError )
    {
      MagickLib::ErrorType errorVal = lastError_.error();
      lastError_.clear();
      
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
