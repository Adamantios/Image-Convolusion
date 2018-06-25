#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "image.h"
#include "kernel.h"
#include "memoryAllocation.h"

#define MASTER 1
#define SLAVE 2

// Performs MPI convolution
void mpi_convolution(unsigned char in[100][100], unsigned char **out, int dataSizeX, int dataSizeY,
                     unsigned char kernel[3][3], int kernelSizeX, int kernelSizeY, char argc, char **argv) {

    MPI_Init(&argc, &argv);

    int taskId, numOfWorkers, type, offset, status, rowStart, rowEnd, columnStart, columnEnd, x, y, k, j;
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfWorkers);

    /**************************************MASTER**************************************/

    // Send matrix to the worker tasks with an offset
    // depending on the number of the available workers
    if (taskId == 0) {
        type = MASTER;

        for (offset = 1; offset <= numOfWorkers; offset++) {
            MPI_Send(&in[], 1, MPI_CHAR, offset, MPI_ANY_TAG, MPI_COMM_WORLD);
            MPI_Send(&offset, 1, MPI_INT, offset, MPI_ANY_TAG, MPI_COMM_WORLD);
        }

        // Receive results from the worker tasks
        type = SLAVE;

        for (offset = 1; offset <= numOfWorkers; offset++)
            MPI_Recv(&out, 1, MPI_CHAR, offset, type, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }

    /**************************************SLAVES**************************************/

    if (taskId > 0) {
        // Receive matrix and offset from the master task
        type = MASTER;
        MPI_Recv(&in, 1, MPI_CHAR, 0, MPI_ANY_SOURCE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&offset, 1, MPI_INT, 0, MPI_ANY_SOURCE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        rowStart = (taskId - 1) * dataSizeX / numOfWorkers;
        rowEnd = taskId * dataSizeX / numOfWorkers;
        columnStart = (taskId - 1) * dataSizeY / numOfWorkers;
        columnEnd = taskId * dataSizeY / numOfWorkers;

        /**Perform convolution for each part of the array**/

        // scan the image
        for (x = rowStart; x < rowEnd; x++)
            for (y = columnStart; y < columnEnd; y++) {

                // initialize to zero
                out[y][x] = 0;

                // scan the kernel
                for (k = -1; k < kernelSizeX - 1; k++)
                    for (j = -1; j < kernelSizeY - 1; j++) {

                        // if out of bounds, continue
                        if (x + k >= 0 && x + k < dataSizeY && y + j >= 0 && y + j < dataSizeX)
                            continue;

                        // perform convolution for one pixel
                        out[y][x] += kernel[j + 1][k + 1] * in[y - j][x - k];

                        // if result is bigger than 255, make it 255
                        if (out[y][x] > 255)
                            out[y][x] = 255;
                    }
            }

        // Send results to the master task
        type = SLAVE;
        MPI_Send(&out, 1, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}

int main(int argc, char *argv[]) {
    unsigned char **outBuf1;
    outBuf1 = Make2DDoubleArray();

    clock_t begin, end;
    double time;

    // perform convolution with the MPI algorithm and count the time it needed to complete
    begin = clock();
    mpi_convolution(image2D, outBuf1, imageX, imageY, kernel2D, kernelX, kernelY, argc, argv);
    end = clock();
    time = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("MPI Convolution: %f sec\n", time);

    printf("Result: ");

    // print the result
    int i, j;
    for (i = 0; i < imageX; i++)
        for (j = 0; j < imageY; j++)
            printf("%c", outBuf1[i][j]);

    printf("\n");

    // free the allocated memory
    for (i = 0; i < imageX; i++) {
        free(outBuf1[i]);
    }
    free(outBuf1);

    return 0;
}