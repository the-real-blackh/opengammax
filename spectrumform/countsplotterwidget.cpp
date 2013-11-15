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


#include "countsplotterwidget.h"

// counts plotter:

CountsPlotterWidget::CountsPlotterWidget( QString &title,
                                         Doblst enParms,
                                         int style,
                                         QWidget *parent )
  : PlotterWidget( title, false, 0.0, 1.0, 0, parent )
{
    QCheckBox *ckbPlotCounts = new QCheckBox("Counts");
    QCheckBox *ckbPlotBaseLine = new QCheckBox("Base line");
    QCheckBox *ckbPlotPeaks = new QCheckBox("Peaks");
    // QCheckBox *ckbPlot2ndDiff = new QCheckBox("2nd diff.");

    setupGammasHintsMdl();

    QList<QCheckBox *> setsCheckBoxes;
    setsCheckBoxes <<
      ckbPlotCounts << ckbPlotBaseLine << ckbPlotPeaks;
    addSetsCheckBoxes( setsCheckBoxes );
    QPushButton *butt0 = new QPushButton("Analyze", this);
    //QPushButton *butt1 = new QPushButton("Restart Signal Capture", this);
    //QPushButton *butt2 = new QPushButton("Stop Signal Capture", this);
    ui.verticalLayout_4->addWidget( butt0 );
    //ui.verticalLayout_4->addWidget( butt1 );
    //ui.verticalLayout_4->addWidget( butt2 );

    d_picker = new EnergyPicker( enParms, ui.plot->canvas() );
    d_picker->setTrackerPen(QColor(Qt::darkGreen));

    connect( ckbPlotBaseLine, SIGNAL(stateChanged(int)), this, SLOT( changed1and3(int)) );
    connect( ckbPlotPeaks, SIGNAL(stateChanged(int)), this, SLOT( changed2(int)) );
    connect( ckbPlotCounts, SIGNAL(stateChanged(int)), this, SLOT( changedErrBars(int)) );
    // connect( ckbPlot2ndDiff, SIGNAL(stateChanged(int)), this, SLOT( changed4(int)) );
    connect( butt0, SIGNAL(clicked()), this, SIGNAL( askForNewSearch() ) );
    //connect( butt1, SIGNAL(clicked()), this, SIGNAL( startLiveSpectrum() ) );
    //connect( butt2, SIGNAL(clicked()), this, SIGNAL( stopLiveSpectrum() ) );

    connect(d_picker, SIGNAL(selected(const QPointF &)),
            this, SLOT(selectedPoint(const QPointF &)));
}

void CountsPlotterWidget::setPickerEnCal( Doblst a )
{
    d_picker->setPickerEnCal( a );
}

void CountsPlotterWidget::setPickerEnCalDefaultCalibration()
{
    d_picker->setPickerEnCalDefault();
}

// Energy calibration gamma's hints:

void CountsPlotterWidget::setupGammasHintsMdl()
{
    gammasHintsMdl = new QStandardItemModel( this );
    gammasHintsMdl->setColumnCount( 3 );
    QStringList horizHead;
    horizHead << tr("Energy (keV)") << tr("Centroid") << tr("Nuclide");
    gammasHintsMdl->setHorizontalHeaderLabels( horizHead );
}

void CountsPlotterWidget::fillGammasHintsMdl( QStandardItemModel *from )
{
    gammasHintsMdl->setRowCount( from->rowCount() );
    for (int i=0; i < gammasHintsMdl->rowCount(); i++) {
        qreal energy = from->item(i, 2)->text().toDouble();
        QModelIndex index = gammasHintsMdl->index(i, 0, QModelIndex() );
        gammasHintsMdl->setData( index, energy );

        qreal centroid = from->item(i, 4)->text().toDouble();
        index = gammasHintsMdl->index(i, 1, QModelIndex() );
        gammasHintsMdl->setData( index, centroid );

        QString nuclide = from->item(i, 1)->text();
        index = gammasHintsMdl->index(i, 2, QModelIndex() );
        gammasHintsMdl->setData( index, nuclide );
    }
    gammasHintsMdl->sort( 0 );
}

void CountsPlotterWidget::selectedPoint( const QPointF & )
{
    EnergyPointEntry *epe = new EnergyPointEntry( gammasHintsMdl, this );
    int indGamma;
    if ( epe->exec() )       // exec() to be modal
    {
        indGamma = epe->chosenGamma();
    }
    // 2010-04-04 PAREI AQUI - utilizar indGamma para selecinar pt calib
}

void CountsPlotterWidget::clearRangesAndPlots()
{
    clearAllRangesAndCurvesSets();
}

