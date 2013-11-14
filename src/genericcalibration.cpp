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

#include "genericcalibration.h"
#include "gslcontainer.h"

GenericCalibration::GenericCalibration( qreal (*pointf) (Doblst, qreal),
                                        QObject *parent )
    : QObject( parent )
{
    typeParms = 0;   // uses f, not f2
    f = pointf;
}

GenericCalibration::GenericCalibration( qreal (*pointf) (QList<Doblst>, qreal),
                                        QObject *parent )
    : QObject( parent )
{
    typeParms = 1;   // uses f2, not f
    f2 = pointf;
}

GenericCalibration::GenericCalibration( Doblst a, qreal (*pointf) (Doblst, qreal), QObject *parent )
    : QObject( parent )
{
    typeParms = 0;   // uses f, not f2
    setParms( a );
    f = pointf;
}

bool GenericCalibration::doCalibPolyN( int grade, Doblst x, Doblst y, Doblst s )
{
    bool ret = false;
    int n = GSL_MIN( GSL_MIN( x.size(), y.size() ), s.size() );
    polyGrade = grade;
    int p = polyGrade + 1;
    if ( ( n >= 2 ) && ( p <= n ) ) {
        f = FunctionsDefinitions::poly;
        setCalibDataPoints( x, y, s );
        ret = doCalib( x, y, x );
    }
    return ret;
}

bool GenericCalibration::doCalibSqrtEn( Doblst x, Doblst y, Doblst s )
{
    bool ret = false;
    int n = GSL_MIN( GSL_MIN( x.size(), y.size() ), s.size() );
    if ( n >= 2 ) {
        f = FunctionsDefinitions::sqrtT;
        setCalibDataPoints( x, y, s );
        ret = doCalib( x, y, x );
    }
    return ret;
}

bool GenericCalibration::doCalibExpPlusConst( Doblst x, Doblst y, Doblst s )
{
    bool ret = false;
    int n = GSL_MIN( GSL_MIN( x.size(), y.size() ), s.size() );
    if ( n >= 3 ) {
        f = FunctionsDefinitions::expPlusConst;
        setCalibDataPoints( x, y, s );
        ret = doCalib( x, y, x );
    }
    return ret;
}

void GenericCalibration::setCalibDataPoints( Doblst x, Doblst y, Doblst s  )
{
    xList = Doblst( x );
    yList = Doblst( y );
    sList = Doblst( s );
}

Doblst GenericCalibration::parms()
{
    return fitParms;
}

void GenericCalibration::setParms( Doblst a )
{
    fitParms.clear();
    for (int i=0; i < a.size(); i++)
        fitParms << a[i];
}

void GenericCalibration::setFixedParms( Doblst a )
{
    fixedParms.clear();
    for (int i=0; i < a.size(); i++)
        fixedParms << a[i];
}

void GenericCalibration::setInitVals( Doblst a )
{
    initVals.clear();
    for (int i=0; i < a.size(); i++)
        initVals << a[i];
}

void GenericCalibration::clearParms()
{
    fitParms.clear();
}

qreal GenericCalibration::y( qreal x )

{
    if ( nParms() > 0 ) {
        return f( fitParms, x );
    }
    else
        return GSL_NAN;
}

bool GenericCalibration::doCalib( Doblst x, Doblst y, Doblst s )
{
    setCalibDataPoints( x, y, s );
    bool ret = true;
    // lin or non-lin:
    fitParms.clear();
    cq = 0.0;
    // lin only:
    cov.clear();
        // non lin only:

    n = GSL_MIN( GSL_MIN( xList.count(), yList.count() ), sList.count() );
    if (n >= 3) {
        if ( typeParms == 0 ) {
            ret = ret && calibTypeF();
        }
        if ( typeParms == 1 ) {
            ret = ret && calibTypeF2();
        }
        if ( typeParms > 1 ) {
            ret = false;
        }
    }
    else {
        ret = false;
    }
    return ret;
}

void GenericCalibration::setDefaultCalibration()
{
    Doblst defaultParms;
    f = FunctionsDefinitions::poly;
    defaultParms << 0.0 << 1.0;
    setParms( defaultParms );
}

bool GenericCalibration::calibTypeF()
{
    bool ret =  false;
    GslContainer * gslctn = new GslContainer( this );
    if ( f == FunctionsDefinitions::poly ) {
        gslctn->execLinearFit( n, polyGrade + 1, xList, yList, sList, fitParms, cov, cq );
        ret = true;
    }
    else if ( f == FunctionsDefinitions::sqrtT ) {
        Doblst xLinList;
        for ( int i=0; i < xList.count(); i++) {
            xLinList << sqrt( xList[i] );
        }
        gslctn->execLinearFit( n, 2, xLinList, yList, sList, fitParms, cov, cq );
        ret = true;
    }
    else if ( f == FunctionsDefinitions::expSimple ) {
        Doblst xLinList;
        Doblst yLinList;
        for ( int i=0; i < yList.count(); i++) {
            if ( yList[i] > 0.001 ) {
                xLinList << xList;
                yLinList << log( yList[i] );
            }
        }
        gslctn->execLinearFit( n, 2, xLinList, yLinList, sList, fitParms, cov, cq );
        ret = true;
    }
    else if ( f == FunctionsDefinitions::expPlusConst ) {
        Doblst xInit;
        xInit << yList[0] << -0.01 << 0.0;
        Doblst fixparms;
        Intlst partfixparms;
        gslctn->genericNonlinearFunc2(
            n, 3, xList, xInit, yList, sList,
            gslctn->expb_f, gslctn->expb_df, gslctn->expb_fdf,
            fitReport,
            fixparms, partfixparms );
        // non-lin only:
        int it;
        qreal csod;
        int status;
        gslctn->getResultsLevMar( it, fitParms, sFitParms, csod, status );
        cq = csod * ( n-3 );
        ret = true;
    }
    return ret;
}

bool GenericCalibration::calibTypeF2()
{
    GslContainer * gslctn = new GslContainer( this );
    bool ret = false;
    if ( f2 == FunctionsDefinitions::gaussiansSum ) {
        // Doblst xInit;
        // height, k-width, centroid

        // fixedParms are the k-widths, already passed to <this>
        Intlst partfixparms;
        partfixparms << nFiPa();

        int p = 2 * nFiPa();
        gslctn->genericNonlinearFunc2(
            n, p, xList, initVals, yList, sList,
            gslctn->gaussSum_f, gslctn->gaussSum_df, gslctn->gaussSum_fdf,
            fitReport,
            fixedParms, partfixparms );
        // non-lin only:
        int it;
        qreal csod;
        int status;
        gslctn->getResultsLevMar( it, fitParms, sFitParms, csod, status );
        cq = csod * ( n-3 );
        ret = true;
    }
    return ret;
}
