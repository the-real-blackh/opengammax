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
#include "sample.h"


bool SpectrumIO::readOgammsp(const QString &fileName, Sample & samp)
{
#if 0
//-->finish this
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return false;

    spectrumTbl.clear();
    QDataStream in(&file);
    in >> spectrumTbl >> ioSamDesc >> ioQtyVal >> ioQtyUnc >> ioPhysUnit
            >> ioStrStart >> ioStrLT >> ioStrRT >> ioEfficEnPts >> ioEnxchan >> ioFwhmxch;

    if ( spectrumTbl.isEmpty() ) return NULL;
    Counts *counts = samp->getCounts();
    counts->updateCounts(&spectrumTbl);
    samp->setCounts(counts);
     //
    m_acqDateTime = QDateTime::fromString( ioStrStart, "dd/MM/yyyy HH:mm:ss");

    if ( !m_acqDateTime.isValid() )
        m_acqDateTime = QDateTime::fromString( "01/01/2000 00:00:01", "dd/MM/yyyy HH:mm:ss");
    samp->setAcqDate(m_acqDateTime);
    // setting members
    samp->setLiveTIme( ioStrLT.toFloat());
    samp->setRealTime( ioStrRT.toFloat());
    samp->setNumChan( spectrumTbl.count());

    ioEnParms.clear();
    ioFwhmParms.clear();
    for (int i=0; i < ioEnxchan.count(); i++) ioEnParms   << ioEnxchan[i].toDouble();
    for (int i=0; i < ioFwhmxch.count(); i++) ioFwhmParms << ioFwhmxch[i].toDouble();
#endif
    return TRUE;
}

/*

bool SpectrumIO::saveOpenGammaSpectrum( const QString &fileName,
      Intlst counts,
      QString samDesc,
      QString startDate;
      QString qtyVal,
      QString qtyUnc,
      QString physUnit,
      QString strStart,
      QString strLT,
      QString strRT,
      QStringList ioEfficEnPts,
      QStringList ioEnxchan,
      QStringList ioFwhmxch )
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    else {
        QDataStream out(&file);
        out << counts << samDesc << qtyVal << qtyUnc << physUnit << strStart
                << strLT << strRT << ioEfficEnPts << ioEnxchan << ioFwhmxch;
        return true;
    }
}

*/
