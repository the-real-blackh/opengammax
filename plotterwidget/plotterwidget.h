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

#ifndef PLOTTERWIDGET_H
#define PLOTTERWIDGET_H

#include <QCheckBox>

#include <qwt_math.h>
#include <qwt_scale_engine.h>
#include <qwt_symbol.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_text.h>

#include "ui_plotterwidget.h"
#include "functionrange.h"
#include "qwtplotcurvewithuncert.h"

class PlotterWidget : public QWidget
{
    Q_OBJECT
public:
    PlotterWidget( QString &plotTitle,
      bool initYLog = false,
      qreal xLoLimit = 5.0,
      qreal yLoLimit = 5.0,
      int style = (int) QwtPlotCurve::Lines,
      QWidget *parent = 0 );
    ~PlotterWidget();
    QList<bool> isEnabledList;
protected slots:
    void logSelected();
    void linSelected();
    void panLeft();
    void panRight();
    void panUp();
    void panDown();
    void zoomInX();
    void zoomInY();
    void zoomOutX();
    void zoomOutY();
protected:
    void addSetsCheckBoxes ( QList<QCheckBox *> setsCheckBoxes );
    void hidePlotGrpbx();
    void setYLog( bool is ) { m_isYLog = is; }
    bool isYLog() { return m_isYLog; }
    void setErrBarsAllEnabled( Doblst xList, Doblst yList, Doblst sList );
    /*
    void setEnabledListAllEnabled( int nPts );
    void setErrBarsWithStatusAlreadySet
      ( Doblst xList, Doblst yList, Doblst sList );
    */
    void changeEnablingOfAPoint( int rw, bool enable );
    void setNCh( int intNCh );
    void plotScatterPlot( QColor c2, QColor c3, QColor c4, QColor c5, int dSize = 7 );
    void plotCurveSet( QList<QwtPlotCurve *> *cSet, QList<FunctionRange *> *frSet, QColor color );
    void unplotCurveSet( QList<QwtPlotCurve *> *cSet );
    void clearFunctionRangeSet( QList<FunctionRange *> *frSet );
    void clearPlotRangeSet( QList<QwtPlotCurve *> *plSet );
    void clearAllRangesAndCurvesSets();
    void showCurveSet( QList<QwtPlotCurve *> *cSet );
    void hideCurveSet( QList<QwtPlotCurve *> *cSet );
    void deleteLists();

    Ui::PlotterWidget ui;
    QwtPlotCurveWithUncert *errBarsQpu;
    //QwtPlotCurve *errBarsQpu;
    QList<QwtPlotCurve *> *crvSet1;
    QList<QwtPlotCurve *> *crvSet2;
    QList<QwtPlotCurve *> *crvSet3;
    QList<QwtPlotCurve *> *crvSet4;
    QList<FunctionRange *> *funcRngSet1;
    QList<FunctionRange *> *funcRngSet2;
    QList<FunctionRange *> *funcRngSet3;
    QList<FunctionRange *> *funcRngSet4;

    void saveSvg( const QString& flnm );

private:
    void plotFunctionRange( QwtPlotCurve*, QColor, FunctionRange* );
    void prepareToPlot( QList<QwtPlotCurve *> *cSet );
    void finalizePlot();
    bool m_isYLog;
    qreal m_xLoLimit;
    qreal m_yLoLimit;
    bool doReplot;
    void showErrorBars( double *x, double *y, double *s, int nPt, QwtPlot *pl );
};

#endif // PLOTTERWIDGET_H
