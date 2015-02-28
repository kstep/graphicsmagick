.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=================
Magick::Exception
=================

*Exception* represents the base class of objects thrown when
Magick++ reports an error. Magick++ throws C++ exceptions synchronous
with the operation where the error occurred. This allows errors to be
trapped within the enclosing code (perhaps the code to process a
single image) while allowing the code to be written with a simple
coding style.

A try/catch block should be placed around any sequence of operations
which can be considered an important body of work. For example, if
your program processes lists of images and some of these images may be
defective, by placing the try/catch block around the entire sequence
of code that processes one image (including instantiating the image
object), you can minimize the overhead of error checking while
ensuring that all objects created to deal with that object are safely
destroyed (C++ exceptions unroll the stack until the enclosing try
block, destroying any objects on the stack).

Note that any objects allocated via 'new' with a pointer on the stack
are not automatically destroyed by unrolling the stack via a C++
exception so that it may be necessary to catch the exception at each
level, destroy any objects allocated via 'new' (or malloc()) and then
re-throw the exception.  This includes constructors which might
encounter an exception while the object is being constructed and
should destroy any already-allocated data.  Magick++ classes are
designed to be **very** tiny so it is recommended to automatically
allocate them on the stack when possible rather than via 'new'.

The pseudo code for the main loop of your program may look like::

  using namespace std;
  for infile in list
  {
    try {
      // Construct an image instance first so that we don't have to worry
      // about object construction failure due to a minor warning exception
      // being thrown.
      Magick::Image image; 

      // Determine if Warning exceptions are thrown.
      // Use is optional.  Set to true to block Warning exceptions.
      image.quiet( false );

      try {
        // Try reading image file
        image.read(infile);
      }
      catch( Magick::WarningCoder &warning )
      {
        // Process coder warning while loading file (e.g. TIFF warning)
        // Maybe the user will be interested in these warnings (or not).
        // If a warning is produced while loading an image, the image
        // can normally still be used (but not if the warning was about
        // something important!)
        cerr << “Coder Warning: “ << warning.what() << endl;
      }
      catch( Magick::Warning &warning )
      {
        // Handle any other Magick++ warning.
        cerr << “Warning: “ << warning.what() << endl;
      }
      catch( Magick::ErrorFileOpen &error ) 
      { 
        // Process Magick++ file open error
        cerr << “Error: “ << error.what() << endl;
        continue; // Try next image.
      }
      try {
        image.rotate(90);
        image.write(“outfile”);
      }
      catch ( MagickExeption & error)
      {
         // Handle problem while rotating or writing outfile.
         cerr << “Caught Magick++ exception: “ << error.what() << endl;
      }
    }
    catch( std::exception &error ) 
    { 
       // Process any other exceptions derived from standard C++ exception
       cerr << “Caught C++ STD exception: “ << error.what() << endl;
    } 
    catch( ... ) 
    { 
      // Process *any* exception (last-ditch effort). There is not a lot
      // you can do here other to retry the operation that failed, or exit
      // the program. 
    }
  }

 The desired location and number of try/catch blocks in your program
 depends how sophisticated its error handling must be. Very simple
 programs may use just one try/catch block.

The *Exception* class is derived from the C++ standard
*std::exception* class. This means that it contains a C++ string
containing additional information about the error (e.g to display to
the user). Obtain access to this string via the what() method.  For
example::

  catch( Exception &error_ )
  {
    cout << "Caught exception: " << error_.what() << endl;
  }

The classes *Warning* and *Error* derive from the *Exception*
class. Exceptions derived from *Warning* are thrown to represent
non-fatal errors which may effect the completeness or quality of the
result (e.g. one image provided as an argument to montage is
defective). In most cases, a *Warning* exception may be ignored by
catching it immediately, processing it (e.g. printing a diagnostic)
and continuing on. Exceptions derived from *Error* are thrown to
represent fatal errors that can not produce a valid result
(e.g. attempting to read a file which does not exist).

The specific derived exception classes are shown in the following tables: 

.. table:: Warning (Suspect but completed) Sub-Classes

   =========================  ==============================================
   Warning                    Warning Description
   =========================  ==============================================
   WarningUndefined           Unspecified type.
   WarningBlob                NOT CURRENTLY USED
   WarningCache               NOT CURRENTLY USED
   WarningCoder               Issued by some coders.
   WarningConfigure           NOT CURRENTLY USED
   WarningCorruptImage        Issued when an image may be corrupt.
   WarningDelegate            Reported by a subordinate program.
   WarningDraw                Reported by the rendering subsystem.
   WarningFileOpen            Reported when file could not be opened.
   WarningImage               NOT CURRENTLY USED
   WarningMissingDelegate     NOT CURRENTLY USED
   WarningModule              NOT CURRENTLY USED
   WarningMonitor             NOT CURRENTLY USED
   WarningOption              Reported when an option is incorrect.
   WarningRegistry            NOT CURRENTLY USED
   WarningResourceLimit       Reported when a resource is exhausted.
   WarningStream              NOT CURRENTLY USED
   WarningType                NOT CURRENTLY USED
   WarningXServer             Warnings reported by the X11 subsystem.
   =========================  ==============================================

.. table:: Error (Failed) Sub-Classes

   =========================  ==============================================
   Error                      Error Description
   =========================  ==============================================
   ErrorUndefined             Unspecified error type.
   ErrorBlob                  Reported by BLOB I/O subsystem.
   ErrorCache                 Reported by the pixel cache subsystem.
   ErrorCoder                 Reported by coders (image format support).
   ErrorConfigure             Reported while loading configuration files.
   ErrorCorruptImage          Reported when the image file is corrupt.
   ErrorDelegate              Reported by a subordinate program
   ErrorDraw                  Reported while drawing on image.
   ErrorFileOpen              Reported when the image file can not be opened.
   ErrorImage                 Reported while drawing.
   ErrorMissingDelegate       Reported when optional add-on library or
                              subordinate program is missing (but is needed).
   ErrorModule                Reported by the module loader subsystem.
   ErrorMonitor               Reported by the progress monitor.
   ErrorOption                Reported when option is malformed or out of range.
   ErrorRegistry              Reported by the image/BLOB registry subsystem.
   ErrorResourceLimit         Reported when a program resource is exhausted.
   ErrorStream                Reported by the pixel stream subsystem.
   ErrorType                  Reported by the type (font) rendering subsystem.
   ErrorXServer               Reported by the X11 subsystem.
   =========================  ==============================================

Note that *ErrorMissingDelegate* is a "catch-all" error reported when
GraphicsMagick is unable to figure out how to open the file.

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| Bob Friesenhahn 1999 - 2015

