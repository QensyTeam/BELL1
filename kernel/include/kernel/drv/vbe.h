#ifndef VBE_H
#define VBE_H

#include <stdint.h>

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_color_t;

#define RGB(r, g, b) ((rgb_color_t){(r), (g), (b)})

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

#define VBE_DISPI_ENABLED      0x01
#define VBE_DISPI_LFB_ENABLED  0x40
#define VBE_DISPI_GETCAPS   0x02
#define VBE_DISPI_8BIT_DAC  0x20

#define VBE_DISPI_LFB_ENABLED       0x40
#define VBE_DISPI_NOCLEARMEM        0x80

#define VBE_DISPI_LFB_PHYSICAL_ADDRESS 0xB8000

#define CHAR_WIDTH 8  // Ширина символа в пикселях
#define CHAR_HEIGHT 16  // Высота символа в пикселях

void set_video_mode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t* fb_addr);
void putpixel(int x, int y, rgb_color_t color);
uint32_t getpixel(int x, int y);
void fill_screen(rgb_color_t color);

extern uint16_t screen_width;
extern uint16_t screen_height;
extern uint32_t* framebuffer;

extern volatile int cursor_x;  // Текущая x-координата курсора
extern volatile int cursor_y;  // Текущая y-координата курсора

static const rgb_color_t fg_color = {0, 183, 235}; // Цвет текста
static const rgb_color_t bg_color = {0, 0, 0};     // Цвет фона

void enable_cursor();
void disable_cursor();
void update_cursor_position();
void scroll_screen();
void vbe_clear_screen(rgb_color_t color);

#endif // VBE_H
