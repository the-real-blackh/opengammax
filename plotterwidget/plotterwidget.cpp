/***************************************************************************
 *  Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com
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

#include <QSvgGenerator>
#include <gsl/gsl_math.h>
#include <qwt_plot_renderer.h>

#include "plotterwidget.h"

PlotterWidget::PlotterWidget( QString &plotTitle,
                              bool initYLog,
                              qreal xLoLimit,
                              qreal yLoLimit,
                              int style,
                              QWidget *parent )
    : QWidget(parent)
{
    ui.setupUi(this);
    crvSet1 = new QList<QwtPlotCurve *>;
    crvSet2 = new QList<QwtPlotCurve *>;
    crvSet3 = new QList<QwtPlotCurve *>;
    crvSet4 = new QList<QwtPlotCurve *>;

    funcRngSet1 = new QList<FunctionRange *>;
    funcRngSet2 = new QList<FunctionRange *>;
    funcRngSet3 = new QList<FunctionRange *>;
    funcRngSet4 = new QList<FunctionRange *>;

    errBarsQpu = new QwtPlotCurveWithUncert;
    errBarsQpu->setStyle( QwtPlotCurve::CurveStyle( QwtPlotCurveWithUncert::Dots ));


    connect(ui.linRbt, SIGNAL(toggled(bool)), this, SLOT(linSelected()));
    connect(ui.logRbt, SIGNAL(toggled(bool)), this, SLOT(logSelected()));

    connect(ui.countsPlotLeftPbt, SIGNAL(clicked()), this, SLOT(panLeft()));
    connect(ui.countsPlotRightPbt, SIGNAL(clicked()), this, SLOT(panRight()));
    connect(ui.countsPlotUpPbt, SIGNAL(clicked()), this, SLOT(panUp()));
    connect(ui.countsPlotDownPbt, SIGNAL(clicked()), this, SLOT(panDown()));
    connect(ui.countsPlotZoomInXPbt, SIGNAL(clicked()), this, SLOT(zoomInX()));
    connect(ui.countsPlotZoomInYPbt, SIGNAL(clicked()), this, SLOT(zoomInY()));
    connect(ui.countsPlotZoomOutXPbt, SIGNAL(clicked()), this, SLOT(zoomOutX()));
    connect(ui.countsPlotZoomOutYPbt, SIGNAL(clicked()), this, SLOT(zoomOutY()));

    m_isYLog = initYLog;
    m_xLoLimit = xLoLimit;
    m_yLoLimit = yLoLimit;

    if ( m_isYLog )
        ui.logRbt->setChecked( true );
    else
        ui.linRbt->setChecked( true );

    ui.plot->setAutoReplot( false );
    ui.plot->setTitle( plotTitle );
    ui.plot->setCanvasBackground( QColor(Qt::white));

    ui.plot->setAutoReplot( true );
}

PlotterWidget::~PlotterWidget()
{
    deleteLists();
}

void PlotterWidget::addSetsCheckBoxes ( QList<QCheckBox *> setsCheckBoxes )
{
    for (int i=0; i < setsCheckBoxes.count(); i++ ) {
        setsCheckBoxes[i]->setChecked( true );
        ui.verticalLayout_7->addWidget( setsCheckBoxes[i] );
    }
}

void PlotterWidget::hidePlotGrpbx()
{
    ui.plotGrpbx->hide();
}

void PlotterWidget::logSelected()
{
    setYLog( true );
    ui.plot->setAxisScaleEngine( QwtPlot::yLeft, new QwtLogScaleEngine );
}

void PlotterWidget::linSelected()
{
    setYLog( false );
    ui.plot->setAxisScaleEngine( QwtPlot::yLeft, new QwtLinearScaleEngine );
}

void PlotterWidget::panLeft()
{
    double xLo = ui.plot->axisScaleDiv( QwtPlot::xBottom ).lowerBound();
    double xUp = ui.plot->axisScaleDiv( QwtPlot::xBottom ).upperBound();
    double dx = 0.1 * (xUp - xLo) ;
    if ( xLo >= m_xLoLimit )
        ui.plot->setAxisScale( QwtPlot::xBottom, xLo - dx, xUp - dx);
}

void PlotterWidget::panRight()
{
    double xLo = ui.plot->axisScaleDiv( QwtPlot::xBottom ).lowerBound();
    double xUp = ui.plot->axisScaleDiv( QwtPlot::xBottom ).upperBound();
    double dx = 0.1 * (xUp - xLo) ;
    ui.plot->setAxisScale( QwtPlot::xBottom, xLo + dx, xUp + dx);
}

void PlotterWidget::panUp()
{
    double yLo = ui.plot->axisScaleDiv( QwtPlot::yLeft ).lowerBound();
    double yUp = ui.plot->axisScaleDiv( QwtPlot::yLeft ).upperBound();
    double dy = 0.1 * (yUp - yLo) ;
    if ( isYLog() )
        ui.plot->setAxisScale( QwtPlot::yLeft, 10.0 * yLo, 10.0 * yUp );
    else
        ui.plot->setAxisScale( QwtPlot::yLeft, yLo + dy, yUp + dy);
}

void PlotterWidget::panDown()
{
    double yLo = ui.plot->axisScaleDiv( QwtPlot::yLeft ).lowerBound();
    double yUp = ui.plot->axisScaleDiv( QwtPlot::yLeft ).upperBound();
    double dy = 0.1 * (yUp - yLo) ;
    if ( isYLog() )
        ui.plot->setAxisScale( QwtPlot::yLeft, 0.1 * yLo, 0.1 * yUp );
    else
        if ( yLo >= m_yLoLimit )
            ui.plot->setAxisScale( QwtPlot::yLeft, yLo - dy, yUp - dy);
}

void PlotterWidget::zoomInX()
{
    double xLo = ui.plot->axisScaleDiv( QwtPlot::xBottom ).lowerBound();
    double xUp = ui.plot->axisScaleDiv( QwtPlot::xBottom ).upperBound();
    double dx = 0.1 * (xUp - xLo) ;
    if ( dx >= 4.0 )
        ui.plot->setAxisScale( QwtPlot::xBottom, xLo + dx, xUp - dx);
}

void PlotterWidget::zoomOutX()
{
    double xLo = ui.plot->axisScaleDiv( QwtPlot::xBottom ).lowerBound();
    double xUp = ui.plot->axisScaleDiv( QwtPlot::xBottom ).upperBound();
    double dx = 0.1 * (xUp - xLo) ;
    ui.plot->setAxisScale( QwtPlot::xBottom, xLo - dx, xUp + dx);
}

void PlotterWidget::zoomInY()
{
    double yLo = ui.plot->axisScaleDiv( QwtPlot::yLeft ).lowerBound();
    double yUp = ui.plot->axisScaleDiv( QwtPlot::yLeft ).upperBound();
    double dy = 0.1 * (yUp - yLo) ;
    ui.plot->setAxisScale( QwtPlot::yLeft, yLo, yUp - dy );
}

void PlotterWidget::zoomOutY()
{
    double yLo = ui.plot->axisScaleDiv( QwtPlot::yLeft ).lowerBound();
    double yUp = ui.plot->axisScaleDiv( QwtPlot::yLeft ).upperBound();
    double dy = 0.1 * (yUp - yLo) ;
    ui.plot->setAxisScale( QwtPlot::yLeft, yLo, yUp + dy );
}

void PlotterWidget::plotCurveSet( QList<QwtPlotCurve *> *cSet, QList<FunctionRange *> *frSet, QColor color )
{
    prepareToPlot( cSet );
    for (int m=0; m < frSet->count(); m++) {
        QwtPlotCurve *crv = new QwtPlotCurve;
        cSet->append( crv );
        plotFunctionRange( cSet->at(m), color, frSet->at(m) );
    }
    finalizePlot();
}

void PlotterWidget::unplotCurveSet( QList<QwtPlotCurve *> *cSet )
{
    prepareToPlot( cSet );
    finalizePlot();
}

void PlotterWidget::clearFunctionRangeSet( QList<FunctionRange *> *frSet )
{
    qDeleteAll( *frSet );
    frSet->clear();
}

void PlotterWidget::clearPlotRangeSet( QList<QwtPlotCurve *> *plSet )
{
    qDeleteAll( *plSet );
    plSet->clear();
}

void PlotterWidget::clearAllRangesAndCurvesSets()
{
    clearFunctionRangeSet( funcRngSet1 );
    clearFunctionRangeSet( funcRngSet2 );
    clearFunctionRangeSet( funcRngSet3 );
    clearFunctionRangeSet( funcRngSet4 );
    clearPlotRangeSet( crvSet1 );
    clearPlotRangeSet( crvSet2 );
    clearPlotRangeSet( crvSet3 );
    clearPlotRangeSet( crvSet4 );
}

void PlotterWidget::prepareToPlot( QList<QwtPlotCurve *> *cSet )
{
    doReplot = ui.plot->autoReplot();
    ui.plot->setAutoReplot( false );
    for (int m=0; m < cSet->size(); m++)
        cSet->at(m)->detach();
    clearPlotRangeSet( cSet );
}

void PlotterWidget::plotFunctionRange( QwtPlotCurve *curve, QColor color, FunctionRange *fr )
{
    int nPt = 300;
    double x[nPt];
    double y[nPt];
    double t = fr->t0;
    double tstep = ( fr->t1 - fr->t0 ) / ( nPt - 1) ;
    if  ( fr->typeParam == FunctionRange::yParms ) {
        for (int i = 0; i < nPt; i++) {
            x[i] = t;
            y[i] = fr->y( fr->parmsy, t );
            if ( isYLog() && ( y[i] < 0.001 ) ) y[i] = 0.001;
            t += tstep;
        }
    }
    else if ( fr->typeParam == FunctionRange::xyParms ) {
        for (int i = 0; i < nPt; i++) {
            x[i] = fr->x( fr->parmsx, t );
            y[i] = fr->y( fr->parmsy, t );
            if ( isYLog() && ( y[i] < 0.001 ) ) y[i] = 0.001;
            t += tstep;
        }
    }
    else if ( fr->typeParam == FunctionRange::yMatrParms ) {
        for (int i = 0; i < nPt; i++) {
            x[i] = t;
            y[i] = fr->yMatr( fr->parmsyVec, t );
            if ( isYLog() && ( y[i] < 0.001 ) ) y[i] = 0.001;
            t += tstep;
        }
    }
    else if ( fr->typeParam == FunctionRange::xyMatrParms ) {
        for (int i = 0; i < nPt; i++) {
            x[i] = fr->x( fr->parmsx, t );
            y[i] = fr->yMatr( fr->parmsyVec, t );
            if ( isYLog() && ( y[i] < 0.001 ) ) y[i] = 0.001;
            t += tstep;
        }
    }
    curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->setPen( QPen( color ) );
    curve->attach( ui.plot );
    curve->setSamples(x, y, nPt);
}

void PlotterWidget::finalizePlot()
{
    ui.plot->setAutoReplot( doReplot );
    ui.plot->replot();
}

void PlotterWidget::saveSvg( const QString& flnm )
{
    QSvgGenerator generator;
    generator.setFileName( flnm );
    generator.setSize(QSize(800, 600));
    QwtPlotRenderer renderer;
    renderer.renderTo(ui.plot, generator);
}

// AQUI 2010-05-28
// Implementar a exportacao do espectro em SVG
/*

#ifdef QT_SVG_LIB
#ifndef QT_NO_FILEDIALOG
    fileName = QFileDialog::getSaveFileName(
        this, "Export File Name", QString(),
        "SVG Documents (*.svg)");
#endif
    if ( !fileName.isEmpty() )
    {
        saveSvg();
    }
#endif
#endif
}

*/








