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

#ifndef ACQUISITION_H
#define ACQUISITION_H

#include <QObject>
#include <QDateTime>

//!  Dados de aquisi&ccedil;&atilde;o do espectro.
/*!
  Cont&eacute;m e gerencia par&acirc;metros de aquisi&ccedil;&atilde;o de um dado espectro gama.
*/

class Acquisition : public QObject
{
    Q_OBJECT
public:
    Acquisition( float LT, float RT, QDateTime start, QObject* parent = 0 )
        : QObject( parent )
    {
        liveTime = LT;
        realTime = RT;
        acqDateTime = start;
    }
    QDateTime acqdt() { return acqDateTime; }

    //! retorna tempo &uacute;til de contagem (LT).
    /*!
      \sa lt()
    */
    float lt() { return liveTime; }

    //! retorna tempo real de contagem (RT).
    /*!
      \sa rt()
    */
    float rt() { return realTime; }


    //! retorna data e hor&aacute;rio de in&iacute;cio da contagem.

private:
    float liveTime;
    float realTime;
    QDateTime acqDateTime;
};

#endif /*ACQUISITION_H_*/
