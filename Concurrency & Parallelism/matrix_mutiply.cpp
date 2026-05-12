#include <thread>
#include <cmath>

void sequential_matmul(long **A, size_t num_rows_a, size_t num_cols_a,
                       long **B, size_t num_rows_b, size_t num_cols_b,
                       long **C) {
    for(size_t i = 0; i < num_rows_a; i++) {
        for(size_t j = 0; j < num_cols_b; j++) {
            C[i][j] = 0;
            for(size_t k = 0; k < num_cols_a; k++) {
                C[i][j] = A[i][k] * B[k][j];
            }
        }
    }
}

void parallel_worker(long **A, size_t num_rows_a, size_t num_cols_a,
                       long **B, size_t num_rows_b, size_t num_cols_b,
                       long **C) {
    for(size_t i = 0; i < num_rows_a; i++) {
        for(size_t j = 0; j < num_cols_b; j++) {
            C[i][j] = 0;
            for(size_t k = 0; k < num_cols_a; k++) {
                C[i][j] = A[i][k] * B[k][j];
            }
        }
    }
}

void parallel_sum(long **A, size_t num_rows_a, size_t num_cols_a,
                  long **B, size_t num_rows_b, size_t num_cols_b,
                  long **C) {
    size_t num_workers = std::thread::hardware_concurrency();
    size_t chunk_size = ceil((float) num_rows_a/num_workers);

    std::thread workers[num_workers];
    for(size_t i = 0; i < num_workers; i++) {
        size_t start_row_c = std::min(i * chunk_size, num_rows_a);
        size_t end_row_c = std::min((i + 1) * chunk_size, num_rows_a);

        workers[i] = std::thread(parallel_worker, A, num_rows_a, num_cols_a,
                                                  B, num_rows_b, num_cols_b,
                                                  C, start_row_c, end_row_c);
    }

    for (auto &w : workers) 
        w.join();
}