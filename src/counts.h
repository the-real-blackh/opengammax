/***************************************************************************
 * # Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com
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


#ifndef COUNTS_H
#define COUNTS_H

#include <QObject>
#include "doblst.h"

//!  Par&acirc;metros de calibra&ccedil;&atilde;o para v&aacute;rios modelos de ajuste.
/*!
  Cont&eacute;m, gerencia e executa calibra&ccedil;&otilde;es e os correspondentes
  par&acirc;metros para conjuntos de pontos de dados.
*/

class Counts : public QObject
{
    Q_OBJECT
public:

//! Construtor de Counts.
/*!
  Cria uma inst&acirc;ncia de Counts a partir de um objeto SpectrumIO.
*/
    Counts(Intlst *ilist);
    bool isValid() { return nchs() != 0 ; }

/*!
  Define e recupera as contagens e incertezas.
*/
    //! retorna contagens do canal i.
/*!
  \param i inteiro contendo o n&uacute;mero do canal.
  \return retorna contagens do canal i.
  \sa setCounts( QList<int> cts ), nchs() e ch( int i )
*/

    void setSecDiffAndSigma( int nSmoothCh );

    int countAt(int i)  { return cntList.at(i); }
    void updateCounts( Intlst & ilst );
    bool clear();
    int sdiff(int i)  { return secDiff.at(i); }
    int nchs() { return cntList.size(); }
    Intlst * getAllCounts() { return &cntList; }

    bool getCountsRange( int i, int n, Doblst &cts, Doblst &sCts );
    bool getSecDiffRange( int cand, int nSdCh, Doblst &secdiff, Doblst &sSecDiff );

    QList<int> cands() { return candidates; }
    int cand(int i)  { return candidates.at(i); }
    int ncands() { return candidates.count(); }
    int findCandidates( const qreal pkCut );

    Intlst cntList;
    Doblst sigmaCounts;
    Doblst secDiff;
    Doblst sigmaSecDiff;
private:
    Intlst candidates;
    void doBinom(int i, Intlst &binom);
    void doSmoothed( int n, Intlst counts, Intlst binom, Doblst &smoothed );
};

#endif // COUNTS_H
