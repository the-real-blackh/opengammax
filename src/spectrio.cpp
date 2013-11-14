/***************************************************************************
 *   Copyright (C) 2007 by Marcelo Francis Maduar                          *
 *   maduar@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <qmath.h>
#include <QVariant>
#include <QFile>
#include <QDirIterator>
#include <QStringList>
#include <QProgressDialog>

#include "spectrio.h"

bool SpectrumIO::loadSpectrum( const QString &fileName )
{
    bool loaded = false;
    QFileInfo fi( fileName );
    QString ext = fi.suffix().toUpper();  // ext == "CHN", "IEC", "OGAMMSP", "ASC" etc
    if ( ext == "OGAMMSP" )
        if ( loadSpectrumOgammsp( fileName ) )
            loaded = true;
    if ( ext == "CHN" )
        if ( loadSpectrumCHN( fileName ) )
            loaded = true;
    if ( ext == "IEC" )
        if ( loadSpectrumIEC1455( fileName ) )
            loaded = true;
    if ( ext == "ASC" )
        if ( loadSpectrumASC( fileName ) )
            loaded = true;
    if ( ext == "N42" )
        if ( loadSpectrumN42( fileName ) )
            loaded = true;
    if ( ext == "CNF" )
        if ( loadSpectrumCAM( fileName ) )
            loaded = false; // 2009-02-17  to be implem'd
    return loaded;
}

bool SpectrumIO::loadSpectrumCHN(const QString &fileName)
{
    m_filename = fileName;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return false;
    inDst = new QDataStream(&file);
    inDst->setByteOrder( QDataStream::LittleEndian );
    if (!getHeaderCHN()) return false;
    if (!getCountsCHN()) return false;
    if (!getFooterCHN()) return false;
    return true;
}


bool SpectrumIO::loadSpectrumASC(const QString &fileName)
{
    bool ret = true;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) return false;
    QTextStream in(&file);
    QStringList lines = in.readAll().split("\n");

    m_RT = lines[0].toFloat( &ret );
    m_LT = lines[1].toFloat( &ret );

    if ( ret ) {
        ioCounts << 0; // default channel 0
        for (int i=0; i < lines.count(); i++) {
            ioCounts << lines[i].toInt( &ret );
            if ( !ret ) break;
        }
    }
    ret = ( m_RT > 0 );
    ret = ( m_LT > 0 );
    ret = ( lines.count() > 3 );
    return ret;
}

bool SpectrumIO::parseN42(const QString &in, QStringList &tokens)
{
    bool rtn = true;
    QString token;
    QChar data;
    QString buffer;
    int n, max, tagdepth, tokenCount;
    enum state { TAG, DATA, QUOTE, UNKN } state;

    buffer = in.simplified();
    // parse into tags and data
    max = buffer.size();
    tagdepth = 0;
    tokenCount = 0;
    for(n = 0, state = UNKN; n < max; n++) {
        data = buffer[n];
        switch(data.unicode()){
        case '<':
            switch ( state ){
            case UNKN:
                token.clear();
                token += data;
                tagdepth++;
                state = TAG;
                break;
            case QUOTE:
                token += data;
                break;
            case DATA:
                tokens += token;
                tokenCount++;
                token.clear();
                token += data;
                tagdepth++;
                state = TAG;
                break;
            case TAG:
                token += data;
                tagdepth++;
                break;
            default:        // this is an error
                break;
            }
            break;
        case '>':
            switch ( state ){
            case TAG:
                tagdepth--;
                if(tagdepth == 0){   // nested tags because of comment
                    token += data;
                    tokens += token;
                    tokenCount++;
                    token.clear();
                    state = UNKN;
                }else{
                    token += data;
                }
                break;
            case DATA:
            case QUOTE:
            case UNKN:
            default:
                break;
            }
            break;
        case ' ':
            // simplified() leaves only single spaces between tokens
            switch ( state ){
            case DATA:
                tokens += token;
                tokenCount++;
                token.clear();
                state = UNKN;
                break;
            case TAG:
            case QUOTE:
                token += data;
                break;
            case UNKN:
                break;
            }
            break;
        case '"':
            switch ( state ){
            case UNKN:
                token.clear();
                token += data;
                state = QUOTE;
                break;
            case QUOTE:
                token += data;
                tokens += token;
                tokenCount++;
                token.clear();
                state = UNKN;
                break;
            case TAG:
            case DATA:
                token += data;
                break;
            }
        default:
            switch ( state ){
            case UNKN:
                token.clear();
                token += data;
                state = DATA;
                break;
            case QUOTE:
            case TAG:
            case DATA:
                token += data;
                break;
            }
            break;
        }
    }
    return TRUE;
}



bool SpectrumIO::loadSpectrumN42(const QString &fileName)
{
    bool rtn = true;
    QString buffer, token;
    QStringList tokens;
    QChar data;
    QTime time;
    int tokenCount;
    int n, max;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) return false;
    QTextStream in(&file);
    buffer = in.readAll();     // get entire file into buffer
    if(!(parseN42(buffer, tokens))){
        return FALSE;
    }
    // Process tokens
    n = 0;
    do{
        token = tokens.takeFirst();
        if(token == "<Measurement>") {
            do{
                token = tokens.takeFirst();
                if(token == "<InstrumentInformation>") {
                    do{
                        token = tokens.takeFirst();
                        if(token == "<InstrumentType>"){
                            do{
                                token = tokens.takeFirst();
                            }while(!(token == "</InstrumentType>") && !tokens.isEmpty());
                        }else if(token == "<Manufacturer>"){
                            do{
                                token = tokens.takeFirst();;
                            }while(!(token == "</Manufacturer>") && !tokens.isEmpty());
                        }else if(token == "<InstrumentModel>"){
                            do{
                                token = tokens.takeFirst();;
                            }while(!(token == "</InstrumentModel>") && !tokens.isEmpty());
                        }else if(token == "<InstrumentID>"){
                            do{
                                token = tokens.takeFirst();;
                            }while(!(token == "</InstrumentID>") && !tokens.isEmpty());
                        }else if(token == "</InstrumentInformation>") {
                            break;
                        }   // skip comments and unknown tags by doing nothing
                    }while(!(tokens.isEmpty()));
                }else if(token == "<MeasuredItemInformation>"){
                    do{
                        token = tokens.takeFirst();;
                        if(token == "<MeasurementLocation>"){
                            do{
                                token = tokens.takeFirst();;
                                if(token == "<Coordinates"){  // --> data is inside the tag so simple compare won't work
                                    do{
                                        token = tokens.takeFirst();
                                        if(token == "</Coordinates>"){
                                            break;
                                        }
                                    }while(!(tokens.isEmpty()));
                                }else if (token == "</MeasurementLocation>"){
                                    break;
                                }
                            }while(!(tokens.isEmpty()));
                        } else if(token == "</MeasuredItemInformation>"){
                            break;
                        }
                    }while(!(tokens.isEmpty()));
                } else if(token == "<Spectrum>"){
                    do{
                        token = tokens.takeFirst();
                        if(token == "<StartTime>"){
                            do{
                                token = tokens.takeFirst();
                                if(token == "</StartTime>"){
                                    break;
                                }else{
                                    // format: 2003-11-22T23:45:19
                                    acqDate = token.left(10);
                                    acqTime = token.right(8);
                                    m_acqDateTime = QDateTime::fromString(token, "yyyy-MM-ddTHH:mm:ss");
                                    if ( !m_acqDateTime.isValid() ){
                                        m_acqDateTime = QDateTime::fromString( "01/01/2000 00:00:01", "dd/MM/yyyy HH:mm:ss");
                                    }
                                    m_samDateTime = m_acqDateTime;
                                }
                            }while(!(tokens.isEmpty()));
                        }else if(token == "<RealTime>"){
                            do{
                                // format: PT60S
                                token = tokens.takeFirst();
                                if(token == "</RealTime>"){
                                    break;
                                }else{
                                    QByteArray textline;
                                    const char *text;
                                    textline = token.toAscii();
                                    text = textline.constData();
                                    n = sscanf(text, "PT%fS", &m_RT);
                                }

                            }while(!(tokens.isEmpty()));
                        }else if(token == "<LiveTime>"){
                            do{
                                // format: PT59.61S
                                token = tokens.takeFirst();
                                if(token == "</LiveTime>"){
                                    break;
                                }else{
                                    QByteArray textline;
                                    const char *text;
                                    textline = token.toAscii();
                                    text = textline.constData();
                                    n = sscanf(text, "PT%fS", &m_LT);
                                }
                            }while(!(tokens.isEmpty()));
                        }else if(token == "<Calibration>"){
                            // --> add code to use this for calibration
                            do{
                                token = tokens.takeFirst();
                                if(token == "<Equation>"){
                                    do{
                                        token = tokens.takeFirst();
                                        if(token == "</Equation>"){
                                            break;
                                        }
                                    }while(!(tokens.isEmpty()));
                                } else if (token == "</Calibration>"){
                                    break;
                                }
                            }while(!tokens.isEmpty());
                        }else if(token == "<ChannelData>"){
                            ioCounts << 0; // default channel 0
                            do{
                                token = tokens.takeFirst();
                                if(token == "</ChannelData>"){
                                    break;
                                }else{
                                    ioCounts << token.toInt(&rtn, 10);
                                }
                            }while(!(tokens.isEmpty()) && rtn);
                        } else if(token == "</Spectrum>"){
                            break;
                        }
                    }while(!(tokens.isEmpty()));
                }else if(token == "</Measurement>") {
                    break;
                }
            }while(!(tokens.isEmpty()));
        }
    } while(!(tokens.isEmpty()));
    return true;
}


bool SpectrumIO::loadSpectrumOgammsp(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return false;

    ioCounts.clear();
    QDataStream in(&file);
    in >> ioCounts >> ioSamDesc >> ioQtyVal >> ioQtyUnc >> ioPhysUnit
            >> ioStrStart >> ioStrLT >> ioStrRT >> ioEfficEnPts >> ioEnxchan >> ioFwhmxch;

    if ( ioCounts.isEmpty() ) return false;
     //
    m_acqDateTime = QDateTime::fromString( ioStrStart, "dd/MM/yyyy HH:mm:ss");
    if ( !m_acqDateTime.isValid() )
        m_acqDateTime = QDateTime::fromString( "01/01/2000 00:00:01", "dd/MM/yyyy HH:mm:ss");
    // setting members
    m_LT = ioStrLT.toFloat();
    m_RT = ioStrRT.toFloat();
    nchs = ioCounts.count();

    ioEnParms.clear();
    ioFwhmParms.clear();
    for (int i=0; i < ioEnxchan.count(); i++) ioEnParms   << ioEnxchan[i].toDouble();
    for (int i=0; i < ioFwhmxch.count(); i++) ioFwhmParms << ioFwhmxch[i].toDouble();

    return true;
}

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

/* NOTE: now listed under IEC 61455 */
bool SpectrumIO::loadSpectrumIEC1455(const QString &fileName)
{
    m_filename = fileName;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return false;
    inTst = new QTextStream(&file);
    QStringList list;
    QString line;
    // all = inTst->readAll(); 2009-11-18 doesn't work, don't know why.

    do {
        line = inTst->readLine();
        list << line;
    } while (!line.isNull());
    int nLm1 = list.count() - 1;

    QProgressDialog progress("Parsing spectrum...", "Abort parsing", 0, nLm1 );
    progress.setWindowModality(Qt::WindowModal);

    m_LT = list[1].mid( 4, 14).toFloat();
    m_RT = list[1].mid(18, 14).toFloat();

    m_acqDateTime = toDateTimeCentury( list[2].mid( 4, 17 ) );
    // m_samDateTime = toDateTimeCentury( list[2].mid( 22, 17 ));

    for (int iL=5; iL <= 8; iL++) {
        ioSamDesc += list[iL] + "\n";
    }
    int iL = 0;
    while ( (list[iL].left( 9) != "A004SPARE") && ( iL < nLm1 ) )
        iL++;
    if ( iL == nLm1 ) return false;
    while ( (list[iL].left(15) != "A004USERDEFINED" ) && ( iL < nLm1 ) )
        iL++;
    if ( iL == nLm1 ) return false;
    iL++;
    do {
        progress.setValue( iL );
         if (progress.wasCanceled())
             break;
        for ( int j=1; j<=5; j++ ) {
            ioCounts << list[iL].mid( 10*j, 10 ).toInt();
        }
        iL++;
    } while ( (!list[iL].isNull()) && (iL < nLm1) );
    return true;
}

