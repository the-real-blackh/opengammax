/***************************************************************************
 *   Copyright (C) 2007, 2009 by Marcelo Francis Maduar                    *
 *   maduar@gmail.com                                                      *
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

#ifndef OPENGAMMAMATHFUNCS_H
#define OPENGAMMAMATHFUNCS_H

#include <QList>

#include "opengammamathfuncs_global.h"
#include "../gslcontainer/doblst.h"

OPENGAMMAMATHFUNCSSHARED_EXPORT void qquercoisa();
OPENGAMMAMATHFUNCSSHARED_EXPORT qreal humMil( QList<Doblst> parms, double t );
OPENGAMMAMATHFUNCSSHARED_EXPORT qreal poly( Doblst, qreal t );
OPENGAMMAMATHFUNCSSHARED_EXPORT double SecondDerivGauss( double x, double ai, double ki, double ci );
OPENGAMMAMATHFUNCSSHARED_EXPORT void SecondDerivGaussPartials( double x, double s, double ai, double ki, double ci,
                                         double &Ji0, double &Ji1, double &Ji2 );

OPENGAMMAMATHFUNCSSHARED_EXPORT qreal gaussian ( Doblst a, double t );
OPENGAMMAMATHFUNCSSHARED_EXPORT qreal erf_i (Doblst a, double t);

OPENGAMMAMATHFUNCSSHARED_EXPORT qreal rightw ( QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t );
OPENGAMMAMATHFUNCSSHARED_EXPORT qreal baselineErfWeightedPolys ( QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t );

// 2010-05-19: ELIMINAR essa abaixo
// OPENGAMMAMATHFUNCSSHARED_EXPORT Doblst cubicSplineCalcParams ( qreal tP, qreal tQ, Doblst cp, Doblst cq );

OPENGAMMAMATHFUNCSSHARED_EXPORT qreal baselineCubicSpline ( QList<Doblst> pms, double t );
OPENGAMMAMATHFUNCSSHARED_EXPORT qreal baselineMean ( QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t );
OPENGAMMAMATHFUNCSSHARED_EXPORT qreal netmultiplet ( Doblst kwidth, Doblst cen, Doblst hei, double t);
OPENGAMMAMATHFUNCSSHARED_EXPORT qreal grossmultiplet ( QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t );
OPENGAMMAMATHFUNCSSHARED_EXPORT qreal plotgrossmultiplet ( QList<Doblst> parms, double t );

OPENGAMMAMATHFUNCSSHARED_EXPORT qreal test_d ( int j, QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t );

OPENGAMMAMATHFUNCSSHARED_EXPORT qreal dgdci ( int j, QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t );
OPENGAMMAMATHFUNCSSHARED_EXPORT qreal dgdai ( int j, QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t );

class OPENGAMMAMATHFUNCSSHARED_EXPORT OpengammaMathFuncs {
public:
    OpengammaMathFuncs();
};

#endif // OPENGAMMAMATHFUNCS_H
