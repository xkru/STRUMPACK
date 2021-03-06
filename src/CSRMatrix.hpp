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
#ifndef CSRMATRIX_HPP
#define CSRMATRIX_HPP
#include <sstream>
#include <fstream>
#include <vector>
#include <tuple>
#include "CompressedSparseMatrix.hpp"
#include "tools.hpp"
#include "blas_lapack_wrapper.hpp"
#include "DistributedMatrix.hpp"
#include "DenseMatrix.hpp"

namespace strumpack {

  // forward declaration
  template<typename scalar_t,typename integer_t> class CSCMatrix;

  template<typename scalar_t,typename integer_t> class CSRMatrix
    : public CompressedSparseMatrix<scalar_t,integer_t> {
    using DistM_t = DistributedMatrix<scalar_t>;
    using DenseM_t = DenseMatrix<scalar_t>;
    using real_t = typename RealType<scalar_t>::value_type;
  public:
    CSRMatrix();
    CSRMatrix(integer_t n, integer_t nnz);
    CSRMatrix(integer_t n, integer_t* ptr, integer_t* ind, scalar_t* values, bool symm_sparsity=false);
    CSRMatrix(const CSRMatrix<scalar_t,integer_t>& A);
    CSRMatrix(CSCMatrix<scalar_t,integer_t>& A);
    CSRMatrix<scalar_t,integer_t>& operator=(const CSRMatrix<scalar_t,integer_t>& A);
    CSRMatrix<scalar_t,integer_t>* clone();
    void spmv(scalar_t* x, scalar_t* y);
    void omp_spmv(scalar_t* x, scalar_t* y);
    void extract_separator(integer_t sep_end, const std::vector<std::size_t>& I, const std::vector<std::size_t>& J,
			   DenseM_t& B, int depth) const;
    void front_multiply(integer_t slo, integer_t shi, integer_t* upd, integer_t dupd,
			const DenseM_t& R, DenseM_t& Sr, DenseM_t& Sc) const;
    void extract_front(scalar_t* F11, scalar_t* F12, scalar_t* F21, integer_t dim_sep, integer_t dim_upd,
		       integer_t sep_begin, integer_t sep_end, integer_t* upd, int depth);
    void extract_F11_block(scalar_t* F, integer_t ldF, integer_t row, integer_t nr_rows,
			   integer_t col, integer_t nr_cols);
    void extract_F12_block(scalar_t* F, integer_t ldF, integer_t row, integer_t nr_rows,
			   integer_t col, integer_t nr_cols, integer_t* upd);
    void extract_F21_block(scalar_t* F, integer_t ldF, integer_t row, integer_t nr_rows,
			   integer_t col, integer_t nr_cols, integer_t* upd);
    void apply_scaling(std::vector<scalar_t>& Dr, std::vector<scalar_t>& Dc);
    void apply_column_permutation(std::vector<integer_t>& perm);
    real_t max_scaled_residual(scalar_t* x, scalar_t* b);
    void strumpack_mc64(int_t job, int_t* num, integer_t* perm, int_t liw, int_t* iw, int_t ldw,
			double* dw, int_t* icntl, int_t* info);
    int read_matrix_market(std::string filename);
    int read_binary(std::string filename);
    void print_dense(std::string name);
    void print_MM(std::string filename);
    void print_binary(std::string filename);

    void extract_separator_2d(integer_t sep_end, const std::vector<std::size_t>& I, const std::vector<std::size_t>& J,
			      DistM_t& B, MPI_Comm comm) const;
    void front_multiply_2d(integer_t sep_begin, integer_t sep_end, integer_t* upd, integer_t dim_upd,
			   DistM_t& R, DistM_t& Srow, DistM_t& Scol, int ctxt_all, MPI_Comm R_comm, int depth);
  };

  template<typename scalar_t,typename integer_t>
  CSRMatrix<scalar_t,integer_t>::CSRMatrix() : CompressedSparseMatrix<scalar_t,integer_t>() {}

  template<typename scalar_t,typename integer_t>
  CSRMatrix<scalar_t,integer_t>::CSRMatrix
  (integer_t n, integer_t* ptr, integer_t* ind, scalar_t* values, bool symm_sparsity)
    : CompressedSparseMatrix<scalar_t,integer_t>(n, ptr, ind, values, symm_sparsity) { }

