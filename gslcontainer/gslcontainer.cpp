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

#include <gsl/gsl_sf_erf.h>
#include <gsl/gsl_sf_gamma.h>

#include "gslcontainer.h"
// #include "opengammamathfuncs.h"
#include "functionsdefinitions.h"

bool GslContainer::genericNonlinearFunc2(
  int n, int p, Doblst t, Doblst xinit, Doblst y, Doblst sigma,
  int (*ff)(const gsl_vector*, void*, gsl_vector*),
  int (*fdf)(const gsl_vector*, void*, gsl_matrix*),
  int (*ffdf)(const gsl_vector*, void*, gsl_vector*, gsl_matrix* ),
  QString& rep,
  Doblst fixParms, Intlst partFixParms
)
{
    if ( n < 2 )
        return false;
    if ( n > t.count() )
        return false;
    m_n = n;
    m_p = p;
    iter = 0;
    covar = gsl_matrix_alloc (p, p);

    double arr_t[n];
    double arr_y[n];
    double arr_sigma[n];
    double arr_xinit[p];
    int nFixParms = fixParms.size();
    double arr_fixparms[nFixParms];
    int np = partFixParms.size();
    int arr_partfixparms[np];
    // arr_nfp[] = { # of parms BGleft, # of parms BGright, # of peaks (kwidths) }; the sum is nFixParms

    /* // AQUI 2009-12-05 verificar se percisa, se sim, poor em outro lugar
    int nPk = p / 2;
    int nParmsPerBaseLine = ( nFixParms - nPk ) / 2;
    int arr_nfp[] = { nParmsPerBaseLine, nParmsPerBaseLine, nPk };
    */
    
    for (int j=0; j<n; j++)
    {
        arr_t[j] = t[j];
        arr_y[j] = y[j];
        arr_sigma[j] = sigma[j];
    }
    for (int i=0; i<p; i++)
    {
        arr_xinit[i] = xinit[i];
    }
    for (int k2=0; k2 < nFixParms; k2++)
    {
        arr_fixparms[k2] = fixParms[k2];
    }

    for (int i=0; i < nFixParms; i++)
    {
        arr_fixparms[i] = fixParms[i];
        rep += QString("   °°   %1").arg( arr_fixparms[i] );
    }

    rep += "\n partfixparms:\n";
    for (int i=0; i < np; i++)
    {
        arr_partfixparms[i] = partFixParms[i];
        rep += QString("   °°   %1").arg( arr_partfixparms[i] );
    }

    struct type3 d = {n, arr_t, arr_y, arr_sigma, arr_fixparms, arr_partfixparms };
    gsl_multifit_function_fdf f;
    gsl_vector_view x = gsl_vector_view_array (arr_xinit, p);
    // recebe xinit como paramatro array e aloca x como gsl_vector_view

    f.f = ff;
    f.df = fdf;
    f.fdf = ffdf;
    f.n = n;
    f.p = p;
    f.params = &d;

    T = gsl_multifit_fdfsolver_lmsder;
    s = gsl_multifit_fdfsolver_alloc (T, n, p);

    gsl_multifit_fdfsolver_set (s, &f, &x.vector);

  //
    rep += "About to do the fit...";
    rep += "\ny +/- sigma-y:\n";
    for (int j=0; j<n; j++)
    {
        rep += QString("%1 +/- %2;\n").arg(arr_y[j]).arg(arr_sigma[j]);
    }
    rep += "\nEnd of y.\n";
    rep += "\n printState(p): \n";
    rep += printState( p );
    // 2009-08-20 AQUI para ativar impressao do jacobiano inicial

    rep += "\n printJacobian(n, p): \n";
    rep += printJacobian( n, p );

    rep += "\n HERE THE FIT iteration \n";

    do
    {
   // printJacobian(n,p);
      iter++;
      status = gsl_multifit_fdfsolver_iterate (s);
      rep += QString( " status = %1     ").arg( gsl_strerror( status ) );
      rep += printState( p ); // 2009-12-04 AQUI para ativar impressao do estado
      if (status)
          break;
      status = gsl_multifit_test_delta (s->dx, s->x,1e-4, 1e-4);
    }
    while (status == GSL_CONTINUE && iter < 500);
    gsl_multifit_covar (s->J, 0.0, covar);

    rep += "\n";
    rep += "\n ********************************************************\n";
    rep += "\n ****** Fim do genericNonlinearFit **********************\n";
    rep += QString( " status = %1     \n").arg( gsl_strerror( status ) );
    rep += "\n ********************************************************\n\n";
    return true;
}

