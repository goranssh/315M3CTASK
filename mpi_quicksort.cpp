#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

#define ARRAY_SIZE 100000

// Function to swap two elements in an array
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i+1], &arr[high]);

    return i+1;
}

void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi-1);
        quicksort(arr, pi+1, high);
    }
}

int main(int argc, char** argv) {
    int rank_id, process_count;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_id);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    int arr[ARRAY_SIZE];
    srand(time(NULL));

    if (rank_id == 0) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            arr[i] = rand() % 1000;
        }
    }

    MPI_Bcast(arr, ARRAY_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    int chunk_size = ARRAY_SIZE / process_count;
    int buffer[chunk_size];
    MPI_Scatter(arr, chunk_size, MPI_INT, buffer, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    quicksort(buffer, 0, chunk_size-1);

    MPI_Gather(buffer, chunk_size, MPI_INT, arr, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank_id == 0) {
        int temp[ARRAY_SIZE];
        int i, j, k;

        for (i = 0; i < process_count; i++) {
            for (j = 0; j < chunk_size; j++) {
                temp[i*chunk_size+j] = arr[i+j*process_count];
            }
        }

        for (k = 0, i = 0, j = chunk_size; k < ARRAY_SIZE; k++) {
            if (i == chunk_size) {
                arr[k] = temp[j++];
            } else if (j == 2*chunk_size) {
                arr[k] = temp[i++];
            } else {
                arr[k] = (temp[i] < temp[j]) ? temp[i++] : temp[j++];
            }
        }
    }

    if (rank_id == 0) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            std::cout << arr[i] << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize();

    return 0;
}
