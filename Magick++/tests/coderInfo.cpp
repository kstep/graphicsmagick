// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 2001
//
// Test Magick::CoderInfo class and Magick::coderInfoList
//

#include <string>
#include <iostream>
#include <list>

#include <Magick++.h>

using namespace std;

using namespace Magick;

int test( MatchType isReadable_, MatchType isWritable_, MatchType isMultiFrame_ )
{
  int result = 0;
  list<CoderInfo> coderList;
  coderInfoList( &coderList, isReadable_, isWritable_, isMultiFrame_ );
  list<CoderInfo>::iterator entry = coderList.begin();
  while( entry != coderList.end() )
    {
      // Readable
      if ( isReadable_ != AnyMatch &&
           (( entry->isReadable() && isReadable_ != TrueMatch ) ||
            ( !entry->isReadable() && isReadable_ != FalseMatch )) )
        {
          cout << "Entry \""
               << entry->name()
               << "\" has unexpected readablity state ("
               << static_cast<int>(entry->isReadable())
               << ")"
               << endl;
          ++result;
        }

      // Writable
      if ( isWritable_ != AnyMatch &&
           (( entry->isWritable() && isWritable_ != TrueMatch ) ||
            ( !entry->isWritable() && isWritable_ != FalseMatch )) )
        {
          cout << "Entry \""
               << entry->name()
               << "\" has unexpected writablity state ("
               << static_cast<int>(entry->isWritable())
               << ")"
               << endl;
          ++result;
        }

      // MultiFrame
      if ( isMultiFrame_ != AnyMatch &&
           (( entry->isMultiFrame() && isMultiFrame_ != TrueMatch ) ||
            ( !entry->isMultiFrame() && isMultiFrame_ != FalseMatch )) )
        {
          cout << "Entry \""
               << entry->name()
               << "\" has unexpected multiframe state ("
               << static_cast<int>(entry->isMultiFrame())
               << ")"
               << endl;
          ++result;
        }

      entry++;
    }

  return result;
}

int main( int /*argc*/, char **argv)
{

  // Initialize ImageMagick install location for Windows
  InitializeMagick(*argv);

  int failures=0;

  try {

    CoderInfo coderInfo("GIF");
    if( coderInfo.name() != string("GIF") )
      {
        cout << "Unexpected coder name \""
             << coderInfo.name()
             << "\""
             << endl;
        ++failures;
      }

    if( coderInfo.description() != string("CompuServe graphics interchange format") )
      {
        cout << "Unexpected coder description \""
             << coderInfo.description()
             << "\""
             << endl;
        ++failures;
      }

    failures += test(AnyMatch,AnyMatch,AnyMatch);
    failures += test(FalseMatch,FalseMatch,FalseMatch);

    failures += test(TrueMatch,AnyMatch,AnyMatch);
    failures += test(FalseMatch,AnyMatch,AnyMatch);

    failures += test(AnyMatch,TrueMatch,AnyMatch);
    failures += test(AnyMatch,FalseMatch,AnyMatch);

    failures += test(AnyMatch,AnyMatch,TrueMatch);
    failures += test(AnyMatch,AnyMatch,FalseMatch);
  }
  catch( Exception &error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }
  catch( exception &error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }
  
  if ( failures )
    {
      cout << failures << " failures" << endl;
      return 1;
    }

  return 0;
}
