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



#include "multiplets.h"
#include "../opengammamathfuncs/opengammamathfuncs.h"

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_erf.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_linalg.h>

Multiplets::Multiplets( PeakList *pklist, Counts *counts,
    GenericCalibration *fwhmCalibration, QObject *parent )
  : QObject( parent )
{
    pks = pklist;
    fwhmCal = fwhmCalibration;
    cts = counts;
    idpks = new QList<Intlst>;
    isValids.clear();
    leftBaseLines = new QList<GenericCalibration *>;
    rightBaseLines = new QList<GenericCalibration *>;
    innerCubicBaseLines = new QList<GenericCalibration *>;
    netMultiplets = new QList<GenericCalibration *>;
    netCountsRange = new QList<Doblst>;
}

Multiplets::~Multiplets()
{
    qDeleteAll( *leftBaseLines );
    qDeleteAll( *rightBaseLines );
    qDeleteAll( *innerCubicBaseLines );
    qDeleteAll( *netMultiplets );
}

void Multiplets::clearAll()
{
    qDeleteAll( *leftBaseLines );
    qDeleteAll( *rightBaseLines );
    qDeleteAll( *innerCubicBaseLines );
    qDeleteAll( *netMultiplets );
    leftBaseLines->clear();
    rightBaseLines->clear();
    innerCubicBaseLines->clear();
    netMultiplets->clear();
}

void Multiplets::resolve( qreal noFwhmOfPeak, qreal noFwhmOfSideBaseline, int typeBaseline,
  int baselinePolyGrade, int interpolationMethod, int fitStrategy )
{
    clearAll();
    idpks->clear();
    isValids.clear();
    defineMultiplets( noFwhmOfPeak + noFwhmOfSideBaseline );
    defineRegions( noFwhmOfSideBaseline, noFwhmOfPeak );
    qDeleteAll( *leftBaseLines );
    leftBaseLines->clear();
    qDeleteAll( *rightBaseLines );
    rightBaseLines->clear();
    defineSideBaselines( typeBaseline, baselinePolyGrade );
    defineInnerBaselines( interpolationMethod, typeBaseline );
    calculateNetCounts(); // will define netMultiplets list
    // doMultipletFit();
}

void Multiplets::defineMultiplets( qreal maxNoFwhmSeparationInMult )
{
    Intlst first;
    first << 0;
    *idpks << first;
    isValids << true;
    for ( int i=1; i < pks->count(); i++ ) {
        // if peak i is far from peak i-1, create an empty multiplet
        qreal distInCh = pks->centroid(i) - pks->centroid(i-1);

        qreal localFwhm = pks->fittedFwhm( fwhmCal, i );
        qreal distInFwhm = distInCh / localFwhm;
        if (  distInFwhm > maxNoFwhmSeparationInMult )
        {
            // new multiplet
            Intlst newmult;
            *idpks << newmult;
            isValids << true;
        }
        // include it in the last multiplet
        idpks->last() << i;
    }
}

void Multiplets::defineRegions( qreal noFwhmOfSideBaseline, qreal noFwhmOfPeak )
{
    for ( int m=0; m < nMult(); m++ )
    {
        int idFirst = idpks->at(m).first();
        int idLast = idpks->at(m).last();
        iniChMuLst << qRound(
            pks->centroid( idFirst )
            - ( 0.5 * noFwhmOfPeak + noFwhmOfSideBaseline ) * pks->fittedFwhm( fwhmCal, idFirst )
            );
        finChMuLst << qRound(
            pks->centroid( idLast )
            + ( 0.5 * noFwhmOfPeak + noFwhmOfSideBaseline ) * pks->fittedFwhm( fwhmCal, idLast )
            );
        finChLeftLst << qRound( pks->centroid( idFirst )
            - 0.5 * noFwhmOfPeak * pks->fittedFwhm( fwhmCal, idFirst ) );
        iniChRightLst << qRound( pks->centroid( idLast )
            + 0.5 * noFwhmOfPeak * pks->fittedFwhm( fwhmCal, idLast ) );
        // 2010-04-22 AQUI!! IMPLEMENTAR:
        // deleting of peaks very close to the spectrum ends:
    }
}

