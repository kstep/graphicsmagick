%define _use_internal_dependency_generator 0

# Allow for selectively disabling from commandline
%{!?quant:%define quant 8}
%{!?perlm:%define perlm 1}
%{!?cplus:%define cplus 1}

Summary: An X application for displaying and manipulating images.
Name: GraphicsMagick
Version: 1.0.3
Release: 1
License: freeware
Group: X11/Applications/Graphics
Source: ftp://ftp.GraphicsMagick.org/pub/%{name}/%{name}-%{version}.tar.bz2
Url: http://www.GraphicsMagick.org/
Buildroot: %{_tmppath}/%{name}-%{version}-root
BuildPrereq: bzip2-devel, freetype-devel, libjpeg-devel, libpng-devel
BuildPrereq: libtiff-devel, zlib-devel
BuildPrereq: freetype-devel >= 2.0.1
Requires: bzip2, freetype, libjpeg, libpng, zlib

%description
GraphicsMagick(TM) is an image display and manipulation tool for the X
Window System. GraphicsMagick can read and write JPEG, TIFF, PNM, GIF,
and Photo CD image formats. It can resize, rotate, sharpen, color
reduce, or add special effects to an image, and when finished you can
either save the completed work in the original format or a different
one. GraphicsMagick also includes command line programs for creating
animated or transparent .gifs, creating composite images, creating
thumbnail images, and more.

GraphicsMagick is one of your choices if you need a program to manipulate
and display images. If you want to develop your own applications
which use GraphicsMagick code or APIs, you need to install
GraphicsMagick-devel as well.

%package devel
Summary: Static libraries and header files for GraphicsMagick app development.
Group: X11/Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
GraphicsMagick-devel contains the static libraries and header files you'll
need to develop GraphicsMagick applications. GraphicsMagick is an image
manipulation program.

If you want to create applications that will use GraphicsMagick code or
APIs, you need to install GraphicsMagick-devel as well as GraphicsMagick.
You do noy need to install it if you just want to use GraphicsMagick,
however.

%if %{perlm}
%package perl
Summary: GraphicsMagick perl bindings
Group: System Environment/Libraries
BuildPrereq: perl >= 5.6.0
Requires: %{name} = %{version}-%{release}, perl >= 5.6.0

%description perl
Perl bindings to GraphicsMagick.

Install GraphicsMagick-perl if you want to use any perl scripts that use
GraphicsMagick.
%endif

%if %{cplus}
%package c++
Summary: GraphicsMagick Magick++ library (C++ bindings)
Group: System Environment/Libraries
Requires: %{name} = %{version}-%{release}

%description c++
This package contains the Magick++ library, a C++ binding to the GraphicsMagick
graphics manipulation library.

Install GraphicsMagick-c++ if you want to use any applications that use
Magick++.

%package c++-devel
Summary: C++ bindings for the GraphicsMagick library
Group: Development/Libraries
Requires: %{name} = %{version}, %{name}-c++ = %{version}
Requires: %{name}-devel = %{version}, bzip2-devel, freetype-devel
Requires: libjpeg-devel, libpng-devel, libtiff-devel, zlib-devel, libxml2-devel

%description c++-devel
GraphicsMagick-devel contains the static libraries and header files you'll
need to develop GraphicsMagick applications using the Magick++ C++ bindings.
GraphicsMagick is an image manipulation program.

If you want to create applications that will use Magick++ code or APIs,
you'll need to install GraphicsMagick-c++-devel, GraphicsMagick-devel and
GraphicsMagick.  You don't need to install it if you just want to use
GraphicsMagick, or if you want to develop/compile applications using the
GraphicsMagick C interface, however.
%endif

%prep
%setup -q

%build
GM=$RPM_BUILD_DIR/%{name}-%{version}
export LDFLAGS="$LDFLAGS -L$GM/magick/.libs"

%if %{perlm}
# Maybe this will fix perl installation issues.
if [ -z "`perl -v | grep 5.8`" ]; then
    export PERLOPTS="PREFIX=$RPM_BUILD_ROOT%{_prefix}"
fi
%endif

# This shouldn't be there yet.
rm PerlMagick/Makefile.PL

%configure --enable-shared --disable-static \
	--with-modules --enable-lzw \
	--without-frozenpaths \
%if %{perlm}
	--with-perl \
	--with-perl-options="$PERLOPTS" \