bool SpectrumIO::loadSpectrumCAM(const QString &fileName)
{
    m_filename = fileName;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return false;
    inTst = new QTextStream(&file);
    // AQUI 2009-11-14 implementar
    return true;
}

bool SpectrumIO::saveOpenGammaSpectrum( const QString &fileName,
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
      QStringList ioFwhmxch )
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    else {
        QDataStream out(&file);
        out << counts << samDesc << qtyVal << qtyUnc << physUnit << strStart
                << strLT << strRT << ioEfficEnPts << ioEnxchan << ioFwhmxch;
        return true;
    }
}

bool SpectrumIO::getHeaderCHN()
{
    qint16 Type;
    qint16 MCA;
    qint16 Segm;
    quint8 StartSec1;
    quint8 StartSec2;
    quint32 RLTime20ms;
    quint32 LVTime20ms;
    quint8 dd1;
    quint8 dd2;
    quint8 MM1;
    quint8 MM2;
    quint8 MM3;
    quint8 yy1;
    quint8 yy2;
    quint8 sep;
    quint8 hh1;
    quint8 hh2;
    quint8 mm1;
    quint8 mm2;
    quint16 ChOffset;
    quint16 LngData;

    QString centryMinus1 = "20";
    const int decadeLimit = 2;

    *inDst >> Type >> MCA >> Segm >> StartSec1 >> StartSec2 >>
      RLTime20ms >> LVTime20ms >> dd1 >> dd2 >> MM1 >> MM2 >> MM3 >>
      yy1 >> yy2 >> sep >> hh1 >> hh2 >> mm1 >> mm2  >>
      ChOffset >> LngData;

    // setting members
    m_LT = 0.02 * LVTime20ms;
    m_RT = 0.02 * RLTime20ms;
    QString ddMMMyyyy = "";
    QString hhmm = "";
    ddMMMyyyy += dd1;
    ddMMMyyyy += dd2;
    ddMMMyyyy += MM1;
    ddMMMyyyy += MM2;
    ddMMMyyyy += MM3;
    QChar decadeChar(yy1);
    int decadeNum = decadeChar.digitValue();
        if (decadeNum > decadeLimit)
        centryMinus1 = "19";
    ddMMMyyyy += centryMinus1;
    ddMMMyyyy += yy1;
    ddMMMyyyy += yy2;
    hhmm += hh1;
    hhmm += hh2;
    hhmm += ":";
    hhmm += mm1;
    hhmm += mm2;
    hhmm += ":";
    hhmm += StartSec1;
    hhmm += StartSec2;
    acqDate = ddMMMyyyy;
    acqTime = hhmm;
    nchs = LngData;

    QLocale usdate( QLocale::English, QLocale::UnitedStates );
    m_acqDateTime = usdate.toDateTime(acqDate + " " + acqTime, "ddMMMyyyy hh:mm:ss");
    if ( !m_acqDateTime.isValid() )
        m_acqDateTime.fromString("01Jan2001 00:00:01", "ddMMMyyyy hh:mm:ss");
    return true;
}

