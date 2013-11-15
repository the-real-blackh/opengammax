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

#ifndef COUNTSPLOTTERWIDGET_H
#define COUNTSPLOTTERWIDGET_H

#include <QStandardItemModel>

#include "../gslcontainer/doblst.h"
#include "../gslcontainer/functionsdefinitions.h"
#include "plotterwidget.h"
#include "peaklist.h"
#include "multiplets.h"
#include "energypicker.h"
#include "energypointentry.h"

class CountsPlotterWidget : public PlotterWidget
{
    Q_OBJECT
public:
    CountsPlotterWidget( QString &title, Doblst enParms, int style, QWidget *parent=0 );
    void clearRangesAndPlots();
    void setCountsErrBars( Doblst x, Doblst y, Doblst s);
    void setPickerEnCal( Doblst );
    void setPickerEnCalDefaultCalibration();
    void setupGammasHintsMdl();
    void fillGammasHintsMdl( QStandardItemModel *from );
    void markCentroids ( Doblst cs, Doblst grossHei );  // centroids
    void markSideBaseLinesExpSimple( PeakList *pks, Multiplets *mts );
    void markSideBaseLinesPoly( PeakList *pks, Multiplets *mts );
    void markSideBaseLinesExpPlusC( PeakList *pks, Multiplets *mts );
    void markInnerBaseLinesCubic( Multiplets *mts );
    void markInnerBaseLinesPeakHeiWei( Multiplets *mts );
    void markMultiplets( Multiplets *mts );
    void saveSvgPlot( const QString& flnm );

private slots:
    void changed1and3( int state );
    void changed2( int state );
    // void changed4( int state );
    void changedErrBars( int state );
    void selectedPoint(const QPointF &);

signals:
    void askForNewSearch();

private:
    EnergyPicker *d_picker;
    QStandardItemModel *gammasHintsMdl;
};

#endif // COUNTSPLOTTERWIDGET_H
