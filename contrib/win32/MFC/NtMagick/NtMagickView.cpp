// NtMagickView.cpp : implementation of the CNtMagickView class
//

#include "stdafx.h"
#include "NtMagick.h"

#include "NtMagickDoc.h"
#include "NtMagickView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNtMagickView

IMPLEMENT_DYNCREATE(CNtMagickView, CView)

BEGIN_MESSAGE_MAP(CNtMagickView, CView)
        //{{AFX_MSG_MAP(CNtMagickView)
        ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
        ON_COMMAND(ID_IMAGE_FLIP_HORIZONTAL, OnImageFlipHorizontal)
        ON_UPDATE_COMMAND_UI(ID_IMAGE_FLIP_HORIZONTAL, OnUpdateImageFlipHorizontal)
        ON_COMMAND(ID_IMAGE_FLIP_VERTICAL, OnImageFlipVertical)
        ON_UPDATE_COMMAND_UI(ID_IMAGE_FLIP_VERTICAL, OnUpdateImageFlipVertical)
        ON_COMMAND(ID_IMAGE_ROTATE_180, OnImageRotate180)
        ON_UPDATE_COMMAND_UI(ID_IMAGE_ROTATE_180, OnUpdateImageRotate180)
        ON_COMMAND(ID_IMAGE_ROTATE_90, OnImageRotate90)
        ON_UPDATE_COMMAND_UI(ID_IMAGE_ROTATE_90, OnUpdateImageRotate90)
        ON_COMMAND(ID_IMAGE_ROTATE_90CCW, OnImageRotate90ccw)
        ON_UPDATE_COMMAND_UI(ID_IMAGE_ROTATE_90CCW, OnUpdateImageRotate90ccw)
        ON_COMMAND(ID_FILE_CLEAR, OnFileClear)
        ON_UPDATE_COMMAND_UI(ID_FILE_CLEAR, OnUpdateFileClear)
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNtMagickView construction/destruction

CNtMagickView::CNtMagickView()
{

  CString szDir;

  // Init local objects
  m_pImage = NULL;

#ifdef MANIPULATE_ENVVARS
  // Get the current working directory
  GetCurrentDirectory(MAX_PATH, szDir.GetBuffer(MAX_PATH + 1));

  // Dynamically set the ImageMagick environment
  if (getenv("MAGICK_MODULE_PATH") == NULL)
  {
    CString s;
    s.Format("MAGICK_MODULE_PATH=%s",szDir);
    putenv(s);
  }

  if (getenv("MAGICK_DELEGATE_PATH") == NULL)
  {
    CString s;
    s.Format("MAGICK_DELEGATE_PATH=%s",szDir);
    putenv(s);
  }
#endif
}

CNtMagickView::~CNtMagickView()
{
  if (m_pImage != NULL)
  {
    delete m_pImage;
  }
}