  template<typename scalar_t,typename integer_t>
  CSRMatrix<scalar_t,integer_t>::CSRMatrix(integer_t n, integer_t nnz)
    : CompressedSparseMatrix<scalar_t,integer_t>(n, nnz) {}

  template<typename scalar_t,typename integer_t>
  CSRMatrix<scalar_t,integer_t>::CSRMatrix(const CSRMatrix<scalar_t,integer_t>& A) {
    this->clone_data(A);
  }

  template<typename scalar_t,typename integer_t> CSRMatrix<scalar_t,integer_t>&
  CSRMatrix<scalar_t,integer_t>::operator=(const CSRMatrix<scalar_t,integer_t>& A) {
    this->clone_data(A);
    return *this;
  }

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::print_dense(std::string name) {
    scalar_t* M = new scalar_t[this->_n * this->_n];
    std::fill(M, M+(this->_n*this->_n), scalar_t(0.));
    for (integer_t row=0; row<this->_n; row++)
      for (integer_t j=this->_ptr[row]; j<this->_ptr[row+1]; j++)
	M[row + this->_ind[j]*this->_n] = this->_val[j];
    std::cout << name << " = [";
    for (integer_t row=0; row<this->_n; row++) {
      for (integer_t col=0; col<this->_n; col++)
	std::cout << M[row + this->_n * col] << " ";
      std::cout << ";" << std::endl;
    }
    std::cout << "];" << std::endl;
    delete[] M;
  }

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::print_MM(std::string filename) {
    std::fstream fs(filename, std::fstream::out);
    if (is_complex<scalar_t>())
      fs << "%%MatrixMarket matrix coordinate complex general\n";
    else
      fs << "%%MatrixMarket matrix coordinate real general\n";
    fs << this->_n << " " << this->_n << " " << this->_nnz << "\n";
    fs.precision(17);
    if (is_complex<scalar_t>()) {
      for (integer_t row=0; row<this->_n; row++)
	for (integer_t j=this->_ptr[row]; j<this->_ptr[row+1]; j++)
	  fs << row+1 << " " << this->_ind[j]+1 << " " << std::real(this->_val[j]) << " " << std::imag(this->_val[j]) << "\n";
    } else {
      for (integer_t row=0; row<this->_n; row++)
	for (integer_t j=this->_ptr[row]; j<this->_ptr[row+1]; j++)
	  fs << row+1 << " " << this->_ind[j]+1 << " " << this->_val[j] << "\n";
    }
  }

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::print_binary(std::string filename) {
    std::ofstream fs(filename, std::ofstream::binary);
    char s = 'R';
    fs.write(&s, sizeof(char));
    if (std::is_same<integer_t,int>()) s = '4';
    else if (std::is_same<integer_t,int64_t>()) s = '8';
    fs.write(&s, sizeof(char));
    if (is_complex<scalar_t>()) {
      if (std::is_same<real_t,float>()) s = 'c';
      else if (std::is_same<real_t,double>()) s = 'z';
    } else {
      if (std::is_same<real_t,float>()) s = 's';
      else if (std::is_same<real_t,double>()) s = 'd';
    }
    fs.write(&s, sizeof(char));
    fs.write((char*)&this->_n, sizeof(integer_t));
    fs.write((char*)&this->_n, sizeof(integer_t));
    fs.write((char*)&this->_nnz, sizeof(integer_t));

    for (integer_t i=0; i<this->_n+1; i++)
      fs.write((char*)(&this->_ptr[i]), sizeof(integer_t));
    for (integer_t i=0; i<this->_nnz; i++)
      fs.write((char*)(&this->_ind[i]), sizeof(integer_t));
    for (integer_t i=0; i<this->_nnz; i++)
      fs.write((char*)(&(this->_val[i])), sizeof(scalar_t));

    if (!fs.good()) {
      std::cout << "Error writing to file !!" << std::endl;
      std::cout << "failbit = " << fs.fail() << std::endl;
      std::cout << "eofbit  = " << fs.eof() << std::endl;
      std::cout << "badbit  = " << fs.bad() << std::endl;
    }
    std::cout << "Wrote " << fs.tellp() << " bytes to file " << filename << std::endl;
    fs.close();
  }

