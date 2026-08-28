#pragma once

#include "clevo_controller.h"
#include <array>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <utility>

class ClevoFan {
public:
    static constexpr uint8_t TEMP_MIN = 30; // 30°C
    static constexpr uint8_t TEMP_MAX = 80; // 80°C
    static constexpr size_t CURVE_SIZE = (TEMP_MAX - TEMP_MIN) + 1; // 51 eleman

    explicit ClevoFan(unsigned long r_fan_info_cmd, uint8_t hysteresis = 2);

    // Telemetriyi okur, buffer'ı günceller ve nesne referansını döner
    int update_telemetry(int fd);
    const ClevoFanTelemetry& get_telemetry() const { return _telemetry; }

    // Curve Yönetimi
    void set_flat_curve(uint8_t default_pct);
    void set_linear_curve(const std::vector<std::pair<uint8_t, uint8_t>>& control_points);
    void set_raw_curve(const std::array<uint8_t, CURVE_SIZE>& raw_curve);
    
    // Anlık sıcaklığa ve histerezise göre hedef PWM duty (0-255) hesaplar
    uint8_t evaluate_target_duty();

private:
    unsigned long _r_fan_info_cmd;
    uint8_t _hysteresis;           // Sıcaklık düşüşlerinde devir dalgalanmasını önleyen eşik
    uint8_t _last_evaluated_temp;  // Son değerlendirilen stabil sıcaklık
    uint8_t _current_target_duty;  // Anlık uygulanan PWM duty (0-255)

    ClevoFanTelemetry _telemetry{};
    std::array<uint8_t, CURVE_SIZE> _curve{}; // Her 1°C için %0-100 hedefi tutar
};