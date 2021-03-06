/*
 * STRUMPACK -- STRUctured Matrices PACKage, Copyright (c) 2014, The Regents of
 * the University of California, through Lawrence Berkeley National Laboratory
 * (subject to receipt of any required approvals from the U.S. Dept. of Energy).
 * All rights reserved.
 *
 * If you have questions about your rights to use or distribute this software,
 * please contact Berkeley Lab's Technology Transfer Department at TTD@lbl.gov.
 *
 * NOTICE. This software is owned by the U.S. Department of Energy. As such, the
 * U.S. Government has been granted for itself and others acting on its behalf a
 * paid-up, nonexclusive, irrevocable, worldwide license in the Software to
 * reproduce, prepare derivative works, and perform publicly and display publicly.
 * Beginning five (5) years after the date permission to assert copyright is
 * obtained from the U.S. Department of Energy, and subject to any subsequent five
 * (5) year renewals, the U.S. Government is granted for itself and others acting
 * on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the
 * Software to reproduce, prepare derivative works, distribute copies to the
 * public, perform publicly and display publicly, and to permit others to do so.
 *
 * Developers: Pieter Ghysels, Francois-Henry Rouet, Xiaoye S. Li.
 *             (Lawrence Berkeley National Lab, Computational Research Division).
 *
 */
#ifndef BLASLAPACKOMPTASK_HPP
#define BLASLAPACKOMPTASK_HPP
#include <iostream>
#include <stdlib.h>
#include "blas_lapack_wrapper.hpp"
#include "strumpack_parameters.hpp"
#include "strumpack_config.h"

namespace strumpack {

  const int OMPTileSize = 64;
  const int OMPThreshold = OMPTileSize*OMPTileSize*OMPTileSize;
  const int gemmOMPThreshold = OMPThreshold;
  const int trsmOMPThreshold = OMPThreshold;
  const int trsvOMPThreshold = OMPTileSize*OMPTileSize;
  const int getrfOMPPanelWidth = 1;
  const int OMPPanelWidth = 8;

#define SGEMM_OMP_TASK_FC FC_GLOBAL_(sgemm_omp_task, SGEMM_OMP_TASK)
#define DGEMM_OMP_TASK_FC FC_GLOBAL_(dgemm_omp_task, DGEMM_OMP_TASK)
#define CGEMM_OMP_TASK_FC FC_GLOBAL_(cgemm_omp_task, CGEMM_OMP_TASK)
#define ZGEMM_OMP_TASK_FC FC_GLOBAL_(zgemm_omp_task, ZGEMM_OMP_TASK)

#define STRSM_OMP_TASK_FC FC_GLOBAL_(strsm_omp_task, STRSM_OMP_TASK)
#define DTRSM_OMP_TASK_FC FC_GLOBAL_(dtrsm_omp_task, DTRSM_OMP_TASK)
#define CTRSM_OMP_TASK_FC FC_GLOBAL_(ctrsm_omp_task, CTRSM_OMP_TASK)
#define ZTRSM_OMP_TASK_FC FC_GLOBAL_(ztrsm_omp_task, ZTRSM_OMP_TASK)

#define STRMM_OMP_TASK_FC FC_GLOBAL_(strmm_omp_task, STRMM_OMP_TASK)
#define DTRMM_OMP_TASK_FC FC_GLOBAL_(dtrmm_omp_task, DTRMM_OMP_TASK)
#define CTRMM_OMP_TASK_FC FC_GLOBAL_(ctrmm_omp_task, CTRMM_OMP_TASK)
#define ZTRMM_OMP_TASK_FC FC_GLOBAL_(ztrmm_omp_task, ZTRMM_OMP_TASK)

#define STRMV_OMP_TASK_FC FC_GLOBAL_(strmv_omp_task, STRMV_OMP_TASK)
#define DTRMV_OMP_TASK_FC FC_GLOBAL_(dtrmv_omp_task, DTRMV_OMP_TASK)
#define CTRMV_OMP_TASK_FC FC_GLOBAL_(ctrmv_omp_task, CTRMV_OMP_TASK)
#define ZTRMV_OMP_TASK_FC FC_GLOBAL_(ztrmv_omp_task, ZTRMV_OMP_TASK)

#define SGER_OMP_TASK_FC FC_GLOBAL_(sger_omp_task, SGER_OMP_TASK)
#define DGER_OMP_TASK_FC FC_GLOBAL_(dger_omp_task, DGER_OMP_TASK)
#define CGERU_OMP_TASK_FC FC_GLOBAL_(cgeru_omp_task, CGERU_OMP_TASK)
#define ZGERU_OMP_TASK_FC FC_GLOBAL_(zgeru_omp_task, ZGERU_OMP_TASK)
#define CGERC_OMP_TASK_FC FC_GLOBAL_(cgerc_omp_task, CGERC_OMP_TASK)
#define ZGERC_OMP_TASK_FC FC_GLOBAL_(zgerc_omp_task, ZGERC_OMP_TASK)

#define SLASWP_OMP_TASK_FC FC_GLOBAL_(slaswp_omp_task, SLASWP_OMP_TASK)
#define DLASWP_OMP_TASK_FC FC_GLOBAL_(dlaswp_omp_task, DLASWP_OMP_TASK)
#define CLASWP_OMP_TASK_FC FC_GLOBAL_(claswp_omp_task, CLASWP_OMP_TASK)
#define ZLASWP_OMP_TASK_FC FC_GLOBAL_(zlaswp_omp_task, ZLASWP_OMP_TASK)

#define SGEMV_OMP_TASK_FC FC_GLOBAL_(sgemv_omp_task, SGEMV_OMP_TASK)
#define DGEMV_OMP_TASK_FC FC_GLOBAL_(dgemv_omp_task, DGEMV_OMP_TASK)
#define CGEMV_OMP_TASK_FC FC_GLOBAL_(cgemv_omp_task, CGEMV_OMP_TASK)
#define ZGEMV_OMP_TASK_FC FC_GLOBAL_(zgemv_omp_task, ZGEMV_OMP_TASK)