BOOL CNtMagickView::PreCreateWindow(CREATESTRUCT& cs)
{
        // TODO: Modify the Window class or styles here by modifying
        //  the CREATESTRUCT cs

        return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CNtMagickView drawing

void CNtMagickView::OnDraw(CDC* pDC)
{
  CNtMagickDoc* pDoc = GetDocument();
  ASSERT_VALID(pDoc);
  CWnd* pMainWnd = AfxGetMainWnd();
  pMainWnd->SetWindowText("ImageMagick Win32 Image Viewer");
  DoDisplayImage();
}

/////////////////////////////////////////////////////////////////////////////
// CNtMagickView diagnostics

#ifdef _DEBUG
void CNtMagickView::AssertValid() const
{
        CView::AssertValid();
}

void CNtMagickView::Dump(CDumpContext& dc) const
{
        CView::Dump(dc);
}

CNtMagickDoc* CNtMagickView::GetDocument() // non-debug version is inline
{
        ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNtMagickDoc)));
        return (CNtMagickDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNtMagickView message handlers

//-----------------------------------------------------------------------
// DoFileOpen()
// Select the image to be displayed
//-----------------------------------------------------------------------

void CNtMagickView::OnFileOpen()
{
  CString   szFolder;
  CString   szFilter;
  szFilter += NTMAGICK_JPEG;
  szFilter += NTMAGICK_BMP;
  szFilter += NTMAGICK_GIF;
  szFilter += NTMAGICK_TIF;
  szFilter += NTMAGICK_ICON;
  szFilter += NTMAGICK_ALL;
  szFilter += "|";

  szFolder = AfxGetApp()->GetProfileString("Image","Path","");

  CFileDialog fileDlg(TRUE,NULL,NULL,NULL,szFilter,NULL);
  fileDlg.m_ofn.Flags|=OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_READONLY;
  fileDlg.m_ofn.lpstrTitle="Choose the image to view";
  fileDlg.m_ofn.lpstrInitialDir= szFolder;

  if (fileDlg.DoModal()== IDOK)
  {
    m_szFile = fileDlg.GetPathName();
    if (DoReadImage())
    {
      DoDisplayImage();

      CFile     fileImage;

      fileImage.SetFilePath(m_szFile);
      szFolder = m_szFile;
      szFolder.TrimRight(fileImage.GetFileName());
      szFolder.TrimRight("\\");
      AfxGetApp()->WriteProfileString("Image","Path", szFolder);
    }
  }
}

//-----------------------------------------------------------------------
// DoReadImage()
// Read image.
//-----------------------------------------------------------------------

BOOL CNtMagickView::DoReadImage()
{
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
    DoDisplayError("Read",e.what());
    return FALSE;
  }

  catch(exception e)
  {
    delete m_pImage;
    m_pImage = NULL;
    DoDisplayError("Read",e.what());
    return FALSE;
  }

  return TRUE;
}

//-----------------------------------------------------------------------
// DoDisplayError()
// Display the cause of any unhandle exceptions.
//-----------------------------------------------------------------------

void CNtMagickView::DoDisplayError(CString szFunction, CString szCause)
{
  CString szMsg;
  szMsg.Format("NtMagickView function [%s] encountered an error.\n%s",szFunction,szCause);
  AfxMessageBox(szMsg,MB_OK);
}

//-----------------------------------------------------------------------
// DoDisplayImage()
// Display the image in the client window.  Scale the image to fit.
//-----------------------------------------------------------------------

void CNtMagickView::DoDisplayImage()
{
  if (m_pImage == NULL)
  {
    return;
  }

  CDC *pDC = GetDC();
  if (pDC != NULL)
  {
    CRect rectClient;
    GetClientRect(rectClient);

    // Clear the background
    pDC->FillSolidRect(rectClient,pDC->GetBkColor());

    // Determine the size of the scaled image
    // Don't allow image to be zoomed
    CSize sizeScaled = Scale(CSize(m_pImage->columns(),m_pImage->rows()), rectClient.Size());

    // Calculate the top-left co-ordinates of the image
    CPoint pt = rectClient.TopLeft();
    int nImageX= ((rectClient.Width()-sizeScaled.cx)/2)+ pt.x ;
    int nImageY = ((rectClient.Height()-sizeScaled.cy)/2) + pt.y;

    // Set up the Windows bitmap header
    BITMAPINFOHEADER bmi;
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biWidth = m_pImage->columns();
    bmi.biHeight = (-1)*m_pImage->rows();
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biCompression = BI_RGB;
    bmi.biSizeImage = 0;
    bmi.biXPelsPerMeter = 0;
    bmi.biYPelsPerMeter = 0;
    bmi.biClrUsed = 0;
    bmi.biClrImportant = 0;

#if QuantumDepth == 8
    // Extract the pixels from Magick++ image object
    PixelPacket *pPixels = m_pImage->getPixels(0,0,m_pImage->columns(),m_pImage->rows());

    // Blast it to the device context
    SetStretchBltMode(pDC->m_hDC,COLORONCOLOR);

    StretchDIBits(pDC->m_hDC,
                  nImageX,
                  nImageY,
                  sizeScaled.cx,
                  sizeScaled.cy,
                  0,
                  0,
                  m_pImage->columns(),
                  m_pImage->rows(),
                  pPixels,
                  (BITMAPINFO *)&bmi,
                  DIB_RGB_COLORS,
                  SRCCOPY);
#elif QuantumDepth == 16
# error "This code only works when ImageMagick is compiled with 8-bit Quantum size"
#endif

  }
}

