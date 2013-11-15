# *************************************************************************
# Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com
# Copyright (C) 2007, 2010 by Marcelo Francis Maduar
# maduar@gmail.com
# This file is part of OpenGamma.
# OpenGamma is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# OpenGamma is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with OpenGammaX. If not, see <http://www.gnu.org/licenses/>.
# *************************************************************************
# -------------------------------------------------
# Project created by QtCreator 2009-11-06T10:54:08
# -------------------------------------------------
# Qwt config
win32 {
#    INCLUDEPATH += c:/qwt-5.2.1/src
#    DEFINES += QWT_DLL
#    LIBS += c:/qwt-5.2.1/lib/qwt5.dll
#    LIBS += c:/qwt-5.2.1/lib/qwtd5.dll
  QWT_LOCATION = c:\qwt-6.1.0

  INCLUDEPATH +=  $${QWT_LOCATION}/include/
  LIBS =          -L$${QWT_LOCATION}/lib -lqwt
  
  #INCLUDEPATH += c:\qt\qt5.1.1\5.1.1\mingw48_32\include
  #CONFIG += console
}
unix {
    INCLUDEPATH += /usr/include/qt4/qt /usr/local/qwt-6.1.0/include
    LIBS += -L/usr/local/qwt-6.1.0/lib -lqwt -lasound
    DEFINES += HAVE_ALSA
}


# Qwt config end
# GnuWin32 / GSL config:
GSL_ROOT=../../gsl-1.16/
win32 {
    LIBS += -L$${GSL_ROOT}/.libs -L$${GSL_ROOT}/cblas/.libs
    INCLUDEPATH += $${GSL_ROOT}
}
LIBS += -lgsl \
    -lgslcblas

# End of GNU GSL entries
# Internal library
INCLUDEPATH += ../opengammamathfuncs
win32:LIBS += -L../build-opengammamathfuncs-Desktop_Qt_5_1_1_MinGW_32bit-Release/release \
    -lopengammamathfuncs -lopengammamathfuncs -lqwt
unix:LIBS += -L./../opengammamathfuncs -L/usr/local/qwt-5.2.2/lib\
    -lopengammamathfuncs -lqwt

# End of internal library
# OpenGamma config:
INCLUDEPATH += ../maininterface \
    ../directories \
    ../spectrumform \
    ../languages \
    ../models \
    ../src \
    ../gslcontainer \
    ../plotterwidget \
    ../IO

QT += opengl \
    sql \
    svg \
    xml

TARGET = opengammax

TEMPLATE = app

SOURCES += ../languages/languages.cpp \
    ../maininterface/maininterface.cpp \
    ../spectrumform/spectrumform.cpp \
    ../src/peaklist.cpp \
    ../src/genericcalibration.cpp \
    ../src/gammaspectrumtree.cpp \
    ../src/analysisoutput.cpp \
    ../src/nuclide.cpp \
    ../directories/directories.cpp \
    ../gslcontainer/gslcontainer.cpp \
    ../plotterwidget/plotterwidget.cpp \
    ../src/counts.cpp \
    ../src/multiplets.cpp \
    ../spectrumform/reports.cpp \
    ../spectrumform/energypointentry.cpp \
    ../spectrumform/energypicker.cpp \
    ../spectrumform/efficiencyplotterwidget.cpp \
    ../spectrumform/countsplotterwidget.cpp \
    ../src/sample.cpp \
    ../spectrumform/fwhmplotterwidget.cpp \
    ../spectrumform/energyplotterwidget.cpp \
    ../models/spectrumformmodels.cpp \
    ../IO/spectrio.cpp \
    ../IO/smallStuff.cpp \
    ../IO/report.cpp \
    ../IO/ortec.cpp \
    ../IO/ogammsp.cpp \
    ../IO/NISTn42.cpp \
    ../IO/camberra.cpp \
    ../IO/InterWinner.cpp \
    ../maininterface/main.cpp \
    ../plotterwidget/qwtplotcurvewithuncert.cpp \
    ../plotterwidget/style.cpp \
    ../maininterface/signalSettings.cpp

unix {
    SOURCES += ../IO/alsaStream.cpp
}

