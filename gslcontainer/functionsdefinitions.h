#ifndef FUNCTIONSDEFINITIONS_H
#define FUNCTIONSDEFINITIONS_H

#include <QObject>
#include <QVarLengthArray>
#include <QVector>
#include <QTime>
#include <gsl/gsl_sf_erf.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_math.h>

#include "doblst.h"

//! Defini&ccedil;&otilde;es de fun&ccedil;&otilde;es est&aacute;ticas.
/*!
  Fun&ccedil;&otilde;es matem&aacute;ticas est&aacute;ticas usadas em gr&aacute;ficos e ajustes.
*/

class FunctionsDefinitions : public QObject
{
     Q_OBJECT

public:

FunctionsDefinitions( QObject* parent = 0 )
    : QObject( parent )
{

}

/*
static double parabx (double t)
{
    return t;
}

static double paraby (double t)
{
    return t*t;
}
*/

//! defini&ccedil;&otilde;es de fun&ccedil;&otilde;es est&aacute;ticas.
/*!
  \sa circx()
*/
static double circx (Doblst a, double t)
{
    return a[0] + a[2]*cos(t);
}

static double circy (Doblst a, double t)
{
    return a[1] + a[2]*sin(t);
}

static double gensin ( Doblst a, double t )
{
    return a[0] + a[1] * (sin (a[2]*t + a[3] ) );
}

static void gensinPartials( double t, double s, QVector<double> a, double * Ji )
{
    Ji[0] = 1.0 / s;  //  d/da0
    Ji[1] = sin (a[2]*t + a[3]) / s;   // d/da1
    Ji[2] = a[1] * cos (a[2]*t + a[3]) * t / s;   // d/da2
    Ji[3] = a[1] * cos (a[2]*t + a[3]) / s;   // d/da3
}

static void gensinVertshiftPhaseFixedPartials( double t, double s,
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

static double poly ( Doblst a, double t)
{
    if ( a.isEmpty() )
        return GSL_NAN;
    else {
    int s = a.size();
    double x = a[s-1];
    for (int i = s-2; i >= 0; i--)    x = a[i] + x*t;
    return x;
    }
}

static double polyLog ( Doblst a, double t)
{
    if ( a.isEmpty() || ( t<=0 ) )
        return GSL_NAN;
    else {
    int s = a.size();
    double x = a[s-1];
    qreal lnT = log(t);
    for (int i = s-2; i >= 0; i--)    x = a[i] + x*lnT;
    return exp(x);
    }
}

static double sqrtT ( Doblst a, double t)
{
    int s = a.size();
    double x = a[s-1];
    for (int i = s-2; i >= 0; i--)    x = a[i] + x*sqrt(t);
    return x;
}

static double fwhmInEnergyPoly( QList<Doblst> enAndFwhmParms, double t )
{
    qreal fwhmCh = poly( enAndFwhmParms[1], t );
    qreal hwhm = 0.5 * fwhmCh;
    return poly( enAndFwhmParms[0], t + hwhm ) - poly( enAndFwhmParms[0], t - hwhm );
}

static double fwhmInEnergySqrtT( QList<Doblst> enAndFwhmParms, double t )
{
    qreal fwhmCh = sqrtT( enAndFwhmParms[1], t );
    qreal hwhm = 0.5 * fwhmCh;
    return poly( enAndFwhmParms[0], t + hwhm ) - poly( enAndFwhmParms[0], t - hwhm );
}

static double gaussian (Doblst a, double t)
{
    return a[0]*exp( -a[1]*pow(t-a[2],2) );
}

static double erf_i (Doblst a, double t)
// a[0] is NOT used, it´s Ok.
{
    return 0.5 * (1 + gsl_sf_erf( sqrt(a[1]) * (t - a[2]) ) );
}

static double expSimple ( Doblst a, double t )
{
    if ( a.isEmpty() )
        return GSL_NAN;
    else
    if ( a.count() != 2 )
        return GSL_NAN;
    else
        return a[0] * exp( a[1]*t );
}

static double expPlusConst ( Doblst a, double t )
{
    if ( a.isEmpty() )
        return GSL_NAN;
    else
    if ( a.count() != 3 )
        return GSL_NAN;
    else
        return a[0] * exp( a[1]*t ) + a[2];
}

static double normalized_erf( Doblst a, double t )
{
    return 0.5 * a[0] * ( 1 + gsl_sf_erf( sqrt(a[1]) * (t - a[2]) ) );       // Model:
      // Yi = p(t).(r0+r1.t+r2.t^2) + (1-p(t)).(l0+l1.t+l2.t^2) +
      //    + sum( ai.exp(-ki.(t-ci)^2), i=0..nPk-1 )
      // where
      // p(t) = sum(ai/sum(a)*(1/2).[1+erf(sqrt(ki).(t-ci))], i=0..nPk-1
      // ki are constant parameters received from gsl
      //
}

static double baseline( Doblst lft, Doblst rgt, QList<Doblst> pks, double t )
{
    double x = 0.0;
    double sumh = 0.0;
    for (int i=0; i < pks.size(); i++)
        sumh += pks[i][0];
    for (int i=0; i < pks.size(); i++)
        x += normalized_erf( pks[i], t );
    double wei = x / sumh;
    return (1-wei) * poly( lft, t) + wei * poly( rgt, t );
}

static double baselineAntiga ( QList<Doblst> parms, double t)
{
    Doblst lft = parms[0];
    Doblst rgt = parms[1];
    double x = 0.0;
    double sumh = 0.0;
    for (int i = 2; i < parms.size(); i++)
        sumh += parms[i][0];
    for (int i = 2; i < parms.size(); i++)
        x += normalized_erf( parms[i], t );
    double wei = x / sumh;
    return (1-wei) * poly( lft, t) + wei * poly( rgt, t );
}

static double gaussiansSumAntiga( QList<Doblst> parms, double t )
{
    double sum = 0.0;
    for ( int i = 2; i < parms.size(); i++ ) {
        sum += gaussian( parms[i], t);
    }
    return sum;
}

static double totalMultipletFunction( QList<Doblst> parms, double t )
{
    double sum = poly( parms[0], t );
    for ( int i = 1; i < parms.size(); i++ ) {
        sum += gaussian( parms[i], t);
    }
    return sum;
}

static double SecondDerivGauss( double x, double ai, double ki, double ci )
{
    double sq = (x-ci)*(x-ci);
    return 2*ai*ki*(2*ki*sq-1) * exp(-ki*sq);
}

static double SecondDerivSmoothedGauss( double x, double ai, double ki, double ci, int n )
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

           /* Partial derivatives: */
           /* Jacobian matrix J(i,j) = dfi / dxj, */
           /* where fi = (Yi - yi)/sigma[i],      */
           /*       Yi = *****************  */
           /* and the xj are the parameters (ai,ki,ci) */

static void SecondDerivGaussPartials( double x, double s, double ai, double ki, double ci,
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

static void SecondDerivSmoothedGaussPartials( double x, double s, double ai, double ki, double ci, int n,
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

static double randomReal( double max )
{
    QTime now = QTime::currentTime();
    qsrand( now.msec() );
    return double( qrand() );
}

// redefinindo algumas funcoes (maio/2010)

static double gaussiansSum( QList<Doblst> parms, double t )
{
    double sum = 0.0;
    int nPk = parms.count();
    for ( int i = 0; i < nPk; i++ ) {
        sum += gaussian( parms[i], t);
    }
    return sum;
}

static double gaussiansSumPartDeriv( QList<Doblst> parms, int j, double t )
{
    int nPk = parms.count();
    double ret;
    if ( j < nPk ) {         // d/dh
        double h = parms[j][0];  // height of j-th peak
        ret = gaussian( parms[j], t );
        ret = ret / h;
    }
    else {                   // d/dc
        double k = parms[j-nPk][1]; // k-width  of j-th peak
        double c = parms[j-nPk][2]; // centroid of j-th peak
        ret = gaussian( parms[j-nPk], t );
        ret = ret * 2.0 * k * (t-c);
    }
    return ret;
}

}; // end of class FunctionDefinitions

#endif // FUNCTIONSDEFINITIONS_H
