#include <stdbool.h>
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "bootloader_init.h"
#include "bootloader_utility.h"
#include "bootloader_common.h"
#include "bootloader_hooks.h"

static const char* TAG = "boot";

static int select_partition_number(bootloader_state_t *bs);

void __attribute__((noreturn)) call_start_cpu0(void) {
    if (bootloader_before_init) {
        bootloader_before_init();
    }

    if (bootloader_init() != ESP_OK) {
        bootloader_reset();
    }

    if (bootloader_after_init) {
        bootloader_after_init();
    }

    ESP_LOGI(TAG, "Custom bootloader for TDBoot");

    #ifdef CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP
        bootloader_utility_load_boot_image_from_deep_sleep();
    #endif

    bootloader_state_t bs = {0};

    int boot_index = select_partition_number(&bs);
    if (boot_index == INVALID_INDEX) {
        bootloader_reset();
    }

    ESP_LOGE(TAG, "!!!Restart reason!!! %d", esp_rom_get_reset_reason(0));

    if (esp_rom_get_reset_reason(0) != RESET_REASON_CPU0_SW) {
        boot_index = -1;
    }

    bootloader_utility_load_boot_image(&bs, boot_index);
}

static int select_partition_number(bootloader_state_t *bs) {
    if (!bootloader_utility_load_partition_table(bs)) {
        ESP_LOGE(TAG, "load partition table error!");
        return INVALID_INDEX;
    }

    return bootloader_utility_get_selected_boot_partition(bs);
}

struct _reent *__getreent(void) {
    return _GLOBAL_REENT;
}