/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sun Jun 23 20:33:38 2002
 */
/* Compiler settings for D:\CVSCurrent\ImageMagick\contrib\win32\ATL\ImageMagickObject\ImageMagickObject.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IMagickImage = {0x7F670536,0x00AE,0x4EDF,{0xB0,0x6F,0x13,0xBD,0x22,0xB2,0x56,0x24}};


const IID LIBID_IMAGEMAGICKOBJECTLib = {0xA2A30AD0,0x24E7,0x450F,{0x8B,0x51,0xF1,0x54,0xBC,0x0C,0xD8,0x3A}};


const IID DIID__IMagickImageEvents = {0x01834743,0xE151,0x45C9,{0x9C,0x43,0x2F,0xC8,0x01,0x14,0xE5,0x39}};


const CLSID CLSID_MagickImage = {0x5630BE5A,0x3F5F,0x4BCA,{0xA5,0x11,0xAD,0x6A,0x63,0x86,0xCA,0xC1}};


#ifdef __cplusplus
}
#endif