  extern "C" {
    void SGEMM_OMP_TASK_FC(char* transa, char* transb, int* m, int* n, int* k, float* alpha, float* a, int* lda, float* b, int* ldb, float* beta, float* c, int* ldc, int* depth);
    void DGEMM_OMP_TASK_FC(char* transa, char* transb, int* m, int* n, int* k, double* alpha, double* a, int* lda, double* b, int* ldb, double* beta, double* c, int* ldc, int* depth);
    void CGEMM_OMP_TASK_FC(char* transa, char* transb, int* m, int* n, int* k, c_float* alpha, c_float* a, int* lda, c_float* b, int* ldb, c_float* beta, c_float* c, int* ldc, int* depth);
    void ZGEMM_OMP_TASK_FC(char* transa, char* transb, int* m, int* n, int* k, c_double* alpha, c_double* a, int* lda, c_double* b, int* ldb, c_double* beta, c_double* c, int* ldc, int* depth);

    void STRSM_OMP_TASK_FC(char* side, char* uplo, char* transa, char* diag, int* M, int* N, float* alpha, float* A, int* lda, float* B, int* ldb, int* depth);
    void DTRSM_OMP_TASK_FC(char* side, char* uplo, char* transa, char* diag, int* M, int* N, double* alpha, double* A, int* lda, double* B, int* ldb, int* depth);
    void CTRSM_OMP_TASK_FC(char* side, char* uplo, char* transa, char* diag, int* M, int* N, c_float* alpha, c_float* A, int* lda, c_float* B, int* ldb, int* depth);
    void ZTRSM_OMP_TASK_FC(char* side, char* uplo, char* transa, char* diag, int* M, int* N, c_double* alpha, c_double* A, int* lda, c_double* B, int* ldb, int* depth);

    void STRMM_OMP_TASK_FC(char* side, char* uplo, char* transa, char* diag, int* M, int* N, float* alpha, float* A, int* lda, float* B, int* ldb, int* depth);
    void DTRMM_OMP_TASK_FC(char* side, char* uplo, char* transa, char* diag, int* M, int* N, double* alpha, double* A, int* lda, double* B, int* ldb, int* depth);
    void CTRMM_OMP_TASK_FC(char* side, char* uplo, char* transa, char* diag, int* M, int* N, c_float* alpha, c_float* A, int* lda, c_float* B, int* ldb, int* depth);
    void ZTRMM_OMP_TASK_FC(char* side, char* uplo, char* transa, char* diag, int* M, int* N, c_double* alpha, c_double* A, int* lda, c_double* B, int* ldb, int* depth);

    void STRMV_OMP_TASK_FC(char* uplo, char* transa, char* diag, int* N, float* A, int* lda, float* X, int* incx, int* depth);
    void DTRMV_OMP_TASK_FC(char* uplo, char* transa, char* diag, int* N, double* A, int* lda, double* X, int* incx, int* depth);
    void CTRMV_OMP_TASK_FC(char* uplo, char* transa, char* diag, int* N, c_float* A, int* lda, c_float* X, int* incx, int* depth);
    void ZTRMV_OMP_TASK_FC(char* uplo, char* transa, char* diag, int* N, c_double* A, int* lda, c_double* X, int* incx, int* depth);

    void SGER_OMP_TASK_FC(int* m, int* n, float* alpha, float* x, int* incx, float* y, int* incy, float* a, int* lda, int* depth);
    void DGER_OMP_TASK_FC(int* m, int* n, double* alpha, double* x, int* incx, double* y, int* incy, double* a, int* lda, int* depth);
    void CGERU_OMP_TASK_FC(int* m, int* n, c_float* alpha, c_float* x, int* incx, c_float* y, int* incy, c_float* a, int* lda, int* depth);
    void ZGERU_OMP_TASK_FC(int* m, int* n, c_double* alpha, c_double* x, int* incx, c_double* y, int* incy, c_double* a, int* lda, int* depth);
    void CGERC_OMP_TASK_FC(int* m, int* n, c_float* alpha, c_float* x, int* incx, c_float* y, int* incy, c_float* a, int* lda, int* depth);
    void ZGERC_OMP_TASK_FC(int* m, int* n, c_double* alpha, c_double* x, int* incx, c_double* y, int* incy, c_double* a, int* lda, int* depth);

    void SLASWP_OMP_TASK_FC(int* N, float* A, int* lda, int* k1, int* k2, int* ipiv, int* incx, int* depth);
    void DLASWP_OMP_TASK_FC(int* N, double* A, int* lda, int* k1, int* k2, int* ipiv, int* incx, int* depth);
    void CLASWP_OMP_TASK_FC(int* N, c_float* A, int* lda, int* k1, int* k2, int* ipiv, int* incx, int* depth);
    void ZLASWP_OMP_TASK_FC(int* N, c_double* A, int* lda, int* k1, int* k2, int* ipiv, int* incx, int* depth);

    void SGEMV_OMP_TASK_FC(char* trans, int* M, int* N, float* alpha, float *A, int* lda, float* X, int* incx, float* beta, float* Y, int* incy, int* depth);
    void DGEMV_OMP_TASK_FC(char* trans, int* M, int* N, double* alpha, double *A, int* lda, double* X, int* incx, double* beta, double* Y, int* incy, int* depth);
    void CGEMV_OMP_TASK_FC(char* trans, int* M, int* N, c_float* alpha, c_float *A, int* lda, c_float* X, int* incx, c_float* beta, c_float* Y, int* incy, int* depth);
    void ZGEMV_OMP_TASK_FC(char* trans, int* M, int* N, c_double* alpha, c_double *A, int* lda, c_double* X, int* incx, c_double* beta, c_double* Y, int* incy, int* depth);
  }

