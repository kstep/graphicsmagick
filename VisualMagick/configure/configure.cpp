// Configure.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Configure.h"
#include "configure_wizard.h"
#include "WaitDlg.h"
#include "CStringEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
ofstream debuglog;
#endif

const char *MODULE_PREFIX = "IM_MOD_";

// /base:"0x66200000"
unsigned long dllbaselocation = 0x67000000L;

/////////////////////////////////////////////////////////////////////////////
// CConfigureApp

BEGIN_MESSAGE_MAP(CConfigureApp, CWinApp)
	//{{AFX_MSG_MAP(CConfigureApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigureApp construction

CConfigureApp::CConfigureApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CConfigureApp object

CConfigureApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigureApp initialization

enum {MULTITHREADEDDLL, SINGLETHREADEDSTATIC, MULTITHREADEDSTATIC, MULTITHREADEDSTATICDLL};
enum {DLLPROJECT, LIBPROJECT, EXEPROJECT};
enum {DISABLED, UTILITY, LIBRARY, STATICLIB, MODULE, ADD_ON, THIRDPARTY, PROJECT};

BOOL useX11Stubs = TRUE;
BOOL decorateFiles = FALSE;
BOOL optionalFiles = FALSE;
BOOL consoleMode = TRUE;
BOOL standaloneMode = TRUE;
BOOL absolutePaths = TRUE;
BOOL onebigdllMode = FALSE;
BOOL generateFontmap = FALSE;

CString release_loc;
CString debug_loc;
CString bin_loc;
CString lib_loc;

CString final_release_loc;
CString final_debug_loc;
CString final_bin_loc;
CString final_lib_loc;

std::list<std::string> libs_list_shared;
std::list<std::string> dependency_list;
std::list<std::string> standard_include;

//
// Get the full path to a file. The first argument is the
// relative path to the DSP directory within the tree. and
// the second argument is the relative path to the actual
// file or directory. All paths are relative to the place
// were configure runs from.
//
std::string get_full_path(std::string root,std::string part)
{
  if (!absolutePaths)
  {
#ifdef _DEBUG
    debuglog  << "get_full_path "
              << "root:" << root.c_str() << ","
              << "return part:" << part.c_str() << endl;
#endif
    return part;
  }
  else
  {
    char full[_MAX_PATH];
    std::string combined;

    combined = root + part;
    
    if( _fullpath( full, combined.c_str(), _MAX_PATH ) != NULL )
    {
#ifdef _DEBUG
      debuglog  << "get_full_path "
                << "combined:" << combined.c_str() << ","
                << "return full:" << full << endl;
#endif
      return full;
    }
    else
    {
#ifdef _DEBUG
      debuglog  << "get_full_path "
                << "root:" << root.c_str() << ","
                << "return part:" << part.c_str() << endl;
#endif
      return part;
    }
  }
}

std::string get_project_name(int project, int runtime,
  std::string root, std::string prefix, std::string name)
{
  std::string filename = root;
  filename += "\\";
	filename += prefix.c_str();
	filename += name.c_str();
  
  switch (project)
  {
    case DLLPROJECT:
    {
      switch(runtime)
      {
        case SINGLETHREADEDSTATIC:
	        filename += "_st_dll.dsp";  // should never happen
          break;
        case MULTITHREADEDSTATIC:
	        filename += "_xt_dll.dsp";  // should never happen
          break;
        case MULTITHREADEDSTATICDLL:
	        filename += "_mtdll_dll.dsp";  // should never happen
          break;
        default:
        case MULTITHREADEDDLL:
	        filename += "_mt_dll.dsp";
          break;
      }
      break;
    }
    case LIBPROJECT:
    {
      switch(runtime)
      {
        case SINGLETHREADEDSTATIC:
	        filename += "_st_lib.dsp";
          break;
        case MULTITHREADEDSTATIC:
	        filename += "_xt_lib.dsp";
          break;
        case MULTITHREADEDSTATICDLL:
	        filename += "_mtdll_lib.dsp";
          break;
        default:
        case MULTITHREADEDDLL:
	        filename += "_mt_lib.dsp";
          break;
      }
      break;
    }
    case EXEPROJECT:
    {
      switch(runtime)
      {
        case SINGLETHREADEDSTATIC:
	        filename += "_st_exe.dsp";
          break;
        case MULTITHREADEDSTATIC:
	        filename += "_xt_exe.dsp";
          break;
        case MULTITHREADEDSTATICDLL:
	        filename += "_mtdll_exe.dsp";
          break;
        default:
        case MULTITHREADEDDLL:
	        filename += "_mt_exe.dsp";
          break;
      }
    }
    break;
  }
  return filename;
}

void CConfigureApp::generate_dependencies(
    ofstream &dsw, bool add_cpp_depends, bool add_util_depends)
{
  CStringEx strDepends;
	for (
		std::list<std::string>::iterator it1a = dependency_list.begin();
		it1a != dependency_list.end();
		it1a++)
	{
    strDepends = (*it1a).c_str();
    if (strDepends.FindNoCase("CORE_%szlib",0) == 0)
    {
      strDepends.Format((*it1a).c_str(),"","");
		  add_project_dependency(dsw, strDepends );
    }
	}

	for (
		std::list<std::string>::iterator it1b = dependency_list.begin();
		it1b != dependency_list.end();
		it1b++)
	{
    strDepends = (*it1b).c_str();
    if (strDepends.FindNoCase("CORE_%sbzlib",0) == 0)
    {
      strDepends.Format((*it1b).c_str(),"","");
		  add_project_dependency(dsw, strDepends );
    }
	}

	for (
		std::list<std::string>::iterator it1c = dependency_list.begin();
		it1c != dependency_list.end();
		it1c++)
	{
    strDepends = (*it1c).c_str();
    if (strDepends.FindNoCase("CORE_%sjpeg",0) == 0)
    {
      strDepends.Format((*it1c).c_str(),"","");
		  add_project_dependency(dsw, strDepends );
    }
	}

	for (
		std::list<std::string>::iterator it1 = dependency_list.begin();
		it1 != dependency_list.end();
		it1++)
	{
    strDepends = (*it1).c_str();
    if (strDepends.FindNoCase("CORE_%szlib",0) == 0)
      continue;
    if (strDepends.FindNoCase("CORE_%sbzlib",0) == 0)
      continue;
    if (strDepends.FindNoCase("CORE_%sjpeg",0) == 0)
      continue;
    if (strDepends.FindNoCase("LIBR_",0) == 0)
    {
      strDepends.Format((*it1).c_str(),"","");
		  add_project_dependency(dsw, strDepends );
    }
	}
	for (
		std::list<std::string>::iterator it2 = dependency_list.begin();
		it2 != dependency_list.end();
		it2++)
	{
    strDepends = (*it2).c_str();
    if (strDepends.FindNoCase("CORE_",0) == 0)
    {
      if (strDepends.FindNoCase("CORE_%smagick",0) == 0)
        continue;
      strDepends.Format((*it2).c_str(),"","");
		  add_project_dependency(dsw, strDepends );
    }
	}
	for (
		std::list<std::string>::iterator it3 = dependency_list.begin();
		it3 != dependency_list.end();
		it3++)
	{
    strDepends = (*it3).c_str();
    if (strDepends.Find("CORE_%smagick",0) == 0)
    {
      strDepends.Format((*it3).c_str(),"","");
		  add_project_dependency(dsw, strDepends );
    }
	}
  if (add_cpp_depends)
  {
	  for (
		  std::list<std::string>::iterator it4 = dependency_list.begin();
		  it4 != dependency_list.end();
		  it4++)
	  {
      strDepends = (*it4).c_str();
      if (strDepends.Find("CORE_%sMagick",0) == 0)
      {
        strDepends.Format((*it4).c_str(),"","");
		    add_project_dependency(dsw, strDepends );
      }
	  }
  }
	for (
		std::list<std::string>::iterator it5 = dependency_list.begin();
		it5 != dependency_list.end();
		it5++)
	{
    strDepends = (*it5).c_str();
    if (strDepends.FindNoCase(MODULE_PREFIX,0) == 0)
    {
      strDepends.Format((*it5).c_str(),"","");
		  add_project_dependency(dsw, strDepends );
    }
	}
  if (add_util_depends)
  {
	  for (
		  std::list<std::string>::iterator it6 = dependency_list.begin();
		  it6 != dependency_list.end();
		  it6++)
	  {
      strDepends = (*it6).c_str();
      if (strDepends.FindNoCase("UTIL_",0) == 0)
      {
        strDepends.Format((*it6).c_str(),"","");
		    add_project_dependency(dsw, strDepends );
      }
	  }
  }
}

