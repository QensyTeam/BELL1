#include <stdio.h>
#include <kernel/kernel.h>
#include <kernel/apps/shell.h>

#define HEAP_START_ADDRESS 0x1000000

void irq_disable();
void irq_enable();

extern size_t heap_used;
extern size_t heap_size;


void kernel_main(__attribute__((unused)) multiboot_info_t* multiboot_info) {
    vbe_mode_info_t* vbe_mode_info = (vbe_mode_info_t*) multiboot_info->vbe_mode_info;
    uint32_t framebuffer_address = vbe_mode_info->physbase;

    // Рассчитываем размер кучи
    size_t heap_size = calculate_heap_size(multiboot_info);

    serial_port_init(COM1);

    // Инициализируем кучу
    kheap_init((void*)HEAP_START_ADDRESS, heap_size);

    set_video_mode(800, 600, 32, (uint32_t*)framebuffer_address);

    psf_v1_init();

    irq_disable();
    
    gdt_init();
    idt_init(GDT_CODE_SEL_1);

    pic_init();
    timer_init();

    irq_enable();
    vbe_clear_screen(RGB(0, 0, 0));
    print_bell_logo();
    ps2_init();
    ps2_keyboard_preinit();
    ps2_keyboard_init();

    shell();
}

