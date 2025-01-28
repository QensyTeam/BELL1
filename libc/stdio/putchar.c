#include <stdio.h>

#if defined(__is_libk)
#include <kernel/gui/psf.h>
#include <kernel/drv/vbe.h> // Для управления цветами
#endif

#if defined(__is_libk)

// Разрешение экрана
int screen_width_print = 800;  // Ширина экрана в пикселях
int screen_height_print = 600; // Высота экрана в пикселях

#endif

int putchar(int ic) {
#if defined(__is_libk)
    char c = (char)ic;

    // Вычисляем максимальное количество символов по горизонтали и вертикали
    int max_columns = screen_width_print / CHAR_WIDTH;   // Символов по горизонтали
    int max_rows = screen_height_print / CHAR_HEIGHT;   // Символов по вертикали

    if (c == '\n') { 
        disable_cursor(); // Отключаем курсор
        // Обработка перевода строки
        cursor_x = 0;     
        cursor_y++;
    } else if (c == '\t') { 
        disable_cursor(); // Отключаем курсор
        // Обработка табуляции
        int tab_size = 4; // Например, табуляция каждые 4 символа
        int spaces_to_next_tab = tab_size - (cursor_x % tab_size);
        for (int i = 0; i < spaces_to_next_tab; i++) {
            fb_putchar(' ', cursor_x, cursor_y, fg_color, bg_color);
            cursor_x++;
        }
    } else if (c == '\b') { 
        disable_cursor(); // Отключаем курсор
        // Обработка стирания
        if (cursor_x > 0) { 
            cursor_x--; // Перемещаем курсор назад
        } else if (cursor_y > 0) { 
            // Если в начале строки, переносим курсор на предыдущую строку
            cursor_y--;
            cursor_x = max_columns - 1; // Перемещаем курсор на последнюю позицию строки
        }
        // Заменяем текущий символ пробелом
        fb_putchar(' ', cursor_x, cursor_y, fg_color, bg_color);
    } else { 
        disable_cursor(); // Отключаем курсор
        // Вывод обычного символа
        fb_putchar((unsigned short)c, cursor_x, cursor_y, fg_color, bg_color);
        cursor_x++; // Перемещаем курсор вправо
    }
    enable_cursor(); // Включаем курсор

    // Если курсор выходит за пределы строки, переносим на следующую
    if (cursor_x >= max_columns) { 
        disable_cursor(); // Отключаем курсор
        cursor_x = 0;
        cursor_y++;
    }

    // Если курсор выходит за пределы экрана по вертикали, сбрасываем его (можно добавить скроллинг)
    if (cursor_y >= max_rows) { 
        disable_cursor(); // Отключаем курсор
        cursor_y = 0; // Сбрасываем в начало экрана
        scroll_screen(); // Прокручиваем экран
    }

#else
    // TODO: Implement stdio and the write system call.
#endif
    return ic;
}
