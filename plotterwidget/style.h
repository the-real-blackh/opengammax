/***************************************************************************
 * # Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com                                           *
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
#ifndef STYLES_H
#define STYLES_H
#include <qwt_plot_curve.h>
#include "ui_style.h"

class StyleDlg : public QDialog
{
    Q_OBJECT
public:
    StyleDlg( const int curStyle, QWidget *parent = 0 );
    int getStyle();
private slots:
    void on_cancelButtonStyle(QPoint pos) {return;};
    void on_okButtonGetStyle(QPoint pos);
private:
    Ui::StyleDlg ui;
};

#endif // STYLES_H
