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

#ifndef QWTPLOTCURVEWITHUNCERT_H
#define QWTPLOTCURVEWITHUNCERT_H

#include <QVector>
#include <qwt_plot_curve.h>

#define STYLE (QwtPlotCurve::Lines)

class QwtPlotCurveWithUncert : public QwtPlotCurve
{
public:
    QwtPlotCurveWithUncert()
        : QwtPlotCurve() {}
    enum CurveStyle // (herited from QwtPlotCurve)
    {
        NoCurve,
        Lines,
        Sticks,
        Steps,
        Dots,
       //ErrorBars,
        UserCurve = 100
    };
    //enum { ErrorBars = UserCurve };
    inline double sy( int i ) const { return m_sy[i]; }
    inline bool isPointEnabled( int i ) const { return m_isPointEnabled[i]; }
    void setDataAllEnabled(
      const double *xData, const double *yData, const double *syData, int size );
    void setStyle( int style );
    void setColors( int color1, int color2, int color3, int color4, int color5 );
    void showScatData( QwtPlot *pl, QColor c2, QColor c3, QColor c4, QColor c5, int dSize = 7 );
    //void enableAPoint( bool enable, int i );
    void drawAnErrorBar( bool enable, int rw );
protected:
    virtual void drawCurve(QPainter *p, int style,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF& canvasRect,
        int from, int to ) const;
    void drawErrorBars(QPainter *p,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        int from, int to ) const;
    //void drawAnErrorBarEnabled(QPainter *painter,
    //    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    //    int iPt ) const;
    void drawAnErrorBarDisabled(QPainter *painter,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        int iPt ) const;
    QVector<qreal> sy() { return m_sy; }
private:
    QVector<qreal> m_sy;
    QVector<bool> m_isPointEnabled;
    QColor barColor;
    QColor serifColor;
};


#endif // QWTPLOTCURVEWITHUNCERT_H
