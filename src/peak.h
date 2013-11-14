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

#ifndef PEAK_H
#define PEAK_H

#include <QString>
#include "constants.h"

class Peak : public QString
{
//	Q_OBJECT
public:
    void setCentroidAndS( double x, double y ) { mcentroid = x; mscentroid = y; }
    void setFwhmAndS( double x, double y ) { mfwhm = x; msfwhm = y; }
    void setHeightAndS( double x, double y ) { mheight = x; msheight = y;}
    void setFittedCentroid( double x, double y ) { mfitcentroid = x; msfitcentroid = y; }
    void setFittedNetHeightAndS( double x, double y ) { mnetheight = x; msnetheight = y; }
    void setIsUsedForCalibration( bool b ) { mIsUsedForCalibration = b; }
    double pkCentroid() { return mcentroid; }
    double sPkCentroid() { return mscentroid; }
    double fwhm() { return mfwhm; }
    double sfwhm() { return msfwhm; }
    double height() { return mheight; }
    double sheight() { return msheight; }
    double fittedHeight() { return mnetheight; }
    double sfittedHeight() { return msnetheight; }
    double fittedCentroid() { return mfitcentroid; }
    double sfittedCentroid() { return msfitcentroid; }
    bool isUsedForCalibration() { return mIsUsedForCalibration; }
private:
    double mcentroid; // experimental centroid channel
    double mscentroid; // and its uncert
    double mfwhm; // experimental fwhm
    double msfwhm; // and its uncert
    double mheight; // experimental height
    double msheight; // and its uncert
    double mnetheight; // multiplet-fitted net height
    double msnetheight; // and its uncert
    double mfitcentroid; // multiplet-fitted centroid
    double msfitcentroid; // and its uncert
    bool mIsUsedForCalibration; // whether is used in channel/energy/fwhm calibration
};

#endif /*PEAK_H_*/
