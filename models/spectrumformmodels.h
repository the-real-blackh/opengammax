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

#ifndef SPECTRUMFORMMODELS_H
#define SPECTRUMFORMMODELS_H

#include <QObject>
#include <QStandardItemModel>
#include <QString>

#include "peaklist.h"
#include "nuclide.h"

class SpectrumFormModels : public QObject
{
    Q_OBJECT
public:
    // setup models:
    SpectrumFormModels( QObject *parent = 0 );

    Intlst positions( Intlst lst, int p );
    Doblst newListByPosits( Doblst lst, Intlst pos );

    void setupRawDataMdl();
    void fillRawDataMdl( Intlst & counts );
        // Energy:
    void setupEnergypointsMdl();
    void setupEnxchanMdl();
    void setupFwhmxchMdl();
    void setupEfficPtsMdl();
    void setupeffxenMdl();

    void setupNuclideTabMdl();
    void setupGammaTabMdl();
    void setupGammasPerNuclideMdl();

    void changeNEfficPts( int nPt);

    void fillStdItemModel( QStandardItemModel *model, const QString &contents );
    void fillGammasPerNuclideMdl();

    void fillEnergypointsMdl(
            PeakList *pklist,
            GenericCalibration *enCal,
            GenericCalibration *fwhmCal,
            GammaList *gammaList,
            NuclideList *nuclideList,
            qreal toler );

    void fillEnxchanMdl( Doblst enParms );
    void fillFwhmxchMdl( Doblst fwhmParms );
    void fillEfficPtsMdl( QStringList zzz );
    void filleffxenMdl( Doblst effParms );

    void setupPeaksFitMdl();
    void fillPeaksFitMdl(
            PeakList *pkl,
            GenericCalibration *enCal,
            GenericCalibration *fwhmCal,
            GenericCalibration *effCal,
            GammaList *gammaList,
            NuclideList *nuclideList,
            qreal toler,
            qreal lt,
            qreal delayInDays );
    void setupActivitiesMdl();
    void fillActivitiesMdl( PeakList *pklist, GenericCalibration *enCal, GenericCalibration *fwhmCal,
                            qreal qty, const QString &unit );
    void setupPartialActivitiesMdl();

    QStandardItemModel *rawDataMdl;
    QStandardItemModel *energypointsMdl;
    QStandardItemModel *enxchanMdl;
    QStandardItemModel *fwhmxchMdl;
    // QStandardItemModel *efficPtsMdl;
    QStandardItemModel *efficPtsMdl;
    QStandardItemModel *effxenMdl;
    QStandardItemModel *peaksFitMdl;
    QStandardItemModel *activitiesMdl;
    QStandardItemModel *partialActivitiesMdl;

    QStandardItemModel *nuclideTabMdl;
    QStandardItemModel *gammaTabMdl;
    QStandardItemModel *gammasPerNuclideMdl;

private:
    qreal weiMean( Doblst vals, Doblst uncs );
    qreal weiUnc( Doblst uncs );

};

#endif // SPECTRUMFORMMODELS_H
