/***************************************************************************
 *  Copyright (C) 2011 by LeDoyle Pingel, lrpingel@yahoo.com
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
#include <QPainter>

#include <qwt_scale_map.h>
#include <qwt_painter.h>
#include <qwt_symbol.h>
#include "maininterface.h"

#include "qwtplotcurvewithuncert.h"
#include "style.h"

/*!
  \brief Draw the line part (without mbols) of a curve interval.
  \param painter Painter
  \param style curve style, see QwtPlotCurve::CurveStyle
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa draw(), drawDots(), drawLines(), drawSteps(), drawSticks()
*/

void QwtPlotCurveWithUncert::drawCurve(QPainter *painter, int style,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF& canvasRect,
    int from, int to ) const
{
    style = STYLE;
    switch (style)
    {
        case Lines:
            if ( testCurveAttribute(Fitted) )
            {
                // we always need the complete
                // curve for fitting
                from = 0;
                to = dataSize() - 1;
            }
            drawLines(painter, xMap, yMap, canvasRect, from, to);
            break;
        case Sticks:
            drawSticks(painter, xMap, yMap, canvasRect, from, to);
            break;
        case Steps:
            drawSteps(painter, xMap, yMap, canvasRect, from, to);
            break;
        case Dots:
            drawDots(painter, xMap, yMap, canvasRect, from, to);
            break;
        //case ErrorBars:
        //    drawErrorBars(painter, xMap, yMap, canvasRect, from, to);
        //    break;
        //
        case NoCurve:
        default:
            break;
    }
}

/*!
  Draw sticks

  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted

  \sa draw(), drawCurve(), drawDots(), drawLines(), drawSteps()
*/
/*
void QwtPlotCurveWithUncert::drawErrorBars(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    int from, int to ) const
{
    for (int i = from; i <= to; i++)
    {
        if (isPointEnabled(i))
            drawAnErrorBarEnabled( painter, xMap, yMap, i );
        else
            drawAnErrorBarDisabled( painter, xMap, yMap, i );
    }
}

void QwtPlotCurveWithUncert::drawAnErrorBarEnabled(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    int iPt ) const
{
    const int xi = xMap.transform( x(iPt) );
    const int y1 = yMap.transform( y(iPt) - sy(iPt) );
    const int y2 = yMap.transform( y(iPt) + sy(iPt) );
    painter->setPen( barColor );
    QwtPainter::drawLine( painter, xi, y1, xi, y2 ); // vertical bar
    painter->setPen( serifColor );
    QwtPainter::drawLine( painter, xi-2, y1, xi+2, y1 ); // lower serif
    QwtPainter::drawLine( painter, xi-2, y2, xi+2, y2 ); // upper serif
}

void QwtPlotCurveWithUncert::drawAnErrorBarDisabled(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    int iPt ) const
{
    const int xi = xMap.transform( x(iPt) );
    const int y1 = yMap.transform( y(iPt) - sy(iPt) );
    const int y2 = yMap.transform( y(iPt) + sy(iPt) );
    painter->setPen( Qt::red );
    QwtPainter::drawLine( painter, xi, y1, xi, y2 ); // vertical bar
    painter->setPen( Qt::red );
    QwtPainter::drawLine( painter, xi-2, y1, xi+2, y1 ); // lower serif
    QwtPainter::drawLine( painter, xi-2, y2, xi+2, y2 ); // upper serif
}
*/
void QwtPlotCurveWithUncert::setDataAllEnabled(const double *xData, const double *yData,
  const double *syData, int nBars )
{
    QwtPlotCurve::setSamples( xData, yData, nBars );
    m_sy.resize( nBars );
    m_isPointEnabled.resize( nBars );
    for ( int i=0; i < nBars; i++ ) {
        m_sy[i] = syData[i];
        m_isPointEnabled[i] = true;
    }
}

/*!
  Set the curve's drawing style

  \param style Curve style
  \sa CurveStyle, style()
*/
void QwtPlotCurveWithUncert::setStyle(int newStyle)
{
    if ( newStyle != (int) style() )
    {
        QwtPlotCurve::setStyle( (QwtPlotCurve::CurveStyle) newStyle );
        itemChanged();
    }
}

void QwtPlotCurveWithUncert::showScatData( QwtPlot *pl, QColor c2, QColor c3, QColor c4, QColor c5,
                                           int dSize )
{
    detach();
    // setRenderHint(QwtPlotItem::RenderAntialiased);
    //setStyle( QwtPlotCurve::CurveStyle( QwtPlotCurveWithUncert::ErrorBars ));
    setStyle( QwtPlotCurve::CurveStyle( QwtPlotCurveWithUncert::Dots ));
    setSymbol( new QwtSymbol( QwtSymbol::Diamond, QColor(c2), QColor(c3), QSize(dSize, dSize)));
    barColor = c4;
    serifColor = c5;
    attach( pl  );
}
/*
void QwtPlotCurveWithUncert::enableAPoint( bool enable, int i )
{
    m_isPointEnabled[i] = enable;
}
*/
