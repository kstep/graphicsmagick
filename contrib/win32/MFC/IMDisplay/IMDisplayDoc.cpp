// IMDisplayDoc.cpp : implementation of the CIMDisplayDoc class
//

#include "stdafx.h"
#include "IMDisplay.h"

#include "IMDisplayDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIMDisplayDoc

IMPLEMENT_DYNCREATE(CIMDisplayDoc, CDocument)

BEGIN_MESSAGE_MAP(CIMDisplayDoc, CDocument)
	//{{AFX_MSG_MAP(CIMDisplayDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIMDisplayDoc construction/destruction

CIMDisplayDoc::CIMDisplayDoc()
: m_pImage( NULL )
{
	// TODO: add one-time construction code here

}

CIMDisplayDoc::~CIMDisplayDoc()
{
    if ( m_pImage )
	delete m_pImage;
}

BOOL CIMDisplayDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CIMDisplayDoc serialization

void CIMDisplayDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CIMDisplayDoc diagnostics

#ifdef _DEBUG
void CIMDisplayDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIMDisplayDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIMDisplayDoc commands

BOOL CIMDisplayDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	m_szFile = lpszPathName;
	DoReadImage();
	
	return TRUE;
}

//-----------------------------------------------------------------------
// DoReadImage()
// Read image.
//-----------------------------------------------------------------------

BOOL CIMDisplayDoc::DoReadImage( void )
{
    BeginWaitCursor();

    // New image object
    if (m_pImage != NULL)
    {
	delete m_pImage;
	m_pImage = NULL;
    }
    m_pImage = new Image;

    // Read the image and handle any exceptions
    try
    {
	m_pImage->read(m_szFile.GetBuffer(MAX_PATH+1));
    }

    catch(Exception e)
    {
	delete m_pImage;
	m_pImage = NULL;
	DoDisplayError("DoReadImage",e.what());
	return FALSE;
    }

    catch(exception e)
    {
	delete m_pImage;
	m_pImage = NULL;
	DoDisplayError("DoReadImage",e.what());
	return FALSE;
    }

    EndWaitCursor();

    return TRUE;
}

//-----------------------------------------------------------------------
// DoDisplayError()
// Display the cause of any unhandle exceptions.
//-----------------------------------------------------------------------

void CIMDisplayDoc::DoDisplayError(CString szFunction, CString szCause)
{
    CString szMsg;
    szMsg.Format("IMDisplayDoc function [%s] encountered an error.\n%s",szFunction,szCause);
    AfxMessageBox(szMsg,MB_OK);
}
