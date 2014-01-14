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
// This is the entry point for all file and hardware I/O


#include <qmath.h>
#include <QVariant>
#include <QFile>
#include <QDirIterator>
#include <QStringList>
#include <QProgressDialog>
#include "counts.h"
#include "spectrio.h"
#ifdef HAVE_ALSA
#include "alsaStream.h"
#endif
#include "sample.h"


static LiveSource_t liveSource = UNDEFINED;

SpectrumIO::SpectrumIO(QObject *parent)
{

}


// counts are stored in the SpectrumIO object by methods below
bool SpectrumIO::readSpectrumFile( const QString& fileName, Sample& sp )
{
    bool rslt = false;

    QFileInfo fi( fileName );
    QString ext = fi.suffix().toUpper();  // ext == "CHN", "IEC", "OGAMMSP", "ASC" etc

    if ( ext == "OGAMMSP" )
        rslt = readOgammsp( fileName, sp);
    if ( ext == "CHN" )
        rslt = readCHN( fileName, sp );
    if ( ext == "IEC" )
        rslt = readIEC1455( fileName, sp );
    if ( ext == "ASC" )
        rslt = readASC( fileName, sp );
    if ( ext == "N42" )
        rslt = readN42( fileName, sp );
    if ( ext == "CNF" )
        rslt = readCAM( fileName, sp );
    return rslt;     // a copy is kept in spectrumTbl
}

#ifdef HAVE_ALSA
bool SpectrumIO::setSettings(bool invert, int threshold){
    if(liveSource == ALSA){
         AlsaStream *alsa = new AlsaStream();  // to do: leak
         return( alsa->setSettings(invert, threshold));
     }else{
         return FALSE;
     }
}
#endif

#ifdef HAVE_ALSA
// counts are read into the SpectrumIO object by methods below
bool SpectrumIO::openLiveSignal( const char* deviceName, Sample & samp )
{
    bool rtn;
    AlsaStream *alsa = new AlsaStream();  // to do: leak
    if(!(alsa->open(deviceName, samp))){
        return false;
    }
    liveSource = ALSA;
    return true;
}
#endif

#ifdef HAVE_ALSA
bool SpectrumIO::readSpectrum(Sample & samp){
   if(liveSource == ALSA){
        AlsaStream *alsa = new AlsaStream();  // to do: leak
        alsa->readSpectrum( samp);
        return TRUE;
    }else{
        return FALSE;
    }
}
#endif

#ifdef HAVE_ALSA
// selection of waveform or spectrum is in the lowest level code
bool SpectrumIO::readWaveforms(Sample & samp){
     if(liveSource == ALSA){
        AlsaStream *alsa = new AlsaStream();  // to do: leak
        alsa->readWaveforms( samp);
        return TRUE;
     }else{
         return FALSE;
     }
}
#endif

#ifdef HAVE_ALSA
bool SpectrumIO::reRead(Sample & samp)          // used to update display
{
     if(liveSource == ALSA){
        AlsaStream *alsa = new AlsaStream();  // to do: leak
        alsa->reRead( samp);
        return TRUE;
     }else{
         return FALSE;
     }
}
#endif

#ifdef HAVE_ALSA
bool SpectrumIO::startDataCapture()
{
     if(liveSource == ALSA){
        AlsaStream *alsa = new AlsaStream();  // to do: leak
        alsa->startCapture();
        return TRUE;
     }else{
         return FALSE;
     }
}
#endif

#ifdef HAVE_ALSA
bool SpectrumIO::stopDataCapture()
{
     if(liveSource == ALSA){
        AlsaStream *alsa = new AlsaStream();  // to do: leak
        alsa->stopCapture();
         return TRUE;
     }else{
         return FALSE;
     }
}
#endif

#ifdef HAVE_ALSA
bool SpectrumIO::closeDataCapture()
{
     if(liveSource == ALSA){
        AlsaStream *alsa = new AlsaStream();  // to do: leak
        alsa->close();
        liveSource = UNDEFINED;
         return TRUE;
     }else{
         return FALSE;
     }
}
#endif

#ifdef HAVE_ALSA
bool SpectrumIO::clear()
{
     if(liveSource == ALSA){
        AlsaStream *alsa = new AlsaStream();  // to do: leak
        alsa->clear();
         return TRUE;
     }else{
         return FALSE;
     }
}
#endif

