/* this ALWAYS GENERATED file contains the proxy stub code */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sun Jun 23 20:33:38 2002
 */
/* Compiler settings for D:\CVSCurrent\ImageMagick\contrib\win32\ATL\ImageMagickObject\ImageMagickObject.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "ImageMagickObject.h"

#define TYPE_FORMAT_STRING_SIZE   1043                              
#define PROC_FORMAT_STRING_SIZE   401                               

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMagickImage, ver. 0.0,
   GUID={0x7F670536,0x00AE,0x4EDF,{0xB0,0x6F,0x13,0xBD,0x22,0xB2,0x56,0x24}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMagickImage_ServerInfo;

#pragma code_seg(".orpc")
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[2];

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x50100a4, /* MIDL Version 5.1.164 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    1,  /* Flags */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

static const unsigned short IMagickImage_FormatStringOffsetTable[] = 
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    50,
    78,
    112,
    140,
    174,
    208,
    242,
    276,
    310,
    338,
    372
    };

static const MIDL_SERVER_INFO IMagickImage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IMagickImage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IMagickImage_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IMagickImage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(20) _IMagickImageProxyVtbl = 
{
    &IMagickImage_ProxyInfo,
    &IID_IMagickImage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *)-1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *)-1 /* IMagickImage::OnStartPage */ ,
    (void *)-1 /* IMagickImage::OnEndPage */ ,
    (void *)-1 /* IMagickImage::get_Count */ ,
    (void *)-1 /* IMagickImage::Add */ ,
    (void *)-1 /* IMagickImage::Remove */ ,
    (void *)-1 /* IMagickImage::Convert */ ,
    (void *)-1 /* IMagickImage::Composite */ ,
    (void *)-1 /* IMagickImage::Montage */ ,
    (void *)-1 /* IMagickImage::Mogrify */ ,
    (void *)-1 /* IMagickImage::Identify */ ,
    (void *)-1 /* IMagickImage::get__NewEnum */ ,
    (void *)-1 /* IMagickImage::get_Item */ ,
    (void *)-1 /* IMagickImage::get_Messages */
};


static const PRPC_STUB_FUNCTION IMagickImage_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IMagickImageStubVtbl =
{
    &IID_IMagickImage,
    &IMagickImage_ServerInfo,
    20,
    &IMagickImage_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

#pragma data_seg(".rdata")

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[2] = 
        {
            
            {
            LPSAFEARRAY_UserSize
            ,LPSAFEARRAY_UserMarshal
            ,LPSAFEARRAY_UserUnmarshal
            ,LPSAFEARRAY_UserFree
            },
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            }

        };


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute, more than 32 methods in the interface.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure OnStartPage */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/*  8 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter piUnk */

/* 16 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
#ifndef _ALPHA_
/* 18 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 20 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 22 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 24 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 26 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnEndPage */

/* 28 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 30 */	NdrFcLong( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x8 ),	/* 8 */
#ifndef _ALPHA_
/* 36 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 38 */	NdrFcShort( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x8 ),	/* 8 */
/* 42 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 46 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Count */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x9 ),	/* 9 */
#ifndef _ALPHA_
/* 58 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x10 ),	/* 16 */
/* 64 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 66 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 68 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 70 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 72 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 74 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Add */

/* 78 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 80 */	NdrFcLong( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0xa ),	/* 10 */
#ifndef _ALPHA_
/* 86 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 88 */	NdrFcShort( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0x8 ),	/* 8 */
/* 92 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pArrayVar */

/* 94 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
#ifndef _ALPHA_
/* 96 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 98 */	NdrFcShort( 0x3c4 ),	/* Type Offset=964 */

	/* Parameter pVar2 */

/* 100 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
#ifndef _ALPHA_
/* 102 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 104 */	NdrFcShort( 0x3d6 ),	/* Type Offset=982 */

	/* Return value */

/* 106 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 108 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Remove */

/* 112 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 114 */	NdrFcLong( 0x0 ),	/* 0 */
/* 118 */	NdrFcShort( 0xb ),	/* 11 */
#ifndef _ALPHA_
#if !defined(_MIPS_) && !defined(_PPC_)
/* 120 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
#else
			NdrFcShort( 0x1c ),	/* MIPS & PPC Stack size/offset = 28 */
