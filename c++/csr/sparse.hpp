#ifndef sparse_hpp
#define sparse_hpp

#include <vector>
#include <string>


// Compressed sparse row (CSR) format
// See, e.g., https://en.wikipedia.org/wiki/Sparse_matrix#Compressed_sparse_row_(CSR,_CRS_or_Yale_format)
class SparseCSR {
public:
  SparseCSR();
  SparseCSR(const std::vector<size_t>&, const std::vector<size_t>&, const std::vector<double>&, 
      bool mem=true);
  SparseCSR(const SparseCSR &); // deep copy

  void init(const std::vector<size_t>&, const std::vector<size_t>&, const std::vector<double>&,
      bool mem=true);

  void read_mkt_file(std::string filename);

  size_t size() const;
  size_t nnz() const;

  void show(std::string) const;

  ~SparseCSR();

public:
  size_t N = 0;
  size_t *rowPtr;
  size_t *colIdx;
  double *val;
  bool ownMemory;
};


void print(const SparseCSR&, std::string);


#endif
