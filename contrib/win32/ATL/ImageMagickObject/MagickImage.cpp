// MagickImage.cpp : Implementation of CMagickImage
#include "stdafx.h"
#include "ImageMagickObject.h"
#include "MagickImage.h"
#include "comvector.h"

const LCID lcidDefault = 0;
const DWORD dwErrorBase = 5000;

/* #define DO_DEBUG 1 */

/////////////////////////////////////////////////////////////////////////////
// CMagickImage

CMagickImage::CMagickImage()
{
#ifdef DO_DEBUG
  DebugString("CMagickImage - new\n");
#endif
  SetWarningHandler(warninghandler);
  SetErrorHandler(errorhandler);
  SetFatalErrorHandler(fatalerrorhandler);
  m_bOnStartPageCalled = FALSE;
	m_bOnFirstTime = FALSE;
  AllocateArgs( nDefaultArgumentSize );
}

CMagickImage::~CMagickImage() 
{
#ifdef DO_DEBUG
	DebugString("CMagickImage - delete\n");
#endif
  DeleteArgs();
}

STDMETHODIMP CMagickImage::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IMagickImage,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CMagickImage::OnStartPage (IUnknown* pUnk)  
{
	if(!pUnk)
		return E_POINTER;

	CComPtr<IScriptingContext> spContext;
	HRESULT hr;

	// Get the IScriptingContext Interface
	hr = pUnk->QueryInterface(IID_IScriptingContext, (void **)&spContext);
	if(FAILED(hr))
		return hr;

	// Get Request Object Pointer
	hr = spContext->get_Request(&m_piRequest);
	if(FAILED(hr))
	{
		spContext.Release();
		return hr;
	}

	// Get Response Object Pointer
	hr = spContext->get_Response(&m_piResponse);
	if(FAILED(hr))
	{
		m_piRequest.Release();
		return hr;
	}
	
	// Get Server Object Pointer
	hr = spContext->get_Server(&m_piServer);
	if(FAILED(hr))
	{
		m_piRequest.Release();
		m_piResponse.Release();
		return hr;
	}
	
	// Get Session Object Pointer
	hr = spContext->get_Session(&m_piSession);
	if(FAILED(hr))
	{
		m_piRequest.Release();
		m_piResponse.Release();
		m_piServer.Release();
		return hr;
	}

	// Get Application Object Pointer
	hr = spContext->get_Application(&m_piApplication);
	if(FAILED(hr))
	{
		m_piRequest.Release();
		m_piResponse.Release();
		m_piServer.Release();
		m_piSession.Release();
		return hr;
	}
	m_bOnStartPageCalled = TRUE;
	return S_OK;
}

STDMETHODIMP CMagickImage::OnEndPage ()  
{
	m_bOnStartPageCalled = FALSE;
	// Release all interfaces
	m_piRequest.Release();
	m_piResponse.Release();
	m_piServer.Release();
	m_piSession.Release();
	m_piApplication.Release();

	return S_OK;
}

// ICollectionOnSTLImpl<> supplies these, but we override and pass through
STDMETHODIMP CMagickImage::get_Count(long *pVal)
{
  HRESULT hr = _baseMagickImage::get_Count(pVal);
	return hr;
}

STDMETHODIMP CMagickImage::get__NewEnum(LPUNKNOWN *pVal)
{
  HRESULT hr = _baseMagickImage::get__NewEnum(pVal);
	return hr;
}

STDMETHODIMP CMagickImage::get_Item(VARIANT varIndex, VARIANT *pVal)
{
  USES_CONVERSION;

  HRESULT hr = E_INVALIDARG;
  VARIANTARG *pvarIndex = &varIndex;
  VARTYPE vt = V_VT(pvarIndex);
  long lIndex = 0;
  CComVariant var;
 
  while (vt == (VT_VARIANT|VT_BYREF))
  {
	  pvarIndex = V_VARIANTREF(pvarIndex);
	  vt = V_VT(pvarIndex);
  }
  if (V_ISARRAY(pvarIndex))
    return hr;
  if ((vt & ~VT_BYREF) == VT_BSTR)
    {
      LPTSTR lpszNext;
      LPTSTR lpszVal = W2T(V_BSTR(pvarIndex));
      var = _T("");
      if (lpszVal)
        {
          Image
            *image;

          ImageInfo
            *image_info;

          ExceptionInfo
            exception;

          long
            id;

				  lpszNext = StrChr(lpszVal, _T('.'));
				  if (lpszNext == NULL)
            lpszNext = _T("%w,%h,%m");
          else
            *lpszNext++ = _T('\0');
          // lookup the registry id using token and pass the image in
          GetExceptionInfo(&exception);
          image=GetImageFromMagickRegistry(lpszVal, &id, &exception);
          if (image != (Image *) NULL)
            {
              TCHAR *text;

              image_info=CloneImageInfo((ImageInfo *) NULL);                
              text=TranslateText(image_info,image,lpszNext);
              DestroyImageList(image);
              DestroyImageInfo(image_info);
              var = text;
              LiberateMemory((void **) &text);
              hr = S_OK;
            }
        }
    }
  else
    {
      Image
        *image;

      ImageInfo
        *image_info;

      ExceptionInfo
        exception;

      long
        id;

      RegistryType
        type;

      size_t
        length;

      id = 0;
      var = _T("");
      switch(vt & ~VT_BYREF)
      {
        case VT_UI1:
          id = V_UI1(pvarIndex);
          break;
        case VT_I2:
          id = V_I2(pvarIndex);
          break;
        case VT_I4:
          id = V_I4(pvarIndex);
          break;
      }
      // lookup the registry id using token and pass the image in
      GetExceptionInfo(&exception);
      image=(Image *)GetMagickRegistry(id,&type,&length,&exception);
      if (image != (Image *) NULL)
        {
          TCHAR *text;

          image_info=CloneImageInfo((ImageInfo *) NULL);                
          text=TranslateText(image_info,image,_T("%w,%h,%m"));
          DestroyImageList(image);
          DestroyImageInfo(image_info);
          var = text;
          LiberateMemory((void **) &text);
          hr = S_OK;
        }
    }
  var.Detach(pVal);
	return hr;
}

