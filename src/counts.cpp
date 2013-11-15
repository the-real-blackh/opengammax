/***************************************************************************
 *   Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com
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


#include <qmath.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_gamma.h>
#include "doblst.h"
#include "counts.h"


Counts::Counts( Intlst *ilst )
{
    cntList.clear();
    for (int i = 0; i < ilst->size(); ++i)
        cntList << ilst->at(i);
    sigmaCounts.clear();
    for (int i = 0; i < ilst->size(); i++)
        sigmaCounts << sqrt( qreal(cntList[i]) );
}

bool Counts::clear( )
{
    cntList.clear();
    sigmaCounts.clear();
    return true;
}

void Counts::updateCounts( Intlst & ilist )
{
    cntList.clear();
    for (int i = 0; i < ilist.size(); ++i)
        cntList << ilist.at(i);
    sigmaCounts.clear();
    for (int i = 0; i < ilist.size(); i++)
        sigmaCounts << sqrt( qreal(cntList[i]) );
}


void Counts::setSecDiffAndSigma( int nSmoothCh )
{
    Intlst binom;
    Doblst smoo;
    secDiff.clear();
    sigmaSecDiff.clear();
    smoo.clear();
    doBinom(2 * nSmoothCh, binom );
    doSmoothed( nSmoothCh, cntList, binom, smoo );
    secDiff << 0.0;
    for (int i = 1; i < smoo.size()-1; i++)
        secDiff << -2*smoo[i] + smoo[i-1] + smoo[i+1];
    secDiff << 0.0;
    sigmaSecDiff << 0.0;
    for (int i = 1; i < smoo.size()-1; i++)
        sigmaSecDiff << sqrt( smoo[i-1] + smoo[i+1] + 4*smoo[i] );
    sigmaSecDiff << 0.0;
}

bool Counts::getCountsRange(int i, int n, Doblst &ctsRange, Doblst &sCtsRange)
{
    int sz = GSL_MIN( cntList.count(), sigmaCounts.count() );
    if ( i+n >= sz ) return false;
    for ( int j = i; j < i+n; j++ ) {
        ctsRange << qreal( cntList [j] );
        sCtsRange << sigmaCounts [j];
    }
    return true;
}

bool Counts::getSecDiffRange(int cand, int nSdCh, Doblst &secDiffRange, Doblst &sSecDiffRange)
{
    int sz = GSL_MIN( secDiff.count(), sigmaSecDiff.count() );
    if ( cand + nSdCh >= sz ) return false;
    for ( int i = cand - nSdCh; i <= cand + nSdCh; i++ ) {
        secDiffRange << secDiff [i];
        sSecDiffRange << sigmaSecDiff [i];
    }
    return true;
}


int Counts::findCandidates( qreal pkCut )
{
    qreal criterion;
    int npk = 0;
    candidates.clear();
    for (int i = 10; i < nchs()-10; i++)
    {
        criterion = -pkCut * sigmaSecDiff[i];  // criterion to accept a candidate peak;
                                             //  pkcut is user-defined
        if (secDiff[i] < criterion)    // if current channel is in a peak,
            if ( (secDiff[i-1] > secDiff[i]) && (secDiff[i+1] > secDiff[i]) ) // if secDiff[i] is a local minimum,
                {
                    npk++;    // then channel i is a candidate peak
                    candidates << i;
                }
    }
    return npk;
}

void Counts::doBinom(int i, Intlst &binom)
// entrada: i
// saida: membro binom[0:i] contendo os binomiais (i 0), (i 1), ..., (i i).
{
    binom.clear();
    for (int j=0; j <= i; j++)
        binom << int(gsl_sf_choose (i, j));
}

void Counts::doSmoothed( int n, Intlst counts, Intlst binom, Doblst &smoothed )
{
    smoothed.clear();
    if (n == 0)
            for ( int i = 0; i < counts.size(); i++ )
              smoothed << qreal(counts[i]);
    else
    {
            int two_n = 2 * n;
            qreal f = pow( 2, two_n );
            for ( int i = 0; i < n; i++ )
                smoothed << 0.0;
            for ( int i = n; i < counts.size() - n; i++ ) {
                int s = 0;
                for ( int j = 0; j <= two_n; j++ )
                s += binom[j] * counts[i+j-n];
                smoothed << s / f;
            }
            while ( smoothed.size() < counts.size())
                smoothed << 0.0;
    }
}
