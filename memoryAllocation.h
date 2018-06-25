#pragma once

// Allocates memory for a 2D array with size 100x100 and returns it
unsigned char **Make2DDoubleArray() {
    unsigned char **theArray;

    if ((theArray = (unsigned char **) malloc(imageX * sizeof(unsigned char *))) == NULL) {
        printf("ERROR: Memory Allocation Failed.\n");
        exit(0);
    }

    int i;
    for (i = 0; i < imageX; i++)
        if ((theArray[i] = (unsigned char *) malloc(imageY * sizeof(unsigned char))) == NULL) {
            printf("ERROR: Memory Allocation Failed.\n");
            exit(0);
        }

    return theArray;
}