: Batch script to run Magick++ tests
@SET PATH=..\..\VisualMagick\bin\;%PATH%
@cd ..\Magick++\tests
@echo Files are output to ..\Magick++\tests
@echo !!! appendImages !!!
@appendImages
@echo !!! attributes !!!
@attributes
@echo !!! averageImages !!!
@averageImages
@echo !!! coalesceImages !!!
@coalesceImages
@echo !!! color !!!
@color
@echo !!! montageImages !!!
@montageImages
@echo !!! morphImages !!!
@morphImages
@echo !!! readWriteBlob !!!
@readWriteBlob
@echo !!! readWriteImages !!!
@readWriteImages