#endif
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0x8 ),	/* 8 */
/* 126 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter varIndex */

/* 128 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
#ifndef _ALPHA_
#if !defined(_MIPS_) && !defined(_PPC_)
/* 130 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* MIPS & PPC Stack size/offset = 8 */
#endif
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 132 */	NdrFcShort( 0x3e4 ),	/* Type Offset=996 */

	/* Return value */

/* 134 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
#if !defined(_MIPS_) && !defined(_PPC_)
/* 136 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
#else
			NdrFcShort( 0x18 ),	/* MIPS & PPC Stack size/offset = 24 */
#endif
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 138 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Convert */

/* 140 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 142 */	NdrFcLong( 0x0 ),	/* 0 */
/* 146 */	NdrFcShort( 0xc ),	/* 12 */
#ifndef _ALPHA_
/* 148 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 150 */	NdrFcShort( 0x0 ),	/* 0 */
/* 152 */	NdrFcShort( 0x8 ),	/* 8 */
/* 154 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pArrayVar */

/* 156 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
#ifndef _ALPHA_
/* 158 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 160 */	NdrFcShort( 0x3f2 ),	/* Type Offset=1010 */

	/* Parameter pVar2 */

/* 162 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
#ifndef _ALPHA_
/* 164 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 166 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Return value */

/* 168 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 170 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Composite */

/* 174 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 176 */	NdrFcLong( 0x0 ),	/* 0 */
/* 180 */	NdrFcShort( 0xd ),	/* 13 */
#ifndef _ALPHA_
/* 182 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0x8 ),	/* 8 */
/* 188 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pArrayVar */

/* 190 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
#ifndef _ALPHA_
/* 192 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 194 */	NdrFcShort( 0x3f2 ),	/* Type Offset=1010 */

	/* Parameter pVar2 */

/* 196 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
#ifndef _ALPHA_
/* 198 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 200 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Return value */

/* 202 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 204 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Montage */

/* 208 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 210 */	NdrFcLong( 0x0 ),	/* 0 */
/* 214 */	NdrFcShort( 0xe ),	/* 14 */
#ifndef _ALPHA_
/* 216 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x8 ),	/* 8 */
/* 222 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pArrayVar */

/* 224 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
#ifndef _ALPHA_
/* 226 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 228 */	NdrFcShort( 0x3f2 ),	/* Type Offset=1010 */

	/* Parameter pVar2 */

/* 230 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
#ifndef _ALPHA_
/* 232 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 234 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Return value */

/* 236 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 238 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Mogrify */

/* 242 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 244 */	NdrFcLong( 0x0 ),	/* 0 */
/* 248 */	NdrFcShort( 0xf ),	/* 15 */
#ifndef _ALPHA_
/* 250 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 254 */	NdrFcShort( 0x8 ),	/* 8 */
/* 256 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pArrayVar */

/* 258 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
#ifndef _ALPHA_
/* 260 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 262 */	NdrFcShort( 0x3f2 ),	/* Type Offset=1010 */

	/* Parameter pVar2 */

/* 264 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
#ifndef _ALPHA_
/* 266 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 268 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Return value */

/* 270 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 272 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 274 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Identify */

/* 276 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 278 */	NdrFcLong( 0x0 ),	/* 0 */
/* 282 */	NdrFcShort( 0x10 ),	/* 16 */
#ifndef _ALPHA_
/* 284 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 288 */	NdrFcShort( 0x8 ),	/* 8 */
/* 290 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pArrayVar */

/* 292 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
#ifndef _ALPHA_
/* 294 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 296 */	NdrFcShort( 0x3f2 ),	/* Type Offset=1010 */

	/* Parameter pVar2 */

/* 298 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
#ifndef _ALPHA_
/* 300 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 302 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Return value */

/* 304 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 306 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 308 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get__NewEnum */

/* 310 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 312 */	NdrFcLong( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x11 ),	/* 17 */
#ifndef _ALPHA_
/* 318 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0x8 ),	/* 8 */
/* 324 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 326 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 328 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 330 */	NdrFcShort( 0x40e ),	/* Type Offset=1038 */

	/* Return value */

/* 332 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 334 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 336 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Item */

