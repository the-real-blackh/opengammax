/***************************************************************************
 *  Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com                                                    *
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
#include "style.h"

StyleDlg::StyleDlg( const int curStyle, QWidget *parent )
  : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(tr("Graphing Style"));
    if ( curStyle == QwtPlotCurve::Lines)
        ui.Lines_rbt->setChecked( true );
    else if ( curStyle == QwtPlotCurve::Sticks)
        ui.Sticks_rbt->setChecked( true );
    else if ( curStyle == QwtPlotCurve::Steps)
        ui.Steps_rbt->setChecked( true );
    else if ( curStyle == QwtPlotCurve::Dots)
        ui.Dots_rbt->setChecked( true );
    else if ( curStyle == QwtPlotCurve::UserCurve)
        ui.ErrorBars_rbt->setChecked( true );
}

void StyleDlg::on_okButtonGetStyle(QPoint)
{
#if 0
    // --> finish this up later
    if ( ui.Lines_rbt->isChecked() )
       Maininterface::setStyle(QwtPlotCurve::Lines);
    else if ( ui.Sticks_rbt->isChecked() )
       Maininterface::setStyle (QwtPlotCurve::Sticks);
    else if ( ui.Steps_rbt->isChecked() )
       return (QwtPlotCurve::Steps);
    else if ( ui.Dots_rbt->isChecked() )
       Maininterface::setStyle (QwtPlotCurve::Dots);
    else if ( ui.ErrorBars_rbt->isChecked() )
       Maininterface::setStyle (QwtPlotCurve::UserCurve);
    else
       Maininterface::setStyle (0);
#endif
    return;
}    
