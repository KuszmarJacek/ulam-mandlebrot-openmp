#include <iostream>
#include <omp.h>
#include <cmath>

#define N 21
#define max(x, y) ((x) > (y) ? (x) : (y))

int TAB[N][N];
unsigned char image[N][N][3];

int ulam_get_map(int x, int y, int n)
{
    x -= (n - 1) / 2;
    y -= n / 2;
    int mx = abs(x), my = abs(y);
    int l = 2 * max(mx, my);
    int d = y >= x ? l * 3 + x + y : l - x - y;

    return pow(l - 1, 2) + d;
}

int isprime(int n)
{
    int p;
    for (p = 2; p * p <= n; p++)
        if (n % p == 0)
            return 0;
    return n > 2;
}

int main()
{
    omp_set_nested(1);
    std::cout << omp_get_nested() << '\n';

    int id1, id2, i, j;

    #pragma omp parallel num_threads(4) private(id1, id2, j)
    {
        id1 = omp_get_thread_num();
        #pragma omp for
        for (j = 0; j < N; j++)
        {
            id2 = omp_get_thread_num();
            #pragma omp parallel num_threads(4) private(i) shared(id1, j)
            {
                #pragma omp for
                for (i = 0; i < N; i++)
                {
                    if (isprime(ulam_get_map(i, j, N))) {
                        TAB[j][i] = 1;
                    }
                    image[j][i][0] = (1 + omp_get_thread_num()) * 200; 
                    image[j][i][1] = (1 + omp_get_thread_num()) * 100; 
                    image[j][i][2] = (1 + omp_get_thread_num()) * 50; 
                }
            }
        }
    }

    for (j = 0; j < N; j++)
    {
        for (i = 0; i < N; i++)
        {
            std::cout << TAB[j][i] << '\t';
        }
        std::cout << '\n';
    }

    FILE *fp;
    char *filename = "new2.ppm";
    char *comment = "# ";
    fp = fopen(filename, "wb");
    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, N, N, 255);

    for (j = 0; j < N; j++) {
        for (i = 0; i < N; i++) {
            if (TAB[j][i] == 1) {
                image[j][i][0] = 255; 
                image[j][i][1] = 255; 
                image[j][i][2] = 255; 
            }
        }
    }

    fwrite(image, 1, 3 * N * N, fp);
    fclose(fp);

    return 0;
}