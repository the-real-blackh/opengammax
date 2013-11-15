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


quint16 nchs;

bool SpectrumIO::readCHN(const QString & fileName, Sample & sp)
{
    //m_filename = fileName
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return false;
    inDst = new QDataStream(&file);
    inDst->setByteOrder( QDataStream::LittleEndian );
    if (!getHeaderCHN(sp)) return false;
    if (!getCountsCHN(sp)) return false;
    if (!getFooterCHN(sp)) return false;
    return true;
}


bool SpectrumIO::getHeaderCHN(Sample & samp)
{
    qint16 Type;
    qint16 MCA;
    qint16 Segm;
    quint8 StartSec1;
    quint8 StartSec2;
    quint32 RLTime20ms;
    quint32 LVTime20ms;
    quint8 dd1;
    quint8 dd2;
    quint8 MM1;
    quint8 MM2;
    quint8 MM3;
    quint8 yy1;
    quint8 yy2;
    quint8 sep;
    quint8 hh1;
    quint8 hh2;
    quint8 mm1;
    quint8 mm2;
    quint16 ChOffset;
    quint16 LngData;

    QString centryMinus1 = "20";
    const int decadeLimit = 2;

    *inDst >> Type >> MCA >> Segm >> StartSec1 >> StartSec2 >>
      RLTime20ms >> LVTime20ms >> dd1 >> dd2 >> MM1 >> MM2 >> MM3 >>
      yy1 >> yy2 >> sep >> hh1 >> hh2 >> mm1 >> mm2  >>
      ChOffset >> LngData;

    // setting members
    samp.liveTime = ( 0.02 * LVTime20ms);
    samp.realTime = (0.02 * RLTime20ms);
    QString ddMMMyyyy = "";
    QString hhmm = "";
    ddMMMyyyy += dd1;
    ddMMMyyyy += dd2;
    ddMMMyyyy += MM1;
    ddMMMyyyy += MM2;
    ddMMMyyyy += MM3;
    QChar decadeChar(yy1);
    int decadeNum = decadeChar.digitValue();
        if (decadeNum > decadeLimit)
        centryMinus1 = "19";
    ddMMMyyyy += centryMinus1;
    ddMMMyyyy += yy1;
    ddMMMyyyy += yy2;
    hhmm += hh1;
    hhmm += hh2;
    hhmm += ":";
    hhmm += mm1;
    hhmm += mm2;
    hhmm += ":";
    hhmm += StartSec1;
    hhmm += StartSec2;
    acqDate = ddMMMyyyy;
    acqTime = hhmm;
    nchs = LngData;

    QLocale usdate( QLocale::English, QLocale::UnitedStates );
    QDateTime dt;
    dt = usdate.toDateTime(acqDate + " " + acqTime, "ddMMMyyyy hh:mm:ss");
    if ( !dt.isValid() )
        dt.fromString("01Jan2001 00:00:01", "ddMMMyyyy hh:mm:ss");
    samp.acqDateTime = dt;
    return true;
}

bool SpectrumIO::getCountsCHN(Sample & samp)
{
    Intlst ilist;
    ilist.clear();
    for (int i=0; i < nchs; i++)
    {
       int c;
       *inDst >> c;
       ilist << c;
    }
    samp.counts->updateCounts(ilist);
    return true;
}

bool SpectrumIO::getFooterCHN(Sample & samp)
{
    quint16 tlrtyp;
    quint16 isCal;

    quint32 uien0;
    quint32 uien1;
    quint32 uien2;
    quint32 uifw0;
    quint32 uifw1;
    quint32 uifw2;

    char reserved [228];
    quint8 samLen;
    quint8 detLen;
    char samDes [63];
    char detDes [63];

    *inDst >> tlrtyp >> isCal >> uien0 >> uien1 >> uien2 >> uifw0 >> uifw1 >> uifw2;
    inDst->readRawData( reserved, 228 );
    *inDst >> detLen;
    inDst->readRawData( detDes, 63 );
    *inDst >> samLen;
    inDst->readRawData( samDes, 63 );

    m_en0 = quint32tofloat(uien0);
    m_en1 = quint32tofloat(uien1);
    m_en2 = quint32tofloat(uien2);
    m_fw0 = quint32tofloat(uifw0);
    m_fw1 = quint32tofloat(uifw1);
    m_fw2 = quint32tofloat(uifw2);

    samp.ioEnFitType = 0;    // in CHN is always poly ( gr. 2 (3 params) or 1 (2 params) )
    samp.ioEnParms.clear();
    samp.ioEnParms << m_en0 << m_en1 << m_en2;
    if ( m_en2 < 1.0e-30 ) {
        samp.ioEnParms.removeLast();
    }
    samp.ioFwhmFitType = 1;    // in CHN is always  poly ( gr. 2 (3 params) or 1 (2 params) )
    samp.ioFwhmParms.clear();
    samp.ioFwhmParms << m_fw0 << m_fw1 << m_fw2;
    if ( m_fw2 < 1.0e-30 ) {
        samp.ioFwhmParms.removeLast();
    }
    samp.ioEfficParms.clear();
    samp.ioEfficEnPts.clear(); // no effic fit in CHN, initializing empty

    //--> QString::fromAscii( samDes, samLen ) + "\n\n" + QString::fromAscii( detDes, detLen );

    return true;
}

