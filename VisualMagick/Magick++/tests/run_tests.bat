@echo off
set srcdir=..\..\..\Magick++\tests
set bindir=..\..\bin\
set PATH=%bindir%;%PATH%

echo appendImages ...
appendImages

echo attributes ...
attributes

echo averageImages ...
averageImages

echo coaleseImages ...
coalesceImages

echo coderInfo ...
coderInfo

echo color ...
color

echo montageImages ...
montageImages

echo morphImages ...
morphImages

echo readWriteBlob ...
readWriteBlob

echo readWriteImages ...
readWriteImages
