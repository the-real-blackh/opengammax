/***************************************************************************
 *   Copyright (C) 2007, 2009 by Marcelo Francis Maduar                    *
 *   maduar@gmail.com                                                      *
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

#include <QApplication>  // soh para usar set- e restoreOverrideCursos
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QTime>
#include <qstring.h>
#include "spectrumformmodels.h"
#include "spectrumform.h"
#include "spectrio.h"
#include "sample.h"

SpectrumForm::SpectrumForm( const QString & fna,
                           const QString & anaNucPa,
                           const QString & calNucPa,
                           Sample & samp,
                           SpectrumIO & io,
                           QwtPlotCurve::CurveStyle style)
{
    ui.setupUi(this);
    sample = &samp;
    Counts *cts = samp.counts;
    cts->setParent( this );
    //
    m_isValid = false;

    // get data from io

    setEnFitFunc( samp.enFitType() );
    setFwhmFitFunc( samp.fwhmFitType() );
    energyCalibration = new GenericCalibration( samp.enParms(), currentEnFitFunc, this );
    energyCalibration->setParms( samp.enParms() );

    fwhmCalibration = new GenericCalibration( samp.fwhmParms(), currentFwhmFitFunc, this );
    fwhmCalibration->setParms( samp.fwhmParms() );

    efficiencyCalibration = new GenericCalibration( samp.efficParms(), FunctionsDefinitions::polyLog, this );
    QStringList zzz = samp.ioEfficEnPts;
    acquis = new Acquisition( samp.liveTime, samp.realTime, samp.acqDateTime, this );

    // set members
    pklist = new PeakList;
    multiplets = new Multiplets( pklist, samp.counts, fwhmCalibration, this );
    reps = new Reports( this );
    funcdef = new FunctionsDefinitions( this );

    QString title = tr("Counts");
    countsPlwd = new CountsPlotterWidget( title, enParms(), style, ui.tab_2);
    ui.horizontalLayout->addWidget( countsPlwd );

    title = tr("Channel / Energy");
    energyPlwd = new EnergyPlotterWidget( title, ui.tab_5 );
    ui.verticalLayout_3->insertWidget( 0, energyPlwd );

    title = tr("Energy / FWHM");
    fwhmPlwd = new FwhmPlotterWidget( title, ui.tab_5 );
    ui.verticalLayout_4->insertWidget( 0, fwhmPlwd );

    title = tr("Efficiency");
    efficPlwd = new EfficiencyPlotterWidget( title, ui.tab_6 );
    ui.horizontalLayout_5->insertWidget( 0, efficPlwd );

    setCurrentFile( fna );
    setAttribute( Qt::WA_DeleteOnClose );

    setAnalysisNuclidePath( anaNucPa );
    //
    // Set validators
    //
    ui.leValue_2->setValidator( new QDoubleValidator( 0.01, 9999.99, 2, ui.leValue_2 ));
    ui.leUncertainty_2->setValidator( new QDoubleValidator( 0.01, 9999.99, 2, ui.leUncertainty_2 ));
    ui.leDelayInDays->setValidator( new QDoubleValidator( 0.01, 9999.99, 3, ui.leDelayInDays ));
    //
    ui.leBinFiltGr->setValidator( new QIntValidator(0, 5, ui.leBinFiltGr ));
    ui.lePkcut->setValidator(new QDoubleValidator(0.01, 20.00, 2, ui.lePkcut ));
    ui.leSideChannelsSearch->setValidator( new QIntValidator(2, 10, ui.leSideChannelsSearch ));
    ui.leNoFwhmOfPeak->setValidator(new QDoubleValidator( 1.0, 6.0, 2, ui.leNoFwhmOfPeak ));
    ui.leNoFwhmOfSideBaseline->setValidator(new QDoubleValidator( 1.0, 15.00, 2, ui.leNoFwhmOfSideBaseline ));
    ui.leBaselinePolyGrade->setValidator( new QIntValidator( 0, 3, ui.leBaselinePolyGrade ));
    ui.leToler->setValidator( new QDoubleValidator( 0.1, 10.0, 1, ui.leToler ));
    ui.leDelayInDays->setValidator( new QDoubleValidator( 0.001, 1000.0, 2, ui.leDelayInDays ));
    //
    ui.neffptsLne->setValidator(new QIntValidator( 2, 30, ui.neffptsLne ));
    ui.polygradeLne->setValidator(new QIntValidator( 1, 10, ui.polygradeLne ));

    // setup models and views

    mdls = new SpectrumFormModels( this );

    mdls->setupRawDataMdl();
    ui.rawDataTbv->setModel( mdls->rawDataMdl );

    mdls->setupEnergypointsMdl();
    ui.energypointsTbv->setModel( mdls->energypointsMdl );

    mdls->setupEnxchanMdl();
    ui.enxchanTbv->setModel( mdls->enxchanMdl );

    mdls->setupFwhmxchMdl();
    ui.fwhmxenTbv->setModel( mdls->fwhmxchMdl );

    mdls->setupPeaksFitMdl();
    ui.fittedPeaksTbv->setModel( mdls->peaksFitMdl );

    mdls->setupActivitiesMdl();
    ui.activitiesTbv->setModel( mdls->activitiesMdl );

    // ui.fittedPeaksTbv->hideColumn( 21 );   // hide idNucl


    //mdls->setupPartialActivitiesMdl();
    //ui.partialActivitiesTbv->setModel( mdls->partialActivitiesMdl );


    nuclideList = new NuclideList;
    gammaList = new GammaList;
    mdls->setupNuclideTabMdl();
    mdls->setupGammaTabMdl();
    mdls->setupGammasPerNuclideMdl();

    ui.gammasPerNuclideTbv->setModel( mdls->gammasPerNuclideMdl );

    // ui.energypointsTbv->hideColumn( 2 );   // hide fwhm and sfwhm as fction of Ch
    // ui.energypointsTbv->hideColumn( 3 );

    mdls->setupEfficPtsMdl();
    ui.efficPtsTbv->setModel( mdls->efficPtsMdl );

    mdls->setupeffxenMdl();
    ui.effxenTbv->setModel( mdls->effxenMdl );

    on_neffptsLne_textChanged( ui.neffptsLne->text() );

    displaySampleData(samp);
    Intlst *ilist = cts->getAllCounts();
    mdls->fillRawDataMdl( *ilist );

    plotCounts(samp);
    // plot2ndDiff();

    if ( nEnParms() == 2 )
        ui.cbxEnChFitType->setCurrentIndex( 1 );
    if ( nEnParms() == 3 )
        ui.cbxEnChFitType->setCurrentIndex( 0 );

    energyPlwd->setNCh( cts->nchs() );
    energyPlwd->plotEnergyFit( enParms(), currentEnFitFunc );

    if ( currentFwhmFitFunc == FunctionsDefinitions::sqrtT )
        ui.cbxFwhmChFitType->setCurrentIndex( 0 );
    if ( currentFwhmFitFunc == FunctionsDefinitions::poly )
        ui.cbxFwhmChFitType->setCurrentIndex( 1 );

    fwhmPlwd->setNCh( cts->nchs() );
    fwhmPlwd->plotFwhmFit( enParms(), fwhmParms(), currentFwhmFitFunc, currentEnAndFwhmFitFunc );

    ui.leNoCh_2->setText( QString("%1").arg( cts->nchs() ));

    setWindowTitle( tr("Spectrum: ") + fna );
    populateComboBoxes( anaNucPa, calNucPa );

    setDefaultAnalysisParmsFields();

    mdls->fillEnxchanMdl( enParms() );
    mdls->fillFwhmxchMdl( fwhmParms() );

    countsPlwd->setPickerEnCal( enParms() );
    mdls->fillEfficPtsMdl( zzz );


    gslcontnr = new GslContainer;

    // QString calibLibFilename = calNucPa + "/" + ui.cbxSelectedAnalysisLib->currentText(); // to be implem'd
    setAnalysisLib( "BasicNuclideLibrary.lib" );
    // 2010-05-05 AQUI desativei porque nao func com espectro ogamm
    // 2010-05-05 HERE disabled because no func-spectrum ogamm
    // on_pbtNewSearch_clicked();

    connect( countsPlwd, SIGNAL(askForNewSearch()),
      this, SLOT(on_pbtNewSearch_clicked()));

    // hide some buttons

    ui.pbtAcceptNewEnergies->hide();
    ui.doEnChFitPbt->hide();

}


SpectrumForm::~SpectrumForm()
{
    qDeleteAll( *pklist );
    pklist->clear();
}

void SpectrumForm::on_cbxSelectedAnalysisLib_activated()
{
    setAnalysisLib( ui.cbxSelectedAnalysisLib->currentText() );
}

void SpectrumForm::setAnalysisLib( const QString &baseName )
{
    QString contents;
    QString analysisLibFilename = analysisNuclidePath() + "/" + baseName;
    if ( io->readNuclideFile( analysisLibFilename, contents ) ) {
        QString nuclideContents = getContentsRange( contents, "Nuclides" );
        QString gammaContents = getContentsRange( contents, "Gammas" );
        nuclideList->fillNuclideList( nuclideContents );
        gammaList->fillGammaList( gammaContents );
        mdls->fillStdItemModel( mdls->nuclideTabMdl, nuclideContents );
        mdls->fillStdItemModel( mdls->gammaTabMdl, gammaContents );
        mdls->fillGammasPerNuclideMdl();
        countsPlwd->fillGammasHintsMdl( mdls->gammasPerNuclideMdl );
    }
}

bool SpectrumForm::saveSpectrumSimple(const QString &fileName, Sample & samp){
    SpectrumIO *sp = new SpectrumIO();
    if(sp->saveSpectrumSimple( fileName, samp)){
        return true;
    } else {
        return false;
    }
}

/*
bool SpectrumForm::saveSpectrum(const QString &fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool saved = false;
    QStringList efficEnPtsStrlst;
    for (int i=0; i < mdls->efficPtsMdl->rowCount(); i++) {
        efficEnPtsStrlst << mdls->efficPtsMdl->item( i, 0)->text();
        efficEnPtsStrlst << mdls->efficPtsMdl->item( i, 1)->text();
        efficEnPtsStrlst << mdls->efficPtsMdl->item( i, 2)->text();
    }
    QStringList enxchanStrlst;
    for (int i=0; i < mdls->enxchanMdl->rowCount(); i++) {
        enxchanStrlst << mdls->enxchanMdl->item( i, 0)->text();
    }
    QStringList fwhmxchStrlst;
    for (int i=0; i < mdls->fwhmxchMdl->rowCount(); i++) {
        fwhmxchStrlst << mdls->fwhmxchMdl->item( i, 0)->text();
    }
    if ( io->saveSpectrum( fileName, cts->getAllCounts(), ui.sampDescrTed->toPlainText(),
                                    ui.leValue_2->text(), ui.leUncertainty_2->text(),
                                    ui.cbUnit_2->currentText(),
                                    ui.startDte->text(), ui.leLT_2->text(), ui.leRT_2->text(),
                                    efficEnPtsStrlst, enxchanStrlst, fwhmxchStrlst ) ) {
        saved = true;
    }
    else {
        QMessageBox::warning( this, tr("Not saved"), tr("File not saved: ") + fileName );
    }
    QApplication::restoreOverrideCursor();
    return saved;
}

*/

