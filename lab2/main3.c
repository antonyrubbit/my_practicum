#include <stdio.h>
#include <ctype.h>
#include "io.h"

void rearrange(char *str) {
    char letters[256] = "";
    char digits[256] = "";
    int l_idx = 0, d_idx = 0;
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (isalpha(str[i])) {
            letters[l_idx++] = str[i];
        } else if (isdigit(str[i])) {
            digits[d_idx++] = str[i];
        }
    }
    letters[l_idx] = '\0';
    digits[d_idx] = '\0';
    
    sprintf(str, "%s%s", letters, digits); // Сначала буквы, потом цифры
}

int main() {
    char str[256];
    input_string(str);
    rearrange(str);
    output_string(str);
    return 0;
}