  template<typename scalar> void gemm_omp_task(char transa, char transb, int m, int n, int k, scalar alpha, scalar* a, int lda, scalar* b, int ldb, scalar beta, scalar* c, int ldc, int depth) {
    if (depth>=params::task_recursion_cutoff_level || double(m)*n*k <= gemmOMPThreshold)
      blas::gemm(transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc);
    else {
      bool opA = transa=='T'||transa=='t'||transa=='C'||transa=='c';
      bool opB = transb=='T'||transb=='t'||transb=='C'||transb=='c';
      if (n >= std::max(m,k)) {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	gemm_omp_task(transa, transb, m, n/2, k, alpha, a, lda, b, ldb, beta, c, ldc, depth+1);                                                            // C0 = alpha * A*B0 + beta * C0
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	gemm_omp_task(transa, transb, m, n-n/2, k, alpha, a, lda, opB ? b+n/2 : b+(n/2)*ldb, ldb, beta, c+(n/2)*ldc, ldc, depth+1);                        // C1 = alpha * A*B1 + beta * C1
#pragma omp taskwait
      } else if (m >= k) {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	gemm_omp_task(transa, transb, m/2, n, k, alpha, a, lda, b, ldb, beta, c, ldc, depth+1);                                                            // C0 = alpha * A0*B + beta * C0
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	gemm_omp_task(transa, transb, m-m/2, n, k, alpha, opA ? a+(m/2)*lda : a+m/2, lda, b, ldb, beta, c+m/2, ldc, depth+1);                              // C1 = alpha * A1*B + beta * C1
#pragma omp taskwait
      } else {
	gemm_omp_task(transa, transb, m, n, k/2, alpha, a, lda, b, ldb, beta, c, ldc, depth);                                                         // C  = alpha * A0*B0 + beta * C
	gemm_omp_task(transa, transb, m, n, k-k/2, alpha, opA ? a+k/2 : a+(k/2)*lda, lda, opB ? b+(k/2)*ldb : b+k/2, ldb, scalar(1.), c, ldc, depth); // C += alpha * A1*B1
      }
    }
  }

