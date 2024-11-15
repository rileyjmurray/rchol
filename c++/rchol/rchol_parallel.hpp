#ifndef rchol_parallel_hpp
#define rchol_parallel_hpp

#include "sparse.hpp"
#include <string>

void rchol(const SparseCSR &A, SparseCSR &G, std::vector<size_t> &permutation, 
    std::vector<int> &S, int threads, std::string);

#endif
