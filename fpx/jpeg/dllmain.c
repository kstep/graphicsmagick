/*
Description:  Main module for dllJPEG libraries.
*/ 


#ifndef __unix
#include <windows.h>
#include "jpegconf.h"

int FAR PASCAL LibMain(
HANDLE hInstance,
WORD   wDataSeg,
WORD   cbHeapSize,
LPSTR  lpszCmdLine)
{
//    if (cbHeapSize != 0) UnlockData(0);
    return(1);
};

int FAR PASCAL WEP(int nParameter)
{
    return(1);
};
#endif //__unix
