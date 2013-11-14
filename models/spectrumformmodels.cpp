/***************************************************************************
*  Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com                                                    *
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

#include "spectrumformmodels.h"

// raw data ( = counts per channel )

SpectrumFormModels::SpectrumFormModels( QObject *parent ) :
    QObject( parent ) {}

void SpectrumFormModels::setupRawDataMdl()
{
    rawDataMdl = new QStandardItemModel( this );
    rawDataMdl->setColumnCount( 10 );
    QStringList horzlb;
    for ( int uni = 0; uni < rawDataMdl->columnCount(); uni++ )
        horzlb << QString("%1").arg( uni );
    rawDataMdl->setHorizontalHeaderLabels( horzlb );
}

void SpectrumFormModels::fillRawDataMdl( Intlst & ilist )
{
    rawDataMdl->setRowCount( ( ilist.count() -1 ) / 10  + 1 );
    QStringList vertlb;
    for ( int dec = 0; dec < rawDataMdl->rowCount(); dec++ )
        vertlb << QString("%1 - %2").arg( 10*dec ).arg( 10*dec+9 );
    rawDataMdl->setVerticalHeaderLabels( vertlb );
    for (int i = 0; i < ilist.count(); ++i) {
        QModelIndex index;
        index = rawDataMdl->index( i/10, i%10, QModelIndex() );
        rawDataMdl->setData( index, QVariant( ilist[i] ) );
    }
}

// energy points:

void SpectrumFormModels::setupEnergypointsMdl()
{
    energypointsMdl = new QStandardItemModel( this );
    energypointsMdl->setColumnCount( 6 );
    QStringList horizHead;
    horizHead << tr("Channel") << tr("Energy (keV)")
        << tr("Lib. Energy (keV)") << tr("Nuclide")
        << tr("FWHM (En)") << tr("s(FWHM) (En)");
    energypointsMdl->setHorizontalHeaderLabels( horizHead );
}

void SpectrumFormModels::fillEnergypointsMdl(
        PeakList *pklist,
        GenericCalibration *enCal,
        GenericCalibration *fwhmCal,
        GammaList *gammaList,
        NuclideList *nuclideList,
        qreal toler )
{
    int nro = ( pklist->npks() );
    QString matchEns;
    // column 0:

    energypointsMdl->clear();
    energypointsMdl->setRowCount( nro );

    for (int i=0; i < nro; i++) {
        QString qstr = QString("%1").arg( pklist->centroid(i) );
        energypointsMdl->setItem( i, 0, new QStandardItem(
            QString("%1").arg( pklist->centroid(i) ) ) );
    }

    // columns 1,2,3:
    for (int i=0; i < nro; i++) {
        energypointsMdl->setItem( i, 1, new QStandardItem(
            QString("%1").arg(pklist->fittedEnergy( enCal, i) ) ) );
        Qt::CheckState state;
        bool isForCalib = pklist->isUsedForCalibration(i);
        if ( isForCalib ) state = Qt::Checked; else state = Qt::Unchecked;
        energypointsMdl->item( i, 1 )->setCheckState( state );

        matchEns = gammaList->matchEnergiesStr( pklist->fittedEnergy( enCal, i), toler );
        if ( matchEns.count() != 0 ) {
            QColor cl;
            cl.setNamedColor("navajowhite"); energypointsMdl->item(i, 1)->setBackground( cl );
            energypointsMdl->setItem(i, 2, new QStandardItem( QString( matchEns ) ) );
            int howMany;
            QString matchNuclides = nuclideList->matchNucs( pklist->fittedEnergy( enCal, i ), toler, gammaList, howMany );
            energypointsMdl->setItem(i, 3, new QStandardItem( QString( matchNuclides ) ) );
            cl.setNamedColor("springgreen");
            energypointsMdl->item(i, 2)->setBackground( cl );
            energypointsMdl->item(i, 3)->setBackground( cl );
        }

    }
    // column 4:
    for (int i=0; i < pklist->npks(); i++) {
        QStandardItem *item = new QStandardItem( QString("%1").arg(pklist->fwhmEn( enCal, i)) );
        energypointsMdl->setItem( i, 4, item );
    }
    // column 5:
    for (int i=0; i < pklist->npks(); i++) {
        QStandardItem *item = new QStandardItem( QString("%1").arg(pklist->sFwhmEn( enCal, i)) );
        energypointsMdl->setItem( i, 5, item );
    }
}

// energy vs. channel parameters:

void SpectrumFormModels::setupEnxchanMdl()
{
    enxchanMdl = new QStandardItemModel( this );
    enxchanMdl->setColumnCount( 1 );
    QStringList horizHead;
    horizHead << tr("Parameters:");
    enxchanMdl->setHorizontalHeaderLabels( horizHead );
}

void SpectrumFormModels::fillEnxchanMdl( Doblst enParms )
{
    enxchanMdl->setRowCount( enParms.count() );
    for (int i=0; i < enParms.count(); i++) {
        QModelIndex index;
        index = enxchanMdl->index(i, 0, QModelIndex() );
        enxchanMdl->setData( index, QVariant( enParms[i] ) );
    }
}

// fwhm vs. energy parameters:

void SpectrumFormModels::setupFwhmxchMdl()
{
    fwhmxchMdl = new QStandardItemModel( this );
    fwhmxchMdl->setColumnCount( 1 );
    QStringList horizHead;
    horizHead << tr("Parameters:");
    fwhmxchMdl->setHorizontalHeaderLabels( horizHead );
}

void SpectrumFormModels::fillFwhmxchMdl( Doblst fwhmParms )
{
    fwhmxchMdl->setRowCount( fwhmParms.count() );
    for (int i=0; i < fwhmParms.count(); i++) {
        QModelIndex index;
        index = fwhmxchMdl->index(i, 0, QModelIndex() );
        fwhmxchMdl->setData( index, QVariant( fwhmParms[i] ) );
    }
}

// Efficiency fit:

void SpectrumFormModels::setupEfficPtsMdl()
{
    efficPtsMdl = new QStandardItemModel( this );
    efficPtsMdl->setColumnCount( 3 );
    QStringList horizHead;
    horizHead << tr("Energy (keV)") << tr("Efficiency (%)") << tr("Uncert.(%)");
    efficPtsMdl->setHorizontalHeaderLabels( horizHead );
}

void SpectrumFormModels::setupeffxenMdl()
{
    effxenMdl = new QStandardItemModel( this );
    effxenMdl->setColumnCount( 1 );
    QStringList horizHead;
    horizHead << tr("Parameters:");
    effxenMdl->setHorizontalHeaderLabels( horizHead );
}

void SpectrumFormModels::fillEfficPtsMdl( QStringList zzz )
{
    int nRo = zzz.count() / 3;
    efficPtsMdl->setRowCount( nRo );
    for (int i=0; i < nRo; i++) {
        for (int j=0; j < 3; j++) {
            QStandardItem *item = new QStandardItem( zzz.at(3*i+j) );
            efficPtsMdl->setItem(i, j, item);
        }
    }
}

void SpectrumFormModels::filleffxenMdl( Doblst effParms )
{
    effxenMdl->setRowCount( effParms.count() );
    for (int i=0; i < effParms.count(); i++) {
        QModelIndex index;
        index = effxenMdl->index(i, 0, QModelIndex() );
        effxenMdl->setData( index, QVariant( effParms[i] ) );
    }
}

// Nuclides/gammas data:

void SpectrumFormModels::setupNuclideTabMdl()
{
    nuclideTabMdl = new QStandardItemModel( this );
    nuclideTabMdl->setColumnCount( 4 );
    QStringList horizHead;
    horizHead << tr("ID Nucl.") << tr("Name") << tr("Half life (value)") << tr("Half life (unit)");
    nuclideTabMdl->setHorizontalHeaderLabels( horizHead );
}

void SpectrumFormModels::setupGammaTabMdl()
{
    gammaTabMdl = new QStandardItemModel( this );
    gammaTabMdl->setColumnCount( 6 );
    QStringList horizHead;
    horizHead << tr("ID Gamma") << tr("ID Nucl.") << tr("Energy (keV)") << tr("Energy (unc. digits)")
      << tr("I(%)") << tr("I(unc. digits)");
    gammaTabMdl->setHorizontalHeaderLabels( horizHead );
}

void SpectrumFormModels::setupGammasPerNuclideMdl()
{
    gammasPerNuclideMdl = new QStandardItemModel( this );
    gammasPerNuclideMdl->setColumnCount( 8 );
    QStringList horizHead;
    horizHead << tr("ID Gama") << tr("Nuclide") << tr("Energy (keV)") << tr("Energy (dígit. incert.)")
      << tr("I(%)") << tr("I(dígit. incert.)") << tr("half-life") << tr("units");
    gammasPerNuclideMdl->setHorizontalHeaderLabels( horizHead );
}

void SpectrumFormModels::fillGammasPerNuclideMdl()
{
    gammasPerNuclideMdl->setRowCount( gammaTabMdl->rowCount() );
    for (int i=0; i < gammasPerNuclideMdl->rowCount(); i++) {
        QString idNucl = gammaTabMdl->item(i, 1)->text();
        int rowOfNuclide = nuclideTabMdl->findItems( idNucl, 0 ).first()->row();
        for (int j=0; j < gammasPerNuclideMdl->columnCount(); j++) {
            QModelIndex index = gammasPerNuclideMdl->index(i, j, QModelIndex() );
            if ( j == 1 ) {
                if ( rowOfNuclide >= 0 )
                    gammasPerNuclideMdl->setData( index, nuclideTabMdl->item( rowOfNuclide, 1 )->text() );
            }
            else if ( j== 2) {
                gammasPerNuclideMdl->setData( index, gammaTabMdl->item(i, j)->text().toDouble() );
            }
            else if ( j == 6) {
                if ( rowOfNuclide >= 0 )
                    gammasPerNuclideMdl->setData( index, nuclideTabMdl->item( rowOfNuclide, 2 )->text().toDouble() );
            }
            else if ( j == 7) {
                if ( rowOfNuclide >= 0 )
                    gammasPerNuclideMdl->setData( index, nuclideTabMdl->item( rowOfNuclide, 3 )->text() );
            }
            else
                gammasPerNuclideMdl->setData( index, gammaTabMdl->item(i, j)->text() );
        }
    }
}
//

void SpectrumFormModels::fillStdItemModel( QStandardItemModel *model, const QString &contents )
{
    QStringList list = contents.split("\n", QString::SkipEmptyParts );
    model->setRowCount( list.count() );
    for (int i=0; i < model->rowCount(); i++) {
        QStringList pieces = list.at(i).split(";", QString::SkipEmptyParts);
        QModelIndex index;
        for (int j=0; j < model->columnCount(); j++) {
            index = model->index(i, j, QModelIndex() ); model->setData( index, QVariant( pieces.value(j) ) );
        }
    }
}

void SpectrumFormModels::setupPeaksFitMdl()
{
    peaksFitMdl = new QStandardItemModel( this );
    peaksFitMdl->setColumnCount( 23 );
    QStringList horizHead;
    horizHead << tr("ch.") << tr("fit ch.") << tr("unc.")
            << tr("energy (keV)")
            << tr("FWHM (ch)") << tr("unc.")
            << tr("fwhm (keV)") << tr("unc.")
            << tr("gr. h") << tr("unc.")
            << tr("fit. h") << tr("unc.")
            << tr("net area") << tr("unc.")
            << tr("En. encontr.")
            << tr("Nuclide")
            << tr("half-life")
            << tr("units")
            << tr("I%") << tr("Effic")
            << tr("Activ. (Bq)") << tr("unc.") << tr("idNucl")
            ;
    peaksFitMdl->setHorizontalHeaderLabels( horizHead );
}

void SpectrumFormModels::fillPeaksFitMdl(
        PeakList *pkl,
        GenericCalibration *enCal,
        GenericCalibration *fwhmCal,
        GenericCalibration *effCal,
        GammaList *gammaList,
        NuclideList *nuclideList,
        qreal toler,
        qreal lt,
        qreal delayInDays)
{
    peaksFitMdl->setRowCount( pkl->npks() );
    for (int i=0; i < pkl->npks(); i++) {
        QModelIndex ndx;
        ndx = peaksFitMdl->index(i, 0, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->centroid(i) ) );
        ndx = peaksFitMdl->index(i, 1, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->fittedCentroid(i) ) );
        ndx = peaksFitMdl->index(i, 2, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->sfittedCentroid(i) ) );
        ndx = peaksFitMdl->index(i, 3, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->fittedEnergy( enCal, i) ) );
        ndx = peaksFitMdl->index(i, 4, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->fwhmCh(i) ) );
        ndx = peaksFitMdl->index(i, 5, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->sfwhmCh(i) ) );
        ndx = peaksFitMdl->index(i, 6, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->fwhmEn( enCal, i ) ) );
        ndx = peaksFitMdl->index(i, 7, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->sFwhmEn( enCal, i) ) );

        ndx = peaksFitMdl->index(i, 8, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->height(i) ) );
        ndx = peaksFitMdl->index(i, 9, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->sheight(i) ) );
        ndx = peaksFitMdl->index(i, 10, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->fittedHeight(i) ) );
        ndx = peaksFitMdl->index(i, 11, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->sfittedHeight(i) ) );

        qreal uncRel = pkl->sfittedHeight(i) / pkl->fittedHeight(i);
        qreal netAr = pkl->netArea( fwhmCal, i);
        ndx = peaksFitMdl->index(i, 12, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( netAr ) );
        ndx = peaksFitMdl->index(i, 13, QModelIndex() ); peaksFitMdl->setData( ndx, QVariant( pkl->sNetArea( fwhmCal, i) ) );

        QString matchEns;
        matchEns = gammaList->matchEnergiesStr(  pkl->fittedEnergy( enCal, i), toler );
        int idNucl = -1;
        if ( matchEns.count() != 0 ) {
            QColor cl;
            cl.setNamedColor("navajowhite"); peaksFitMdl->item(i, 3)->setBackground( cl );

            peaksFitMdl->setItem(i, 14, new QStandardItem( QString( matchEns ) ) );
            int howMany;
            QString matchNuclides = nuclideList->matchNucs(  pkl->fittedEnergy( enCal, i ), toler, gammaList, howMany );
            peaksFitMdl->setItem(i, 15, new QStandardItem( QString("%1").arg( matchNuclides ) ) );

            if ( howMany == 1 ) {
                idNucl = gammaList->matchIdnucls( pkl->fittedEnergy( enCal, i ), toler ).at(0);
                qreal matchT12 = nuclideList->halfLifeByIdnucl( idNucl );
                QString matchT12unit = nuclideList->halfLifeUnitByIdnucl( idNucl );

                // ********* CALCULO DAS ATIVIDADES PARCIAIS **************
                qreal mtchIPer = gammaList->matchIPerc( pkl->fittedEnergy( enCal, i ), toler ).at(0);
                qreal effic = exp( effCal->y( log( pkl->fittedEnergy( enCal, i )) ) ); // QQQ pohr a energia tabelada
                qreal ativ = netAr / ( (0.01*mtchIPer) * (0.01*effic) * 0.95 * lt );
                qreal sAtiv = ativ * uncRel;
                qreal decayFact = exp( log(2.0) * ( delayInDays / 365.25 ) / matchT12 );
                ativ *= decayFact;
                sAtiv *= decayFact;

                cl.setNamedColor("springgreen");
                peaksFitMdl->item(i, 14)->setBackground( cl );
                peaksFitMdl->item(i, 15)->setBackground( cl );

                peaksFitMdl->setItem(i, 16, new QStandardItem( QString("%1").arg( matchT12 ) ) );
                peaksFitMdl->setItem(i, 17, new QStandardItem( matchT12unit ) );
                peaksFitMdl->setItem(i, 18, new QStandardItem( QString("%1").arg( mtchIPer ) ) );
                peaksFitMdl->setItem(i, 19, new QStandardItem( QString("%1").arg( effic ) ) );
                peaksFitMdl->setItem(i, 20, new QStandardItem( QString("%1").arg( ativ ) ) );
                peaksFitMdl->setItem(i, 21, new QStandardItem( QString("%1").arg( sAtiv) ) );
            }
        }
        peaksFitMdl->setItem(i, 22, new QStandardItem( QString("%1").arg( idNucl) ) );
    }
}

void SpectrumFormModels::setupActivitiesMdl()
{
    activitiesMdl = new QStandardItemModel( this );
    activitiesMdl->setColumnCount( 3 );
    QStringList horizHead;
    horizHead << tr("Nuclide") << tr("Activity (Bq)") << tr("incert.");
    activitiesMdl->setHorizontalHeaderLabels( horizHead );
}

Intlst SpectrumFormModels::positions( Intlst lst, int p )
{
    Intlst ret;
    int iniSrch = 0;
    while ( lst.indexOf( p, iniSrch ) != -1 ) {
        ret << lst.indexOf( p, iniSrch );
        iniSrch = ret.last() + 1;
    }
    return ret;
}

Doblst SpectrumFormModels::newListByPosits( Doblst lst, Intlst pos )
{
    Doblst ret;
    for (int i=0; i < pos.count(); i++ )
        ret << lst[ pos[i] ];
    return ret;
}

void SpectrumFormModels::fillActivitiesMdl(
  PeakList * pkl, GenericCalibration *enCal, GenericCalibration *fwhmCal, qreal qty,
  const QString &unit )
{
    Intlst idsOfPeaks;
    QStringList matchNucs;
    Doblst ativPart;
    Doblst sAtivPart;
    for (int i=0; i < pkl->npks(); i++) {
        int idnu = peaksFitMdl->item(i, 21)->text().toInt();
        if ( idnu >= 0 ) { // if line is univoquely identified
            idsOfPeaks << idnu;
            matchNucs << peaksFitMdl->item(i, 15)->text();
            ativPart << peaksFitMdl->item(i, 20)->text().toDouble();
            sAtivPart << peaksFitMdl->item(i, 21)->text().toDouble();
        }
    }
    int nIdPeaks = idsOfPeaks.count();
    int minId = 30000;
    int maxId = -1;
    for (int i=0; i < nIdPeaks; i++) {
        minId = qMin( minId, idsOfPeaks[i] );
        maxId = qMax( maxId, idsOfPeaks[i] );
    }
    Intlst uniqueNucId;
    QStringList uniqueMatchNucs;
    Doblst actsMeans;
    Doblst actsUncerts;
    for (int id = minId; id <= maxId; id++) {
        Intlst posits = positions( idsOfPeaks, id );
        if ( !posits.isEmpty() ) {
            uniqueNucId << id;
            uniqueMatchNucs << matchNucs[ posits[0] ];
            Doblst actsPerNuc;
            Doblst sActsPerNuc;
            actsPerNuc = newListByPosits( ativPart, posits );
            sActsPerNuc = newListByPosits( sAtivPart, posits );

            // actsMeans << actsPerNuc.mean();
            actsMeans << weiMean( actsPerNuc, sActsPerNuc );

            // actsUncerts << actsPerNuc.mean() * 0.1; // qqq
            actsUncerts << weiUnc( sActsPerNuc );
        }
    }

    QStringList horizHead;
    QString unidResul;
    unidResul = "Bq/" + unit;
    horizHead << tr("Nuclide") << unidResul << tr("incert.");
    activitiesMdl->setHorizontalHeaderLabels( horizHead );


    int nRow = uniqueNucId.count();
    activitiesMdl->setRowCount( nRow );

    for (int i=0; i < nRow; i++) {
        QStandardItem *it0 = new QStandardItem( uniqueMatchNucs[i] );
        QStandardItem *it1 = new QStandardItem(QString("%1").arg( actsMeans[i] / qty ));
        QStandardItem *it2 = new QStandardItem(QString("%1").arg( actsUncerts[i] / qty ));
        activitiesMdl->setItem( i, 0, it0 );
        activitiesMdl->setItem( i, 1, it1 );
        activitiesMdl->setItem( i, 2, it2 );
    }
}

void SpectrumFormModels::setupPartialActivitiesMdl()
{
    activitiesMdl = new QStandardItemModel( this );
    activitiesMdl->setColumnCount( 5 );
    QStringList horizHead;
    horizHead << tr("Match En") << tr("I%") << tr("Efic.") << tr("Activ. (Bq)")<< tr("unc.");
    activitiesMdl->setHorizontalHeaderLabels( horizHead );
}

void SpectrumFormModels::changeNEfficPts( int nPt )
{
    efficPtsMdl->setRowCount( nPt );
}

qreal SpectrumFormModels::weiMean( Doblst vals, Doblst uncs )
{
    qreal x = 0.0;
    int nVals = vals.size();
    int nUncs = uncs.size();
    if ( ( nVals == nUncs ) && ( nVals > 0 ) ) {
        qreal sumA = 0.0;
        qreal sumB = 0.0;
        for (int i=0; i < nVals; i++) {
            qreal invVar = 1.0 / ( uncs[i] * uncs[i] );
            sumA += vals[i] * invVar;
            sumB += invVar;
        }
        x = sumA / sumB;
    }
    return x;
}

qreal SpectrumFormModels::weiUnc( Doblst uncs )
{
    qreal x = 0.0;
    int nUncs = uncs.size();
    if ( nUncs > 0 ) {
        qreal sumB = 0.0;
        for (int i=0; i < nUncs; i++) {
            qreal invVar = 1.0 / ( uncs[i] * uncs[i] );
            sumB += invVar;
        }
        x = pow( sumB, -0.5 );
    }
    return x;
}