void Multiplets::defineSideBaselines( int typeBaseline, int polyGrade )
{
    if ( typeBaseline == 0 ) {
        blFitFunc = FunctionsDefinitions::expSimple;
    }
    if ( typeBaseline == 1 ) {
        blFitFunc = FunctionsDefinitions::poly;
        baselinePolyGrade = polyGrade;
    }
    if ( typeBaseline == 2 ) {
        blFitFunc = FunctionsDefinitions::expPlusConst;
    }
    for ( int m=0; m < nMult(); m++ ) {
        Doblst xL, bgL, sBgL, xR, bgR, sBgR;
        bool validRange = true;
        GenericCalibration *leftBL = new GenericCalibration( blFitFunc, this );
        GenericCalibration *rightBL = new GenericCalibration( blFitFunc, this );
        *leftBaseLines << leftBL;
        *rightBaseLines << rightBL;
        validRange = validRange && getRange( iniChMuLst[m], finChLeftLst[m], xL, bgL, sBgL );
        validRange = validRange && getRange( iniChRightLst[m], finChMuLst[m], xR, bgR, sBgR );
        bool validCalib = true;
        if ( validRange ) {
            leftBL->setPolyGrade( baselinePolyGrade );
            validCalib = validCalib && leftBL->doCalib( xL, bgL, sBgL );
            rightBL->setPolyGrade( baselinePolyGrade );
            validCalib = validCalib && rightBL->doCalib( xR, bgR, sBgR );
        }
        if ( !validCalib ) {
            setInvalid(m);
        }
    }
}

void Multiplets::defineInnerBaselines(int interpolationMethod, int typeBaseline)
{
    if ( interpolationMethod == 0 ) // peak-height weight
        for ( int m=0; m < nMult(); m++ )
            peakHeightWeightInterpolation( m );//
    if ( interpolationMethod == 1 ) {                   // cubic spline
        qDeleteAll( *innerCubicBaseLines );
        /*
        if ( typeBaseline == 0 ) {         // base line: exp simple
            for ( int m=0; m < nMult(); m++ ) {
                GenericCalibration *cubicBL = new GenericCalibration( FunctionsDefinitions::poly, this );
                Doblst absoluteLeftBL;
                Doblst absoluteRightBL;

                qreal iniCh;
                iniCh = -qreal( iniLeft(m) );
                qreal p0, p1, p2;
                p0 = leftBL(m)[0] + leftBL(m)[1] * iniCh +   leftBL(m)[2] * pow(iniCh,2);
                p1 =                leftBL(m)[1]         + 2*leftBL(m)[2] * iniCh;
                p2 =                                          leftBL(m)[2];
                absoluteLeftBL << p0 << p1 << p2;

                iniCh = -qreal( iniRight(m) );
                absoluteRightBL
                  << rightBL(m)[0] + rightBL(m)[1] * iniCh +   rightBL(m)[2] * pow(iniCh,2)
                  <<                 rightBL(m)[1]         + 2*rightBL(m)[2] * iniCh
                  <<                                           rightBL(m)[2]
                                                                                      ;
                cubicBL->setParms( cubicSplineCalcParams( finLeft(m), iniRight(m), absoluteLeftBL, absoluteRightBL ) );
                *innerCubicBaseLines << cubicBL;
            }
        }
        */
        if ( typeBaseline == 1 ) {         // base line: poly
            for ( int m=0; m < nMult(); m++ ) {
                if ( m == 0 ) {
                    m = 0;
                }
                GenericCalibration *cubicBL = new GenericCalibration( FunctionsDefinitions::poly, this );
                *innerCubicBaseLines << cubicBL;
                if ( isValid(m) ) {
                    Doblst absoluteLeftBL;
                    Doblst absoluteRightBL;

                    // qreal p0, p1, p2;

                    Doblst lBL = leftBL(m);
                    Doblst rBL = rightBL(m);

                    int patch = qMin( lBL.count(), rBL.count() );
                    if ( patch < 2 )
                        setInvalid(m);
                    if ( isValid(m) ) {

                        qreal tP = finLeft(m);
                        qreal tQ = iniRight(m);

                        qreal iniCh;

                        iniCh = -qreal( iniLeft(m) );
                        qreal pP = poly( leftBL(m), iniCh + tP  );
                        Doblst leftDer;
                        for (int i=1; i < leftBL(m).count(); i++ ) {
                            qreal zzz = i * leftBL(m).at(i);
                            leftDer << zzz;
                        }
                        qreal pPline = poly( leftDer, iniCh + tP  );


                        iniCh = -qreal( iniRight(m) );
                        qreal qQ = poly ( rightBL(m), iniCh + tQ );
                        Doblst rightDer;
                        for (int i=1; i < rightBL(m).count(); i++ )
                            rightDer << i * rightBL(m).at(i);
                        qreal qQline = poly( rightDer, iniCh + tQ  );

                        double a_data[] = { 1.0,  tP, pow(tP,2),   pow(tP,3),
                                            0.0, 1.0,      2*tP, 3*pow(tP,2),
                                            1.0,  tQ, pow(tQ,2),   pow(tQ,3),
                                            0.0, 1.0,      2*tQ, 3*pow(tQ,2)  };

                        double b_data[] = { pP, pPline, qQ, qQline };

                        gsl_matrix_view m = gsl_matrix_view_array (a_data, 4, 4);
                        gsl_vector_view b =  gsl_vector_view_array (b_data, 4);
                        gsl_vector *x = gsl_vector_alloc (4);
                        int s;
                        gsl_permutation * p = gsl_permutation_alloc (4);
                        gsl_linalg_LU_decomp (&m.matrix, p, &s);
                        gsl_linalg_LU_solve (&m.matrix, p, &b.vector, x);
                        gsl_permutation_free (p);

                        Doblst splineParams;
                        for (int i=0; i<4; i++) splineParams << gsl_vector_get( x, i);

                        cubicBL->setParms( splineParams );
                    }
                }
            }
        }
        /*
        if ( typeBaseline == 2 ) {         // base line: Exp + const

           for ( int m=0; m < nMult(); m++ ) {
                GenericCalibration *cubicBL = new GenericCalibration( FunctionsDefinitions::poly, this );
                Doblst absoluteLeftBL;
                Doblst absoluteRightBL;
                qreal iniCh;
                iniCh = -qreal( iniLeft(m) );
                qreal p0, p1, p2;
                p0 = leftBL(m)[0] + leftBL(m)[1] * iniCh +   leftBL(m)[2] * pow(iniCh,2);
                p1 =                leftBL(m)[1]         + 2*leftBL(m)[2] * iniCh;
                p2 =                                          leftBL(m)[2];
                absoluteLeftBL << p0 << p1 << p2;
                iniCh = -qreal( iniRight(m) );
                absoluteRightBL
                  << rightBL(m)[0] + rightBL(m)[1] * iniCh +   rightBL(m)[2] * pow(iniCh,2)
                  <<                 rightBL(m)[1]         + 2*rightBL(m)[2] * iniCh
                  <<                                           rightBL(m)[2]
                                                                                      ;
                cubicBL->setParms( cubicSplineCalcParams( finLeft(m), iniRight(m), absoluteLeftBL, absoluteRightBL ) );
                *innerCubicBaseLines << cubicBL;
            }
        }
        */
    }
}

