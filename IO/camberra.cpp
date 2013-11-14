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

#include <qmath.h>
#include <QVariant>
#include <QFile>
#include <QDirIterator>
#include <QStringList>
#include <QProgressDialog>

#include "spectrio.h"
#include "sample.h"

bool SpectrumIO::readCAM(const QString &fileName, Sample & sp)
{
    m_filename = fileName;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return NULL;
    inTst = new QTextStream(&file);
    // AQUI 2009-11-14 implementar
    return TRUE;
}