QString GslContainer::printState( int p )
{
    QString rp = QString ("iter: %1 - x: ").arg( iter );
    for (int i=0; i < p; ++i)
        rp += QString("%1, ").arg( gsl_vector_get (s->x, i) );
    rp += QString ("\n iter: %1 - f: \n").arg( iter );
    for (uint j=0; j < s->f->size; ++j)
        rp += QString("%1, ").arg( gsl_vector_get (s->f, j) );
    rp += QString (" - |f(x)| = %1\n").arg( gsl_blas_dnrm2 (s->f) );
    return rp;
}

QString GslContainer::printJacobian(const int n, const int p)
{
    QString rep;
    rep += "\n Jacobian: \n";
    for (int i=0; i<n; i++)
    {
        rep += "\n";
        for (int j=0; j<p; j++)
        {
            double a = gsl_matrix_get( s->J, i, j );
            rep += QString("%1   ").arg(a);
        }
    }
    return rep;
}

void GslContainer::getResultsLevMar(int &it, Doblst &x, Doblst &sigma,
  double &csod, int &stat )
{
    #define FIT(i) gsl_vector_get(s->x, i)
    #define ERR(i) sqrt(gsl_matrix_get(covar,i,i))

    x.clear(); 
    sigma.clear();
    it = iter;
    for (int i=0; i < m_p; i++)
    {
        x << FIT(i);
        sigma << ERR(i);
    }
    csod = pow( gsl_blas_dnrm2(s->f), 2.0 ) / (m_n - m_p); // csod = Chi-Squared Over Dof, Dof = Degrees Of Freedom
    stat = status;
}

void GslContainer::execLinearFit( int n, int p, Doblst xlst, Doblst ylst, Doblst slst,
  Doblst &a, Doblst &covar, double &chisq )
{
	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;
	X = gsl_matrix_alloc( n, p);
	y = gsl_vector_alloc( n );
	w = gsl_vector_alloc( n );
	c = gsl_vector_alloc( p );
	cov = gsl_matrix_alloc( p, p );

        a.clear();
        covar.clear();
        chisq = 0.0;
	
	for (int i=0; i < n; ++i)
	{
		double xpot = 1.0;
		for (int j=0; j < p; ++j)
		{
			gsl_matrix_set( X, i, j, xpot );
			xpot *= xlst[i];
		}
		gsl_vector_set( y, i, ylst[i] );
		gsl_vector_set( w, i, 1.0/( slst[i] + slst[i] ));
	}
	
	gsl_multifit_linear_workspace * work
	  = gsl_multifit_linear_alloc( n, p );
	gsl_multifit_wlinear( X, w, y, c, cov, &chisq, work );
	gsl_multifit_linear_free( work );

	for (int i=0; i < p; ++i)
	{
		a << gsl_vector_get( c, i );
	 	for (int j=0; j < p; ++j)
	 		covar << gsl_matrix_get( cov, i, j );
	}
	
	gsl_matrix_free( X );
	gsl_vector_free( y );
	gsl_vector_free( w );
	gsl_vector_free( c );
	gsl_matrix_free( cov );
	return;
}

/* -- model functions for peak search (2nd-deriv gaussian) */

double (*fpt1) ( double, double, double, double );