  template<typename scalar_t,typename integer_t> int
  CSRMatrix<scalar_t,integer_t>::read_binary(std::string filename) {
    std::ifstream fs(filename, std::ifstream::in | std::ifstream::binary);
    char s;
    fs.read(&s, sizeof(s));
    if (s != 'R') {
      std::cerr << "Error: matrix is not in binary CSR format." << std::endl;
      return 1;//throw "Error: matrix is not in binary CSR format.";
    }
    fs.read(&s, sizeof(s));
    if (sizeof(integer_t) != s-'0') {
      std::cerr << "Error: matrix integer_t type does not match, input matrix uses " << (s-'0') << " bytes per integer." << std::endl;
      return 1;//throw "Error: matrix integer_t type does not match input matrix.";
    }
    fs.read(&s, sizeof(s));
    if ((!is_complex<scalar_t>() && std::is_same<real_t,float>() && s!='s') ||
	(!is_complex<scalar_t>() && std::is_same<real_t,double>() && s!='d') ||
	(is_complex<scalar_t>() && std::is_same<real_t,float>() && s!='c') ||
	(is_complex<scalar_t>() && std::is_same<real_t,double>() && s!='z')) {
      std::cerr << "Error: scalar type of input matrix does not match, input matrix is of type " << s << std::endl;
      return 1;//throw "Error: scalar type of input matrix does not match";
    }
    fs.read((char*)&this->_n, sizeof(integer_t));
    fs.read((char*)&this->_n, sizeof(integer_t));
    fs.read((char*)&this->_nnz, sizeof(integer_t));
    std::cout << "# Reading matrix with n=" << number_format_with_commas(this->_n)
	      << ", nnz=" << number_format_with_commas(this->_nnz) << std::endl;
    this->symmetric_sparsity = false;
    this->_ptr = new integer_t[this->_n+1];
    this->_ind = new integer_t[this->_nnz];
    this->_val = new scalar_t[this->_nnz];
    for (integer_t i=0; i<this->_n+1; i++) fs.read((char*)(&this->_ptr[i]), sizeof(integer_t));
    for (integer_t i=0; i<this->_nnz; i++) fs.read((char*)(&this->_ind[i]), sizeof(integer_t));
    for (integer_t i=0; i<this->_nnz; i++) fs.read((char*)(&this->_val[i]), sizeof(scalar_t));
    fs.close();
    return 0;
  }

  template<typename scalar_t,typename integer_t>
  CSRMatrix<scalar_t,integer_t>::CSRMatrix(CSCMatrix<scalar_t,integer_t>& A) {
    this->_n = A.size();
    this->_nnz = A.nnz();
    this->symmetric_sparsity = A.has_symmetric_sparsity();
    this->_ptr = new integer_t[this->_n+1];
    this->_ind = new integer_t[A.nnz()];
    this->_val = new scalar_t[A.nnz()];
    std::vector<integer_t> rowsums(this->_n, 0);
    for (integer_t col=0; col<this->_n; col++)
      for (integer_t i=A.get_ptr()[col]; i<A.get_ptr()[col+1]; i++)
	rowsums[A.get_ind()[i]]++;
    this->_ptr[0] = 0;
    for (integer_t r=0; r<this->_n; r++) this->_ptr[r+1] = this->_ptr[r] + rowsums[r];
    std::fill(rowsums.begin(), rowsums.end(), 0);
    for (integer_t col=0; col<this->_n; col++) {
      for (integer_t i=A.get_ptr()[col]; i<A.get_ptr()[col+1]; i++) {
	integer_t row = A.get_ind()[i];
	integer_t j = this->_ptr[row] + rowsums[row]++;
	this->_val[j] = A.get_val()[i];
	this->_ind[j] = col;
      }
    }
  }

