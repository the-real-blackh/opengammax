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


#include "energyplotterwidget.h"

//
// energy & FWHM plotter:
//

EnergyPlotterWidget::EnergyPlotterWidget( QString &title, QWidget *parent )
  : PlotterWidget( title, FALSE, 0.0, 0.1,QwtPlotCurve::Lines,  parent )
{
    hidePlotGrpbx();
    FunctionRange *fr1 = new FunctionRange();
    funcRngSet1->append( fr1 );
}

void EnergyPlotterWidget::setNCh( int intNCh )
{
    nCh = qreal( intNCh );
}

void EnergyPlotterWidget::plotEnergyPoints()
{
    if ( !energies.isEmpty() ) {
        setErrBarsAllEnabled( centroids, energies, sEnergies );
        ui.plot->setAxisScale( QwtPlot::xBottom, 0.0, nCh );
        ui.plot->setAxisScale( QwtPlot::yLeft, 0.1, 3000.0 );
        plotScatterPlot( Qt::gray, Qt::green, Qt::cyan, Qt::yellow, 6 );
    }
    else
        unplotErrBars();
}

void EnergyPlotterWidget::plotEnergyFit( Doblst enPms, qreal (*enFunc) (Doblst, qreal) )
{
    ui.plot->setTitle("Energy (keV) vs. channel" );
    if ( !enPms.isEmpty() ) {
        enParms = enPms;
        FunctionRange *fr1 = funcRngSet1->at(0);
        fr1->typeParam = FunctionRange::yParms;
        fr1->y = enFunc;
        fr1->parmsy = enParms;
        fr1->t0 = 0.0;
        fr1->t1 = nCh;
        ui.plot->setAxisScale( QwtPlot::xBottom, 0.0, nCh );
        ui.plot->setAxisScale( QwtPlot::yLeft, 0.1, 3000.0 );
        plotCurveSet( crvSet1, funcRngSet1, Qt::darkGreen );
    }
    else
        unplotFit();
}

void EnergyPlotterWidget::unplotFit()
{
    unplotCurveSet( crvSet1 );
}

void EnergyPlotterWidget::unplotErrBars()
{
    Doblst empty;
    setErrBarsAllEnabled( empty, empty, empty );
    plotScatterPlot( Qt::yellow, Qt::darkGreen, Qt::cyan, Qt::blue, 3 );
}

void EnergyPlotterWidget::setMultidimPoints( Doblst pma, Doblst pmb, Doblst pmc,
                                                 Doblst pmd, Doblst pme )
{
    centroids = pma;
    energies = pmb;
    sEnergies = pmc;
    fwhms = pmd;
    sFwhms = pme;
}
