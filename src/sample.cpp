/***************************************************************************
 *   Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com
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



#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QTextStream>
#include <QDataStream>
#include <QFile>
#include "doblst.h"
#include "counts.h"
#include <qdatetime.h>
#include <doblst.h>
#include "sample.h"


bool Sample::clear(){
    counts->clear();
    sampleSize = 0.0;
    sampleUncert = 0.0;
    realTime = 0.0;
    liveTime = 0.0;
    acqDateTime.currentDateTime();
    sampleDateTime.currentDateTime();
    return true;
}