/* 338 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 340 */	NdrFcLong( 0x0 ),	/* 0 */
/* 344 */	NdrFcShort( 0x12 ),	/* 18 */
#ifndef _ALPHA_
#if !defined(_MIPS_) && !defined(_PPC_)
/* 346 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
#else
			NdrFcShort( 0x20 ),	/* MIPS & PPC Stack size/offset = 32 */
#endif
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 348 */	NdrFcShort( 0x0 ),	/* 0 */
/* 350 */	NdrFcShort( 0x8 ),	/* 8 */
/* 352 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter varIndex */

/* 354 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
#ifndef _ALPHA_
#if !defined(_MIPS_) && !defined(_PPC_)
/* 356 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* MIPS & PPC Stack size/offset = 8 */
#endif
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 358 */	NdrFcShort( 0x3e4 ),	/* Type Offset=996 */

	/* Parameter pVal */

/* 360 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
#ifndef _ALPHA_
#if !defined(_MIPS_) && !defined(_PPC_)
/* 362 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
#else
			NdrFcShort( 0x18 ),	/* MIPS & PPC Stack size/offset = 24 */
#endif
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 364 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Return value */

/* 366 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
#if !defined(_MIPS_) && !defined(_PPC_)
/* 368 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
#else
			NdrFcShort( 0x1c ),	/* MIPS & PPC Stack size/offset = 28 */
#endif
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 370 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Messages */

/* 372 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 374 */	NdrFcLong( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0x13 ),	/* 19 */
#ifndef _ALPHA_
/* 380 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x8 ),	/* 8 */
/* 386 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 388 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
#ifndef _ALPHA_
/* 390 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 392 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Return value */

/* 394 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 396 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 398 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/*  4 */	NdrFcLong( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x0 ),	/* 0 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 14 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 16 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 18 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 20 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 22 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 24 */	
			0x11, 0x0,	/* FC_RP */
/* 26 */	NdrFcShort( 0x3aa ),	/* Offset= 938 (964) */
/* 28 */	
			0x13, 0x10,	/* FC_OP */
/* 30 */	NdrFcShort( 0x2 ),	/* Offset= 2 (32) */
/* 32 */	
			0x13, 0x0,	/* FC_OP */
/* 34 */	NdrFcShort( 0x390 ),	/* Offset= 912 (946) */
/* 36 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 38 */	NdrFcShort( 0x18 ),	/* 24 */
/* 40 */	NdrFcShort( 0xa ),	/* 10 */
/* 42 */	NdrFcLong( 0x8 ),	/* 8 */
/* 46 */	NdrFcShort( 0x6c ),	/* Offset= 108 (154) */
/* 48 */	NdrFcLong( 0xd ),	/* 13 */
/* 52 */	NdrFcShort( 0x8c ),	/* Offset= 140 (192) */
/* 54 */	NdrFcLong( 0x9 ),	/* 9 */
/* 58 */	NdrFcShort( 0xba ),	/* Offset= 186 (244) */
/* 60 */	NdrFcLong( 0xc ),	/* 12 */
/* 64 */	NdrFcShort( 0x280 ),	/* Offset= 640 (704) */
/* 66 */	NdrFcLong( 0x24 ),	/* 36 */
/* 70 */	NdrFcShort( 0x2a8 ),	/* Offset= 680 (750) */
/* 72 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 76 */	NdrFcShort( 0x2c4 ),	/* Offset= 708 (784) */
/* 78 */	NdrFcLong( 0x10 ),	/* 16 */
/* 82 */	NdrFcShort( 0x2dc ),	/* Offset= 732 (814) */
/* 84 */	NdrFcLong( 0x2 ),	/* 2 */
/* 88 */	NdrFcShort( 0x2f4 ),	/* Offset= 756 (844) */
/* 90 */	NdrFcLong( 0x3 ),	/* 3 */
/* 94 */	NdrFcShort( 0x30c ),	/* Offset= 780 (874) */
/* 96 */	NdrFcLong( 0x14 ),	/* 20 */
/* 100 */	NdrFcShort( 0x324 ),	/* Offset= 804 (904) */
/* 102 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (101) */
/* 104 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 106 */	NdrFcShort( 0x2 ),	/* 2 */
/* 108 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 110 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 112 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 114 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 116 */	NdrFcShort( 0x8 ),	/* 8 */
/* 118 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (104) */
/* 120 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 122 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 124 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 126 */	NdrFcShort( 0x4 ),	/* 4 */
/* 128 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 132 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 134 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 136 */	NdrFcShort( 0x4 ),	/* 4 */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 140 */	NdrFcShort( 0x1 ),	/* 1 */
/* 142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 146 */	0x13, 0x0,	/* FC_OP */
/* 148 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (114) */
/* 150 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 152 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 154 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 156 */	NdrFcShort( 0x8 ),	/* 8 */
/* 158 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 160 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 162 */	NdrFcShort( 0x4 ),	/* 4 */
/* 164 */	NdrFcShort( 0x4 ),	/* 4 */
/* 166 */	0x11, 0x0,	/* FC_RP */
/* 168 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (124) */
/* 170 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 172 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 174 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 182 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 186 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 188 */	NdrFcShort( 0xffffff46 ),	/* Offset= -186 (2) */
/* 190 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 192 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 194 */	NdrFcShort( 0x8 ),	/* 8 */
/* 196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 198 */	NdrFcShort( 0x6 ),	/* Offset= 6 (204) */
/* 200 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 202 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 204 */	
			0x11, 0x0,	/* FC_RP */