STDMETHODIMP CMagickImage::Remove(VARIANT varIndex)
{
  USES_CONVERSION;

  HRESULT hr = E_INVALIDARG;
  VARIANTARG *pvarIndex = &varIndex;
  VARTYPE vt = V_VT(pvarIndex);
 
  while (vt == (VT_VARIANT|VT_BYREF))
  {
	  pvarIndex = V_VARIANTREF(pvarIndex);
	  vt = V_VT(pvarIndex);
  }
  if (V_ISARRAY(pvarIndex))
    return hr;
  switch(vt & ~VT_BYREF)
  {
    case VT_BSTR:
    {
      if (!V_ISBYREF(pvarIndex))
      {
        CComVariant var;
        LPTSTR lpszNext;
        LPTSTR lpszVal = W2T(V_BSTR(pvarIndex));
        var = _T("");
        if (lpszVal)
          {
            Image
              *image;

            ExceptionInfo
              exception;

            long
              id;

				    lpszNext = StrChr(lpszVal, _T('.'));
				    if (lpszNext == NULL)
              lpszNext = _T("%w,%h,%m");
            else
              *lpszNext++ = _T('\0');
            // lookup the registry id using token and pass the image in
            GetExceptionInfo(&exception);
            image=GetImageFromMagickRegistry(lpszVal, &id, &exception);
            if (image != (Image *) NULL)
              {
                DestroyImageList(image);
                if (DeleteMagickRegistry(id))
                  hr = S_OK;
              }
          }
      }
      break;
    }
    case VT_UI1:
    {
      long id = V_UI1(pvarIndex);
      if (DeleteMagickRegistry(id))
        hr = S_OK;
    }
    case VT_I2:
    {
      long id = V_I2(pvarIndex);
      if (DeleteMagickRegistry(id))
        hr = S_OK;
    }
    case VT_I4:
    {
      long id = V_I4(pvarIndex);
      if (DeleteMagickRegistry(id))
        hr = S_OK;
    }
  }
	return hr;
}

STDMETHODIMP CMagickImage::Add(SAFEARRAY **pArrayVar, VARIANT *pVar)
{
  return S_OK;
}

#include <exdisp.h>

static long SafeArraySize(SAFEARRAY *psa)
{
  HRESULT hr;
	long lBoundl, lBoundu;

	hr = ::SafeArrayGetLBound(psa, 1, &lBoundl);
  if (FAILED(hr))
    return 0;
	hr = ::SafeArrayGetUBound(psa, 1, &lBoundu);
  if (FAILED(hr))
    return 0;
  return (lBoundu - lBoundl + 1);
}

#ifdef USETHIS_CODE
int COMMagickFifoBuffer(const Image *image,const void *data,const size_t length)
{
  SAFEARRAYBOUND NewArrayBounds[1];  // 1 Dimension
  size_t tlen=length;
  CMagickImage *pMagickImage = (CMagickImage *)image->client_data;
  if ((pMagickImage != NULL) && (pMagickImage->pSafeArray != NULL))
  {
	  DWORD dwSizeOfChunk;
	  unsigned char	*pReturnBuffer = NULL;
	  HRESULT hr = S_OK;
		long lBoundl, lBoundu, lCount;

	  dwSizeOfChunk = (DWORD)length;
    /* is this the signal that says we are all done? */
    if ((dwSizeOfChunk == 0) && (data == (void *) NULL))
    {
      if (pMagickImage->m_bOnFirstTime == FALSE)
        {
          //pMagickImage->m_piResponse->End();
        }
    }
	  if ((dwSizeOfChunk == 0) || (dwSizeOfChunk == 0xFFFFFFFF))
		  return tlen;

		hr = ::SafeArrayGetLBound(pMagickImage->pSafeArray, 1, &lBoundl);
    if (FAILED(hr))
      return tlen;
		hr = ::SafeArrayGetUBound(pMagickImage->pSafeArray, 1, &lBoundu);
    if (FAILED(hr))
      return tlen;

		lCount = lBoundu - lBoundl + 1;
    NewArrayBounds[0].lLbound = 0;   // Start-Index 0
    NewArrayBounds[0].cElements = dwSizeOfChunk+lCount;  // # Elemente
    hr = SafeArrayRedim(pMagickImage->pSafeArray, NewArrayBounds);
    if (FAILED(hr))
      return tlen;

    hr = SafeArrayAccessData(pMagickImage->pSafeArray, (void**)&pReturnBuffer);
	  if( FAILED(hr) )
		  return tlen;
	  memcpy( pReturnBuffer, (unsigned char *)data+lCount, dwSizeOfChunk );
    hr = SafeArrayUnaccessData(pMagickImage->pSafeArray);
	  if( FAILED(hr) )
		  return tlen;
  }
  return(tlen);
}
#endif

