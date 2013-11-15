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
#define VERSION "0.1.1"


#include <QFileDialog>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include "maininterface.h"
#include "languages.h"
#include "directories.h"
#include "counts.h"
#include "style.h"
#include "sample.h"
#include "signalSettings.h"


static QTimer *timerDisplay;

static Intlst ilist;
static Counts cnts(&ilist);
static Sample sam(&cnts);
static SpectrumIO io;
static SpectrumForm *child;

MainInterface::MainInterface(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainInterfaceClass)
{
    ui->setupUi(this);
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    //updateWindowMenu();
    //connect( ui->menuWindow, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)),
            this, SLOT(updateMenus()));
    // setup timer for display updates

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)), this, SLOT(setActiveSubWindow(QWidget *)));
    updateMenus();
    readSettings();
    setWindowTitle(tr("OpenGammaX"));
    on_actionAbout_triggered();
}

MainInterface::~MainInterface()
{
    //delete ui;
}

QString MainInterface::getCurrentLang()
{
    return m_lang;
}


void MainInterface::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (activeMdiChild()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}


SpectrumForm *MainInterface::createMdiChild( const QString & fName, Sample & samp, SpectrumIO & io )
{
    child = new SpectrumForm( fName, analysisNuclibPath, calibNuclibPath, sam, io );
    mdiArea->addSubWindow(child);
    return child;
}


// This method calls other methods that do open, read, and close
void MainInterface::on_actionReadSpectrumFile_triggered()

{
    QString fileName = QFileDialog::getOpenFileName( this,
        tr("Open a spectrum"), spectraPath,
        tr("NIST n42 (*.n42);;Ortec CHN (*.chn);;OpenGamma OGAMMSP (*.ogammsp);;IEC 1455 (*.iec);;ASCII (*.asc);;Genie CAM (*.cnf)"));

    if(!(io.readSpectrumFile(fileName, sam))) {
        statusBar()->showMessage(tr("Unable to read file"), 0);
        return;
    }
    sam.fileName = fileName;
    child = createMdiChild( fileName, sam, io );
    QApplication::setOverrideCursor( Qt::WaitCursor );  
    statusBar()->showMessage(tr("File loaded"), 5000);
    child->showMaximized();
    QApplication::restoreOverrideCursor();
}


//This method calls lower level that open, and start periodic data capturing.
// It includes open and read functions, there is no separate "open" button


void MainInterface::on_actionOpenLiveSignal_triggered(){
    bool rtn;
    QString path("/dev/snd/");
    QString fileName = QFileDialog::getOpenFileName( this,
         tr("Open a live data stream."), path, "(hwC*D*)");
#ifdef HAVE_ALSA
    rtn = io.openLiveSignal( fileName, sam);
    if(rtn = false){
        statusBar()->showMessage(tr("Unable to open live signal"), 0);
        return;
    }
#endif
    sam.fileName = fileName;
    sam.acqDateTime.currentDateTime();
    child = createMdiChild( fileName, sam, io );
    reRead();
    statusBar()->showMessage(tr("Live signal established"), 10000);
    timerDisplay = new QTimer(this);
    connect(timerDisplay, SIGNAL(timeout()), this, SLOT(reRead()));
    timerDisplay->start(TIMER_PERIOD);   // immediately start rereading data
    return;
}

void MainInterface::on_actionReadSpectrum_triggered()
{
#ifdef HAVE_ALSA
    io.readSpectrum(sam);
#endif
}


void MainInterface::on_actionReadWaveforms_triggered()
{
#ifdef HAVE_ALSA
    io.readWaveforms(sam);
#endif
}

void MainInterface::reRead()  // used for display update only
{
    /*
    bool rtn;

    rtn = io.reRead(sam);
    if(rtn == false){
        statusBar()->showMessage(tr("Unable to re-read source of live data"), 0);
       return;
    }
    QApplication::setOverrideCursor( Qt::WaitCursor );
    child->rePlotSample(sam);
    //statusBar()->showMessage(tr("Live Data Updated"), 2000);
    child->showMaximized();
    QApplication::restoreOverrideCursor();
    */
}


