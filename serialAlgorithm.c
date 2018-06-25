#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "image.h"
#include "kernel.h"
#include "memoryAllocation.h"

// Performs serial convolution
void serial_convolution(unsigned char in[100][100], unsigned char** out, int dataSizeX, int dataSizeY,
	unsigned char kernel[3][3], int kernelSizeX, int kernelSizeY){
	int x, y, k, j;

	// scan the image
	for (x = 0; x < dataSizeX; x++)
	for (y = 0; y < dataSizeY; y++) {

		// initialize to zero
		out[y][x] = 0;

		// scan the kernel
		for (k = -1; k < kernelSizeX - 1; k++)
		for (j = -1; j < kernelSizeY - 1; j++){

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
}

int main(int argc, char* argv[]){
	unsigned char **outBuf1;
	outBuf1 = Make2DDoubleArray();

	clock_t begin, end;
	double time;

	// perform convolution with the serial algorithm and count the time it needed to complete
	begin = clock();
	serial_convolution(image2D, outBuf1, imageX, imageY, kernel2D, kernelX, kernelY);
	end = clock();
	time = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Serial Convolution: %f sec\n", time);

	printf("Result: ");
	
	// print the result
	int i, j;
	for (i = 0; i < imageX; i++)
	for (j = 0; j < imageY; j++)
		printf("%c", outBuf1[i][j]);

	printf("\n");

	// free the allocated memory
	for (i = 0; i < imageX; i++){
		free(outBuf1[i]);
	}
	free(outBuf1);

	return 0;
}