HRESULT CMagickImage::DispatchToImage(IDispatch* pdisp,CComObject<CMagickImage>** ppMagickImage)
{
  // Given an IDispatch*, convert it (if possible) to a CComObject<CMagickImage>*
  IMagickImage* pinterface;
  if(FAILED(pdisp->QueryInterface(IID_IMagickImage,
        reinterpret_cast<void**>(&pinterface))))
  {
    return DISP_E_TYPEMISMATCH;
  }
  *ppMagickImage = static_cast<CComObject<CMagickImage>*>(pinterface);
  pinterface->Release();
  return S_OK;
}

HRESULT CMagickImage::UnknownToImage(IUnknown* punk,CComObject<CMagickImage>** ppMagickImage)
{
  // Given an IUnknown*, convert it (if possible) to a CComObject<CMagickImage>*
  IMagickImage* pinterface;
  if(FAILED(punk->QueryInterface(IID_IMagickImage,
        reinterpret_cast<void**>(&pinterface))))
  {
    return DISP_E_TYPEMISMATCH;
  }
  *ppMagickImage = static_cast<CComObject<CMagickImage>*>(pinterface);
  pinterface->Release();
  return S_OK;
}

STDMETHODIMP CMagickImage::get_Messages(VARIANT *pVar)
{
  HRESULT hr = S_OK;

  if (m_coll.size())
  {
    CComVector<VARIANT> v(m_coll.size());
    if( !v )
    {
      //m_coll.clear();
      return E_OUTOFMEMORY;
    }
    else
    {
      // WARNING: This nested code block is required because
      // CComVectorData ctor performs a SafeArrayAccessData
      // and you have to make sure the SafeArrayUnaccessData
      // is called (which it is in the CComVectorData dtor)
      // before you use the CComVector::DetachTo(...).
      CComVectorData<VARIANT> msgs(v);
      if( !msgs )
      {
        //m_coll.clear();
        return E_OUTOFMEMORY;
      }
      else
      {
        for(int index = 0; index < m_coll.size(); ++index)
        {
          CComVariant vt(m_coll[index]);
          HRESULT hr = vt.Detach(&msgs[index]);
        }
      }
    }    
    V_VT(pVar) = VT_ARRAY | VT_VARIANT;
    V_ARRAY(pVar) = v.Detach();
    //m_coll.clear();
  }
  return hr;
}

#ifdef SUPPORT_OBJECTS
HRESULT CMagickImage::FormatRequest(BSTR *pVal)
{
  HRESULT hr = E_INVALIDARG;
  TCHAR sz[128];
  CComBSTR bstrResult;

  if (m_bstrIOType == L"xtrnfile")
  {
    wsprintf(sz, _T("xtrnfile:%S"),m_bstrName);
    bstrResult = sz;
    bstrResult.CopyTo(pVal);
    hr = S_OK;
  }
  else if (m_bstrIOType == L"xtrnimage")
  {
    wsprintf(sz, _T("xtrnimage:0x%lx,0x%lx"),
      (unsigned long)(&pImage_info),(unsigned long)(&pImage));
    bstrResult = sz;
    bstrResult.CopyTo(pVal);
    hr = S_OK;
  }
  else if (m_bstrIOType == L"xtrnblob")
  {
    wsprintf(sz, _T("xtrnblob:0x%lx,0x%lx,%S"),
      (unsigned long)(&pBlob_data),(unsigned long)(&iBlob_length),
        m_bstrName);
    bstrResult = sz;
    bstrResult.CopyTo(pVal);
    hr = S_OK;
  }
  else if (m_bstrIOType == L"xtrnstream")
  {
    wsprintf(sz, _T("xtrnstream:0x%lx,0x%lx,%S"),
      (unsigned long)(&COMMagickFifoBuffer),(unsigned long)this,m_bstrName);
    bstrResult = sz;
    bstrResult.CopyTo(pVal);
    hr = S_OK;
  }
  else if (m_bstrIOType == L"xtrnarray")
  {
    wsprintf(sz, _T("xtrnarray:0x%lx,%S"),
      (unsigned long)(pSafeArray), m_bstrName);
    bstrResult = sz;
    bstrResult.CopyTo(pVal);
    hr = S_OK;
  }
	return hr;
}
#endif

STDMETHODIMP CMagickImage::Convert(SAFEARRAY **pArrayVar, VARIANT *pVar)
{
  USES_CONVERSION;

  HRESULT hr;

  EmptyArgs();
  AddArgs(L"-convert");
  hr = Perform(CompositeImageCommand,pArrayVar,pVar);
	if (FAILED(hr))
    {
      hr = MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,dwErrorBase+1001);
      Error(A2W("The Convert operation failed"),IID_IMagickImage,hr);
    }
  return hr;
}

STDMETHODIMP CMagickImage::Composite(SAFEARRAY **pArrayVar, VARIANT *pVar)
{
  USES_CONVERSION;

  HRESULT hr;

  EmptyArgs();
  AddArgs(L"-composite");
  hr = Perform(CompositeImageCommand,pArrayVar,pVar);
	if (FAILED(hr))
    {
      hr = MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,dwErrorBase+1002);
      Error(A2W("The Composite operation failed"),IID_IMagickImage,hr);
    }
  return hr;
}

