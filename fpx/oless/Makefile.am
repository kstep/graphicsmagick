
# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.4 foreign

#INCLUDES = -I$(top_srcdir)/basics -I$(top_srcdir)/fpx -I$(top_srcdir)/ole \
#	-I$(top_srcdir)/ri_image -I$(top_srcdir)/jpeg -I$(top_srcdir)/oless \
#	-I$(top_srcdir)/oless/h
INCLUDES =  \
	-I$(top_srcdir)/oless -I$(top_srcdir)/oless/h

# A convenience library
noinst_LTLIBRARIES = libOless.la

# Sources to compile into library
libOless_la_SOURCES = \
	ascii.cxx \
	ascii.hxx \
	byteordr.hxx \
	cdocfile.cxx \
	chinst.cxx \
	dbg.hxx \
	dfbasis.hxx \
	dffuncs.cxx \
	dfiter.cxx \
	dfstream.cxx \
	difat.cxx \
	dir.cxx \
	dirp.cxx \
	docfile.cxx \
	entry.cxx \
	expdf.cxx \
	expdf.hxx \
	expiter.cxx \
	expiter.hxx \
	expst.cxx \
	expst.hxx \
	fat.cxx \
	flashpix.h \
	funcs.cxx \
	h/cdocfile.hxx \
	h/chinst.hxx \
	h/dbg.hxx \
	h/dfexcept.hxx \
	h/dffuncs.hxx \
	h/dfmsp.hxx \
	h/dfver.h \
	h/difat.hxx \
	h/dir.hxx \
	h/dirfunc.hxx \
	h/docfile.hxx \
	h/docfilep.hxx \
	h/entry.hxx \
	h/error.hxx \
	h/fat.hxx \
	h/funcs.hxx \
	h/handle.hxx \
	h/header.hxx \
	h/mem.hxx \
	h/msf.hxx \
	h/msffunc.hxx \
	h/msfiter.hxx \
	h/ole.hxx \
	h/owchar.h \
	h/page.hxx \
	h/piter.hxx \
	h/props.h \
	h/propstm.hxx \
	h/ref.hxx \
	h/refilb.hxx \
	h/revert.hxx \
	h/rexpdf.hxx \
	h/sstream.hxx \
	h/storage.h \
	h/storagep.h \
	h/tchar.h \
	h/vect.hxx \
	h/vectfunc.hxx \
	header.cxx \
	header.cxx \
	iter.cxx \
	iter.hxx \
	logfile.hxx \
	mem.cxx \
	mread.hxx \
	msf.cxx \
	msfiter.cxx \
	mstream.cxx \
	page.cxx \
	refilb.cxx \
	rexpdf.cxx \
	sstream.cxx \
	storage.cxx \
	time.cxx \
	time.hxx \
	vect.cxx \
	wchar.c

# Additional files to distribute
EXTRA_DIST = \
	build.script \
	commk.gcc \
	commk.msc \
	depend.gcc \
	depend.msc \
	dfhead.cxx \
	doc/msreadme.txt \
	doc/read_me1.txt \
	doc/read_me2.txt \
	doc/readme1st.txt \
	exphead.cxx \
	makefile.gcc \
	makefile.msc \
	msfhead.cxx \
	props/chresult.hxx \
	props/debtrace.hxx \
	props/depend.gcc \
	props/depend.msc \
	props/h/makefile.gcc \
	props/h/ntstatus.h \
	props/h/propapi.h \
	props/h/propmac.hxx \
	props/h/propset.hxx \
	props/h/propvar.hxx \
	props/h/stgprops.hxx \
	props/h/windef.h \
	props/makefile.gcc \
	props/makefile.msc \
	props/ntprop.cxx \
	props/ntpropb.cxx \
	props/olechar.h \
	props/pch.cxx \
	props/propdbg.hxx \
	props/prophdr.hxx \
	props/propstg.cxx \
	props/propstg.hxx \
	props/propstm.cxx \
	props/propvar.cxx \
	props/psetstg.cxx \
	props/psetstg.hxx \
	props/qsort.c \
	props/qsort.h \
	props/stgvarb.cxx \
	props/utils.cxx \
	props/utils.hxx \
	refprop.def \
	refstg.def \
	renamess.bat \
	test/proptest/chresult.hxx \
	test/proptest/cpropvar.cxx \
	test/proptest/cpropvar.hxx \
	test/proptest/depend.gcc \
	test/proptest/makefile.gcc \
	test/proptest/makefile.msc \
	test/proptest/proptest.cxx \
	test/proptest/proptest.hxx \
	test/proptest/testdoc.cxx \
	test/reftest/dft0 \
	test/reftest/drt.dfl \
	test/reftest/makefile.gcc \
	test/reftest/makefile.msc \
	test/reftest/refilb.cxx \
	test/reftest/refilb.hxx \
	test/reftest/reftest.cxx \
	test/reftest/time.cxx \
	test/reftest/time.hxx \
	test/stgdrt/depend.gcc \
	test/stgdrt/depend.msc \
	test/stgdrt/drt.cxx \
	test/stgdrt/drt.hxx \
	test/stgdrt/headers.cxx \
	test/stgdrt/ilb.cxx \
	test/stgdrt/ilb.hxx \
	test/stgdrt/illeg.cxx \
	test/stgdrt/illeg.hxx \
	test/stgdrt/makefile.gcc \
	test/stgdrt/makefile.msc \
	test/stgdrt/stgdrt \
	test/stgdrt/strlist.cxx \
	test/stgdrt/strlist.hxx \
	test/stgdrt/tests.cxx \
	test/stgdrt/tests.hxx \
	test/stgdrt/util.cxx \
	test/stgdrt/util.hxx \
	test/stgdrt/wchar.c \
	test/stgdrt/winio.hxx \
	test/stgdrt/wintype.hxx \
	test/stgdrt/wrap.cxx \
	test/stgdrt/wrap.hxx

# Headers which are not installed but which are distributed
noinst_HEADERS = \
  ascii.hxx byteordr.hxx dbg.hxx dfbasis.hxx expdf.hxx expiter.hxx \
  expst.hxx iter.hxx logfile.hxx mread.hxx time.hxx
