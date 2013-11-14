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

#ifndef CAPTURE_H
#define CAPTURE_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QTextStream>
#include <QDataStream>
#include <QFile>
#include "alsaStream.h"

class Capture : public QObject
{
    Q_OBJECT
public:
    Capture ( QObject *parent = 0 )
        : QObject( parent ) {}
    bool openSrc( const QString);
    QList <int> rdSrc(void);
    QList <int> rdSrcPulse(void);
    bool srcCapture(void);
    QString srcName;
    FILE srcFd;

    bool openDst( const QString);    
    bool wrDst(QList <int>);
    QString dstName;
    QFile *dstQf;
    bool openN42(const QString fileName);
    bool wrN42(QList <int>);


private:
    int nchs;           // number of channels (ADC resolution)
    QString acqDate;
    QString acqTime;
    QString m_filename;
    QDataStream* inDst;
    QTextStream* inTst;
    QDateTime m_acqDateTime;
    QDateTime m_samDateTime;
    AlsaStream alsa;    // data sources
};

#endif /*CAPTURE_H*/