STDMETHODIMP CMagickImage::Mogrify(SAFEARRAY **pArrayVar, VARIANT *pVar)
{
  USES_CONVERSION;

  HRESULT hr;

  EmptyArgs();
  AddArgs(L"-mogrify");
  hr = E_INVALIDARG;
  // hr = Perform(MogrifyImageCommand,pArrayVar,pVar);
	if (FAILED(hr))
    {
      hr = MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,dwErrorBase+1003);
      Error(A2W("The Mogrify operation failed"),IID_IMagickImage,hr);
    }
  return hr;
}

STDMETHODIMP CMagickImage::Montage(SAFEARRAY **pArrayVar, VARIANT *pVar)
{
  USES_CONVERSION;

  HRESULT hr;

  EmptyArgs();
  AddArgs(L"-montage");
  hr = Perform(MontageImageCommand,pArrayVar,pVar);
	if (FAILED(hr))
    {
      hr = MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,dwErrorBase+1004);
      Error(A2W("The Montage operation failed"),IID_IMagickImage,hr);
    }
  return hr;
}

STDMETHODIMP CMagickImage::Identify(SAFEARRAY **pArrayVar, VARIANT *pVar)
{
  USES_CONVERSION;

  HRESULT hr;

  EmptyArgs();
  AddArgs(L"-identify");
  hr = Perform(IdentifyImageCommand,pArrayVar,pVar);
	if (FAILED(hr))
    {
      hr = MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,dwErrorBase+1005);
      Error(A2W("The Identify operation failed"),IID_IMagickImage,hr);
    }
  return hr;
}

