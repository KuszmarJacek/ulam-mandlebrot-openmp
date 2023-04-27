#include <math.h>
#include <iostream>
#include "omp.h"

using namespace std;

constexpr int iXmax = 4000;
constexpr int iYmax = 4000;
constexpr double CxMin = -2.5;
constexpr double CxMax = 1.5;
constexpr double CyMin = -2.0;
constexpr double CyMax = 2.0;
constexpr double PixelWidth = (CxMax - CxMin) / iXmax;
constexpr double PixelHeight = (CyMax - CyMin) / iYmax;
constexpr double EscapeRadius = 2;
constexpr double ER2 = EscapeRadius * EscapeRadius;
constexpr int IterationMax = 200;
unsigned char color[iYmax][iXmax][3];

void writeImage(const char* fileName) {
    const char *comment = "# ";
    FILE *fp = fopen(fileName, "wb"); 
    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, iXmax, iYmax, 255);
    fwrite(color, 1, 3 * iYmax * iXmax, fp);
    fclose(fp);
}

int main(int argc, char *argv[])
{
    int nthreads = atoi(argv[1]);
    int grain = atoi(argv[2]);
    int iX, iY, i;

    double Cx, Cy;
    double Zx, Zy;
    double Zx2, Zy2;

    int Iteration;

    double start = start = omp_get_wtime();
    int threadIterCount[nthreads]{0};
    int threadNumber;

    #pragma omp parallel private(threadNumber) shared(threadIterCount) num_threads(nthreads)
    {
        threadNumber = omp_get_thread_num();
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;

        // #pragma omp for private(Cy, Cx, Zx, Zy, Zx2, Zy2, iX, iY, Iteration) schedule(static)
        // #pragma omp for private(Cy, Cx, Zx, Zy, Zx2, Zy2, iX, iY, Iteration) schedule(dynamic, grain)
        #pragma omp for private(Cy, Cx, Zx, Zy, Zx2, Zy2, iX, iY, Iteration) schedule(dynamic)
        // #pragma omp for private(Cy, Cx, Zx, Zy, Zx2, Zy2, iX, iY, Iteration) schedule(guided)
        // #pragma omp for private(Cy, Cx, Zx, Zy, Zx2, Zy2, iX, iY, Iteration) schedule(guided)
        for (iY = 0; iY < iYmax; iY++)
        {
            Cy = CyMin + iY * PixelHeight;
            if (fabs(Cy) < PixelHeight / 2)
                Cy = 0.0; 

            for (iX = 0; iX < iXmax; iX++)
            {
                Cx = CxMin + iX * PixelWidth;
                Zx = 0.0;
                Zy = 0.0;
                Zx2 = Zx * Zx;
                Zy2 = Zy * Zy;
                
                for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++)
                {
                    threadIterCount[threadNumber]++;
                    Zy = 2 * Zx * Zy + Cy;
                    Zx = Zx2 - Zy2 + Cx;
                    Zx2 = Zx * Zx;
                    Zy2 = Zy * Zy;
                }
                if (Iteration == IterationMax)
                { 
                    color[iY][iX][0] = 0;
                    color[iY][iX][1] = 0;
                    color[iY][iX][2] = 0;
                }
                else
                {                         
                    color[iY][iX][0] = r; 
                    color[iY][iX][1] = g; 
                    color[iY][iX][2] = b; 
                };
            }
        } 
    }

    double stop = omp_get_wtime();
    cout << "Time: " << stop - start << endl;

    for (int i = 0; i < nthreads; i++)
    {
        std::cout << threadIterCount[i] << ", ";
    }
    std::cout << '\n';

    writeImage("new.ppm");
}