int GslContainer::dgauss_f (const gsl_vector * x, void *data, gsl_vector * f)
{
    size_t n = ((struct type3 *)data)->n;
    double *t = ((struct type3 *)data)->t;
    double *y = ((struct type3 *)data)->y;
    double *sigma = ((struct type3 *) data)->sigma;
    // double *fp = ((struct type3 *) data)->fixparms;

    double ai = gsl_vector_get (x, 0); // height
    double ki = gsl_vector_get (x, 1); // width-related param
    double ci = gsl_vector_get (x, 2); // centroid

    size_t i;

    for (i = 0; i < n; i++)
    {
        double Yi = FunctionsDefinitions::SecondDerivGauss(t[i], ai, ki, ci);
        gsl_vector_set (f, i, (Yi - y[i])/sigma[i]);
    }
    return GSL_SUCCESS;
}

int GslContainer::dgauss_df (const gsl_vector * x, void *data, gsl_matrix * J)
{
    size_t n = ((struct type3 *)data)->n;
    double *t = ((struct type3 *)data)->t;
    double *sigma = ((struct type3 *) data)->sigma;
    double ai = gsl_vector_get (x, 0);
    double ki = gsl_vector_get (x, 1);
    double ci = gsl_vector_get (x, 2);
    double Ji[3];
    size_t i;
    for (i = 0; i < n; i++)
    {
        /* Jacobian matrix J(i,j) = dfi / dxj, */
        /* where fi = (Yi - yi)/sigma[i],      */
        /*       Yi = *****************  */
        /* and the xj are the parameters (ai,ki,ci) */
        FunctionsDefinitions::SecondDerivGaussPartials( t[i], sigma[i], ai, ki, ci, Ji[0], Ji[1], Ji[2]);
        gsl_matrix_set (J, i, 0, Ji[0]);
        gsl_matrix_set (J, i, 1, Ji[1]);
        gsl_matrix_set (J, i, 2, Ji[2]);
    }
    return GSL_SUCCESS;
}

int GslContainer::dgauss_fdf (const gsl_vector * x, void *data, gsl_vector * f, gsl_matrix * J)
{
    dgauss_f (x, data, f);
    dgauss_df (x, data, J);
    return GSL_SUCCESS;
}

/* -- model functions for exponencial + constant term */

int GslContainer::expb_f (const gsl_vector * x, void *data, gsl_vector * f)
{
    size_t n = ((struct type3 *)data)->n;
    double *t = ((struct type3 *)data)->t;
    double *y = ((struct type3 *)data)->y;
    double *sigma = ((struct type3 *) data)->sigma;
    // double *fp = ((struct type3 *) data)->fixparms;

    // a * exp(k*t) + b
    double ai = gsl_vector_get (x, 0);
    double ki = gsl_vector_get (x, 1);
    double bi = gsl_vector_get (x, 2);

    size_t i;

    for (i = 0; i < n; i++)
    {
        Doblst pms;
        pms << ai << ki << bi;
        double Yi = FunctionsDefinitions::expPlusConst( pms, t[i] );
        gsl_vector_set (f, i, (Yi - y[i])/sigma[i]);
    }
    return GSL_SUCCESS;
}

int GslContainer::expb_df (const gsl_vector * x, void *data, gsl_matrix * J)
{
    size_t n = ((struct type3 *)data)->n;
    double *t = ((struct type3 *)data)->t;
    double *sigma = ((struct type3 *) data)->sigma;

    // a * exp(k*t) + b
    double ai = gsl_vector_get (x, 0);
    double ki = gsl_vector_get (x, 1);
    double bi = gsl_vector_get (x, 2);

    double Ji[3];
    size_t i;
    for (i = 0; i < n; i++)
    {
        /* Jacobian matrix J(i,j) = dfi / dxj, */
        /* where fi = (Yi - yi)/sigma[i],      */
        /*       Yi = *****************  */
        /* and the xj are the parameters (ai,ki,bi) */

        qreal ekt = exp( ki*t[i] );
        qreal s = sigma[i];

        Ji[0] = ekt / s;
        Ji[1] = ki * ai * ekt / s;
        Ji[2] = 1.0 / s;

        gsl_matrix_set (J, i, 0, Ji[0]);
        gsl_matrix_set (J, i, 1, Ji[1]);
        gsl_matrix_set (J, i, 2, Ji[2]);
    }
    return GSL_SUCCESS;
}

