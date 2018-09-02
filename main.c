#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

//#define DEBUG
#define JOBS 50000000
#define N        102400
double A[N];
int head = 0;
int size = 0;

void produce() {
//    double x = rand() % N;
    double x = 1;
    int ok = 0;
    while (ok == 0) {
        #pragma omp flush(size)
        #pragma omp critical
        {
            if (size < N) {
                A[(head + size) % N] = x;
                ++size;
#ifdef DEBUG
                printf("%d produces %f %d\n", omp_get_thread_num(), x, size);
#endif
                ok = 1;
            }
        }
    }
}

double consume() {
    double x = 0;
    int ok = 0;
    while (ok == 0) {
        #pragma omp flush(size)
        #pragma omp critical
        {
            if (size) {
                x = A[head];
                A[head] = 0;
                head = (head + 1) % N;
                size--;
#ifdef DEBUG
                printf("%d consumes %f %d\n", omp_get_thread_num(), x, size);
#endif
                ok = 1;
            }
        }
    }
    return x;
}

int main(int argc, const char* argv[])
{
    int num_pro = 2, num_cons = 2;
    if (argc > 2) {
        num_pro = atoi(argv[1]);
        num_cons = atoi(argv[2]);
    }
//    srand(time(NULL));
    double sum = 0, runtime;
    int flag = 0;

    runtime = omp_get_wtime();

    omp_set_nested(1);

    #pragma omp parallel sections
    {
        #pragma omp section
        #pragma omp parallel for num_threads(num_pro)
        for (int i = 0; i < JOBS; ++i)
            produce();


        #pragma omp section
        #pragma omp parallel for reduction(+:sum) num_threads(num_cons)
            for (int i = 0; i < JOBS; ++i)
                sum += consume();
    }
    runtime = omp_get_wtime() - runtime;
    printf("%d %d %f %f\n", num_pro, num_cons, runtime, sum);
}