void MainInterface::on_actionUnPause_triggered()
{
    if (timerDisplay != NULL) {
        timerDisplay->start(TIMER_PERIOD);
#ifdef HAVE_ALSA
        io.startDataCapture();
#endif
    }
}

void MainInterface::on_actionStop_triggered()
{
    if (timerDisplay != NULL) {
        timerDisplay->stop();
#ifdef HAVE_ALSA
        io.stopDataCapture();
#endif
    }
}

void MainInterface::on_actionPause_triggered()
{
    if (timerDisplay != NULL) {
        timerDisplay->stop();
#ifdef HAVE_ALSA
        io.stopDataCapture();
#endif
    }
}

void MainInterface::on_actionRestart_triggered()
{
    if (timerDisplay != NULL) {
        timerDisplay->start(TIMER_PERIOD);
#ifdef HAVE_ALSA
        io.startDataCapture();
#endif
    }
}

void MainInterface::on_actionSavePlot_triggered()
{
    if (child->savePlot() )
        statusBar()->showMessage(tr("Spectrum plot saved"), 2000);
    else
        statusBar()->showMessage(tr("Spectrum plot saved"), 2000);
}

void MainInterface::on_actionSaveSpectrum_triggered()
{
    QString fileName = QFileDialog::getSaveFileName( this,
        tr("Save spectrum"), spectraPath,
        tr("NIST n42 (*.n42)"));
    // --> make certain everything is stored in the Sample object
    if ( io.saveSpectrumSimple(fileName, sam) )
        statusBar()->showMessage(tr("Spectrum saved"), 5000);
    else
        statusBar()->showMessage(tr("Spectrum save failed"), 0);
  return;
}


void MainInterface::on_actionAnalyze_triggered()
{
    child->analyzeSpectrum();
    statusBar()->showMessage(tr("Spectrum analyzed"), 2000);
}

void MainInterface::on_actionSet_F100D3P_triggered()
{
    child->setF100D3P();
    statusBar()->showMessage(tr("F100D3P was set"), 2000);
}

void MainInterface::on_actionSet_XTRA_F100_triggered()
{
    child->setXTRA_F100();
    statusBar()->showMessage(tr("XTRA_F100 was set"), 2000);
}

int MainInterface::getCurrentStyle(){
    return(m_style);
}

void MainInterface::setStyle(int n){
    m_style = n;
}

void MainInterface::on_actionFileLocations_triggered()

{
    DirectoriesDlg *dirdlg = new DirectoriesDlg( spectraPath, analysisNuclibPath, calibNuclibPath );
    if ( dirdlg->exec() )       // exec() to be modal
    {
        QStringList newValues = dirdlg->getNewValues();
        spectraPath        = newValues[0];
        analysisNuclibPath = newValues[1];
        calibNuclibPath    = newValues[2];
    }
}

void MainInterface::on_actionLanguage_triggered()

{
    LanguageDlg *lngdlg = new LanguageDlg( getCurrentLang() );
    if ( lngdlg->exec() )       // exec() to be modal
    {
        QString newlang = lngdlg->getLanguage();
        if ( newlang != "")
        {
            m_lang = newlang;
            QMessageBox::information(this, "Interface's language",
            "The interface's language was set to " + m_lang + ".\n"
            "This setting will apply the next time you execute the application.");
        }
    }
}

void MainInterface::on_actionSignalSettings_triggered()
{
    SignalSettingsDlg *signaldlg = new SignalSettingsDlg() ;
    if ( signaldlg->exec() )       // exec() to be modal
    {
       return;
    }
}


void MainInterface::on_actionAbout_triggered()
{
   QMessageBox::about(this, "About OpenGammaX version " VERSION,
               "<b>OpenGammaX</b> is an opensource application<br>"
               "for gamma-ray and x-ray spectrum analysis.<br>"
               "Both live spectra and spectra files are supported.<br>"
               "Derived by LeDoyle Pingel, lrpingel@yahoo.com<br>"
               "from OpenGamma by Marcelo Francis M&aacute;duar,<br>"
               "mmaduar@ipen.br.<br><br>"
                "To start select a file or live signal source from file menu.");
}

void MainInterface::on_actionExit_triggered()
{
    qApp->closeAllWindows();
}


