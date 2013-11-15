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
#include "sample.h"
#include "spectrio.h"


bool SpectrumIO::readASC(const QString& fileName, Sample & sp)
{
    bool ret = true;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) return false;
    QTextStream in(&file);
    QStringList lines = in.readAll().split("\n");

    sp.realTime = (lines[0].toFloat( &ret ));
    sp.liveTime = (lines[1].toFloat( &ret ));

    Intlst ilist;
    if ( ret ) {
        ilist.clear();
        ilist << 0; // default channel 0
        for (int i=0; i < lines.count(); i++) {
            ilist << lines[i].toInt( &ret );
            if ( !ret ) break;
        }
        sp.counts->updateCounts(ilist);
    }
    //ret = ( m_RT > 0 );
    //ret = ( m_LT > 0 );
    //ret = ( lines.count() > 3 );
    return true;
}


/* NOTE: now listed under IEC 61455 */
bool SpectrumIO::readIEC1455(const QString& fileName, Sample & sample)
{
    m_filename = fileName;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return false;
    inTst = new QTextStream(&file);
    QStringList list;
    QString line;
    // all = inTst->readAll(); 2009-11-18 doesn't work, don't know why.

    do {
        line = inTst->readLine();
        list << line;
    } while (!line.isNull());
    int nLm1 = list.count() - 1;

    QProgressDialog progress("Parsing spectrum...", "Abort parsing", 0, nLm1 );
    progress.setWindowModality(Qt::WindowModal);

    sample.liveTime = (list[1].mid( 4, 14).toFloat());
    sample.realTime = (list[1].mid(18, 14).toFloat());

    QDateTime acqDateTime = toDateTimeCentury( list[2].mid( 4, 17 ) );
    sample.acqDateTime = acqDateTime;
    // m_samDateTime = toDateTimeCentury( list[2].mid( 22, 17 ));
    QString desc;
    for (int iL=5; iL <= 8; iL++) {
        desc += list[iL] + "\n";
    }
    sample.remarks = desc;
    int iL = 0;
    while ( (list[iL].left( 9) != "A004SPARE") && ( iL < nLm1 ) )
        iL++;
    if ( iL == nLm1 ) return NULL;
    while ( (list[iL].left(15) != "A004USERDEFINED" ) && ( iL < nLm1 ) )
        iL++;
    if ( iL == nLm1 ) return NULL;
    iL++;

    Intlst ilist;
    ilist.clear();
    ilist << 0; // default channel 0
    do {
        progress.setValue( iL );
         if (progress.wasCanceled())
             break;
        for ( int j=1; j<=5; j++ ) {
            ilist << list[iL].mid( 10*j, 10 ).toInt();
        }
        iL++;
    } while ( (!list[iL].isNull()) && (iL < nLm1) );
    sample.counts->updateCounts(ilist);
    return true;
}


bool SpectrumIO::readNuclideFile( const QString &fileName, QString &contents )
{
    bool loaded = false;
    if ( !fileName.isEmpty() ) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&file);
            contents = stream.readAll();
            loaded = true;
        }
    }
    return loaded;
}