HRESULT CMagickImage::Perform(unsigned int (*func)(ImageInfo *image_info,
  const int argc,LPTSTR *argv,ExceptionInfo *exception),
  SAFEARRAY **pArrayVar, VARIANT *pVar)
{
  USES_CONVERSION;

  HRESULT hr = E_INVALIDARG;

#ifdef _DEBUG
  //_DbgBreak();
#endif

#ifdef DO_DEBUG
	DebugString("CMagickImage - Perform enter\n");
#endif
  m_coll.clear();
#ifdef SUPPORT_OBJECTS
  CComObject<CMagickImage>* pMagickImage;
#endif

  CComVectorData<VARIANT> rg(*pArrayVar);
  if( !rg ) 
  {
#ifdef DO_DEBUG
	  DebugString("CMagickImage - Perform exit BAD1\n");
#endif
    return E_UNEXPECTED;
  }

  int iLastVal = rg.Length();
  bool bFoundOption = false;
  for( int i = 0; i < iLastVal; ++i )
  {
    VARIANT &varIndex = rg[i];
    VARIANTARG *pvarIndex = &varIndex;
    VARTYPE vt = V_VT(pvarIndex);

    while (vt == (VT_VARIANT|VT_BYREF))
    {
	    pvarIndex = V_VARIANTREF(pvarIndex);
	    vt = V_VT(pvarIndex);
    }
    if (V_ISARRAY(pvarIndex))
    {
      TCHAR sz[128];

      if (V_ISBYREF(pvarIndex))
        {
//----->
	        SAFEARRAY *psa = *V_ARRAYREF(pvarIndex);
//------->
          if (psa)
            {
              VARTYPE vatype = (V_VT(pvarIndex) & ~(VT_ARRAY | VT_BYREF));
              int ndim = SafeArrayGetDim(psa);
              if (ndim != 1)
              {
#ifdef DO_DEBUG
	              DebugString("CMagickImage - Perform exit BAD2\n");
#endif
                return E_INVALIDARG;
              }
              if (i < (iLastVal-1))
//------------>
               {
                  bool bFoundIt = false;
                  // This is any array that is not the last one in the arg
                  // list. This means it must be an input so we just pass
                  // it along.
                  switch(vatype)
//--------------->
                  {
                    case VT_UI1:
                    {
                      wsprintf(sz, _T("xtrnarray:0x%lx,"),(unsigned long)(psa));
                      hr = AddArgs(sz);
                      break;
                    }
                    default:
//----------------->
                    {
                      CComVectorData<VARIANT> vals(psa);
                      if (vals)
//--------------------->
                        {
                          VARIANT &varFirst = vals[0];
                          VARIANTARG *pvarFirst = &varFirst;
                          if (V_VT(pvarFirst) ==  VT_BSTR)
//------------------------->
                            {
                              VARIANT &varSecond = vals[1];
                              VARIANTARG *pvarSecond = &varSecond;
                              if (V_ISARRAY(pvarSecond))
//--------------------------->
                              {
                                if (V_ISBYREF(pvarSecond))
                                  {
                                    VARTYPE vatype2 = (V_VT(pvarSecond) & ~(VT_ARRAY | VT_BYREF));
                                    if (vatype2 == VT_UI1)
                                      {
	                                      SAFEARRAY *psax = *V_ARRAYREF(pvarSecond);
                                        int ndim2 = SafeArrayGetDim(psax);
                                        if (ndim2 != 1)
                                        {
#ifdef DO_DEBUG
	                                        DebugString("CMagickImage - Perform exit BAD3\n");
#endif
                                          return E_INVALIDARG;
                                        }
                                        LPTSTR lpszVal2 = W2T(pvarFirst->bstrVal);
                                        wsprintf(sz, _T("xtrnarray:0x%lx,%s"),
                                          (unsigned long)(psax),lpszVal2);
                                        hr = AddArgs(sz);
                                      }
                                  }
                                else
                                  {
                                    VARTYPE vatype2 = (V_VT(pvarSecond) & ~(VT_ARRAY));
                                    if (vatype2 == VT_UI1)
                                      {
	                                      SAFEARRAY *psax = V_ARRAY(pvarSecond);
                                        int ndim2 = SafeArrayGetDim(psax);
                                        if (ndim2 != 1)
                                        {
#ifdef DO_DEBUG
	                                        DebugString("CMagickImage - Perform exit BAD4\n");
#endif
                                          return E_INVALIDARG;
                                        }
                                        LPTSTR lpszVal2 = W2T(pvarFirst->bstrVal);
                                        wsprintf(sz, _T("xtrnarray:0x%lx,%s"),
                                          (unsigned long)(psax),lpszVal2);
                                        hr = AddArgs(sz);
                                      }
                                  }
//--------------------------->
                              } // end of V_ISARRAY
//------------------------->
                            } // end of == VT_BSTR
                          else
                            {
                              wsprintf(sz, _T("xtrnarray:0x%lx,"),(unsigned long)(psa));
                              hr = AddArgs(sz);
                            }
                          vals.UnaccessData();
                          break;
//--------------------->
                        } // end of vals not NULL
//----------------->
                    } // end of default case
//--------------->
                  } // end of the switch
//------------->
                }
              else
                {
                  // This is the last thing in the arg list and thus must
                  // the output array. We check the contents to a string of
                  // characters that says what format to encode the data in.
                  if (vatype == VT_UI1)
                    {
                      // the output is passed as an array of bytes - this
                      // is the way that VB does it.
                      LPCWSTR pReturnBuffer = NULL;
                      long size = SafeArraySize(psa);
                      long sizeneeded;
                      char *ptrANSI;
                      hr = SafeArrayAccessData(psa, (void**)&pReturnBuffer);
	                    if(SUCCEEDED(hr))
                        {
#ifdef DO_DEBUG
	                        DebugString("CMagickImage - before w2a\n");
#endif
                          sizeneeded = WideCharToMultiByte(
                            CP_ACP, 0, pReturnBuffer, size/2, (LPSTR)NULL, 0, NULL, NULL);
                          if (sizeneeded)
                            {
                              ptrANSI = new char [sizeneeded + 1];
                              WideCharToMultiByte(
                                CP_ACP, 0, pReturnBuffer, size, (LPSTR)ptrANSI, sizeneeded, NULL, NULL);
                              ptrANSI[sizeneeded]='\0';
#ifdef DO_DEBUG
	                            DebugString("CMagickImage - after w2a\n");
#endif
                              hr = SafeArrayUnaccessData(psa);
                              SafeArrayDestroy(psa);

                              SAFEARRAY* pSafeArray = SafeArrayCreateVector(VT_UI1,0,0);
                              wsprintf(sz, _T("xtrnarray:0x%lx,%s"),
                                (unsigned long)(pSafeArray),ptrANSI);
                              hr = AddArgs(sz);
                              if (ptrANSI)
                                delete ptrANSI;
                              V_VT(pvarIndex) = VT_ARRAY | VT_UI1 | VT_BYREF;		
                              *V_ARRAYREF(pvarIndex) = pSafeArray;
                            }
                          else
                            {
#ifdef DO_DEBUG
	                            DebugString("CMagickImage - Perform exit BADA\n");
#endif
                              return E_INVALIDARG;
                            }
                        }
                      else
                      {
#ifdef DO_DEBUG
	                      DebugString("CMagickImage - Perform exit BAD5\n");
#endif
                        return E_INVALIDARG;
                      }
                    }
                  else
                    {
                      // the output is passed as a variant that is a BSTR
                      // - this is the way that VBSCRIPT and ASP does it.
                      CComVectorData<VARIANT> vals(psa);
                      if (vals)
                        {
                          VARIANT &varFirst = vals[0];
                          VARIANTARG *pvarFirst = &varFirst;
                          if (V_VT(pvarFirst) ==  VT_BSTR)
                            {
                              LPTSTR lpszVal = W2T(pvarFirst->bstrVal);
                              vals.UnaccessData();
                              SafeArrayDestroy(psa);

                              SAFEARRAY* pSafeArray = SafeArrayCreateVector(VT_UI1,0,0);
                              wsprintf(sz, _T("xtrnarray:0x%lx,%s"),
                                (unsigned long)(pSafeArray),lpszVal);
                              hr = AddArgs(sz);
                              V_VT(pvarIndex) = VT_ARRAY | VT_UI1 | VT_BYREF;		
                              *V_ARRAYREF(pvarIndex) = pSafeArray;
                            }
                        }
                      else
                      {
#ifdef DO_DEBUG
	                      DebugString("CMagickImage - Perform exit BAD6\n");
#endif
                        return E_INVALIDARG;
                      }
                    }
                }
            }
          else
          {
//------->
#ifdef DO_DEBUG
	          DebugString("CMagickImage - Perform exit BAD7\n");
#endif
            return E_INVALIDARG;
          }
        }
      else
//----->
        {
	        SAFEARRAY *psa = V_ARRAY(pvarIndex);
          if (psa)
            {
              int ndim = SafeArrayGetDim(psa);
              SafeArrayDestroy(psa);
            }
          psa = SafeArrayCreateVector(VT_UI1,0,0);		
          V_VT(pvarIndex) = VT_ARRAY | VT_UI1;		
          V_ARRAY(pvarIndex) = psa;
          wsprintf(sz, _T("xtrnarray:0x%lx,"),(unsigned long)(psa));
          hr = AddArgs(sz);
        }

    }
    else
    {
      switch(vt)
      {
        case VT_VARIANT: // invalid, should never happen
        case VT_EMPTY:
        case VT_NULL:
          bFoundOption = false;
          break;

#ifdef SUPPORT_OBJECTS
        case VT_DISPATCH:
        {
          IDispatch *pDispatch;

          bFoundOption = false;
          if (V_ISBYREF(pvarIndex))
            pDispatch = *V_DISPATCHREF(pvarIndex);
          else
            pDispatch = V_DISPATCH(pvarIndex);

          if (pDispatch)
          {
            DispatchToImage(pDispatch,&pMagickImage);
          }
          break;
        }

        case VT_UNKNOWN:
        {
          IUnknown *pUnknown;

          bFoundOption = false;
          if (V_ISBYREF(pvarIndex))
            pUnknown = *V_UNKNOWNREF(pvarIndex);
          else
            pUnknown = V_UNKNOWN(pvarIndex);

          if (pUnknown)
          {
            UnknownToImage(pUnknown,&pMagickImage);
          }
          break;
        }
#endif

        case VT_BSTR:
        {
          LPTSTR lpszVal = W2T(pvarIndex->bstrVal);
				  LPTSTR lpszNext;

          bFoundOption = false;
          // is this a command line option argument?
          if ((*lpszVal == _T('+')) || (*lpszVal == _T('-')))
          {
            bFoundOption = true;
				    lpszNext = StrChr(lpszVal, _T('='));
				    if (lpszNext == NULL)
              hr = AddArgs(V_BSTR(pvarIndex));
            else
            {
				      int nLength = lpszNext - lpszVal;
				      if (nLength > 16)
                hr = AddArgs(V_BSTR(pvarIndex));
              else
              {
                *lpszNext = _T('\0');
                hr = AddArgs(lpszVal);
                hr = AddArgs(++lpszNext);
              }
              break;
            }
          }
          else
            hr = AddArgs(V_BSTR(pvarIndex));
          break;
        }

        case VT_UI1:
        case VT_I2:
        case VT_I4:
        case VT_R4:
        case VT_R8:
        case VT_DECIMAL:
        {
          VARIANT variant;

          bFoundOption = false;
          VariantInit(&variant);
          hr = VariantChangeTypeEx(&variant, pvarIndex, lcidDefault, 0, VT_BSTR);
          if (SUCCEEDED(hr) && V_VT(&variant) == VT_BSTR)
          {
            hr = AddArgs(V_BSTR(&variant));
          }
          VariantClear(&variant);
          break;
        }

        default:
#ifdef DO_DEBUG
          DebugString("CMagickImage - Perform exit BAD8\n");
#endif
          return E_INVALIDARG;
      }
    }
  }
#ifdef DO_DEBUG
	DebugString("CMagickImage - Perform before execute\n");
#endif
  hr = Execute(func);
#ifdef DO_DEBUG
	DebugString("CMagickImage - Perform after execute\n");
#endif

  if (m_coll.size())
  {
    CComVector<VARIANT> v(m_coll.size());
    if( !v )
    {
      //m_coll.clear();
#ifdef DO_DEBUG
      DebugString("CMagickImage - Perform exit BAD9\n");
#endif
      return E_OUTOFMEMORY;
    }
    else
    {
      // WARNING: This nested code block is required because
      // CComVectorData ctor performs a SafeArrayAccessData
      // and you have to make sure the SafeArrayUnaccessData
      // is called (which it is in the CComVectorData dtor)
      // before you use the CComVector::DetachTo(...).
      CComVectorData<VARIANT> msgs(v);
      if( !msgs )
      {
        //m_coll.clear();
        return E_OUTOFMEMORY;
      }
      else
      {
        for(int index = 0; index < m_coll.size(); ++index)
        {
          CComVariant vt(m_coll[index]);
          HRESULT hr = vt.Detach(&msgs[index]);
        }
      }
    }    
    V_VT(pVar) = VT_ARRAY | VT_VARIANT;
    V_ARRAY(pVar) = v.Detach();
    //m_coll.clear();
  }
#ifdef DO_DEBUG
  DebugString("CMagickImage - Perform return\n");
#endif
  return hr;
}

