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


#ifndef NUCLIDE_H
#define NUCLIDE_H

#include <QObject>
#include <QStringList>
#include "doblst.h"
#include <qmath.h>

class Nuclide : public QObject
{
    Q_OBJECT
public:
    Nuclide( QStringList pieces, QObject *parent = 0 );
    int idnucl() const { return midnucl; }
    QString name() const { return mname; }
    qreal halflife() const { return mhalflife; }
    QString unit() const { return munit; }
private:
    int midnucl;
    QString mname;
    qreal mhalflife;
    QString munit;
};

class GammaTransition : public QObject
{
    Q_OBJECT
public:
    GammaTransition( QStringList pieces, QObject *parent = 0 );
    int idgamma() const { return midgamma; }
    int idnucl() const { return midnucl; }
    qreal energyval() const { return menergyval; }
    int energyunc() const { return menergyunc; }
    qreal ipercval() const { return mipercval; }
    int ipercunc() const { return mipercunc; }
private:
    int midgamma;
    int midnucl;
    qreal menergyval;
    int menergyunc;
    qreal mipercval;
    int mipercunc;
};

class GammaList : public QList<GammaTransition *>
{
public:
    Doblst matchEnergies( qreal energy, qreal toler );
    Doblst matchIPerc( qreal energy, qreal toler );
    QString matchEnergiesStr( qreal energy, qreal toler );
    Intlst matchIdnucls( qreal energy, qreal toler );
    void fillGammaList( const QString &contents );
};

class NuclideList : public QList<Nuclide *>
{
public:
    QStringList namesList( QList<int> idnuclList );
    QString nameByIdnucl( int id );
    qreal halfLifeByIdnucl( int id );
    QString halfLifeUnitByIdnucl( int id );
    void fillNuclideList( const QString &contents );
    QString matchNucs( qreal energy, qreal toler, GammaList *gammaList, int &howMany );
    qreal matchHalfLife( qreal energy, qreal toler, GammaList *gammaList );
};

#endif // NUCLIDE_H