  template<typename scalar_t,typename integer_t> CSRMatrix<scalar_t,integer_t>*
  CSRMatrix<scalar_t,integer_t>::clone() {
    CSRMatrix<scalar_t,integer_t>* t = new CSRMatrix<scalar_t,integer_t>();
    t->clone_data(*this);
    return t;
  }

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::spmv(scalar_t* x, scalar_t* y) {
    for (integer_t row=0; row<this->_n; row++) {
      scalar_t yrow = scalar_t(0.);
      for (integer_t j=this->_ptr[row]; j<this->_ptr[row+1]; j++)
	yrow += this->_val[j] * x[this->_ind[j]];
      y[row] = yrow;
    }
    STRUMPACK_FLOPS((is_complex<scalar_t>()?4:1)*static_cast<long long int>(2.*double(this->nnz())-double(this->size())));
    STRUMPACK_BYTES((sizeof(scalar_t)*4+sizeof(integer_t))*static_cast<long long int>(this->nnz())
		    + sizeof(integer_t)*static_cast<long long int>(this->_n));
  }

#if defined(__INTEL_MKL__)
  template<> void CSRMatrix<float>::spmv(float* x, float* y)
  { char trans = 'N'; mkl_cspblas_scsrgemv(&no, &n, this->_val, this->_ptr, this->_ind, x, y);
    STRUMPACK_FLOPS(static_cast<long long int>(2.*double(this->nnz())-double(this->size()))); }
  template<> void CSRMatrix<double>::spmv(double* x, double* y)
  { char trans = 'N'; mkl_cspblas_dcsrgemv(&no, &n, this->_val, this->_ptr, this->_ind, x, y);
    STRUMPACK_FLOPS(static_cast<long long int>(2.*double(this->nnz())-double(this->size()))); }
  template<> void CSRMatrix<c_float>::spmv(c_float* x, c_float* y)
  { char trans = 'N'; mkl_cspblas_ccsrgemv(&no, &n, this->_val, this->_ptr, this->_ind, x, y);
    STRUMPACK_FLOPS(4*static_cast<long long int>(2.*double(this->nnz())-double(this->size()))); }
  template<> void CSRMatrix<c_double>::spmv(c_double* x, c_double* y)
  { char trans = 'N'; mkl_cspblas_zcsrgemv(&no, &n, this->_val, this->_ptr, this->_ind, x, y);
    STRUMPACK_FLOPS(4*static_cast<long long int>(2.*double(this->nnz())-double(this->size()))); }
#endif

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::omp_spmv(scalar_t* x, scalar_t* y) {
#pragma omp parallel for
    for (integer_t row=0; row<this->_n; ++row) {
      scalar_t yrow = scalar_t(0.);
      for (integer_t j=this->_ptr[row]; j<this->_ptr[row+1]; ++j)
	yrow += this->_val[j] * x[this->_ind[j]];
      y[row] = yrow;
    }
    STRUMPACK_FLOPS((is_complex<scalar_t>()?4:1)*static_cast<long long int>(2.*double(this->nnz())-double(this->size())));
    STRUMPACK_BYTES((sizeof(scalar_t)*4+sizeof(integer_t))*static_cast<long long int>(this->nnz())
		    + sizeof(integer_t)*static_cast<long long int>(this->_n));
  }

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::strumpack_mc64
  (int_t job, int_t* num, integer_t* perm, int_t liw, int_t* iw, int_t ldw,
   double* dw, int_t* icntl, int_t* info) {
    int_t n = this->_n;
    int_t nnz = this->_nnz;
    double* dval = new double[nnz];
    int_t* col_ptr = new int_t[n+1+nnz+n+n];
    int_t* row_ind = col_ptr + n + 1;
    int_t* permutation = row_ind + nnz;
    int_t* rowsums = permutation + n;
    for (int_t i=0; i<n; i++) rowsums[i] = 0;
    for (int_t col=0; col<n; col++)
      for (int_t i=this->_ptr[col]; i<this->_ptr[col+1]; i++)
	rowsums[this->_ind[i]]++;
    col_ptr[0] = 1;  // start from 1, because mc64 is fortran!
    for (int_t r=0; r<n; r++) col_ptr[r+1] = col_ptr[r] + rowsums[r];
    for (int_t i=0; i<n; i++) rowsums[i] = 0;
    for (int_t col=0; col<n; col++) {
      for (int_t i=this->_ptr[col]; i<this->_ptr[col+1]; i++) {
	int_t row = this->_ind[i];
	int_t j = col_ptr[row] - 1 + rowsums[row]++;
	if (is_complex<scalar_t>())
	  dval[j] = static_cast<double>(std::abs(this->_val[i]));
	else dval[j] = static_cast<double>(std::real(this->_val[i]));
	row_ind[j] = col + 1;
      }
    }
    strumpack_mc64ad_(&job, &n, &nnz, col_ptr, row_ind, dval, num, permutation, &liw, iw, &ldw, dw, icntl, info);
    for (int_t i=0; i<n; i++) perm[i] = permutation[i] - 1;
    delete[] col_ptr;
    delete[] dval;
  }

