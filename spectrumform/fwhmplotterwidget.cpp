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


#include "fwhmplotterwidget.h"

//
// energy & FWHM plotter:
//

FwhmPlotterWidget::FwhmPlotterWidget( QString &title, QWidget *parent )
  : PlotterWidget( title, false, 0.0, 0.1, QwtPlotCurve::Lines, parent )
{
    hidePlotGrpbx();
    FunctionRange *fr1 = new FunctionRange();
    funcRngSet1->append( fr1 );
}

void FwhmPlotterWidget::setNCh( int intNCh )
{
    nCh = qreal( intNCh );
}

void FwhmPlotterWidget::plotFwhmPoints ()
{
    if ( !fwhms.isEmpty() ) {
        setErrBarsAllEnabled( energies, fwhms, sFwhms );
        // setErrBarsWithStatusAlreadySet( energies, fwhms, sFwhms );
        ui.plot->setAxisScale( QwtPlot::xBottom, 0.0, 3000.0 );
        ui.plot->setAxisScale( QwtPlot::yLeft, 0.1, 7.0 );
        plotScatterPlot( Qt::yellow, Qt::darkBlue, Qt::cyan, Qt::green, 6 );
    }
    else
        unplotErrBars();
}

void FwhmPlotterWidget::plotFwhmFit( Doblst enPms, Doblst fwhmPms,
  qreal (*fwhmFunc) (Doblst, qreal), qreal (*enAndFwhmFunc) (QList<Doblst>, qreal) )
{
    ui.plot->setTitle("FWHM (keV) vs. Energy (keV)" );
    if ( !fwhmPms.isEmpty() ) {
        enParms = enPms;
        fwhmParms = fwhmPms;
        FunctionRange *fr1 = funcRngSet1->at(0);
        fr1->typeParam = FunctionRange::xyMatrParms;
        fr1->x = FunctionsDefinitions::poly;
        fr1->parmsx = enParms;
        fr1->yMatr = enAndFwhmFunc;
        QList<Doblst> enAndFwhmParms;
        enAndFwhmParms << enParms << fwhmParms;
        fr1->parmsyVec = enAndFwhmParms;
        fr1->t0 = 0.0;
        fr1->t1 = nCh;
        ui.plot->setAxisScale( QwtPlot::xBottom, 0.0, 3000.0 );
        ui.plot->setAxisScale( QwtPlot::yLeft, 0.1, 7.0 );
        plotCurveSet( crvSet1, funcRngSet1, Qt::darkBlue );
    }
    else
        unplotFit();
}

void FwhmPlotterWidget::unplotFit()
{
    unplotCurveSet( crvSet1 );
}

void FwhmPlotterWidget::unplotErrBars()
{
    Doblst empty;
    // setErrBarsWithStatusAlreadySet( empty, empty, empty );
    setErrBarsAllEnabled( empty, empty, empty );
    plotScatterPlot( Qt::yellow, Qt::darkGreen, Qt::cyan, Qt::blue, 3 );
}

void FwhmPlotterWidget::setMultidimPoints( Doblst pma, Doblst pmb, Doblst pmc,
                                                 Doblst pmd, Doblst pme )
{
    centroids = pma;
    energies = pmb;
    sEnergies = pmc;
    fwhms = pmd;
    sFwhms = pme;
    // setEnabledListAllEnabled( centroids.count() );
}