bool SpectrumForm::savePlot()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString fileName = canonPath + "/"  + m_baseName + ".svg";
    bool saved = false;
    if ( saveSpectrumPlot( fileName ) ) {
        saved = true;
        QApplication::restoreOverrideCursor();
    }
    else {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning( this, tr("Not saved"), tr("File not saved: ") + fileName );
    }
    return saved;
}

bool SpectrumForm::saveSpectrumPlot( const QString & fileName )
{
    bool ret = false;
    if ( !fileName.isEmpty() ) {
        QFile file(fileName);
        if ( !file.open(QIODevice::WriteOnly) )
            ret = false;
        else {
            countsPlwd->saveSvgPlot( fileName );
            ret = true;
        }
    }
    return ret;
}

void SpectrumForm::setNuclideFileName( const QString & s )
{
    m_nuclideFileName = s;
}


QString SpectrumForm::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void SpectrumForm::setCurrentFile(const QString & fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    canonPath = QFileInfo(fileName).canonicalPath();
    m_baseName = QFileInfo(fileName).baseName();

    // setWindowModified(false);
    // setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString SpectrumForm::strippedName(const QString & fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void SpectrumForm::on_pbtClearEnergyCalibration_clicked()
{
    energyCalibration->clearParms();
    mdls->fillEnxchanMdl( enParms() );
    countsPlwd->setPickerEnCal( enParms() );
}

void SpectrumForm::on_doEnChFitPbt_clicked()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    bool ret = false;
    if ( !(isEnCalib()) ) {
        determineFwhmOfPeaks();
        if ( hasPeaks() ) {
            mdls->fillEnergypointsMdl (
                pklist, energyCalibration, fwhmCalibration, gammaList, nuclideList, toler() );
            ui.energypointsTbv->resizeRowsToContents();
            ret = true;
        }
    }
    else
        ret = true;
    if ( ret ) {
        Doblst xlst, ylst, slst;
        for (int i=0; i < pklist->npks(); i++) {
            if ( mdls->energypointsMdl->item(i, 1)->checkState() == Qt::Checked ) {
         // model's field fittedEnergy
                xlst << pklist->centroid(i);
                ylst << mdls->energypointsMdl->item(i, 1)->text().toDouble();
                // slst << data->sLibEnergy(i);
            }
        }
        if ( doEnergyCalib
             ( xlst, ylst, slst, ui.cbxEnChFitType->currentIndex() ) ) {
//            ui.teFitLog->appendPlainText( data->reportPeakWidths() );
            mdls->fillEnxchanMdl( enParms() );
            // fillFwhmxenMdl(); // ?????
            // 2010-03-14 REIMOLKEMENTAR!!!!!!
            countsPlwd->setPickerEnCal( enParms() );
            // data.setEnergycal..
            // picker setenergycal
            // energyPlwd->setFwhmChFit( data->fwhmParms(), 0.0, xlst.max(), 0.0, ylst.max() );
            ret = true;
        }
        else
            ret = false;
    }
    QApplication::restoreOverrideCursor();
    if ( !ret )
        messageCantPerformFit();
}

