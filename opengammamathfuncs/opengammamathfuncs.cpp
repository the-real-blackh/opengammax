/***************************************************************************
 *   Copyright (C) 2007, 2009 by Marcelo Francis Maduar                    *
 *   maduar@gmail.com                                                      *
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

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_erf.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_linalg.h>

#include "opengammamathfuncs.h"

OpengammaMathFuncs::OpengammaMathFuncs()
{
}

double poly ( Doblst a, double t)
{
    int s = a.size();
    double x = a[s-1];
    for (int i = s-2; i >= 0; i--)    x = a[i] + x*t;
    return x;
}

qreal humMil ( QList<Doblst> parms, double t)
{
    return t;
}

double parabx (double t)
{
    return t;
}

double paraby (double t)
{
    return t*t;
}

 double circx (double t)
{
    return cos(t);
}

 double circy (double t)
{
    return sin(t);
}

 double gensin ( Doblst a, double t )
{
    return a[0] + a[1] * (sin (a[2]*t + a[3] ) );
}

void gensinPartials( double t, double s, Doblst a, double * Ji )
{
    Ji[0] = 1.0 / s;  //  d/da0
    Ji[1] = sin (a[2]*t + a[3]) / s;   // d/da1
    Ji[2] = a[1] * cos (a[2]*t + a[3]) * t / s;   // d/da2
    Ji[3] = a[1] * cos (a[2]*t + a[3]) / s;   // d/da3
}

void gensinVertshiftPhaseFixedPartials( double t, double s,
  Doblst a, double * fp, double * Ji )
{
    // posicao vertical: fp0
    // amplitude: a0
    // veloc angular: a1
    // fase: fp1
    // ===> fp0+a0*sin(a1*t+fp1)
    Ji[0] = sin (a[1]*t + fp[1]) / s;   // d/da0
    Ji[1] = a[0] * cos (a[1]*t + fp[1]) * t / s;   // d/da1
}

double gaussian (Doblst a, double t)
{
    return a[0]*exp( -a[1]*pow(t-a[2],2) );
}

double erf_i (Doblst a, double t)
// a[0] is NOT used, it´s Ok.
{
    return 0.5 * (1 + gsl_sf_erf( sqrt(a[1]) * (t - a[2]) ) );
}

 double normalized_erf( Doblst a, double t )
{
    return 0.5 * a[0] * (1 + gsl_sf_erf( sqrt(a[1]) * (t - a[2]) ) ) ;
      // Yi = p(t).(r0+r1.t+r2.t^2) + (1-p(t)).(l0+l1.t+l2.t^2) +
      //    + sum( ai.exp(-ki.(t-ci)^2), i=0..nPk-1 )
      // where
      // p(t) = sum(ai/sum(a)*(1/2).[1+erf(sqrt(ki).(t-ci))], i=0..nPk-1
      // ki are constant parameters received from gsl
      //
}

double SecondDerivGauss( double x, double ai, double ki, double ci )
{
    double sq = (x-ci)*(x-ci);
    return 2*ai*ki*(2*ki*sq-1) * exp(-ki*sq);
}

double SecondDerivSmoothedGauss( double x, double ai, double ki, double ci, int n )
{
    double f1 = gsl_pow_4 (-n) * ai;
    double parc = 0.0;
    for (int j=0; j <= 2*n; j++) {
        double f2 = gsl_sf_choose (2*n, j);
        f2 *= exp(-ki*(x+j-n-ci));
        parc += f2;
    }
    return f1 * parc;
}

           // Partial derivatives:
           // Jacobian matrix J(i,j) = dfi / dxj,
           // where fi = (Yi - yi)/sigma[i],
           //       Yi = *****************
           // and the xj are the parameters (ai,ki,ci)

 void SecondDerivGaussPartials( double x, double s, double ai, double ki, double ci,
                                         double &Ji0, double &Ji1, double &Ji2 )
{
           double xci = x - ci;
           double sq = xci * xci;
           double e = exp(-ki*sq);
           double fact = 2*ki*sq;
           Ji0 = 2*ki*(fact-1) * e/s;  //  d/dai
           Ji1 = -2*ai*(0.5*fact*fact - 2.5*fact + 1) * e/s;   // d/dki
           Ji2 =  4*ai*ki*ki*xci*(fact - 3.0) * e/s;           // d/dci
}

 void SecondDerivSmoothedGaussPartials( double x, double s, double ai, double ki, double ci, int n,
                                         double &Ji0, double &Ji1, double &Ji2 )
{
    double f2 = gsl_pow_4 (-n);
    double f1 = f2 * ai;
    double xci = x - ci;
    double a1 = 0.0;
    double k1 = 0.0;
    double c1 = 0.0;
    for (int j=0; j <= 2*n; j++) {
        int jn = j - n;
        double fbin = gsl_sf_choose( 2*n, j);
        double fexp = exp(xci + jn);
        double fa1 = 2.0 * ki *(-1 + 2.0*ki*gsl_pow_2(xci + jn) );
        double fk1 = 2.0 * (-1 + 5.0*ki*gsl_pow_2(xci + jn) - 2.0*gsl_pow_2(ki)*gsl_pow_4(xci+jn) );
        double fc1 = 4.0 * gsl_pow_2(ki) * (-3 + 2.0 * ki * gsl_pow_2(xci + jn) ) * (jn - xci);
        a1 += fbin * fexp * fa1;
        k1 += fbin * fexp * fk1;
        c1 += fbin * fexp * fc1;
    }
    Ji0 = f2 * a1 / s;
    Ji1 = f1 * k1 / s;
    Ji2 = f1 * c1 / s;
}

qreal rightw ( QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t )
// = pd, peso da direita
{
    double sumh = 0.0;
    int npk = hei.count();
    for (int i = 0; i < npk; i++)
        sumh += hei[i];
    double Yi = 0.0;
    for (int i = 0; i < npk; i++) {
        Doblst locpms;
        locpms << hei[i]/sumh << fixedparms[2][i] << cen[i];
        Yi += normalized_erf( locpms, t );
    }
    return Yi;
}

qreal baselineErfWeightedPolys ( QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t)
{
    double wei = rightw( fixedparms, cen, hei, t );
    return (1-wei) * poly( fixedparms[0], t) + wei * poly( fixedparms[1], t );
}

/* AQUI: 2010-05-19: eliminar
Doblst cubicSplineCalcParams ( qreal tP, qreal tQ, Doblst cp, Doblst cq )
{
    double a_data[] = { 1.0, tP, pow(tP,2), pow(tP,3),
                        0.0, 1.0, 2*tP, 3*pow(tP,2),
                        1.0, tQ, pow(tQ,2), pow(tQ,3),
                        0.0, 1.0, 2*tQ, 3*pow(tQ,2) };
    double b_data[] = { poly( cp, tP), cp[1]+2*cp[2]*tP,
                        poly( cq, tQ), cq[1]+2*cq[2]*tQ };

    gsl_matrix_view m = gsl_matrix_view_array (a_data, 4, 4);
    gsl_vector_view b =  gsl_vector_view_array (b_data, 4);
    gsl_vector *x = gsl_vector_alloc (4);
    int s;
    gsl_permutation * p = gsl_permutation_alloc (4);
    gsl_linalg_LU_decomp (&m.matrix, p, &s);
    gsl_linalg_LU_solve (&m.matrix, p, &b.vector, x);
    gsl_permutation_free (p);
    Doblst xLst;
    for (int i=0; i<4; i++) xLst << gsl_vector_get( x, i);
    return xLst;
}
*/

