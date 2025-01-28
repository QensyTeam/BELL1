#include <stddef.h> // Для определения NULL
#include <ctype.h>  // Для isdigit и isspace

unsigned long long strtoull(const char* str, char** endptr, int base) {
    const char* s = str;
    unsigned long long result = 0;
    unsigned long long prev_result = 0;

    // Пропускаем начальные пробельные символы
    while (isspace(*s)) {
        s++;
    }

    // Обрабатываем возможный знак числа
    if (*s == '+') {
        s++;
    } else if (*s == '-') {
        s++;  // Отрицательные числа игнорируются для unsigned long long
    }

    // Определяем базу (систему счисления)
    if (base == 0) {
        if (*s == '0') {
            if (*(s + 1) == 'x' || *(s + 1) == 'X') {
                base = 16;
                s += 2;
            } else {
                base = 8;
                s++;
            }
        } else {
            base = 10;
        }
    } else if (base == 16) {
        if (*s == '0' && (*(s + 1) == 'x' || *(s + 1) == 'X')) {
            s += 2;
        }
    }

    // Основной цикл для преобразования строки в число
    while (*s) {
        int digit = 0;

        if (isdigit(*s)) {
            digit = *s - '0';
        } else if (*s >= 'a' && *s <= 'f') {
            digit = *s - 'a' + 10;
        } else if (*s >= 'A' && *s <= 'F') {
            digit = *s - 'A' + 10;
        } else {
            break;  // Не является допустимым символом для данной системы счисления
        }

        if (digit >= base) {
            break;  // Символ превышает допустимое значение для данной базы
        }

        prev_result = result;
        result = result * base + digit;

        // Проверка на переполнение
        if (result < prev_result) {
            result = -1;  // Устанавливаем максимальное значение при переполнении
            break;
        }

        s++;
    }

    // Устанавливаем endptr на символ, который прервал анализ
    if (endptr) {
        *endptr = (char*)s;
    }

    return result;
}
