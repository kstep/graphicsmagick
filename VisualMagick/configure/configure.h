// Configure.h : main header file for the CONFIGURE application
//

#if !defined(AFX_CONFIGURE_H__38147984_A99A_11D3_8F4C_002018383984__INCLUDED_)
#define AFX_CONFIGURE_H__38147984_A99A_11D3_8F4C_002018383984__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <fstream.h>
#include <string>
#include <list>

/////////////////////////////////////////////////////////////////////////////
// CConfigureApp:
// See Configure.cpp for the implementation of this class
//

class CConfigureApp : public CWinApp
{
public:
	CConfigureApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigureApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CConfigureApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void write_dsw_start(ofstream &dsw);
	void write_dsw_end(ofstream &dsw);
	void begin_project(ofstream &dsw,
          const char *name, const char *filename);
	void end_project(ofstream &dsw);
	void add_project_dependency(ofstream &dsw,
          const char *dep_name);
  void process_project_type(ofstream &dsw,
          const char *root, const int runtime,
          const char *stype, const int btype);
  void process_project_replacements(ofstream &dsw,
          const char *root, const char *stype);
	void write_lib_dsp(
    bool dll,
    int runtime,
    int project_type,
		std::string root,
		std::string search,
		std::string name,
		std::string prefix,
		std::string extn,
		std::list<std::string> &libs_list_shared,
		std::list<std::string> &libs_list_release,
		std::list<std::string> &libs_list_debug,
		std::list<std::string> &defines_list,
		std::list<std::string> &includes_list,
	  std::list<std::string> &source_list,
	  std::list<std::string> &exclude_list);
	void write_exe_dsp(
    int runtime,
    int project_type,
		std::string root,
		std::string search,
		std::string name,
		std::string prefix,
		std::string extn,
		std::list<std::string> &libs_list_shared,
		std::list<std::string> &libs_list_release,
		std::list<std::string> &libs_list_debug,
		std::list<std::string> &defines_list,
		std::list<std::string> &includes_list,
	  std::list<std::string> &source_list,
	  std::list<std::string> &exclude_list);
	void begin_group(ofstream &dsp, const char *group_name);
	void end_group(ofstream &dsp);
	void add_file(ofstream &dsp, const char *filename);
	void generate_dir(ofstream &dsp, const char *dir,
          const char *specS, int nestinglevel, int project_type,
          std::list<std::string> &exclude_list);
  void process_utility(ofstream &dsw,
          const char *root, const char *filename,
          int runtime, int project_type);
  void process_library(ofstream &dsw,
          const char *root, const char *filename,
          int runtime, int project_type);
  void process_3rd_party_library(ofstream &dsw,
          const char *root, const char *filename,
          int runtime, int project_type);
  void process_module(ofstream &dsw,
          const char *root, const char *filename,
          int runtime, int project_type);
  void process_one_folder(ofstream &dsw,
          const char *root, WIN32_FIND_DATA	&data,
          int project_type, int projectType);
  void generate_dependencies(ofstream &dsw, bool gen_cpp, bool gen_util);
  bool is_project_type(const char *root, const int project_type);
};

BOOL BrowseForFolder(HWND hOwner, char* szTitle, char* szRetval);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURE_H__38147984_A99A_11D3_8F4C_002018383984__INCLUDED_)
