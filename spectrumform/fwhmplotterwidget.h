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

#ifndef FWHMPLOTTERWIDGET_H
#define FWHMPLOTTERWIDGET_H

#include <QStandardItemModel>

#include "../gslcontainer/doblst.h"
#include "../gslcontainer/functionsdefinitions.h"
#include "plotterwidget.h"
#include "peaklist.h"
#include "multiplets.h"

class FwhmPlotterWidget : public PlotterWidget
{
    Q_OBJECT
public:
    FwhmPlotterWidget( QString &title, QWidget *parent=0 );
    void setNCh( int intNCh );
    void setMultidimPoints( Doblst pma, Doblst pmb, Doblst pmc,
                                                     Doblst pmd, Doblst pme );
    void plotFwhmFit( Doblst, Doblst, qreal (*) (Doblst, qreal), qreal (*) (QList<Doblst>, qreal) );

    void plotFwhmPoints();
//    void plotPoints ();
    void unplotFit();
    void unplotErrBars();

private:
    Doblst centroids;
    Doblst energies;
    Doblst sEnergies;
    Doblst fwhms;
    Doblst sFwhms;

    Doblst enParms;
    Doblst fwhmParms;
    qreal nCh;
};

#endif // FWHMPLOTTERWIDGET_H