void SpectrumForm::on_doFwhmChFitPbt_clicked()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    bool ret = false;
    if (!isEnCalib())
        setDefaultEnergyCalibration();
    if ( !isFwhmCalib() )
        searchForPeaks();
    ret = hasPeaks();
    if ( ret ) {
        Doblst xlst, ylst, slst;
        for (int i=0; i < pklist->count(); i++) {
            if ( mdls->energypointsMdl->item(i, 1)->checkState() == Qt::Checked ) {
         // model's field fittedEnergy
                xlst << pklist->centroid(i);
                ylst << pklist->fwhmCh(i);
                slst << pklist->sfwhmCh(i);
            }
        }

        setFwhmFitFunc( ui.cbxFwhmChFitType->currentIndex() );

        if ( doFwhmCalib( xlst, ylst, slst, ui.cbxFwhmChFitType->currentIndex()) ) {
            mdls->fillFwhmxchMdl( fwhmCalibration->parms() );
            fwhmPlwd->plotFwhmFit( energyCalibration->parms(), fwhmCalibration->parms(),
              currentFwhmFitFunc, currentEnAndFwhmFitFunc );
            ui.pteWidthFitLog->appendPlainText( tr("======== AFTER FIT: ===========") + "\n");
            ui.pteWidthFitLog->appendPlainText( reps->
              reportPeakWidthsAndFit( pklist, energyCalibration, fwhmCalibration,
                                      ui.cbxFwhmChFitType->currentIndex()) );
            ret = true;
        }
        else
            ret = false;
    }
    QApplication::restoreOverrideCursor();
    if ( !ret )
        messageCantPerformFit();
}