//-------------------------------------------------------------------
// Scale()
// Proportionally scale the first rectangle so that it occupies the
// maximum area of the target rectangle
//-------------------------------------------------------------------

CSize CNtMagickView::Scale(CSize sizeSrc, CSize sizeTgt)
{

  CSize size;

  // Obtain the scaling factor
  float fScale = ScaleFactor(FALSE,sizeSrc,sizeTgt);

  // Calculate the size of the sized rectangle
  size.cx = ((float) sizeSrc.cx * fScale) + 0.5;
  size.cy = ((float) sizeSrc.cy * fScale) + 0.5;

  // Ensure roundings errors don't make scaled rectangle too large
  if (size.cx > sizeTgt.cx)
  {
    size.cx = sizeTgt.cx;
  }

  if (size.cy > sizeTgt.cy)
  {
    size.cy = sizeTgt.cy;
  }

  // Return dimensions of the rectangle
  return size;
}

//-------------------------------------------------------------------
// ScaleFactor()
// Return the ratio required to proprtionally scale a rectangle
//-------------------------------------------------------------------

float CNtMagickView::ScaleFactor(BOOL bAllowZoom, CSize sizeSrc, CSize sizeTgt)
{
  float fScale = 1;

  // If the image is smaller than the target don't scale zoom

  if ((sizeSrc.cx < sizeTgt.cx)&& (sizeSrc.cy < sizeTgt.cy))
  {
    if (bAllowZoom)
    {
      float fScaleW = (float) sizeSrc.cx / (float) sizeTgt.cx;
      float fScaleH = (float)  sizeSrc.cy / (float) sizeTgt.cy;

      if (fScaleH < fScaleW)
      {
        fScale = fScaleH;
      }
      else
      {
        fScale = fScaleW;
      }
    }
    else
    {
      fScale = 1;
    }
  }

  // If the image is equal to the target don't scale

  if ((sizeSrc.cx == sizeTgt.cx) && (sizeSrc.cy == sizeTgt.cy))
  {
    fScale = 1;
  }

  // If the current image is wider than the target image,
  // reduce the size of the image whilst preserving the aspect
  // ratio of the original image.

  if ((sizeSrc.cx > sizeTgt.cx) && (sizeSrc.cy <= sizeTgt.cy))
  {
    fScale = (float) sizeTgt.cx / (float) sizeSrc.cx;
  }

  // If the current image is taller than the target image,
  // reduce the size of the image whilst preserving the aspect
  // ratio of the original image.

  if (((int)sizeSrc.cx <= sizeTgt.cx) && ((int)sizeSrc.cy > sizeTgt.cy))
    {
      fScale = (float) sizeTgt.cy / (float) sizeSrc.cy;
    }

  // If the Jpeg image is wider and taller than the client rectangle,
  // reduce the size of the image whilst preserving the aspect
  // ratio of the original image.

  if (((int)sizeSrc.cx > sizeTgt.cx) && ((int)sizeSrc.cy > sizeTgt.cy))
  {
    float fScaleW = (float) sizeTgt.cx / (float) sizeSrc.cx;
    float fScaleH = (float) sizeTgt.cy / (float) sizeSrc.cy;

    if (fScaleH < fScaleW)
      {
        fScale = fScaleH;
      }
    else
      {
        fScale = fScaleW;
      }
  }

  return fScale;
}

//-----------------------------------------------------------------------
//  UpdateUI()
//  User interface controls are only active when an image has been read.
//-----------------------------------------------------------------------

void CNtMagickView::UpdateUI(CCmdUI *pCmdUI)
{
  if (m_pImage == NULL)
  {
    pCmdUI->Enable(FALSE);
  }
  else
  {
    pCmdUI->Enable(TRUE);
  }
}

//-----------------------------------------------------------------------
//  OnImageFlipHorizontal()
//  Flip horizontally
//-----------------------------------------------------------------------

