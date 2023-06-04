#include <iostream>
#include <mpi.h>
#include <fstream>
#include <cmath>
#include <sys/time.h>
#include <arm_neon.h>



//#define _PRINT
#define _TEST

using namespace std;



int N;
const int L = 100;
int LOOP = 1;
float **origin_data;
float **matrix = nullptr;

int NUM_THREADS = 4;

ofstream res_stream;

void init_data();
void init_matrix();
double calculate_MPI_block();
double calculate_MPI_cycle();
double calculate_MPI_pipeline();
double calculate_MPI_SIMD();
double calculate_MPI_OMP();
double calculate_MPI_OMP_SIMD();
void print_matrix();
void test(int);
void print_result(double);


void init_data() {
    origin_data = new float* [N], matrix = new float* [N];
    auto* tmp = new float[N * N];
    for (int i = 0; i < N; i++) {
        origin_data[i] = new float[N], matrix[i] = tmp + i * N;
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = 0, origin_data[i][j] = 0;
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = i; j < N; j++) {
            origin_data[i][j] = rand() * 1.0 / RAND_MAX * L;
        }
    }
    for (int i = 0; i < N - 1; i++) {
        for (int j = i + 1; j < N; j++) {
            for (int k = 0; k < N; k++) {
                origin_data[j][k] += origin_data[i][k];
            }
        }
    }
}


void init_matrix() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = origin_data[i][j];
        }
    }
}


int main() {
    MPI_Init(nullptr, nullptr);
#ifdef _TEST
    res_stream.open("result.csv", ios::out);
    LOOP = 5;
    for (int i = 500; i <= 3000; i += 500)
        test(i);
    res_stream.close();
#endif
#ifdef _PRINT
    test(10);
#endif
    MPI_Finalize();
    return 0;


    
}



void test(int n) {
    N = n;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        cout << "=================================== " << N << " ===================================" << endl;
        res_stream << N;
    }
    struct timeval start {};
    struct timeval end {};
    double time = 0;
    init_data();
   
    // ====================================== MPI_block ======================================
    time = 0;
    for (int i = 0; i < LOOP; i++) {
        time += calculate_MPI_block();
    }
    if (rank == 0) {
        cout << "MPI_block:" << time / LOOP << "ms" << endl;
        print_result(time);
    }
}


double calculate_MPI_block() {
    double start_time, end_time;

    int rank;
    int size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
  
    if (rank == 0) {
        init_matrix();
    }
    start_time = MPI_Wtime();
    int task_num = ceil(N * 1.0 / size);
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            int start = i * task_num;
            int end = (i + 1) * task_num;
            if (i == size - 1){
                end = N;
            }
            MPI_Send(&matrix[start][0], (end - start) * N, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        }
    }
    
    else {
        if (rank != size - 1) {
            MPI_Recv(&matrix[rank * task_num][0], task_num * N, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(&matrix[rank * task_num][0], (N - rank * task_num) * N, MPI_FLOAT, 0, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        }
    }


    int start = rank * task_num;
    int end = (rank + 1) * task_num < N ? (rank + 1) * task_num : N;
    for (int k = 0; k < N; k++) {
  
        if (k >= start && k < end) {
            for (int j = k + 1; j < N; j++) {
                matrix[k][j] /= matrix[k][k];
            }
            matrix[k][k] = 1;
            for (int p = 0; p < size; p++) {
                if (p != rank) {
                    MPI_Send(&matrix[k][0], N, MPI_FLOAT, p, 0, MPI_COMM_WORLD);
                }
            }
        }
            
        else {
            MPI_Recv(&matrix[k][0], N, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        
        for (int i = max(k + 1, start); i < end; i++) {
            for (int j = k + 1; j < N; j++) {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
    }
    end_time = MPI_Wtime();
    return (end_time - start_time) * 1000;
}

void print_matrix() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.2f ", matrix[i][j]);
        }
        printf("\n");
    }
}



void print_result(double time) {
#ifdef _TEST
    res_stream << "," << time / LOOP;
#endif
#ifdef _PRINT
    print_matrix();
#endif
}
