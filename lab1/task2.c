#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 50

void fill_random(int arr[], int size) {
    for (int i = 0; i < size; i++)
        arr[i] = rand() % 101 - 50; // от -50 до 50
}

void print_array(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%4d ", arr[i]);
    printf("\n");
}

int remove_multiples(int arr[], int size) {
    int write_idx = 0;
    for (int i = 0; i < size; i++) {
        if (arr[i] % 3 != 0 && arr[i] % 5 != 0) {
            arr[write_idx++] = arr[i];
        }
    }
    return write_idx;
}

int main() {
    srand(time(NULL));
    int B[SIZE];
    fill_random(B, SIZE);
    printf("Исходный массив B:\n");
    print_array(B, SIZE);

    int new_size = remove_multiples(B, SIZE);
    printf("После удаления кратных 3 или 5:\n");
    print_array(B, new_size);
    return 0;
}