void Multiplets::peakHeightWeightInterpolation( int m )
{

}

void Multiplets::calculateNetCounts()
{
    qDeleteAll( *netMultiplets );
    netMultiplets->clear();
    for ( int m=0; m < nMult(); m++ ) {
        GenericCalibration *netMultiplet = new GenericCalibration(
                FunctionsDefinitions::gaussiansSum, this );
        *netMultiplets << netMultiplet;
        if ( isValid(m) ) {
            int iniRange = finChLeftLst[m] + 1;
            int finRange = iniChRightLst[m] - 1;

            Doblst x, y, s;
            // getRange drops 0-count chans before build x, y, s
            // x is the "shifted" (by <iniRange> channels) channel, i.e., x[0] is always 0
            // and y[i] is the counts at channel < iniRange + x[i] >
            getRange( iniRange, finRange, x, y, s );

            int nPt = x.count(); // = y.count() = s.count()
            Doblst baselineCounts, netCounts;
            for ( int i = 0; i < nPt; i++ ) {
                baselineCounts << innerCubicBaseLines->at(m)->y( x[i] + iniRange );
                netCounts << y[i] - baselineCounts[i];
            }

            Doblst fPms;
            for ( int j = 0; j < nPk(m); j++ ) {
                fPms << pks->fittedK( fwhmCal, idPk(m,j) );
            }

            Doblst iniVals;
            for ( int j = 0; j < nPk(m); j++ ) {
                iniVals << pks->height( idPk(m,j) );
            }
            for ( int j = 0; j < nPk(m); j++ ) {
                iniVals << pks->centroid( idPk(m,j) ) - iniRange;
            }

            netMultiplet->setFixedParms( fPms );
            netMultiplet->setInitVals( iniVals );
            netMultiplet->doCalib( x, netCounts, s );

            for ( int j = 0; j < nPk(m); j++ ) {
                qreal hei = netMultiplet->parm(j);
                qreal cen = iniRange + netMultiplet->parm(j + nPk(m));
                qreal shei = netMultiplet->sparm(j);
                qreal scen = netMultiplet->sparm(j + nPk(m));
                pks->setFittedPeak( idPk(m,j), cen, scen, hei, shei );
            }
        }
    }
}

