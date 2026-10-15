#include "array_proc.h"
#include <stdlib.h>

int remove_multiples(int arr[], int size) {
    int write_idx = 0;
    for (int i = 0; i < size; i++) {
        if (arr[i] % 3 != 0 && arr[i] % 5 != 0) {
            arr[write_idx++] = arr[i];
        }
    }
    return write_idx;
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