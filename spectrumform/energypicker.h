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


#ifndef ENERGYPICKER_H
#define ENERGYPICKER_H

#include <qwt_plot_picker.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>

#include "genericcalibration.h"

class EnergyPicker : public QwtPlotPicker
{
public:
    EnergyPicker( Doblst enParms, QWidget *can ) :
        QwtPlotPicker (QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, can )
            { enCal = new GenericCalibration( enParms, FunctionsDefinitions::poly, this );
    }
    void setPickerEnCal( Doblst a );
    void setPickerEnCalDefault();
    // reimplemented from QwtPlotPicker:
    QwtText trackerText( const QPointF &pos ) const;
private:
    GenericCalibration *enCal;
};

#endif // ENERGYPICKER_H
