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

#ifndef GENERICCALIBRATION_H
#define GENERICCALIBRATION_H

#include <QObject>
#include "doblst.h"
#include "functionsdefinitions.h"

//!  Par&acirc;metros de calibra&ccedil;&atilde;o para v&aacute;rios modelos de ajuste.
//!  Parametric calibration of various models to fit.
/*!
  Cont&eacute;m, gerencia e executa calibra&ccedil;&otilde;es e os correspondentes
  par&acirc;metros para conjuntos de pontos de dados.

  Contains, manages and performs calibrations and corresponding
  parameters for sets of data points.
*/

class GenericCalibration : public QObject
{
    Q_OBJECT
public:
    GenericCalibration( qreal (*pointf) (Doblst, qreal),
                        QObject *parent = 0 );
    GenericCalibration( qreal (*pointf) (QList<Doblst>, qreal),
                        QObject *parent = 0 );
    GenericCalibration( Doblst a, qreal (*pointf) (Doblst, qreal),
                        QObject *parent = 0 );
    qreal (*f) (Doblst, qreal);
    qreal (*f2) (QList<Doblst>, qreal);
    bool doCalib( Doblst x, Doblst y, Doblst s );
    bool doCalibPolyN( int grade, Doblst x, Doblst y, Doblst s );
    bool doCalibSqrtEn( Doblst x, Doblst y, Doblst s );
    bool doCalibExpPlusConst( Doblst x, Doblst y, Doblst s );
    void setCalibDataPoints( Doblst x, Doblst y, Doblst s );
    Doblst getXList() { return xList; }
    Doblst getYList() { return yList; }
    int nParms() { return fitParms.count(); }

    void setDefaultCalibration();

    bool calibTypeF();
    bool calibTypeF2();

    void setParms( Doblst a );
    void setFixedParms( Doblst a );
    void setInitVals( Doblst a );
    void clearParms();
    Doblst parms();
    qreal parm( int i) { return fitParms[i]; }
    qreal sparm( int i) { return sFitParms[i]; }
    bool isCalib() { return ( nParms() != 0 ); }
    qreal y( qreal x );
    QString getFitReport() { return fitReport; }
    void setPolyGrade( int gr ) { polyGrade = gr; }
private:
    Doblst xList;
    Doblst sList;
    Doblst yList;
    Doblst fixedParms;
    int nFiPa() { return fixedParms.count(); }

    Doblst initVals;
    int typeParms;
    QString fitReport;
    int polyGrade; // used only for polynomial fit
    int n;
    Doblst fitParms;
    Doblst sFitParms;
    Doblst cov;
    qreal cq;
};

#endif /*GENERICCALIBRATION_H_*/
