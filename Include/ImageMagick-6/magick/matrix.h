/*
  Copyright 1999-2012 ImageMagick Studio LLC, a non-profit organization
  dedicated to making software imaging solutions freely available.
  
  You may not use this file except in compliance with the License.
  obtain a copy of the License at
  
    http://www.imagemagick.org/script/license.php
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  MagickCore graphic resample methods.
*/
#ifndef _MAGICKCORE_MATRIX_H
#define _MAGICKCORE_MATRIX_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport double
  **AcquireMagickMatrix(const size_t,const size_t),
  **RelinquishMagickMatrix(double **,const size_t);

extern MagickExport MagickBooleanType
  GaussJordanElimination(double **,double **,const size_t,const size_t);

extern MagickExport void
  LeastSquaresAddTerms(double **,double **,const double *,const double *,
    const size_t, const size_t);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