  // TODO tasked!!
  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::extract_separator
  (integer_t sep_end, const std::vector<std::size_t>& I, const std::vector<std::size_t>& J, DenseM_t& B, int depth) const {
    integer_t m = I.size();
    integer_t n = J.size();
    if (m == 0 || n == 0) return;
    for (integer_t i=0; i<m; i++) {
      integer_t r = I[i];
      auto cmin = this->_ind[this->_ptr[r]]; // indices sorted in increasing order
      auto cmax = this->_ind[this->_ptr[r+1]-1];
      for (integer_t k=0; k<n; k++) {
	integer_t c = J[k];
	if (c >= cmin && c <= cmax && (r < sep_end || c < sep_end)) {
	  auto a_pos = this->_ptr[r];
	  auto a_max = this->_ptr[r+1];
	  while (a_pos < a_max-1 && this->_ind[a_pos] < c) a_pos++;
	  B(i,k) = (this->_ind[a_pos] == c) ? this->_val[a_pos] : scalar_t(0.);
	} else B(i,k) = scalar_t(0.);
      }
    }
  }

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::extract_separator_2d
  (integer_t sep_end, const std::vector<std::size_t>& I, const std::vector<std::size_t>& J,
   DistM_t& B, MPI_Comm comm) const {
    if (!B.active()) return;
    integer_t m = I.size();
    integer_t n = J.size();
    if (m == 0 || n == 0) return;
    B.zero();
    for (integer_t i=0; i<m; i++) {
      integer_t r = I[i];
      auto cmin = this->_ind[this->_ptr[r]]; // indices sorted in increasing order
      auto cmax = this->_ind[this->_ptr[r+1]-1];
      for (integer_t k=0; k<n; k++) {
	integer_t c = J[k];
	if (c >= cmin && c <= cmax && (r < sep_end || c < sep_end)) {
	  auto a_pos = this->_ptr[r];
	  auto a_max = this->_ptr[r+1]-1;
	  while (a_pos<a_max && this->_ind[a_pos]<c) a_pos++;
	  if (this->_ind[a_pos] == c) B.global(i, k, this->_val[a_pos]);
	}
      }
    }
  }

