#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "array_io.h"
#include "array_proc.h"

#define SIZE 10

int main() {
    srand(time(NULL));
    int Q[SIZE];
    fill_random(Q, SIZE, -20, 20);
    printf("Массив Q:\n");
    print_array(Q, SIZE);

    int X;
    printf("Введите X: ");
    scanf("%d", &X);

    int found = 0;
    for (int i = 0; i < SIZE; i++) {
        if (Q[i] == X) { found = 1; break; }
    }

    if (found) {
        printf("Число %d найдено.\n", X);
    } else {
        int idx = find_closest(Q, SIZE, X);
        printf("Число %d не найдено. Ближайшее: %d (индекс %d)\n", X, Q[idx], idx);
    }
    return 0;
}