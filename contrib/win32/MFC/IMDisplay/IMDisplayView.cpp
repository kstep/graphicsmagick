// IMDisplayView.cpp : implementation of the CIMDisplayView class
//

#include "stdafx.h"
#include "IMDisplay.h"

#include "ResizeDialog.h"
#include "RollDialog.h"
#include "RotateDialog.h"
#include "ShearDialog.h"
#include "IMDisplayDoc.h"
#include "IMDisplayView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIMDisplayView

IMPLEMENT_DYNCREATE(CIMDisplayView, CScrollView)

BEGIN_MESSAGE_MAP(CIMDisplayView, CScrollView)
	//{{AFX_MSG_MAP(CIMDisplayView)
	ON_COMMAND(IDM_HALFSIZE, OnHalfsize)
	ON_COMMAND(IDM_ORIGINALSIZE, OnOriginalsize)
	ON_COMMAND(IDM_DOUBLESIZE, OnDoublesize)
	ON_COMMAND(IDM_RESIZE, OnResize)
	ON_COMMAND(IDM_REVERT, OnRevert)
	ON_COMMAND(ID_TRANSFORM_ROTATE, OnRotate)
	ON_COMMAND(ID_TRANSFORM_ROTATELEFT, OnRotateLeft)
	ON_COMMAND(ID_TRANSFORM_ROTATERIGHT, OnRotateRight)
	ON_COMMAND(ID_TRANSFORM_FLIP, OnFlip)
	ON_COMMAND(ID_TRANSFORM_FLOP, OnFlop)
	ON_COMMAND(ID_TRANSFORM_SHEAR, OnShear)
	ON_COMMAND(ID_TRANSFORM_ROLL, OnRoll)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_TRANSFORM_CHOP, OnChop)
	ON_COMMAND(ID_TRANSFORM_CROP, OnCrop)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORM_CHOP, OnUpdateChop)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORM_CROP, OnUpdateCrop)
	ON_COMMAND(ID_TRANSFORM_TRIMEDGES, OnTrimedges)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIMDisplayView construction/destruction

CIMDisplayView::CIMDisplayView()
:mOffscreenDC( NULL ), mViewDirty( true ), mUndoImage( NULL )
{
	// setup the CRectTracker styles
	//		I want a simple dotted line, with resize handles outside
	m_tracker.m_nStyle &= ~CRectTracker::solidLine;
	m_tracker.m_nStyle ^= CRectTracker::dottedLine;
	m_tracker.m_nStyle &= ~CRectTracker::resizeInside;
	m_tracker.m_nStyle ^= CRectTracker::resizeOutside;
}

CIMDisplayView::~CIMDisplayView()
{
    if ( mOffscreenDC )
	delete mOffscreenDC;

    if ( mUndoImage )
	delete mUndoImage;
}

BOOL CIMDisplayView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CIMDisplayView drawing

void CIMDisplayView::OnDraw(CDC* pDC)
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    DoDisplayImage( pDoc->GetImage(), pDC );
}

void CIMDisplayView::OnInitialUpdate()
{
    CScrollView::OnInitialUpdate();

    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    mBaseGeo.width( pDoc->GetImage()->columns() );
    mBaseGeo.height( pDoc->GetImage()->rows() );

    SetViewSize();
}

/////////////////////////////////////////////////////////////////////////////
// CIMDisplayView printing

BOOL CIMDisplayView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CIMDisplayView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CIMDisplayView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CIMDisplayView diagnostics

#ifdef _DEBUG
void CIMDisplayView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CIMDisplayView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CIMDisplayDoc* CIMDisplayView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIMDisplayDoc)));
	return (CIMDisplayDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIMDisplayView message handlers

void CIMDisplayView::OnLButtonDown(UINT nFlags, CPoint point) 
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    CRect rectSave;
    m_tracker.GetTrueRect(rectSave);
    if (m_tracker.HitTest(point) < 0)
    {
		// just to demonstrate CRectTracker::TrackRubberBand
		CRectTracker tracker;
		if (tracker.TrackRubberBand(this, point, true))
		{
			// MessageBeep(0); // beep indicates TRUE

			// see if rubber band intersects with the doc's tracker
			CRect rectT;
			tracker.m_rect.NormalizeRect(); // so intersect rect works
			if (rectT.IntersectRect(tracker.m_rect, m_tracker.m_rect))
			{
				/*
				// dotted line for outline
				m_tracker.m_nStyle &= CRectTracker::dottedLine;

				// if so, put resize handles on it (ie. select it)
				if (m_tracker.m_nStyle & CRectTracker::resizeInside)
				{
					// swap from resize inside to resize outside for effect
					m_tracker.m_nStyle &= ~CRectTracker::resizeInside;
					m_tracker.m_nStyle |= CRectTracker::resizeOutside;
				}
				else
				{
					// just use inside resize handles on first time
					m_tracker.m_nStyle &= ~CRectTracker::resizeOutside;
					m_tracker.m_nStyle |= CRectTracker::resizeInside;
				}
				*/

				UpdateTheView( false );
			}

			m_tracker.m_rect = tracker.m_rect;
		} else {	// clear the selection!
			m_tracker.m_rect.left	= m_tracker.m_rect.right = 0;
			m_tracker.m_rect.top	= m_tracker.m_rect.bottom = 0;
		}

		UpdateTheView( false );
    }
    else if (m_tracker.Track(this, point, true))
    {
		UpdateTheView( false );
    }

    CScrollView::OnLButtonDown(nFlags, point);
}