  /** TODO this can be done more cleverly */
  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::front_multiply_2d
  (integer_t sep_begin, integer_t sep_end, integer_t* upd, integer_t dim_upd,
   DistM_t& R, DistM_t& Srow, DistM_t& Scol, int ctxt_all, MPI_Comm R_comm, int depth) {
    // redistribute R and Srow and Scol to 1d block cyclic column
    // distribution to avoid communication below
    DistM_t R_bc(R.ctxt(), R.rows(), R.cols(), R.rows(), R.NB());
    DistM_t Srow_bc(R.ctxt(), Srow.rows(), Srow.cols(), Srow.rows(), Srow.NB());
    DistM_t Scol_bc(R.ctxt(), Scol.rows(), Scol.cols(), Scol.rows(), Scol.NB());
    // TODO use copy routines from DistM_t
    scalapack::pgemr2d(R.rows(), R.cols(), R.data(), 1, 1, R.desc(), R_bc.data(), 1, 1, R_bc.desc(), ctxt_all);
    scalapack::pgemr2d(Srow.rows(), Srow.cols(), Srow.data(), 1, 1, Srow.desc(), Srow_bc.data(), 1, 1, Srow_bc.desc(), ctxt_all);
    scalapack::pgemr2d(Scol.rows(), Scol.cols(), Scol.data(), 1, 1, Scol.desc(), Scol_bc.data(), 1, 1, Scol_bc.desc(), ctxt_all);

    if (R.active()) {
      long long int local_flops = 0;
      auto dim_sep = sep_end - sep_begin;
      auto n = R_bc.cols();
      for (integer_t row=sep_begin; row<sep_end; row++) { // separator rows
	auto upd_ptr = 0;
	for (integer_t j=this->_ptr[row]; j<this->_ptr[row+1]; j++) {
	  auto col = this->_ind[j];
	  if (col >= sep_begin) {
	    if (col < sep_end) {
	      scalapack::pgeadd('N', 1, n, this->_val[j], R_bc.data(), col-sep_begin+1, 1, R_bc.desc(),
				scalar_t(1.), Srow_bc.data(), row-sep_begin+1,  1, Srow_bc.desc());
	      scalapack::pgeadd('N', 1, n, this->_val[j], R_bc.data(), row-sep_begin+1, 1, R_bc.desc(),
				scalar_t(1.), Scol_bc.data(), col-sep_begin+1, 1, Scol_bc.desc());
	      local_flops += 4 * n;
	    } else {
	      while (upd_ptr<dim_upd && upd[upd_ptr]<col) upd_ptr++;
	      if (upd_ptr == dim_upd) break;
	      if (upd[upd_ptr] == col) {
		scalapack::pgeadd('N', 1, n, this->_val[j], R_bc.data(), dim_sep+upd_ptr+1, 1, R_bc.desc(),
				  scalar_t(1.), Srow_bc.data(), row-sep_begin+1,  1, Srow_bc.desc());
		scalapack::pgeadd('N', 1, n, this->_val[j], R_bc.data(), row-sep_begin+1,  1, R_bc.desc(),
				  scalar_t(1.), Scol_bc.data(), dim_sep+upd_ptr+1, 1, Scol_bc.desc());
		local_flops += 4 * n;
	      }
	    }
	  }
	}
      }
      for (integer_t i=0; i<dim_upd; i++) { // remaining rows
	integer_t row = upd[i];
	for (integer_t j=this->_ptr[row]; j<this->_ptr[row+1]; j++) {
	  integer_t col = this->_ind[j];
	  if (col >= sep_begin) {
	    if (col < sep_end) {
	      scalapack::pgeadd('N', 1, n, this->_val[j], R_bc.data(), col-sep_begin+1, 1, R_bc.desc(),
				scalar_t(1.), Srow_bc.data(), dim_sep+i+1,  1, Srow_bc.desc());
	      scalapack::pgeadd('N', 1, n, this->_val[j], R_bc.data(), dim_sep+i+1, 1, R_bc.desc(),
				scalar_t(1.), Scol_bc.data(), col-sep_begin+1, 1, Scol_bc.desc());
	      local_flops += 4 * n;
	    } else break;
	  }
	}
      }
      if (R.is_master()) { STRUMPACK_FLOPS((is_complex<scalar_t>() ? 4 : 1) * local_flops); }
    }
    scalapack::pgemr2d(Srow.rows(), Srow.cols(), Srow_bc.data(), 1, 1, Srow_bc.desc(), Srow.data(), 1, 1, Srow.desc(), ctxt_all);
    scalapack::pgemr2d(Scol.rows(), Scol.cols(), Scol_bc.data(), 1, 1, Scol_bc.desc(), Scol.data(), 1, 1, Scol.desc(), ctxt_all);
  }

  // TODO parallel -> will be hard to do efficiently
  // assume F11, F12 and F21 are set to zero
  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::extract_front
  (scalar_t* F11, scalar_t* F12, scalar_t* F21, integer_t dim_sep, integer_t dim_upd,
   integer_t sep_begin, integer_t sep_end, integer_t* upd, int depth) {
    for (integer_t row=0; row<dim_sep; row++) { // separator rows
      integer_t upd_ptr = 0;
      for (integer_t j=this->_ptr[row+sep_begin]; j<this->_ptr[row+sep_begin+1]; j++) {
	integer_t col = this->_ind[j];
	if (col >= sep_begin) {
	  if (col < sep_end) F11[row + (col-sep_begin)*dim_sep] = this->_val[j];
	  else {
	    while (upd_ptr<dim_upd && upd[upd_ptr]<col) upd_ptr++;
	    if (upd_ptr == dim_upd) break;
	    if (upd[upd_ptr] == col) F12[row + upd_ptr*dim_sep] = this->_val[j];
	  }
	}
      }
    }
    for (integer_t i=0; i<dim_upd; i++) { // update rows
      integer_t row = upd[i];
      for (integer_t j=this->_ptr[row]; j<this->_ptr[row+1]; j++) {
	integer_t col = this->_ind[j];
	if (col >= sep_begin) {
	  if (col < sep_end) F21[i + (col-sep_begin)*dim_upd] = this->_val[j];
	  else break;
	}
      }
    }
  }

