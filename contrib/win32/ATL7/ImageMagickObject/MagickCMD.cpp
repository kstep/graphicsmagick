#include <iostream>
#include <atlbase.h>
#include <atlsafe.h>
#import "ImageMagickObject.tlb" no_namespace
using namespace std;

typedef enum
{
  cmdUnknown,
  cmdConvert,
  cmdComposite,
  cmdIdentify,
  cmdMogrify,
  cmdMontage
} CommandType;

static struct
  Commands
  {
    char
      *name;

    CommandType
      code;
  } Commands[] =
  {
    { "",          cmdUnknown   },
    { "convert",   cmdConvert   },
    { "composite", cmdComposite },
    { "identify",  cmdIdentify  },
    { "mogrify",   cmdMogrify   },
    { "montage",   cmdMontage   }
  };

int main(int argc, char* argv[])
{
  int
    index,
    status = 0;

  char
    *name;

  CommandType
    code = cmdUnknown;

  // We must have at least a command, input, and output
  if (argc < 4)
    return 0;

  index = 1;
  while ((name = Commands[index].name))
  {
    if (stricmp(name,argv[1]) == 0)
    {
      code = Commands[index].code;
      break;
    }
    index++;
  }
  if (code == cmdUnknown)
    return 0;

  CoInitialize(NULL);

  try
  {
    CComVariant
      result;

    SAFEARRAY
      **ppsa = (SAFEARRAY**) NULL;

    IMagickImagePtr pImageProc(__uuidof(MagickImage));
    if (pImageProc == NULL)
      status = 1;
    else
    {
      {
        // Define the array bound structure
        CComSafeArrayBound bound[1];
        bound[0].SetCount(argc-2);
        bound[0].SetLowerBound(0);
        CComSafeArray<VARIANT> args(bound);
        if( !args )
          status = 2;
        else
        {
          for(index = 2; index < argc; ++index)
          {
            CComVariant vt(argv[index]);
            HRESULT hr = vt.Detach(&args[index-2]);
          }
          switch(code)
          {
            case cmdConvert:
              result = pImageProc->Convert(args.GetSafeArrayPtr());
              break;
            case cmdComposite:
              result = pImageProc->Composite(args.GetSafeArrayPtr());
              break;
            case cmdIdentify:
              result = pImageProc->Identify(args.GetSafeArrayPtr());
              break;
            case cmdMogrify:
              result = pImageProc->Mogrify(args.GetSafeArrayPtr());
              break;
            case cmdMontage:
              result = pImageProc->Montage(args.GetSafeArrayPtr());
              break;
          }
          pImageProc.Release();
        }
      }
    }
  }
  catch(_com_error ex)
  {
    HRESULT
      res = ex.Error();  

    _bstr_t
      desc = ex.Description();  

    printf("Error %s (0x%08x)\n",(char *)desc,res);
    status = 4;
  }

  CoUninitialize();
  return status;
}