  template<typename scalar> void gemv_omp_task(char trans, int m, int n, scalar alpha, scalar *a, int lda, scalar *x, int incx, scalar beta, scalar *y, int incy, int depth) {
    if (depth>=params::task_recursion_cutoff_level || 2*double(m)*n <= OMPTileSize*OMPTileSize/*OMPThreshold*/)
      blas::gemv(trans, m, n, alpha, a, lda, x, incx, beta, y, incy);
    else {
      if (trans=='T' || trans=='t' || trans=='C' || trans=='c') {
	if (n >= m) {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  gemv_omp_task(trans, m, n/2, alpha, a, lda, x, incx, beta, y, incy, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  gemv_omp_task(trans, m, n-n/2, alpha, a+(n/2)*lda, lda, x, incx, beta, y+(n/2)*incy, incy, depth+1);
#pragma omp taskwait
	} else {
	  if (n <= /*OMPTileSize*/64) {
	    scalar tmp[/*OMPTileSize*/64];
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	    gemv_omp_task(trans, m/2, n, alpha, a, lda, x, incx, beta, y, incy, depth+1);
#pragma omp task shared(tmp) final(depth >= params::task_recursion_cutoff_level-1) mergeable
	    gemv_omp_task(trans, m-m/2, n, alpha, a+m/2, lda, x+(m/2)*incx, incx, scalar(0.), tmp, 1, depth+1);
#pragma omp taskwait
	    for (int i=0; i<n; i++) y[i*incy] += tmp[i];
	  } else {
	    gemv_omp_task(trans, m/2, n, alpha, a, lda, x, incx, beta, y, incy, depth);
	    gemv_omp_task(trans, m-m/2, n, alpha, a+m/2, lda, x+(m/2)*incx, incx, scalar(1.), y, incy, depth);
	  }
	}
      } else if (trans=='N' || trans=='n') {
	if (m >= n) {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  gemv_omp_task(trans, m/2, n, alpha, a, lda, x, incx, beta, y, incy, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  gemv_omp_task(trans, m-m/2, n, alpha, a+m/2, lda, x, incx, beta, y+(m/2)*incy, incy, depth+1);
#pragma omp taskwait
	} else {
	  if (m <= /*OMPTileSize*/64) {
	    scalar tmp[/*OMPTileSize*/64];
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	    gemv_omp_task(trans, m, n/2, alpha, a, lda, x, incx, beta, y, incy, depth+1);
#pragma omp task shared(tmp) final(depth >= params::task_recursion_cutoff_level-1) mergeable
	    gemv_omp_task(trans, m, n-n/2, alpha, a+(n/2)*lda, lda, x+(n/2)*incx, incx, scalar(0.), tmp, 1, depth+1);
#pragma omp taskwait
	    for (int i=0; i<m; i++) y[i*incy] += tmp[i];
	  } else {
	    gemv_omp_task(trans, m, n/2, alpha, a, lda, x, incx, beta, y, incy, depth);
	    gemv_omp_task(trans, m, n-n/2, alpha, a+(n/2)*lda, lda, x+(n/2)*incx, incx, scalar(1.), y, incy, depth);
	  }
	}
      }
    }
  }

#if 0 //_OPENMP >= 201307

  template<typename scalar> void trsv_omp_task(char uplo, char transa, char diag, int n, scalar* a, int lda, scalar* x, int incx, int depth) {
    if (depth>=params::task_recursion_cutoff_level || double(n) <= OMPTileSize)
      blas::trsv(uplo, transa, diag, n, a, lda, x, incx);
    else {
      if ((uplo=='L' || uplo=='l') && (transa=='N' || transa=='n')) {
#pragma omp taskgroup
	{
	  for (int i=0; i<n; i+=OMPTileSize) {
#pragma omp task firstprivate(i) default(shared) depend(inout:x[i])
	    blas::trsv(uplo, transa, diag, std::min(OMPTileSize, n-i), a+i+i*lda, lda, x+i*incx, incx);
	    for (int j=i+OMPTileSize; j<n; j+=OMPTileSize)
#pragma omp task firstprivate(i,j) default(shared) depend(in:x[i]) depend(inout:x[j])
	      blas::gemv(transa, std::min(OMPTileSize, n-j), OMPTileSize, scalar(-1.), a+j+i*lda, lda, x+i*incx, 1, scalar(1.), x+j*incx, incx);
	  }
	}
      } else if ((uplo=='U' || uplo=='u') && (transa=='N' || transa=='n')) {
#pragma omp taskgroup
	{
	  for (int i=n-n%OMPTileSize; i>=0; i-=OMPTileSize) {
#pragma omp task firstprivate(i) default(shared) depend(inout:x[i])
	    blas::trsv(uplo, transa, diag, std::min(OMPTileSize, n-i), a+i+i*lda, lda, x+i*incx, incx);
	    for (int j=i-OMPTileSize; j>=0; j-=OMPTileSize)
#pragma omp task firstprivate(i,j) default(shared) depend(in:x[i]) depend(inout:x[j])
	      blas::gemv(transa, OMPTileSize, std::min(OMPTileSize, n-i), scalar(-1.), a+j+i*lda, lda, x+i*incx, 1, scalar(1.), x+j*incx, incx);
	  }
	}
      } else {
	std::cerr << "trsv_omp_task not implemented with this combination of side, uplo and trans" << std::endl;
	abort();
      }
    }
  }

#else

  template<typename scalar> void trsv_omp_task(char uplo, char transa, char diag, int n, scalar* a, int lda, scalar* x, int incx, int depth) {
    if (depth>=params::task_recursion_cutoff_level || double(n)*n <= OMPTileSize*OMPTileSize/*OMPThreshold*/)
      blas::trsv(uplo, transa, diag, n, a, lda, x, incx);
    else {
      if ((uplo=='L' || uplo=='l') && (transa=='N' || transa=='n')) {
	trsv_omp_task(uplo, transa, diag, n/2, a, lda, x, incx, depth);        // A00*X00 = alpha * B00
	gemv_omp_task(transa, n-n/2, n/2, scalar(-1.), a+n/2, lda, x, incx, scalar(1.), x+(n/2)*incx, incx, depth);                   // B10 = alpha * B10 - A10*B00
	trsv_omp_task(uplo, transa, diag, n-n/2, a+n/2+(n/2)*lda, lda, x+(n/2)*incx, incx, depth);         // A11*X10 = B10
      } else if ((uplo=='U' || uplo=='u') && (transa=='N' || transa=='n')) {
	trsv_omp_task(uplo, transa, diag, n-n/2, a+n/2+(n/2)*lda, lda, x+(n/2)*incx, incx, depth);          // A11*X10 = B10
	gemv_omp_task(transa, n/2, n-n/2, scalar(-1.), a+n/2*lda, lda, x+(n/2)*incx, incx, scalar(1.), x, incx, depth);                   // B00 = alpha * B00 - A01*B10
	trsv_omp_task(uplo, transa, diag, n/2, a, lda, x, incx, depth);          // A00*X00 = alpha * B00
      } else {
	std::cerr << "trsv_omp_task not implemented with this combination of side, uplo and trans" << std::endl;
	abort();
      }
    }
  }
#endif

  template<typename scalar> void trmm_omp_task(char side, char uplo, char transa, char diag, int m, int n, scalar alpha, scalar* a, int lda, scalar* b, int ldb, int depth) {
    if (m == 0 || n == 0) return;
    if (depth>=params::task_recursion_cutoff_level || double(m)*n*n <= OMPThreshold)
      blas::trmm(side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb);
    else {
      if (side == 'L' || side == 'l') {
	if (n >= m) {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  trmm_omp_task(side, uplo, transa, diag, m, n/2, alpha, a, lda, b, ldb, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  trmm_omp_task(side, uplo, transa, diag, m, n-n/2, alpha, a, lda, b+(n/2)*ldb, ldb, depth+1);
#pragma omp taskwait
	} else {
	  bool opA = transa=='T'||transa=='t'||transa=='C'||transa=='c';
	  if (uplo == 'U' || uplo == 'u') {
	    if (opA) {
	      trmm_omp_task(side, uplo, transa, diag, m-m/2, m-m/2, alpha, a+m/2+(m/2)*lda, lda, b+m/2, ldb, depth);
	      gemm_omp_task(transa, 'N', m-m/2, n, m/2, alpha, a+(m/2)*lda, lda, b, ldb, scalar(1.), b+m/2, ldb, depth);
	      trmm_omp_task(side, uplo, transa, diag, m/2, m/2, alpha, a, lda, b, ldb, depth);
	    } else {
	      trmm_omp_task(side, uplo, transa, diag, m/2, m/2, alpha, a, lda, b, ldb, depth);
	      gemm_omp_task(transa, 'N', m/2, n, m-m/2, alpha, a+(m/2)*lda, lda, b+m/2, ldb, scalar(1.), b, ldb, depth);
	      trmm_omp_task(side, uplo, transa, diag, m-m/2, m-m/2, alpha, a+m/2+(m/2)*lda, lda, b+m/2, ldb, depth);
	    }
	  } else {
	    if (opA) {
	      trmm_omp_task(side, uplo, transa, diag, m/2, m/2, alpha, a, lda, b, ldb, depth);
	      gemm_omp_task(transa, 'N', m/2, n, m-m/2, alpha, a+m/2, lda, b+m/2, ldb, scalar(1.), b, ldb, depth);
	      trmm_omp_task(side, uplo, transa, diag, m-m/2, m-m/2, alpha, a+m/2+(m/2)*lda, lda, b+m/2, ldb, depth);
	    } else {
	      trmm_omp_task(side, uplo, transa, diag, m-m/2, m-m/2, alpha, a+m/2+(m/2)*lda, lda, b+m/2, ldb, depth);
	      gemm_omp_task(transa, 'N', m-m/2, n, m/2, alpha, a+m/2, lda, b, ldb, scalar(1.), b+m/2, ldb, depth);
	      trmm_omp_task(side, uplo, transa, diag, m/2, m/2, alpha, a, lda, b, ldb, depth);
	    }
	  }
	}
      } else { // side == R/r
	if (n >= m) {
	  bool opA = transa=='T'||transa=='t'||transa=='C'||transa=='c';
	  if (uplo == 'U' || uplo == 'u') {
	    if (opA) {
	      trmm_omp_task(side, uplo, transa, diag, m, n/2, alpha, a, lda, b, ldb, depth);
	      gemm_omp_task('N', transa, m, n/2, n-n/2, alpha, b+(n/2)*ldb, ldb, a+(n/2)*lda, lda, scalar(1.), b, ldb, depth);
	      trmm_omp_task(side, uplo, transa, diag, m, n-n/2, alpha, a+n/2+(n/2)*lda, lda, b+(n/2)*ldb, ldb, depth);
	    } else {
	      trmm_omp_task(side, uplo, transa, diag, m, n-n/2, alpha, a+n/2+(n/2)*lda, lda, b+(n/2)*ldb, ldb, depth);
	      gemm_omp_task('N', transa, m, n-n/2, n/2, alpha, b, ldb, a+(n/2)*lda, lda, scalar(1.), b+(n/2)*ldb, ldb, depth);
	      trmm_omp_task(side, uplo, transa, diag, m, n/2, alpha, a, lda, b, ldb, depth);
	    }
	  } else {
	    if (opA) {
	      trmm_omp_task(side, uplo, transa, diag, m, n-n/2, alpha, a+n/2+(n/2)*lda, lda, b+(n/2)*ldb, ldb, depth);
	      gemm_omp_task('N', transa, m, n-n/2, n/2, alpha, b, ldb, a+n/2, lda, scalar(1.), b+(n/2)*ldb, ldb, depth);
	      trmm_omp_task(side, uplo, transa, diag, m, n/2, alpha, a, lda, b, ldb, depth);
	    } else {
	      trmm_omp_task(side, uplo, transa, diag, m, n/2, alpha, a, lda, b, ldb, depth);
	      gemm_omp_task('N', transa, m, n/2, n-n/2, alpha, b+(n/2)*ldb, ldb, a+n/2, lda, scalar(1.), b, ldb, depth);
	      trmm_omp_task(side, uplo, transa, diag, m, n-n/2, alpha, a+n/2+(n/2)*lda, lda, b+(n/2)*ldb, ldb, depth);
	    }
	  }
	} else {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  trmm_omp_task(side, uplo, transa, diag, m/2, n, alpha, a, lda, b, ldb, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  trmm_omp_task(side, uplo, transa, diag, m-m/2, n, alpha, a, lda, b+m/2, ldb, depth+1);
#pragma omp taskwait
	}
      }
    }
  }

  // it seems this is never called?! so optimized code is not tested
  template<typename scalar> inline void trmv_omp_task(char uplo, char transa, char diag, int N, scalar* A, int lda, scalar* X, int incx, int depth) {
    blas::trmv(uplo, transa, diag, N, A, lda, X, incx);

    // if (incx == 1) trmm_omp_task('L', uplo, transa, diag, N, 1, scalar(1.), A, lda, X, N, depth);
    // else trmv(uplo, transa, diag, N, A, lda, X, incx);

    // if (N == 0) return;
    // if (depth>=params::task_recursion_cutoff_level || double(N)*N <= OMPTileSize*OMPTileSize)
    //   trmv(uplo, transa, diag, N, A, lda, X, incx);
    // else {
    //   std::cout << "hello from trmv!!!" << std::endl;
    //   if (uplo == 'U' || uplo == 'u') {
    //     trmv_omp_task(uplo, transa, diag, N/2, A, lda, X, incx, depth);
    //     if (transa=='T'||transa=='t'||transa=='C'||transa=='c')
    // 	gemv_omp_task(transa, N-N/2, N/2, scalar(1.), A+(N/2)*lda, lda, X+(N/2)*incx, incx, scalar(1.), X, incx, depth);
    //     else gemv_omp_task(transa, N/2, N-N/2, scalar(1.), A+(N/2)*lda, lda, X+(N/2)*incx, incx, scalar(1.), X, incx, depth);
    //     trmv_omp_task(uplo, transa, diag, N-N/2, A+N/2+(N/2)*lda, lda, X+(N/2)*incx, incx, depth);
    //   } else {
    //     trmv_omp_task(uplo, transa, diag, N-N/2, A+N/2+(N/2)*lda, lda, X+(N/2)*incx, incx, depth);
    //     if (transa=='T'||transa=='t'||transa=='C'||transa=='c')
    // 	gemv_omp_task(transa, N/2, N-N/2, scalar(1.), A+N/2, lda, X, incx, scalar(1.), X+(N/2)*incx, incx, depth);
    //     else gemv_omp_task(transa, N-N/2, N/2, scalar(1.), A+N/2, lda, X, incx, scalar(1.), X+(N/2)*incx, incx, depth);
    //     trmv_omp_task(uplo, transa, diag, N/2, A, lda, X, incx, depth);
    //   }
    // }
  }

  template<typename scalar> void trsm_omp_task(char side, char uplo, char transa, char diag, int m, int n, scalar alpha, scalar* a, int lda, scalar* b, int ldb, int depth) {
    if (double(m)*m*n <= trsmOMPThreshold || depth>=params::task_recursion_cutoff_level)
      blas::trsm(side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb);
    else {
      if ((side=='L' || side=='l') && (uplo=='L' || uplo=='l') && (transa=='N' || transa=='n')) {
	if (n >= m) {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  trsm_omp_task(side, uplo, transa, diag, m, n/2, alpha, a, lda, b, ldb, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  trsm_omp_task(side, uplo, transa, diag, m, n-n/2, alpha, a, lda, b+n/2*ldb, ldb, depth+1);
#pragma omp taskwait
	} else {
	  trsm_omp_task(side, uplo, transa, diag, m/2, n, alpha, a, lda, b, ldb, depth);
	  gemm_omp_task('N', 'N', m-m/2, n, m/2, scalar(-1.), a+m/2, lda, b, ldb, alpha, b+m/2, ldb, depth);
	  trsm_omp_task(side, uplo, transa, diag, m-m/2, n, scalar(1.), a+m/2+m/2*lda, lda, b+m/2, ldb, depth);
	}
      } else if ((side=='R' || side=='r') && (uplo=='U' || uplo=='u') && (transa=='N' || transa=='n')) {
	if (m >= n) {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  trsm_omp_task(side, uplo, transa, diag, m/2, n, alpha, a, lda, b, ldb, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  trsm_omp_task(side, uplo, transa, diag, m-m/2, n, alpha, a, lda, b+m/2, ldb, depth+1);
#pragma omp taskwait
	} else {
	  trsm_omp_task(side, uplo, transa, diag, m, n/2, alpha, a, lda, b, ldb, depth);
	  gemm_omp_task('N', 'N', m, n-n/2, n/2, scalar(-1.), b, ldb, a+n/2*lda, lda, alpha, b+n/2*ldb, ldb, depth);
	  trsm_omp_task(side, uplo, transa, diag, m, n-n/2, scalar(1.), a+n/2+n/2*lda, lda, b+n/2*ldb, ldb, depth);
	}
      } else if ((side=='L' || side=='l') && (uplo=='U' || uplo=='u') && (transa=='N' || transa=='n')) {
	if (n >= m) {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  trsm_omp_task(side, uplo, transa, diag, m, n/2, alpha, a, lda, b, ldb, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
	  trsm_omp_task(side, uplo, transa, diag, m, n-n/2, alpha, a, lda, b+n/2*ldb, ldb, depth+1);
#pragma omp taskwait
	} else {
	  trsm_omp_task(side, uplo, transa, diag, m-m/2, n, alpha, a+m/2+m/2*lda, lda, b+m/2, ldb, depth);
	  gemm_omp_task('N', 'N', m/2, n, m-m/2, scalar(-1.), a+m/2*lda, lda, b+m/2, ldb, alpha, b, ldb, depth);
	  trsm_omp_task(side, uplo, transa, diag, m/2, n, scalar(1.), a, lda, b, ldb, depth);
	}
      } else {
	std::cerr << "trsm_omp_task not implemented with this combination of side, uplo and trans" << std::endl;
	abort();
      }
    }
  }

  template<typename scalar> void laswp_omp_task(int n, scalar* a, int lda, int k1, int k2, int* ipiv, int incx, int depth) {
    if (depth>=params::task_recursion_cutoff_level || n <= OMPTileSize)
      blas::laswp(n, a, lda, k1, k2, ipiv, incx);
    else {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
      laswp_omp_task(n/2, a, lda, k1, k2, ipiv, incx, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
      laswp_omp_task(n-n/2, a+(n/2)*lda, lda, k1, k2, ipiv, incx, depth+1);
#pragma omp taskwait
    }
  }

  // recursive with row pivoting, drop in replacement for regular getrf (parameter col is for internal use)
  template<typename scalar> void getrf_omp_task(int m, int n, scalar* a, int lda, int* ipiv, int* info, int depth, int col=0) {
    //  getrfmod(m, n, a, lda, ipiv, info, depth);
    if (depth>=params::task_recursion_cutoff_level || n <= 1) {
      blas::getrf(m, n, a, lda, ipiv, info); // sequential part, can make this very skinny or add parallelism
      if (*info > 0) info += col;
    } else {
      *info = 0;
      int k = std::min(m,n/2);
      getrf_omp_task(m, k, a, lda, ipiv, info, depth, col);
      if (*info) return;
      laswp_omp_task(n-k, a+k*lda, lda, 1, k, ipiv, 1, depth);
      trsm_omp_task('L', 'L', 'N', 'U', k, n-k, scalar(1.), a, lda, a+k*lda, lda, depth);
      if (m > k){
	gemm_omp_task('N', 'N', m-k, n-k, k, scalar(-1.), a+k, lda, a+k*lda, lda, scalar(1.), a+k+k*lda, lda, depth);
	getrf_omp_task(m-k, n-k, a+k+k*lda, lda, &ipiv[k], info, depth, col+k);
	if (*info) return;
	laswp_omp_task(k, a+k, lda, 1, std::min(m-k, n-k), &ipiv[k], 1, depth);
	for (int i=0; i<std::min(m-k, n-k); i++) ipiv[k+i] += k;
      }
    }
  }

  template<typename scalar> void geru_omp_task(int m, int n, scalar alpha, scalar* x, int incx, scalar* y, int incy, scalar* a, int lda, int depth) {
    if (depth>=params::task_recursion_cutoff_level || 2*double(m)*n <= OMPThreshold)
      blas::geru(m, n, alpha, x, incx, y, incy, a, lda);
    else {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
      geru_omp_task(m/2, n/2, alpha, x, incx, y, incy, a, lda, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
      geru_omp_task(m-m/2, n/2, alpha, x+(m/2)*incx, incx, y, incy, a+m/2, lda, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
      geru_omp_task(m/2, n-n/2, alpha, x, incx, y+(n/2)*incy, incy, a+(n/2)*lda, lda, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
      geru_omp_task(m-m/2, n-n/2, alpha, x+(m/2)*incx, incx, y+(n/2)*incy, incy, a+m/2+(n/2)*lda, lda, depth+1);
#pragma omp taskwait
    }
  }

  template<typename scalar> void gerc_omp_task(int m, int n, scalar alpha, scalar* x, int incx, scalar* y, int incy, scalar* a, int lda, int depth) {
    if (depth>=params::task_recursion_cutoff_level || 2*double(m)*n <= OMPThreshold)
      blas::gerc(m, n, alpha, x, incx, y, incy, a, lda);
    else {
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
      gerc_omp_task(m/2, n/2, alpha, x, incx, y, incy, a, lda, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
      gerc_omp_task(m-m/2, n/2, alpha, x+(m/2)*incx, incx, y, incy, a+m/2, lda, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
      gerc_omp_task(m/2, n-n/2, alpha, x, incx, y+(n/2)*incy, incy, a+(n/2)*lda, lda, depth+1);
#pragma omp task final(depth >= params::task_recursion_cutoff_level-1) mergeable
      gerc_omp_task(m-m/2, n-n/2, alpha, x+(m/2)*incx, incx, y+(n/2)*incy, incy, a+m/2+(n/2)*lda, lda, depth+1);
#pragma omp taskwait
    }
  }


  template<typename scalar> inline void getrs_omp_task(char trans, int m, int n, scalar *a, int lda, int* piv, scalar *b, int ldb, int* flag, int depth) {
    if (depth>=params::task_recursion_cutoff_level)
      blas::getrs(trans, m, n, a, lda, piv, b, ldb, flag);
    else {
      flag = 0;
      if (trans=='N' || trans=='n') {
	if (n==1) {
	  blas::laswp(1, b, ldb, 1, m, piv, 1);
	  trsv_omp_task('L', 'N', 'U', m, a, lda, b, 1, depth);
	  trsv_omp_task('U', 'N', 'N', m, a, lda, b, 1, depth);
	} else {
	  laswp_omp_task(n, b, ldb, 1, m, piv, 1, depth);
	  trsm_omp_task('L', 'L', 'N', 'U', m, n, scalar(1.), a, lda, b, ldb, depth);
	  trsm_omp_task('L', 'U', 'N', 'N', m, n, scalar(1.), a, lda, b, ldb, depth);
	}
      } else {
	std::cerr << "getrs_omp_task not implemented for transpose" << std::endl;
	abort();
      }
    }
  }

} // end namespace strumpack

#endif