void SpectrumForm::on_doefficfitPbt_clicked()

{
    doEfficFit();
}

void SpectrumForm::on_pbtNewSearch_clicked()
{
    analyzeSpectrum();
   /*
    QApplication::setOverrideCursor( Qt::WaitCursor );
    bool ret = false;
    if (!isEnCalib())
        setDefaultEnergyCalibration();
    searchForPeaks();
    ret = ( pklist->npks() > 1);
    QApplication::restoreOverrideCursor();
    if ( !ret )
        messageCantPerformFit();
        */
}

void SpectrumForm::setDefaultEnergyCalibration()
{
    energyCalibration->setDefaultCalibration();
    countsPlwd->setPickerEnCalDefaultCalibration();
    mdls->fillEnxchanMdl( enParms() );
}

void SpectrumForm::searchForPeaks()
{
    qDeleteAll( *pklist );
    pklist->clear();
    countsPlwd->clearRangesAndPlots();
    multiplets->clearAll();
    Counts *cts = sample->counts;
    determineFwhmOfPeaks();
    if ( hasPeaks() ) {
        mdls->fillEnergypointsMdl (
                    pklist, energyCalibration, fwhmCalibration, gammaList, nuclideList, toler() );
        ui.energypointsTbv->resizeRowsToContents();

        Doblst grossHeights;
        for ( int i=0; i < pklist->count(); i++ )
            grossHeights << cts->cntList.at( qRound( pklist->centroid(i) ) );
        countsPlwd->markCentroids( pklist->centroids(), grossHeights );

        energyPlwd->setMultidimPoints(
                    pklist->centroids(),
                    pklist->fittedEnergies( energyCalibration ),
                    pklist->sFittedEnergies( energyCalibration ),
                    pklist->fwhmsInEnergy( energyCalibration ),
                    pklist->sFwhmsInEnergy( energyCalibration )
                    );

        energyPlwd->plotEnergyPoints();

        fwhmPlwd->setMultidimPoints(
                    pklist->centroids(),
                    pklist->fittedEnergies( energyCalibration ),
                    pklist->sFittedEnergies( energyCalibration ),
                    pklist->fwhmsInEnergy( energyCalibration ),
                    pklist->sFwhmsInEnergy( energyCalibration )
                    );

        fwhmPlwd->plotFwhmPoints();

        /*
        connect( mdls->energypointsMdl, SIGNAL(itemChanged(QStandardItem*)),
          this, SLOT(energyPointChanged(QStandardItem*)));

        */

    }
}

