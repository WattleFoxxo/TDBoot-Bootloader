#ifndef BOOTLOADER_HOOKS_H
#define BOOTLOADER_HOOKS_H

void __attribute__((weak)) bootloader_before_init(void);

void __attribute__((weak)) bootloader_after_init(void);

#endif