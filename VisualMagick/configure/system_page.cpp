// system_page.cpp : implementation file
//

#include "stdafx.h"
#include "configure.h"
#include "system_page.h"

#include "configure_wizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


string get_project_name(int project, int runtime,
                        string root, string prefix, string name);
extern BOOL visualStudio7;


/////////////////////////////////////////////////////////////////////////////
// CSystemPage property page

IMPLEMENT_DYNCREATE(CSystemPage, CPropertyPage)

CSystemPage::CSystemPage(CConfigureWizard *IniWizard): CPropertyPage(CSystemPage::IDD)
{
	//{{AFX_DATA_INIT(CSystemPage)
	m_tempDebug = _T("");
	m_tempRelease = _T("");
	m_outputBin = _T("");
	m_outputLib = _T("");
	//}}AFX_DATA_INIT

        m_Wizard = IniWizard;
}


CSystemPage::~CSystemPage()
{
  m_Wizard = NULL;
}


void CSystemPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemPage)
	DDX_Text(pDX, IDC_TEMP_DEBUG, m_tempDebug);
	DDX_Text(pDX, IDC_TEMP_RELEASE, m_tempRelease);
	DDX_Text(pDX, IDC_OUTPUT_BIN, m_outputBin);
	DDX_Text(pDX, IDC_OUTPUT_LIB, m_outputLib);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSystemPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSystemPage)
	ON_BN_CLICKED(IDC_TEMP_RELEASE_BROWSE, OnTempReleaseBrowse)
	ON_BN_CLICKED(IDC_TEMP_DEBUG_BROWSE, OnTempDebugBrowse)
	ON_BN_CLICKED(IDC_OUTPUT_BIN_BROWSE, OnOutputBinBrowse)
	ON_BN_CLICKED(IDC_OUTPUT_LIB_BROWSE, OnOutputLibBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemPage message handlers

BOOL CSystemPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSystemPage::OnTempReleaseBrowse() 
{
	char str[1024];

	UpdateData(TRUE);
	if (BrowseForFolder(m_hWnd, "Select Release directory", str))
  {
    lstrcat(str, "\\");
	  m_tempRelease = str;
  }
	UpdateData(FALSE);
}


void CSystemPage::OnTempDebugBrowse() 
{
	char str[1024];

	UpdateData(TRUE);
	if (BrowseForFolder(m_hWnd, "Select Debug directory", str))
  {
    lstrcat(str, "\\");
	  m_tempDebug = str;
  }
	UpdateData(FALSE);
}

void CSystemPage::OnOutputBinBrowse() 
{
	char str[1024];

	UpdateData(TRUE);
	if (BrowseForFolder(m_hWnd, "Select Bin directory", str))
  {
    lstrcat(str, "\\");
	  m_outputBin = str;
  }
	UpdateData(FALSE);
}


void CSystemPage::OnOutputLibBrowse() 
{
	char str[1024];

	UpdateData(TRUE);
	if (BrowseForFolder(m_hWnd, "Select Lib directory", str))
  {
    lstrcat(str, "\\");
	  m_outputLib = str;
  }
	UpdateData(FALSE);
}


/**This function reads one line from the text file and store it to the string.*/
string & ReadLine(FILE *f, string & pstr, long MaxStrSize)
{
  char c;

  pstr="";
  while (!feof(f))
     {
     c = getc(f);
     if (c == '\n') break;
     if (c == '\r') continue;
     if ((unsigned char)c == 0xFF)
	if(feof(f)) break;	//Fix the situation, when 0xFF is read before eof

     pstr += c;
     if(MaxStrSize>0)
        if(--MaxStrSize==0) break;
     }

 return(pstr);
}



BOOL CSystemPage::OnSetActive() 
{
  CPropertySheet* psheet = (CPropertySheet*) GetParent();
  psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

  //////////////////////////////////////////////////////
  // Here should be filled paths if found inside already generated .vcproj
  if(m_Wizard)
  {
    int projectType = m_Wizard->m_Page2.m_projectType;
    visualStudio7 = m_Wizard->m_Page2.m_visualStudio7;
    
    string root = "..\\utilities";
    string prefix = "UTIL_gm";
    string name;

    string theprojectname = get_project_name(EXEPROJECT, projectType, root, prefix, name);

    FILE *F = fopen(theprojectname.c_str(),"rb");
    if(F)
    {
      string line;
      bool is_release = false;
      bool is_debug = false;

      while(!feof(F))
      {
        ReadLine(F,line,1024);
        if(line.length()<=0) continue;
      
        const char *Pos;

        if(visualStudio7)
        {
          Pos = strstr(line.c_str(), "Name=\"Release|");
          if(Pos)
          {
            is_release = true;
            is_debug = false;
            continue;
          }
          Pos = strstr(line.c_str(), "Name=\"Debug|");
          if(Pos)
          {
            is_release = false;
            is_debug = true;
            continue;
          }

        
          const char *TMP_DIR = "IntermediateDirectory=\"";
          Pos = strstr(line.c_str(), TMP_DIR);
          if(Pos!=NULL)
          {
            const char *Pos2 = strstr(Pos+strlen(TMP_DIR), "\\UTIL_gm\"");
            if(Pos2!=NULL)
            {
              line[Pos2 - line.c_str() + 1] = 0;

              if(is_debug)
              {
                UpdateData(TRUE);
                m_tempDebug = Pos + strlen(TMP_DIR);
                UpdateData(FALSE);
              }
              if(is_release)
              {
                UpdateData(TRUE);
                m_tempRelease = Pos + strlen(TMP_DIR);
                UpdateData(FALSE);
              }
            }
            continue;
          }

          const char *OUT_DIR = "OutputDirectory=\"";
          Pos = strstr(line.c_str(), OUT_DIR);
          if(Pos!=NULL)
          {
            const char *Pos2 = strstr(Pos+strlen(OUT_DIR), "\"");
            if(Pos2!=NULL)
            {
              line[Pos2 - line.c_str() + 1] = 0;

              UpdateData(TRUE);
              m_outputBin = Pos + strlen(OUT_DIR);
              UpdateData(FALSE);
            }
            continue;
          }
        }
         
        else
        {
          Pos = strstr(line.c_str(), " - Win32 Release");
          if(Pos)
          {
            is_release = true;
            is_debug = false;
            continue;
          }
          Pos = strstr(line.c_str(), " - Win32 Debug");
          if(Pos)
          {
            is_release = false;
            is_debug = true;
            continue;
          }

          const char *OUT_DIR2 = "PROP Output_Dir \"";
          Pos = strstr(line.c_str(), OUT_DIR2);
          if(Pos!=NULL)
          {
            const char *Pos2 = strstr(Pos+strlen(OUT_DIR2), "\"");
            if(Pos2!=NULL)
            {
              line[Pos2 - line.c_str() + 1] = 0;

              UpdateData(TRUE);
              m_outputBin = Pos + strlen(OUT_DIR2);
              UpdateData(FALSE);
            }
            continue;
          }


          const char *TMP_DIR6 = "PROP Intermediate_Dir \"";
          Pos = strstr(line.c_str(), TMP_DIR6);
          if(Pos!=NULL)
          {
            const char *Pos2 = strstr(Pos+strlen(TMP_DIR6), "\\UTIL_gm\"");
            if(Pos2!=NULL)
            {
              line[Pos2 - line.c_str() + 1] = 0;

              if(is_debug)
              {
                UpdateData(TRUE);
                m_tempDebug = Pos + strlen(TMP_DIR6);
                UpdateData(FALSE);
              }
              if(is_release)
              {
                UpdateData(TRUE);
                m_tempRelease = Pos + strlen(TMP_DIR6);
                UpdateData(FALSE);
              }
            }
            continue;          
          }
 
        }
 
      }
      fclose(F);
    }

  }
  //////////////////////////////////////////////////////

	
  return CPropertyPage::OnSetActive();
}