void CIMDisplayView::OnHalfsize() 
{
    Geometry	newGeo( 50, 50 );
    newGeo.percent( true );
    ScaleImage( newGeo );
}

void CIMDisplayView::OnOriginalsize() 
{
    ScaleImage( mBaseGeo );
}

void CIMDisplayView::OnDoublesize() 
{
    Geometry	newGeo( 200, 200 );
    newGeo.percent( true );
    ScaleImage( newGeo );
}

void CIMDisplayView::OnResize() 
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    CResizeDialog   dlg;

    dlg.Width( pDoc->GetImage()->columns() );
    dlg.Height( pDoc->GetImage()->rows() );

    if ( dlg.DoModal() == IDOK ) {
	Geometry	newGeo( dlg.Width(), dlg.Height() );
	newGeo.percent( dlg.IsPercentage() );
	ScaleImage( newGeo );
    }
}

void CIMDisplayView::OnRevert() 
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    // we revert by rereading the original image!
    pDoc->DoReadImage(); 
    
    UpdateTheView();
}

void CIMDisplayView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
    SetViewSize();
}


void CIMDisplayView::OnRotate() 
{
    CRotateDialog   dlg;
    dlg.Angle( 180 );	// good default as any...

    if ( dlg.DoModal() == IDOK ) {
	RotateImage( dlg.Angle() );
    }
}

void CIMDisplayView::OnRotateLeft() 
{
    RotateImage( -90 );
}

void CIMDisplayView::OnRotateRight() 
{
    RotateImage( 90 );
}

void CIMDisplayView::OnFlip() 
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    SetupUndo();

    BeginWaitCursor();

    try {
	pDoc->GetImage()->flip();
    }

    catch(Exception e)
    {
	DoDisplayError("OnFlip",e.what());
    }
    
    EndWaitCursor();

    UpdateTheView();
}

void CIMDisplayView::OnFlop() 
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    SetupUndo();

    BeginWaitCursor();

    try {
	pDoc->GetImage()->flop();
    }

    catch(Exception e)
    {
	DoDisplayError("OnFlip",e.what());
    }
    
    EndWaitCursor();

    UpdateTheView();
	
}

void CIMDisplayView::OnShear() 
{
    CShearDialog   dlg;
    dlg.XShear( 45 );	// good default as any...
    dlg.YShear( 45 );	// good default as any...

    if ( dlg.DoModal() == IDOK ) {
	CIMDisplayDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	SetupUndo();

	BeginWaitCursor();

	try {
	    pDoc->GetImage()->shear( dlg.XShear(), dlg.YShear() );
	}

	catch(Exception e)
	{
	    DoDisplayError("OnShear",e.what());
	}
    
	EndWaitCursor();

	UpdateTheView();
    }
}


void CIMDisplayView::OnRoll() 
{
    CRollDialog   dlg;
    dlg.HPixels( 25 );	// good default as any...
    dlg.VPixels( 25 );	// good default as any...

    if ( dlg.DoModal() == IDOK ) {
	CIMDisplayDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	SetupUndo();

	BeginWaitCursor();

	try {
	    pDoc->GetImage()->roll( dlg.HPixels(), dlg.VPixels() );
	}

	catch(Exception e)
	{
	    DoDisplayError("OnRoll",e.what());
	}
    
	EndWaitCursor();

	UpdateTheView();
    }
}


void CIMDisplayView::OnChop() 
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    SetupUndo();

    BeginWaitCursor();

    try {
	CRect trackRect;
	m_tracker.GetTrueRect(trackRect);

	Geometry    newGeo( trackRect.Width(), trackRect.Height(), trackRect.left, trackRect.top );
	pDoc->GetImage()->chop( newGeo );
    }

    catch(Exception e)
    {
	DoDisplayError("OnChop",e.what());
    }

    EndWaitCursor();

    UpdateTheView();
}

