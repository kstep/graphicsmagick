// welcome_page.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "welcome_page.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWelcomePage, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// CWelcomePage property page

CWelcomePage::CWelcomePage() : CPropertyPage(CWelcomePage::IDD)
{
	//{{AFX_DATA_INIT(CWelcomePage)
	m_welcomeText = _T("");
	//}}AFX_DATA_INIT
}

CWelcomePage::~CWelcomePage()
{
}

void CWelcomePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWelcomePage)
	DDX_Text(pDX, IDC_WELCOME_TEXT, m_welcomeText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWelcomePage, CPropertyPage)
	//{{AFX_MSG_MAP(CWelcomePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CWelcomePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_welcomeText  = "Welcome!\n\n";
	
	m_welcomeText += "This is the VisualMagick configuration wizard. It will help you set up";
	m_welcomeText += " your build environment and customize the build process for your needs.";
	m_welcomeText += " It's purpose is to creat Visual C++ 6.0 compatible project (DSP) and";
	m_welcomeText += " workspace files.\n\n";

	m_welcomeText += "When the build environment has been created you can build the system in ";
	m_welcomeText += "either the Visual Studio IDE, or run it from within this wizard at your option.\n\n";

	m_welcomeText += "Good Luck from - The ImageMagick Studio Development Team";

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWelcomePage::OnSetActive() 
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();
	psheet->SetWizardButtons(PSWIZB_NEXT);
	
	return CPropertyPage::OnSetActive();
}
