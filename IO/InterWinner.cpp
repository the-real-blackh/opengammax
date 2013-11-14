/***************************************************************************
 * # Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com
 *   Copyright (C) 2007, 2009 by Marcelo Francis Maduar, maduar@gmail.com                                                      *
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

#include <qmath.h>
#include <QVariant>
#include <QFile>
#include <QDirIterator>
#include <QStringList>
#include <QProgressDialog>

#include "spectrio.h"


QList<QList<qreal> > SpectrumIO::getInterwinnerOutputFile( const QString &fileName )
{
    QFile file(fileName);
    QList<QList<qreal> > data;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QList<qreal> rl;
        rl << 0.0;
        data << rl;
        return data;
    }
    QTextStream in(&file);
    // parse Interwinner output
    QStringList lines = in.readAll().split("\n");

    int i = 0;
    int j = 0;
    QString ln;
    if ( lines.size() > 0 )
        do {
            ln = lines[i];
            ++i;
        }
        while ( (ln.mid(10, 16) != "List by energies") && (i < lines.size() ) );

    if ( (i < lines.size() ) && (i > 0) )
    {
        j = i + 3;
        if ( j < lines.size() ) {
            ln = lines.at(j);
            while ( (ln.left(1) == "|" ) && ((j+1) < lines.size()) ) {
                QList<qreal> reallist;
                qreal energy  = ln.mid( 6, 7 ).toDouble();
                qreal fwhm    = ln.mid(15, 5 ).toDouble();
                qreal gross   = ln.mid(27,11 ).toDouble();
                qreal net     = ln.mid(39,11 ).toDouble();
                qreal uncert  = ln.mid(51, 8 ).toDouble();
                reallist << energy << fwhm << gross << net << uncert;
                data << reallist;
                j++;
                ln = lines.at(j);
            }
        }
    }
    return data;
}