void SpectrumForm::setF100D3P()
{
    ui.neffptsLne->setText("16");
    ui.polygradeLne->setText("4");
    QStringList zzz;
    zzz
            << "59.54" << "1.294" << "2.40"
            << "81.00" << "2.566" << "2.77"
            << "88.03" << "3.247" << "2.80"
           << "122.06" << "4.071" << "2.16"
           << "136.47" << "3.981" << "3.10"
           << "165.86" << "3.853" << "2.15"
           << "276.40" << "2.441" << "2.90"
           << "302.85" << "2.211" << "2.67"
           << "356.02" << "1.890" << "2.59"
           << "383.85" << "1.872" << "3.10"
           << "604.69" << "1.134" << "2.20"
           << "795.84" << "0.882" << "2.27"
           << "898.04" << "0.818" << "2.57"
          << "1173.24" << "0.641" << "2.18"
          << "1332.50" << "0.564" << "2.13"
          << "1836.06" << "0.423" << "2.60";
    mdls->fillEfficPtsMdl( zzz );
    doEfficFit();
}

void SpectrumForm::setXTRA_F100()
{
    ui.neffptsLne->setText("16");
    ui.polygradeLne->setText("5");
    QStringList zzz;
    zzz
            << "30.97" << "5.6695" <<  "3.90"
            << "34.92" << "6.1099" <<  "6.81"
            << "81.00" << "6.4203" <<  "4.12"
           << "302.85" << "2.4402" <<  "9.63"
           << "356.02" << "2.1317" <<  "4.01"
           << "383.85" << "2.0689" << "13.24"
           << "563.23" << "1.4999" <<  "6.02"
           << "569.32" << "1.4975" <<  "4.43"
           << "604.69" << "1.4089" <<  "1.56"
           << "661.66" << "1.3218" <<  "9.33"
           << "795.84" << "1.1080" <<  "1.81"
           << "801.93" << "1.1115" <<  "6.93"
          << "1115.55" << "0.8598" << "10.89"
          << "1173.24" << "0.8235" << "13.32"
          << "1332.50" << "0.7364" << "13.37"
          << "1365.16" << "0.7343" << "16.03";
    mdls->fillEfficPtsMdl( zzz );
    doEfficFit();
}

void SpectrumForm::analyzeSpectrum() // called by slot on_Analyze....
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    QString allRep;

    searchForPeaks();
    // if ( !isEnCalib() || !isFwhmCalib() )
        on_doFwhmChFitPbt_clicked();
    if ( hasPeaks() ) {
        resolveMultiplets();
        mdls->fillPeaksFitMdl( pklist, energyCalibration, fwhmCalibration,
                               efficiencyCalibration, gammaList, nuclideList, toler(),
                               acquis->lt(),
                               ui.leDelayInDays->text().toDouble()
                               );
        // 2010-06-23 tirei, acertar depois e repor!!
        // 06/23/2010 I took, and then hit reset!
        /*
        mdls->fillActivitiesMdl( pklist, energyCalibration, fwhmCalibration, ui.leValue_2->text().toDouble(),
            ui.cbUnit_2->currentText() );
        */
    }
    QApplication::restoreOverrideCursor();
}

