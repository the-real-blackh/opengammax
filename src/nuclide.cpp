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



#include "nuclide.h"

Nuclide::Nuclide( QStringList pieces, QObject *parent  ) :
        QObject( parent )
{
    midnucl = pieces[0].toInt();
    mname = pieces[1];
    mhalflife = pieces[2].toDouble();
    munit = pieces[3];
}

QStringList NuclideList::namesList( QList<int> idnuclList )
{
    QStringList ret;
    for (int i=0; i < idnuclList.count(); i++ ) {
        int idnucl = idnuclList[i];
        QString nameNucl = nameByIdnucl( idnucl );
        ret << nameNucl + " * ";
    }
    return ret;
}

QString NuclideList::nameByIdnucl( int id )
{
    QString ret;
    for (int i=0; i < count(); i++)
        if ( at(i)->idnucl() == id ) {
            ret = at(i)->name();
            break;
        }
    return ret;
}

qreal NuclideList::halfLifeByIdnucl( int id )
{
    qreal ret;
    for (int i=0; i < count(); i++)
        if ( at(i)->idnucl() == id ) {
            ret = at(i)->halflife();
            break;
        }
    return ret;
}

QString NuclideList::halfLifeUnitByIdnucl( int id )
{
    QString ret;
    for (int i=0; i < count(); i++)
        if ( at(i)->idnucl() == id ) {
            ret = at(i)->unit();
            break;
        }
    return ret;
}

GammaTransition::GammaTransition( QStringList pieces, QObject *parent ) :
        QObject( parent )
{
    midgamma = pieces[0].toInt();
    midnucl = pieces[1].toInt();
    menergyval = pieces[2].toDouble();
    menergyunc = pieces[3].toInt();
    mipercval = pieces[4].toDouble();
    mipercunc = pieces[5].toInt();
}

Doblst GammaList::matchEnergies( qreal energy, qreal toler )
{
  Doblst ret;
  for ( int i=0; i < count(); i++ ) {
      if ( fabs( energy - at(i)->energyval() ) < toler )
          ret << at(i)->energyval();
    }
    return ret;
}

Doblst GammaList::matchIPerc( qreal energy, qreal toler )
{
  Doblst ret;
  for ( int i=0; i < count(); i++ ) {
      if ( fabs( energy - at(i)->energyval() ) < toler )
          ret << at(i)->ipercval();
    }
    return ret;
}

Intlst GammaList::matchIdnucls( qreal energy, qreal toler )
{
    Intlst ret;
    for ( int i=0; i < count(); i++ )
      if ( fabs( energy - at(i)->energyval() ) < toler )
          ret << at(i)->idnucl();
    return ret;
}

void NuclideList::fillNuclideList( const QString &contents )
{
    clear();
    QStringList list = contents.split("\n", QString::SkipEmptyParts );
    int n = list.count();
    for (int i=0; i < n; i++) {
        QStringList pieces = list.at(i).split(";", QString::SkipEmptyParts);
        append( new Nuclide( pieces ) );
    }
}

void GammaList::fillGammaList( const QString &contents )
{
    clear();
    QStringList list = contents.split("\n", QString::SkipEmptyParts );
    int n = list.count();
    for (int i=0; i < n; i++) {
        QStringList pieces = list.at(i).split(";", QString::SkipEmptyParts);
        append( new GammaTransition( pieces ) );
    }
}

QString GammaList::matchEnergiesStr( qreal energy, qreal toler )
{
    QString ret;
    Doblst num = matchEnergies( energy, toler );
    for (int i=0; i < num.count(); i++ )
        ret += QString("%1").arg( num[i] ) + ", ";
    if ( ret.length() >= 2 ) ret.chop(2); // remove the extra ", "
    return ret;
}

QString NuclideList::matchNucs( qreal energy, qreal toler, GammaList *gammaList, int &howMany )
{
    QList<int> ids = gammaList->matchIdnucls( energy, toler );
    howMany = ids.count();
    QString ret;
    // Doblst num = dataMatchEnergies( energy );

    ret = namesList( ids ).join(", ");
    /*
    for (int i=0; i < ids.count(); i++ )
        ret += QString("%1").arg( ids[i] ) + ", ";
    if ( ret.length() >= 2 ) ret.chop(2); // remove the extra ", "
    */
    return ret;
}