bool SpectrumIO::getCountsCHN()
{
    for (int i=0; i < nchs; i++)
    {
       int c;
       *inDst >> c;
       ioCounts << c;
    }
    return true;
}

bool SpectrumIO::getFooterCHN()
{
    quint16 tlrtyp;
    quint16 isCal;

    quint32 uien0;
    quint32 uien1;
    quint32 uien2;
    quint32 uifw0;
    quint32 uifw1;
    quint32 uifw2;

    char reserved [228];
    quint8 samLen;
    quint8 detLen;
    char samDes [63];
    char detDes [63];

    *inDst >> tlrtyp >> isCal >> uien0 >> uien1 >> uien2 >> uifw0 >> uifw1 >> uifw2;
    inDst->readRawData( reserved, 228 );
    *inDst >> detLen;
    inDst->readRawData( detDes, 63 );
    *inDst >> samLen;
    inDst->readRawData( samDes, 63 );

    m_en0 = quint32tofloat(uien0);
    m_en1 = quint32tofloat(uien1);
    m_en2 = quint32tofloat(uien2);
    m_fw0 = quint32tofloat(uifw0);
    m_fw1 = quint32tofloat(uifw1);
    m_fw2 = quint32tofloat(uifw2);

    ioEnFitType = 0;    // in CHN is always poly ( gr. 2 (3 params) or 1 (2 params) )
    ioEnParms.clear();
    ioEnParms << m_en0 << m_en1 << m_en2;
    if ( m_en2 < 1.0e-30 ) {
        ioEnParms.removeLast();
    }
    ioFwhmFitType = 1;    // in CHN is always  poly ( gr. 2 (3 params) or 1 (2 params) )
    ioFwhmParms.clear();
    ioFwhmParms << m_fw0 << m_fw1 << m_fw2;
    if ( m_fw2 < 1.0e-30 ) {
        ioFwhmParms.removeLast();
    }
    ioEfficParms.clear();
    ioEfficEnPts.clear(); // no effic fit in CHN, initializing empty

    ioSamDesc = QString::fromAscii( samDes, samLen ) + "\n\n" + QString::fromAscii( detDes, detLen );
    return true;
}