void CMagickImage::warninghandler(const ExceptionType warning,const char *message,
  const char *qualifier)
{
#ifdef STORE_MESSAGES
  error_callback->error_number=errno;
  errno=0;
  if (!message)
    return;
  FormatString(error_callback->warning_text,"Warning %d: %.1024s%s%.1024s%s%s%.64s%s",warning,
    message,qualifier ? " (" : "",qualifier ? qualifier : "",
    qualifier? ")" : "",error_callback->error_number ? " [" : "",
    error_callback->error_number ? strerror(error_callback->error_number) : "",error_callback->error_number ? "]" : "");
  CComVariant var(error_callback->warning_text);
  error_callback->m_coll.push_back(var);
#endif
  char warning_text[MaxTextExtent];

  if (!message)
  {
	  DebugString("CMagickImage - warning with no message\n");
    return;
  }
  FormatString(warning_text,"CMagickImage - warning %d: %.1024s%s%.1024s%s%s%.64s%s\n",warning,
    message,qualifier ? " (" : "",qualifier ? qualifier : "",
    qualifier? ")" : "",errno ? " [" : "",
    errno ? strerror(errno) : "",errno ? "]" : "");
	DebugString(warning_text);
}

void CMagickImage::errorhandler(const ExceptionType warning,const char *message,
  const char *qualifier)
{
  char error_text[MaxTextExtent];

  if (!message)
  {
	  DebugString("CMagickImage - error with no message\n");
    return;
  }
  FormatString(error_text,"CMagickImage - error %d: %.1024s%s%.1024s%s%s%.64s%s\n",warning,
    message,qualifier ? " (" : "",qualifier ? qualifier : "",
    qualifier? ")" : "",errno ? " [" : "",
    errno ? strerror(errno) : "",errno ? "]" : "");
	DebugString(error_text);
}

