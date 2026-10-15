#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "array_io.h"
#include "array_proc.h"

#define SIZE 50

int main() {
    srand(time(NULL));
    int B[SIZE];
    fill_random(B, SIZE, -50, 50);
    printf("Исходный массив B:\n");
    print_array(B, SIZE);

    int new_size = remove_multiples(B, SIZE);
    printf("После удаления кратных 3 или 5:\n");
    print_array(B, new_size);
    return 0;
}