// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Definition of Magick::Exception and derived classes
// Magick::Warning* and Magick::Error*.  Derived from C++ STD
// 'exception' class for convenience.
//
// These classes form part of the Magick++ user interface.
//

#if !defined(Magick_Exception_header)
#define Magick_Exception_header

#include <string>
#include <exception>

#include "Magick++/Include.h"

namespace Magick
{
  class Exception : public std::exception
  {
  public:
    Exception( const std::string& what_ );
    Exception( const Exception& original_ );
    Exception& operator= (const Exception& original_ );
    virtual const char* what () const throw();
    virtual ~Exception ( ) throw () { };

  private:
    std::string _what;
  };

  //
  // Warnings
  //

  class Warning : public Exception
  {
  public:
    explicit Warning ( const std::string& what_ );
  };
  
  class WarningUndefined : public Warning
  {
  public:
    explicit WarningUndefined ( const std::string& what_ );
  };
  
  class WarningResourceLimit : public Warning
  {
  public:
    explicit WarningResourceLimit ( const std::string& what_ );
  };
  
  class WarningXServer : public Warning
  {
  public:
   explicit WarningXServer ( const std::string& what_ );
  };
  
  class WarningOption : public Warning
  {
  public:
    explicit WarningOption ( const std::string& what_ );
  };
  
  class WarningDelegate : public Warning
  {
  public:
    explicit WarningDelegate ( const std::string& what_ );
  };
  
  class WarningMissingDelegate : public Warning
  {
  public:
    explicit WarningMissingDelegate ( const std::string& what_ );
  };
  
  class WarningCorruptImage: public Warning
  {
  public:
    explicit WarningCorruptImage ( const std::string& what_ );
  };
  
  class WarningFileOpen: public Warning
  {
  public:
    explicit WarningFileOpen ( const std::string& what_ );
  };

  class WarningBlob: public Warning
  {
  public:
    explicit WarningBlob ( const std::string& what_ );
  };

  class WarningCache: public Warning
  {
  public:
    explicit WarningCache ( const std::string& what_ );
  };

  //
  // Error exceptions
  //

  class Error : public Exception
  {
  public:
    explicit Error ( const std::string& what_ );
  };

  class ErrorUndefined : public Error
  {
  public:
    explicit ErrorUndefined ( const std::string& what_ );
  };
  
  class ErrorResourceLimit : public Error
  {
  public:
    explicit ErrorResourceLimit ( const std::string& what_ );
  };
  
  class ErrorXServer : public Error
  {
  public:
    explicit ErrorXServer ( const std::string& what_ );
  };
  
  class ErrorOption : public Error
  {
  public:
    explicit ErrorOption ( const std::string& what_ );
  };
  
  class ErrorDelegate : public Error
  {
  public:
    explicit ErrorDelegate ( const std::string& what_ );
  };
  
  class ErrorMissingDelegate : public Error
  {
  public:
    explicit ErrorMissingDelegate ( const std::string& what_ );
  };
  
  class ErrorCorruptImage: public Error
  {
  public:
    explicit ErrorCorruptImage ( const std::string& what_ );
  };
  
  class ErrorFileOpen: public Error
  {
  public:
    explicit ErrorFileOpen ( const std::string& what_ );
  };

  class ErrorBlob: public Error
  {
  public:
    explicit ErrorBlob ( const std::string& what_ );
  };

  class ErrorCache: public Error
  {
  public:
    explicit ErrorCache ( const std::string& what_ );
  };

  //
  // No user-serviceable components beyond this point.
  //

  // Throw exception based on raw data
  void throwExceptionExplicit( const MagickLib::ExceptionType severity_,
			       const char* reason_,
			       const char* description_ = 0 );

  // Thow exception based on ImageMagick's ExceptionInfo
  void throwException( MagickLib::ExceptionInfo &exception_ );

} // namespace Magick


// Print exception to stream.
std::ostream& operator<<( std::ostream& stream_,
			  const Magick::Exception& exception_);

#endif // Magick_Exception_header
