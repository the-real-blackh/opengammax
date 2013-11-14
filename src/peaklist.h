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

#ifndef PEAKLIST_H
#define PEAKLIST_H

#include "peak.h"
#include "gslcontainer.h"
#include "genericcalibration.h"
#include "counts.h"

class PeakList : public QList<Peak*>
{
public:
    void appendPeak( Peak *pk );
    void addNewPeak( double, double, double, double, double, double );
    void setFittedPeak( int i, qreal c, qreal sc, qreal h, qreal sh );
    Doblst getFittedPeak( GenericCalibration *fwhmCal, int i );
    qreal fwhm( int i ) { return at(i)->fwhm(); }
    qreal sfwhm( int i ) { return at(i)->sfwhm(); }
    qreal height( int i ) { return at(i)->height(); }
    qreal sheight( int i ) { return at(i)->sheight(); }
    void setIsUsedForCalibration( int i, bool b ) { at(i)->setIsUsedForCalibration( b ); }
    bool isUsedForCalibration( int i ) { return at(i)->isUsedForCalibration(); }
    //
    Doblst fittedEnergies( GenericCalibration *enCal )
      { Doblst l; for (int i=0; i < size(); i++) l << fittedEnergy( enCal, i ); return l; }
    Doblst sFittedEnergies( GenericCalibration *enCal )
      { Doblst l; for (int i=0; i < size(); i++) l << sFittedEnergy( enCal, i ); return l; }
    Doblst fwhmsInEnergy( GenericCalibration *enCal )
      { Doblst l; for (int i=0; i < size(); i++) l << fwhmEn( enCal, i ); return l; }
    Doblst sFwhmsInEnergy( GenericCalibration *enCal )
      { Doblst l; for (int i=0; i < size(); i++) l << sFwhmEn( enCal, i ); return l; }

    Doblst centroids() { Doblst l; for (int i=0; i < count(); i++) l << at(i)->pkCentroid(); return l; }
    Doblst peaksHeights() { Doblst l; for (int i=0; i < count(); i++) l << at(i)->height(); return l; }

    int npks() { return count(); }

    qreal centroid( int i ) { return at(i)->pkCentroid(); }
    qreal sCentroid( int i ) { return at(i)->sPkCentroid(); }

    qreal fwhmCh( int i ) { return fwhm(i); }
    qreal sfwhmCh( int i ) { return sfwhm(i); }

    qreal fittedEnergy( GenericCalibration *enCal, int i ) { return enCal->y( centroid(i) ); }
    qreal sFittedEnergy( GenericCalibration *enCal, int i );
    qreal fwhmEn( GenericCalibration *enCal, int i );
    qreal sFwhmEn( GenericCalibration *enCal, int i );
    // qreal libEnergy( int i );
    // QString libNucName( int i );


    qreal fittedFwhm( GenericCalibration *fwhmCal, int i ) { return fwhmCal->y( centroid(i) ) ; } // ajus

    qreal fittedHeight( int i ) { return at(i)->fittedHeight()         ; } // ajus
    qreal sfittedHeight( int i ) { return at(i)->sfittedHeight()       ; } // ajus
    qreal fittedCentroid( int i ) { return at(i)->fittedCentroid()    ; }
    qreal sfittedCentroid( int i ) { return at(i)->sfittedCentroid()  ; }
    qreal fittedK( GenericCalibration *fwhmCal, int i )
        { return FOURLN2 * pow( fittedFwhm( fwhmCal, i ), -2.0 )      ; }

    qreal netArea( GenericCalibration *fwhmCal, int i ) { return FACTAREA * fittedHeight(i) * fittedFwhm( fwhmCal, i ); }
    qreal sNetArea( GenericCalibration *fwhmCal, int i ) { return FACTAREA * sfittedHeight(i) * fittedFwhm( fwhmCal, i ); }

    bool calcPeakWidths( int nSdCh, Counts *cts, GslContainer *gslcontnr, QString &rep );

private:
    bool gauss2ndDerivFit( Doblst secDiff, Doblst sigmaSecDiff,
        qreal& centr0, qreal& sCentr0, qreal& width, qreal& swidth,
        qreal& height, qreal& sheight, QString& rep, GslContainer *gslcontnr );

};

#endif /*PEAKLIST_H_*/
