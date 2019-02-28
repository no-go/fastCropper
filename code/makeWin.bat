@echo off

REM Pfad und Maschinentyp anpassen!
call "c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

set COMPILER=cl
set LINKER=link

%COMPILER% -c fastCropper.cpp -EHsc /GS /analyze- /W1 /Zc:wchar_t /Zi /Gm- /Od /Ob2 /fp:precise /D "_CRT_SECURE_NO_DEPRECATE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /D "_VC80_UPGRADE=0x0710" /errorReport:prompt /WX- /Zc:forScope /Gd /Oy-
%LINKER% -out:fastCropper.exe /subsystem:windows fastCropper.obj "IlmImf.lib" "opencv_contrib2411.lib" "opencv_features2d2411.lib" "zlib.lib" "libjasper.lib" "libjpeg.lib" "libtiff.lib" "libpng.lib" "opencv_ml2411.lib" "opencv_core2411.lib" "opencv_imgproc2411.lib" "opencv_gpu2411.lib" "opencv_highgui2411.lib" "fltk.lib" "comctl32.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /MACHINE:X86 /NODEFAULTLIB:"libcmt"


