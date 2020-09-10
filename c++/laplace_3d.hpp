#ifndef LAPLACE_3D_HPP
#define LAPLACE_3D_HPP

#include <cassert>
#include <vector>

template <typename T>
void laplace_3d(size_t n, std::vector<size_t> &rowPtr, std::vector<size_t> &colIdx, 
    std::vector<T> &val) {
  size_t N = n*n*n;
  size_t nnz = 0;
  rowPtr.reserve(N+1); rowPtr.push_back(nnz);
  colIdx.reserve(7*N);
  val.reserve(7*N);

  size_t n2 = n*n;
  for (size_t i=0; i<n; i++) {
    for (size_t j=0; j<n; j++) {
      for (size_t k=0; k<n; k++) {
        size_t idx = k+j*n+i*n2;
        if (i>0) {
          colIdx.push_back(idx-n2);
          val.push_back(-1);
          nnz++;
        }
        if (j>0) {
          colIdx.push_back(idx-n);
          val.push_back(-1);
          nnz++;
        }
        if (k>0) {
          colIdx.push_back(idx-1);
          val.push_back(-1);
          nnz++;
        }
        // self
        colIdx.push_back(idx);
        val.push_back(6);
        nnz++;
        if (k<n-1) {
          colIdx.push_back(idx+1);
          val.push_back(-1);
          nnz++;
        }
        if (j<n-1) {
          colIdx.push_back(idx+n);
          val.push_back(-1);
          nnz++;
        }
        if (i<n-1) {
          colIdx.push_back(idx+n2);
          val.push_back(-1);
          nnz++;
        }
        rowPtr.push_back(nnz);
      }
    }
  }
#if 0
  assert(rowPtr.size()==N+1);
  assert(colIdx.size()==val.size());
  assert(colIdx.size()==7*(n-2)*(n-2)*(n-2)+6*(n-2)*(n-2)*6+5*(n-2)*12+4*1*8);
#endif
}


// input: sparse matrix in csr format
// output: the upper triangular submatrix (including diagonal) in csr format
template <typename T>
void triu_csr
(const std::vector<size_t> &rowPtrA, const std::vector<size_t> &colIdxA, const std::vector<T> &valA,
 std::vector<size_t> &rowPtrU, std::vector<size_t> &colIdxU, std::vector<T> &valU) {
  // get matrix size and nnz
  size_t n = rowPtrA.size()-1;
  size_t nnz = rowPtrA[n];
  // allocate memory
  rowPtrU.resize(n+1, 0);
  colIdxU.reserve((nnz-n)/2);
  valU.reserve((nnz-n)/2);
  // get upper triangular
  for (size_t r=0; r<n; r++) {
    size_t start = rowPtrA[r];
    size_t end = rowPtrA[r+1];
    for (size_t i=start; i<end; i++) {
      if (r <= colIdxA[i]) {
        colIdxU.push_back(colIdxA[i]);
        valU.push_back(valA[i]);
      }
    }
    rowPtrU[r+1] = valU.size();
  }
}

template <typename T>
void change_to_positive_edge(std::vector<T> &val) {
  for(size_t i = 0; i < val.size(); i++)
  {
    val[i] *= -1;
  }
}

template <typename T>
void convert_to_laplace(const std::vector<size_t> &rowPtr, const std::vector<size_t> &colIdx, 
    const std::vector<T> &val, std::vector<size_t> &rowPtrL, std::vector<size_t> &colIdxL, std::vector<T> &valL, 
    std::vector<size_t> &rowPtrA, std::vector<size_t> &colIdxA, std::vector<T> &valA) {
      


  // get matrix size and nnz
  size_t n = rowPtr.size() - 1;
  size_t nnz = rowPtr[n];
  // allocate memory
  rowPtrL.resize(n + 2, 0);
  colIdxL.reserve(nnz + n + 3);
  valL.reserve(nnz + n + 3);
  T overall = 0.0;
  for (size_t r = 0; r < n; r++) {
    size_t start = rowPtr[r];
    size_t end = rowPtr[r + 1];
    
    for (size_t i = start; i < end; i++) {
      colIdxL.push_back(colIdx[i]);
      valL.push_back(val[i]);
      
    }

    
    // add the term that makes it a laplace
    start = rowPtrA[r];
    end = rowPtrA[r + 1];
    T sum = 0.0;
    for (size_t i = start; i < end; i++) {
      sum += valA[i];
    }

    
    if(std::abs(sum) > 1e-09)
    {
      colIdxL.push_back(n);
      valL.push_back(-sum);
    }
    rowPtrL[r+1] = valL.size();
    overall -= sum;
  }

  if(std::abs(overall) > 1e-09)
  {
    colIdxL.push_back(n);
    valL.push_back(-overall);
  }
  rowPtrL[n + 1] = valL.size();

}



#endif