HEADERS += ../languages/languages.h \
    ../maininterface/maininterface.h \
    ../spectrumform/spectrumform.h \
    ../src/peaklist.h \
    ../src/peak.h \
    ../src/genericcalibration.h \
    ../src/gammaspectrumtree.h \
    ../src/constants.h \
    ../src/analysisoutput.h \
    ../src/acquisition.h \
    ../src/nuclide.h \
    ../directories/directories.h \
    ../gslcontainer/gslcontainer.h \
    ../plotterwidget/plotterwidget.h \
    ../plotterwidget/functionrange.h \
    ../src/counts.h \
    ../src/multiplets.h \
    ../spectrumform/energypicker.h \
    ../spectrumform/energypointentry.h \
    ../spectrumform/reports.h \
    ../src/sample.h \
    ../spectrumform/countsplotterwidget.h \
    ../spectrumform/efficiencyplotterwidget.h \
    ../gslcontainer/doblst.h \
    ../gslcontainer/functionsdefinitions.h \
    ../spectrumform/fwhmplotterwidget.h \
    ../spectrumform/energyplotterwidget.h \
    ../models/spectrumformmodels.h \
    ../opengammamathfuncs/opengammamathfuncs_global.h \
    ../opengammamathfuncs/opengammamathfuncs.h \
    ../IO/spectrio.h \
    ../plotterwidget/qwtplotcurvewithuncert.h \
    ../plotterwidget/style.h \
    ../maininterface/signalSettings.h

unix {
    HEADERS += ../IO/alsaStream.h
}

FORMS += ../languages/languages.ui \
    ../maininterface/maininterface.ui \
    ../spectrumform/spectrumform.ui \
    ../directories/directories.ui \
    ../plotterwidget/plotterwidget.ui \
    ../spectrumform/energypointentry.ui \
    ../plotterwidget/style.ui \
    ../maininterface/signalSettings.ui
OTHER_FILES += ../license.txt \
    ../gpl.txt \
    ../opengamma_pt-BR.ts \
    nuclide_libraries/AIEA-2007-soil.lib \
    nuclide_libraries/BasicNuclideLibrary.lib \
    nuclide_libraries/pisciro.lib \
    nuclide_libraries/NPL-2008-GH.lib \
    nuclide_libraries/NPL-2008.lib \
    README \
    Makefile \
    ../opengammamathfuncs/opengammamathfuncs.pro.user \
    ../opengammamathfuncs/opengammamathfuncs.pro \
    ../opengammamathfuncs/Makefile \
    ../IO/spectrioNUcpp \
    ../nuclide_libraries/pisciro.lib \
    ../nuclide_libraries/NPL-2008-GH.lib \
    ../nuclide_libraries/NPL-2008.lib \
    ../nuclide_libraries/BasicNuclideLibrary.lib \
    ../nuclide_libraries/AIEA-2007-soil.lib \
    ../testfiles/spec0034.n42 \
    ../testfiles/spec0034.n42.rtf \
    ../testfiles/spec0034.n42.png \
    ../testfiles/README.TXT \
    ../testfiles/newest.n42 \
    ../testfiles/newer.n42 \
    ../testfiles/new.n42 \
    ../testfiles/na22.dat \
    ../testfiles/N42SpectrometerExample.n42 \
    ../testfiles/mn54.dat \
    ../testfiles/G1400.n42 \
    ../testfiles/G1400.MCA \
    ../testfiles/G1400.CHN \
    ../testfiles/G1400.ASC \
    ../testfiles/G1305.n42 \
    ../testfiles/G1305.MCA \
    ../testfiles/G1305.CHN \
    ../testfiles/G1305.ASC \
    ../testfiles/G1304.svg \
    ../testfiles/G1304.n42 \
    ../testfiles/G1304.MCA \
    ../testfiles/G1304.CHN \
    ../testfiles/G1304.ASC \
    ../testfiles/G1303.n42 \
    ../testfiles/G1303.MCA \
    ../testfiles/G1303.CHN \
    ../testfiles/G1303.ASC \
    ../testfiles/G1302.ogammsp \
    ../testfiles/G1302.n42 \
    ../testfiles/G1302.MCA \
    ../testfiles/G1302.CHN \
    ../testfiles/G1302.ASC \
    ../testfiles/G1301.n42 \
    ../testfiles/G1301.MCA \
    ../testfiles/G1301.CHN \
    ../testfiles/G1301.ASC \
    ../testfiles/G1300.n42 \
    ../testfiles/G1300.MCA \
    ../testfiles/G1300.CHN \
    ../testfiles/G1300.ASC \
    ../testfiles/G1200.n42 \
    ../testfiles/G1200.MCA \
    ../testfiles/G1200.ILT \
    ../testfiles/G1200.CHN \
    ../testfiles/G1100.n42 \
    ../testfiles/G1100.MCA \
    ../testfiles/G1100.CHN \
    ../testfiles/G1100.ASC \
    ../testfiles/G1_DATA.WRI \
    ../testfiles/G1_DATA.TXT \
    ../testfiles/cs137.n42 \
    ../testfiles/cs137.dat \
    ../testfiles/co60 (copy).n42 \
    ../testfiles/co60.n42 \
    ../testfiles/co60.dat \
    ../testfiles/co57.n42 \
    ../testfiles/co57.dat \
    ../testfiles/chnfile.n42

#../readme.txt \

RESOURCES += ./mdi.qrc