void SpectrumForm::messageCantPerformFit()
{
    ui.pteWidthFitLog->appendPlainText( tr("\n Can't perform fit!") );
    QMessageBox::warning( this, tr("Didn't fit!"),
        tr("This spectrum couldn't be fitted!<b>"));
}

void SpectrumForm::displaySampleData( Sample & samp)
{
    QString str;
    ui.sampDescrTed->setText( samp.remarks);
    ui.startDte->setDateTime(samp.acqDateTime);
    ui.leLT_2->setText( str.setNum(samp.liveTime, 'f', 1));
    ui.leRT_2->setText( str.setNum(samp.realTime, 'f', 1));
    // 2010-03-24 TALVEZ AQUI NAO SEJA o LUGAR !!!
    ui.leNoCh_2->setText(str.setNum(samp.numChans));
    ui.leValue_2->setText(str.setNum(samp.sampleSize) );
    ui.leUncertainty_2->setText( str.setNum(samp.sampleUncert, 'f', 1) );
    //--> fix this:  ui.cbUnit_2 = samp.getSampleUnits();
    ui.leNoCh_2->setText( str.setNum(samp.sampleSize) );
}

/*
QString SpectrumForm::opengammaResults()
{
    return data->getOpenGammaResults();
    QString rr = "OpenGamma results:";
    return rr;
}
*/

QList<QList<qreal> > SpectrumForm::getOneOgammaSpectrumResults()
{
    return oneOgammaSpectrumResults();
}

/* 2010-01-25 TO BE IMPLEMENTED
void SpectrumForm::on_buttonBox_clicked( QAbstractButton * button )
{

    switch ( ui.buttonBox->buttonRole( button ) ) {
        case QDialogButtonBox::RejectRole: Auto-destroy the child when its close button is clicked

        case QDialogButtonBox::ResetRole:
        //        setFormFromMembers();
                break;
        case QDialogButtonBox::ApplyRole:
        //        setMembersFromForm();
                break;
        default:
                break;
    }
}
*/

void SpectrumForm::populateComboBoxes( const QString &analysisNuclibPath,
                                       const QString &calibNuclibPath )
{
    QStringList files;
    QString fileWildcard = "*.lib";

    QDir dir1( analysisNuclibPath );
    files = dir1.entryList(QStringList(fileWildcard),
                                  QDir::Files | QDir::NoSymLinks);
    ui.cbxSelectedAnalysisLib->insertItems( 0, files );

    QDir dir2( calibNuclibPath );
    files = dir2.entryList(QStringList(fileWildcard),
                                  QDir::Files | QDir::NoSymLinks);
    ui.cbxSelectedCalibLib->insertItems( 0, files );
}

void SpectrumForm::plotCounts(Sample & samp)
{
    Doblst xLst, yLst, sLst;
    Counts *cts = samp.counts;
    int N = cts->nchs();
    for (int i = 0; i < N; ++i)
    {
        qreal c = qreal( cts->cntList.at(i) );
        if ( c > 0.001 ) {
            xLst << qreal(i);
            yLst << c;
            if ( c > 1.001)
                sLst << sqrt(c);
            else
                sLst << 0.999;
        }
    }
    countsPlwd->setCountsErrBars( xLst, yLst, sLst );
}

void SpectrumForm::rePlotSample(Sample & sam)
{
    char buf[40];
    Counts *cts = sam.counts;
    Intlst *ip = cts->getAllCounts();

    multiplets = new Multiplets( pklist, cts, fwhmCalibration, this );  // memory leak

    mdls->fillRawDataMdl( *ip );
    plotCounts(sam);
    energyPlwd->setNCh( cts->nchs() );
    energyPlwd->plotEnergyFit( enParms(), currentEnFitFunc );
    fwhmPlwd->setNCh( cts->nchs() );
    fwhmPlwd->plotFwhmFit( enParms(), fwhmParms(), currentFwhmFitFunc, currentEnAndFwhmFitFunc );

    QString str;
    str.setNum((sam.liveTime/1000), 'f', 1);    // display in seconds
    ui.leLT_2->setText(str);
    str.setNum(sam.realTime, 'f', 1);
    ui.leRT_2->setText(str);
    str.setNum(sam.numChans);
    ui.leNoCh_2->setText(str);
}

