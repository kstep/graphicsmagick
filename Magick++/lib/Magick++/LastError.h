// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Definition of LastError (ImageMagick error handler)
//
// This class serves to collect the last error/warning
// reported by the ImageMagick error callback.
//
// This class is a Magick++ implementation class and is not intended
// for use by end-users.
// 
#if !defined (LastError_header)
#define LastError_header

#include <iostream>

namespace Magick
{
    // Forward declaration
    class LastError;

    //
    // The actual singleton class
    //
    class LastErrorInst
    {
	// Accessed only via LastError
	friend class LastError;

	private:

	LastErrorInst( void );
	~LastErrorInst( void );

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
	LastErrorInst ( const LastErrorInst& original_ );
    
	// Don't support assignment
	LastErrorInst& operator = ( const LastErrorInst& original_ );
    
	MagickLib::ErrorType   _error;
	MagickLib::WarningType _warning;
	int                    _syserror;
	std::string            _message;
	std::string            _qualifier;
    };


    //
    // Interface class to access singleton.
    //
    class LastError
    {
	friend class LastErrorGuard;
	public:
	// Default constructor
	LastError( void );

	// Destructor
	~LastError( void );

	// Assignment operator
	LastError& LastError::operator=(const LastError& value_);

	// Copy constructor
	LastError( const LastError& value_ );

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

	// Indicate that destruction is desired
	void doDelete();

	static LastErrorInst* _instance;  // Singleton reference
	static unsigned       _refCount;  // References
	static bool           _doDelete;  // Deletion desired
    };
}


//
// Pass-through methods to singleton implementation
//

// Test to see if error or warning is set
inline bool Magick::LastError::isError( void ) const
{
    return _instance->isError();
}
    
// Clear out existing error info
inline void Magick::LastError::clear ( void )
{
    _instance->clear();
}
    
// Error code
inline void Magick::LastError::error ( MagickLib::ErrorType error_ )
{
    _instance->error(error_);
}
inline MagickLib::ErrorType Magick::LastError::error ( void ) const 
{
    return _instance->error();
}

// Warning code
inline void Magick::LastError::warning ( MagickLib::WarningType warning_ )
{
    _instance->warning( warning_ );
}
inline MagickLib::WarningType Magick::LastError::warning ( void ) const
{
    return _instance->warning();
}
    
// System errno
inline void Magick::LastError::syserror ( int syserror_ )
{
    _instance->syserror( syserror_ );
}
inline int Magick::LastError::syserror ( void ) const
{
    return _instance->syserror();
}
    
// Error message
inline void Magick::LastError::message ( std::string message_ )
{
    _instance->message( message_ );
}
inline std::string Magick::LastError::message ( void ) const
{
    return _instance->message();
}
    
// Error qualifier
inline void Magick::LastError::qualifier ( std::string qualifier_ )
{
    _instance->qualifier( qualifier_ );
}
inline std::string Magick::LastError::qualifier ( void ) const
{
    return _instance->qualifier();
}
    
// Throw exception corresponding to error (if any)
inline void Magick::LastError::throwException( void )
{
    _instance->throwException();
}

#endif // LastError_header
