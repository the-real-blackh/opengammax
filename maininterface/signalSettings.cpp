/***************************************************************************
 * # Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com                                                    *
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

#include <QtGui>
#include <qwt_plot_curve.h>
#include "maininterface.h"
#include "signalSettings.h"


SignalSettingsDlg::SignalSettingsDlg( QWidget *parent )
    : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(tr("Signal Settings"));
    if ( curInvert == TRUE)
        ui.invertSignal_rbt->setChecked( true );
    else
        ui.noInvertSignal_rbt->setChecked( true );
    ui.threshold_sb->setValue(curThreshold);
}

void SignalSettingsDlg::on_okButtonSignal_clicked()
{
    if(ui.invertSignal_rbt->isChecked())
        curInvert = TRUE;
    if(ui.noInvertSignal_rbt->isChecked())
       curInvert = FALSE;
    curThreshold = ui.threshold_sb->value();
    SpectrumIO *specio = new SpectrumIO;
    specio->setSettings( curInvert,  curThreshold);
    this->done(0);
}    
