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


#ifndef MULTIPLETS_H
#define MULTIPLETS_H

#include "peaklist.h"

class Multiplets : public QObject
{
    Q_OBJECT
public:
    Multiplets( PeakList *pklist, Counts *counts,
                GenericCalibration *fwhmCalibration, QObject *parent = 0 );
    ~Multiplets();

    int nMult() { return idpks->count(); }
    int nPk( int m ) { return idpks->at(m).count(); }
    int idPk( int m, int j ) { return idpks->at(m).at(j); }

    int iniLeft( int m ) { return iniChMuLst[m]; }
    int iniRight( int m ) { return iniChRightLst[m]; }
    int finLeft( int m) { return finChLeftLst[m]; }
    int nLeft( int m ) { return finChLeftLst[m] - iniChMuLst[m] + 1; }
    int nRight( int m ) { return finChMuLst[m] - iniChRightLst[m] + 1; }

    void setInvalid( int m ) { isValids[m] = false; }
    bool isValid( int m ) { return isValids[m]; }

    void clearAll();
    void resolve( qreal, qreal, int, int, int, int );
    //
    void fitPeaksOfMultiplet( QString &rep );
    void setBaselinePolyGrade( int gr ) { baselinePolyGrade = gr; }
    void fitBaseLines( int, Doblst, Doblst, Doblst, Doblst, Doblst, Doblst );
    void fitBaselineCubicSpline( qreal tP, qreal tQ, Doblst lBg, Doblst rBg );
    Doblst leftBL( int m ) { return leftBaseLines->at(m)->parms(); }
    Doblst rightBL( int m ) { return rightBaseLines->at(m)->parms(); }

    Doblst xLPts( int m ) { return leftBaseLines->at(m)->getXList(); }
    Doblst yLPts( int m ) { return leftBaseLines->at(m)->getYList(); }
    QString fitReportL( int m ) { return leftBaseLines->at(m)->getFitReport(); }
    Doblst xRPts( int m ) { return rightBaseLines->at(m)->getXList(); }
    Doblst yRPts( int m ) { return rightBaseLines->at(m)->getYList(); }
    QString fitReportR( int m ) { return rightBaseLines->at(m)->getFitReport(); }

    // Doblst innerCubicBL( int m ) { return innerCubicBaseLines->at(m)->parms(); }
    Doblst innerBL( int m ) { return innerCubicBaseLines->at(m)->parms(); }
    QList<Doblst> totalMultipletParms( int m );

    Doblst cubic() { return m_cubic; }
    // void setCubic( Doblst x ) { m_cubic = x; }
    void calcPointsForPlot( Doblst &xL, Doblst &yL, int nSubd );
        // 2009-08-25 - members after multiplet fit:
        // 2010-01-27 AQUI retomando.
    // Doblst peaksFittedHeights() { Doblst l; for (int i=0; i < size(); i++) l << at(i)->fittedHeight(); return l; }
    // Doblst peaksFittedCentrois() { Doblst l; for (int i=0; i < size(); i++) l << at(i)->fittedCentroid(); return l; }
    void setMultipletFittedPeak( Doblst x, Doblst sigmax );
private:
    void defineMultiplets( qreal maxNoFwhmSeparationInMult );
    void defineRegions( qreal noFwhmOfSideBaseline, qreal noFwhmOfPeak );
    void defineSideBaselines( int typeBaseline, int baselinePolyGrade );
    void defineInnerBaselines( int interpolationMethod, int typeBaseline );
    void calculateNetCounts();

    void peakHeightWeightInterpolation( int m );

    bool getRange( int iniRange, int finRange, Doblst &x, Doblst &bg, Doblst &sBg );
    double ( *blFitFunc ) ( Doblst, double );
    int baselinePolyGrade;

    void doMultipletFit();
    //

    PeakList *pks;
    Counts *cts;
    Counts *sCts;
    QList<Doblst> *netCountsRange;

    GenericCalibration *fwhmCal;

    QList<Intlst> *idpks;

    QList<GenericCalibration *> *leftBaseLines;
    QList<GenericCalibration *> *rightBaseLines;
    QList<GenericCalibration *> *innerCubicBaseLines;
    QList<GenericCalibration *> *netMultiplets;

    QList<bool> isValids;

    Intlst iniChMuLst;
    Intlst finChMuLst;
    Intlst finChLeftLst;
    Intlst iniChRightLst;

    Doblst m_cubic;
};

#endif // MULTIPLETS_H
