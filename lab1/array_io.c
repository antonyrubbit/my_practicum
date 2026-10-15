#include <stdio.h>
#include <stdlib.h>
#include "array_io.h"

void fill_random(int arr[], int size, int min, int max) {
    for (int i = 0; i < size; i++)
        arr[i] = rand() % (max - min + 1) + min;
}

void print_array(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%4d ", arr[i]);
    printf("\n");
}