qreal baselineCubicSpline ( QList<Doblst> pms, double t)
{
    return poly( pms[5], t);
}

qreal baselineMean ( QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t)
{
    return 0.5 * ( poly( fixedparms[0], t) + poly( fixedparms[1], t ) ) ;
}

qreal netmultiplet ( Doblst kwidth, Doblst cen, Doblst hei, double t)
{
    int npk = hei.count();
    double Yi = 0.0;
    for (int i = 0; i < npk; i++) {
        Doblst locpms;
        locpms << hei[i] << kwidth[i] << cen[i];
        Yi += gaussian( locpms, t );
    }
    return Yi;
}

qreal grossmultiplet ( QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t )
{
    return baselineMean( fixedparms, cen, hei, t) + netmultiplet( fixedparms[2], cen, hei, t );
}

qreal grossmultipletwithcubicbl ( QList<Doblst> fixedparms, Doblst cen, Doblst hei, Doblst cubic, double t )
{
    return poly( cubic, t ) + netmultiplet( fixedparms[2], cen, hei, t );
}

qreal plotgrossmultiplet ( QList<Doblst> parms, double t )
{
    QList<Doblst> fixedparms;
    fixedparms << parms[0] << parms[1] << parms[4];
    return grossmultipletwithcubicbl( fixedparms, parms[2], parms[3], parms[5], t );
}

qreal test_d ( int jj, QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t )
{
    int npk = hei.count();
    qreal sumh = 0.0;
    for (int i = 0; i < npk; i++)
        sumh += hei[i];
    double dYidxj = 0.0;
    if (( jj >= 0 ) && ( jj < 2 * npk)) {
        if ( jj < npk )
            dYidxj = dgdci( jj, fixedparms, cen, hei, t );
        else
            dYidxj = dgdai( jj-npk, fixedparms, cen, hei, t );
    }
        else
    dYidxj = -1.0;
    return dYidxj;
}

qreal dgdci ( int j, QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t )
{
    qreal ki = fixedparms[2][j];
    qreal sqkipi = sqrt( ki ) / M_SQRTPI;
    qreal BlMinBr = poly( fixedparms[0], t) - poly( fixedparms[1], t );
    qreal suma = hei.sum();
    Doblst locpms;
    locpms << hei[j] << fixedparms[2][j] << cen[j];
    qreal Fi = gaussian( locpms, t );
    return ( sqkipi*BlMinBr / suma + 2*ki*(t-cen[j]) ) * Fi;
}

qreal dgdai ( int j, QList<Doblst> fixedparms, Doblst cen, Doblst hei, double t )
{
    qreal BlMinBr = poly( fixedparms[0], t) - poly( fixedparms[1], t );
    qreal suma = hei.sum();
    qreal sumajerfj = 0.0;
    for (int j2=0; j2 < hei.count(); j2++) {
        Doblst locpms;
        locpms << hei[j2] << fixedparms[2][j2] << cen[j2];
        sumajerfj += hei[j2] * erf_i( locpms, t );
    }
    Doblst locpms;
    locpms << hei[j] << fixedparms[2][j] << cen[j];
    qreal erfi = erf_i( locpms, t);
    qreal Fi = gaussian( locpms, t );
    return BlMinBr * ( pow(suma,2)*sumajerfj - erfi/suma ) + Fi/hei[j];
}
