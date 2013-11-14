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

#include <QTextFrame>
#include <QTextTable>

#include "reports.h"
#include "functionsdefinitions.h"

QString Reports::reportCandidates( Counts *cts )
{
    QString r;
    r += QString(tr("(#1) Candidate peaks report:\n"));
    for (int i = 0; i < cts->ncands(); ++i) {
        r += QString(tr("#:%1 ; ch:%2.\n"))
          .arg( i )
          .arg( cts->cand(i) );
    }
    return r;
}

QString Reports::reportPeakWidths( PeakList *pklist, GenericCalibration *enCal )
{
    QString r;
    r += QString(tr("(#2) Peak widths report:\n"));
    QList<Doblst> testy;
    Doblst empty;
    testy << enCal->parms() << empty;
    for (int i=0; i < pklist->npks(); ++i) {


        qreal rA = pklist->centroid(i);
        qreal rB = pklist->fwhmCh(i);
        qreal rB1 = pklist->sfwhmCh(i);
        qreal rC = pklist->fittedEnergy( enCal, i );
        qreal rC1 = pklist->sFittedEnergy( enCal, i );
        qreal rD = pklist->fwhmEn( enCal, i );
        qreal rD1 = pklist->sFwhmEn( enCal, i);
        qreal rE = pklist->height(i);
        qreal rE1 = pklist->sheight(i);

        r += QString(tr("ch:%1 ; fwhmCh:%2 +- %3 ; energy:%4 +- %5 ; "
                        "fwhmEn:%6 +- %7; h:%8 +- %9.\n"))
          .arg( pklist->centroid(i) )
          .arg( pklist->fwhmCh(i)) .arg( pklist->sfwhmCh(i) )
          .arg( pklist->fittedEnergy( enCal, i ) ) .arg( pklist->sFittedEnergy( enCal, i ) )
          .arg( pklist->fwhmEn( enCal, i ) ).arg( pklist->sFwhmEn( enCal, i) )
          .arg( pklist->height(i) ).arg( pklist->sheight(i) );
    }
    r += tr("\n");
    return r;
}

QString Reports::reportPeakWidthsAndFit( PeakList *pklist, GenericCalibration *enCal,
                                   GenericCalibration *fwhmCal, int fwhmFitType )
{
    QString r;
    r += QString(tr("(#3) Peak widths report and fit:\n"));
    QList<Doblst> enAndFwhmPms;
    enAndFwhmPms << enCal->parms() << fwhmCal->parms();

    double (*currentYFunc) (QList<Doblst>, double);
    if ( fwhmFitType == 0 )
        currentYFunc = FunctionsDefinitions::fwhmInEnergySqrtT; // a0 + a1 * sqrt(En)
    if ( fwhmFitType == 1 )
        currentYFunc = FunctionsDefinitions::fwhmInEnergyPoly; // a0 + a1*En + a2*En^2

    for (int i=0; i < pklist->npks(); ++i) {
        r += QString(tr("ch:%1 ; fwhmCh:%2 +- %3 ; "
                        "fittedFwhm:%4; fittedK:%5;\n"
                        "               fwhmEn:%6 +- %7; fittedFwhmEn: %8.\n"))
          .arg( pklist->centroid(i) )
          .arg( pklist->fwhmCh(i)) .arg( pklist->sfwhmCh(i) )
          .arg( pklist->fittedFwhm( fwhmCal, i ) )
          .arg( pklist->fittedK( fwhmCal, i) )
          .arg( pklist->fwhmEn( enCal, i ) ).arg( pklist->sFwhmEn( enCal, i) )
          .arg( currentYFunc( enAndFwhmPms, pklist->centroid(i) )  );
    }
    r += tr("\n");
    return r;
}

QString Reports::reportRegions( Multiplets *multiplets )
{
    QString r;
    r += QString(tr("(#4) Regions report:\n"));
    for (int m=0; m < multiplets->nMult(); ++m) {
        r += QString(tr("Multiplet #%1:\n")).arg( m );
        r += QString(tr("Left: %1, n=%2. Right: %3, n=%4.\n"))
             .arg( multiplets->iniLeft( m ) )
             .arg( multiplets->nLeft( m ) )
             .arg( multiplets->iniRight( m ) )
             .arg( multiplets->nRight( m ) );
    }
    r += tr("\n");
    return r;
}

QString Reports::reportBaseLines( Multiplets *multiplets )
{
    QString r;
    r += QString(tr("(#5) Base lines report:\n"));
    for (int m=0; m < multiplets->nMult(); ++m) {
        r += QString(tr("Multiplet #%1:\n")).arg( m );
        for ( int i=0; i < multiplets->xLPts(m).count(); i++ )
            r += QString(tr("%1: %2; ")
             .arg( multiplets->xLPts(m)[i] )
             .arg( multiplets->yLPts(m)[i] ));
        r += tr("\n");
        for ( int i=0; i < multiplets->xRPts(m).count(); i++ )
            r += QString(tr("%1: %2; ")
             .arg( multiplets->xRPts(m)[i] )
             .arg( multiplets->yRPts(m)[i] ));
        r += tr("\n");
    }
    return r;
}

QString Reports::reportBaseLinesFit( Multiplets *multiplets )
{
    QString r;
    r += QString(tr("(#6) Base lines fits:\n"));
    for (int m=0; m < multiplets->nMult(); ++m) {
        r += QString(tr("Multiplet #%1:\n")).arg( m );
        for ( int i=0; i < multiplets->xLPts(m).count(); i++ )
            r += multiplets->fitReportL( m );
        r += tr("\n");
        for ( int i=0; i < multiplets->xLPts(m).count(); i++ )
            r += multiplets->fitReportR( m );
        r += tr("\n");
        r += tr("\n");
    }
    return r;
}
