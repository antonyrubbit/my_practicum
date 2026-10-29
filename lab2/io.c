#include <stdio.h>
#include <string.h>
#include "io.h"

void input_string(char *str) {
    printf("Введите строку (буквы и цифры): ");
    fgets(str, 256, stdin);
    str[strcspn(str, "\n")] = 0;
}

void output_string(const char *str) {
    printf("Результат: %s\n", str);
}