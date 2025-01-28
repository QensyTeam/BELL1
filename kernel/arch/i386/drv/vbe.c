#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/kernel.h>

uint16_t screen_width;
uint16_t screen_height;
uint32_t* framebuffer;

static void vbe_write(uint16_t index, uint16_t data) {
    __asm__ volatile ("outw %0, %1" : : "a"(index), "Nd"(VBE_DISPI_IOPORT_INDEX));
    __asm__ volatile ("outw %0, %1" : : "a"(data), "Nd"(VBE_DISPI_IOPORT_DATA));
}

void set_video_mode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t* fb_addr) {
    screen_width = width;
    screen_height = height;
    framebuffer = fb_addr;

    vbe_write(0x00, 0x4F02); 
    vbe_write(0x01, width);   
    vbe_write(0x02, height);  
    vbe_write(0x03, bpp);     
    vbe_write(0x04, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED); 
}

void putpixel(int x, int y, rgb_color_t color) {
    if (x >= 0 && x < screen_width && y >= 0 && y < screen_height) {
        uint32_t pixel_offset = y * screen_width + x;
        framebuffer[pixel_offset] = ((uint32_t)color.red << 16) | ((uint32_t)color.green << 8) | (uint32_t)color.blue;
    }
}

uint32_t getpixel(int x, int y) {
    if (x >= 0 && x < screen_width && y >= 0 && y < screen_height) {
        uint32_t pixel_offset = y * screen_width + x;
        return framebuffer[pixel_offset];
    }

    return 0;
}


void fill_screen(rgb_color_t color) {
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, color);
        }
    }
}

volatile int cursor_x = 0;  // Текущая x-координата курсора
volatile int cursor_y = 0;  // Текущая y-координата курсора
volatile bool cursor_visible = true; // Видимость курсора

void enable_cursor() {
    // Рисуем курсор как нижнее подчеркивание высотой 2 пикселя
    draw_Rectangle(cursor_x * CHAR_WIDTH, (cursor_y + 1) * CHAR_HEIGHT - 1, CHAR_WIDTH, 1, fg_color);
}

void disable_cursor() {
    // Очищаем то место, где был курсор
    draw_Rectangle(cursor_x * CHAR_WIDTH, (cursor_y + 1) * CHAR_HEIGHT - 1, CHAR_WIDTH, 1, bg_color);
}

void update_cursor_position() {
    if (cursor_visible) {
        disable_cursor();
        enable_cursor();
    }
}

void scroll_screen() {
    // Сдвигаем все строки на одну вверх
    for (int y = 0; y < screen_height - CHAR_HEIGHT; y++) {
        for (int x = 0; x < screen_width; x++) {
            framebuffer[y * screen_width + x] = framebuffer[(y + CHAR_HEIGHT) * screen_width + x];
        }
    }

    // Очищаем последнюю строку
    for (int y = screen_height - CHAR_HEIGHT; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, bg_color);
        }
    }

    // Перемещаем курсор на последнюю строку
    cursor_y = (screen_height / CHAR_HEIGHT) - 1;
    cursor_x = 0;
}

void vbe_clear_screen(rgb_color_t color) {
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, color);  // Заполняем каждый пиксель экрана заданным цветом
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}