%else
	--without-perl \
%endif
%if %{cplus}
	--with-magick-plus-plus \
%else
	--without-magick-plus-plus \
%endif
	--with-quantum-depth=%{quant}
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

# Remove unpackaged files.
rm -f `find $RPM_BUILD_ROOT%{_libdir}/perl*/ -name perllocal.pod -type f`
rm -f `find $RPM_BUILD_ROOT%{_libdir}/perl*/ -name .packlist -type f`
rmdir $RPM_BUILD_ROOT%{_includedir}/magick
rmdir $RPM_BUILD_ROOT%{_datadir}/%{name}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(644, root, root, 755)
%doc README.txt NEWS
%doc %{_datadir}/%{name}-%{version}
%{_libdir}/lib%{name}.so.*
%dir %{_libdir}/%{name}-%{version}
%{_libdir}/%{name}-%{version}/*.mgk
%dir %{_libdir}/%{name}-%{version}/modules-Q%{quant}
%{_libdir}/%{name}-%{version}/modules-Q%{quant}/*/*.so
%{_libdir}/%{name}-%{version}/modules-Q%{quant}/coders/*.mgk
%attr(755, root, root) %{_bindir}/gm
%attr(644, root, man) %{_mandir}/man1/%{name}-config.1.gz
%attr(644, root, man) %{_mandir}/man1/gm.1.gz
%attr(644, root, man) %{_mandir}/man4/*gz
%attr(644, root, man) %{_mandir}/man5/*gz

%files devel
%defattr(644, root, root, 755)
%dir %{_includedir}/%{name}/magick
%{_includedir}/%{name}/magick/*
%{_libdir}/lib%{name}.*a
%{_libdir}/lib%{name}.so
%dir %{_libdir}/%{name}-%{version}/modules-Q%{quant}
%{_libdir}/%{name}-%{version}/modules-Q%{quant}/*/*.la
%{_libdir}/pkgconfig/%{name}.pc
%{_bindir}/%{name}-config

%if %{perlm}
%files perl
%defattr(644, root, root, 755)
%dir %{_libdir}/perl*/site_perl/*/*/Graphics
%{_libdir}/perl*/site_perl/*/*/Graphics/Magick.pm
%dir %{_libdir}/perl*/site_perl/*/*/auto/Graphics/Magick
%{_libdir}/perl*/site_perl/*/*/auto/Graphics/Magick/*
%attr(644, root, man) %{_mandir}/man3/*gz
%endif

%if %{cplus}
%files c++
%defattr(644, root, root, 755)
%{_libdir}/lib%{name}++.so.*

%files c++-devel
%defattr(644, root, root, 755)
%{_includedir}/%{name}/Magick++.h
%dir %{_includedir}/%{name}/Magick++
%{_includedir}/%{name}/Magick++/*
%{_libdir}/lib%{name}++.*a
%{_libdir}/lib%{name}++.so
%{_libdir}/pkgconfig/%{name}++.pc
%{_bindir}/%{name}++-config
%attr(644, root, man) %{_mandir}/man1/%{name}++-config.1.gz
%endif

%changelog
* Mon Jul 28 2003 Bob Friesenhahn <bfriesen@graphicsmagick.org> 1.1-1
- Changed default quantum depth to 8 bits.

* Mon Jun 09 2003 Troy Edwards <vallimar@sexorcisto.net> 1.0.2-3
- removed libungif as a dependancy
- changed with/without-magick_plus_plus -> with/without-magick-plus-plus
- Use --with-perl-options instead of patching Makefile.in
- Only set a perl prefix if we aren't using perl >= 5.8.0

* Sat Jun 07 2003 Troy Edwards <vallimar@sexorcisto.net> 1.0.2-2
- patch to pass DESTDIR to install-exec-perl since some users
  reported problems

* Sat Jun 07 2003 Troy Edwards <vallimar@sexorcisto.net> 1.0.2-1
- updated to 1.0.2

* Tue May 05 2003 Troy Edwards <vallimar@sexorcisto.net> 1.0-1
- updated to 1.0 Final

* Fri Apr 25 2003 Troy Edwards <vallimar@sexorcisto.net> 1.0-Beta1-1
- updated to 1.0-Beta1

* Mon Oct 22 2002 Troy Edwards <vallimar@sexorcisto.net>
- updated to 1.0.0pre
