#include "clevo_controller.h"
#include "clevo_hw_defs.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fstream>
#include <iostream>
#include <algorithm>

static int g_tuxedo_fd = -1;

extern "C" {

int clevo_init(void) {
    if (g_tuxedo_fd >= 0) return 0;
    g_tuxedo_fd = open(DEV_PATH, O_RDWR);
    return (g_tuxedo_fd >= 0) ? 0 : -1;
}

int clevo_get_fd(void) {
    return g_tuxedo_fd;
}

void clevo_close(void) {
    if (g_tuxedo_fd >= 0) {
        close(g_tuxedo_fd);
        g_tuxedo_fd = -1;
    }
}

int clevo_set_rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
    std::ofstream br_file(LED_BRIGHTNESS_PATH);
    if (!br_file.is_open()) return -1;
    br_file << static_cast<int>(brightness) << "\n";

    std::ofstream color_file(LED_COLOR_PATH);
    if (!color_file.is_open()) return -2;
    color_file << static_cast<int>(r) << " " 
               << static_cast<int>(g) << " " 
               << static_cast<int>(b) << "\n";

    return 0;
}

void parse_raw_fan(uint32_t raw, ClevoFanTelemetry* t) {
    t->duty_raw     = static_cast<uint8_t>(raw & 0xFF);
    t->temp         = static_cast<uint8_t>((raw >> 8) & 0xFF);
    t->raw_tach     = static_cast<uint16_t>((raw >> 16) & 0xFFFF);
    t->duty_percent = static_cast<uint8_t>((t->duty_raw / 255.0) * 100.0);
    
    if (t->raw_tach > 0 && t->raw_tach != 0xFFFF) {
        t->rpm = static_cast<uint16_t>(2156220 / t->raw_tach);
    } else {
        t->rpm = 0;
    }
}

int clevo_get_fan_telemetry(uint8_t fan_id, ClevoFanTelemetry* telemetry) {
    if (g_tuxedo_fd < 0 || !telemetry) return -1;

    unsigned long cmd;
    int32_t raw_val = 0;

    switch (fan_id) {
        case 1: cmd = R_CL_FANINFO1;
        case 2: cmd = R_CL_FANINFO2;
        default: cmd = R_CL_FANINFO1;
    }

    if (ioctl(g_tuxedo_fd, cmd, &raw_val) < 0) {
        return -2;
    }

    parse_raw_fan(static_cast<uint32_t>(raw_val), telemetry);
    return 0;
}

int clevo_set_fan_speed(uint8_t cpu_pct, uint8_t gpu_pct) {
    if (g_tuxedo_fd < 0) return -1;

    uint8_t cpu_duty = static_cast<uint8_t>((std::min<uint8_t>(cpu_pct, 100) / 100.0) * 255);
    uint8_t gpu_duty = static_cast<uint8_t>((std::min<uint8_t>(gpu_pct, 100) / 100.0) * 255);

    // Byte0: CPU PWM, Byte1: GPU PWM, Byte2: Fan3 (0)
    int32_t payload = (cpu_duty & 0xFF << 0) | ((gpu_duty & 0xFF) << 8);

    return ioctl(g_tuxedo_fd, W_CL_FANSPEED, &payload);
}

int clevo_set_fan_auto(void) {
    if (g_tuxedo_fd < 0) return -1;

    int32_t fan_ids[] = {0, 1, 0xFF};
    for (int32_t id : fan_ids) {
        ioctl(g_tuxedo_fd, W_CL_FANAUTO, &id);
    }
    return 0;
}

} // extern "C"