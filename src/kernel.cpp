#include <graphics/vbe.h>
#include <kernelUtils.h>
#include <libc/stdio.h>

extern "C" __attribute__((section("kernel_entry"))) void kMain(void) {
    const char* copyMSG = "-- DiskOS v0.1 - Copyright Michal Pazurek 2022 --\n\r"
                          "--   Github:  https://github.com/xxMichalPk/   --\n\r\n\r";
    const char* prompt = "diOS:> ";
    initKernel();
    vbe->ColorsChange(0xFF22FFAA, bgColor);
    printf("%s", copyMSG);
    vbe->ColorsChange(0xFFFF2050, bgColor);
    printf("%s", prompt);
    vbe->ColorsChange(0xFFDDDDDD, bgColor);
    for (;;);
}