void CIMDisplayView::OnUpdateChop(CCmdUI* pCmdUI) 
{
    // Enable/Disable the user-interface object
    pCmdUI->Enable( m_tracker.m_rect.Width() > 0 );
}

void CIMDisplayView::OnCrop() 
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    SetupUndo();

    BeginWaitCursor();

    try {
	CRect trackRect;
	m_tracker.GetTrueRect(trackRect);

	Geometry    newGeo( trackRect.Width(), trackRect.Height(), trackRect.left, trackRect.top );
	pDoc->GetImage()->crop( newGeo );
    }

    catch(Exception e)
    {
	DoDisplayError("OnCrop",e.what());
    }

    EndWaitCursor();

    UpdateTheView();
}

void CIMDisplayView::OnUpdateCrop(CCmdUI* pCmdUI) 
{
    // Enable/Disable the user-interface object
    pCmdUI->Enable( m_tracker.m_rect.Width() > 0 );
}

void CIMDisplayView::OnTrimedges() 
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    SetupUndo();

    BeginWaitCursor();

    try {
	pDoc->GetImage()->trim( );
    }

    catch(Exception e)
    {
	DoDisplayError("onTrimEdges",e.what());
    }

    EndWaitCursor();

    UpdateTheView();
}

void CIMDisplayView::OnEditUndo() 
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    pDoc->SetImage( mUndoImage );
    mUndoImage = NULL;

    UpdateTheView();
}

void CIMDisplayView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
    // Enable/Disable the user-interface object
    pCmdUI->Enable( mUndoImage != NULL );
}



void CIMDisplayView::OnEditSelectAll() 
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    m_tracker.m_rect = CRect(0,0,pDoc->GetImage()->columns(), pDoc->GetImage()->rows());
    UpdateTheView( false );
}

void CIMDisplayView::OnEditCopy() 
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    OpenClipboard();
    EmptyClipboard();

    unsigned long nPixels = m_tracker.m_rect.Width() * m_tracker.m_rect.Height();

    /*
    CClientDC cdc(this);

    BITMAP  bitmap;
    CBitmap * junk = new CBitmap();
#if 0
    junk->CreateCompatibleBitmap(&cdc,m_tracker.m_rect.Width(),m_tracker.m_rect.Height());
    junk->GetBitmap(&bitmap);
#else
    bitmap.bmType = 0;
    bitmap.bmWidth = m_tracker.m_rect.Width();
    bitmap.bmHeight = m_tracker.m_rect.Height();
    bitmap.bmWidthBytes = bitmap.bmWidth * 4;
    bitmap.bmPlanes = 1;
    bitmap.bmBitsPixel = 32;
    bitmap.bmBits = NULL;
    junk->CreateBitmapIndirect( &bitmap );
#endif

    CDC dc;
    dc.CreateCompatibleDC( &cdc );
    dc.SelectObject(junk);
*/

    BITMAP  bitmap;
    bitmap.bmType = 0;
    bitmap.bmWidth = m_tracker.m_rect.Width();
    bitmap.bmHeight = m_tracker.m_rect.Height();
    bitmap.bmWidthBytes = bitmap.bmWidth * 4;
    bitmap.bmPlanes = 1;
    bitmap.bmBitsPixel = 32;
    bitmap.bmBits = NULL;

    long    memSize = nPixels * bitmap.bmBitsPixel;
    HANDLE  theBitsH = (HANDLE) ::GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, memSize);
    if (theBitsH == NULL)
	 DoDisplayError( "OnEditCopy", GetLastError() );
    else {
	PixelPacket *pPixels = pDoc->GetImage()->getPixels(m_tracker.m_rect.left,
							   m_tracker.m_rect.top,
							   m_tracker.m_rect.Width(),
							   m_tracker.m_rect.Height());

	RGBQUAD * theBits = (RGBQUAD *) ::GlobalLock((HGLOBAL) theBitsH);
	RGBQUAD *pDestPixel = theBits;
	if ( bitmap.bmBits == NULL )
	    bitmap.bmBits = theBits;

	for( unsigned long nPixelCount = nPixels; nPixelCount ; nPixelCount-- )
	{
		pDestPixel->rgbRed	    = Downscale(pPixels->red);
	    pDestPixel->rgbGreen    = Downscale(pPixels->green);
	    pDestPixel->rgbBlue	    = Downscale(pPixels->blue);
	    pDestPixel->rgbReserved = 0;
	    ++pDestPixel;
	    ++pPixels;
	}

#if 0
	DWORD	result = junk->SetBitmapBits( memSize, theBits );
	if ( result == 0 )
	     DoDisplayError( "OnEditCopy", GetLastError() );
#else
    bitmap.bmBits = theBits;
    CBitmap * junk = new CBitmap();
    junk->CreateBitmapIndirect( &bitmap );
#endif
	::GlobalUnlock((HGLOBAL) theBitsH);

	SetClipboardData(CF_BITMAP, junk->m_hObject);
	CloseClipboard();

	delete junk;
    }

}


