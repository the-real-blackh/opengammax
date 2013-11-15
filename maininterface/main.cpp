/***************************************************************************
 *   Copyright (C) 2007, 2009 by Marcelo Francis Maduar                    *
 *   maduar@gmail.com                                                      *
 *                                                                         *
 *   This file is part of OpenGamma.                                       *
 *                                                                         *
 *   OpenGamma is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   OpenGamma is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with OpenGamma. If not, see <http://www.gnu.org/licenses/>.     *
 ***************************************************************************/

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QSettings>
#include <QDir>
#include <QImageReader>

#include "maininterface.h"

#include <windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR args, int n)
{
    int main(int argc, char *argv[]);
    char* argv[2] = {"hello", NULL};
    return main(1, argv);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(mdi);

    qDebug() << QDir(":/models/images").entryList() << endl;
    qDebug() << QImageReader::supportedImageFormats() << endl;

    QSettings settings("OpenGamma", "Settings");
    QString app_lang = QString( settings.value("lang", "en").toString() );

    QTranslator opengammaTranslator;
    opengammaTranslator.load("opengamma_" + app_lang);
    a.installTranslator(&opengammaTranslator);

    MainInterface w;
    w.show();

    return a.exec();
}
