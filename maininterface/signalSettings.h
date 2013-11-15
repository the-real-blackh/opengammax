/***************************************************************************
 *  # Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SIGNAL_SETTINGS_H
#define SIGNAL_SETTINGS_H

#include "ui_signalSettings.h"

static bool curInvert = true;
static int curThreshold = 200;      // out of ADC range of 16384

class SignalSettingsDlg : public QDialog
{
    Q_OBJECT
public:
    SignalSettingsDlg( QWidget *parent = 0 );
   bool getInvert() {return curInvert;};
    int getThreshold(){return curThreshold;};
public slots:
    void on_cancelButtonSignal_clicked() {this->done(0);};
    void on_okButtonSignal_clicked();
private:
    Ui::SignalSettingsDlg ui;
};

#endif // SIGNAL_SETTINGS_H