int GslContainer::expb_fdf (const gsl_vector * x, void *data, gsl_vector * f, gsl_matrix * J)
{
    expb_f (x, data, f);
    expb_df (x, data, J);
    return GSL_SUCCESS;
}

/* -- model functions for gaussSum */
/* Remembering definitions in FunctionsDefinitions:

static double gaussian (Doblst a, double t)
{
    return a[0]*exp( -a[1]*pow(t-a[2],2) );
}
static double gaussiansSum( QList<Doblst> parms, double t )
{
    double sum = 0.0;
    for ( int i = 0; i < parms.size(); i++ ) {
        sum += gaussian( parms[i], t);
    }
    return sum;
}
*/

int GslContainer::gaussSum_f (const gsl_vector * x, void *data, gsl_vector * f)
{
    size_t n = ((struct type3 *)data)->n;
    double *t = ((struct type3 *)data)->t;
    double *y = ((struct type3 *)data)->y;
    double *sigma = ((struct type3 *) data)->sigma;
    double *fp = ((struct type3 *) data)->fixparms;
    int *numFixPms = ((struct type3 *) data)->nfp;

    Doblst hei, cen;
    int p = x->size;
    int npk = p / 2;
    for (int j=0; j < npk; j++) hei << gsl_vector_get (x, j);
    for (int j=npk; j < 2*npk; j++) cen << gsl_vector_get (x, j);

    QList<Doblst> pms;
    for (int j=0; j < npk; j++) {
        Doblst pmsj;
        pmsj << hei[j] << fp[j] << cen[j];
        pms << pmsj;
    }

    size_t i;
    for (i = 0; i < n; i++)
    {
        double Yi = FunctionsDefinitions::gaussiansSum( pms, t[i] );
        gsl_vector_set (f, i, (Yi - y[i])/sigma[i]);
    }
    return GSL_SUCCESS;
}

int GslContainer::gaussSum_df (const gsl_vector * x, void *data, gsl_matrix * J)
{
    size_t n = ((struct type3 *)data)->n;
    double *t = ((struct type3 *)data)->t;
    double *y = ((struct type3 *)data)->y;
    double *sigma = ((struct type3 *) data)->sigma;
    double *fp = ((struct type3 *) data)->fixparms;
    int *numFixPms = ((struct type3 *) data)->nfp;

    Doblst hei, cen;
    int p = x->size;
    int npk = p / 2;
    for (int j=0; j < npk; j++) hei << gsl_vector_get (x, j);
    for (int j=npk; j < 2*npk; j++) cen << gsl_vector_get (x, j);

    QList<Doblst> pms;
    for (int j=0; j < npk; j++) {
        Doblst pmsj;
        pmsj << hei[j] << fp[j] << cen[j];
        pms << pmsj;
    }

    size_t i;
    for (i = 0; i < n; i++)
    {
        // Jacobian matrix J(i,j) = dfi / dxj,
        // where fi = (Yi - yi)/sigma[i],
        //       Yi = *****************
        // and the xj are the parameters (ai,ki,bi)

        for (int j = 0; j < p; j++) {
            double Yi = FunctionsDefinitions::gaussiansSumPartDeriv( pms, j, t[i] );
            gsl_matrix_set ( J, i, j, Yi / sigma[i] );
        }
    }

    return GSL_SUCCESS;
}

int GslContainer::gaussSum_fdf (const gsl_vector * x, void *data, gsl_vector * f, gsl_matrix * J)
{
    gaussSum_f (x, data, f);
    gaussSum_df (x, data, J);
    return GSL_SUCCESS;
}

/* -- model functions for test */

