/***************************************************************************
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


#ifndef REPORTS_H
#define REPORTS_H

#include <QObject>
#include <QString>

#include "counts.h"
#include "peaklist.h"
#include "multiplets.h"

class Reports : public QObject
{
    Q_OBJECT
public:
    Reports( QObject *parent=0 ) :
      QObject( parent ) {}
    QString reportCandidates( Counts *cts );
    QString reportPeakWidths( PeakList *pklist, GenericCalibration *enCal );
    QString reportPeakWidthsAndFit( PeakList *pklist, GenericCalibration *enCal,
                                       GenericCalibration *fwhmCal, int fwhmFitType );
    QString reportRegions( Multiplets *multiplets );
    QString reportBaseLines( Multiplets *multiplets );
    QString reportBaseLinesFit( Multiplets *multiplets );
};

#endif // REPORTS_H
