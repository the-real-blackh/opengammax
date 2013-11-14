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


#include "efficiencyplotterwidget.h"

//
// efficiency plotter:
//

EfficiencyPlotterWidget::EfficiencyPlotterWidget( QString &title, QWidget *parent )
  : PlotterWidget( title, FALSE, 0.0, 0.01,QwtPlotCurve::Lines, parent )
{
    QCheckBox *ckbPlotEfficPoints = new QCheckBox("Points");
    QCheckBox *ckbPlotFit = new QCheckBox("Fit");

    QList<QCheckBox *> setsCheckBoxes;
    setsCheckBoxes <<
      ckbPlotEfficPoints << ckbPlotFit;
    addSetsCheckBoxes( setsCheckBoxes );

    connect( ckbPlotFit, SIGNAL(stateChanged(int)), this, SLOT( changed1(int)) );
    connect( ckbPlotEfficPoints, SIGNAL(stateChanged(int)), this, SLOT( changedErrBars(int)) );

    FunctionRange *fr1 = new FunctionRange();
    fr1->typeParam = FunctionRange::yParms;
    fr1->y = FunctionsDefinitions::poly;
    funcRngSet1->append( fr1 );
}

void EfficiencyPlotterWidget::plotBarsAndCurve( Doblst xlst, Doblst ylst, Doblst slst, Doblst parms )
{
    setEfficiencyErrBars( xlst, ylst, slst );
    plotScatterPlot( Qt::darkYellow, Qt::darkGreen, Qt::cyan, Qt::blue, 5 );
    funcRngSet1->at(0)->y = FunctionsDefinitions::polyLog;
    funcRngSet1->at(0)->parmsy = parms;
    funcRngSet1->at(0)->t0 = 30.0; //           the effic curve begins at 30 keV
    funcRngSet1->at(0)->t1 = xlst.max();

    plotCurveSet( crvSet1, funcRngSet1, Qt::magenta );
}

void EfficiencyPlotterWidget::setEfficiencyErrBars( Doblst x, Doblst y, Doblst s)
{
    setErrBarsAllEnabled( x, y, s );
}

void EfficiencyPlotterWidget::changed1(int state )
{
    if ( state == Qt::Checked )
        showCurveSet( crvSet1 );
    else
        hideCurveSet( crvSet1 );
}

void EfficiencyPlotterWidget::changedErrBars(int state )
{
    if ( state == Qt::Checked )
        errBarsQpu->show();
    else
        errBarsQpu->hide();
}
