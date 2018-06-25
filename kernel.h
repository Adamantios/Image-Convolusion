#pragma once

// define Kernel X size
int kernelX = 3;

// define Kernel Y size
int kernelY = 3;

// define a 2D 3x3 Kernel Array
unsigned char kernel2D[3][3] = {{0, 1,  0},                         /*  initializers for row indexed by 0 */
                                {1, -4, 1},                         /*  initializers for row indexed by 1 */
                                {0, 1,  0}};                       /*  initializers for row indexed by 2 */