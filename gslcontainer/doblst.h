#ifndef DOBLST_H
#define DOBLST_H

#include <qmath.h>
#include <QList>

//!  Lista de n&uacute;meros inteiros.
/*!
  Lista de n&uacute;meros inteiros com c&aacute;lculo de m&aacute;ximo, m&iacute;nimo e soma.
*/

class Intlst : public QList<int>
{
public:
    //! m&iacute;nimo da lista.
    /*!
      \sa min()
    */
    int min()
    {
        int n = size();
        int m = 0;
        if (n > 0) {
            m = at(0);
            for (int i=1; i < n; i++)
                if (at(i) < m) m = at(i);
        }
        return m;
    }
    //! m&aacute;ximo da lista.
    /*!
      \sa max()
    */
    int max()
    {
        int n = size();
        qreal m = 0;
        if (n > 0) {
            m = at(0);
            for (int i=1; i < n; i++)
                if (at(i) > m) m = at(i);
        }
        return m;
    }
    //! soma dos n&uacute;meros da lista.
    /*!
      \sa sum()
    */
    int sum()
    {
        int n = size();
        qreal m = 0;
        for (int i=0; i < n; i++)  m += at(i);
        return m;
    }
};

class Doblst : public QList<qreal>
{
public:
    //! m&iacute;nimo da lista.
    /*!
      \sa min()
    */
    qreal min()
    {
        int n = size();
        qreal x = 0.0;
        if (n > 0) {
            x = at(0);
            for (int i=1; i < n; i++)
                if (at(i) < x) x = at(i);
        }
        return x;
    }
    //! m&aacute;ximo da lista.
    /*!
      \sa max()
    */
    qreal max()
    {
        int n = size();
        qreal x = 0.0;
        if (n > 0) {
            x = at(0);
            for (int i=1; i < n; i++)
                if (at(i) > x) x = at(i);
        }
        return x;
    }
    //! soma dos n&uacute;meros da lista.
    /*!
      \sa sum()
    */
    qreal sum()
    {
        int n = size();
        qreal x = 0.0;
        for (int i=0; i < n; i++)  x += at(i);
        return x;
    }
    //! m&eacute;dia dos n&uacute;meros da lista.
    /*!
      \sa sum()
    */
    qreal mean()
    {
        int n = size();
        return sum() / n;
    }
};

#endif // DOBLST_H
