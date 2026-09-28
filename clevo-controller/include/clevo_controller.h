#ifndef CLEVO_CONTROLLER_H
#define CLEVO_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t temp;          // Sıcaklık (°C)
    uint8_t duty_percent;  // Fan Hızı Yüzdesi (0-100)
    uint8_t duty_raw;      // Ham PWM Değeri (0-255)
    uint16_t rpm;          // Devir/Dakika
    uint16_t raw_tach;     // Takometre Periyodu (Tachometer Period)
} ClevoFanTelemetry;

// Donanım Başlatma ve Kapatma
int clevo_init(void);
void clevo_close(void);

// RGB Aydınlatma Kontrolü
int clevo_set_brightness(uint8_t br);
int clevo_set_rgb(uint8_t r, uint8_t g, uint8_t b);

// Fan Kontrolleri
int clevo_get_fan_telemetry(uint8_t fan_id, ClevoFanTelemetry* telemetry); // 1: CPU, 2: GPU
static inline int clevo_get_cpu_telemetry(ClevoFanTelemetry* telemetry) {return clevo_get_fan_telemetry(1, telemetry);}
static inline int clevo_get_gpu_telemetry(ClevoFanTelemetry* telemetry) {return clevo_get_fan_telemetry(2, telemetry);}
int clevo_set_fan_speed(uint8_t cpu_pct, uint8_t gpu_pct);                // 0-100%
int clevo_set_fan_auto(void);

void parse_raw_fan(uint32_t raw, ClevoFanTelemetry* t);
#ifdef __cplusplus
}
#endif

#endif // CLEVO_CONTROLLER_H