  // TODO threading in front_multiply?
  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::front_multiply
  (integer_t slo, integer_t shi, integer_t* upd, integer_t dupd,
   const DenseM_t& R, DenseM_t& Sr, DenseM_t& Sc) const {
    long long int local_flops = 0;
    const integer_t nbvec = R.cols();
    const integer_t ds = shi - slo;
    for (auto row=slo; row<shi; row++) { // separator rows
      integer_t upd_ptr = 0;
      for (auto j=this->_ptr[row]; j<this->_ptr[row+1]; j++) {
	auto col = this->_ind[j];
	if (col >= slo) {
	  if (col < shi) {
	    for (integer_t c=0; c<nbvec; c++) {
	      Sr(row-slo, c) += this->_val[j] * R(col-slo, c);
	      Sc(col-slo, c) += this->_val[j] * R(row-slo, c);
	    }
	    local_flops += 4 * nbvec;
	  } else {
	    while (upd_ptr<dupd && upd[upd_ptr]<col) upd_ptr++;
	    if (upd_ptr == dupd) break;
	    if (upd[upd_ptr] == col) {
	      for (integer_t c=0; c<nbvec; c++) {
		Sr(row-slo, c) += this->_val[j] * R(ds+upd_ptr, c);
		Sc(ds+upd_ptr, c) += this->_val[j] * R(row-slo, c);
	      }
	      local_flops += 4 * nbvec;
	    }
	  }
	}
      }
    }
    for (integer_t i=0; i<dupd; i++) { // remaining rows
      auto row = upd[i];
      for (auto j=this->_ptr[row]; j<this->_ptr[row+1]; j++) {
	auto col = this->_ind[j];
	if (col >= slo) {
	  if (col < shi) {
	    for (integer_t c=0; c<nbvec; c++) {
	      Sr(ds+i, c) += this->_val[j] * R(col-slo, c);
	      Sc(col-slo, c) += this->_val[j] * R(ds+i, c);
	    }
	    local_flops += 4 * nbvec;
	  } else break;
	}
      }
    }
    STRUMPACK_FLOPS((is_complex<scalar_t>() ? 4 : 1) * local_flops);
  }

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::apply_scaling(std::vector<scalar_t>& Dr, std::vector<scalar_t>& Dc) {
#pragma omp parallel for
    for (integer_t j=0; j<this->_n; j++)
      for (integer_t i=this->_ptr[j]; i<this->_ptr[j+1]; i++)
	this->_val[i] = this->_val[i] * Dr[j] * Dc[this->_ind[i]];
    STRUMPACK_FLOPS((is_complex<scalar_t>()?6:1)*static_cast<long long int>(2.*double(this->nnz())));
  }

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::apply_column_permutation(std::vector<integer_t>& perm) {
    integer_t* iperm = new integer_t[this->_n];
    for (integer_t i=0; i<this->_n; i++) iperm[perm[i]] = i;
#pragma omp parallel for
    for (integer_t row=0; row<this->_n; row++) {
      for (integer_t i=this->_ptr[row]; i<this->_ptr[row+1]; i++)
	this->_ind[i] = iperm[this->_ind[i]];
      sort_indices_values<scalar_t>(&this->_ind[this->_ptr[row]], &this->_val[this->_ptr[row]],
				    integer_t(0), this->_ptr[row+1]-this->_ptr[row]);
    }
    delete[] iperm;
  }

