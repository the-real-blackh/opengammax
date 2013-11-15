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

#include <QtGui>
#include <QFileDialog>

#include "directories.h"

DirectoriesDlg::DirectoriesDlg
  ( QString spectraPath, QString analysisNuclibPath, QString calibNuclibPath, QWidget *parent )
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.leSelectedSpectraDir->setText( spectraPath );
    ui.leSelectedAnalysisNucLibDir->setText( analysisNuclibPath );
    ui.leSelectedCalibNucLibDir->setText( calibNuclibPath );
}

void DirectoriesDlg::on_pbSelectSpectraDir_clicked()

{
    QString dirName = QFileDialog::getExistingDirectory( this, tr("Select folder"), ui.leSelectedSpectraDir->text() );
    if (!dirName.isEmpty())
        ui.leSelectedSpectraDir->setText( dirName );
}

void DirectoriesDlg::on_pbSelectAnalysisNucLibDir_clicked()

{
    QString dirName = QFileDialog::getExistingDirectory( this, tr("Select folder"), ui.leSelectedAnalysisNucLibDir->text() );
    if (!dirName.isEmpty())
        ui.leSelectedAnalysisNucLibDir->setText( dirName );
}

void DirectoriesDlg::on_pbSelectCalibNucLibDir_clicked()

{
    QString dirName = QFileDialog::getExistingDirectory( this, tr("Select folder"), ui.leSelectedCalibNucLibDir->text() );
    if (!dirName.isEmpty())
        ui.leSelectedCalibNucLibDir->setText( dirName );
}

QStringList DirectoriesDlg::getNewValues()
{
    QStringList lst;
    lst << ui.leSelectedSpectraDir->text()
      << ui.leSelectedAnalysisNucLibDir->text()
      << ui.leSelectedCalibNucLibDir->text();
    return lst;
}