/*
void SpectrumForm::plot2ndDiff()
{
    SpectrumForm form;
    Counts *cts = sample->counts;
    Doblst xLst, yLst;
    int N = cts->nchs();
    for (int i = 0; i < N; ++i)
    {
        qreal c = qreal( cts->sdiff(i) );
        xLst << qreal(i);
        yLst << c;
    }
    // countsPlwd->setCountsErrBars( xLst, yLst, sLst );
}
*/

QString SpectrumForm::getContentsRange( const QString & contents, const QString & header )
{
    QString range;
    QStringList list = contents.split("\n", QString::SkipEmptyParts );
    int first = list.indexOf( header) + 1;
    int last = list.indexOf( "End " + header ) - 1;
    for (int i = first; i <= last; i++)
        range += list.at(i) + "\n";
    return range;
}

void SpectrumForm::doEfficFit()
{
    bool canFit = false;
    int n = ui.neffptsLne->text().toInt();
    int p = ui.polygradeLne->text().toInt() + 1;
    if (p <= n)
        canFit = true;
    for (int row = 0; row < mdls->efficPtsMdl->rowCount(); row++) {
        for (int col = 0; col < mdls->efficPtsMdl->columnCount(); col++) {
            if ( mdls->efficPtsMdl->item(row, col) == 0 ) {
                canFit = false;
                break;
            }
        }
        if ( !canFit )
            break;
    }
    if (canFit) {
        Doblst xlst, ylst, slst;
        qreal xmin = 0.0;
        qreal xmax = 0.0;
        for (int row = 0; row < mdls->efficPtsMdl->rowCount(); row++) {
            xlst << mdls->efficPtsMdl->item(row, 0)->text().toDouble();
            if (xlst[row] < xmin) xmin = xlst[row];
            if (xlst[row] > xmax) xmax = xlst[row];
            ylst << mdls->efficPtsMdl->item(row, 1)->text().toDouble();
            slst << mdls->efficPtsMdl->item(row, 2)->text().toDouble();
        }
        Doblst xLogLst, yLogLst, sForFit, sForPlot;
        for (int i=0; i < xlst.count(); i++) {
            xLogLst << log(xlst[i]);
            yLogLst << log(ylst[i]);
            sForFit << 0.01 * slst[i];
            sForPlot << ylst[i] * sForFit[i];
        }
        efficiencyCalibration->doCalibPolyN( p, xLogLst, yLogLst, sForFit );
        mdls->filleffxenMdl( effParms() );
        efficPlwd->plotBarsAndCurve( xlst, ylst, sForPlot, effParms() );
    }
    else {
        mdls->effxenMdl->setRowCount( 1 );
        QModelIndex index;
        index = mdls->effxenMdl->index(0, 0, QModelIndex() );
        mdls->effxenMdl->setData( index, QVariant( tr("No fit.") ) );
    }
}

void SpectrumForm::determineFwhmOfPeaks()
{
    Doblst secDiff;
    Doblst sigmaSecDiff;
    QString outRep;
    Counts *cts;
    cts = sample->counts;
    cts->setSecDiffAndSigma( binFiltGr() );
    cts->findCandidates( pkcut() );
    cts->setSecDiffAndSigma( 0 );
    m_hasPeaks = pklist->calcPeakWidths( sideChannelsSearch(), cts, gslcontnr, outRep );
    ui.pteWidthFitLog->appendPlainText( outRep );
    ui.pteWidthFitLog->appendPlainText( reps->reportCandidates( cts ) );
    ui.pteWidthFitLog->appendPlainText( reps->reportPeakWidths( pklist, energyCalibration ) );
}

bool SpectrumForm::doEnergyCalib( Doblst xlst, Doblst ylst, Doblst slst, int fTy )
{
    /*  2010-03-15 IMPLEMENAR!
        if ( fwhmChFitType() == 0 )
            return fwhmCalibration->doCalibSqrtEn( xlst, ylst, slst );
        else
            return fwhmCalibration->doCalibPolyN( 3, xlst, ylst, slst );
    // return energyCalibration->doCalibPolyN( 3, xlst, ylst, slst );
            */
    return false;
}

