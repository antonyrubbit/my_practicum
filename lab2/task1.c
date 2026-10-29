#include <stdio.h>

int main() {
    for (int n = 10; n <= 99; n++) {
        int tens = n / 10;   // десятки
        int units = n % 10;  // единицы
        
        // Удвоенная сумма цифр равна произведению
        if (2 * (tens + units) == tens * units) {
            printf("%d ", n);
        }
    }
    printf("\n");
    return 0;
}