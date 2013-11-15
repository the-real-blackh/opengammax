del /s /q opengammax || goto :error
rmdir /s /q opengammax || goto :error
mkdir opengammax || goto :error
copy "C:\qt\qt5.1.1\5.1.1\mingw48_32\bin\libstdc++-6.dll" opengammax || goto :error
copy "C:\qt\qt5.1.1\5.1.1\mingw48_32\bin\libgcc_s_dw2-1.dll" opengammax || goto :error
copy "C:\qt\qt5.1.1\5.1.1\mingw48_32\bin\libwinpthread-1.dll" opengammax || goto :error
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\bin\Qt5Core.dll opengammax || goto :error
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\bin\icuin51.dll opengammax || goto :error
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\bin\icuuc51.dll opengammax || goto :error
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\bin\icudt51.dll opengammax || goto :error
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\bin\Qt5Gui.dll opengammax || goto :error
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\bin\Qt5Svg.dll opengammax || goto :error
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\bin\Qt5Widgets.dll opengammax || goto :error
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\bin\Qt5Xml.dll opengammax || goto :error
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\bin\Qt5OpenGL.dll opengammax || goto :error
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\bin\Qt5PrintSupport.dll opengammax || goto :error
copy c:\qwt-6.1.0\lib\qwt.dll opengammax || goto :error
copy ..\..\gsl-1.16\.libs\libgsl-0.dll opengammax || goto :error
copy ..\..\gsl-1.16\cblas\.libs\libgslcblas-0.dll opengammax || goto :error
copy ..\build-opengammamathfuncs-Desktop_Qt_5_1_1_MinGW_32bit-Release\Release\opengammamathfuncs.dll opengammax || goto :error
copy ..\build-opengammax-Desktop_Qt_5_1_1_MinGW_32bit-Release\Release\opengammax.exe opengammax || goto :error
mkdir opengammax\models || goto :error
mkdir opengammax\models\images || goto :error
copy ..\models\images\* opengammax\models\images\ || goto :error
mkdir opengammax\testfiles || goto :error
copy ..\testfiles\* opengammax\testfiles\ || goto :error
mkdir opengammax\nuclide_libraries || goto :error
copy ..\nuclide_libraries\* opengammax\nuclide_libraries\ || goto :error
copy qt-win32.conf opengammax\qt.conf || goto :error
mkdir opengammax\plugins
mkdir opengammax\plugins\platforms
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\plugins\platforms\qwindows.dll opengammax\plugins\platforms || goto :error
mkdir opengammax\plugins\imageformats
copy c:\qt\qt5.1.1\5.1.1\mingw48_32\plugins\imageformats\*.dll opengammax\plugins\imageformats || goto :error
goto :EOF

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