void MainInterface::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);

    //ui->actionReadSpectrumFile->setEnabled( hasMdiChild );
    //ui->actionReadLiveData->setEnabled( hasMdiChild );
    //ui->actionStartDataCapture->setEnabled( hasMdiChild );
    //ui->actionStopDataCapture->setEnabled( hasMdiChild );
    //ui->actionSaveSpectrumData->setEnabled( hasMdiChild );
    //ui->actionSave->setEnabled( hasMdiChild );
    //ui->actionSavePlot->setEnabled( hasMdiChild );
    //ui->actionSaveAs->setDisabled( true );
    ui->action_Paste->setEnabled(hasMdiChild);
    ui->actionClose->setEnabled(hasMdiChild);
    ui->actionClose_All->setEnabled(hasMdiChild);
    ui->actionTile->setEnabled(hasMdiChild);
    ui->actionCascade->setEnabled(hasMdiChild);
    ui->actionNext->setEnabled(hasMdiChild);
    ui->actionPrevious->setEnabled(hasMdiChild);
    ui->actionAnalyze->setEnabled(hasMdiChild);
    // ui->// separatorAct->setVisible(hasMdiChild);

    // bool hasSelection = (activeMdiChild() &&
    //                     activeMdiChild()->textCursor().hasSelection());
    //cutAct->setEnabled(hasSelection);
    //copyAct->setEnabled(hasSelection);
}

/*
void MainInterface::updateWindowMenu()
{
    ui->menuWindow->clear();
    ui->menuWindow->addAction( ui->actionClose );
    ui->menuWindow->addSeparator();
    ui->menuWindow->addAction( ui->actionTile );
    ui->menuWindow->addAction( ui->actionCascade );
    ui->menuWindow->addSeparator();
    ui->menuWindow->addAction( ui->actionNext );
    ui->menuWindow->addAction( ui->actionPrevious );
    ui->menuWindow->addSeparator();

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    // separatorAct->setVisible(!windows.isEmpty());  // ???
    for (int i = 0; i < windows.size(); ++i) {
         SpectrumForm *child = qobject_cast<SpectrumForm *>(windows.at(i)->widget());

         QString text;
         if (i < 9) {
             text = tr("&%1 %2").arg(i + 1)
                                .arg(child->userFriendlyCurrentFile());
         } else {
             text = tr("%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
         }
         QAction *action  = ui->menuWindow->addAction(text);
         action->setCheckable(true);
         action->setChecked(child == activeMdiChild());
         connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
         windowMapper->setMapping(action, windows.at(i));
    }
}
*/

SpectrumForm *MainInterface::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<SpectrumForm *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainInterface::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        SpectrumForm *mdiChild = qobject_cast<SpectrumForm *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainInterface::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;

    QList<QMdiSubWindow *> activatHist = mdiArea->subWindowList( QMdiArea::ActivationHistoryOrder );
    
    QStringList lista;
    foreach (QMdiSubWindow *win, activatHist) {
        SpectrumForm *mdiChild = qobject_cast<SpectrumForm *>(win->widget());
        lista += mdiChild->baseName();
    }

    SpectrumForm *lastChild = qobject_cast<SpectrumForm *>(activatHist.last()->widget());
    // statusBar()->showMessage(tr("adesat: ") + lista.last(), 2000);

    statusBar()->showMessage(tr("adesat: ") + lastChild->baseName(), 2000);

    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainInterface::writeSettings()
{
    QSettings settings("OpenGamma", "Settings");
    settings.setValue("lang", m_lang);
    settings.setValue("size", size());
    settings.setValue("pos", pos());

    settings.setValue("specPath",  spectraPath);
    settings.setValue("analysNucPath",  analysisNuclibPath);
    settings.setValue("calibNucPath",  calibNuclibPath);
}

void MainInterface::readSettings()
{
    QSettings settings("OpenGamma", "Settings");
    m_lang = settings.value("lang", "en").toString();
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    //
    spectraPath = settings.value("specPath", "c:").toString();
    analysisNuclibPath = settings.value("analysNucPath", "c:").toString();
    calibNuclibPath = settings.value("calibNucPath", "c:").toString();
}

