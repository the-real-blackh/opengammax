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

#ifndef SPECTRIO_H
#define SPECTRIO_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QTextStream>
#include <QDataStream>

#include "doblst.h"

class SpectrumIO : public QObject
{
    Q_OBJECT
public:
    SpectrumIO ( QObject *parent = 0 )
        : QObject( parent ) {}
    bool loadSpectrum(const QString &fileName );
    bool loadSpectrumCHN(const QString &fileName);
    bool loadSpectrumOgammsp(const QString &fileName);
    bool loadSpectrumIEC1455(const QString &fileName);
    bool loadSpectrumCAM(const QString &fileName);
    bool loadSpectrumASC(const QString &fileName);
    bool loadSpectrumN42(const QString &fileName);
    bool parseN42(const QString &buffer, QStringList &tokens);

    bool loadNuclideFile( const QString &fileName, QString &contents );

    QDateTime toDateTimeCentury(const QString &s);

    Intlst getCounts();
    //
    bool saveOpenGammaSpectrum( const QString &fileName,
      QList<int> counts,
      QString samDesc,
      QString qtyVal,
      QString qtyUnc,
      qint32 physUnit,
      QString strStart,
      QString strLT,
      QString strRT,
      QStringList ioEfficEnPts,
      QStringList ioEnxchan,
      QStringList ioFwhmxch );

    //
    QStringList fileList( QString path );
    QList<QList<qreal> > getInterwinnerOutputFile( const QString &fileName );
    QList<QList<qreal> > getOpenGammaBatchResults( const QString &fileName );
    QString reportSet( QList<QList<QList<qreal> > > set );
    QString reportSetToFile( QList<QList<QList<qreal> > > set, QStringList names );

/*
    bool loadSpectrumSPE(const QString &fileName);
    bool loadSpectrumXML(const QString &fileName);
    bool saveFile(const QString &fileName);
    bool parseFile(const QString &fileName);
*/
    int enFitType() { return ioEnFitType; }
    Doblst enParms() { return ioEnParms; }
    int fwhmFitType() { return ioFwhmFitType; }
    Doblst fwhmParms() { return ioFwhmParms; }
    Doblst efficParms() { return ioEfficParms; }
    //
    float LT() { return m_LT; }
    float RT() { return m_RT; }
    QDateTime acqDataTime() { return m_acqDateTime; }
    QDateTime samDataTime() { return m_samDateTime; }
    QString samDescr() { return ioSamDesc; }
    QString qtyUnc() { return ioQtyUnc; }
    QString qtyVal() { return ioQtyVal; }
    QStringList efficEnPts() { return ioEfficEnPts; }

private:

    qreal quint32tofloat( quint32 n );
    unsigned short int nchs;
    float m_LT;
    float m_RT;
    float m_en0;
    float m_en1;
    float m_en2;
    float m_fw0;
    float m_fw1;
    float m_fw2;
    QString acqDate;
    QString acqTime;
    QString m_filename;
    bool getHeaderCHN();
    bool getCountsCHN();
    bool getFooterCHN();
    QDataStream* inDst;
    QTextStream* inTst;
    QDateTime m_acqDateTime;
    QDateTime m_samDateTime;

    QList<int> ioCounts;
    QString ioSamDesc;
    QString ioQtyVal;
    QString ioQtyUnc;
    qint32 ioPhysUnit;
    QString ioStrStart;
    QString ioStrLT;
    QString ioStrRT;
    int ioEnFitType;
    Doblst ioEnParms;
    int ioFwhmFitType;
    Doblst ioFwhmParms;
    QStringList ioEfficEnPts;
    Doblst ioEfficParms;
    QStringList ioEnxchan;
    QStringList ioFwhmxch;
};

#endif /*SPECTRIO_H_*/
