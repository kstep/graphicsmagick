//+--------------------------------------------------------------
//
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
// 
//  Copyright (c) 1999 Digital Imaging Group, Inc.
// 
//  Contents: DRT support functions
//
//---------------------------------------------------------------

#ifndef __UTIL_HXX__
#define __UTIL_HXX__

extern BOOL fExitOnFail;

void SetData(void);
void CleanData(void);
void UnsetData(void);
void out(char *fmt, ...);
void error(int code, char *fmt, ...);
void errorprint(char *fmt, ...);
char *OlecsOut(OLECHAR const *ptcs);
char *ScText(SCODE sc);
HRESULT Result(HRESULT hr);
HRESULT IllResult(char *pszText, HRESULT hr);
char *VerifyStructure(IStorage *pstg, char *pszStructure);
char *CreateStructure(IStorage *pstg, char *pszStructure);
void VerifyStat(STATSTG *pstat, OLECHAR *ptcsName, DWORD type, DWORD grfMode);
BOOL Exists(OLECHAR *file);
ULONG Length(OLECHAR *file);
BOOL IsEqualTime(FILETIME ttTime, FILETIME ttCheck);
void GetFullPath(OLECHAR *file, OLECHAR *path, int len);
HRESULT drtMemAlloc(ULONG ulcb, void **ppv);
void drtMemFree(void *pv);
char *GuidText(GUID const *pguid);

#endif // #ifndef __UTIL_HXX__
