
/***************************************************************************
 * # Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com                                                    *
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


#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <stdlib.h>
#include <iostream>
#include <qpen.h>
#include <qwt_plot_layout.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>

using namespace std;

#include "maininterface.h"
#include "spectrio.h"
#include "sample.h"



bool SpectrumIO::saveNISTn42Spectrum( const QString &fileName, Sample & samp){
    bool rtn;

    QFlags <QIODevice::OpenModeFlag> OpenMode(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);

    char buf[200], *bp;
    bp = &buf[0];
    Counts *counts = samp.counts;
    Intlst *spect = counts->getAllCounts();
    QFile nuFile(fileName);
    if (!nuFile.open(QFile::WriteOnly | QFile::Truncate))
        return false;
    QTextStream n42Image(&nuFile);
    QString str;

    n42Image << "<Measurement>" << endl;
    n42Image << "    <Remarks>" << endl;
    n42Image << samp.remarks << endl;
    n42Image << "    </Remarks>" << endl;
    n42Image << "    <InstrumentInformation>" << endl;
    n42Image << "        <InstrumentType>" << endl;
    n42Image << "        </InstrumentType>" << endl;
    n42Image << "        <Manufacturer>" << endl;
    n42Image << "        </Manufacturer>" << endl;
    n42Image << "        <InstrumentModel>" << endl;
    n42Image << "        </InstrumentModel>" << endl;
    n42Image <<  "        <InstrumentID>" << endl;
    n42Image << "        </InstrumentID>" << endl;
    n42Image << "    </InstrumentInformation>" << endl;
    n42Image << "    <MeasuredItemInformation>" << endl;
    n42Image << "        <MeasurementLocation>" << endl;
    n42Image << "            <Coordinates>" << endl;        // The spec calls for coordinates inside the braces
    n42Image << "            </Coordinates>" << endl;
    n42Image << "        </MeasurementLocation>" << endl;
    n42Image << "    </MeasuredItemInformation>" << endl;
    n42Image << "    <Spectrum>" << endl;
    n42Image << "        <StartTime>" << endl;
    n42Image << "        " << samp.sampleDateTime.toString(Qt::ISODate) << endl;
    n42Image << "        </StartTime>" << endl;
    n42Image << "        <RealTime>" << endl;
    n42Image << "            " << str.setNum(samp.realTime, 'f', 1) << endl;
    n42Image << "        </RealTime>" << endl;
    n42Image << "        <LiveTime>" << endl;
    n42Image << "            " << str.setNum(samp.liveTime, 'f', 1) << endl;
    n42Image << "        </LiveTime>" << endl;
    n42Image << "        <Calibration>" << endl;
    n42Image << "            <Equation>" << endl;
    n42Image << "            </Equation>" << endl;
    n42Image << "        </Calibration>" << endl;
    n42Image << "        <ChannelData>" << endl;
    if(!spect->empty()){
        int sz = spect->size();
        for (int i=0; i < sz;){
            n42Image << "            ";
            for(int n=0; i < sz && n < 8; i++, n++){
                sprintf(buf, "%0.5d ", spect->at(i));
                n42Image << buf;
            }
            n42Image << endl;
        }
    }
    n42Image << "        </ChannelData>" << endl;
    n42Image << "    </Spectrum>" << endl;
    n42Image << "</Measurement>" << endl;
    n42Image.flush();
    return true;
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
    return true;
}



bool SpectrumIO::readN42(const QString &fileName, Sample& samp)
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
        return false;
    }
    // Process tokens
    Intlst * ilist = new Intlst();
    ilist->clear();
    Counts *cp = new Counts(ilist);
    samp.counts = cp;
    n = 0;
    do{
        token = tokens.takeFirst();
        if(token == "<Measurement>") {
            do{
                token = tokens.takeFirst();
                if(token == "<Remarks>"){
                  do{
                        token = tokens.takeFirst();
                        samp.remarks += token;
                    }while(!(token == "</Remarks>") && !tokens.isEmpty());
                }
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
                            }while(token != "</InstrumentID>" && !tokens.isEmpty());
                        }else
                            break;
                    } while(token != "</InstrumentInformation>" && !(tokens.isEmpty()));  // skip comments and unknown tags by doing nothing
                } else if(token == "<MeasuredItemInformation>"){
                    do{
                        token = tokens.takeFirst();
                        if(token == "<MeasurementLocation>"){
                            do{
                                token = tokens.takeFirst();;
                                if(token == "<Coordinates"){  // --> data is inside the tag so simple compare won't work
                                    do{
                                        token = tokens.takeFirst();
                                    }while(token != "</Coordinates>" && !(tokens.isEmpty()));
                                }
                            }while((token != "</MeasurementLocation>") && !(tokens.isEmpty()));
                        }
                    }while((token != "</MeasuredItemInformation>") && !(tokens.isEmpty()));
                } else if(token == "<Spectrum>"){
                    do{
                        token = tokens.takeFirst();
                        if(token == "<StartTime>"){
                            do{
                                token = tokens.takeFirst();
                                // format: 2003-11-22T23:45:19
                                samp.acqDateTime = QDateTime::fromString( token, "yyyy-MM-ddTHH:mm:ss");
                                if ( !samp.acqDateTime.isValid() ){
                                    samp.acqDateTime = QDateTime::fromString( "01/01/1990 00:00:01", "dd/MM/yyyy HH:mm:ss");
                                }
                            }while((token != "</StartTime>") && !(tokens.isEmpty()));
                        }else if(token == "<RealTime>"){
                            do{
                                // format: PT60S
                                token = tokens.takeFirst();
                                \
                                QByteArray textline;
                                const char *text;
                                textline = token.toLatin1();
                                text = textline.constData();
                                n = sscanf(text, "PT%fS", &samp.realTime);
                            }while((token != "</RealTime>") && !(tokens.isEmpty()));
                        }else if(token == "<LiveTime>"){
                            do{
                                // format: PT59.61S
                                token = tokens.takeFirst();
                                QByteArray textline;
                                const char *text;
                                textline = token.toLatin1();
                                text = textline.constData();
                                n = sscanf(text, "PT%fS", &samp.liveTime);
                            }while((token != "</LiveTime>") && !(tokens.isEmpty()));
                        }else if(token == "<Calibration>"){
                            // --> add code to use this for calibration
                            do{
                                token = tokens.takeFirst();
                                if(token == "<Equation>"){
                                    do{
                                        token = tokens.takeFirst();
                                    }while((token != "</Equation>") && !(tokens.isEmpty()));
                                }
                            }while((token != "</Calibration>") && !tokens.isEmpty());
                        }else if(token == "<Calibration Type"){
                            //<Calibration Type="Other" ID="qcc">
                        }else if(token == "<Remarks>"){
                            do{
                                token = tokens.takeFirst();
                            }while((token != "</Remarks>") && !(tokens.isEmpty()));
                        }else if(token == "<ChannelData>"){
                            *ilist << 0; // default channel 0
                            do{
                                token = tokens.takeFirst();
                               *ilist << token.toInt(&rtn, 10);
                            }while((token != "</ChannelData>") && !(tokens.isEmpty()));
                        }
                    }while((token != "</Spectrum>") && !(tokens.isEmpty()));
                }
            }while((token != "</Measurement>") && !(tokens.isEmpty()));
        }
    } while(!(tokens.isEmpty()));
    cp->updateCounts(*ilist);
    return true;
}