void CIMDisplayView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
    // Enable/Disable the user-interface object
    pCmdUI->Enable( m_tracker.m_rect.Width() > 0 );
}


BOOL CIMDisplayView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	BOOL	isTracker = m_tracker.SetCursor( pWnd, nHitTest );
	if ( isTracker == 0 )
		return CScrollView::OnSetCursor(pWnd, nHitTest, message);
	else
		return isTracker;
}

/////////////////////////////////////////////////////////////////////////////
// Internal stuff

void CIMDisplayView::SetViewSize()
{
    CSize sizeTotal;
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if ( pDoc->GetImage() ) {
	sizeTotal.cx = pDoc->GetImage()->columns();
	sizeTotal.cy = pDoc->GetImage()->rows();
    } else {
	sizeTotal.cx = sizeTotal.cy = 100;
    }
    SetScrollSizes(MM_TEXT, sizeTotal);
}

void CIMDisplayView::UpdateTheView( bool inClearTracker )
{
    mViewDirty = true;
    Invalidate();
    if ( inClearTracker )
	m_tracker.m_rect = CRect(0,0,0,0);
    OnUpdate( this, NULL, NULL );
}

void CIMDisplayView::ScaleImage( Geometry& inGeometry )
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    SetupUndo();

    BeginWaitCursor();

    try {
	// NOTE: should this be scale or sample?
	pDoc->GetImage()->scale( inGeometry );
    }

    catch(Exception e)
    {
	DoDisplayError("ScaleImage",e.what());
    }
    
    EndWaitCursor();

    UpdateTheView();
}

void CIMDisplayView::RotateImage( double inDegrees )
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    SetupUndo();

    BeginWaitCursor();

    try {
	pDoc->GetImage()->rotate( inDegrees );
    }

    catch(Exception e)
    {
	DoDisplayError("RotateImage",e.what());
    }
    
    EndWaitCursor();

    UpdateTheView();
}

void CIMDisplayView::SetupUndo()
{
    CIMDisplayDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    mUndoImage = new Image( *pDoc->GetImage() );   
}

//-----------------------------------------------------------------------
// DoDisplayImage()
// Display the image in the client window.  Scale the image to fit.
//-----------------------------------------------------------------------

