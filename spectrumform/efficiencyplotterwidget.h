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

#ifndef EFFICIENCYPLOTTERWIDGET_H
#define EFFICIENCYPLOTTERWIDGET_H

#include <QStandardItemModel>

#include "../gslcontainer/doblst.h"
#include "../gslcontainer/functionsdefinitions.h"
#include "plotterwidget.h"

class EfficiencyPlotterWidget : public PlotterWidget
{
    Q_OBJECT
public:
    EfficiencyPlotterWidget( QString &title, QWidget *parent=0 );
    void plotBarsAndCurve( Doblst xlst, Doblst ylst, Doblst slst, Doblst parms );
    void setEfficiencyErrBars( Doblst x, Doblst y, Doblst s);
private slots:
    void changed1( int state );
    void changedErrBars( int state );
};

#endif // EFFICIENCYPLOTTERWIDGET_H
