#include <stdio.h>
#include "kernel/kernel.h"
#include "kernel/drv/keyboard_buffer.h"
#include <stdarg.h>

// Вспомогательная функция для обработки ввода символа с поддержкой Backspace
static void scan_char(char* c) {
    *c = keyboard_get_char();  // Получаем символ из буфера
    if (*c == '\b') {  // Если символ - Backspace
        // Удаляем последний символ (перемещаем курсор назад)
        putchar('\b');  // Перемещаем курсор назад
        putchar(' ');   // Печатаем пробел, чтобы стереть символ
        putchar('\b');  // Снова перемещаем курсор назад
        // Не записываем символ в переменную c, поскольку он был удален
    } else {
        putchar(*c);  // Печатаем символ
    }
}


// Вспомогательная функция для обработки ввода целого числа
// Вспомогательная функция для обработки ввода целого числа с поддержкой Backspace
static void scan_int(int* num, int base) {
    char c;
    int sign = 1;  // По умолчанию положительное число
    *num = 0;

    // Пропускаем пробелы
    while ((c = keyboard_get_char()) == ' ' || c == '\n' || c == '\r');

    // Проверка на знак
    if (c == '-') {
        sign = -1;  // Отрицательное число
        c = keyboard_get_char();
    } else if (c == '+') {
        c = keyboard_get_char();  // Пропускаем знак '+'
    }

    // Чтение цифр
    while ((c >= '0' && c <= '9') || c == '\b') {  // Обрабатываем Backspace
        if (c == '\b') {  // Обработка Backspace
            if (*num > 0) {  // Если есть что удалять
                *num /= 10;  // Удаляем последнюю цифру
                putchar('\b');  // Перемещаем курсор назад
                putchar(' ');   // Печатаем пробел для стирания
                putchar('\b');  // Возвращаем курсор
            }
        } else {
            *num = *num * base + (c - '0');  // Конвертация символа в цифру и добавление её к числу
            putchar(c);  // Печатаем символ
        }
        c = keyboard_get_char();
    }

    *num *= sign;  // Применяем знак
}


// Функция для обработки строк
static void scan_string(char* str, size_t max_len) {
    char c;
    size_t i = 0;

    // Пропускаем пробелы
    while ((c = keyboard_get_char()) == ' ' || c == '\n' || c == '\r');

    // Читаем строку
    while (c != '\n' && c != '\r' && i < max_len - 1) {
        if (c == '\b') {
            // Обработка Backspace: стираем последний символ, если есть что стирать
            if (i > 0) {
                i--;
                putchar('\b');
                putchar(' ');  // Печатаем пробел, чтобы стереть символ
                putchar('\b');
            }
        } else {
            str[i++] = c;
            putchar(c);  // Печатаем символ
        }
        c = keyboard_get_char();
    }
    str[i] = '\0';  // Завершаем строку
}

// Основная функция scanf
int scanf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    const char* p = format;
    char c;
    while (*p != '\0') {
        if (*p == '%') {
            p++;  // Переходим к следующему символу после '%'

            if (*p == 'd') {  // Чтение целого числа
                int* i = va_arg(args, int*);
                scan_int(i, 10);  // 11 - максимальное количество цифр в int
            } else if (*p == 's') {  // Чтение строки
                char* str = va_arg(args, char*);
                scan_string(str, 256);  // 256 - максимальная длина строки
            } else if (*p == 'c') {  // Чтение одного символа
                char* ch = va_arg(args, char*);
                scan_char(ch);
            }
        } else {
            // Пропускаем символы, не связанные с форматами
            c = keyboard_get_char();
            putchar(c);  // Печатаем символ
            if (c != *p) {
                continue;
            }
        }
        p++;
    }

    va_end(args);
    return 0;
}
