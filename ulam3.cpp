#include <iostream>
#include <omp.h>
#include <cmath>

#define N 10
#define max(x,y) ((x) > (y) ? (x) : (y))

int TAB[N][N];

int ulam_get_map(int x, int y, int n)
{
    x -= (n - 1) / 2;
    y -= n / 2;
    int mx = abs(x), my = abs(y);
    int l = 2 * max(mx, my);
    int d = y >= x ? l * 3 + x + y : l - x - y;

    return pow(l - 1, 2) + d;
}

// int isprime(int n)
// {
//     int p;
//     for (p = 2; p * p <= n; p++)
//         if (n % p == 0)
//             return 0;
//     return n > 2;
// }

// wlacz omp_set_nested

int main()
{
    omp_set_nested(1);
    std::cout << omp_get_nested() << '\n';

    int id1, id2, i, j;

    #pragma omp parallel num_threads(2) private(id1, id2, j)
    {
        // std::cout << "Level 1: " << omp_get_thread_num() << '\n';
        id1 = omp_get_thread_num();

        #pragma omp barrier
        std::cout << "----\n";

        #pragma omp for
        for(j = 0; j < N; j++) {
            #pragma omp parallel num_threads(2) private(id2, i) shared(id1, j)
            {
                id2 = omp_get_thread_num();

                #pragma omp for
                for(i = 0; i < N; i++) {
                    // #pragma omp critical tylko do testu
                    // std::cout << "Level 2: " << omp_get_thread_num() << '\n';
                    TAB[j][i] = ulam_get_map(i, j, N);
                }
            }
        }
    }

    for(j = 0; j < N; j++) {
        for(i = 0; i < N; i++) {
            std::cout << TAB[j][i] << '\t';
        }
        std::cout << '\n';
    }

    return 0;
}