int GslContainer::test_f (const gsl_vector * x, void *data, gsl_vector * f)
{
    size_t n = ((struct type3 *)data)->n;
    // size_t np = ((struct type3 *)data)->np; // QQQ QQQ QQQ AQUI
    double *t = ((struct type3 *)data)->t;
    double *y = ((struct type3 *)data)->y;
    double *sigma = ((struct type3 *) data)->sigma;
    double *fp = ((struct type3 *) data)->fixparms;
    // int *pfp = ((struct type3 *) data)->partfixparms; // QQQ QQQ QQQ AQUI

    // get x to set a, c
    Doblst c, a;
    int p = x->size;
    int npk = p / 2;
    for (int j=0; j < npk; j++) c << gsl_vector_get (x, j);
    for (int j=npk; j < 2*npk; j++) a << gsl_vector_get (x, j);

    // set fixpms
    Doblst leftbl, rightbl, kwidths;
    // for (int i=0; i < pfp[0]; i++) leftbl << fp[i]; // QQQ QQQ QQQ AQUI
    // for (int i=0; i < pfp[1]; i++) rightbl << fp[ pfp[0]+i ]; // QQQ QQQ QQQ AQUI
    // for (int i=0; i < pfp[2]; i++) kwidths << fp[ pfp[0]+pfp[1]+i ]; // QQQ QQQ QQQ AQUI
    QList<Doblst> fixpms;
    fixpms << leftbl << rightbl << kwidths;

    // set f
    for ( uint i=0; i < n; i++ ) {
        // double Yi = baseline( fixpms, c, a, t[i] );
        // double Yi = netmultiplet( kwidths, c, a, t[i] );

        // double Yi = FunctionsDefinitions::multipletFunction( fixpms, c, a, t[i] );
        // QQQ QQQ QQQ
        double Yi = 7.77;
        gsl_vector_set (f, i, (Yi - y[i])/sigma[i]);
        // gsl_vector_set (f, i, Yi); // AQUI! soh usar para testes
    }
    return GSL_SUCCESS;
}

// 2009-08-21 PAREI AQUI
// 2010-01-27 AQUI retomar
// test_f acima está OK, contem agora a funçao do multipleto
// agora falta definir o jacobiano abaixo, test_df

int GslContainer::test_df (const gsl_vector * x, void *data, gsl_matrix * J)
{
    size_t n = ((struct type3 *)data)->n;
    // size_t np = ((struct type3 *)data)->np; // QQQ QQQ QQQ Aqui
    double *t = ((struct type3 *)data)->t;
    double *y = ((struct type3 *)data)->y;
    double *sigma = ((struct type3 *) data)->sigma;
    double *fp = ((struct type3 *) data)->fixparms;
    int *pfp = ((struct type3 *) data)->nfp; // partfixparms;

    // get x to set a, c
    Doblst c, a;
    int p = x->size;
    int npk = p / 2;
    for (int j=0; j < npk; j++) c << gsl_vector_get (x, j);
    for (int j=npk; j < 2*npk; j++) a << gsl_vector_get (x, j);

    // set fixpms
    Doblst leftbl, rightbl, kwidths;
    for (int i=0; i < pfp[0]; i++) leftbl << fp[i];
    for (int i=0; i < pfp[1]; i++) rightbl << fp[ pfp[0]+i ];
    for (int i=0; i < pfp[2]; i++) kwidths << fp[ pfp[0]+pfp[1]+i ];
    QList<Doblst> fixpms;
    fixpms << leftbl << rightbl << kwidths;

    // set J
    for ( uint i = 0; i < n; i++ ) {
        for ( int j = 0; j < p; j++ ) {
            // QQQ QQQ QQQ
            // qreal dYidxj = test_d( j, fixpms, c, a, t[i] );
            qreal dYidxj = 7.77;
            gsl_matrix_set (J, i, j, dYidxj / sigma[i] );
            // gsl_matrix_set (J, i, j, dYidxj); // AQUI!! soh para testes, tirar depois
        }
    }
    return GSL_SUCCESS;
}

int GslContainer::test_fdf (const gsl_vector * x, void *data, gsl_vector * f, gsl_matrix * J)
{
    test_f (x, data, f);
    test_df (x, data, J);
    return GSL_SUCCESS;
}
