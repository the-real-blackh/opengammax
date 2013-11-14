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

#ifndef SPECTRUMFORM_H
#define SPECTRUMFORM_H

#include <QtGui/QDialog>
#include <QDateTime>
#include <QDomDocument>
#include <QStandardItemModel>
#include <QStringList>

#include "ui_spectrumform.h"
#include "spectrumformmodels.h"
#include "countsplotterwidget.h"
#include "energyplotterwidget.h"
#include "fwhmplotterwidget.h"
#include "efficiencyplotterwidget.h"
#include "counts.h"
#include "../gslcontainer/gslcontainer.h"
#include "constants.h"
#include "spectrio.h"
#include "genericcalibration.h"
#include "peaklist.h"
#include "multiplets.h"
#include "acquisition.h"
#include "sample.h"
// #include "zoomer.h"
#include "nuclide.h"
#include "reports.h"
#include "sample.h"

class SpectrumForm : public QDialog
{
    Q_OBJECT
public:
    SpectrumForm(){}
    SpectrumForm( const QString & fna,
        const QString & anaNucPa,
        const QString & calNucPa,
        Sample & samp,
        SpectrumIO & io,
        QwtPlotCurve::CurveStyle style = (QwtPlotCurve::Lines));
    ~SpectrumForm();
    // void closeEvent(QCloseEvent *event);
    Sample *sample;
    bool saveSpectrum(const QString & fileName);
    bool saveSpectrumSimple(const QString & fileName, Sample & samp);
    bool savePlot();
    bool saveSpectrumPlot( const QString & fileName );
    void setNuclideFileName(const QString & nucFileName );
    QString baseName() { return m_baseName; }

    void setAnalysisNuclidePath( const QString & s ) { m_anaNucPa = s; }
    QString analysisNuclidePath() const { return m_anaNucPa; }

    void setDefaultAnalysisParmsFields();

    void analyzeSpectrum();
    void setF100D3P();
    void setXTRA_F100();

    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

    void displaySampleData(Sample & sp);
    void displayNuclideTable();

    void setPeakFitsCurves();
    //
    void setcrvsPkFit();
    void rePlotSample(Sample & samp);
    void plotCounts(Sample & samp);
    //void plot2ndDiff();
    void plotMultipletsFit();
    void messageCantPerformFit();
    QString opengammaResults();
    QList<QList<qreal> > getOneOgammaSpectrumResults();
    QList<QList<qreal> > oneOgammaSpectrumResults();

    bool isFwhmCalib() { return fwhmCalibration->isCalib(); }
    bool isEnCalib() { return energyCalibration->isCalib(); }

    int nFwhmParms() { return fwhmCalibration->nParms(); }
    int nEnParms() { return energyCalibration->nParms(); }
    int nEffParms() { return efficiencyCalibration->nParms(); }
    Doblst fwhmParms() { return fwhmCalibration->parms(); }
    Doblst enParms() { return energyCalibration->parms(); }
    Doblst effParms() { return efficiencyCalibration->parms(); }

    qreal fwhmParm( int i) { return fwhmCalibration->parm(i); }
   // novos membros publicos:
    //Sample *sample;
    PeakList *pklist;
    GenericCalibration *energyCalibration;
    GenericCalibration *fwhmCalibration;
    GenericCalibration *efficiencyCalibration;
    Multiplets *multiplets;
    Reports *reps;
    FunctionsDefinitions *funcdef;

    qreal multipletFunctionAntiga (qreal t, Doblst lP, Doblst rP, Doblst c, Doblst a, Doblst k);

    bool hasPeaks() { return m_hasPeaks; }
    //
    bool doEnergyCalib( Doblst xlst, Doblst ylst, Doblst slst, int fTy );
    bool doFwhmCalib( Doblst xlst, Doblst ylst, Doblst slst, int fTy );

    QString getFwhmFitParms();
    QString getOpenGammaResults();

    void setFwhmParms( Doblst a );
    void setEnergyParms( Doblst a );
    void setDefaultEnergyCalibration();
    void searchForPeaks();

    // analysis parms:
    int binFiltGr() { return ui.leBinFiltGr->text().toInt(); }
    qreal pkcut() { return ui.lePkcut->text().toDouble(); }
    int sideChannelsSearch() { return ui.leSideChannelsSearch->text().toInt(); }
    qreal noFwhmOfPeak() { return ui.leNoFwhmOfPeak->text().toDouble(); }
    qreal noFwhmOfSideBaseline() { return ui.leNoFwhmOfSideBaseline->text().toDouble(); }
    int baselinePolyGrade() { return ui.leBaselinePolyGrade->text().toInt(); }
    qreal toler() { return ui.leToler->text().toDouble(); }
    qreal delayInDays() { return ui.leDelayInDays->text().toDouble(); }
       //
    int typeBaseline() { return ui.cbxTypeBaseline->currentIndex(); }
    int interpolationMethod() { return ui.cbxInterpolationMethod->currentIndex(); }  
    int fitStrategy() { return ui.cbxFitStrategy->currentIndex(); }
    SpectrumIO *io;

private slots:
// Button Box clicked: to be impl'd
//  void on_buttonBox_clicked( QAbstrsamactButton * button );
// energy and efficiency-fits related
    void on_neffptsLne_textChanged( QString text )
        { mdls->changeNEfficPts( ui.neffptsLne->text().toInt() ); }
    void on_doefficfitPbt_clicked();

    void on_pbtNewSearch_clicked();
    void on_doEnChFitPbt_clicked();
    void on_doFwhmChFitPbt_clicked();
    void on_pbtClearEnergyCalibration_clicked();

    // void energyPointChanged(QStandardItem *item);

    // Nuclide libraries selection:

    void on_cbxSelectedAnalysisLib_activated();

private:
    Ui::SpectrumForm ui;
    void setCurrentFile(const QString &fileName);
    QString m_baseName;
    QString strippedName(const QString &fullFileName);

    QString m_nuclideFileName;

    bool m_isValid;

    QString m_anaNucPa;
    QString getContentsRange( const QString &contents, const QString &header );

    void determineFwhmOfPeaks();

    void doEnChFit();
    void doFwhmChFit();
    void doEfficFit();
    qreal (*currentEnFitFunc) (Doblst, qreal);
    qreal (*currentFwhmFitFunc) (Doblst, qreal);
    qreal (*currentEnAndFwhmFitFunc) (QList<Doblst>, qreal);

    void setEnFitFunc( int i );
    void setFwhmFitFunc( int i );

    //
    void resolveMultiplets();
    //
    void identifyGammas();
    //
    QString curFile;
    QString canonPath;


    void setAnalysisLib( const QString &baseName );

    SpectrumFormModels *mdls;

    CountsPlotterWidget *countsPlwd;
    EnergyPlotterWidget *energyPlwd;
    FwhmPlotterWidget *fwhmPlwd;
    EfficiencyPlotterWidget *efficPlwd;

    QDomDocument *domDoc;
   //
    void populateComboBoxes( const QString &analysisNuclibPath, const QString &calibNuclibPath );
    bool loadAnalysisNuclideTable();

    Acquisition *acquis;
    QList<int> candidates;
    NuclideList *nuclideList;
    GammaList *gammaList;

    bool m_hasPeaks;
    //
    GslContainer *gslcontnr;

};

#endif /* SPECTRUMFORM_H */
