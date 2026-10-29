#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Функция ввода строки
void input_string(char *str) {
    printf("Введите строку: ");
    fgets(str, 256, stdin);
    str[strcspn(str, "\n")] = 0; // Удаляем символ новой строки
}

// Функция вывода строки
void output_string(const char *str) {
    printf("Результат: %s\n", str);
}

// Функция фильтрации
void filter_words(char *str) {
    char result[256] = "";
    char *token = strtok(str, " ");
    
    while (token != NULL) {
        // Ищем только английские 'A' и 'a' с помощью strchr
        if (strchr(token, 'A') || strchr(token, 'a')) {
            strcat(result, token);
            strcat(result, " ");
        }
        token = strtok(NULL, " ");
    }
    strcpy(str, result); // Копируем результат обратно
}

int main() {
    char str[256];
    input_string(str);
    filter_words(str);
    output_string(str);
    return 0;
}