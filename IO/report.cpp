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

QString SpectrumIO::reportSet( QList<QList<QList<qreal> > > set )
{
    QString rep;
    for (int i=0; i < set.size(); i++) {
        rep.append("\n");
        for (int j=0; j < set[i].size(); j++) {
            QString ln = "\n";
            for (int k=0; k < set[i][j].size(); k++) {
                ln.append( QString("%1 * ").arg( set[i][j][k] ) );
            }
            rep.append( ln );
        }
        rep.append( QString("\n **%1**").arg( set[i].size() ) );
    }
    rep.append( QString("\n ***%1***").arg( set.size() ) );
    return rep;
}

QString SpectrumIO::reportSetToFile( QList<QList<QList<qreal> > > set, QStringList names )
{
    QString rep;
    QString ln = "\n";
    for (int i=0; i < set.size(); i++) {
        rep.append("\n");
        for (int j=0; j < set[i].size(); j++) {
            for (int k=0; k < set[i][j].size(); k++) {
                ln.append( QString("%1 * ").arg( set[i][j][k] ) );
            }
            rep.append( ln );
        }
        rep.append( QString("\n **%1**").arg( set[i].size() ) );
    }
    rep.append( QString("\n ***%1***").arg( set.size() ) );
    rep.append( names.join( ln ) );
    return rep;
}


