#include <iostream>
#include <mpi.h>
#include <fstream>
#include <cmath>
#include <sys/time.h>
#include <arm_neon.h>



//#define _PRINT
#define _TEST

using namespace std;

// ============================================== ==============================================
int N;
const int L = 100;
int LOOP = 1;
float **origin_data;
float **matrix = nullptr;

int NUM_THREADS = 8;

ofstream res_stream;

void init_data();
void init_matrix();
void calculate_serial();
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
    
    time = 0;
    for (int i = 0; i < LOOP; i++) {
        init_matrix();
        gettimeofday(&start, nullptr);
        calculate_serial();
        gettimeofday(&end, nullptr);
        time += ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) * 1.0 / 1000;
    }
    if (rank == 0) {
        cout << "serial:" << time / LOOP << "ms" << endl;
        print_result(time);
    }
}




void calculate_serial() {
    for (int k = 0; k < N; k++) {
        for (int j = k + 1; j < N; j++) {
            matrix[k][j] = matrix[k][j] / matrix[k][k];
        }
        matrix[k][k] = 1;
        for (int i = k + 1; i < N; i++) {
            for (int j = k + 1; j < N; j++) {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
    }
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