void CNtMagickView::OnImageFlipHorizontal()
{
  if (m_pImage == NULL)
  {
    return;
  }

  try
  {
    m_pImage->flop();
  }

  catch(Exception e)
  {
    DoDisplayError("Flop",e.what());
    return;
  }

  catch(exception e)
  {
    DoDisplayError("Flop",e.what());
    return;
  }

  DoDisplayImage();
}

void CNtMagickView::OnUpdateImageFlipHorizontal(CCmdUI* pCmdUI)
{
  UpdateUI(pCmdUI);
}

//-----------------------------------------------------------------------
//  OnImageFlipVertical()
//  Flip vertically
//-----------------------------------------------------------------------

void CNtMagickView::OnImageFlipVertical()
{
  if (m_pImage == NULL)
  {
    return;
  }

  try
  {
    m_pImage->flip();
  }

  catch(Exception e)
  {
    DoDisplayError("Flip",e.what());
    return;
  }

  catch(exception e)
  {
    DoDisplayError("Flip",e.what());
    return;
  }

  DoDisplayImage();
}

void CNtMagickView::OnUpdateImageFlipVertical(CCmdUI* pCmdUI)
{
  UpdateUI(pCmdUI);
}

//-----------------------------------------------------------------------
//  OnImageRotate180()
//  Rotate image 180 degrees
//-----------------------------------------------------------------------

void CNtMagickView::OnImageRotate180()
{
  if (m_pImage == NULL)
  {
    return;
  }

  try
  {
    m_pImage->rotate(180);
  }

  catch(Exception e)
  {
    DoDisplayError("Rotate 180",e.what());
    return;
  }

  catch(exception e)
  {
    DoDisplayError("Rotate 180",e.what());
    return;
  }

  DoDisplayImage();
}

void CNtMagickView::OnUpdateImageRotate180(CCmdUI* pCmdUI)
{
  UpdateUI(pCmdUI);
}

//-----------------------------------------------------------------------
//  OnImageRotate90()
//  Rotate image 90 degrees clockwise
//-----------------------------------------------------------------------

void CNtMagickView::OnImageRotate90()
{
  if (m_pImage == NULL)
  {
    return;
  }

  try
  {
    m_pImage->rotate(90);
  }

  catch(Exception e)
  {
    DoDisplayError("Rotate 90",e.what());
    return;
  }

  catch(exception e)
  {
    DoDisplayError("Rotate 90",e.what());
    return;
  }

  DoDisplayImage();
}

void CNtMagickView::OnUpdateImageRotate90(CCmdUI* pCmdUI)
{
  UpdateUI(pCmdUI);
}

//-----------------------------------------------------------------------
//  OnImageRotate90ccw()
//  Rotate image 90 degrees clockwise
//-----------------------------------------------------------------------

void CNtMagickView::OnImageRotate90ccw()
{
  if (m_pImage == NULL)
  {
    return;
  }

  try
  {
    m_pImage->rotate(-90);
  }

  catch(Exception e)
  {
    DoDisplayError("Rotate -90",e.what());
    return;
  }

  catch(exception e)
  {
    DoDisplayError("Rotate -90",e.what());
    return;
  }

  DoDisplayImage();
}

void CNtMagickView::OnUpdateImageRotate90ccw(CCmdUI* pCmdUI)
{
  UpdateUI(pCmdUI);
}

//-----------------------------------------------------------------------
//  OnFileClear()
//  Clear image
//-----------------------------------------------------------------------

void CNtMagickView::OnFileClear()
{
  if (m_pImage == NULL)
  {
    return;
  }

  CRect rectClient;
  CDC * pDC;

  // Remove image and clear client area
  delete m_pImage;
  m_pImage = NULL;
  pDC = GetDC();
  GetClientRect(rectClient);
  if (pDC != NULL)
  {
    pDC->FillSolidRect(rectClient,pDC->GetBkColor());
  }
}

void CNtMagickView::OnUpdateFileClear(CCmdUI* pCmdUI)
{
  UpdateUI(pCmdUI);
}