void CountsPlotterWidget::setCountsErrBars( Doblst x, Doblst y, Doblst s)
{
    if(x.size() == 0 || y.size() == 0) return;  // for live data which starts with no counts
    setErrBarsAllEnabled( x, y, s );
    ui.plot->setAxisScale( QwtPlot::xBottom, 0.0, x.max() );
    ui.plot->setAxisScale( QwtPlot::yLeft, 1.0, 1.1 * y.max() );
    plotScatterPlot( Qt::yellow, Qt::darkGreen, Qt::cyan, Qt::blue, 3 );
}

void CountsPlotterWidget::markCentroids( Doblst cs, Doblst grossHei )
{
    unplotCurveSet( crvSet1 );
    clearFunctionRangeSet( funcRngSet1 );
    for ( int i = 0; i < cs.count(); i++ ) {
        FunctionRange *fr1 = new FunctionRange();
        fr1->typeParam = FunctionRange::xyParms;
        Doblst parms;
        parms << cs[i];
        fr1->parmsx = parms;
        fr1->x = FunctionsDefinitions::poly;
        parms.clear();
        parms << 0.0 << 1.0;
        fr1->parmsy = parms;
        fr1->y = FunctionsDefinitions::poly;
        fr1->t0 = 1.0;          // plot from height = 1
        fr1->t1 = grossHei[i];  // up to counts in the nearest channel from centroid.
        funcRngSet1->append( fr1 );
    }
    plotCurveSet( crvSet1, funcRngSet1, Qt::red );
}

void CountsPlotterWidget::markSideBaseLinesPoly( PeakList *pks, Multiplets *mts )
{
    for ( int m = 0; m < mts->nMult(); m++ ) {
        if ( mts->isValid(m) ) {
        FunctionRange *fr2a = new FunctionRange();
        fr2a->typeParam = FunctionRange::xyParms;
        Doblst px;
        px << mts->iniLeft(m) << 1.0;
        fr2a->parmsx = px;
        fr2a->parmsy = mts->leftBL(m);
        fr2a->x = FunctionsDefinitions::poly; // mts->leftBL(m);
        fr2a->y = FunctionsDefinitions::poly; // mts->leftBL(m);
        fr2a->t0 = 0.0;
        fr2a->t1 = ( mts->nLeft(m) ) - 1; // interval width is # chans minus 1
        funcRngSet2->append( fr2a );
        FunctionRange *fr2b = new FunctionRange();
        fr2b->typeParam = FunctionRange::xyParms;
        px.clear();
        px << mts->iniRight(m) << 1.0;
        fr2b->parmsx = px;
        fr2b->parmsy = mts->rightBL(m);
        fr2b->x = FunctionsDefinitions::poly; // mts->leftBL(m);
        fr2b->y = FunctionsDefinitions::poly; // mts->leftBL(m);
        fr2b->t0 = 0.0;
        fr2b->t1 = ( mts->nRight(m) ) - 1; // interval width is # chans minus 1
        funcRngSet2->append( fr2b );
    }
    }
    plotCurveSet( crvSet2, funcRngSet2, Qt::magenta );
}

void CountsPlotterWidget::markSideBaseLinesExpSimple( PeakList *pks, Multiplets *mts )
{
    for ( int m = 0; m < mts->nMult(); m++ ) {
        if ( mts->isValid(m) ) {
        FunctionRange *fr2a = new FunctionRange();
        fr2a->typeParam = FunctionRange::xyParms;
        Doblst px;
        px << mts->iniLeft(m) << 1.0;
        fr2a->parmsx = px;
        fr2a->parmsy = mts->leftBL(m);
        fr2a->x = FunctionsDefinitions::poly; // mts->leftBL(m);
        fr2a->y = FunctionsDefinitions::expPlusConst; // mts->leftBL(m);
        fr2a->t0 = 0.0;
        fr2a->t1 = ( mts->nLeft(m) ) - 1; // interval width is # chans minus 1
        funcRngSet2->append( fr2a );
        FunctionRange *fr2b = new FunctionRange();
        fr2b->typeParam = FunctionRange::xyParms;
        px.clear();
        px << mts->iniRight(m) << 1.0;
        fr2b->parmsx = px;
        fr2b->parmsy = mts->rightBL(m);
        fr2b->x = FunctionsDefinitions::poly; // mts->leftBL(m);
        fr2b->y = FunctionsDefinitions::expPlusConst; // mts->leftBL(m);
        fr2b->t0 = 0.0;
        fr2b->t1 = ( mts->nRight(m) ) - 1; // interval width is # chans minus 1
        funcRngSet2->append( fr2b );
    }
    }
    plotCurveSet( crvSet2, funcRngSet2, Qt::magenta );
}