bool Multiplets::getRange( int iniRange, int finRange, Doblst &x, Doblst &bg, Doblst &sBg )
{
    bool ret = true;
    x.clear();
    bg.clear();
    sBg.clear();
    if ( iniRange >= 0 ) {
        int nCh = finRange - iniRange + 1;
        Doblst ctsRange;
        Doblst sCtsRange;
        ret = ret && cts->getCountsRange( iniRange, nCh, ctsRange, sCtsRange );
        if ( ret )
            for ( int j = 0; j < nCh; j++ )
                if ( ctsRange[j] != 0 ) { // drop zero-count channels for the fit
                    x << qreal( j );
                    bg << qreal( ctsRange[j] );
                    sBg << qreal( sCtsRange[j] );
            }
    }
    else
        ret = false;
    return ret;
}

QList<Doblst> Multiplets::totalMultipletParms( int m )
{
    QList<Doblst> ret;
    Doblst parmsm = innerCubicBaseLines->at(m)->parms();
    ret << parmsm;

    for ( int j = 0; j < nPk(m); j++ ) {
        Doblst testy;
        testy = pks->getFittedPeak( fwhmCal, idPk(m,j) );
        ret << testy;
    }
    return ret;
}

void Multiplets::setMultipletFittedPeak( Doblst x, Doblst sigmax )
{
/*
    int n = x.count() / 2;
    for ( int i = 0; i < n; i++ ) {
        at(i)->setFittedCentroidAndHeight( x[i], x[n+i] );
    }
*/
}

void Multiplets::fitPeaksOfMultiplet( QString &rep )
{
/*
    Doblst t, xinit, y, sigmay, fixedparms;
    QList<int> partfixedparms;
    xinit << peaksCentroids() << peaksHeights();
    fixedparms << leftBaseLine->parms()
      << rghtBaseLine->parms()
      // << peaksKWidths()
    ;
    for (int i=0; i < nCh(); i++) {
        if ( cnt(i) != 0 ) { // drop zero-count channels for the fit
            t << qreal( iniChMu() + i );
            y << qreal( cnt(i) );
            sigmay << sqrt( qreal(cnt(i)) );
        }
    }

    int p = xinit.size();

    // 2009-07-30 AQUI colocando temporarily esse rep:

    rep = "";
    rep += QString(" \n \n \n *********** Inicio do fit do multipleto ***************");
    rep += QString("\n nCh: %1; p: %2; t.size: %3; xinit.size: %4; y.size: %5; sigmay: %6; fixedparms: %7.\n")
      .arg( nCh() ).arg( p ). arg(t.size()). arg(xinit.size()). arg(y.size()). arg(sigmay.size())
      .arg( fixedparms.size() );
    rep += "\n t: \n";
    for (int i=0; i<t.size(); i++) {
        rep += QString("%1: %2, ").arg(i).arg(t[i]);
    }
    rep += "\n xinit: \n";
    for (int i=0; i<xinit.size(); i++) {
            rep += QString("%1: %2, ").arg(i).arg(xinit[i]);
    }
    rep += "\n y: \n";
    for (int i=0; i<y.size(); i++) {
        rep += QString("%1: %2, ").arg(i).arg(y[i]);
    }
    rep += "\n sigmay: \n";
    for (int i=0; i<sigmay.size(); i++) {
        rep += QString("%1: %2, ").arg(i).arg(sigmay[i]);
    }
    rep += "\n fixedparms: \n";
    for (int i=0; i < fixedparms.size(); i++) {
        rep += QString("%1: %2, ").arg(i).arg(fixedparms[i]);
    }

    rep += "\n spline baseline: \n";
    for (int i=0; i < cubic().count(); i++) {
        rep += QString("%1: %2, ").arg(i).arg(cubic()[i]);
    }

    // partfixedparms[] = { # of parms BGleft, # of parms BGright, # of peaks (kwidths) }; the sum is nFixParms

    partfixedparms << leftBaseLine->parms().size() << rghtBaseLine->parms().size()
    //  << peaksKWidths().size() // 2010-02-24 AQUI reimplementarb
    ;
*/

// 2009-08-20 AQUI debugando
/*
    gslFit->genericNonlinearFunc(
      nCh(), p, t, xinit, y, sigmay, fixedparms, partfixedparms,
      gslFit->multiplet_f, gslFit->multiplet_df, gslFit->multiplet_fdf,
      rep );
*/
/*
    gslFit->genericNonlinearFunc(
      nCh(), p, t, xinit, y, sigmay, fixedparms, partfixedparms,
      gslFit->test_f, gslFit->test_df, gslFit->test_fdf,
      rep );

    int iter;
    Doblst x, sigmax;
    qreal chisqoverdeg;
    int status;

    gslFit->getResultsLevMar( iter, x, sigmax, chisqoverdeg, status );
    // 2009-08-25 AQUI setting new height and centroid
    setMultipletFittedPeak( x, sigmax );
*/
}