static bool doesDirExist(const char *name)
{
  // check to see if the path exists
  std::string libpath;
	WIN32_FIND_DATA	libdata;
	HANDLE libhandle = FindFirstFile(name, &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
    FindClose(libhandle);
		return ((libdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
            FILE_ATTRIBUTE_DIRECTORY);
  }
  return false;
}

static void add_includes(std::list<std::string> &includes_list,
  std::string &libpath, int levels)
{
	WIN32_FIND_DATA	libdata;
	HANDLE libhandle = FindFirstFile(libpath.c_str(), &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
    FindClose(libhandle);
		if ((libdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
    {
      std::string ipath = "..\\";
      for (int i=0; i<levels; i++)
        ipath += "..\\";
      ipath += libpath;
		  includes_list.push_back(ipath);

      ipath = libpath;
      ipath += "\\*";
      libhandle = FindFirstFile(ipath.c_str(), &libdata);
	    if (libhandle != INVALID_HANDLE_VALUE)
      {
	      static const char *exclude_from_build[] =
	      {
		      ".",
		      "..",
		      "CVS",
		      NULL
	      };

	      do
	      {
		      bool skip = false;

		      for (int i=0; exclude_from_build[i] != NULL; i++)
			      if (stricmp(libdata.cFileName, exclude_from_build[i]) == 0)
              skip = true;

		      if (skip) continue;

		      if ((libdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
          {
            ipath = "..\\";
            ipath += libpath;
            ipath += "\\";
            ipath += libdata.cFileName;
            if (doesDirExist(ipath.c_str()))
            {
              ipath = libpath;
              ipath += "\\";
              ipath += libdata.cFileName;
			        add_includes(includes_list, ipath, levels);
            }
          }
	      } while (FindNextFile(libhandle, &libdata));
        FindClose(libhandle);
      }
    }
  }
}

static bool process_one_entry(const char *entry, int nLinesRead,
  std::list<std::string> &defines_list,
  std::list<std::string> &includes_list,
  std::list<std::string> &source_list,
  std::list<std::string> &exclude_list,
  std::list<std::string> &lib_release_list,
  std::list<std::string> &lib_debug_list,
  int runtime)
{
  CStringEx sTempString = entry;
  // check for a valid file by looking for the magick
  // sequence in the first four characters of the file.
  if ((nLinesRead == 0) &&
	  (sTempString.Left(4) != "#ECF"))
		  return true;
  // skip any blank lines
  if (sTempString.GetLength() <= 0)
	  return true;
  // skip over any comments first - we allow three types
  if (sTempString[0] == _T('#'))
	  return true;
  if (sTempString.Left(3) == TEXT("rem"))
	  return true;
  if (sTempString.Left(3) == TEXT(";"))
	  return true;
  // now see if the line will parse as a name=value pair
  CString sName = sTempString.GetField(_T(" = "),0);
  CString sValue = sTempString.GetField(_T(" = "),1);
  if (!sName.IsEmpty() && !sValue.IsEmpty())
  {
    std::string temp;
    temp = sValue;
    if (sName.CompareNoCase(_T("DEFINE")) == 0)
	    defines_list.push_back(temp);
    if (sName.CompareNoCase(_T("INCLUDE")) == 0)
	    includes_list.push_back(temp);
    if (sName.CompareNoCase(_T("SOURCE")) == 0)
	    source_list.push_back(temp);
    if (sName.CompareNoCase(_T("EXCLUDE")) == 0)
	    exclude_list.push_back(temp);
    if (sName.CompareNoCase(_T("LIBRARY")) == 0)
    {
	    lib_release_list.push_back(temp);
	    lib_debug_list.push_back(temp);
    }
    if (sName.CompareNoCase(_T("LIBRELEASE")) == 0)
    {
	    lib_release_list.push_back(temp);
    }
    if (sName.CompareNoCase(_T("LIBDEBUG")) == 0)
    {
	    lib_debug_list.push_back(temp);
    }
    if (sName.CompareNoCase(_T("SUBSYSTEM")) == 0)
    {
      if (sValue.CompareNoCase(_T("CONSOLE")) == 0)
        consoleMode = TRUE;
      if (sValue.CompareNoCase(_T("WINDOWS")) == 0)
        consoleMode = FALSE;
    }
    switch(runtime)
    {
      default:
      case SINGLETHREADEDSTATIC:
      case MULTITHREADEDSTATIC:
      case MULTITHREADEDSTATICDLL:
        if (sName.CompareNoCase(_T("DEFINESTATIC")) == 0)
	        defines_list.push_back(temp);
        break;
      case MULTITHREADEDDLL:
        if (sName.CompareNoCase(_T("DEFINEDLL")) == 0)
	        defines_list.push_back(temp);
        break;
    }
  }
  return true;
}

int load_environment_file( const char *inputfile,
  std::list<std::string> &defines_list,
  std::list<std::string> &includes_list,
  std::list<std::string> &source_list,
  std::list<std::string> &exclude_list,
  std::list<std::string> &lib_release_list,
  std::list<std::string> &lib_debug_list,
  int runtime)
{
  char szBuf[2048];
  int nLinesRead = 0;
  ifstream inpStream;
  inpStream.open( inputfile, ios::in | ios::nocreate );
  if ( inpStream ) {
    for (;;) {
      inpStream.getline( szBuf, 2047, '\n' );
      if ( inpStream.eof() ) {
        // last line may contain text also
        // (if it's not terminated with '\n' EOF is returned)
        return process_one_entry( szBuf, nLinesRead,
                  defines_list, includes_list, source_list, exclude_list,
                    lib_release_list, lib_debug_list,
                      runtime );
      }
      if ( !inpStream.good() ||
        !process_one_entry( szBuf, nLinesRead,
            defines_list, includes_list, source_list, exclude_list,
              lib_release_list, lib_debug_list,
                runtime) )
        return false;      
      nLinesRead++;
    }
  }
  inpStream.close();
  inpStream.clear();
  return true;
}

void CConfigureApp::process_utility(ofstream &dsw,
  const char *root, const char *filename, int runtime, int project_type)
{
	CStringEx basename = filename;
  std::string name = basename.GetField('.',0);
  std::string extn = basename.GetField('.',1);
  std::string prefix = "UTIL_";
  std::string staging = root;
  int levels;

	std::list<std::string> libs_list_release;
	std::list<std::string> libs_list_debug;
	std::list<std::string> includes_list;
  std::list<std::string> defines_list;
  std::list<std::string> source_list;
  std::list<std::string> exclude_list;

  std::string envpath;
  std::string search;
  envpath = staging;
  if (project_type == UTILITY)
  {
    envpath += "\\UTILITY.txt";
	  search = "";
    consoleMode = TRUE; /* set the default */
    load_environment_file(envpath.c_str(),
      defines_list, includes_list, source_list, exclude_list,
        libs_list_release,libs_list_debug,
          runtime);
  }
  else
  {
    envpath += "\\PROJECT.txt";
    search = "*";
    consoleMode = FALSE; /* set the default */
    load_environment_file(envpath.c_str(),
      defines_list, includes_list, source_list, exclude_list,
        libs_list_release,libs_list_debug,
          runtime);
  }

	for (std::list<std::string>::iterator it = exclude_list.begin();
			it != exclude_list.end();
			  it++)
	{
		if (stricmp(filename, (*it).c_str()) == 0)
      return;
	}

  if (runtime == MULTITHREADEDDLL)
	{
		defines_list.push_back("_DLL");
		defines_list.push_back("_MAGICKMOD_");
  }
  else
  {
		defines_list.push_back("_LIB");
  }

  CStringEx strDepends = staging.c_str();
  levels = strDepends.GetFieldCount('\\');
  std::string extra;

  for (int i=0; i<levels; i++)
    extra += "..\\";
	includes_list.push_back(extra);

  extra = "..\\magick";
  add_includes(includes_list, extra, levels-2);

  if (strDepends.FindNoCase("\\SDL") >= 0)
  {
    extra = "..\\SDL";
    add_includes(includes_list, extra, levels-2);
  }
  if (strDepends.FindNoCase("..\\jp2",0) == 0)
  {
    extra = "..\\jp2";
    add_includes(includes_list, extra, levels-2);
  }
  if (strDepends.FindNoCase("..\\Magick++",0) == 0)
  {
    extra = "..\\Magick++";
    add_includes(includes_list, extra, levels-2);
  }

#ifdef _DEBUG
  debuglog  << "process_utility "
            << "runtime:" << runtime <<","
            << "project_type:" << project_type << ","
            << "staging:" << staging.c_str() << ","
            << "search:" << search.c_str() << ","
            << "name:" << name.c_str() << ","
            << "prefix:" << prefix.c_str() << ","
            << "extn:" << extn.c_str() << endl;
#endif

	write_exe_dsp(
    runtime,
		project_type,
    staging,
    search,
		name,
		prefix,
    extn,
		libs_list_shared,
		libs_list_release,
		libs_list_debug,
		defines_list,
    includes_list,
    source_list,
    exclude_list);

	dependency_list.push_back(prefix + "%s" + name + "%s");

  std::string project;
  std::string pname;
  pname = prefix + name;
  project = get_project_name(
    EXEPROJECT,runtime,staging.substr(1),prefix,name);
  switch (runtime)
  {
    case MULTITHREADEDSTATIC:
    case SINGLETHREADEDSTATIC:
    case MULTITHREADEDSTATICDLL:
	    begin_project(dsw, pname.c_str(), project.c_str());
      if (!standaloneMode)
        generate_dependencies(dsw,(extn.compare("cpp") == 0), false);
	    end_project(dsw);
      break;
    default:
    case MULTITHREADEDDLL:
	    begin_project(dsw, pname.c_str(), project.c_str());
      if (!standaloneMode)
      {
	      add_project_dependency(dsw, "CORE_magick");
        if (useX11Stubs)
	        add_project_dependency(dsw, "CORE_xlib");
        if (extn.compare("cpp") == 0)
        {
		      add_project_dependency(dsw, "CORE_Magick++");
        }
        CStringEx strDepends = staging.c_str();
        if (strDepends.Find("\\SDL") >= 0)
        {
		      add_project_dependency(dsw, "CORE_SDL");
        }
        if (strDepends.Find("..\\jp2",0) == 0)
        {
		      add_project_dependency(dsw, "CORE_jp2");
        }
      }
	    end_project(dsw);
      break;
  }
}

void CConfigureApp::process_library(ofstream &dsw,
    const char *root, const char *filename, int runtime, int project_type)
{
  bool dll = false;
	CStringEx basename = filename;
  std::string name = basename.GetField('.',0);
  std::string extn = basename.GetField('.',1);
  std::string prefix = "CORE_";
  std::string staging = root;
  int levels;

	std::list<std::string> libs_list_release;
	std::list<std::string> libs_list_debug;
	std::list<std::string> includes_list;
  std::list<std::string> defines_list;
  std::list<std::string> source_list;
  std::list<std::string> exclude_list;

  if (runtime == MULTITHREADEDDLL)
	{
    dll = true;
		defines_list.push_back("_DLL");
		defines_list.push_back("_MAGICKMOD_");
  }
  else
  {
    dll = false;
		defines_list.push_back("_LIB");
  }
  // For the static library case we force the system to build this
  // as a standard library instead of a DLL. Magick++ and some of
  // the other libraries are build this way for various reasons.
  if (project_type == STATICLIB)
    dll = false;

  CStringEx strDepends = staging.c_str();
  levels = strDepends.GetFieldCount('\\');

  std::string extra;
  extra = "..\\zlib";
  add_includes(includes_list, extra, levels-2);
  extra = "..\\jpeg";
  add_includes(includes_list, extra, levels-2);
  //extra = "..\\tiff\\libtiff";
  //add_includes(includes_list, extra, levels-2);
  extra = "..\\lcms\\src";
  add_includes(includes_list, extra, levels-2);
  extra = "..\\lcms\\source";
  add_includes(includes_list, extra, levels-2);
  extra = "..\\ttf\\include";
  add_includes(includes_list, extra, levels-2);
  extra = "..\\libxml\\include";
  add_includes(includes_list, extra, levels-2);
  add_includes(includes_list, staging, levels-2);

  std::string envpath;
  envpath = staging;
  if (project_type == LIBRARY)
  {
    envpath += "\\LIBRARY.txt";
    load_environment_file(envpath.c_str(),
      defines_list, includes_list, source_list, exclude_list,
        libs_list_release,libs_list_debug,
          runtime);
  }
  if (project_type == STATICLIB)
  {
    envpath += "\\STATICLIB.txt";
    load_environment_file(envpath.c_str(),
      defines_list, includes_list, source_list, exclude_list,
        libs_list_release,libs_list_debug,
          runtime);
  }

#ifdef _DEBUG
  debuglog  << "process_library "
            << "dll:" << dll << ","
            << "runtime:" << runtime <<","
            << "project_type:" << project_type << ","
            << "staging:" << staging.c_str() << ","
            << "search:" << "*,"
            << "name:" << name.c_str() << ","
            << "prefix:" << prefix.c_str() << endl;
#endif

	write_lib_dsp(
    dll,
    runtime, // multi-threaded
    project_type,
    staging,
		"*",
		name,
		prefix,
    extn,
		libs_list_shared,
		libs_list_release,
		libs_list_debug,
		defines_list,
    includes_list,
    source_list,
    exclude_list);

  std::string project;
  std::string pname;
  pname = prefix + name;
	dependency_list.push_back(prefix + "%s" + name + "%s");
	project = get_project_name(
    dll?DLLPROJECT:LIBPROJECT,runtime,staging.substr(1),prefix,name);
  if (dll && (runtime==MULTITHREADEDDLL))
  {
	  begin_project(dsw, pname.c_str(), project.c_str());
    if (name.compare("magick") == 0)
    {
      if (useX11Stubs)
	      add_project_dependency(dsw, "CORE_xlib");
	    //add_project_dependency(dsw, "CORE_tiff");
	    add_project_dependency(dsw, "CORE_jpeg");
	    add_project_dependency(dsw, "CORE_zlib");
	    add_project_dependency(dsw, "CORE_lcms");
	    add_project_dependency(dsw, "CORE_ttf");
	    add_project_dependency(dsw, "CORE_libxml");
    }
    if (name.compare("Magick++") == 0)
    {
	    add_project_dependency(dsw, "CORE_magick");
    }
    if (name.compare("SDL") == 0)
    {
	    add_project_dependency(dsw, "CORE_magick");
    }
    if (name.compare("hdf") == 0)
    {
	    add_project_dependency(dsw, "CORE_zlib");
    }
    if (name.compare("pdf") == 0)
    {
	    add_project_dependency(dsw, "CORE_tiff");
	    add_project_dependency(dsw, "CORE_zlib");
    }
    if (name.compare("ps2") == 0)
    {
	    add_project_dependency(dsw, "CORE_tiff");
	    add_project_dependency(dsw, "CORE_zlib");
    }
    if (name.compare("ps3") == 0)
    {
	    add_project_dependency(dsw, "CORE_tiff");
	    add_project_dependency(dsw, "CORE_zlib");
    }
    if (name.compare("png") == 0)
    {
	    add_project_dependency(dsw, "CORE_zlib");
    }
    if (name.compare("tiff") == 0)
    {
	    add_project_dependency(dsw, "CORE_jpeg");
	    add_project_dependency(dsw, "CORE_zlib");
    }
	  end_project(dsw);
  }
  else
  {
 	  begin_project(dsw, pname.c_str(), project.c_str());
	  end_project(dsw);
  }
}

void AddExtraLibs(std::string &name,std::string root,
      std::list<std::string> &libs_list_release,
      std::list<std::string> &libs_list_debug)
{
  std::string libpath;
  std::string extra_path;
  std::string extra_lvls;
	WIN32_FIND_DATA	libdata;
  bool gotRelease, gotDebug;

  gotRelease=gotDebug=false;

  CStringEx getcount = root.c_str();
  int levels = getcount.GetFieldCount('\\');
  for (int j=0; j<(levels-2); j++)
    extra_lvls += "..\\";
  extra_path = "..\\";
  extra_path += root;

  // look for release libs that exist and add them into the module
  libpath = extra_path;
  libpath += "\\";
  libpath += name;
  libpath += "Release.lib";
	HANDLE libhandle = FindFirstFile(libpath.c_str(), &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
    std::string extralibrary;
	  do
	  {
      extralibrary = extra_lvls;
      extralibrary += extra_path;
      extralibrary += "\\";
      extralibrary += libdata.cFileName;
		  libs_list_release.push_back(extralibrary);
      gotRelease=true;
	  } while (FindNextFile(libhandle, &libdata));
    FindClose(libhandle);
  }

  // look for debug libs that exist and add them into the module
  libpath = extra_path;
  libpath += "\\";
  libpath += name;
  libpath += "Debug.lib";
	libhandle = FindFirstFile(libpath.c_str(), &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
    std::string extralibrary;
	  do
	  {
      extralibrary = extra_lvls;
      extralibrary += extra_path;
      extralibrary += "\\";
      extralibrary += libdata.cFileName;
		  libs_list_debug.push_back(extralibrary);
      gotDebug=true;
	  } while (FindNextFile(libhandle, &libdata));
    FindClose(libhandle);
  }

  if (gotRelease==false || gotDebug==false)
  {
    // look for ANY libs that exist and add them into the module
    libpath = extra_path;
    libpath += "\\*.lib";
	  libhandle = FindFirstFile(libpath.c_str(), &libdata);
	  if (libhandle != INVALID_HANDLE_VALUE)
    {
      std::string extralibrary;
	    do
	    {
        extralibrary = extra_lvls;
        extralibrary += extra_path;
        extralibrary += "\\";
        extralibrary += libdata.cFileName;
		    libs_list_release.push_back(extralibrary);
		    libs_list_debug.push_back(extralibrary);
	    } while (FindNextFile(libhandle, &libdata));
      FindClose(libhandle);
    }
  }
}

void CConfigureApp::process_module(ofstream &dsw,
    const char *root, const char *filename,
      int runtime, int project_type)
{
  bool dll = false;
	CStringEx basename = filename;
  std::string name = basename.GetField('.',0);
  std::string extn = basename.GetField('.',1);
  std::string prefix;
  std::string staging = root;
  int i,levels;

  if (project_type == MODULE)
    prefix = MODULE_PREFIX;    

	std::list<std::string> libs_list_release;
	std::list<std::string> libs_list_debug;
	std::list<std::string> includes_list;
  std::list<std::string> defines_list;
  std::list<std::string> source_list;
  std::list<std::string> exclude_list;

  if (runtime == MULTITHREADEDDLL)
	{
    dll = true;
		defines_list.push_back("_DLL");
  }
  else
  {
    dll = false;
		defines_list.push_back("_LIB");
  }

  CStringEx strDepends = root;
  levels = strDepends.GetFieldCount('\\');

  std::string extra;
  for (i=0; i<levels; i++)
    extra += "..\\";
	includes_list.push_back(extra);
  extra = "..\\magick";
  add_includes(includes_list, extra, levels-2);

  if (name.compare("png") == 0)
  {
    extra = "..\\zlib";
    add_includes(includes_list, extra, levels-2);
  }
  if (name.compare("pdf") == 0)
  {
    extra = "..\\zlib";
    add_includes(includes_list, extra, levels-2);
    extra = "..\\tiff\\libtiff";
    add_includes(includes_list, extra, levels-2);
  }
  if (name.compare("ps2") == 0)
  {
    extra = "..\\zlib";
    add_includes(includes_list, extra, levels-2);
    extra = "..\\tiff\\libtiff";
    add_includes(includes_list, extra, levels-2);
  }
  if (name.compare("ps3") == 0)
  {
    extra = "..\\zlib";
    add_includes(includes_list, extra, levels-2);
    extra = "..\\tiff\\libtiff";
    add_includes(includes_list, extra, levels-2);
  }
  if (name.compare("miff") == 0)
  {
    extra = "..\\zlib";
    add_includes(includes_list, extra, levels-2);
    extra = "..\\bzlib";
    add_includes(includes_list, extra, levels-2);
  }
  if ((name.compare("svg") == 0) || (name.compare("url") == 0))
  {
    extra = "..\\libxml";
    add_includes(includes_list, extra, levels-2);
#ifdef USETHIS
    extra = "..\\autotrace";
    add_includes(includes_list, extra, levels-2);
#endif
  }
  if (name.compare("hdf") == 0)
  {
    extra = "..\\zlib";
    add_includes(includes_list, extra, levels-2);
  }

  // add include paths to this modules directory and any subdirs
  extra = "..\\";
  extra += name;
  add_includes(includes_list, extra, levels-2);

	WIN32_FIND_DATA	libdata;
  std::string libpath;
  std::string dependency;
  // look for xxxx.txt files to see if there is a third party
  // library or a core library that is needed for this module
  extra.erase();
  for (i=0; i<levels-1; i++)
    extra += "..\\";
  extra += name;
  libpath = extra;
  libpath += "\\LIBRARY.txt";
  HANDLE libhandle = FindFirstFile(libpath.c_str(), &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
    FindClose(libhandle);
    dependency = "CORE_";
    dependency += name;
  }
  libpath = extra;
  libpath += "\\STATICLIB.txt";
  libhandle = FindFirstFile(libpath.c_str(), &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
    FindClose(libhandle);
    dependency = "CORE_";
    dependency += name;
  }
  libpath = extra;
  libpath += "\\THIRDPARTY.txt";
  libhandle = FindFirstFile(libpath.c_str(), &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
    FindClose(libhandle);
    dependency = "LIBR_";
    dependency += name;
  }

  // This is a kind of kludge that is on its way to becoming a more
  // general facility. It looks for special libraries that are named
  // a specific way and adds these in if it finds them. If it does
  // not find the specially named ones, it add anything it finds in.
  AddExtraLibs(name,root,libs_list_release,libs_list_debug);

  std::string envpath;
  std::string search;
  envpath = root;
  if (project_type == MODULE)
  {
    envpath += "\\MODULE.txt";
    search = "";
    onebigdllMode = FALSE;
    load_environment_file(envpath.c_str(),
      defines_list, includes_list, source_list, exclude_list,
        libs_list_release,libs_list_debug,
          runtime);
  }
  if (project_type == ADD_ON)
  {
    envpath += "\\ADD_ON.txt";
    search = "*";
    onebigdllMode = TRUE;
    // we force this to always be built as a dll
    dll = true;
    load_environment_file(envpath.c_str(),
      defines_list, includes_list, source_list, exclude_list,
        libs_list_release,libs_list_debug,
          runtime);
  }

#ifdef _DEBUG
  debuglog  << "process_module "
            << "dll:" << dll << ","
            << "runtime:" << runtime <<","
            << "project_type:" << project_type << ","
            << "staging:" << staging.c_str() << ","
            << "search:" << search.c_str() << ","
            << "name:" << name.c_str() << ","
            << "prefix:" << prefix.c_str() << endl;
#endif

 	write_lib_dsp(
    dll,
    runtime, // multi-threaded
    project_type,
    root,
		search,
		name,
		prefix,
    extn,
		libs_list_shared,
		libs_list_release,
		libs_list_debug,
		defines_list,
    includes_list,
    source_list,
    exclude_list);

  std::string project;
  std::string pname;
  pname = prefix + name;
  if (project_type == MODULE)
	  dependency_list.push_back(prefix + "%s" + name + "%s");
  if (project_type == ADD_ON)
	  dependency_list.push_back(name);
	project = get_project_name(
    dll?DLLPROJECT:LIBPROJECT,runtime,staging.substr(1),prefix,name);
  if(runtime == MULTITHREADEDDLL)
  {
	  begin_project(dsw, pname.c_str(), project.c_str());
    {
	    add_project_dependency(dsw, "CORE_magick");
      if (dependency.length() > 0)
	      add_project_dependency(dsw, dependency.c_str());
      if (name.compare("label") == 0)
      {
        if (useX11Stubs)
		      add_project_dependency(dsw, "CORE_xlib");
      }
      if (name.compare("miff") == 0)
      {
	      add_project_dependency(dsw, "CORE_zlib");
		    add_project_dependency(dsw, "CORE_bzlib");
      }
      if (name.compare("png") == 0)
      {
	      add_project_dependency(dsw, "CORE_zlib");
      }
      if (name.compare("pdf") == 0)
      {
	      add_project_dependency(dsw, "CORE_tiff");
	      add_project_dependency(dsw, "CORE_zlib");
      }
      if (name.compare("ps") == 0)
      {
	      add_project_dependency(dsw, "CORE_tiff");
	      add_project_dependency(dsw, "CORE_zlib");
      }
      if (name.compare("ps2") == 0)
      {
	      add_project_dependency(dsw, "CORE_tiff");
	      add_project_dependency(dsw, "CORE_zlib");
      }
      if (name.compare("ps3") == 0)
      {
	      add_project_dependency(dsw, "CORE_tiff");
	      add_project_dependency(dsw, "CORE_zlib");
      }
      if (name.compare("x") == 0)
      {
        if (useX11Stubs)
		      add_project_dependency(dsw, "CORE_xlib");
      }
      if (name.compare("xwd") == 0)
      {
        if (useX11Stubs)
		      add_project_dependency(dsw, "CORE_xlib");
      }
      if ((name.compare("svg") == 0) || (name.compare("url") == 0))
      {
		    add_project_dependency(dsw, "CORE_libxml");
#ifdef USETHIS
        if (doesDirExist("..\\..\\autotrace"))
		      add_project_dependency(dsw, "CORE_autotrace");
#endif
      }
	    end_project(dsw);
    }
  }
  else
  {
	  begin_project(dsw, pname.c_str(), project.c_str());
	  end_project(dsw);
  }
}

void CConfigureApp::process_3rd_party_library(ofstream &dsw,
  const char *root, const char *filename, int runtime, int project_type)
{
  bool dll = false;
	CStringEx basename = filename;
  std::string name = basename.GetField('.',0);
  std::string prefix = "LIBR_";
  std::string staging = root;

	std::list<std::string> libs_list_release;
	std::list<std::string> libs_list_debug;
	std::list<std::string> includes_list;
  std::list<std::string> defines_list;

  if (runtime == MULTITHREADEDDLL)
	{
    dll = true;
		defines_list.push_back("_DLL");
		defines_list.push_back("_MAGICKMOD_");
  }
  else
  {
    dll = false;
		defines_list.push_back("_LIB");
  }

  std::string libpath;
  libpath = "..\\";
  libpath += staging;
  std::string dependency;
	WIN32_FIND_DATA	libdata;
  HANDLE libhandle;

	libhandle = FindFirstFile(libpath.c_str(), &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
    FindClose(libhandle);

    std::string project;
    std::string pname;
    pname = prefix + name;
		dependency_list.push_back(prefix + "%s" + name + "%s");
	  project = get_project_name(
      dll?DLLPROJECT:LIBPROJECT,runtime,staging.substr(1),prefix,name);
    libhandle = FindFirstFile(project.c_str(), &libdata);
	  if (libhandle != INVALID_HANDLE_VALUE)
    {
      FindClose(libhandle);

      switch(runtime)
      {
        case MULTITHREADEDDLL:
        {
	        begin_project(dsw, pname.c_str(), project.c_str());
          if (name.compare("png") == 0)
          {
	          add_project_dependency(dsw, "CORE_zlib");
          }
          if (name.compare("pdf") == 0)
          {
	          add_project_dependency(dsw, "CORE_tiff");
	          add_project_dependency(dsw, "CORE_zlib");
          }
          if (name.compare("ps2") == 0)
          {
	          add_project_dependency(dsw, "CORE_tiff");
	          add_project_dependency(dsw, "CORE_zlib");
          }
          if (name.compare("ps3") == 0)
          {
	          add_project_dependency(dsw, "CORE_tiff");
	          add_project_dependency(dsw, "CORE_zlib");
          }
          if (name.compare("hdf") == 0)
          {
	          add_project_dependency(dsw, "CORE_jpeg");
	          add_project_dependency(dsw, "CORE_zlib");
          }
          if (name.compare("tiff") == 0)
          {
	          add_project_dependency(dsw, "CORE_jpeg");
	          add_project_dependency(dsw, "CORE_zlib");
          }
	        end_project(dsw);
          break;
        }
        default:
        {
	        begin_project(dsw, pname.c_str(), project.c_str());
	        end_project(dsw);
          break;
        }  
      }
    }
  }
}

void CConfigureApp::process_one_folder(ofstream &dsw,
  const char *root, WIN32_FIND_DATA	&data,
  int project_type, int runtimeOption)
{
  CString subpath;

  switch (project_type)
  {
  case UTILITY:
    {
      if (!optionalFiles)
      {
        CStringEx strTest = root;
        if (strTest.FindNoCase("\\test") >= 0)
          return;
        if (strTest.FindNoCase("\\demo") >= 0)
          return;
        if (strTest.FindNoCase("\\contrib") >= 0)
          return;
      }
      // Look for any C files first and generate a project for each file that
      // is found.
      subpath = "..\\";
	    subpath += root;
      subpath += "\\*.c";
	    WIN32_FIND_DATA	subdata;
	    HANDLE subhandle = FindFirstFile(subpath, &subdata);
	    if (subhandle != INVALID_HANDLE_VALUE)
      {
	      do
	      {
          process_utility(dsw, root, subdata.cFileName, runtimeOption, project_type);
	      } while (FindNextFile(subhandle, &subdata));
        FindClose(subhandle);
      }

      // Look for any C++ files next and generate a project for each file that
      // is found.
      subpath = "..\\";
	    subpath += root;
      subpath += "\\*.cpp";
	    subhandle = FindFirstFile(subpath, &subdata);
	    if (subhandle != INVALID_HANDLE_VALUE)
      {
	      do
	      {
          process_utility(dsw, root, subdata.cFileName, runtimeOption, project_type);
	      } while (FindNextFile(subhandle, &subdata));
        FindClose(subhandle);
      }
    }
    break;
  case PROJECT:
    {
      // MFC does not link for single threaded or multithreaded dll
      if ((runtimeOption == SINGLETHREADEDSTATIC) ||
            (runtimeOption == MULTITHREADEDSTATICDLL))
              return;
      if (!optionalFiles)
      {
        CStringEx strTest = root;
        if (strTest.FindNoCase("\\test") >= 0)
          return;
        if (strTest.FindNoCase("\\demo") >= 0)
          return;
        if (strTest.FindNoCase("\\contrib") >= 0)
          return;
      }
      // check to see if there seems to be some source code in the
      // location specified and then process the entire folder as one
      // project if this test passes.
      subpath = "..\\";
	    subpath += root;
      subpath += "\\*.cpp";
	    WIN32_FIND_DATA	subdata;
	    HANDLE subhandle = FindFirstFile(subpath, &subdata);
	    if (subhandle != INVALID_HANDLE_VALUE)
      {
	      {
	        CStringEx thepath = root;
          int count = thepath.GetFieldCount('\\');
          std::string parent = thepath.GetField('\\',count-1);
          parent += ".cpp";
          process_utility(dsw, root, parent.c_str(), runtimeOption, project_type);
        }
        FindClose(subhandle);
      }
    }
    break;
  case LIBRARY:
  case STATICLIB:
    process_library(dsw, root, data.cFileName, runtimeOption, project_type);
    break;
  case ADD_ON:
  case MODULE:
    {
      bool bFoundSomething = false;
      // Look for any C files first and generate a project for each file that
      // is found.
      subpath = "..\\";
	    subpath += root;
      subpath += "\\*.cpp";
	    WIN32_FIND_DATA	subdata;
	    HANDLE subhandle = FindFirstFile(subpath, &subdata);
	    if (subhandle != INVALID_HANDLE_VALUE)
      {
        bFoundSomething = true;
	      do
	      {
          process_module(dsw, root, subdata.cFileName,
            runtimeOption, project_type);
          if (project_type == ADD_ON)
            break;
	      } while (FindNextFile(subhandle, &subdata));
        FindClose(subhandle);
      }

      if ((project_type == ADD_ON) && bFoundSomething)
        break;

      // Look for any C++ files next and generate a project for each file that
      // is found.
      subpath = "..\\";
	    subpath += root;
      subpath += "\\*.c";
	    subhandle = FindFirstFile(subpath, &subdata);
	    if (subhandle != INVALID_HANDLE_VALUE)
      {
        bFoundSomething = true;
	      do
	      {
          process_module(dsw, root, subdata.cFileName,
            runtimeOption, project_type);
          if (project_type == ADD_ON)
            break;
	      } while (FindNextFile(subhandle, &subdata));
        FindClose(subhandle);
      }
    }
    break;
  case THIRDPARTY:
    process_3rd_party_library(dsw, root, data.cFileName, runtimeOption, project_type);
    break;
  }
}

bool CConfigureApp::is_project_type(const char *root, const int project_type)
{
  HANDLE handle;
  WIN32_FIND_DATA	data;

  std::string searchpath = root;
  searchpath += "\\";
  switch (project_type)
  {
  case PROJECT:
    searchpath += "PROJECT.txt";
    break;
  case UTILITY:
    searchpath += "UTILITY.txt";
    break;
  case LIBRARY:
    searchpath += "LIBRARY.txt";
    break;
  case STATICLIB:
    searchpath += "STATICLIB.txt";
    break;
  case MODULE:
    searchpath += "MODULE.txt";
    break;
  case ADD_ON:
    searchpath += "ADD_ON.txt";
    break;
  case THIRDPARTY:
    searchpath += "THIRDPARTY.txt";
    break;
  default:
    return false;
  }
  handle = FindFirstFile(searchpath.c_str(), &data);
  FindClose(handle);
  if (handle != INVALID_HANDLE_VALUE)
    return true;
  return false;
}

void GetFileTime(LPFILETIME lpft)
{
  SYSTEMTIME st;
  FILETIME ft;

  GetLocalTime(&st);

  if (st.wMinute > 30)  
    st.wMinute = 30;
  else
    st.wMinute =  0;
  st.wSecond = 0;
  st.wMilliseconds = 0;

  SystemTimeToFileTime(&st,&ft);
  LocalFileTimeToFileTime(&ft,lpft);
}

BOOL SetFileTimeEx(LPCTSTR lpFileName)
{
  BOOL fSetFileTime;
  FILETIME FileTime;

  GetFileTime(&FileTime);
  HANDLE hFile = CreateFile(lpFileName,
                            GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            FILE_FLAG_BACKUP_SEMANTICS,
                            NULL);

  if (hFile == INVALID_HANDLE_VALUE)
    return(FALSE);

  fSetFileTime = SetFileTime(hFile,&FileTime,&FileTime,&FileTime);

  CloseHandle(hFile);

  return(fSetFileTime);

}

void CConfigureApp::process_project_replacements(ofstream &dsw,
      const char *root, const char *stype)
{
    int project_type = DISABLED;
    HANDLE tophandle;
	  WIN32_FIND_DATA	topdata;

    // Scan all top level directories and process the ones
    // that we are allowed to.
    std::string rootpath = root;
    rootpath += "\\*";
	  tophandle = FindFirstFile(rootpath.c_str(), &topdata);
	  do
	  {
	    if (tophandle == INVALID_HANDLE_VALUE)
        break;
		  if ((topdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
              FILE_ATTRIBUTE_DIRECTORY)
      {
        HANDLE handle;
	      WIN32_FIND_DATA	data;

        if (stricmp(topdata.cFileName,".") == 0)
          continue;
        if (stricmp(topdata.cFileName,"..") == 0)
          continue;

        // Now look to see if there is a directory by the same name
        // one level above our config level
        std::string searchpath;
        searchpath = "..\\";
        searchpath += root;
        searchpath += "\\";
        searchpath += topdata.cFileName;
	      handle = FindFirstFile(searchpath.c_str(), &data);
	      if (handle == INVALID_HANDLE_VALUE)
          continue;
        FindClose(handle);

		    if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
                FILE_ATTRIBUTE_DIRECTORY)
        {
	        WIN32_FIND_DATA	nestdata;
          HANDLE nesthandle;

          // Now look for all files with the specified spec in the
          // current directory
          std::string filepath;
          filepath = root;
          filepath += "\\";
          filepath += topdata.cFileName;
          filepath += "\\";
          filepath += stype;
	        nesthandle = FindFirstFile(filepath.c_str(), &nestdata);
          do
          {
	          if (nesthandle == INVALID_HANDLE_VALUE)
              break;

            filepath = root;
            filepath += "\\";
            filepath += topdata.cFileName;
            filepath += "\\";
            filepath += nestdata.cFileName;
            {
              char drive[_MAX_DRIVE];
              char dir[_MAX_DIR];
              char fname[_MAX_FNAME];
              char ext[_MAX_EXT];

              _splitpath( filepath.c_str(), drive, dir, fname, ext );

              rootpath = "..\\";
              rootpath += root;
              rootpath += "\\";
              rootpath += topdata.cFileName;
              rootpath += "\\";
              rootpath += fname;
	            handle = FindFirstFile(rootpath.c_str(), &nestdata);
	            if (handle != INVALID_HANDLE_VALUE)
              {
                std::string renamed;
                FindClose(handle);
                renamed = rootpath;
                renamed += ".bak";
                MoveFile(rootpath.c_str(),renamed.c_str());
                if (CopyFile(filepath.c_str(),rootpath.c_str(),FALSE))
                {
                  SetFileTimeEx(rootpath.c_str());
                }
              }
              else
              {
                // If the file does not exist, then we are free to copy it over.
                if (CopyFile(filepath.c_str(),rootpath.c_str(),FALSE))
                {
                  SetFileTimeEx(rootpath.c_str());
                }
              }
            }
	        } while (FindNextFile(nesthandle, &nestdata));
          FindClose(nesthandle);

          rootpath = root;
          rootpath += "\\";
          rootpath += data.cFileName;
          process_project_replacements(dsw,rootpath.c_str(),stype);
        }
      }
	  } while (FindNextFile(tophandle, &topdata));
    FindClose(tophandle);
}

void CConfigureApp::process_project_type(ofstream &dsw,
      const char *root, int runtime, const char *stype, const int btype)
{
    int project_type = DISABLED;
    HANDLE tophandle;
    // Scan all top level directories and process the ones
    // that we are allowed to.
	  WIN32_FIND_DATA	topdata;
    std::string rootpath = root;
    rootpath += "\\*";
	  tophandle = FindFirstFile(rootpath.c_str(), &topdata);
	  do
	  {
	    if (tophandle == INVALID_HANDLE_VALUE)
        break;
		  if ((topdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
              FILE_ATTRIBUTE_DIRECTORY)
      {
        HANDLE handle;
	      WIN32_FIND_DATA	data;

        if (stricmp(topdata.cFileName,".") == 0)
          continue;
        if (stricmp(topdata.cFileName,"..") == 0)
          continue;

        std::string searchpath = root;
        searchpath += "\\";
        searchpath += topdata.cFileName;
        searchpath += "\\";
        searchpath += stype;

	      handle = FindFirstFile(searchpath.c_str(), &data);
	      if (handle == INVALID_HANDLE_VALUE)
          continue;
        FindClose(handle);

        searchpath = "..\\";
        searchpath += root;
        searchpath += "\\";
        searchpath += topdata.cFileName;

	      handle = FindFirstFile(searchpath.c_str(), &data);
	      if (handle == INVALID_HANDLE_VALUE)
          continue;
        FindClose(handle);
		    if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
                FILE_ATTRIBUTE_DIRECTORY)
        {
          rootpath = root;
          rootpath += "\\";
          rootpath += topdata.cFileName;
          process_one_folder(dsw,rootpath.c_str(),data,btype,runtime);
          if (btype == UTILITY || btype == PROJECT
            || btype == ADD_ON || btype == MODULE)
          {
            rootpath = root;
            rootpath += "\\";
            rootpath += data.cFileName;
            process_project_type(dsw,rootpath.c_str(),runtime,stype,btype);
          }
        }
      }
	  } while (FindNextFile(tophandle, &topdata));
    FindClose(tophandle);
}

class MyWaitDlg : public CWaitDialog
{
public:
  MyWaitDlg (bool *status) :
    CWaitDialog (status, NULL, NULL, NULL)
  {
    nCurrent = 1;
    nTotal = 10;
    nPercent = 0;
  }

  void Pumpit()
  {
	  nPercent = (nCurrent * 100)/nTotal;
	  SetPercentComplete(nPercent);
	  Pump();
    nCurrent++;
    if (nCurrent > nTotal)
      nCurrent = 0;
  }

  int
    nCurrent,
    nTotal,
    nPercent;
};

#define THIS_SUB_KEY FALSE
#define ALL_SUB_KEYS TRUE

#define SIGNAL_EVENT    TRUE
#define WAIT_FOR_CHANGE FALSE

class CRegistry
{
   private:
      CRegistry( const CRegistry& ) {};
      CRegistry& operator=( const CRegistry& ) { return( *this ); };

   private:
      void m_Initialize( void );

   protected:
      HKEY m_KeyHandle;
      HKEY m_RegistryHandle;

      LONG m_ErrorCode;

      CString m_ClassName;
      CString m_ComputerName;
      CString m_KeyName;
      CString m_RegistryName;
      DWORD   m_NumberOfSubkeys;
      DWORD   m_NumberOfValues;

      /*
      ** Data items filled in by QueryInfo
      */

      DWORD    m_LongestSubkeyNameLength;
      DWORD    m_LongestClassNameLength;
      DWORD    m_LongestValueNameLength;
      DWORD    m_LongestValueDataLength;
      DWORD    m_SecurityDescriptorLength;
      FILETIME m_LastWriteTime;

   public:

      enum _Keys
      {
         keyLocalMachine         = (DWORD) HKEY_LOCAL_MACHINE,
         keyClassesRoot          = (DWORD) HKEY_CLASSES_ROOT,
         keyPerformanceData      = (DWORD) HKEY_PERFORMANCE_DATA,
         keyUsers                = (DWORD) HKEY_USERS,
         keyCurrentUser          = (DWORD) HKEY_CURRENT_USER,
#if ( WINVER >= 0x400 )
         keyCurrentConfiguration = (DWORD) HKEY_CURRENT_CONFIG,
         keyDynamicData          = (DWORD) HKEY_DYN_DATA
#endif
      };

      enum KeyValueTypes
      {
         typeBinary                 = REG_BINARY,
         typeDoubleWord             = REG_DWORD,
         typeDoubleWordLittleEndian = REG_DWORD_LITTLE_ENDIAN,
         typeDoubleWordBigEndian    = REG_DWORD_BIG_ENDIAN,
         typeUnexpandedString       = REG_EXPAND_SZ,
         typeSymbolicLink           = REG_LINK,
         typeMultipleString         = REG_MULTI_SZ,
         typeNone                   = REG_NONE,
         typeResourceList           = REG_RESOURCE_LIST,
         typeString                 = REG_SZ
      };

      enum CreateOptions
      {
         optionsNonVolatile = REG_OPTION_NON_VOLATILE,
         optionsVolatile    = REG_OPTION_VOLATILE
      };

      enum CreatePermissions
      {
         permissionAllAccess        = KEY_ALL_ACCESS,
         permissionCreateLink       = KEY_CREATE_LINK,
         permissionCreateSubKey     = KEY_CREATE_SUB_KEY,
         permissionEnumerateSubKeys = KEY_ENUMERATE_SUB_KEYS,
         permissionExecute          = KEY_EXECUTE,
         permissionNotify           = KEY_NOTIFY,
         permissionQueryValue       = KEY_QUERY_VALUE,
         permissionRead             = KEY_READ,
         permissionSetValue         = KEY_SET_VALUE,
         permissionWrite            = KEY_WRITE
      };

      enum CreationDisposition
      {
         dispositionCreatedNewKey     = REG_CREATED_NEW_KEY,
         dispositionOpenedExistingKey = REG_OPENED_EXISTING_KEY
      };

      enum NotifyChangeFlag
      {
         changeKeyAndSubkeys    = TRUE,
         changeSpecifiedKeyOnly = FALSE
      };

      enum NotifyChangeFilter
      {
         notifyName       = REG_NOTIFY_CHANGE_NAME,
         notifyAttributes = REG_NOTIFY_CHANGE_ATTRIBUTES,
         notifyLastSet    = REG_NOTIFY_CHANGE_LAST_SET,
         notifySecurity   = REG_NOTIFY_CHANGE_SECURITY
      };

      CRegistry();

      virtual ~CRegistry();

      virtual BOOL Close( void );
      virtual BOOL Connect( HKEY key_to_open = HKEY_CURRENT_USER,
                            LPCTSTR computer_name = NULL );
      virtual BOOL EnumerateKeys( const DWORD subkey_index,
                                  CString&    subkey_name,
                                  CString&    class_name );
      virtual BOOL EnumerateValues( const DWORD    value_index,
                                    CString&       name_of_value,
                                    KeyValueTypes& type_code,
                                    LPBYTE         data_buffer,
                                    DWORD&         size_of_data_buffer );
      virtual BOOL GetBinaryValue( LPCTSTR name_of_value, CByteArray& return_array );
      virtual void GetClassName( CString& class_name ) const;
      virtual void GetComputerName( CString& computer_name ) const;
      virtual BOOL GetDoubleWordValue( LPCTSTR name_of_value, DWORD& return_value );
      virtual BOOL GetErrorCode( void ) const;
      virtual void GetKeyName( CString& key_name ) const;
      virtual DWORD GetNumberOfSubkeys( void ) const;
      virtual DWORD GetNumberOfValues( void ) const;
      virtual void GetRegistryName( CString& registry_name ) const;
      virtual BOOL GetStringValue( LPCTSTR name_of_value, CString& return_string );
      virtual BOOL GetStringArrayValue( LPCTSTR name_of_value, CStringArray& return_array );
      virtual BOOL GetValue( LPCTSTR name_of_value, CByteArray& return_array );
      virtual BOOL GetValue( LPCTSTR name_of_value, DWORD& return_value );
      virtual BOOL GetValue( LPCTSTR name_of_value, CString& return_string );
      virtual BOOL GetValue( LPCTSTR name_of_value, CStringArray& return_array );
      virtual BOOL Open( LPCTSTR name_of_subkey_to_open,
         const CreatePermissions security_access_mask = permissionAllAccess );
      virtual BOOL QueryInfo( void );
      virtual BOOL QueryValue( LPCTSTR        name_of_value,
                               KeyValueTypes& value_type,
                               LPBYTE         address_of_buffer,
                               DWORD&         size_of_buffer );

      virtual BOOL UnLoad( LPCTSTR name_of_subkey_to_unload );

};

CRegistry::CRegistry()
{
   m_Initialize();
}

CRegistry::~CRegistry()
{
   if ( m_RegistryHandle != (HKEY) NULL )
      Close();
   m_Initialize();
}

void CRegistry::m_Initialize( void )
{
   m_ClassName.Empty();
   m_ComputerName.Empty();
   m_KeyName.Empty();
   m_RegistryName.Empty();

   m_KeyHandle                    = (HKEY) NULL;
   m_ErrorCode                    = 0L;
   m_NumberOfSubkeys              = 0;
   m_LongestSubkeyNameLength      = 0;
   m_LongestClassNameLength       = 0;
   m_NumberOfValues               = 0;
   m_LongestValueNameLength       = 0;
   m_LongestValueDataLength       = 0;
   m_SecurityDescriptorLength     = 0;
   m_LastWriteTime.dwLowDateTime  = 0;
   m_LastWriteTime.dwHighDateTime = 0;
   m_RegistryHandle               = (HKEY) NULL;
}

BOOL CRegistry::Close( void )
{
   if ( m_KeyHandle != (HKEY) NULL )
   {
      ::RegCloseKey( m_KeyHandle );
      m_KeyHandle = (HKEY) NULL;
   }

   if ( m_RegistryHandle == (HKEY) NULL )
      return( TRUE );

   m_ErrorCode = ::RegCloseKey( m_RegistryHandle );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      m_RegistryHandle = (HKEY) NULL;
      m_Initialize();
      return( TRUE );
   }
   else
      return( FALSE );
}

BOOL CRegistry::Connect( HKEY key_to_open, LPCTSTR name_of_computer )
{
   try
   {
      if ( key_to_open == (HKEY) keyClassesRoot ||
        key_to_open == (HKEY) keyCurrentUser )
      {
         if ( name_of_computer == NULL )
         {
            m_RegistryHandle = key_to_open;
            m_ErrorCode = ERROR_SUCCESS;
         }
         else
            m_ErrorCode = ERROR_INVALID_HANDLE;
      }
      else
      {
         m_ErrorCode = ::RegConnectRegistry( (LPTSTR) name_of_computer, key_to_open, &m_RegistryHandle );
      }

      if ( m_ErrorCode == ERROR_SUCCESS )
      {
         if ( name_of_computer == NULL )
         {
            TCHAR computer_name[ MAX_PATH ];
            DWORD size = MAX_PATH;

            if ( ::GetComputerName( computer_name, &size ) == FALSE )
               m_ComputerName.Empty();
            else
               m_ComputerName = computer_name;
         }
         else
            m_ComputerName = name_of_computer;

         if ( key_to_open == HKEY_LOCAL_MACHINE )
            m_RegistryName = TEXT( "HKEY_LOCAL_MACHINE" );
         else if ( key_to_open == HKEY_CLASSES_ROOT )
            m_RegistryName = TEXT( "HKEY_CLASSES_ROOT" );
         else if ( key_to_open == HKEY_USERS )
            m_RegistryName = TEXT( "HKEY_USERS" );
         else if ( key_to_open == HKEY_CURRENT_USER )
            m_RegistryName = TEXT( "HKEY_CURRENT_USER" );
         else if ( key_to_open == HKEY_PERFORMANCE_DATA )
            m_RegistryName = TEXT( "HKEY_PERFORMANCE_DATA" );
#if ( WINVER >= 0x400 )
         else if ( key_to_open == HKEY_CURRENT_CONFIG )
            m_RegistryName = TEXT( "HKEY_CURRENT_CONFIG" );
         else if ( key_to_open == HKEY_DYN_DATA )
            m_RegistryName = TEXT( "HKEY_DYN_DATA" );
#endif
         else
            m_RegistryName = TEXT( "Unknown" );

         return( TRUE );
      }
      else
         return( FALSE );
   }
   catch( ... )
   {
      m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
      return( FALSE );
   }
}

#define DIMENSION_OF( argument ) ( sizeof( argument ) / sizeof( *( argument ) ) )

BOOL CRegistry::EnumerateKeys( const DWORD subkey_index, CString& subkey_name, CString& class_name )
{
   TCHAR subkey_name_string[ 2048 ];
   TCHAR class_name_string[ 2048 ];

   DWORD size_of_subkey_name_string = DIMENSION_OF( subkey_name_string ) - 1;
   DWORD size_of_class_name_string  = DIMENSION_OF( class_name_string  ) - 1;

   ::ZeroMemory( subkey_name_string, sizeof( subkey_name_string ) );
   ::ZeroMemory( class_name_string,  sizeof( class_name_string  ) );

   m_ErrorCode = ::RegEnumKeyEx( m_KeyHandle, 
                                 subkey_index, 
                                 subkey_name_string, 
                                &size_of_subkey_name_string,
                                 NULL,
                                 class_name_string,
                                &size_of_class_name_string,
                                &m_LastWriteTime );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      subkey_name = subkey_name_string;
      class_name  = class_name_string;

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::EnumerateValues( const DWORD    value_index, 
                                 CString&       name_of_value, 
                                 KeyValueTypes& type_code,
                                 LPBYTE         data_buffer,
                                 DWORD&         size_of_data_buffer )
{
   DWORD temp_type_code = type_code;

   TCHAR temp_name[ 2048 ];

   ::ZeroMemory( temp_name, sizeof( temp_name ) );

   DWORD temp_name_size = DIMENSION_OF( temp_name );

   try
   {
      m_ErrorCode = ::RegEnumValue( m_KeyHandle,
                                    value_index,
                                    temp_name,
                                   &temp_name_size,
                                    NULL,
                                   &temp_type_code,
                                    data_buffer,
                                   &size_of_data_buffer );

      if ( m_ErrorCode == ERROR_SUCCESS )
      {
         type_code     = (KeyValueTypes) temp_type_code;
         name_of_value = temp_name;

         return( TRUE );
      }
      else
         return( FALSE );
   }
   catch( ... )
   {
      m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
      return( FALSE );
   }
}

BOOL CRegistry::GetBinaryValue( LPCTSTR name_of_value, CByteArray& return_array )
{
   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   QueryInfo();

   DWORD size_of_buffer = m_LongestValueDataLength;

   LPBYTE memory_buffer = (LPBYTE) new BYTE[ size_of_buffer ];

   if ( memory_buffer == NULL )
   {
      m_ErrorCode = ::GetLastError();
      return( FALSE );
   }

   BOOL return_value = TRUE;

   KeyValueTypes type = typeBinary;

   if ( QueryValue( name_of_value, type, memory_buffer, size_of_buffer ) != FALSE )
   {
      /*
      ** We've got data so give it back to the caller
      */

      return_array.RemoveAll();

      DWORD index = 0;

      while( index < size_of_buffer )
      {
         return_array.Add( memory_buffer[ index ] );
         index++;
      }

      return_value = TRUE;
   }
   else
      return_value = FALSE;

   delete [] memory_buffer;

   return( return_value );
}

void CRegistry::GetClassName( CString& class_name ) const
{
   class_name = m_ClassName;
}

void CRegistry::GetComputerName( CString& computer_name ) const
{
   computer_name = m_ComputerName;
}

BOOL CRegistry::GetDoubleWordValue( LPCTSTR name_of_value, DWORD& return_value )
{
   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   DWORD size_of_buffer = sizeof( DWORD );

   KeyValueTypes type = typeDoubleWord;

   return( QueryValue( name_of_value, type, (LPBYTE) &return_value, size_of_buffer ) );
}

BOOL CRegistry::GetErrorCode( void ) const
{
   return( m_ErrorCode );
}

void CRegistry::GetKeyName( CString& key_name ) const
{
   key_name = m_KeyName;
}

DWORD CRegistry::GetNumberOfSubkeys( void ) const
{
   return( m_NumberOfSubkeys );
}

DWORD CRegistry::GetNumberOfValues( void ) const
{
   return( m_NumberOfValues );
}

void CRegistry::GetRegistryName( CString& registry_name ) const
{
   registry_name = m_RegistryName;
}

BOOL CRegistry::GetStringValue( LPCTSTR name_of_value, CString& return_string )
{
   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   TCHAR temp_string[ 2048 ];
   DWORD size_of_buffer = 2048;

   ::ZeroMemory( temp_string, sizeof( temp_string ) );

   KeyValueTypes type = typeString;

   if ( QueryValue( name_of_value, type, (LPBYTE) temp_string, size_of_buffer ) != FALSE )
   {
      return_string = temp_string;
      return( TRUE );
   }
   else
   {
      return_string.Empty();
      return( FALSE );
   }
}

BOOL CRegistry::GetStringArrayValue( LPCTSTR name_of_value, CStringArray& return_array )
{
   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   QueryInfo();

   DWORD size_of_buffer = m_LongestValueDataLength;

   LPBYTE memory_buffer = (LPBYTE) new BYTE[ size_of_buffer ];

   if ( memory_buffer == NULL )
   {
      m_ErrorCode = ::GetLastError();
      return( FALSE );
   }

   BOOL return_value = TRUE;

   KeyValueTypes type = typeMultipleString; // A double NULL terminated string

   if ( QueryValue( name_of_value, type, memory_buffer, size_of_buffer ) != FALSE )
   {
      LPTSTR strings = (LPTSTR) memory_buffer;

      return_array.RemoveAll();

      while( strings[ 0 ] != 0x00 )
      {
         return_array.Add( (LPCTSTR) strings );
         strings += ( _tcslen( (LPCTSTR) strings ) + 1 ); // Paul Ostrowski [postrowski@xantel.com]
      }

      return_value = TRUE;
   }
   else
      return_value = FALSE;

   delete [] memory_buffer;  
   return( return_value );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, CByteArray& return_array )
{
   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( GetBinaryValue( name_of_value, return_array ) );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, DWORD& return_value )
{
   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( GetDoubleWordValue( name_of_value, return_value ) );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, CString& return_string )
{
   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( GetStringValue( name_of_value, return_string ) );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, CStringArray& return_array )
{
   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( GetStringArrayValue( name_of_value, return_array ) );
}

BOOL CRegistry::Open( LPCTSTR name_of_subkey_to_open, const CreatePermissions security_access_mask )
{
   try
   {
      if ( m_KeyHandle != (HKEY) NULL )
      {
         ::RegCloseKey( m_KeyHandle );
         m_KeyHandle = (HKEY) NULL;
      }

      m_ErrorCode = ::RegOpenKeyEx( m_RegistryHandle, name_of_subkey_to_open, NULL, security_access_mask, &m_KeyHandle );

      if ( m_ErrorCode == ERROR_SUCCESS )
      {
         QueryInfo();
         m_KeyName = name_of_subkey_to_open;

         return( TRUE );
      }
      else
         return( FALSE );
   }
   catch( ... )
   {
      m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
      return( FALSE );
   }
}

BOOL CRegistry::QueryInfo( void )
{
   TCHAR class_name[ 2048 ];

   ::ZeroMemory( class_name, sizeof( class_name ) );
   DWORD size_of_class_name = DIMENSION_OF( class_name ) - 1; 
   m_ErrorCode = ::RegQueryInfoKey( m_KeyHandle,
                                    class_name,
                                   &size_of_class_name,
                          (LPDWORD) NULL,
                                   &m_NumberOfSubkeys,
                                   &m_LongestSubkeyNameLength,
                                   &m_LongestClassNameLength,
                                   &m_NumberOfValues,
                                   &m_LongestValueNameLength,
                                   &m_LongestValueDataLength,
                                   &m_SecurityDescriptorLength,
                                   &m_LastWriteTime );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      m_ClassName = class_name;
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::QueryValue( LPCTSTR        name_of_value, 
                            KeyValueTypes& value_type, 
                            LPBYTE         address_of_buffer, 
                            DWORD&         size_of_buffer )
{
   /*
   ** address_of_buffer and size_of_buffer can be NULL
   */

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   // We were passed a pointer, do not trust it

   try
   {
      DWORD temp_data_type = (DWORD) value_type;

      m_ErrorCode = ::RegQueryValueEx( m_KeyHandle,
                              (LPTSTR) name_of_value,
                                       NULL,
                                      &temp_data_type,
                                       address_of_buffer,
                                      &size_of_buffer );

      if ( m_ErrorCode == ERROR_SUCCESS )
      {
         value_type = (KeyValueTypes) temp_data_type;
         return( TRUE );
      }
      else
      {
         return( FALSE );
      }
   }
   catch( ... )
   {
      m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
      return( FALSE );
   }
}

BOOL CRegistry::UnLoad( LPCTSTR name_of_subkey_to_unload )
{
  if ( name_of_subkey_to_unload == NULL )
  {
    m_ErrorCode = ERROR_INVALID_PARAMETER;
    return( FALSE );
  }
  try
  {
    m_ErrorCode = ::RegUnLoadKey( m_KeyHandle, name_of_subkey_to_unload );
    if ( m_ErrorCode == ERROR_SUCCESS )
      return( TRUE );
    else
    return( FALSE );
  }
  catch( ... )
  {
    return( FALSE );
  }
}

BOOL CConfigureApp::InitInstance()
{
	// Standard initialization
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	CConfigureWizard wizard;

#ifdef _DEBUG
  debuglog.open ("configure.log", ofstream::out | ofstream::app);
  debuglog << "The log has been opened" << endl;;
#endif

  m_pMainWnd = &wizard.m_Page1;

  release_loc = "..\\Release\\";
  debug_loc = "..\\Debug\\";
  bin_loc = "..\\bin\\";
  lib_loc = "..\\lib\\";

  wizard.m_Page2.m_useX11Stubs = useX11Stubs;
  wizard.m_Page2.m_decorateFiles = decorateFiles;
  wizard.m_Page2.m_optionalFiles = optionalFiles;
  wizard.m_Page2.m_standalone = standaloneMode;
  wizard.m_Page2.m_generateFontmap = generateFontmap;

  wizard.m_Page3.m_tempRelease = release_loc;
  wizard.m_Page3.m_tempDebug = debug_loc;
  wizard.m_Page3.m_outputBin = bin_loc;
  wizard.m_Page3.m_outputLib = lib_loc;

  if (!generateFontmap)
  {
    CRegistry registry;
    if ( registry.Connect( HKEY_LOCAL_MACHINE ) == TRUE )
    {
      if (
            (registry.Open( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
                        CRegistry::permissionRead) == TRUE)
            ||
            (registry.Open( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts",
                        CRegistry::permissionRead) == TRUE)
        ) {
        CString filename;
        registry.QueryInfo();

        BOOL result = TRUE;

        filename = bin_loc + "fonts.mgk";
	      ofstream fontmap(filename);
        fontmap << "<?xml version=\"1.0\"?>" << endl;
        for (int index=0; index<registry.GetNumberOfValues(); index++)
        {
          CStringEx name_of_value;
          CRegistry::KeyValueTypes type_code;
          BYTE data_buffer[2048];
          LPBYTE ptr_data_buffer = data_buffer;
          DWORD size_of_data_buffer = 2048;
          result = registry.EnumerateValues( index, name_of_value, type_code,
                    data_buffer, size_of_data_buffer );

          // The font name looks like this: "XYZ Bold Italic (TrueType)"
          CStringEx font_format, font_name, font_full_name;
          int name_length = name_of_value.GetLength();
          int type_pos = name_of_value.ReverseFind("(");
          if (type_pos > 0)
          {
            font_full_name = name_of_value.Left(type_pos-1);
            font_name = font_full_name;
            font_name.FindReplace(" ","-",TRUE);
            font_format = name_of_value.Mid(type_pos);
            if (font_format.FindNoCase("(TrueType)") == 0)
            {
              // Information we need to create:
              //  format="type1"
              //  metrics="/usr/share/fonts/afms/adobe/phvb8an.afm"
              //  glyphs="/usr/share/fonts/default/Type1/n019044l.pfb"
              //  name="Helvetica-Narrow-Bold"
              //  fullname="Helvetica Narrow Bold"
              //  familyname="Helvetica"
              //  weight="Bold"
              //  version="0.1"
              //  alias="NimbusSanL-BoldCond"
              //
              fontmap << "<fontmap>" << endl;
              fontmap << "  <font"
                      << " format=\"truetype\""
                      << " glyphs=\"c:\\Windows\\Fonts\\" << data_buffer << "\""
                      << " fullname=\"" << font_full_name << "\""
                      << " name=\"" << font_name << "\""
                      << " />" << endl;
              fontmap << "</fontmap>" << endl;
            }
          }
        }
        fontmap << "</xml>" << endl;
        registry.Close();
      }
    }
  }

	int nResponse = wizard.DoModal();
  if (nResponse == ID_WIZFINISH)
	{
 	  bool
      bContinue = TRUE;

    useX11Stubs = wizard.m_Page2.m_useX11Stubs;
    decorateFiles = wizard.m_Page2.m_decorateFiles;
    optionalFiles = wizard.m_Page2.m_optionalFiles;
    absolutePaths = standaloneMode = wizard.m_Page2.m_standalone;
    generateFontmap = wizard.m_Page2.m_generateFontmap;
    release_loc = wizard.m_Page3.m_tempRelease;
    debug_loc = wizard.m_Page3.m_tempDebug;
    bin_loc = wizard.m_Page3.m_outputBin;
    lib_loc = wizard.m_Page3.m_outputLib;

    int projectType = wizard.m_Page2.m_projectType;

	  MyWaitDlg waitdlg(&bContinue);

    CString theprojectname;
    switch (projectType)
    {
      case SINGLETHREADEDSTATIC:
	      theprojectname = "..\\VisualStaticST.dsw";
        break;
      case MULTITHREADEDSTATIC:
	      theprojectname = "..\\VisualStaticMT.dsw";
        break;
      case MULTITHREADEDSTATICDLL:
	      theprojectname = "..\\VisualStaticMTDLL.dsw";
        break;
      default:
      case MULTITHREADEDDLL:
	      theprojectname = "..\\VisualDynamicMT.dsw";
        break;
    }
    ofstream dsw(theprojectname);

		write_dsw_start(dsw);

    waitdlg.Pumpit();

    standard_include.push_back("..\\..");
    standard_include.push_back("..\\..\\magick");
    standard_include.push_back("..\\..\\xlib");
    standard_include.push_back("..\\..\\Magick++\\lib");
    //standard_include.push_back("..\\..\\MagickArgs");

    // Write all library project files:
    if (projectType == MULTITHREADEDDLL)
		{
      if (!useX11Stubs)
      {
		    libs_list_shared.push_back("X11.lib");
      }
		}
		libs_list_shared.push_back("kernel32.lib");
		libs_list_shared.push_back("user32.lib");
		libs_list_shared.push_back("gdi32.lib");
		libs_list_shared.push_back("odbc32.lib");
		libs_list_shared.push_back("odbccp32.lib");
		libs_list_shared.push_back("ole32.lib");
		libs_list_shared.push_back("oleaut32.lib");
		libs_list_shared.push_back("winmm.lib");
		libs_list_shared.push_back("dxguid.lib");
		libs_list_shared.push_back("wsock32.lib");
		libs_list_shared.push_back("advapi32.lib");
		//libs_list_shared.push_back("scrnsave.lib");

    waitdlg.Pumpit();
    process_project_replacements(dsw,"..","*.in");
    waitdlg.Pumpit();
    process_project_type(dsw,"..",projectType,"THIRDPARTY.txt",THIRDPARTY);
    waitdlg.Pumpit();
    process_project_type(dsw,"..",projectType,"LIBRARY.txt",   LIBRARY);
    waitdlg.Pumpit();
    process_project_type(dsw,"..",projectType,"STATICLIB.txt", STATICLIB);
    waitdlg.Pumpit();
    process_project_type(dsw,"..",projectType,"MODULE.txt",    MODULE);
    waitdlg.Pumpit();
    process_project_type(dsw,"..",projectType,"ADD_ON.txt",    ADD_ON);
    waitdlg.Pumpit();
    // consoleMode = TRUE;
    process_project_type(dsw,"..",projectType,"UTILITY.txt",   UTILITY);
    waitdlg.Pumpit();
    // consoleMode = FALSE;
    process_project_type(dsw,"..",projectType,"PROJECT.txt",   PROJECT);
    waitdlg.Pumpit();

    consoleMode = TRUE;
	  begin_project(dsw, "All", ".\\All\\All.dsp");
    waitdlg.Pumpit();
    generate_dependencies(dsw, true, true);
    end_project(dsw);

    waitdlg.Pumpit();
		write_dsw_end(dsw);
	}
#ifdef _DEBUG
  debuglog.close();
#endif
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Visual C++ workspace write routine.

void CConfigureApp::write_dsw_start(ofstream &dsw)
{
	dsw << "Microsoft Developer Studio Workspace File, Format Version 6.00" << endl;
	dsw << "# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!" << endl;
	dsw << endl;
	dsw << "###############################################################################" << endl;
	dsw << endl;
}

void CConfigureApp::write_dsw_end(ofstream &dsw)
{
	dsw << "Global:" << endl;
	dsw << endl;
	dsw << "Package=<5>" << endl;
	dsw << "{{{" << endl;
	dsw << "}}}" << endl;
	dsw << endl;
	dsw << "Package=<3>" << endl;
	dsw << "{{{" << endl;
	dsw << "}}}" << endl;
	dsw << endl;
	dsw << "###############################################################################" << endl;
	dsw << endl;
}

void CConfigureApp::begin_project(ofstream &dsw, const char *name, const char *filename)
{
	dsw << "Project: \"" << name << "\"=\"" << filename << "\" - Package Owner=<4>" << endl;
	dsw << endl;
	dsw << "Package=<5>" << endl;
	dsw << "{{{" << endl;
	dsw << "}}}" << endl;
	dsw << endl;
	dsw << "Package=<4>" << endl;
	dsw << "{{{" << endl;
}

void CConfigureApp::end_project(ofstream &dsw)
{
	dsw << "}}}" << endl;
	dsw << endl;
	dsw << "###############################################################################" << endl;
	dsw << endl;
}

void CConfigureApp::add_project_dependency(ofstream &dsw, const char *dep_name)
{
	dsw << "    Begin Project Dependency" << endl;
	dsw << "    Project_Dep_Name " << dep_name << endl;
	dsw << "    End Project Dependency" << endl;
}

typedef struct _ConfigureInfo
{
  char
    *name,
    *extn,
    *group;
} ConfigureInfo;

void CConfigureApp::write_lib_dsp(
  bool dll,
  int runtime,
  int project_type,
	std::string root,
	std::string search,
	std::string dspname,
	std::string prefix,
	std::string extn,
	std::list<std::string> &libs_list_shared,
	std::list<std::string> &libs_list_release,
	std::list<std::string> &libs_list_debug,
	std::list<std::string> &defines_list,
	std::list<std::string> &includes_list,
	std::list<std::string> &source_list,
  std::list<std::string> &exclude_list)
{
  CString libname = prefix.c_str();
  libname += dspname.c_str();

	ofstream dsp(get_project_name(
    dll?DLLPROJECT:LIBPROJECT,runtime,root,prefix,dspname).c_str());

  CString outname;
  std::string bin_path;
  std::string lib_path;
  std::string debug_path;
  std::string release_path;
  std::string extra_path;
  CStringEx getcount = root.c_str();
  int levels = getcount.GetFieldCount('\\');
  {
    for (int j=0; j<(levels-2); j++)
      extra_path += "..\\";
  }
  if (bin_loc[0]=='.')
    bin_path += extra_path;
  bin_path += bin_loc;
  if (lib_loc[0]=='.')
    lib_path += extra_path;
  lib_path += lib_loc;
  if (debug_loc[0]=='.')
    debug_path += extra_path;
  debug_path += debug_loc;
  if (release_loc[0]=='.')
    release_path += extra_path;
  release_path += release_loc;

  // Temporary kludge to identify COM DLL's that need to be registered as part
  // of the build process.
  bool isCOMproject = false;
  CStringEx strPath = root.c_str();
  if (strPath.FindNoCase("\\ATL\\",0) > 0)
    isCOMproject = true;

	dsp << "# Microsoft Developer Studio Project File - Name=\"" << libname << "\" - Package Owner=<4>" << endl;
	dsp << "# Microsoft Developer Studio Generated Build File, Format Version 6.00" << endl;
	dsp << "# ** DO NOT EDIT **" << endl;
	dsp << endl;
  if (dll)
	  dsp << "# TARGTYPE \"Win32 (x86) Dynamic-Link Library\" 0x0102" << endl;
  else
	  dsp << "# TARGTYPE \"Win32 (x86) Static Library\" 0x0104" << endl;
	dsp << endl;
	dsp << "CFG=" << libname << " - Win32 Release" << endl;
	dsp << "!MESSAGE This is not a valid makefile. To build this project using NMAKE," << endl;
	dsp << "!MESSAGE use the Export Makefile command and run" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE NMAKE /f \"" << libname << ".mak\"." << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE You can specify a configuration when running NMAKE" << endl;
	dsp << "!MESSAGE by defining the macro CFG on the command line. For example:" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE NMAKE /f \"" << libname << ".mak\" CFG=\"" << libname << " - Win32 Release\"" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE Possible choices for configuration are:" << endl;
	dsp << "!MESSAGE " << endl;
  if (dll)
  {
	  dsp << "!MESSAGE \"" << libname << " - Win32 Release\" (based on \"Win32 (x86) Dynamic-Link Library\")" << endl;
	  dsp << "!MESSAGE \"" << libname << " - Win32 Debug\" (based on \"Win32 (x86) Dynamic-Link Library\")" << endl;
  }
  else
  {
	  dsp << "!MESSAGE \"" << libname << " - Win32 Release\" (based on \"Win32 (x86) Static Library\")" << endl;
	  dsp << "!MESSAGE \"" << libname << " - Win32 Debug\" (based on \"Win32 (x86) Static Library\")" << endl;
  }
	dsp << "!MESSAGE " << endl;
	dsp << endl;
	dsp << "# Begin Project" << endl;
	dsp << "# PROP AllowPerConfigDependencies 0" << endl;
	dsp << "# PROP Scc_ProjName \"\"" << endl;
	dsp << "# PROP Scc_LocalPath \"\"" << endl;
	dsp << "CPP=cl.exe" << endl;
	dsp << "MTL=midl.exe" << endl;
	dsp << "RSC=rc.exe" << endl;
	dsp << endl;

	dsp << "!IF  \"$(CFG)\" == \"" << libname << " - Win32 Release\"" << endl;
	dsp << endl;

	dsp << "# PROP Use_MFC 0" << endl;
	dsp << "# PROP Use_Debug_Libraries 0" << endl;
	dsp << "# PROP Output_Dir \"" << get_full_path(root + "\\",lib_path).c_str() << "\"" << endl;
	dsp << "# PROP Intermediate_Dir \"" << get_full_path(root + "\\",release_path).c_str() << libname << "\"" << endl;
	dsp << "# PROP Target_Dir \"\"" << endl;
  if (dll)
	  dsp << "LIB32=link.exe -lib" << endl;
	dsp << "# ADD CPP /nologo";
  switch(runtime)
  {
    case SINGLETHREADEDSTATIC:
      dsp << " /ML";
      break;
    case MULTITHREADEDSTATIC:
      dsp << " /MT";
      break;
    default:
    case MULTITHREADEDSTATICDLL:
    case MULTITHREADEDDLL:
      dsp << " /MD";
      break;
  }
  dsp << " /W3 /GX /O2";
	{
		for (
			std::list<std::string>::iterator it = standard_include.begin();
			it != standard_include.end();
			it++)
		{
		  std::string relpath;
		  if (!absolutePaths)
		    relpath = extra_path;
		  relpath += *it;
		  dsp << " /I \"" << get_full_path("",relpath).c_str() << "\"";
		}
	}
	{
		for (
			std::list<std::string>::iterator it = includes_list.begin();
			it != includes_list.end();
			it++)
		{
			dsp << " /I \"" << get_full_path(root + "\\",*it).c_str() << "\"";
		}
	}
  dsp << " /D \"NDEBUG\" /D \"WIN32\" /D \"_WINDOWS\" /D \"_VISUALC_\" /D \"NeedFunctionPrototypes\"";
	{
		for (
			std::list<std::string>::iterator it = defines_list.begin();
			it != defines_list.end();
			it++)
		{
			dsp << " /D \"" << (*it).c_str() << "\"";
		}
	}
	dsp << " /FD /c" << endl;

  if (dll && isCOMproject)
  {
	  std::string sources;
    
    sources = "..\\";
    sources += root;
    sources += "\\";
	  dsp << "# ADD MTL /Oicf /out \"" << get_full_path("",sources).c_str() << "\"" << endl;
  }
  else
  {
    dsp << "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32" << endl;
  }
	dsp << "# ADD RSC /l 0x409 /d \"NDEBUG\"" << endl;
	dsp << "BSC32=bscmake.exe" << endl;
	dsp << "# ADD BSC32 /nologo" << endl;
  outname = prefix.c_str();
  if (project_type != ADD_ON)
    outname += "RL_";
  outname += dspname.c_str();
  if (project_type != ADD_ON)
    outname += "_";
  if (dll)
  {
	  dsp << "LINK32=link.exe" << endl;
	  dsp << "# ADD LINK32";
  }
  else
  {
	  dsp << "LIB32=link.exe -lib" << endl;
	  dsp << "# ADD LIB32";
  }

  if (dll)
  {
	  dsp << " /libpath:\"" << get_full_path(root + "\\",lib_path).c_str() << "\"";
  }
  // in special mode we hard code the references to all the
  // needed libraries directly into the project file instead of
  // creating them as workspace dependencies.
  if (onebigdllMode)
  {
    switch (runtime)
    {
      case MULTITHREADEDSTATIC:
      case SINGLETHREADEDSTATIC:
      case MULTITHREADEDSTATICDLL:
        {
          CStringEx strDepends, strTemp;
	        for (
		        std::list<std::string>::iterator it1a = dependency_list.begin();
		        it1a != dependency_list.end();
		        it1a++)
	        {
            strTemp = (*it1a).c_str();
            /* look for entries with a xxx%syyy%szzz format */
            if (strTemp.FindNoCase("%s",0) > 0)
            {
              /* ignore any entries for utility programs as well */
              if (strTemp.FindNoCase("UTIL_",0) != 0)
              {
                strDepends.Format((*it1a).c_str(),"RL_","_.lib");
			          dsp << " " << strDepends << "";
              }
            }
          }
        }
        break;
      default:
      case MULTITHREADEDDLL:
        {
          CStringEx strDepends, strTemp;
          strDepends.Format("CORE_%smagick%s","RL_","_.lib");
			    dsp << " " << strDepends << "";
          if (useX11Stubs)
          {
            strDepends.Format("CORE_%sxlib%s","RL_","_.lib");
			      dsp << " " << strDepends << "";
          }
          if (extn.compare("cpp") == 0)
          {
            strDepends.Format("CORE_%sMagick++%s","RL_","_.lib");
			      dsp << " " << strDepends << "";
          }
          CStringEx strPath = root.c_str();
          if (strPath.Find("\\SDL") >= 0)
          {
            strDepends.Format("CORE_%sSDL%s","RL_","_.lib");
			      dsp << " " << strDepends << "";
          }
          if (strPath.Find("..\\jp2",0) == 0)
          {
            strDepends.Format("CORE_%sjp2%s","RL_","_.lib");
			      dsp << " " << strDepends << "";
          }
        }
        break;
	  }
  }
  if (dll)
  {
	  {
		  for (
			  std::list<std::string>::iterator it = libs_list_shared.begin();
			  it != libs_list_shared.end();
			  it++)
		  {
			  dsp << " " << (*it).c_str() << "";
		  }
	  }
  }
	{
		for (
			std::list<std::string>::iterator it = libs_list_release.begin();
			it != libs_list_release.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
  if (dll)
  {
	  dsp << " /nologo /subsystem:windows /dll";
    if (search.compare("*") != 0)
    {
      dsp.setf( ios::hex );
      dsp << " /base:\"0x" << dllbaselocation << "\"";
      dllbaselocation += 0x00010000L;
    }
    dsp << " /incremental:no /machine:I386";
    dsp << " /pdb:\"" << get_full_path(root + "\\",bin_path).c_str() << outname << ".pdb\"";
	  dsp << " /out:\"" << get_full_path(root + "\\",bin_path).c_str() << outname << ".dll\"";
  }
  else
  {
	  dsp << " /nologo /machine:I386";
	  dsp << " /out:\"" << get_full_path(root + "\\",lib_path).c_str() << outname << ".lib\"";
  }
	dsp << endl;

  if (dll && isCOMproject)
  {
	  std::string trigger, target;
    
    trigger = "\"";
    trigger += get_full_path(root + "\\",release_path);
    trigger += libname;
    trigger += "\\regsvr32.trg\"";

    target = "\"";
    target += get_full_path(root + "\\",bin_path);
    target += outname;
    target += ".dll\"";

	  dsp << "# Begin Custom Build - Performing registration" << endl;
	  dsp << trigger.c_str() << " : " << target.c_str() << endl;
	  dsp << "\tmidle /tlb " << trigger.c_str() << endl;
	  dsp << "\techo regsvr32 exec. time >" << trigger.c_str() << endl;
	  dsp << "# End Custom Build" << endl;
  }
	dsp << endl;

	dsp << "!ELSEIF  \"$(CFG)\" == \"" << libname << " - Win32 Debug\"" << endl;
	dsp << endl;

	dsp << "# PROP Use_MFC 0" << endl;
	dsp << "# PROP Use_Debug_Libraries 1" << endl;
	dsp << "# PROP Output_Dir \"" << get_full_path(root + "\\",lib_path).c_str() << "\"" << endl;
	dsp << "# PROP Intermediate_Dir \"" << get_full_path(root + "\\",debug_path).c_str() << libname << "\"" << endl;
	dsp << "# PROP Target_Dir \"\"" << endl;
  if (dll)
	  dsp << "LIB32=link.exe -lib" << endl;
	dsp << "# ADD CPP /nologo";
  switch(runtime)
  {
    case SINGLETHREADEDSTATIC:
      dsp << " /MLd";
      break;
    case MULTITHREADEDSTATIC:
      dsp << " /MTd";
      break;
    default:
    case MULTITHREADEDSTATICDLL:
    case MULTITHREADEDDLL:
      dsp << " /MDd";
      break;
  }
  dsp << " /W3 /Gm /GX /Zi /Od";
	{
		for (
			std::list<std::string>::iterator it = standard_include.begin();
			it != standard_include.end();
			it++)
		{
		  std::string relpath;
 		  if (!absolutePaths)
		    relpath = extra_path;
		  relpath += *it;
			dsp << " /I \"" << get_full_path("",relpath).c_str() << "\"";
		}
	}
	{
		for (
			std::list<std::string>::iterator it = includes_list.begin();
			it != includes_list.end();
			it++)
		{
			dsp << " /I \"" << get_full_path(root + "\\",*it).c_str() << "\"";
		}
	}
  dsp << " /D \"_DEBUG\" /D \"WIN32\" /D \"_WINDOWS\" /D \"_VISUALC_\" /D \"NeedFunctionPrototypes\"";
	{
		for (
			std::list<std::string>::iterator it = defines_list.begin();
			it != defines_list.end();
			it++)
		{
			dsp << " /D \"" << (*it).c_str() << "\"";
		}
	}
	dsp << " /FD /c" << endl;

  if (dll && isCOMproject)
  {
	  std::string sources;
    
    sources = "..\\";
    sources += root;
    sources += "\\";
	  dsp << "# ADD MTL /Oicf /out \"" << get_full_path("",sources).c_str() << "\"" << endl;
  }
  else
  {
	  dsp << "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32" << endl;
  }
	dsp << "# ADD RSC /l 0x409 /d \"_DEBUG\"" << endl;
	dsp << "BSC32=bscmake.exe" << endl;
	dsp << "# ADD BSC32 /nologo" << endl;
  outname = prefix.c_str();
  if (project_type != ADD_ON)
    outname += "DB_";
  outname += dspname.c_str();
  if (project_type != ADD_ON)
    outname += "_";
  if (dll)
  {
	  dsp << "LINK32=link.exe" << endl;
	  dsp << "# ADD LINK32";
  }
  else
  {
	  dsp << "LIB32=link.exe -lib" << endl;
	  dsp << "# ADD LIB32";
  }

  if (dll)
  {
	  dsp << " /libpath:\"" << get_full_path(root + "\\",lib_path).c_str() << "\"";
  }
  // in special mode we hard code the references to all the
  // needed libraries directly into the project file instead of
  // creating them as workspace dependencies.
  if (onebigdllMode)
  {
    switch (runtime)
    {
      case MULTITHREADEDSTATIC:
      case SINGLETHREADEDSTATIC:
      case MULTITHREADEDSTATICDLL:
        {
          CStringEx strDepends, strTemp;
	        for (
		        std::list<std::string>::iterator it1a = dependency_list.begin();
		        it1a != dependency_list.end();
		        it1a++)
	        {
            strTemp = (*it1a).c_str();
            strTemp = (*it1a).c_str();
            /* look for entries with a xxx%syyy%szzz format */
            if (strTemp.FindNoCase("%s",0) > 0)
            {
              /* ignore any entries for utility programs as well */
              if (strTemp.FindNoCase("UTIL_",0) != 0)
              {
                strDepends.Format((*it1a).c_str(),"DB_","_.lib");
			          dsp << " " << strDepends << "";
              }
            }
          }
        }
        break;
      default:
      case MULTITHREADEDDLL:
        {
          CStringEx strDepends, strTemp;
          strDepends.Format("CORE_%smagick%s","DB_","_.lib");
			    dsp << " " << strDepends << "";
          if (useX11Stubs)
          {
            strDepends.Format("CORE_%sxlib%s","DB_","_.lib");
			      dsp << " " << strDepends << "";
          }
          if (extn.compare("cpp") == 0)
          {
            strDepends.Format("CORE_%sMagick++%s","DB_","_.lib");
			      dsp << " " << strDepends << "";
          }
          CStringEx strPath = root.c_str();
          if (strPath.Find("\\SDL") >= 0)
          {
            strDepends.Format("CORE_%sSDL%s","DB_","_.lib");
			      dsp << " " << strDepends << "";
          }
          if (strPath.Find("..\\jp2",0) == 0)
          {
            strDepends.Format("CORE_%sjp2%s","DB_","_.lib");
			      dsp << " " << strDepends << "";
          }
        }
        break;
	  }
  }
  if (dll)
  {
	  {
		  for (
			  std::list<std::string>::iterator it = libs_list_shared.begin();
			  it != libs_list_shared.end();
			  it++)
		  {
			  dsp << " " << (*it).c_str() << "";
		  }
	  }
  }
	{
		for (
			std::list<std::string>::iterator it = libs_list_debug.begin();
			it != libs_list_debug.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
  if (dll)
  {
	  dsp << " /nologo /subsystem:windows /dll";
    if (search.compare("*") != 0)
    {
      dsp.setf( ios::hex );
      dsp << " /base:\"0x" << dllbaselocation << "\"";
      dllbaselocation += 0x00010000L;
    }
    dsp << " /incremental:no /debug /machine:I386";
    dsp << " /pdb:\"" << get_full_path(root + "\\",bin_path).c_str() << outname << ".pdb\"";
	  dsp << " /out:\"" << get_full_path(root + "\\",bin_path).c_str() << outname << ".dll\"";
  }
  else
  {
	  dsp << " /nologo /machine:I386";
	  dsp << " /out:\"" << get_full_path(root + "\\",lib_path).c_str() << outname << ".lib\"";
  }
	dsp << endl;

  if (dll && isCOMproject)
  {
	  std::string trigger, target;
    
    trigger = "\"";
    trigger += get_full_path(root + "\\",debug_path);
    trigger += libname;
    trigger += "\\regsvr32.trg\"";

    target = "\"";
    target += get_full_path(root + "\\",bin_path);
    target += outname;
    target += ".dll\"";

	  dsp << "# Begin Custom Build - Performing registration" << endl;
	  dsp << trigger.c_str() << " : " << target.c_str() << endl;
	  dsp << "\tregsvr32 /s /c " << target.c_str() << endl;
	  dsp << "\techo regsvr32 exec. time >" << trigger.c_str() << endl;
	  dsp << "# End Custom Build" << endl;
  }
	dsp << endl;

	dsp << "!ENDIF " << endl;
	dsp << endl;
	dsp << "# Begin Target" << endl;
	dsp << endl;
	dsp << "# Name \"" << libname << " - Win32 Release\"" << endl;
	dsp << "# Name \"" << libname << " - Win32 Debug\"" << endl;

  CString dir;
  CString spec;
  CString group;

  const ConfigureInfo
    valid_dirs[] = {
    { "\\",          ".c",   "src"     },
    { "\\",          ".cpp", "src"     },
    { "\\",          ".rc",  "src"     },
    { "\\",          ".def", "src"     },
    { "\\",          ".idl", "src"     },
    { "\\",          ".h",   "include" },
    { NULL,          NULL,   NULL      }
  };

  group = valid_dirs[0].group;
	begin_group(dsp, group);
  {
		for (std::list<std::string>::iterator it = source_list.begin();
			  it != source_list.end();
			    it++)
		{
	    add_file(dsp, (*it).c_str());
		}
	}
  for (int i=0; valid_dirs[i].name != NULL; i++)
  {
    if (group.CompareNoCase(valid_dirs[i].group) != 0)
    {
	    end_group(dsp);
      group = valid_dirs[i].group;
	    begin_group(dsp, group);
    }
    dir = root.c_str();
    dir += valid_dirs[i].name;
    if (search.length() > 0)
	    spec = search.c_str();
    else
	    spec = dspname.c_str();
    spec += valid_dirs[i].extn;
	  generate_dir(dsp, dir, spec, 0, project_type, exclude_list);
  }
	end_group(dsp);

	// End .dsp file:
	dsp << "# End Target" << endl;
	dsp << "# End Project" << endl;
}

void CConfigureApp::write_exe_dsp(
  int runtime,
  int project_type,
	std::string root,
	std::string search,
	std::string dspname,
	std::string prefix,
	std::string extn,
	std::list<std::string> &libs_list_shared,
	std::list<std::string> &libs_list_release,
	std::list<std::string> &libs_list_debug,
	std::list<std::string> &defines_list,
	std::list<std::string> &includes_list,
	std::list<std::string> &source_list,
  std::list<std::string> &exclude_list)
{
  CString libname = prefix.c_str();
  libname += dspname.c_str();

	ofstream dsp(get_project_name(
    EXEPROJECT,runtime,root,prefix,dspname).c_str());

  CString outname;
  std::string bin_path;
  std::string lib_path;
  std::string debug_path;
  std::string release_path;
  std::string extra_path;
  CStringEx getcount = root.c_str();
  int levels = getcount.GetFieldCount('\\');
  {
    for (int j=0; j<(levels-2); j++)
      extra_path += "..\\";
  }
  if (bin_loc[0]=='.')
    bin_path += extra_path;
  bin_path += bin_loc;
  if (lib_loc[0]=='.')
    lib_path += extra_path;
  lib_path += lib_loc;
  if (debug_loc[0]=='.')
    debug_path += extra_path;
  debug_path += debug_loc;
  if (release_loc[0]=='.')
    release_path += extra_path;
  release_path += release_loc;

	dsp << "# Microsoft Developer Studio Project File - Name=\"" << libname << "\" - Package Owner=<4>" << endl;
	dsp << "# Microsoft Developer Studio Generated Build File, Format Version 6.00" << endl;
	dsp << "# ** DO NOT EDIT **" << endl;
	dsp << endl;
	dsp << "# TARGTYPE \"Win32 (x86) Console Application\" 0x0103" << endl;
	dsp << endl;
	dsp << "CFG=" << libname << " - Win32 Release" << endl;
	dsp << "!MESSAGE This is not a valid makefile. To build this project using NMAKE," << endl;
	dsp << "!MESSAGE use the Export Makefile command and run" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE NMAKE /f \"" << libname << ".mak\"." << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE You can specify a configuration when running NMAKE" << endl;
	dsp << "!MESSAGE by defining the macro CFG on the command line. For example:" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE NMAKE /f \"" << libname << ".mak\" CFG=\"" << libname << " - Win32 Release\"" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE Possible choices for configuration are:" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE \"" << libname << " - Win32 Release\" (based on \"Win32 (x86) Console Application\")" << endl;
	dsp << "!MESSAGE \"" << libname << " - Win32 Debug\" (based on \"Win32 (x86) Console Application\")" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << endl;
	dsp << "# Begin Project" << endl;
	dsp << "# PROP AllowPerConfigDependencies 0" << endl;
	dsp << "# PROP Scc_ProjName \"\"" << endl;
	dsp << "# PROP Scc_LocalPath \"\"" << endl;
	dsp << "CPP=cl.exe" << endl;
	dsp << "MTL=midl.exe" << endl;
	dsp << "RSC=rc.exe" << endl;
	dsp << endl;

	dsp << "!IF  \"$(CFG)\" == \"" << libname << " - Win32 Release\"" << endl;
	dsp << endl;

	dsp << "# PROP Use_MFC 0" << endl;
	dsp << "# PROP Use_Debug_Libraries 0" << endl;
	dsp << "# PROP Output_Dir \"" << get_full_path(root + "\\",bin_path).c_str() << "\"" << endl;
	dsp << "# PROP Intermediate_Dir \"" << get_full_path(root + "\\",release_path).c_str() << libname << "\"" << endl;
	dsp << "# PROP Target_Dir \"\"" << endl;
	dsp << "LIB32=link.exe -lib" << endl;
	dsp << "# ADD CPP /nologo";
  switch(runtime)
  {
    case SINGLETHREADEDSTATIC:
      dsp << " /ML";
      break;
    case MULTITHREADEDSTATIC:
      dsp << " /MT";
      break;
    default:
    case MULTITHREADEDSTATICDLL:
    case MULTITHREADEDDLL:
      dsp << " /MD";
      break;
  }
  dsp << " /W3 /GX /O2";
	{
		dsp << " /I \".\"";
		for (
			std::list<std::string>::iterator it = standard_include.begin();
			it != standard_include.end();
			it++)
		{
      std::string relpath;
      if (!absolutePaths)
        relpath = extra_path;
      relpath += *it;
			dsp << " /I \"" << get_full_path("",relpath).c_str() << "\"";
		}
	}
	{
		for (
			std::list<std::string>::iterator it = includes_list.begin();
			it != includes_list.end();
			it++)
		{
			dsp << " /I \"" << get_full_path(root + "\\",*it).c_str() << "\"";
		}
	}
  dsp << " /D \"NDEBUG\" /D \"WIN32\" ";
  if (consoleMode)
    dsp << "/D \"_CONSOLE\" ";
  else
    dsp << "/D \"_WINDOWS\" ";
  dsp << "/D \"_VISUALC_\" /D \"NeedFunctionPrototypes\"";
	{
		for (
			std::list<std::string>::iterator it = defines_list.begin();
			it != defines_list.end();
			it++)
		{
			dsp << " /D \"" << (*it).c_str() << "\"";
		}
	}
  if ((project_type == PROJECT) && (runtime == MULTITHREADEDDLL))
    dsp << "/D \"_AFXDLL\"";
	dsp << " /FD /c" << endl;

  dsp << "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32" << endl;
	dsp << "# ADD RSC /l 0x409 /d \"NDEBUG\"" << endl;
	dsp << "BSC32=bscmake.exe" << endl;
	dsp << "# ADD BSC32 /nologo" << endl;
	dsp << "LINK32=link.exe" << endl;

	dsp << "# ADD LINK32";
	dsp << " /libpath:\"" << get_full_path(root + "\\",lib_path).c_str() << "\"";
  // in standalone mode we hard code the references to all the
  // needed libraries directly into the project file instead of
  // creating them as workspace dependencies.
  if (standaloneMode)
  {
    switch (runtime)
    {
      case MULTITHREADEDSTATIC:
      case SINGLETHREADEDSTATIC:
      case MULTITHREADEDSTATICDLL:
        {
          CStringEx strDepends, strTemp;
	        for (
		        std::list<std::string>::iterator it1a = dependency_list.begin();
		        it1a != dependency_list.end();
		        it1a++)
	        {
            strTemp = (*it1a).c_str();
            /* look for entries with a xxx%syyy%szzz format */
            if (strTemp.FindNoCase("%s",0) > 0)
            {
              /* ignore any entries for utility programs as well */
              if (strTemp.FindNoCase("UTIL_",0) != 0)
              {
                strDepends.Format((*it1a).c_str(),"RL_","_.lib");
			          dsp << " " << strDepends << "";
              }
            }
          }
        }
        break;
      default:
      case MULTITHREADEDDLL:
        {
          CStringEx strDepends, strTemp;
          strDepends.Format("CORE_%smagick%s","RL_","_.lib");
			    dsp << " " << strDepends << "";
          if (useX11Stubs)
          {
            strDepends.Format("CORE_%sxlib%s","RL_","_.lib");
			      dsp << " " << strDepends << "";
          }
          if (extn.compare("cpp") == 0)
          {
            strDepends.Format("CORE_%sMagick++%s","RL_","_.lib");
			      dsp << " " << strDepends << "";
          }
          CStringEx strPath = root.c_str();
          if (strPath.Find("\\SDL") >= 0)
          {
            strDepends.Format("CORE_%sSDL%s","RL_","_.lib");
			      dsp << " " << strDepends << "";
          }
          if (strPath.Find("..\\jp2",0) == 0)
          {
            strDepends.Format("CORE_%sjp2%s","RL_","_.lib");
			      dsp << " " << strDepends << "";
          }
        }
        break;
	  }
  }
	{
		for (
			std::list<std::string>::iterator it = libs_list_shared.begin();
			it != libs_list_shared.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
	{
		for (
			std::list<std::string>::iterator it = libs_list_release.begin();
			it != libs_list_release.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
	dsp << " /nologo ";
  if (consoleMode)
    dsp << "/subsystem:console ";
  else
    dsp << "/subsystem:windows ";
  dsp << "/incremental:no /machine:I386 ";
  if (decorateFiles)
    outname = prefix.c_str();
  else
    outname = "";
  if (decorateFiles)
    outname += "RL_";
  outname += dspname.c_str();
  dsp << "/pdb:\"" << get_full_path(root + "\\",bin_path).c_str() << outname;
  if (decorateFiles)
    dsp << "_";
  dsp << ".pdb\"";
  dsp << " /out:\"" << get_full_path(root + "\\",bin_path).c_str() << outname;
  if (decorateFiles)
    dsp << "_";
  dsp << ".exe\"";
	dsp << endl;
	dsp << endl;

	dsp << "!ELSEIF  \"$(CFG)\" == \"" << libname << " - Win32 Debug\"" << endl;
	dsp << endl;

	dsp << "# PROP Use_MFC 0" << endl;
	dsp << "# PROP Use_Debug_Libraries 1" << endl;
	dsp << "# PROP Output_Dir \"" << get_full_path(root + "\\",bin_path).c_str() << "\"" << endl;
	dsp << "# PROP Intermediate_Dir \"" << get_full_path(root + "\\",debug_path).c_str() << libname << "\"" << endl;
	dsp << "# PROP Target_Dir \"\"" << endl;
	dsp << "LIB32=link.exe -lib" << endl;
	dsp << "# ADD CPP /nologo";
  switch(runtime)
  {
    case SINGLETHREADEDSTATIC:
      dsp << " /MLd";
      break;
    case MULTITHREADEDSTATIC:
      dsp << " /MTd";
      break;
    default:
    case MULTITHREADEDSTATICDLL:
    case MULTITHREADEDDLL:
      dsp << " /MDd";
      break;
  }
  dsp << " /W3 /Gm /GX /Zi /Od";
	{
		dsp << " /I \".\"";
		for (
			std::list<std::string>::iterator it = standard_include.begin();
			it != standard_include.end();
			it++)
		{
      std::string relpath;
      if (!absolutePaths)
        relpath = extra_path;
      relpath += *it;
			dsp << " /I \"" << get_full_path("",relpath).c_str() << "\"";
		}
	}
	{
		for (
			std::list<std::string>::iterator it = includes_list.begin();
			it != includes_list.end();
			it++)
		{
			dsp << " /I \"" << get_full_path(root + "\\",*it).c_str() << "\"";
		}
	}
  dsp << " /D \"_DEBUG\" /D \"WIN32\" ";
  if (consoleMode)
    dsp << "/D \"_CONSOLE\" ";
  else
    dsp << "/D \"_WINDOWS\" ";
  dsp << "/D \"_VISUALC_\" /D \"NeedFunctionPrototypes\"";
	{
		for (
			std::list<std::string>::iterator it = defines_list.begin();
			it != defines_list.end();
			it++)
		{
			dsp << " /D \"" << (*it).c_str() << "\"";
		}
	}
  if ((project_type == PROJECT) && (runtime == MULTITHREADEDDLL))
    dsp << "/D \"_AFXDLL\"";
	dsp << " /FD /c" << endl;
	
	dsp << "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32" << endl;
	dsp << "# ADD RSC /l 0x409 /d \"_DEBUG\"" << endl;
	dsp << "BSC32=bscmake.exe" << endl;
	dsp << "# ADD BSC32 /nologo" << endl;
	dsp << "LINK32=link.exe" << endl;

	dsp << "# ADD LINK32";
	dsp << " /libpath:\"" << get_full_path(root + "\\",lib_path).c_str() << "\"";
  // in standalone mode we hard code the references to all the
  // needed libraries directly into the project file instead of
  // creating them as workspace dependencies.
  if (standaloneMode)
  {
    switch (runtime)
    {
      case MULTITHREADEDSTATIC:
      case SINGLETHREADEDSTATIC:
      case MULTITHREADEDSTATICDLL:
        {
          CStringEx strDepends, strTemp;
	        for (
		        std::list<std::string>::iterator it1a = dependency_list.begin();
		        it1a != dependency_list.end();
		        it1a++)
	        {
            strTemp = (*it1a).c_str();
            strTemp = (*it1a).c_str();
            /* look for entries with a xxx%syyy%szzz format */
            if (strTemp.FindNoCase("%s",0) > 0)
            {
              /* ignore any entries for utility programs as well */
              if (strTemp.FindNoCase("UTIL_",0) != 0)
              {
                strDepends.Format((*it1a).c_str(),"DB_","_.lib");
			          dsp << " " << strDepends << "";
              }
            }
          }
        }
        break;
      default:
      case MULTITHREADEDDLL:
        {
          CStringEx strDepends, strTemp;
          strDepends.Format("CORE_%smagick%s","DB_","_.lib");
			    dsp << " " << strDepends << "";
          if (useX11Stubs)
          {
            strDepends.Format("CORE_%sxlib%s","DB_","_.lib");
			      dsp << " " << strDepends << "";
          }
          if (extn.compare("cpp") == 0)
          {
            strDepends.Format("CORE_%sMagick++%s","DB_","_.lib");
			      dsp << " " << strDepends << "";
          }
          CStringEx strPath = root.c_str();
          if (strPath.Find("\\SDL") >= 0)
          {
            strDepends.Format("CORE_%sSDL%s","DB_","_.lib");
			      dsp << " " << strDepends << "";
          }
          if (strPath.Find("..\\jp2",0) == 0)
          {
            strDepends.Format("CORE_%sjp2%s","DB_","_.lib");
			      dsp << " " << strDepends << "";
          }
        }
        break;
	  }
  }
	{
		for (
			std::list<std::string>::iterator it = libs_list_shared.begin();
			it != libs_list_shared.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
	{
		for (
			std::list<std::string>::iterator it = libs_list_debug.begin();
			it != libs_list_debug.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
  if (decorateFiles)
    outname = prefix.c_str();
  else
    outname = "";
  if (decorateFiles)
    outname += "DB_";
  outname += dspname.c_str();
	dsp << " /nologo ";
  if (consoleMode)
    dsp << "/subsystem:console ";
  else
    dsp << "/subsystem:windows ";
  dsp << "/incremental:no /debug /machine:I386 ";
  dsp << "/pdb:\"" << get_full_path(root + "\\",bin_path).c_str() << outname;
  if (decorateFiles)
    dsp << "_";
  dsp << ".pdb\"";
  dsp << " /out:\"" << get_full_path(root + "\\",bin_path).c_str() << outname;
  if (decorateFiles)
    dsp << "_";
  dsp << ".exe\"";
	dsp << endl;

	dsp << "!ENDIF " << endl;
	dsp << endl;
	dsp << "# Begin Target" << endl;
	dsp << endl;
	dsp << "# Name \"" << libname << " - Win32 Release\"" << endl;
	dsp << "# Name \"" << libname << " - Win32 Debug\"" << endl;

  CString dir;
  CString spec;
  CString group;

  const ConfigureInfo
    valid_dirs[] = {
    { "\\",          ".c",   "src"     },
    { "\\",          ".cpp", "src"     },
    { "\\",          ".rc",  "src"     },
    { "\\",          ".def", "src"     },
    { "\\",          ".idl", "src"     },
    { "\\",          ".h",   "include" },
    { NULL,          NULL,   NULL      }
  };

  group = valid_dirs[0].group;
	begin_group(dsp, group);
  {
		for (std::list<std::string>::iterator it = source_list.begin();
			  it != source_list.end();
			    it++)
		{
	    add_file(dsp, (*it).c_str());
		}
	}
  for (int i=0; valid_dirs[i].name != NULL; i++)
  {
    if (group.CompareNoCase(valid_dirs[i].group) != 0)
    {
	    end_group(dsp);
      group = valid_dirs[i].group;
	    begin_group(dsp, group);
    }

    dir = root.c_str();
    dir += valid_dirs[i].name;
    if (search.length() > 0)
	    spec = search.c_str();
    else
	    spec = dspname.c_str();
    spec += valid_dirs[i].extn;
	  generate_dir(dsp, dir, spec, 0, project_type, exclude_list);
  }
	end_group(dsp);

	// End .dsp file:
	dsp << "# End Target" << endl;
	dsp << "# End Project" << endl;
}

void CConfigureApp::generate_dir(ofstream &dsp,
    const char *dir, const char *spec,
    int nestinglevel, int project_type,
    std::list<std::string> &exclude_list)
{
	static const char *exclude_from_build[] =
	{
		".",
		"..",
		"CVS",
		NULL
	};

	CString path = dir;
	if (path.GetAt(path.GetLength() - 1) != '\\') {
    path += "\\";
  }

	WIN32_FIND_DATA	data;

	HANDLE handle = FindFirstFile(path + "*", &data);
	if (handle != INVALID_HANDLE_VALUE)
  {
	  do
	  {
		  bool skip = false;

		  for (int i=0; exclude_from_build[i] != NULL; i++)
      {
			  if (stricmp(data.cFileName, exclude_from_build[i]) == 0)
          skip = true;
      }

		  if (skip) continue;

		  if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
      {
        if (is_project_type(path  + data.cFileName, project_type) == true)
			    generate_dir(dsp, path + data.cFileName, spec,
            ++nestinglevel, project_type, exclude_list);
      }
	  } while (FindNextFile(handle, &data));
    FindClose(handle);
  }

  CString otherpath = "..\\";
  otherpath += path;
	handle = FindFirstFile(otherpath + spec, &data);
	if (handle != INVALID_HANDLE_VALUE)
  {
	  do
	  {
		  bool skip = false;

		  for (int i=0; exclude_from_build[i] != NULL; i++)
			  if (stricmp(data.cFileName, exclude_from_build[i]) == 0)
          skip = true;

		  for (std::list<std::string>::iterator it = exclude_list.begin();
			    it != exclude_list.end();
			      it++)
		  {
			  if (stricmp(data.cFileName, (*it).c_str()) == 0)
          skip = true;
		  }

		  if (skip) continue;

		  if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			  continue;
      if (project_type == UTILITY || project_type == PROJECT
          || project_type == ADD_ON || project_type == MODULE)
      {
        CStringEx getcount = path;
        int levels = getcount.GetFieldCount('\\');
        std::string relpath = "..\\";
        for (int j=0; j<(levels-3); j++)
          relpath += "..\\";
        relpath += path;
        relpath += data.cFileName;
	      add_file(dsp, get_full_path(dir,relpath).c_str());
      }
      else
      {
        std::string relpath = otherpath + data.cFileName;
	      add_file(dsp, get_full_path("",relpath).c_str());
      }

	  } while (FindNextFile(handle, &data));
    FindClose(handle);
  }
}

void CConfigureApp::begin_group(ofstream &dsp, const char *group_name)
{
	dsp << "# Begin Group \"" << group_name << "\"" << endl;
	//dsp << endl;
	dsp << "# PROP Default_Filter \"\"" << endl;
}

void CConfigureApp::end_group(ofstream &dsp)
{
	dsp << "# End Group" << endl;
}

void CConfigureApp::add_file(ofstream &dsp, const char *filename)
{
	dsp << "# Begin Source File" << endl;
	//dsp << endl;
	dsp << "SOURCE=" << filename << endl;
	dsp << "# End Source File" << endl;
}

BOOL BrowseForFolder(HWND hOwner, char* szTitle, char* szRetval)
{
	BROWSEINFO		info;
	LPITEMIDLIST	itemidlist;
	char			szDirectory[_MAX_PATH];
	LPMALLOC		pMalloc;

	memset(szDirectory, '\0', _MAX_PATH);

	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		info.hwndOwner = hOwner;
		info.pidlRoot = NULL;
		info.pszDisplayName = szDirectory;
		info.lpszTitle = szTitle;
		info.ulFlags = 0;
		info.lpfn = NULL;

		itemidlist = SHBrowseForFolder(&info);
		if (itemidlist != NULL)
		{
			SHGetPathFromIDList(itemidlist, szRetval);
			pMalloc->Free(itemidlist);
			pMalloc->Release();
			return TRUE;
		}
		else // User clicked Cancel
		{
			pMalloc->Release();
			return FALSE;
		}
	}
	else
		return FALSE;
}

int Replace( LPCTSTR str, LPCTSTR pcszKey, 
              LPCTSTR pcszReplacement,
                BOOL bIgnoreCase = FALSE )
{
	ASSERT( pcszKey != NULL );
	ASSERT( *pcszKey != _T('\0') );
	ASSERT( pcszReplacement != NULL );
	
	if ( pcszKey == NULL ||
        *pcszKey == _T('\0') ||
          pcszReplacement == NULL )
		return -1;
	
	int iStrLength = lstrlen( str );
	int iKeyLength = lstrlen( pcszKey );
	
	// A quick optimization; if the key is larger than our current string,
	// it cannot exist within -- get out of here.
	
	if ( iKeyLength > iStrLength )
		return 0;
	
	// First, figure out how large a buffer we need. If the replacement
	// is smaller or equal to the key, we can simply allocate the same
	// size as the number of characters currently in the string.  If not,
	// we can figure out the maximum number of keys that will be found,
	// multiply by the size differential and allocate that much more.
	// Once we have a number, we can allocate the buffer.
	
	int iBufLength;
	int iRepLength = lstrlen( pcszReplacement );
	
	if ( iRepLength <= iKeyLength )
	{
		iBufLength = iStrLength + 1;
	}
	else
	{
		int iMaxKeys = iStrLength / iKeyLength + 1;
		int iDelta = iRepLength - iKeyLength;
		iBufLength = iStrLength + iMaxKeys * iDelta + 1;
	}
	
	LPTSTR p = new TCHAR[iBufLength];
	memset( p, 0, iBufLength );
	
	if ( p == NULL )
		return -1;
	
	// declare some strings to handle case insensitivity
	CString strStr( str );
	CString strKey( pcszKey );
	
	if ( bIgnoreCase )
	{
		strStr.MakeUpper();
		strKey.MakeUpper();
	}
	
	// set up some loop controls and get to work!
	LPCTSTR pSource = str;
	LPCTSTR pCurr = strStr;
	LPTSTR pOut = p;
	int iReplacements = 0;
	
	while ( *pCurr != _T('\0') )
	{
		// if we find a match...
		if ( _tcsnccmp( pCurr, strKey, iKeyLength ) == 0 )
		{
			// copy the replacement string into the output buffer
			lstrcpy( pOut, pcszReplacement );
			
			// Increment the output pointer by the string just copied.  
			// Increment the source and current pointers by the key length.
			// Increment the replacement count.
			
			pOut += iRepLength;
			pSource += iKeyLength;
			pCurr += iKeyLength;			
			iReplacements++;
		}
		else
		{
			// otherwise, copy the character across and increment pointers
			*( pOut++ ) = *( pSource++ );
			pCurr++;
		}
	}
	
	str = p;
	delete [] p;
	return iReplacements;
}

BOOL ParseReplacement( LPCTSTR p, CString& strKey, CString& strData )
{
	CString strTmp( p );
	int iIndex = strTmp.Find( _T('=') );
	
	if ( iIndex > -1 )
	{
		strKey = strTmp.Left( iIndex );
		strData = strTmp.Right( strTmp.GetLength() - iIndex - 1 );
		return !strKey.IsEmpty() && !strData.IsEmpty();
	}
	
	return FALSE;
}

BOOL ConvertTemplate( char *input, CStringList* pReplacements )
{
	if ( ( pReplacements != NULL ) && !pReplacements->IsEmpty() )
	{
		CString strKey, strData;
		
		for ( POSITION pos = pReplacements->GetHeadPosition();
		pos != NULL;
		pReplacements->GetNext( pos ) )
		{
			CString& str = pReplacements->GetAt( pos );			
			if ( ParseReplacement( str, strKey, strData ) )
			{
				Replace( input, strKey, strData );
			}
		}
	}
  return TRUE;
}

int ReadTemplateFile( CString inputfile, CString outputfile )
{
  CStringList Replacements;
  char szBuf[2048];
  int nLinesRead = 0;
	ofstream dsp(outputfile);
  ifstream inpStream;
  inpStream.open( inputfile, ios::in | ios::nocreate );
  if ( inpStream ) {
    for (;;) {
      inpStream.getline( szBuf, 2047, '\n' );
      if ( inpStream.eof() ) {
        // last line may contain text also
        // (if it's not terminated with '\n' EOF is returned)
        return ConvertTemplate( szBuf, &Replacements );
      }
      if ( !inpStream.good() || !ConvertTemplate( szBuf, &Replacements ) )
        return FALSE;      
      dsp << szBuf << endl;
      nLinesRead++;
    }
  }
  inpStream.close();
  inpStream.clear();
  return nLinesRead;
}