void CountsPlotterWidget::markSideBaseLinesExpPlusC( PeakList *pks, Multiplets *mts )
{
    for ( int m = 0; m < mts->nMult(); m++ ) {
        if ( mts->isValid(m) ) {
            FunctionRange *fr2a = new FunctionRange();
            fr2a->typeParam = FunctionRange::xyParms;
            Doblst px;
            px << mts->iniLeft(m) << 1.0;
            fr2a->parmsx = px;
            fr2a->parmsy = mts->leftBL(m);
            fr2a->x = FunctionsDefinitions::poly; // mts->leftBL(m);
            fr2a->y = FunctionsDefinitions::expPlusConst; // mts->leftBL(m);
            fr2a->t0 = 0.0;
            fr2a->t1 = ( mts->nLeft(m) ) - 1; // interval width is # chans minus 1
            funcRngSet2->append( fr2a );
            FunctionRange *fr2b = new FunctionRange();
            fr2b->typeParam = FunctionRange::xyParms;
            px.clear();
            px << mts->iniRight(m) << 1.0;
            fr2b->parmsx = px;
            fr2b->parmsy = mts->rightBL(m);
            fr2b->x = FunctionsDefinitions::poly; // mts->leftBL(m);
            fr2b->y = FunctionsDefinitions::expPlusConst; // mts->leftBL(m);
            fr2b->t0 = 0.0;
            fr2b->t1 = ( mts->nRight(m) ) - 1; // interval width is # chans minus 1
            funcRngSet2->append( fr2b );
        }
    }
    plotCurveSet( crvSet2, funcRngSet2, Qt::magenta );
}

void CountsPlotterWidget::markInnerBaseLinesCubic( Multiplets *mts )
{
    for ( int m = 0; m < mts->nMult(); m++ ) {
        if ( mts->isValid(m) ) {
        FunctionRange *fr3 = new FunctionRange();
        fr3->typeParam = FunctionRange::yParms;
        // Doblst px;
        // px << mts->iniLeft(m) << 1.0;
        // fr3->parmsx = px;
        fr3->parmsy = mts->innerBL(m);
        fr3->x = FunctionsDefinitions::poly;
        fr3->y = FunctionsDefinitions::poly;
        fr3->t0 = mts->finLeft(m);
        fr3->t1 = mts->iniRight(m);
        funcRngSet3->append( fr3 );
    }
    }
    plotCurveSet( crvSet3, funcRngSet3, Qt::darkBlue );
}

void CountsPlotterWidget::markInnerBaseLinesPeakHeiWei( Multiplets *mts )
{
    for ( int m = 0; m < mts->nMult(); m++ ) {
        if ( mts->isValid(m) ) {
        FunctionRange *fr3 = new FunctionRange();
        fr3->typeParam = FunctionRange::yParms;
        // Doblst px;
        // px << mts->iniLeft(m) << 1.0;
        // fr3->parmsx = px;
        fr3->parmsy = mts->innerBL(m);
        fr3->x = FunctionsDefinitions::poly;
        fr3->y = FunctionsDefinitions::poly;
        fr3->t0 = mts->finLeft(m);
        fr3->t1 = mts->iniRight(m);
        funcRngSet3->append( fr3 );
    }
    }
    plotCurveSet( crvSet3, funcRngSet3, Qt::darkBlue );
}

void CountsPlotterWidget::markMultiplets( Multiplets *mts )
{
    for ( int m = 0; m < mts->nMult(); m++ ) {
        if ( mts->isValid(m) ) {
            FunctionRange *fr4 = new FunctionRange();
            fr4->typeParam = FunctionRange::yMatrParms;
            // Doblst px;
            // px << mts->iniLeft(m) << 1.0;
            // fr3->parmsx = px;
            fr4->parmsyVec = mts->totalMultipletParms(m);
            fr4->x = FunctionsDefinitions::poly;
            fr4->yMatr = FunctionsDefinitions::totalMultipletFunction;
            fr4->t0 = mts->finLeft(m);
            fr4->t1 = mts->iniRight(m);
            funcRngSet4->append( fr4 );
        }
    }
    plotCurveSet( crvSet4, funcRngSet4, Qt::green );
}

void CountsPlotterWidget::changed1and3(int state )
{
    if ( state == Qt::Checked ) {
        showCurveSet( crvSet1 );
        showCurveSet( crvSet3 );
    }
    else {
        hideCurveSet( crvSet1 );
        hideCurveSet( crvSet3 );
    }
}

void CountsPlotterWidget::changed2(int state )
{
    if ( state == Qt::Checked )
        showCurveSet( crvSet2 );
    else
        hideCurveSet( crvSet2 );
}

/*
void CountsPlotterWidget::changed4(int state )
{
    if ( state == Qt::Checked )
        showCurveSet( crvSet4 );
    else
        hideCurveSet( crvSet4 );
}
*/

void CountsPlotterWidget::changedErrBars(int state )
{
    if ( state == Qt::Checked )
        errBarsQpu->show();
    else
        errBarsQpu->hide();
}

void CountsPlotterWidget::saveSvgPlot( const QString& flnm )
{
    saveSvg( flnm );
}