void PlotterWidget::showCurveSet( QList<QwtPlotCurve *> *cSet )
{
    for (int m=0; m < cSet->count(); m++)
        cSet->at(m)->show();
}

void PlotterWidget::hideCurveSet( QList<QwtPlotCurve *> *cSet )
{
    for (int m=0; m < cSet->count(); m++)
        cSet->at(m)->hide();
}


void PlotterWidget::plotScatterPlot( QColor diaFill, QColor diaCont, QColor bar, QColor serif, int dSize )
{
    doReplot = ui.plot->autoReplot();
    ui.plot->setAutoReplot( false );
    errBarsQpu->showScatData( ui.plot, diaFill, diaCont, bar, serif, dSize );
    finalizePlot();
}

void PlotterWidget::deleteLists()
{
    qDeleteAll( *crvSet1 );
    qDeleteAll( *crvSet2 );
    qDeleteAll( *crvSet3 );
    qDeleteAll( *crvSet4 );
    qDeleteAll( *funcRngSet1 );
    qDeleteAll( *funcRngSet2 );
    qDeleteAll( *funcRngSet3 );
    qDeleteAll( *funcRngSet4 );
}

void PlotterWidget::setErrBarsAllEnabled( Doblst xList, Doblst yList, Doblst sList )
{
    int nPt = GSL_MIN( GSL_MIN( xList.count(), yList.count() ), sList.count() );
    double x[nPt], y[nPt], s[nPt];
    for (int i=0; i < nPt; i++) {
        x[i] = xList[i];
        y[i] = yList[i];
        if ( isYLog() && ( y[i] < 0.001 ) ) y[i] = 0.001;
        s[i] = sList[i];
    }
    errBarsQpu->setDataAllEnabled( x, y, s, nPt );
}


/*
void PlotterWidget::setErrBarsWithStatusAlreadySet(
    Doblst xList, Doblst yList, Doblst sList )
{
    int nPt = GSL_MIN( GSL_MIN( xList.count(), yList.count() ), sList.count() );
    double x[nPt], y[nPt], s[nPt];
    bool isEnabled[nPt];
    for (int i=0; i < nPt; i++) {
        x[i] = xList[i];
        y[i] = yList[i];
        if ( isYLog() && ( y[i] < 0.001 ) ) y[i] = 0.001;
        s[i] = sList[i];
        isEnabled[i] = isEnabledList[i];
    }
    errBarsQpu->setDataAndEnabling( x, y, s, isEnabled, nPt );
}
*/

void PlotterWidget::changeEnablingOfAPoint( int rw, bool enable )
{
    isEnabledList[ rw ] = enable;
}