// methods to read SpectrumIO data

Intlst SpectrumIO::getCounts()
{
    Intlst cs;
    for (int i=0; i < ioCounts.size(); ++i) cs << ioCounts[i];
    return cs;
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

QList<QList<qreal> > SpectrumIO::getInterwinnerOutputFile( const QString &fileName )
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

QString SpectrumIO::reportSet( QList<QList<QList<qreal> > > set )
{
    QString rep;
    for (int i=0; i < set.size(); i++) {
        rep.append("\n");
        for (int j=0; j < set[i].size(); j++) {
            QString ln = "\n";
            for (int k=0; k < set[i][j].size(); k++) {
                ln.append( QString("%1 * ").arg( set[i][j][k] ) );
            }
            rep.append( ln );
        }
        rep.append( QString("\n **%1**").arg( set[i].size() ) );
    }
    rep.append( QString("\n ***%1***").arg( set.size() ) );
    return rep;
}

QString SpectrumIO::reportSetToFile( QList<QList<QList<qreal> > > set, QStringList names )
{
    QString rep;
    QString ln = "\n";
    for (int i=0; i < set.size(); i++) {
        rep.append("\n");
        for (int j=0; j < set[i].size(); j++) {
            for (int k=0; k < set[i][j].size(); k++) {
                ln.append( QString("%1 * ").arg( set[i][j][k] ) );
            }
            rep.append( ln );
        }
        rep.append( QString("\n **%1**").arg( set[i].size() ) );
    }
    rep.append( QString("\n ***%1***").arg( set.size() ) );
    rep.append( names.join( ln ) );
    return rep;
}

bool SpectrumIO::loadNuclideFile( const QString &fileName, QString &contents )
{
    bool loaded = false;
    if ( !fileName.isEmpty() ) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&file);
            contents = stream.readAll();
            loaded = true;
        }
    }
    return loaded;
}

qreal SpectrumIO::quint32tofloat( quint32 n )
{
    quint32 n1     = n / 8388608; // it´s 2^23
    qreal signif   = 1.0 + (n % 8388608) * pow(2.0, -23.0);
    int signal;
    if ( n1 / 256 == 1) signal = -1; else signal = 1;
    int exp2 = n1 % 256;
    qreal expon = pow(2.0, exp2 - 127);
    return signal * expon * signif;
}