bool SpectrumForm::doFwhmCalib( Doblst xlst, Doblst ylst, Doblst slst, int fTy  )
{
    if ( fTy == 0 )
        return fwhmCalibration->doCalibSqrtEn( xlst, ylst, slst );
    else
        return fwhmCalibration->doCalibPolyN( 2, xlst, ylst, slst );
}


void SpectrumForm::setEnFitFunc( int i )
{
    switch ( i ) {
    case 0:
        currentEnFitFunc = FunctionsDefinitions::poly; // ( gr. 2)
        break;
    case 1:
        currentEnFitFunc = FunctionsDefinitions::poly; // ( gr. 1)
        break;
    }
}

void SpectrumForm::setFwhmFitFunc( int i )
{
    switch ( i ) {
    case 0:
        currentFwhmFitFunc = FunctionsDefinitions::sqrtT;
        currentEnAndFwhmFitFunc = FunctionsDefinitions::fwhmInEnergySqrtT;
        break;
    case 1:
        currentFwhmFitFunc = FunctionsDefinitions::poly; // (gr. 2)
        currentEnAndFwhmFitFunc = FunctionsDefinitions::fwhmInEnergyPoly;
        break;
    }
}

QList<QList<qreal> > SpectrumForm::oneOgammaSpectrumResults()
{
    QList<QList<qreal> > lst;
    for (int i=0; i < pklist->npks(); ++i) {
        QList<qreal> lstElem;
        lstElem
          << pklist->fittedEnergy( energyCalibration, i )

          << pklist->netArea( fwhmCalibration, i )
          << pklist->sNetArea( fwhmCalibration, i);
        lst << lstElem;
    }
    return lst;
}

void SpectrumForm::resolveMultiplets()
{
    multiplets->clearAll();
    multiplets->resolve( noFwhmOfPeak(), noFwhmOfSideBaseline(), typeBaseline(),
      baselinePolyGrade(), interpolationMethod(),
      fitStrategy() );

    ui.pteBaseLinesFitLog->appendPlainText( reps->reportRegions( multiplets ) );
    ui.pteBaseLinesFitLog->appendPlainText( reps->reportBaseLines( multiplets ) );
    // ui.pteBaseLinesFitLog->appendPlainText( reps->reportBaseLinesFit( multiplets ) );

    if ( typeBaseline() == 0 ) // exp (simple)
        countsPlwd->markSideBaseLinesExpSimple( pklist, multiplets );
    if ( typeBaseline() == 1 ) // poly
        countsPlwd->markSideBaseLinesPoly( pklist, multiplets );
    if ( typeBaseline() == 2 ) // exp + cte
        countsPlwd->markSideBaseLinesExpPlusC( pklist, multiplets );
    // 2010-04-21 soh habilitado para poly
    if ( typeBaseline() == 1 ) {
        if ( interpolationMethod() == 0 ) // peak-height weight
            countsPlwd->markInnerBaseLinesPeakHeiWei( multiplets );
        if ( interpolationMethod() == 1 ) // cubic spline
            countsPlwd->markInnerBaseLinesCubic( multiplets );
    }
    countsPlwd->markMultiplets( multiplets );
}

void SpectrumForm::setDefaultAnalysisParmsFields()
{
    ui.leBinFiltGr->setText( QString("%1").arg( 2 ) );
    ui.lePkcut->setText( QString("%1").arg( 0.7 ) );
    ui.leSideChannelsSearch->setText( QString("%1").arg( 6 ) );
    ui.leNoFwhmOfPeak->setText( QString("%1").arg( 5.0 ) );
    ui.leNoFwhmOfSideBaseline->setText( QString("%1").arg( 3.0 ) );
    ui.leBaselinePolyGrade->setText( QString("%1").arg( 2 ) );
    ui.leToler->setText( QString("%1").arg( 0.5 ) );
    ui.leDelayInDays->setText( QString("%1").arg( 1.0 ) );
    //
    ui.cbxTypeBaseline->setCurrentIndex( 1 );
    ui.cbxInterpolationMethod->setCurrentIndex( 1 );
    ui.cbxFitStrategy->setCurrentIndex( 0 );
    //
    ui.leValue_2->setText( QString("%1").arg( 1.0 ) );
    ui.leUncertainty_2->setText( QString("%1").arg( 1.0 ) );
}