void CIMDisplayView::DoDisplayImage( Image* inImage, CDC* pDC )
{
    // make sure we're getting a valid image!
    if (inImage == NULL)
    {
      return;
    }

    // if the view is dirty, dispose the old offscreen!
    if ( mViewDirty == true ) {
	delete mOffscreenDC;
	mOffscreenDC = NULL;
    }

    // make sure we have a valid destination DC
    if (pDC != NULL)
    {
	bool isPrinting = pDC->IsPrinting();

	// if we don't already have a ready offscreen, then create it!
	if ( !mOffscreenDC ) {
	    mOffscreenDC = new CDC();
	    mOffscreenDC->CreateCompatibleDC( pDC );

	    CRect rectClient;
	    GetClientRect(rectClient);

	    // Clear the background
	    mOffscreenDC->FillSolidRect(rectClient,mOffscreenDC->GetBkColor());

	    // Set up the Windows bitmap header
	    BITMAPINFOHEADER bmi;
	    bmi.biSize = sizeof(BITMAPINFOHEADER);	// Size of structure
	    bmi.biWidth = inImage->columns();	// Bitmaps width in pixels
	    bmi.biHeight = (-1)*inImage->rows();	// Bitmaps height n pixels
	    bmi.biPlanes = 1;				// Number of planes in the image
	    bmi.biBitCount = 32;			// The number of bits per pixel
	    bmi.biCompression = BI_RGB;		// The type of compression used
	    bmi.biSizeImage = 0;			// The size of the image in bytes
	    bmi.biXPelsPerMeter = 0;			// Horizontal resolution
	    bmi.biYPelsPerMeter = 0;			// Veritical resolution
	    bmi.biClrUsed = 0;			// Number of colors actually used
	    bmi.biClrImportant = 0;			// Colors most important
	    mBMI = bmi;	// keep it for clipboard use...

	#if QuantumDepth == 8
	    // Determine the size of the scaled image
	    // Don't allow image to be zoomed
	    CSize sizeScaled = Scale(CSize(inImage->columns(),inImage->rows()), rectClient.Size());

	    // Calculate the top-left co-ordinates of the image
	    CPoint pt = rectClient.TopLeft();
	    int nImageX= ((rectClient.Width()-sizeScaled.cx)/2)+ pt.x ;
	    int nImageY = ((rectClient.Height()-sizeScaled.cy)/2) + pt.y;

	    // Extract the pixels from Magick++ image object
	    PixelPacket *pPixels = inImage->getPixels(0,0,inImage->columns(),inImage->rows());

	    // Blast it to the device context
	    SetStretchBltMode(mOffscreenDC,COLORONCOLOR);

	    StretchDIBits
	    (
		mOffscreenDC,		// handle of device context 
		nImageX,		// x-coordinate of upper-left corner of dest. rect. 
		nImageY,		// y-coordinate of upper-left corner of dest. rect.
		sizeScaled.cx,		// width of destination rectangle 
		sizeScaled.cy,		// height of destination rectangle
		0,			// x-coordinate of upper-left corner of source rect. 
		0, 			// y-coordinate of upper-left corner of source rect
		inImage->columns(),	// width of source rectangle 
		inImage->rows(),	// height of source rectangle 
		pPixels,		// address of bitmap bits
		(BITMAPINFO *)&bmi,	// address of bitmap data 
		DIB_RGB_COLORS,		// usage 
		SRCCOPY			// raster operation code
	    );
	#elif QuantumDepth == 16
	    // Extract the pixels from Magick++ image object and down convert to 8-bit quantum

	    PixelPacket *pPixels = inImage->getPixels(0,0,inImage->columns(),inImage->rows());

	    RGBQUAD *prgbaDIB = 0;
	    HBITMAP hBitmap = CreateDIBSection
	    (
		mOffscreenDC->GetSafeHdc(),		// handle to device context 
		(BITMAPINFO *)&bmi,	// pointer to structure containing bitmap size, format, and color data
		DIB_RGB_COLORS,		// color data type indicator: RGB values or palette indices 
		(void**)&prgbaDIB,	// pointer to variable to receive a pointer to the bitmap's bit values
		NULL,			// optional handle to a file mapping object 
		0			// offset to the bitmap bit values within the file mapping object 
	    );

	    if ( !hBitmap )
		return;

	    unsigned long nPixels = inImage->columns() * inImage->rows();
	    RGBQUAD *pDestPixel = prgbaDIB;
	    for( unsigned long nPixelCount = nPixels; nPixelCount ; nPixelCount-- )
	    {
      		pDestPixel->rgbRed		= Downscale(pPixels->red);
			pDestPixel->rgbGreen	= Downscale(pPixels->green);
			pDestPixel->rgbBlue		= Downscale(pPixels->blue);
			pDestPixel->rgbReserved = 0;
			++pDestPixel;
			++pPixels;
	    }
	#endif

	    // Now copy the bitmap to devi.
	    mOffscreenDC->SelectObject( hBitmap );
	}

	pDC->BitBlt( 0, 0, inImage->columns(), inImage->rows(), mOffscreenDC, 0, 0, SRCCOPY );
	mViewDirty = false; // not any more!

	// draw the marching ants, if any
	if ( !isPrinting && m_tracker.m_rect.Width() && m_tracker.m_rect.Height() )
		m_tracker.Draw( pDC );
    }
}

//-----------------------------------------------------------------------
// DoDisplayError()
// Display the cause of any unhandle exceptions.
//-----------------------------------------------------------------------

void CIMDisplayView::DoDisplayError(CString szFunction, CString szCause)
{
    CString szMsg;
    szMsg.Format("IMDisplayView function [%s] encountered an error.\n%s",szFunction,szCause);
    AfxMessageBox(szMsg,MB_OK);
}

void CIMDisplayView::DoDisplayError(CString szFunction, DWORD inError )
{
    LPVOID lpMsgBuf;
    FormatMessage( 
	FORMAT_MESSAGE_ALLOCATE_BUFFER | 
	    FORMAT_MESSAGE_FROM_SYSTEM | 
	    FORMAT_MESSAGE_IGNORE_INSERTS,
	NULL,
	inError,
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	(LPTSTR) &lpMsgBuf,
	0,
	NULL 
    );
    MessageBox( (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
    LocalFree( lpMsgBuf );
}
