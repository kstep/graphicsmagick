@echo off
set srcdir=..\..\..\Magick++\tests\
set bindir=..\..\bin\
set PATH=%bindir%;%PATH%

echo exceptions ...
exceptions.exe

echo appendImages ...
appendImages.exe

echo attributes ...
attributes.exe

echo averageImages ...
averageImages.exe

echo coaleseImages ...
coalesceImages.exe

echo coderInfo ...
coderInfo.exe

echo color ...
color.exe

echo montageImages ...
montageImages.exe

echo morphImages ...
morphImages.exe

echo readWriteBlob ...
readWriteBlob.exe

echo readWriteImages ...
readWriteImages.exe
