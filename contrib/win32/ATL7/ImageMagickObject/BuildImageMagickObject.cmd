@echo off
if {%1} == {} (
  (@echo -)
  (@echo BuildImageMagickObject debug ^| release ^| clean PATH_TO_ROOT NAME_OF_VISUALMAGICK)
  (@echo -)
  (@echo  Example: BuildIt release ..\..\..\.. VisualMagick)
  (@echo -)
  goto :EOF
)
if not defined DevEnvDir (call vsvars32)
set PATH_TO_ROOT=..\..\..\..
if not {%2} == {} (
  set PATH_TO_ROOT=%2
)
if not exist %PATH_TO_ROOT%\magick (
  (@echo Please supply the path to the root of the ImageMagick directory as the second parameter)
  goto :EOF
)
set PATH_TO_MAGICK=%PATH_TO_ROOT%\VisualMagick
if not {%3} == {} (
  set PATH_TO_MAGICK=%PATH_TO_ROOT%\%3
)
if not exist %PATH_TO_MAGICK%\lib\X11.lib (
  (@echo Please supply the name of the VisualMagick directory as the third parameter)
  goto :EOF
)
set MAGICK_HOME=%PATH_TO_MAGICK%\bin
if not exist %MAGICK_HOME% (
  (@echo Problem - the bin subdirectory of VisualMagick is missing. Check that paths for errors)
  goto :EOF
)
if {%1}=={clean} (
  if exist ImageMagickObject.dll (
    regsvr32 /s /u ImageMagickObject.dll
    del ImageMagickObject.dll
  )
  del /Q *.obj >nul 2>&1
  del /Q *.ilk >nul 2>&1
  del /Q *.pdb >nul 2>&1
  del /Q *.exe >nul 2>&1
  del /Q *.res >nul 2>&1
  del /Q *.mgk >nul 2>&1
  del /Q ImageMagickObject.tl? >nul 2>&1
  del /Q ImageMagickObject.idl >nul 2>&1
  del /Q ImageMagickObject.lib >nul 2>&1
  del /Q ImageMagickObject.h >nul 2>&1
  del /Q ImageMagickObject.res >nul 2>&1
  del /Q ImageMagickObject.exp >nul 2>&1
  del /Q ImageMagickObject_p.c >nul 2>&1
  del /Q ImageMagickObject_i.c >nul 2>&1
  del /Q dlldata.c >nul 2>&1
  goto :EOF
)
copy %MAGICK_HOME%\*.mgk .\ >nul 2>&1
if not exist gm_messages.bin (
::::gm convert LOCALE:..\..\..\..\locale\locale.mgk LOCALEH:..\..\..\..\magick\locale_c.h
::::gm convert LOCALE:..\..\..\..\locale\locale.mgk LOCALEMC:gm_messages.mc
  copy ..\..\..\..\magick\gm_messages.mc gm_messages.mc
  mc gm_messages.mc
  move MSG00001.bin gm_messages.bin
  copy gm_messages.bin ..\..\..\..\magick\gm_messages.bin
)
if not exist gm_messages.bin (
  (@echo Problem - the gm_messages.bin file is missing)
  goto :EOF
)
rc /fo ImageMagickObject.res ImageMagickObject.rc
if not exist ImageMagickObject.res (
  (@echo Problem - the compile of RC files for ImageMagickObject failed)
  goto :EOF
)
if {%1}=={debug} (
  if not exist %PATH_TO_MAGICK%\lib\CORE_DB_magick_.lib (
    (@echo Problem - the lib subdirectory of VisualMagick is missing important libraries)
    goto :EOF
  )
  cl /LDd /EHsc /I%PATH_TO_ROOT%\ /Zi /D_DEBUG /MTd ImageMagickObject.cpp %PATH_TO_MAGICK%\lib\CORE_DB_*.lib %PATH_TO_MAGICK%\lib\IM_MOD_DB_*.lib ImageMagickObject.def winmm.lib dxguid.lib wsock32.lib advapi32.lib comsvcs.lib ImageMagickObject.res /link /IDLOUT:ImageMagickObject.idl
)
if {%1}=={release} (
  if not exist %PATH_TO_MAGICK%\lib\CORE_RL_magick_.lib (
    (@echo Problem - the lib subdirectory of VisualMagick is missing important libraries)
    goto :EOF
  )
  cl /LD /EHsc /I%PATH_TO_ROOT%\ /Zi /MT ImageMagickObject.cpp %PATH_TO_MAGICK%\lib\CORE_RL_*.lib %PATH_TO_MAGICK%\lib\IM_MOD_RL_*.lib ImageMagickObject.def winmm.lib dxguid.lib wsock32.lib advapi32.lib comsvcs.lib ImageMagickObject.res /link /IDLOUT:ImageMagickObject.idl
)
if not exist ImageMagickObject.dll (
  (@echo Problem - the ImageMagickObject DLL is missing. It did not build correctly)
  goto :EOF
)
:copy %MAGICK_HOME%\*.dll .\quarantine\ >nul 2>&1
:::::copy ImageMagickObject.dll \\Kinyani\D$\SOAPTests\
:::::copy ImageMagickObject.pdb \\Kinyani\D$\SOAPTests\
if {%1}=={x_debug} (
  move ImageMagickObject.dll .\quarantine\ >nul 2>&1
  move ImageMagickObject.pdb .\quarantine\ >nul 2>&1
  regsvr32 /s .\quarantine\ImageMagickObject.dll
  (@echo ImageMagickObject DLL is registered in the quarantine area!)
) else (
  regsvr32 /s ImageMagickObject.dll
)
cl /EHsc /Zi /D_DEBUG /MTd MagickCMD.cpp
rc /fo gm.res gm.rc
if not exist gm.res (
  (@echo Problem - the compile of RC files for gm.exe failed)
  goto :EOF
)
cl /EHsc /I..\..\..\..\ /Zi /D_DEBUG /D_VISUALC_ /DWIN32 /MTd ..\..\..\..\utilities\gm.c gm.res ImageMagickObject.lib 
if {%1}=={x_debug} (
  move gm.exe .\quarantine\ >nul 2>&1
  move gm.pdb .\quarantine\ >nul 2>&1
)