/* 206 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (174) */
/* 208 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 210 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 218 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 220 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 222 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 224 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 226 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 230 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 234 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 238 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 240 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (208) */
/* 242 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 244 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 250 */	NdrFcShort( 0x6 ),	/* Offset= 6 (256) */
/* 252 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 254 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 256 */	
			0x11, 0x0,	/* FC_RP */
/* 258 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (226) */
/* 260 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 262 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 264 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 266 */	NdrFcShort( 0x2 ),	/* Offset= 2 (268) */
/* 268 */	NdrFcShort( 0x10 ),	/* 16 */
/* 270 */	NdrFcShort( 0x2b ),	/* 43 */
/* 272 */	NdrFcLong( 0x3 ),	/* 3 */
/* 276 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 278 */	NdrFcLong( 0x11 ),	/* 17 */
/* 282 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 284 */	NdrFcLong( 0x2 ),	/* 2 */
/* 288 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 290 */	NdrFcLong( 0x4 ),	/* 4 */
/* 294 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 296 */	NdrFcLong( 0x5 ),	/* 5 */
/* 300 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 302 */	NdrFcLong( 0xb ),	/* 11 */
/* 306 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 308 */	NdrFcLong( 0xa ),	/* 10 */
/* 312 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 314 */	NdrFcLong( 0x6 ),	/* 6 */
/* 318 */	NdrFcShort( 0xd6 ),	/* Offset= 214 (532) */
/* 320 */	NdrFcLong( 0x7 ),	/* 7 */
/* 324 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 326 */	NdrFcLong( 0x8 ),	/* 8 */
/* 330 */	NdrFcShort( 0xd0 ),	/* Offset= 208 (538) */
/* 332 */	NdrFcLong( 0xd ),	/* 13 */
/* 336 */	NdrFcShort( 0xfffffeb2 ),	/* Offset= -334 (2) */
/* 338 */	NdrFcLong( 0x9 ),	/* 9 */
/* 342 */	NdrFcShort( 0xffffff7a ),	/* Offset= -134 (208) */
/* 344 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 348 */	NdrFcShort( 0xc2 ),	/* Offset= 194 (542) */
/* 350 */	NdrFcLong( 0x24 ),	/* 36 */
/* 354 */	NdrFcShort( 0xc0 ),	/* Offset= 192 (546) */
/* 356 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 360 */	NdrFcShort( 0xba ),	/* Offset= 186 (546) */
/* 362 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 366 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (598) */
/* 368 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 372 */	NdrFcShort( 0xe6 ),	/* Offset= 230 (602) */
/* 374 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 378 */	NdrFcShort( 0xe4 ),	/* Offset= 228 (606) */
/* 380 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 384 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (610) */
/* 386 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 390 */	NdrFcShort( 0xe0 ),	/* Offset= 224 (614) */
/* 392 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 396 */	NdrFcShort( 0xce ),	/* Offset= 206 (602) */
/* 398 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 402 */	NdrFcShort( 0xcc ),	/* Offset= 204 (606) */
/* 404 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 408 */	NdrFcShort( 0xd2 ),	/* Offset= 210 (618) */
/* 410 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 414 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (614) */
/* 416 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 420 */	NdrFcShort( 0xca ),	/* Offset= 202 (622) */
/* 422 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 426 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (626) */
/* 428 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 432 */	NdrFcShort( 0xc6 ),	/* Offset= 198 (630) */
/* 434 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 438 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (634) */
/* 440 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 444 */	NdrFcShort( 0xbe ),	/* Offset= 190 (634) */
/* 446 */	NdrFcLong( 0x10 ),	/* 16 */
/* 450 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 452 */	NdrFcLong( 0x12 ),	/* 18 */
/* 456 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 458 */	NdrFcLong( 0x13 ),	/* 19 */
/* 462 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 464 */	NdrFcLong( 0x16 ),	/* 22 */
/* 468 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 470 */	NdrFcLong( 0x17 ),	/* 23 */
/* 474 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 476 */	NdrFcLong( 0xe ),	/* 14 */
/* 480 */	NdrFcShort( 0x9e ),	/* Offset= 158 (638) */
/* 482 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 486 */	NdrFcShort( 0xa4 ),	/* Offset= 164 (650) */
/* 488 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 492 */	NdrFcShort( 0x6a ),	/* Offset= 106 (598) */
/* 494 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 498 */	NdrFcShort( 0x68 ),	/* Offset= 104 (602) */
/* 500 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 504 */	NdrFcShort( 0x66 ),	/* Offset= 102 (606) */
/* 506 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 510 */	NdrFcShort( 0x60 ),	/* Offset= 96 (606) */
/* 512 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 516 */	NdrFcShort( 0x5a ),	/* Offset= 90 (606) */
/* 518 */	NdrFcLong( 0x0 ),	/* 0 */
/* 522 */	NdrFcShort( 0x0 ),	/* Offset= 0 (522) */
/* 524 */	NdrFcLong( 0x1 ),	/* 1 */
/* 528 */	NdrFcShort( 0x0 ),	/* Offset= 0 (528) */
/* 530 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (529) */
/* 532 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 534 */	NdrFcShort( 0x8 ),	/* 8 */
/* 536 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 538 */	
			0x13, 0x0,	/* FC_OP */
