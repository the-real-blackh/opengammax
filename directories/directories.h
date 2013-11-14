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

#ifndef DIRECTORIES_H
#define DIRECTORIES_H

#include "ui_directories.h"

class DirectoriesDlg : public QDialog
{
    Q_OBJECT
public:
    DirectoriesDlg( QString spectraPath, QString analysisNuclibPath, QString calibNuclibPath, QWidget *parent = 0 );
    QStringList getNewValues();

private slots:
    void on_pbSelectSpectraDir_clicked();
    void on_pbSelectAnalysisNucLibDir_clicked();
    void on_pbSelectCalibNucLibDir_clicked();

private:
    Ui::DirectoriesDlg ui;
};

#endif // DIRECTORIES_H