void CMagickImage::fatalerrorhandler(const ExceptionType error,const char *message,
  const char *qualifier)
{
  char fatalerror_text[MaxTextExtent];

  if (!message)
  {
	  DebugString("CMagickImage - fatal error with no message\n");
    return;
  }
  FormatString(fatalerror_text,"CMagickImage - fatal error %d: %.1024s%s%.1024s%s%s%.64s%s",error,
    (message ? message : "ERROR"),
    qualifier ? " (" : "",qualifier ? qualifier : "",qualifier ? ")" : "",
    errno ? " [" : "",errno ? strerror(errno) : "",
    errno? "]" : "");
	DebugString(fatalerror_text);
  _DbgBreak();
}

HRESULT CMagickImage::Execute(unsigned int (*func)(ImageInfo *image_info,
  const int argc,char **argv,ExceptionInfo *exception))
{
  unsigned int retcode = 0;

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  /*
    Set defaults.
  */
  GetExceptionInfo(&exception);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  retcode = (func)(image_info, GetArgc(), GetArgv(), &exception);
  DestroyImageInfo(image_info);
  DestroyExceptionInfo(&exception);
  if (!retcode)
    return E_UNEXPECTED;
  return S_OK;
}

// Command line argument processing methods
HRESULT CMagickImage::AddArgs(VARIANTARG *rgvarg)
{
  HRESULT hr = S_OK;
  VARTYPE vt = V_VT(rgvarg);

  while (vt == (VT_VARIANT|VT_BYREF))
  {
	  rgvarg = V_VARIANTREF(rgvarg);
	  vt = V_VT(rgvarg);
  }
  if (V_ISARRAY(rgvarg))
  {
	  SAFEARRAY *psa = V_ISBYREF(rgvarg) ? *V_ARRAYREF(rgvarg) : V_ARRAY(rgvarg);
	  void **pav;
	  int index;
	  long *pArrayIndex, *pLowerBound, *pUpperBound;
	  VARIANT variant;

	  int dim = SafeArrayGetDim(psa);

	  VariantInit(&variant);
	  V_VT(&variant) = (vt & ~VT_ARRAY) | VT_BYREF;

	  pArrayIndex = new long [dim];
	  pLowerBound= new long [dim];
	  pUpperBound= new long [dim];
	  pav = new void *[dim];

	  for(index = 0; index < dim; ++index)
    {
      pav[index] = (void *) NULL;
      SafeArrayGetLBound(psa, index+1, &pLowerBound[index]);
      SafeArrayGetLBound(psa, index+1, &pArrayIndex[index]);
      SafeArrayGetUBound(psa, index+1, &pUpperBound[index]);
	  }

	  hr = SafeArrayLock(psa);
	  if (SUCCEEDED(hr))
    {
      while (index >= 0)
      {
		    hr = SafeArrayPtrOfIndex(psa, pArrayIndex, &V_BYREF(&variant));
		    if (FAILED(hr))
		      break;

        hr = AddArgs(&variant);
		    if (FAILED(hr))
		      break;

        for (index = dim-1; index >= 0; --index)
        {
          if (++pArrayIndex[index] <= pUpperBound[index])
            break;

		      pArrayIndex[index] = pLowerBound[index];
        }
      }

      /* preserve previous error code */
      HRESULT hr2 = SafeArrayUnlock(psa);
      if (SUCCEEDED(hr))
        hr = hr2;
	  }

    delete pArrayIndex;
    delete pLowerBound;
    delete pUpperBound;
    delete pav;

    return hr;
  }
  switch(vt & ~VT_BYREF)
  {
    case VT_VARIANT: /* invalid, should never happen */
    case VT_EMPTY:
    case VT_NULL:
      break;

    case VT_BSTR:
    {
      if (V_ISBYREF(rgvarg))
        hr = AddArgs(*V_BSTRREF(rgvarg));
      else
        hr = AddArgs(V_BSTR(rgvarg));
      break;
    }

#ifdef SUPPORT_OBJECTS
    case VT_DISPATCH:
    {
      IDispatch *pDispatch;

      if (V_ISBYREF(rgvarg))
        pDispatch = *V_DISPATCHREF(rgvarg);
      else
        pDispatch = V_DISPATCH(rgvarg);

      if (pDispatch)
      {
        CComBSTR bstrIOType;
        CComBSTR bstrName;
        CComBSTR bstrTemp;

        CComQIPtr<IDispatch> ptrDisp(pDispatch);
        CComQIPtr<IMagickImage> ptrObject;
        ptrObject = ptrDisp;
        ptrObject->get_Name(&bstrIOType);
        ptrObject->get_Name(&bstrName);
        bstrTemp = bstrIOType;
        bstrTemp += _T("@");
        bstrTemp += bstrName;
        hr = AddArgs(bstrTemp);
      }
      break;
    }

    case VT_UNKNOWN:
    {
      IUnknown *pUnknown;

      if (V_ISBYREF(rgvarg))
        pUnknown = *V_UNKNOWNREF(rgvarg);
      else
        pUnknown = V_UNKNOWN(rgvarg);

      if (pUnknown)
      {
        CComBSTR bstrName;

        CComPtr<IUnknown> ptrUnk(pUnknown);
        CComQIPtr<IMagickImage> ptrObject;
        ptrObject = ptrUnk;
        ptrObject->get_Name(&bstrName);
        hr = AddArgs(bstrName);
      }
      break;
    }
#endif

    case VT_DECIMAL:
    {
      VARIANT variant;
      VariantInit(&variant);
      hr = VariantChangeTypeEx(&variant, rgvarg, lcidDefault, 0, VT_R8);
      if (SUCCEEDED(hr) && V_VT(&variant) == VT_R8)
      {
        /* sv_setnv(sv, V_R8(&variant)); */
      }
      VariantClear(&variant);
      break;
    }

    case VT_BOOL:
    case VT_UI1:
    case VT_I2:
    case VT_I4:
    case VT_R4:
    case VT_R8:
    case VT_ERROR:
    case VT_DATE:
    case VT_CY:
    default:
    {
      VARIANT variant;

      VariantInit(&variant);
      hr = VariantChangeTypeEx(&variant, rgvarg, lcidDefault, 0, VT_BSTR);
      if (SUCCEEDED(hr) && V_VT(&variant) == VT_BSTR)
      {
        hr = AddArgs(V_BSTR(&variant));
      }
      VariantClear(&variant);
      break;
    }
  }
  return hr;
}

