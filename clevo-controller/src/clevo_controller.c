#include "clevo_controller.h"
#include "clevo_hw_defs.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

static int g_tuxedo_fd = -1;


int clevo_init(void) {
    if (g_tuxedo_fd >= 0) return 0;
    
    g_tuxedo_fd = open(DEV_PATH, O_RDWR);
    if (g_tuxedo_fd < 0) return -1;

    return 0;
}

void clevo_close(void) {
    if (g_tuxedo_fd < 0) return;

    close(g_tuxedo_fd);
    g_tuxedo_fd = -1;
}

// Tamamen Saf C: sysfs brightness düğümüne yazar
int clevo_set_brightness(uint8_t br) {
    int fd = open(LED_BRIGHTNESS_PATH, O_WRONLY);
    if (fd < 0) return -1;

    char buf[16];
    int len = snprintf(buf, sizeof(buf), "%u\n", (unsigned int)br);
    if (len <= 0) {
        close(fd);
        return -2;
    }

    ssize_t written = write(fd, buf, (size_t)len);
    close(fd);

    if (written != (ssize_t)len) return -3;
    return 0;
}

// Tamamen Saf C: sysfs multi_intensity düğümüne yazar (R G B)
int clevo_set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    int fd = open(LED_COLOR_PATH, O_WRONLY);
    if (fd < 0) return -1;

    char buf[32];
    int len = snprintf(buf, sizeof(buf), "%u %u %u\n", 
                       (unsigned int)r, (unsigned int)g, (unsigned int)b);
    if (len <= 0) {
        close(fd);
        return -2;
    }

    ssize_t written = write(fd, buf, (size_t)len);
    close(fd);

    if (written != (ssize_t)len) return -3;
    return 0;
}

void parse_raw_fan(uint32_t raw, ClevoFanTelemetry* t) {
    if (!t) return;

    t->duty_raw     = (uint8_t)(raw & 0xFF);
    t->temp         = (uint8_t)((raw >> 8) & 0xFF);
    t->raw_tach     = (uint16_t)((raw >> 16) & 0xFFFF);
    t->duty_percent = (uint8_t)(((double)t->duty_raw / 255.0) * 100.0);
    
    if (t->raw_tach > 0 && t->raw_tach != 0xFFFF) {
        t->rpm = (uint16_t)(2156220 / t->raw_tach);
    } else {
        t->rpm = 0;
    }
}

int clevo_get_fan_telemetry(uint8_t fan_id, ClevoFanTelemetry* telemetry) {
    if (g_tuxedo_fd < 0 || !telemetry) return -1;

    unsigned long cmd;
    int32_t raw_val = 0;

    switch (fan_id) {
        case 1:     cmd = R_CL_FANINFO1; break;
        case 2:     cmd = R_CL_FANINFO2; break;
    }

    if (ioctl(g_tuxedo_fd, cmd, &raw_val) < 0) {
        return -2;
    }

    parse_raw_fan((uint32_t)raw_val, telemetry);
    return 0;
}

int clevo_set_fan_speed(uint8_t cpu_pct, uint8_t gpu_pct) {
if (g_tuxedo_fd < 0) return -1;

    uint8_t cpu_clamped = (cpu_pct > 100) ? 100 : cpu_pct;
    uint8_t gpu_clamped = (gpu_pct > 100) ? 100 : gpu_pct;

    uint8_t cpu_duty = (uint8_t)(((double)cpu_clamped / 100.0) * 255.0);
    uint8_t gpu_duty = (uint8_t)(((double)gpu_clamped / 100.0) * 255.0);

    int32_t payload = (int32_t)cpu_duty | ((int32_t)gpu_duty << 8);

    if (ioctl(g_tuxedo_fd, W_CL_FANSPEED, &payload) < 0) {
        return -2;
    }
    return 0;
}

int clevo_set_fan_auto(void) {
    if (g_tuxedo_fd < 0) return -1;

    int32_t fan_ids[] = {0, 1, 0xFF};
    for (size_t i = 0; i < sizeof(fan_ids) / sizeof(fan_ids[0]); ++i) {
        ioctl(g_tuxedo_fd, W_CL_FANAUTO, &fan_ids[i]);
    }
    return 0;
}
