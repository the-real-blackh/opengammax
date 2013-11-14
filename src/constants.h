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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <gsl/gsl_math.h>

//!  Constantes reais.
/*!
  Constantes reais num&eacute;ricas usadas nos c&aacute;lculos.
*/

const double TWOSQRTLN2 = 2 * sqrt(log(2));
const double FOURLN2 = 4.0 * log(2.0);
const double FACTAREA = 0.5 * sqrt(3.14159265 / log(2.0) );

// used in:
// qreal fwhm = TWOSQRTLN2 * pow(width, -0.5);
// parms << FOURLN2 * pow( at(i)->fittedFwhm(), -2.0 );

#endif // CONSTANTS_H
