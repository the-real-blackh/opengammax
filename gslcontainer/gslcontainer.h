#ifndef GSLCONTAINER_H_
#define GSLCONTAINER_H_

#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit.h>  // linear fit
#include <gsl/gsl_multifit_nlin.h> // nonlinear fit
#include <QObject>
#include <QString>

#include "doblst.h"

struct type3 {
       size_t n;
       double * t;
       double * y;
       double * sigma;
       double * fixparms;
       int * nfp;
    };

//!  Cont&ecirc;iner para chamada das fun&ccedil;&otilde;es de ajuste da biblioteca GSL.
/*!
  Classe para preparar e receber dados para a execuccedil;&atilde;o
  de algumas funcoes da biblioteca GSL para ajuste de curvas a dados experimentais.
*/

class GslContainer : public QObject
{
    Q_OBJECT
public:
    GslContainer (QObject *parent = 0)
        : QObject( parent ) {}
    bool genericNonlinearFunc2(
      int n, int p, Doblst t, Doblst xinit, Doblst y, Doblst sigma,
      int (*ff)(const gsl_vector*, void*, gsl_vector*),
      int (*fdf)(const gsl_vector*, void*, gsl_matrix*),
      int (*ffdf)(const gsl_vector*, void*, gsl_vector*, gsl_matrix* ),
      QString& rep,
      Doblst fixParms = Doblst(), Intlst partFixParms = Intlst()
    );
    void getResultsLevMar(int &it, Doblst &x, Doblst &sigma, double &csod,
      int &stat );
    void execLinearFit( int n, int p, Doblst xlst, Doblst ylst, Doblst slst,
      Doblst &a, Doblst &covar, double &chisq );
       /* -- model functions for 2nd derivative of a gaussian */
    static int dgauss_f (const gsl_vector * x, void *data, gsl_vector * f);
    static int dgauss_df (const gsl_vector * x, void *data, gsl_matrix * J);
    static int dgauss_fdf (const gsl_vector * x, void *data, gsl_vector * f, gsl_matrix * J);
    /* -- model functions for multiplet fit */
    /* ---- model functions for total multiplet */
    static int multiplet_f (const gsl_vector * x, void *data2, gsl_vector * f);
    static int multiplet_df (const gsl_vector * x, void *data2, gsl_matrix * J);
    static int multiplet_fdf (const gsl_vector * x, void *data2, gsl_vector * f, gsl_matrix * J);
    /* ---- model functions for base-line stripped multiplet (=gaussiand sum) */
    static int gaussSum_f (const gsl_vector * x, void *data, gsl_vector * f);
    static int gaussSum_df (const gsl_vector * x, void *data, gsl_matrix * J);
    static int gaussSum_fdf (const gsl_vector * x, void *data, gsl_vector * f, gsl_matrix * J);
       /* -- model functions for exponential + contant background */
    static int expb_f (const gsl_vector * x, void *data, gsl_vector * f);
    static int expb_df (const gsl_vector * x, void *data, gsl_matrix * J);
    static int expb_fdf (const gsl_vector * x, void *data, gsl_vector * f, gsl_matrix * J);
       /* -- model functions for ... */
    static int gensin_f (const gsl_vector * x, void *data, gsl_vector * f);
    static int gensin_df (const gsl_vector * x, void *data, gsl_matrix * J);
    static int gensin_fdf (const gsl_vector * x, void *data, gsl_vector * f, gsl_matrix * J);

    static int gensina0a4fix_f (const gsl_vector * x, void *data, gsl_vector * f);
    static int gensina0a4fix_df (const gsl_vector * x, void *data, gsl_matrix * J);
    static int gensina0a4fix_fdf (const gsl_vector * x, void *data, gsl_vector * f, gsl_matrix * J);

    static int test_f (const gsl_vector * x, void *data, gsl_vector * f);
    static int test_df (const gsl_vector * x, void *data, gsl_matrix * J);
    static int test_fdf (const gsl_vector * x, void *data, gsl_vector * f, gsl_matrix * J);

private:
    int m_n;
    int m_p;
    gsl_matrix* covar;
    int (*mf)(const gsl_vector*, void*, gsl_vector*);
    gsl_vector* x;
    gsl_vector* y;
    gsl_vector* sigma;
    size_t iter;
    int status;
    const gsl_multifit_fdfsolver_type *T;
    gsl_multifit_fdfsolver* s;
    QString rep;
    QString printState( int p );
    QString printJacobian(const int n, const int p);
};

#endif /*GSLCONTAINER_H_*/