/* 540 */	NdrFcShort( 0xfffffe56 ),	/* Offset= -426 (114) */
/* 542 */	
			0x13, 0x0,	/* FC_OP */
/* 544 */	NdrFcShort( 0x192 ),	/* Offset= 402 (946) */
/* 546 */	
			0x13, 0x0,	/* FC_OP */
/* 548 */	NdrFcShort( 0x1e ),	/* Offset= 30 (578) */
/* 550 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 552 */	NdrFcLong( 0x2f ),	/* 47 */
/* 556 */	NdrFcShort( 0x0 ),	/* 0 */
/* 558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 560 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 562 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 564 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 566 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 568 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 570 */	NdrFcShort( 0x1 ),	/* 1 */
/* 572 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 574 */	NdrFcShort( 0x4 ),	/* 4 */
/* 576 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 578 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 580 */	NdrFcShort( 0x10 ),	/* 16 */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0xa ),	/* Offset= 10 (594) */
/* 586 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 588 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 590 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (550) */
/* 592 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 594 */	
			0x13, 0x0,	/* FC_OP */
/* 596 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (568) */
/* 598 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 600 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 602 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 604 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 606 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 608 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 610 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 612 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 614 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 616 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 618 */	
			0x13, 0x0,	/* FC_OP */
/* 620 */	NdrFcShort( 0xffffffa8 ),	/* Offset= -88 (532) */
/* 622 */	
			0x13, 0x10,	/* FC_OP */
/* 624 */	NdrFcShort( 0xffffffaa ),	/* Offset= -86 (538) */
/* 626 */	
			0x13, 0x10,	/* FC_OP */
/* 628 */	NdrFcShort( 0xfffffd8e ),	/* Offset= -626 (2) */
/* 630 */	
			0x13, 0x10,	/* FC_OP */
/* 632 */	NdrFcShort( 0xfffffe58 ),	/* Offset= -424 (208) */
/* 634 */	
			0x13, 0x10,	/* FC_OP */
/* 636 */	NdrFcShort( 0xffffffa2 ),	/* Offset= -94 (542) */
/* 638 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 640 */	NdrFcShort( 0x10 ),	/* 16 */
/* 642 */	0x6,		/* FC_SHORT */
			0x2,		/* FC_CHAR */
