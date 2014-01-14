/***************************************************************************
 * # Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com
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
#include <QFile>

#ifdef HAVE_ALSA
#include "alsaStream.h"
#endif
#include "doblst.h"
#include "counts.h"
#include "sample.h"


enum LiveSource { UNDEFINED, ALSA, OSS };
typedef LiveSource LiveSource_t;

class SpectrumIO : public QObject
{
    Q_OBJECT
public:

    SpectrumIO ( QObject *parent = 0 );
    bool clear();
    bool readSpectrumFile(const QString & fileName, Sample & sp);
    bool readCHN(const QString & fileName, Sample & sp);
    bool readOgammsp(const QString & fileName, Sample & sp);
    bool readIEC1455(const QString & fileName, Sample & sp);
    bool readCAM(const QString & fileName, Sample & sp);
    bool readASC(const QString & fileName, Sample & sp);
    bool readN42(const QString & fileName, Sample & sp);

#ifdef HAVE_ALSA
    bool openLiveSignal(const char* deviceName, Sample & sam);
    bool readSpectrum(Sample & sam);
    bool readWaveforms(Sample & sam);
    bool startDataCapture(void);
    bool stopDataCapture(void);
    bool closeDataCapture(void);
#endif
    bool reRead(Sample & samp);
    bool parseN42(const QString &buffer, QStringList & tokens);
    bool writeN42(const QString);
    bool encodeN42(Intlst *spect);

#ifdef HAVE_ALSA
    bool openLiveSpectrum(const QString &fileName, Sample & sp );
    bool signalCapture(void);
    bool setSettings(bool invert, int threshold);
#endif
    bool open(const QString & fileName);

    bool writeSpectrum( const QString &fileName, Sample & sample );

    bool readNuclideFile( const QString &fileName, QString & contents );
    QDateTime toDateTimeCentury(const QString & s);

    bool saveSpectrumSimple( const QString &fileName, Sample & sample);

    bool saveNISTn42Spectrum( const QString &fileName, Sample & sample);

    bool saveSpectrum(const QString &fileName,
                      Sample & sample,
                      QStringList ioEfficEnPts,
                      QStringList ioEnxchan,
                      QStringList ioFwhmxch );

    bool saveOpenGammaSpectrum(const QString & fileName,
                               Sample & sample,
                               QStringList ioEfficEnPts,
                               QStringList ioEnxchan,
                               QStringList ioFwhmxch );

    QStringList fileList( QString path );
    QList<QList<qreal> > getInterwinnerOutputFile( const QString &fileName );
    QList<QList<qreal> > getOpenGammaBatchResults( const QString &fileName );
    QString reportSet( QList<QList<QList<qreal> > > set );
    QString reportSetToFile( QList<QList<QList<qreal> > > set, QStringList names );

#ifdef HAVE_ALSA
    // here are links to the lowest levels
    AlsaStream *alsa;
#endif

private:
    QString srcName;
    QString dstName;
    QFile *dstQf;
    qreal quint32tofloat( quint32 n );
    float m_en0;
    float m_en1;
    float m_en2;
    float m_fw0;
    float m_fw1;
    float m_fw2;
    QString acqDate;
    QString acqTime;
    QString m_filename;
    bool getHeaderCHN(Sample & sp);
    bool getCountsCHN(Sample & sp);
    bool getFooterCHN(Sample & sp);
    QDataStream* inDst;
    QTextStream* inTst;
};

#endif /*SPECTRIO_H_*/
