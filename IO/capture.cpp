
/***************************************************************************
* Copyright (C) 2011 by LeDoyle Roy Pingel
*
* This file is part of OpenGammaCapture (ogcapture).
* OpenGammaCapture is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* OpenGammaCapture is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with OpenGammaX. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#include <qmath.h>
#include <QVariant>
#include <QFile>
#include <QDirIterator>
#include <QStringList>
#include <QProgressDialog>
#include <QIODevice>
#include <stdio.h>
#include "spectrio.h"
#include "alsaStream.h"



bool Capture::openSrc( const QString fileName )
{
    srcName = fileName;
    // later on add interfaces besides alsa
    srcFd = alsa.open(srcName);
    return TRUE;
}


 QList <int>  Capture::rdSrc(void)
 {
     return(alsa.rdData());
 }

 QList <int>  Capture::rdSrcPulse(void)
 {
     return( alsa.rdPulse());
 }

 bool Capture::srcCapture(void)
 {
     return(alsa.capture());
 }

bool Capture::openDst( const QString fileName )
{
    dstName = fileName;
    bool rtn = FALSE;
/*
    QFileInfo fi( fileName );
    QString ext = fi.suffix().toUpper();  // ext == "CHN", "IEC", "OGAMMSP", "ASC" etc
    if ( ext == "OGAMMSP" )
        printf("Unsupported file type.\n");
    else if ( ext == "CHN" )
        printf("Unsupported file type.\n");
    else if ( ext == "IEC" )
        printf("Unsupported file type.\n");
    else if ( ext == "ASC" )
        printf("Unsupported file type.\n");
    else if ( ext == "N42" )
        rtn = openFileN42(fileName);
    else if ( ext == "CNF" )
        printf("Unsupported file type.\n");
    else
        printf("Unidentified file type.\n");
*/
    rtn = openN42(fileName);
    return rtn;
}

bool Capture::wrDst( QList <int> spect )
{
    bool rtn = FALSE;
/*
    QFileInfo fi( dstName );
    QString ext = fi.suffix().toUpper();  // ext == "CHN", "IEC", "OGAMMSP", "ASC" etc
    if ( ext == "OGAMMSP" )
        printf("Unsupported file type.\n");
    else if ( ext == "CHN" )
        printf("Unsupported file type.\n");
    else if ( ext == "IEC" )
        printf("Unsupported file type.\n");
    else if ( ext == "ASC" )
        printf("Unsupported file type.\n");
    else if ( ext == "N42" )
        rtn = writeFileN42(void);
    else if ( ext == "CNF" )
        printf("Unsupported file type.\n");
    else
        printf("Unidentified file type.\n");
*/
    rtn = wrN42(spect);
    return rtn;
}

