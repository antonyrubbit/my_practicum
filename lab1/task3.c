#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SIZE 10

void fill_random(int arr[], int size) {
    for (int i = 0; i < size; i++)
        arr[i] = rand() % 41 - 20; // от -20 до 20
}

void print_array(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%4d ", arr[i]);
    printf("\n");
}

int find_closest(int arr[], int size, int X) {
    int best_idx = 0;
    int best_diff = abs(arr[0] - X);
    for (int i = 1; i < size; i++) {
        int diff = abs(arr[i] - X);
        if (diff < best_diff) {
            best_diff = diff;
            best_idx = i;
        }
    }
    return best_idx;
}

int main() {
    srand(time(NULL));
    int Q[SIZE];
    fill_random(Q, SIZE);
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