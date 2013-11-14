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


#ifndef SAMPLE_H
#define SAMPLE_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QTextStream>
#include <QDataStream>
#include <QFile>
#include "doblst.h"
#include "counts.h"
#include <qdatetime.h>

#define DISPLAY_CHANNELS 4096

class Sample : public QObject
{
    Q_OBJECT
public:
    Sample(Counts *cnts){counts = cnts;}
   bool  clear();
    int enFitType() { return ioEnFitType; }
    Doblst enParms() { return ioEnParms; }
    int fwhmFitType() { return ioFwhmFitType; }
    Doblst fwhmParms() { return ioFwhmParms; }
    Doblst efficParms() { return ioEfficParms; }

    QString fileName;   // source of sample
    QString remarks;
    Counts *counts;
    float sampleSize;
    float sampleUncert;
    int sampleUnits;
    float realTime;
    float liveTime;
    QDateTime acqDateTime;
    QDateTime sampleDateTime;
    int numChans;
    int ioEnFitType;
    Doblst ioEnParms;
    int ioFwhmFitType;
    Doblst ioFwhmParms;
    QStringList ioEfficEnPts;
    Doblst ioEfficParms;
    QStringList ioEnxchan;
    QStringList ioFwhmxch;
private:


};

#endif // SAMPLE_H
