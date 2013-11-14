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

#ifndef FUNCTIONRANGE_H
#define FUNCTIONRANGE_H

#include <QObject>
#include "../gslcontainer/doblst.h"

class FunctionRange : public QObject
{
    Q_OBJECT
public:
    enum Parametrization { yParms, xyParms, yMatrParms, xyMatrParms };
    Parametrization typeParam;
    qreal (*x) (Doblst, qreal);
    qreal (*y) (Doblst, qreal);
    qreal (*yMatr) (QList<Doblst>, qreal);
    Doblst parmsx;
    Doblst parmsy;
    QList<Doblst> parmsyVec;
    qreal t0;
    qreal t1;
};

#endif // FUNCTIONRANGE_H