// Save out spectrum and associated data to a file.  Original source does not matter.

bool SpectrumIO::saveSpectrumSimple( const QString &fileName, Sample & samp)
{
    bool written = false;
    QFileInfo fi( fileName );
    QString ext = fi.suffix().toUpper();  // ext == "CHN", "IEC", "ASC" etc
    if ( ext == "CHN" )
        written = false;
    if ( ext == "IEC" )
        written = false;
    if ( ext == "ASC" )
        written = false;
    if ( ext == "N42" ){
        written  = saveNISTn42Spectrum( fileName, samp);
    }
    if ( ext == "CNF" )
        written = false;
    return written;
}


/*
bool SpectrumIO::saveSpectrum( const QString &fileName,
                   Intlst counts,
                   QString samDesc,
                   QString qtyVal,
                   QString qtyUnc,
                   QString physUnit,
                   QString strStart,
                   QString strLT,
                   QString strRT,
                   QStringList ioEfficEnPts,
                   QStringList ioEnxchan,
                   QStringList ioFwhmxch ){
    bool written = false;
    QFileInfo fi( fileName );
    QString ext = fi.suffix().toUpper();  // ext == "CHN", "IEC", "ASC" etc
    // ogammsp requires much more information and does not work at this time
    if ( ext == "OGAMMSP" ){
       written = saveOpenGammaSpectrum( fileName,counts, samDesc,qtyVal,qtyUnc, physUnit,
               strStart, strLT, strRT, ioEfficEnPts,ioEnxchan,ioFwhmxch );
    }
}

*/

QDateTime SpectrumIO::toDateTimeCentury(const QString &s)
{
    // 2-digit year specifications will be interpreted as years between 1980 and 2079.
    QString sloc = s;
    int yy = s.mid(6, 2).toInt();
    if ( yy < 80 )
        sloc.insert( 6, "20" );
    else
        sloc.insert( 6, "19" );
    return QDateTime::fromString( sloc, "dd/MM/yyyy hh:mm:ss" );
}



QStringList SpectrumIO::fileList( QString path )
{
    QDirIterator it( path, QStringList("*.lib"), QDir::NoFilter, QDirIterator::Subdirectories);
    QStringList files;
    while (it.hasNext()) {
        files << it.next();
    }
    return files;
}


// This has not been tested.
QList<QList<qreal> > SpectrumIO::getOpenGammaBatchResults( const QString &fileName )
{
    QFile file(fileName);
    QList<QList<qreal> > data;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QList<qreal> rl;
        rl << 0.0;
        data << rl;
        return data;
    }
    QTextStream in(&file);
    // parse Interwinner output
    QStringList lines = in.readAll().split("\n");

    int i = 0;
    int j = 0;
    QString ln;
    if ( lines.size() > 0 )
        do {
            ln = lines[i];
            ++i;
        }
        while ( (ln.mid(10, 16) != "List by energies") && (i < lines.size() ) );

    if ( (i < lines.size() ) && (i > 0) )
    {
        j = i + 3;
        if ( j < lines.size() ) {
            ln = lines.at(j);
            while ( (ln.left(1) == "|" ) && ((j+1) < lines.size()) ) {
                QList<qreal> reallist;
                qreal energy  = ln.mid( 6, 7 ).toDouble();
                qreal fwhm    = ln.mid(15, 5 ).toDouble();
                qreal gross   = ln.mid(27,11 ).toDouble();
                qreal net     = ln.mid(39,11 ).toDouble();
                qreal uncert  = ln.mid(51, 8 ).toDouble();
                reallist << energy << fwhm << gross << net << uncert;
                data << reallist;
                j++;
                ln = lines.at(j);
            }
        }
    }
    return data;
}




qreal SpectrumIO::quint32tofloat( quint32 n )
{
    quint32 n1     = n / 8388608; // it�s 2^23
    qreal signif   = 1.0 + (n % 8388608) * pow(2.0, -23.0);
    int signal;
    if ( n1 / 256 == 1) signal = -1; else signal = 1;
    int exp2 = n1 % 256;
    qreal expon = pow(2.0, exp2 - 127);
    return signal * expon * signif;
}
