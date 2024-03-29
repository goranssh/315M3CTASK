#include <mpi.h>
#include <CL/cl.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>

void quickSort(std::vector<int> &data, int left, int right) {
    int pivot, i, j;
    if(left < right) {
        pivot = left;
        i = left;
        j = right;
        while(i < j) {
            while(data[i] <= data[pivot] && i < right)
                i++;
            while(data[j] > data[pivot])
                j--;
            if(i < j){
                std::swap(data[i], data[j]);
            }
        }
        std::swap(data[pivot], data[j]);
        quickSort(data, left, j-1);
        quickSort(data, j+1, right);
    }
}

int main(int argc, char** argv) {
    int rank_id, process_count;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_id);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    int data_size = 12;
    int local_data_size = data_size / process_count;
    std::vector<int> local_data(local_data_size);
    srand(rank_id);
    std::generate(local_data.begin(), local_data.end(), [](){ return rand() % 100; });
    std::vector<int> all_data(data_size);
    MPI_Gather(local_data.data(), local_data_size, MPI_INT, all_data.data(), local_data_size, MPI_INT, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();
    if(rank_id == 0) {
        quickSort(all_data, 0, data_size - 1);
    }
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    if(rank_id == 0)
        std::cout << "Elapsed time: " << elapsed_time << " seconds" << std::endl;

    MPI_Finalize();
    return 0;
}

