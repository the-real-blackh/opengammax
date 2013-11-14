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

#include <QTextStream>
#include <QString>

#include "peaklist.h"
#include "constants.h"
#include "../opengammamathfuncs/opengammamathfuncs.h"
#include "gslcontainer.h"

void PeakList::appendPeak( Peak *pk )
{
	*this << pk;
}

void PeakList::addNewPeak( double centroid, double sCentroid,
                           double fwhm, double sfwhm,
                           double height, double sheight )
{
	Peak *pk = new Peak();
        pk->setCentroidAndS( centroid, sCentroid );
        pk->setFwhmAndS( fwhm, sfwhm );
        pk->setHeightAndS ( height, sheight );
        pk->setIsUsedForCalibration( true );
	*this << pk;
}

qreal PeakList::sFittedEnergy( GenericCalibration *enCal, int i )
{
    qreal dEdCh = GSL_NAN;
    if (enCal->nParms() == 3)
        dEdCh = enCal->parm(1) + 2 * enCal->parm(2) * centroid(i);
    if (enCal->nParms() == 2)
        dEdCh = enCal->parm(1);
    return sCentroid(i) * qAbs(dEdCh);
}

qreal PeakList::fwhmEn( GenericCalibration *enCal, int i )
{
    qreal hwhm = 0.5 * fwhmCh( i );
    return enCal->y( centroid(i) + hwhm ) - enCal->y( centroid(i) - hwhm );
}

qreal PeakList::sFwhmEn( GenericCalibration *enCal, int i )
{
    qreal hsfwhm = 0.5 * sfwhmCh(i);
    return enCal->y( centroid(i) + hsfwhm ) - enCal->y( centroid(i) - hsfwhm );
}

void PeakList::setFittedPeak( int i, qreal c, qreal sc, qreal h, qreal sh )
{
    at(i)->setFittedCentroid( c, sc );
    at(i)->setFittedNetHeightAndS( h, sh );
}

Doblst PeakList::getFittedPeak( GenericCalibration *fwhmCal, int i )
{
    Doblst ret; // fitted: height << kwidth << centroid
    ret << at(i)->fittedHeight()
      << fittedK( fwhmCal, i )
      << at(i)->fittedCentroid();
    return ret;
}

bool PeakList::calcPeakWidths( int nSdCh, Counts *cts, GslContainer *gslcontnr, QString &rep )
{
    clear();
    rep = "Peaks widths calculation:\n";
    for (int i=0; i < cts->ncands(); ++i)
    {
        qreal ch, sch, width, swidth, height, sheight;
        Doblst secDiffRange;
        Doblst sigmaSecDiffRange;
        rep += QString("Peak %1:\n").arg(i);
        QString fitRep;
        if ( cts->getSecDiffRange( cts->cand(i), nSdCh, secDiffRange, sigmaSecDiffRange ) ) {
            // next func. returns:
            // ch, width, swidth, height, sheight, fitrep
            gauss2ndDerivFit( secDiffRange, sigmaSecDiffRange,
              ch, sch, width, swidth, height, sheight, fitRep,
              gslcontnr );
            qreal sfwhm = 0.1;
            qreal fwhm = 1.0;
            if ( width > 0.0 ) {
                sfwhm = sqrt(log(2.0)) * pow(width, -1.5) * swidth;
                fwhm = TWOSQRTLN2 * pow(width, -0.5);
            }
            // 2009-09-15 AQUI criterio para escolha de picos no ajuste FWHM
            // 2009-09-15 HERE criterion for choosing the set of peaks FWHM

            if ( ( sfwhm / fwhm < 2.0 ) && ( sheight / height < 2.0 ) ) {
                // if it's a good peak, apply energy cal and add it
                // qreal fEn, sfEn, dEdk;
                // fE(k) = E'(k) * fk(k), where E'(k) = 2 * aE * k + bE
                // dEdk = 2 * energyCalibration->parm(2) * ch + energyCalibration->parm(1);
                // fEn = dEdk * fwhm;
                // sfEn = dEdk * sfwhm;
                addNewPeak( ch - nSdCh + cts->cand(i), sch, fwhm, sfwhm, height, sheight );
            }
        }
        else
            fitRep = "Fit was not performed";
        rep += fitRep;
    }
    return !( isEmpty() );
}

bool PeakList::gauss2ndDerivFit( Doblst secDiffRange, Doblst sigmaSecDiffRange,
    qreal& centr0, qreal& sCentr0, qreal& width, qreal& swidth,
    qreal& height, qreal& sheight, QString& rep, GslContainer *gslcontnr )

{
    int p = 3;
    Doblst t, xinit, y, sigmay;
    int nPt = GSL_MIN( secDiffRange.count(), sigmaSecDiffRange.count() );
    if ( nPt < 3 ) return false;
    int ch0 = (nPt-1) / 2 ; // <-- central channel of the range
    height = fabs( secDiffRange[ch0] );
    xinit << height << 1.0 << qreal( ch0 );
    for (int j=0; j < nPt; j++)
    {
        t << qreal(j);
        y << secDiffRange[j];
        sigmay << sigmaSecDiffRange[j];
    }

    Doblst fp; Intlst pfp; // In this case is dummy, can be left empty
    gslcontnr->genericNonlinearFunc2( nPt, p, t, xinit, y, sigmay,
      gslcontnr->dgauss_f, gslcontnr->dgauss_df, gslcontnr->dgauss_fdf, rep, fp, pfp );
    int iter;
    Doblst x;
    Doblst sigmax;
    double chisqoverdeg;
    int status;
    rep += "===============getResults:***=================== \n";
    gslcontnr->getResultsLevMar( iter, x, sigmax, chisqoverdeg, status );
    height = x[0];
    sheight = sigmax[0];
    width = x[1];
    swidth = sigmax[1];
    centr0 = x[2];
    sCentr0 = sigmax[2];
    return true;
}