/* 644 */	0x2,		/* FC_CHAR */
			0x38,		/* FC_ALIGNM4 */
/* 646 */	0x8,		/* FC_LONG */
			0x39,		/* FC_ALIGNM8 */
/* 648 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 650 */	
			0x13, 0x0,	/* FC_OP */
/* 652 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (638) */
/* 654 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 656 */	NdrFcShort( 0x20 ),	/* 32 */
/* 658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 660 */	NdrFcShort( 0x0 ),	/* Offset= 0 (660) */
/* 662 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 664 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 666 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 668 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 670 */	NdrFcShort( 0xfffffe66 ),	/* Offset= -410 (260) */
/* 672 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 674 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 676 */	NdrFcShort( 0x4 ),	/* 4 */
/* 678 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 680 */	NdrFcShort( 0x0 ),	/* 0 */
/* 682 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 684 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 686 */	NdrFcShort( 0x4 ),	/* 4 */
/* 688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 690 */	NdrFcShort( 0x1 ),	/* 1 */
/* 692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 696 */	0x13, 0x0,	/* FC_OP */
/* 698 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (654) */
/* 700 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 702 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 704 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 706 */	NdrFcShort( 0x8 ),	/* 8 */
/* 708 */	NdrFcShort( 0x0 ),	/* 0 */
/* 710 */	NdrFcShort( 0x6 ),	/* Offset= 6 (716) */
/* 712 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 714 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 716 */	
			0x11, 0x0,	/* FC_RP */
/* 718 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (674) */
/* 720 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 722 */	NdrFcShort( 0x4 ),	/* 4 */
/* 724 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 728 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 730 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 732 */	NdrFcShort( 0x4 ),	/* 4 */
/* 734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 736 */	NdrFcShort( 0x1 ),	/* 1 */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0x0 ),	/* 0 */
/* 742 */	0x13, 0x0,	/* FC_OP */
/* 744 */	NdrFcShort( 0xffffff5a ),	/* Offset= -166 (578) */
/* 746 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 748 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 750 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 752 */	NdrFcShort( 0x8 ),	/* 8 */
/* 754 */	NdrFcShort( 0x0 ),	/* 0 */
/* 756 */	NdrFcShort( 0x6 ),	/* Offset= 6 (762) */
/* 758 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 760 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 762 */	
			0x11, 0x0,	/* FC_RP */
/* 764 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (720) */
/* 766 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 768 */	NdrFcShort( 0x8 ),	/* 8 */
/* 770 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 772 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 774 */	NdrFcShort( 0x10 ),	/* 16 */
/* 776 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 778 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 780 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (766) */
			0x5b,		/* FC_END */
/* 784 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 786 */	NdrFcShort( 0x18 ),	/* 24 */
/* 788 */	NdrFcShort( 0x0 ),	/* 0 */
/* 790 */	NdrFcShort( 0xa ),	/* Offset= 10 (800) */
/* 792 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 794 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 796 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (772) */
/* 798 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 800 */	
			0x11, 0x0,	/* FC_RP */
