#include <kernel/kernel.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define HISTORY_SIZE 10 // Количество команд в истории

static char command_history[HISTORY_SIZE][100]; // Массив для хранения истории команд
static int history_count = 0; // Количество сохранённых команд
static int history_index = -1; // Текущий индекс для просмотра истории

void print_bell_logo() {
    printf("\n");
    printf("  *=============================================================*\n");
    printf("  *       :::::::::       ::::::::::       :::        :::       *\n");
    printf("  *      :+:    :+:      :+:              :+:        :+:        *\n");
    printf("  *     +:+    +:+      +:+              +:+        +:+         *\n");
    printf("  *    +#++:++#+       +#++:++#         +#+        +#+          *\n");
    printf("  *   +#+    +#+      +#+              +#+        +#+           *\n");
    printf("  *  #+#    #+#      #+#              #+#        #+#            *\n");
    printf("  * #########       ##########       ########## ##########      *\n");
    printf("  *=============================================================*\n");
    printf("  *       :::::::                :::::::                  :::   *\n");
    printf("  *     :+:   :+:              :+:   :+:               :+:+:    *\n");
    printf("  *    +:+   +:+              +:+   +:+                 +:+     *\n");
    printf("  *   +#+   +:+              +#+   +:+                 +#+      *\n");
    printf("  *  +#+   +#+              +#+   +#+                 +#+       *\n");
    printf("  * #+#   #+#      #+#     #+#   #+#      #+#        #+#        *\n");
    printf("  * #######       ###      #######       ###      #######       *\n");
    printf("  *=============================================================*\n\n");
}

void print_help() {
    printf("\n\nAvailable commands:\n");
    printf("  help            - Show this help message\n");
    printf("  clear           - Clear the screen\n");
    printf("  echo <text>     - Displays the text you enter\n");
    printf("  logo            - Displays logo\n");
    printf("  reboot          - Rebooting PC\n");
    printf("  off             - Shutdown PC\n");
}

// Добавляем команду в историю
void add_to_history(const char* command) {
    if (strlen(command) == 0) return; // Пустые команды не сохраняем

    // Если история заполнена, сдвигаем элементы
    if (history_count == HISTORY_SIZE) {
        for (int i = 1; i < HISTORY_SIZE; i++) {
            strcpy(command_history[i - 1], command_history[i]);
        }
        history_count--;
    }

    // Сохраняем новую команду в историю
    strcpy(command_history[history_count], command);
    history_count++;
    history_index = history_count; // Сбрасываем индекс просмотра истории
}

// Функция для очистки текущей строки
void clear_current_line(size_t command_pos) {
    for (size_t i = 0; i < command_pos; i++) {
        printf("\b \b"); // Удаляем каждый символ текущей строки
    }
}

void browse_history(char* command, size_t* command_pos, int direction) {
    if (direction == -1) { // Стрелка вверх
        if (history_index > 0) {
            history_index--;
        }
    } else if (direction == 1) { // Стрелка вниз
        if (history_index < history_count - 1) {
            history_index++;
        }
    }

    // Если индекс в пределах истории, выводим команду
    if (history_index >= 0 && history_index < history_count) {
        clear_current_line(*command_pos); // Очищаем строку
        strcpy(command, command_history[history_index]);
        *command_pos = strlen(command);
        printf("%s", command); // Выводим команду из истории
    } 
}

// Функция для перемещения курсора
void move_cursor_left(size_t* command_pos) {
    disable_cursor(); // Отключаем курсор
    if (*command_pos > 0) {
        (*command_pos)--;
        cursor_x--;
    }
    enable_cursor(); // Включаем курсор
}

void move_cursor_right(size_t* command_pos, size_t command_length) {
    disable_cursor(); // Отключаем курсор
    if (*command_pos < command_length) {
        (*command_pos)++;
        cursor_x++;
    }
    enable_cursor(); // Включаем курсор
}

// Сдвигаем текст после позиции курсора вправо для вставки нового символа
void shift_text_right(char* command, size_t command_pos, size_t command_length) {
    for (size_t i = command_length; i > command_pos; i--) {
        command[i] = command[i - 1];
    }
}


void shell() {
    // Цикл для работы с консолью
    char command[100];
    size_t command_pos = 0;  // Позиция курсора в строке
    size_t command_length = 0;  // Длина введенной команды

    while (1) {
        printf("\nShell> ");
        command_pos = 0;  // Сброс позиции курсора

        // Очищаем буфер команды
        for (size_t i = 0; i < sizeof(command); i++) {
            command[i] = '\0';
        }

        while (1) {
            uint16_t c = keyboard_get_char();  // Получаем символ с клавиатуры

            if (c == '\n') {  // Если нажата клавиша Enter
                break;
            } else if (c == '\b') {  // Если нажата клавиша Backspace
                if (command_pos > 0) {
                    command_pos--;
                    command[command_pos] = '\0';  // Удаляем символ из буфера
                    printf("\b \b");  // Удаляем символ с экрана
                    command_length--;  // Уменьшаем длину команды
                }
            } else if (c == '\x1B') {  // ESC
                c = keyboard_get_char();  // Читаем следующий символ после ESC
                if (c == '[') {
                    c = keyboard_get_char();  // Читаем следующий символ после [
                    if (c == 'A') {  // Стрелка вверх
                        browse_history(command, &command_pos, -1);
                    } else if (c == 'B') {  // Стрелка вниз
                        browse_history(command, &command_pos, 1);
                    } else if (c == 'C') {  // Стрелка вправо
                        move_cursor_right(&command_pos, command_length);
                    } else if (c == 'D') {  // Стрелка влево
                        move_cursor_left(&command_pos);
                    }
                }
            } else if (c >= 32 && c <= 126) {  // Печатные символы
                if (command_pos < sizeof(command) - 1) {
                    // Если курсор не в конце строки, сдвигаем символы
                    if (command_pos < command_length) {
                        shift_text_right(command, command_pos, command_length); // Сдвигаем текст вправо
                    }

                    // Вставляем символ в строку
                    command[command_pos++] = (char)c;
                    command_length = (command_pos > command_length) ? command_pos : command_length;  // Обновляем длину строки
                    printf("%c", c);  // Печатаем символ
                }
            }
        }

        if (command_pos == 0) {
            continue;  // Если строка пуста, повторяем приглашение
        }

        add_to_history(command);  // Добавляем команду в историю

        // Обработка команд
        if (strcmp(command, "help") == 0) {
            print_help();
        } else if (strcmp(command, "off") == 0) {
            printf("\nShutting down...\n");
            sleep(1000);
            outw(0x604, 0x2000);
        } else if (strcmp(command, "reboot") == 0) {
            printf("\nRebooting...\n");
            sleep(1000);
            outb(0x64, 0xFE);
        } else if (strcmp(command, "logo") == 0) {
            vbe_clear_screen(RGB(0, 0, 0));
            print_bell_logo();
        } else if (strcmp(command, "clear") == 0) {
            vbe_clear_screen(RGB(0, 0, 0));
        } else if (strncmp(command, "echo ", 5) == 0) {  // Проверяем команду echo
            printf("\n%s\n", command + 5);  // Выводим текст после echo
        } else {
            printf("\nUnknown command: %s\n", command);
            printf("Type 'help' for a list of available commands.\n");
        }
    }
}
