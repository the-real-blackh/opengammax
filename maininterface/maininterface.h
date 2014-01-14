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

#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <QMainWindow>
#ifdef Q_OS_LINUX
#include <QMdiSubWindow>
#endif
#ifdef Q_OS_WIN32
#include <QMdiSubwindow>
#endif
#include <QSignalMapper>
    //SpectrumIO *io = new SpectrumIO(this);
#include "ui_maininterface.h"
#include "spectrumform.h"
#include "spectrumformmodels.h"


//#define NO_TIMER
#define TIMER_PERIOD  2000


class MainInterface : public QMainWindow
{
    Q_OBJECT
public:
    MainInterface(QWidget *parent = 0);
    ~MainInterface();
    QString getCurrentLang();
    int getCurrentStyle();
    void setStyle(int n);
    void readLiveSignal();

protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void on_actionReadSpectrumFile_triggered();
    void on_actionOpenLiveSignal_triggered();
    void on_actionReadSpectrum_triggered();
    void on_actionReadWaveforms_triggered();
    void on_actionRestart_triggered();
    void on_actionStop_triggered();
    void on_actionPause_triggered();
    void on_actionUnPause_triggered();
    void on_actionSaveSpectrum_triggered();
    void reRead();                            // used by timer for continuous update of display
    void on_actionSavePlot_triggered();
    void on_actionAnalyze_triggered();
    void on_actionSet_F100D3P_triggered();
    void on_actionSet_XTRA_F100_triggered();
    void on_actionExit_triggered();
    void on_actionFileLocations_triggered();
    void on_actionLanguage_triggered();
    //void on_actionPlottingStyle_triggered();
    void on_actionSignalSettings_triggered();
    void on_actionAbout_triggered();
    void updateMenus();
    //void updateWindowMenu();

    void setActiveSubWindow(QWidget *window);
private:
    void continueLS();
    void stopLS();
    void pauseLS();
    void reStartLS();

    SpectrumForm *createMdiChild( const QString & fileName, Sample & samp, SpectrumIO & io,
        std::function<void()> onClose );
    void openSignalSource();
    Ui::MainInterfaceClass *ui;
    QMdiArea *mdiArea;
    QSignalMapper *windowMapper;
    SpectrumForm *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);
    void readSettings();
    void writeSettings();
    void getSpecifiedDirectoryResults( QString Diretorio );
    // default paths:
    QString spectraPath;
    QString analysisNuclibPath;
    QString calibNuclibPath;
    QString m_lang;
    int m_style;
    bool readWaveform;
};

#endif // MAININTERFACE_H