  template<typename scalar_t,typename integer_t> int
  CSRMatrix<scalar_t,integer_t>::read_matrix_market(std::string filename) {
    std::vector<std::tuple<integer_t,integer_t,scalar_t>> A;
    try {
      A = this->read_matrix_market_entries(filename);
    } catch (...) { return 1; }
    std::sort(A.begin(), A.end(), [](const std::tuple<integer_t,integer_t,scalar_t>& a, const std::tuple<integer_t,integer_t,scalar_t>& b) -> bool {
	// sort based on the row,column indices
	return std::make_tuple(std::get<0>(a),std::get<1>(a)) < std::make_tuple(std::get<0>(b), std::get<1>(b));
      });

    this->_ptr = new integer_t[this->_n+1];
    this->_ind = new integer_t[this->_nnz];
    this->_val = new scalar_t[this->_nnz];
    integer_t row = -1;
    for (integer_t i=0; i<this->_nnz; i++) {
      this->_val[i] = std::get<2>(A[i]);
      this->_ind[i] = std::get<1>(A[i]);
      auto new_row = std::get<0>(A[i]);
      if (new_row != row) {
	for (int j=row+1; j<=new_row; j++) this->_ptr[j] = i;
	row = new_row;
      }
    }
    for (int j=row+1; j<=this->_n; j++) this->_ptr[j] = this->_nnz;
    return 0;
  }

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::extract_F11_block
  (scalar_t* F, integer_t ldF, integer_t row, integer_t nr_rows, integer_t col, integer_t nr_cols) {
    for (integer_t r=row; r<std::min(row+nr_rows, this->_n); r++) {
      integer_t j = this->_ptr[r];
      while (j<this->_ptr[r+1] && this->_ind[j] < col) j++;
      for ( ; j<this->_ptr[r+1]; j++) {
	integer_t c = this->_ind[j];
	if (c < col+nr_cols) F[r-row + (c-col)*ldF] = this->_val[j];
	else break;
      }
    }
  }

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::extract_F12_block
  (scalar_t* F, integer_t ldF, integer_t row, integer_t nr_rows,
   integer_t col, integer_t nr_cols, integer_t* upd) {
    for (integer_t r=row; r<std::min(row+nr_rows, this->_n); r++) {
      integer_t upd_pos = 0;
      for (integer_t j=this->_ptr[r]; j<this->_ptr[r+1]; j++) {
	integer_t c = this->_ind[j];
	if (c >= col) {
	  while (upd_pos<nr_cols && upd[upd_pos]<c) upd_pos++;
	  if (upd_pos == nr_cols) break;
	  if (upd[upd_pos] == c) F[r-row + upd_pos*ldF] = this->_val[j];
	}
      }
    }
  }

  template<typename scalar_t,typename integer_t> void
  CSRMatrix<scalar_t,integer_t>::extract_F21_block
  (scalar_t* F, integer_t ldF, integer_t row, integer_t nr_rows,
   integer_t col, integer_t nr_cols, integer_t* upd) {
    for (integer_t i=row; i<std::min(row+nr_rows, this->_n); i++) {
      integer_t r = upd[i-row];
      integer_t j = this->_ptr[r];
      while (j<this->_ptr[r+1] && this->_ind[j] < col) j++;
      for ( ; j<this->_ptr[r+1]; j++) {
	integer_t c = this->_ind[j];
	if (c < col+nr_cols) F[i-row + (c-col)*ldF] = this->_val[j];
	else break;
      }
    }
  }

  template<typename scalar_t,typename integer_t> typename RealType<scalar_t>::value_type
  CSRMatrix<scalar_t,integer_t>::max_scaled_residual(scalar_t* x, scalar_t* b) {
    real_t m = real_t(0.);
#pragma omp parallel for reduction(max:m)
    for (integer_t i=0; i<this->_n; i++) {
      auto true_res = b[i];
      auto abs_res = std::abs(b[i]);
      for (integer_t j=this->_ptr[i]; j<this->_ptr[i+1]; ++j) {
	true_res -= this->_val[j] * x[this->_ind[j]];
	abs_res += std::abs(this->_val[j]) * std::abs(x[this->_ind[j]]);
      }
      m = std::max(m, std::abs(true_res) / std::abs(abs_res));
    }
    return m;
  }

} // end namespace strumpack

#endif //CSRMATRIX_H
