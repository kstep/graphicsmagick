// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Implementation of thread support
//

#define MAGICK_IMPLEMENTATION

#include "Magick++/Exception.h"
#include "Magick++/Thread.h"

// Default constructor
Magick::MutexLock::MutexLock(void)
{
#if defined(HasPTHREADS)
  ::pthread_mutexattr_t attr;
  int sysError = 0;
  if ( (sysError = ::pthread_mutexattr_init( &attr )) == 0 )
    if ( (sysError = ::pthread_mutexattr_setpshared( &attr, PTHREAD_PROCESS_PRIVATE )) == 0 )
      if ( (sysError = ::pthread_mutex_init( &_mutex, &attr )) == 0 )
	return;
  throw Magick::ErrorOption( "mutex initialization failed" );
#endif
}

// Destructor
Magick::MutexLock::~MutexLock(void)
{
#if defined(HasPTHREADS)
  if ( ::pthread_mutex_destroy( &_mutex ) == 0 )
    return;
  throw Magick::ErrorOption( "mutex destruction failed" );
#endif
}

// Lock mutex
void Magick::MutexLock::lock(void)
{
#if defined(HasPTHREADS)
  if ( ::pthread_mutex_lock( &_mutex ) == 0)
    return;
  throw Magick::ErrorOption( "mutex lock failed" );
#endif
}

// Unlock mutex
void Magick::MutexLock::unlock(void)
{
#if defined(HasPTHREADS)
  if ( ::pthread_mutex_unlock( &_mutex ) == 0)
    return;
  throw Magick::ErrorOption( "mutex unlock failed" );
#endif
}
