/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkSymmetricEigenAnalysis_hxx
#define itkSymmetricEigenAnalysis_hxx

#include "itkSymmetricEigenAnalysis.h"
#include "vnl/vnl_math.h"

namespace itk
{
template< typename TMatrix, typename TVector, typename TEigenMatrix >
unsigned int
SymmetricEigenAnalysis< TMatrix, TVector, TEigenMatrix >::ComputeEigenValues(const TMatrix  & A,
                                                                             TVector  & D) const
{
  double *workArea1 = new double[m_Dimension];

  // Copy the input matrix
  double *inputMatrix = new double[m_Dimension * m_Dimension];

  unsigned int k = 0;

  for ( unsigned int row = 0; row < m_Dimension; row++ )
    {
    for ( unsigned int col = 0; col < m_Dimension; col++ )
      {
      inputMatrix[k++] = A(row, col);
      }
    }

  ReduceToTridiagonalMatrix(inputMatrix, D, workArea1, workArea1);
  const unsigned int eigenErrIndex =
    ComputeEigenValuesUsingQL(D, workArea1);

  delete[] workArea1;
  delete[] inputMatrix;

  return eigenErrIndex; //index of eigen value that could not be computed
}

template< typename TMatrix, typename TVector, typename TEigenMatrix >
unsigned int
SymmetricEigenAnalysis< TMatrix, TVector, TEigenMatrix >::ComputeEigenValuesAndVectors(
  const TMatrix  & A,
  TVector        & EigenValues,
  TEigenMatrix   & EigenVectors) const
{
  double *workArea1 = new double[m_Dimension];
  double *workArea2 = new double[m_Dimension * m_Dimension];

  // Copy the input matrix
  double *inputMatrix = new double[m_Dimension * m_Dimension];

  unsigned int k = 0;

  for ( unsigned int row = 0; row < m_Dimension; row++ )
    {
    for ( unsigned int col = 0; col < m_Dimension; col++ )
      {
      inputMatrix[k++] = A(row, col);
      }
    }

  ReduceToTridiagonalMatrixAndGetTransformation(
    inputMatrix, EigenValues, workArea1, workArea2);
  const unsigned int eigenErrIndex =
    ComputeEigenValuesAndVectorsUsingQL(EigenValues, workArea1, workArea2);

  // Copy eigenVectors
  k = 0;
  for ( unsigned int row = 0; row < m_Dimension; row++ )
    {
    for ( unsigned int col = 0; col < m_Dimension; col++ )
      {
      EigenVectors[row][col] = workArea2[k++];
      }
    }

  delete[] workArea2;
  delete[] workArea1;
  delete[] inputMatrix;

  return eigenErrIndex; //index of eigen value that could not be computed
}

template< typename TMatrix, typename TVector, typename TEigenMatrix >
void
SymmetricEigenAnalysis< TMatrix, TVector, TEigenMatrix >::ReduceToTridiagonalMatrix(double *a, VectorType & d,
                                                                                    double *e, double *e2) const
{
  double d__1;

  /* Local variables */
  double f, g, h;
  int    i, j, k, l;
  double scale;

  for ( i = 0; i < static_cast< int >( m_Order ); ++i )
    {
    d[i] = a[m_Order - 1 + i * m_Dimension];
    a[m_Order - 1 + i * m_Dimension] = a[i + i * m_Dimension];
    }

  for ( i = m_Order - 1; i >= 0; --i )
    {
    l = i - 1;
    h = 0.;
    scale = 0.;

    /*     .......... scale row (algol tol then not needed) .......... */
    for ( k = 0; k <= l; ++k )
      {
      scale += vnl_math_abs(d[k]);
      }

    if ( scale == 0. )
      {
      for ( j = 0; j <= l; ++j )
        {
        d[j] = a[l + j * m_Dimension];
        a[l + j * m_Dimension] = a[i + j * m_Dimension];
        a[i + j * m_Dimension] = 0.;
        }
      e[i] = 0.;
      e2[i] = 0.;
      continue;
      }
    for ( k = 0; k <= l; ++k )
      {
      d[k] /= scale;
      h += d[k] * d[k];
      }

    e2[i] = scale * scale * h;
    f = d[l];
    d__1 = std::sqrt(h);
    g = ( -1.0 ) * vnl_math_sgn0(f) * vnl_math_abs(d__1);
    e[i] = scale * g;
    h -= f * g;
    d[l] = f - g;
    if ( l != 0 )
      {
      /*     .......... form a*u .......... */
      for ( j = 0; j <= l; ++j )
        {
        e[j] = 0.;
        }

      for ( j = 0; j <= l; ++j )
        {
        f = d[j];
        g = e[j] + a[j + j * m_Dimension] * f;

        for ( k = j + 1; k <= l; ++k )
          {
          g += a[k + j * m_Dimension] * d[k];
          e[k] += a[k + j * m_Dimension] * f;
          }
        e[j] = g;
        }

      /*     .......... form p .......... */
      f = 0.;

      for ( j = 0; j <= l; ++j )
        {
        e[j] /= h;
        f += e[j] * d[j];
        }

      h = f / ( h + h );
      /*     .......... form q .......... */
      for ( j = 0; j <= l; ++j )
        {
        e[j] -= h * d[j];
        }

      /*     .......... form reduced a .......... */
      for ( j = 0; j <= l; ++j )
        {
        f = d[j];
        g = e[j];

        for ( k = j; k <= l; ++k )
          {
          a[k + j * m_Dimension] = a[k + j * m_Dimension] - f * e[k] - g * d[k];
          }
        }
      }

    for ( j = 0; j <= l; ++j )
      {
      f = d[j];
      d[j] = a[l + j * m_Dimension];
      a[l + j * m_Dimension] = a[i + j * m_Dimension];
      a[i + j * m_Dimension] = f * scale;
      }
    }
}

template< typename TMatrix, typename TVector, typename TEigenMatrix >
void
SymmetricEigenAnalysis< TMatrix, TVector, TEigenMatrix >::ReduceToTridiagonalMatrixAndGetTransformation(double *a,
                                                                                                        VectorType & d,
                                                                                                        double *e,
                                                                                                        double *z)
const
{
  double d__1;

  /* Local variables */
  double       f, g, h;
  unsigned int i, j, k, l;
  double       scale, hh;

  for ( i = 0; i < m_Order; ++i )
    {
    for ( j = i; j < m_Order; ++j )
      {
      z[j + i * m_Dimension] = a[j + i * m_Dimension];
      }
    d[i] = a[m_Order - 1 + i * m_Dimension];
    }

  for ( i = m_Order - 1; i > 0; --i )
    {
    l = i - 1;
    h = 0.0;
    scale = 0.0;

    /*     .......... scale row (algol tol then not needed) .......... */
    for ( k = 0; k <= l; ++k )
      {
      scale += vnl_math_abs(d[k]);
      }

    if ( scale == 0.0 )
      {
      e[i] = d[l];

      for ( j = 0; j <= l; ++j )
        {
        d[j] = z[l + j * m_Dimension];
        z[i + j * m_Dimension] = 0.0;
        z[j + i * m_Dimension] = 0.0;
        }
      }
    else
      {
      for ( k = 0; k <= l; ++k )
        {
        d[k] /= scale;
        h += d[k] * d[k];
        }

      f = d[l];
      d__1 = std::sqrt(h);
      g = ( -1.0 ) * vnl_math_sgn0(f) * vnl_math_abs(d__1);
      e[i] = scale * g;
      h -= f * g;
      d[l] = f - g;

      /*     .......... form a*u .......... */
      for ( j = 0; j <= l; ++j )
        {
        e[j] = 0.0;
        }

      for ( j = 0; j <= l; ++j )
        {
        f = d[j];
        z[j + i * m_Dimension] = f;
        g = e[j] + z[j + j * m_Dimension] * f;

        for ( k = j + 1; k <= l; ++k )
          {
          g += z[k + j * m_Dimension] * d[k];
          e[k] += z[k + j * m_Dimension] * f;
          }

        e[j] = g;
        }

      /*     .......... form p .......... */
      f = 0.0;

      for ( j = 0; j <= l; ++j )
        {
        e[j] /= h;
        f += e[j] * d[j];
        }

      hh = f / ( h + h );

      /*     .......... form q .......... */
      for ( j = 0; j <= l; ++j )
        {
        e[j] -= hh * d[j];
        }

      /*     .......... form reduced a .......... */
      for ( j = 0; j <= l; ++j )
        {
        f = d[j];
        g = e[j];

        for ( k = j; k <= l; ++k )
          {
          z[k + j * m_Dimension] = z[k + j * m_Dimension] - f * e[k] - g * d[k];
          }

        d[j] = z[l + j * m_Dimension];
        z[i + j * m_Dimension] = 0.0;
        }
      }

    d[i] = h;
    }

  /*     .......... accumulation of transformation matrices .......... */
  for ( i = 1; i < m_Order; ++i )
    {
    l = i - 1;
    z[m_Order - 1 + l * m_Dimension] = z[l + l * m_Dimension];
    z[l + l * m_Dimension] = 1.0;
    h = d[i];
    if ( h != 0.0 )
      {
      for ( k = 0; k <= l; ++k )
        {
        d[k] = z[k + i * m_Dimension] / h;
        }

      for ( j = 0; j <= l; ++j )
        {
        g = 0.0;

        for ( k = 0; k <= l; ++k )
          {
          g += z[k + i * m_Dimension] * z[k + j * m_Dimension];
          }

        for ( k = 0; k <= l; ++k )
          {
          z[k + j * m_Dimension] -= g * d[k];
          }
        }
      }

    for ( k = 0; k <= l; ++k )
      {
      z[k + i * m_Dimension] = 0.0;
      }
    }

  for ( i = 0; i < m_Order; ++i )
    {
    d[i] = z[m_Order - 1 + i * m_Dimension];
    z[m_Order - 1 + i * m_Dimension] = 0.0;
    }

  z[m_Order - 1 + ( m_Order - 1 ) * m_Dimension] = 1.0;
  e[0] = 0.0;
}

template< typename TMatrix, typename TVector, typename TEigenMatrix >
unsigned int
SymmetricEigenAnalysis< TMatrix, TVector, TEigenMatrix >::ComputeEigenValuesUsingQL(VectorType & d, double *e) const
{
  const double c_b10 = 1.0;

  /* Local variables */
  double       c, f, g, h;
  unsigned int i, j, l, m;
  double       p, r, s, c2, c3 = 0.0;
  double       s2 = 0.0;
  double       dl1, el1;
  double       tst1, tst2;

  unsigned int ierr = 0;

  if ( m_Order == 1 )
    {
    return 1;
    }

  for ( i = 1; i < m_Order; ++i )
    {
    e[i - 1] = e[i];
    }

  f = 0.;
  tst1 = 0.;
  e[m_Order - 1] = 0.;

  for ( l = 0; l < m_Order; ++l )
    {
    j = 0;
    h = vnl_math_abs(d[l]) + vnl_math_abs(e[l]);
    if ( tst1 < h )
      {
      tst1 = h;
      }
    /*     .......... look for small sub-diagonal element .......... */
    for ( m = l; m < m_Order - 1; ++m )
      {
      const double abs_e_m = vnl_math_abs(e[m]);
      tst2 = tst1 + abs_e_m;
      if ( ! ( abs_e_m  > 0 ) )
        {
        break;
        }
      /*     .......... e(n) is always zero, so there is no exit */
      /*                through the bottom of the loop .......... */
      }

    if ( m != l )
      {
      do
        {
        if ( j == 30 )
          {
          /*     .......... set error -- no convergence to an */
          /*                eigenvalue after 30 iterations .......... */
          ierr = l + 1;
          return ierr;
          }
        ++j;
        /*     .......... form shift .......... */
        g = d[l];
        p = ( d[l + 1] - g ) / ( e[l] * 2. );
        r = vnl_math_hypot(p, c_b10);
        d[l] = e[l] / ( p + vnl_math_sgn0(p) * vnl_math_abs(r) );
        d[l + 1] = e[l] * ( p + vnl_math_sgn0(p) * vnl_math_abs(r) );
        dl1 = d[l + 1];
        h = g - d[l];

        for ( i = l + 2; i < m_Order; ++i )
          {
          d[i] -= h;
          }

        f += h;
        /*     .......... ql transformation .......... */
        p = d[m];
        c = 1.;
        c2 = c;
        el1 = e[l + 1];
        s = 0.;
        for ( i = m - 1; i >= l; --i )
          {
          c3 = c2;
          c2 = c;
          s2 = s;
          g = c * e[i];
          h = c * p;
          r = vnl_math_hypot(p, e[i]);
          e[i + 1] = s * r;
          s = e[i] / r;
          c = p / r;
          p = c * d[i] - s * g;
          d[i + 1] = h + s * ( c * g + s * d[i] );
          if ( i == l )
            {
            break;
            }
          }

        p = -s * s2 * c3 * el1 * e[l] / dl1;
        e[l] = s * p;
        d[l] = c * p;
        tst2 = tst1 + vnl_math_abs(e[l]);
        }
      while ( tst2 > tst1 );
      }

    p = d[l] + f;

    if ( m_OrderEigenValues == OrderByValue )
      {
      // Order by value
      for ( i = l; i > 0; --i )
        {
        if ( p >= d[i - 1] )
          {
          break;
          }
        d[i] = d[i - 1];
        }
      d[i] = p;
      }
    else if ( m_OrderEigenValues == OrderByMagnitude )
      {
      // Order by magnitude.. make eigen values positive
      for ( i = l; i > 0; --i )
        {
        if ( vnl_math_abs(p) >= vnl_math_abs(d[i - 1]) )
          {
          break;
          }
        d[i] = d[i - 1];
        }
      d[i] = p;
      }
    else
      {
      d[l] = p;
      }
    }

  return ierr;    //ierr'th eigen value that couldn't be computed
}

template< typename TMatrix, typename TVector, typename TEigenMatrix >
unsigned int
SymmetricEigenAnalysis< TMatrix, TVector, TEigenMatrix >::ComputeEigenValuesAndVectorsUsingQL(VectorType & d,
                                                                                              double *e,
                                                                                              double *z) const
{
  const double c_b10 = 1.0;

  /* Local variables */
  double       c, f, g, h;
  unsigned int i, j, k, l, m;
  double       p, r, s, c2, c3 = 0.0;
  double       s2 = 0.0;
  double       dl1, el1;
  double       tst1, tst2;

  unsigned int ierr = 0;

  if ( m_Order == 1 )
    {
    return 1;
    }

  for ( i = 1; i < m_Order; ++i )
    {
    e[i - 1] = e[i];
    }

  f = 0.0;
  tst1 = 0.;
  e[m_Order - 1] = 0.;

  for ( l = 0; l < m_Order; ++l )
    {
    j = 0;
    h = vnl_math_abs(d[l]) + vnl_math_abs(e[l]);
    if ( tst1 < h )
      {
      tst1 = h;
      }

    /*     .......... look for small sub-diagonal element .......... */
    for ( m = l; m < m_Order - 1; ++m )
      {
      const double abs_e_m = vnl_math_abs(e[m]);
      tst2 = tst1 + abs_e_m;
      if ( ! (abs_e_m > 0 ) )
        {
        break;
        }

      /*     .......... e(n) is always zero, so there is no exit */
      /*                through the bottom of the loop .......... */
      }

    if ( m != l )
      {
      do
        {
        if ( j == 30 )
          {
          /*     .......... set error -- no convergence to an */
          /*                eigenvalue after 30 iterations .......... */
          ierr = l + 1;
          return ierr;
          }
        ++j;
        /*     .......... form shift .......... */
        g = d[l];
        p = ( d[l + 1] - g ) / ( e[l] * 2. );
        r = vnl_math_hypot(p, c_b10);
        d[l] = e[l] / ( p + vnl_math_sgn0(p) * vnl_math_abs(r) );
        d[l + 1] = e[l] * ( p + vnl_math_sgn0(p) * vnl_math_abs(r) );
        dl1 = d[l + 1];
        h = g - d[l];

        for ( i = l + 2; i < m_Order; ++i )
          {
          d[i] -= h;
          }

        f += h;
        /*     .......... ql transformation .......... */
        p = d[m];
        c = 1.0;
        c2 = c;
        el1 = e[l + 1];
        s = 0.;

        for ( i = m - 1; i >= l; --i )
          {
          c3 = c2;
          c2 = c;
          s2 = s;
          g = c * e[i];
          h = c * p;
          r = vnl_math_hypot(p, e[i]);
          e[i + 1] = s * r;
          s = e[i] / r;
          c = p / r;
          p = c * d[i] - s * g;
          d[i + 1] = h + s * ( c * g + s * d[i] );

          /*     .......... form vector .......... */
          for ( k = 0; k < m_Order; ++k )
            {
            h = z[k + ( i + 1 ) * m_Dimension];
            z[k + ( i + 1 ) * m_Dimension] = s * z[k + i * m_Dimension] + c * h;
            z[k + i * m_Dimension] = c * z[k + i * m_Dimension] - s * h;
            }
          if ( i == l )
            {
            break;
            }
          }

        p = -s * s2 * c3 * el1 * e[l] / dl1;
        e[l] = s * p;
        d[l] = c * p;
        tst2 = tst1 + vnl_math_abs(e[l]);
        }
      while ( tst2 > tst1 );
      }

    d[l] += f;
    }

  /*     .......... order eigenvalues and eigenvectors .......... */
  if ( m_OrderEigenValues == OrderByValue )
    {
    // Order by value
    for ( i = 0; i < m_Order - 1; ++i )
      {
      k = i;
      p = d[i];

      for ( j = i + 1; j < m_Order; ++j )
        {
        if ( d[j] >= p )
          {
          continue;
          }
        k = j;
        p = d[j];
        }

      if ( k == i )
        {
        continue;
        }
      d[k] = d[i];
      d[i] = p;

      for ( j = 0; j < m_Order; ++j )
        {
        p = z[j + i * m_Dimension];
        z[j + i * m_Dimension] = z[j + k * m_Dimension];
        z[j + k * m_Dimension] = p;
        }
      }
    }
  else if ( m_OrderEigenValues == OrderByMagnitude )
    {
    // Order by magnitude
    for ( i = 0; i < m_Order - 1; ++i )
      {
      k = i;
      p = d[i];

      for ( j = i + 1; j < m_Order; ++j )
        {
        if ( vnl_math_abs(d[j]) >= vnl_math_abs(p) )
          {
          continue;
          }
        k = j;
        p = d[j];
        }

      if ( k == i )
        {
        continue;
        }

      d[k] = d[i];
      d[i] = p;

      for ( j = 0; j < m_Order; ++j )
        {
        p = z[j + i * m_Dimension];
        z[j + i * m_Dimension] = z[j + k * m_Dimension];
        z[j + k * m_Dimension] = p;
        }
      }
    }

  return ierr;
}
}  // end namespace itk

#endif