/* 802 */	NdrFcShort( 0xfffffd8c ),	/* Offset= -628 (174) */
/* 804 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 806 */	NdrFcShort( 0x1 ),	/* 1 */
/* 808 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 812 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 814 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 816 */	NdrFcShort( 0x8 ),	/* 8 */
/* 818 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 820 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 822 */	NdrFcShort( 0x4 ),	/* 4 */
/* 824 */	NdrFcShort( 0x4 ),	/* 4 */
/* 826 */	0x13, 0x0,	/* FC_OP */
/* 828 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (804) */
/* 830 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 832 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 834 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 836 */	NdrFcShort( 0x2 ),	/* 2 */
/* 838 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 842 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 844 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 846 */	NdrFcShort( 0x8 ),	/* 8 */
/* 848 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 850 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 852 */	NdrFcShort( 0x4 ),	/* 4 */
/* 854 */	NdrFcShort( 0x4 ),	/* 4 */
/* 856 */	0x13, 0x0,	/* FC_OP */
/* 858 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (834) */
/* 860 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 862 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 864 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 866 */	NdrFcShort( 0x4 ),	/* 4 */
/* 868 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 872 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 874 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 876 */	NdrFcShort( 0x8 ),	/* 8 */
/* 878 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 880 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 882 */	NdrFcShort( 0x4 ),	/* 4 */
/* 884 */	NdrFcShort( 0x4 ),	/* 4 */
/* 886 */	0x13, 0x0,	/* FC_OP */
/* 888 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (864) */
/* 890 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 892 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 894 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 896 */	NdrFcShort( 0x8 ),	/* 8 */
/* 898 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 900 */	NdrFcShort( 0x0 ),	/* 0 */
/* 902 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 904 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 906 */	NdrFcShort( 0x8 ),	/* 8 */
/* 908 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 910 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 912 */	NdrFcShort( 0x4 ),	/* 4 */
/* 914 */	NdrFcShort( 0x4 ),	/* 4 */
/* 916 */	0x13, 0x0,	/* FC_OP */
/* 918 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (894) */
/* 920 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 922 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 924 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 926 */	NdrFcShort( 0x8 ),	/* 8 */
/* 928 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 930 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 932 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 934 */	NdrFcShort( 0x8 ),	/* 8 */
/* 936 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 938 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 940 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 942 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (924) */
/* 944 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 946 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 948 */	NdrFcShort( 0x28 ),	/* 40 */
/* 950 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (932) */
/* 952 */	NdrFcShort( 0x0 ),	/* Offset= 0 (952) */
/* 954 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 956 */	0x38,		/* FC_ALIGNM4 */
			0x8,		/* FC_LONG */
/* 958 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 960 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffc63 ),	/* Offset= -925 (36) */
			0x5b,		/* FC_END */
/* 964 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 966 */	NdrFcShort( 0x0 ),	/* 0 */
/* 968 */	NdrFcShort( 0x4 ),	/* 4 */
/* 970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 972 */	NdrFcShort( 0xfffffc50 ),	/* Offset= -944 (28) */
/* 974 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 976 */	NdrFcShort( 0x6 ),	/* Offset= 6 (982) */
/* 978 */	
			0x13, 0x0,	/* FC_OP */
/* 980 */	NdrFcShort( 0xfffffeba ),	/* Offset= -326 (654) */
/* 982 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 984 */	NdrFcShort( 0x1 ),	/* 1 */
/* 986 */	NdrFcShort( 0x10 ),	/* 16 */
/* 988 */	NdrFcShort( 0x0 ),	/* 0 */
/* 990 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (978) */
/* 992 */	
			0x12, 0x0,	/* FC_UP */
/* 994 */	NdrFcShort( 0xfffffeac ),	/* Offset= -340 (654) */
/* 996 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 998 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1000 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (992) */
/* 1006 */	
			0x11, 0x0,	/* FC_RP */
/* 1008 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1010) */
/* 1010 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1014 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1018 */	NdrFcShort( 0xfffffe80 ),	/* Offset= -384 (634) */
/* 1020 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1022 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1028) */
/* 1024 */	
			0x13, 0x0,	/* FC_OP */
/* 1026 */	NdrFcShort( 0xfffffe8c ),	/* Offset= -372 (654) */
/* 1028 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1030 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1032 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1036 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1024) */
/* 1038 */	
			0x11, 0x10,	/* FC_RP */
/* 1040 */	NdrFcShort( 0xfffffbf2 ),	/* Offset= -1038 (2) */

			0x0
        }
    };

const CInterfaceProxyVtbl * _ImageMagickObject_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IMagickImageProxyVtbl,
    0
};

const CInterfaceStubVtbl * _ImageMagickObject_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IMagickImageStubVtbl,
    0
};

PCInterfaceName const _ImageMagickObject_InterfaceNamesList[] = 
{
    "IMagickImage",
    0
};

const IID *  _ImageMagickObject_BaseIIDList[] = 
{
    &IID_IDispatch,
    0
};


#define _ImageMagickObject_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _ImageMagickObject, pIID, n)

int __stdcall _ImageMagickObject_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_ImageMagickObject_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo ImageMagickObject_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _ImageMagickObject_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _ImageMagickObject_StubVtblList,
    (const PCInterfaceName * ) & _ImageMagickObject_InterfaceNamesList,
    (const IID ** ) & _ImageMagickObject_BaseIIDList,
    & _ImageMagickObject_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