HRESULT CMagickImage::AddArgs(BSTR widestr)
{
  HRESULT hr = E_OUTOFMEMORY;
#ifdef DO_DEBUG
  char debug_text[MaxTextExtent];
#endif

  if (m_argvIndex >= m_argc)
    return hr;

  hr = S_OK;
  MAKE_ANSIPTR_FROMWIDE(ptrANSI, widestr);
  m_argv[m_argvIndex++] = ptrANSI;

#ifdef DO_DEBUG
  FormatString(debug_text,"CMagickImage - arg: %s\n",ptrANSI);
	DebugString(debug_text);
#endif

  if (m_argvIndex >= m_argc)
    hr = ReAllocateArgs( nDefaultArgumentSize );

  return hr;
}

HRESULT CMagickImage::AddArgs(LPTSTR lpstr)
{
  HRESULT hr = E_OUTOFMEMORY;
#ifdef DO_DEBUG
  char debug_text[MaxTextExtent];
#endif

  if (m_argvIndex >= m_argc)
    return hr;

  hr = S_OK;
#ifdef _UNICODE
  MAKE_ANSIPTR_FROMWIDE(ptrANSI, lpstr);
#else
  MAKE_COPY_OF_ANSI(ptrANSI, lpstr);
#endif
  m_argv[m_argvIndex++] = ptrANSI;

#ifdef DO_DEBUG
  FormatString(debug_text,"CMagickImage - arg: %s\n",ptrANSI);
	DebugString(debug_text);
#endif

  if (m_argvIndex >= m_argc)
    hr = ReAllocateArgs( nDefaultArgumentSize );

  return hr;
}

HRESULT CMagickImage::AllocateArgs(int cArgc)
{
  m_argv = new LPTSTR [cArgc];
  m_argv_t = new LPTSTR [cArgc];

  if ((m_argv == NULL) || (m_argv_t == NULL))
    return E_OUTOFMEMORY;
  m_argc = cArgc;

  m_argvIndex = 0;
  for (int i=0; i<m_argc; i++)
  {
    m_argv[i] = NULL;
    m_argv_t[i] = NULL;
  }
  return S_OK;
}

HRESULT CMagickImage::ReAllocateArgs(int cArgc)
{
  LPTSTR *argv = m_argv;
  LPTSTR *argv_t = m_argv_t;
  int argc = m_argc + cArgc;

  argv = new LPTSTR [argc];
  argv_t = new LPTSTR [argc];

  if ((argv == NULL) || (argv_t == NULL))
    return E_OUTOFMEMORY;

  for (int i=0; i<argc; i++)
  {
    if (i < m_argc)
    {
      argv[i] = m_argv[i];
      argv_t[i] = m_argv_t[i];
    }
    else
    {
      argv[i] = NULL;
      argv_t[i] = NULL;
    }
  }
  if (m_argv)
  {
    delete m_argv;
    m_argv = argv;
  }
  if (m_argv_t)
  {
    delete m_argv_t;
    m_argv_t = argv_t;
  }
  m_argc = argc;
  return S_OK;
}

void CMagickImage::DeleteArgs()
{
  EmptyArgs();
  if (m_argv)
    delete m_argv;
  if (m_argv_t)
    delete m_argv_t;
}

char **CMagickImage::GetArgv()
{
  return m_argv;
}

char **CMagickImage::GetArgvT()
{
  return m_argv_t;
}

int CMagickImage::GetArgc()
{
  return m_argvIndex;
}

void CMagickImage::EmptyArgs()
{
  for (int i=0; i<m_argc; i++)
  {
    if (m_argv[i] != NULL)
      delete (void *) (m_argv[i]);
    m_argv[i] = NULL;
    if (m_argv_t[i] != NULL)
      delete (void *) (m_argv_t[i]);
    m_argv_t[i] = NULL;
  }
  m_argvIndex = 0;
}

LPTSTR CMagickImage::StrChr(LPTSTR lpsz, TCHAR ch)
{
	LPTSTR p = NULL;
	while (*lpsz)
	{
		if (*lpsz == ch)
		{
			p = lpsz;
			break;
		}
		lpsz = CharNext(lpsz);
	}
	return p;
}