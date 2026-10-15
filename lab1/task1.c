#include <stdio.h>

int has_distinct_digits(int n) {
    int d[10] = {0};
    while (n > 0) {
        int digit = n % 10;
        if (d[digit]) return 0;
        d[digit] = 1;
        n /= 10;
    }
    return 1;
}

int main() {
    for (int i = 1000; i <= 9999; i++) {
        if (has_distinct_digits(i))
            printf("%d ", i);
    }
    printf("\n");
    return 0;
}