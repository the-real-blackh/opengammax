# *************************************************************************
# Copyright (C) 2007, 2009 by Marcelo Francis Maduar
# maduar@gmail.com
# Copyright (C) 2011 by LeDoyle Roy Pingel
# ledoylepingel@hughes.net
#
# This file is part of OpenGammaX.
# OpenGammaX is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# OpenGammaX is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with OpenGammaX. If not, see <http://www.gnu.org/licenses/>.
# *************************************************************************

# ********** ATENCAO *********************
# ESTE PROJETO FOI CONGELADO EM 23/04/2010
# DEIXEI APENAS COMO TEMPLATE PARA PROJETOS DE BIBLIOTECAS:
# SO (LINUX) ou DLL (WINDOWS)
#
# THIS PROJECT HAS BEEN FROZEN IN 23/04/2010
# LEFT ONLY AS A TEMPLATE FOR DESIGN OF LIBRARIES:
# OS (LINUX) or DLL (WINDOWS)
#
# Unfrozen 1/6/2011
# Being adapted to provide open MCA software


QT -= gui
TARGET = opengammamathfuncs
TEMPLATE = lib
DEFINES += OPENGAMMAMATHFUNCS_LIBRARY
SOURCES += opengammamathfuncs.cpp
HEADERS += opengammamathfuncs.h \
    opengammamathfuncs_global.h \
    doblst.h
win32:win32-g++ { 
    LIBS += -L../../gsl-1.16/.libs -L../../gsl-1.16/cblas/.libs
    INCLUDEPATH += ../../gsl-1.16
}
LIBS += -lgsl \
    -lgslcblas

unix:DESTDIR = ../opengammamathfuncs

