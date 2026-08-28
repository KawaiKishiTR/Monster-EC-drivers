#include "clevo_fan.hpp"
#include "clevo_controller.h"
#include <sys/ioctl.h>
#include <cmath>

ClevoFan::ClevoFan(unsigned long r_fan_info_cmd, uint8_t hysteresis)
    : _r_fan_info_cmd(r_fan_info_cmd),
      _hysteresis(hysteresis),
      _last_evaluated_temp(0),
      _current_target_duty(0) {
    // Varsayılan güvenli doğrusal eğri: 30°C -> %30, 80°C -> %100
    set_linear_curve({{30, 30}, {55, 50}, {70, 75}, {80, 100}});
}

int ClevoFan::update_telemetry(int fd) {
    if (fd < 0) return -1;

    int32_t raw_val = 0;
    if (ioctl(fd, _r_fan_info_cmd, &raw_val) < 0) {
        return -2;
    }

    parse_raw_fan(static_cast<uint32_t>(raw_val), &_telemetry);

    return 0;
}

void ClevoFan::set_flat_curve(uint8_t default_pct) {
    uint8_t clamped = std::min<uint8_t>(default_pct, 100);
    _curve.fill(clamped);
}

void ClevoFan::set_raw_curve(const std::array<uint8_t, CURVE_SIZE>& raw_curve) {
    for (size_t i = 0; i < CURVE_SIZE; ++i) {
        _curve[i] = std::min<uint8_t>(raw_curve[i], 100);
    }
}

void ClevoFan::set_linear_curve(const std::vector<std::pair<uint8_t, uint8_t>>& points) {
    if (points.empty()) return;

    // Noktaları sıcaklığa göre sırala
    auto sorted_pts = points;
    std::sort(sorted_pts.begin(), sorted_pts.end(), 
              [](const auto& a, const auto& b) { return a.first < b.first; });

    for (uint8_t t = TEMP_MIN; t <= TEMP_MAX; ++t) {
        size_t idx = t - TEMP_MIN;

        if (t <= sorted_pts.front().first) {
            _curve[idx] = std::min<uint8_t>(sorted_pts.front().second, 100);
        } else if (t >= sorted_pts.back().first) {
            _curve[idx] = std::min<uint8_t>(sorted_pts.back().second, 100);
        } else {
            // İki nokta arası doğrusal enterpolasyon (Linear Interpolation)
            for (size_t i = 0; i < sorted_pts.size() - 1; ++i) {
                if (t >= sorted_pts[i].first && t <= sorted_pts[i + 1].first) {
                    float t0 = sorted_pts[i].first;
                    float t1 = sorted_pts[i + 1].first;
                    float p0 = sorted_pts[i].second;
                    float p1 = sorted_pts[i + 1].second;
                    
                    float factor = (static_cast<float>(t) - t0) / (t1 - t0);
                    _curve[idx] = static_cast<uint8_t>(std::clamp(p0 + factor * (p1 - p0), 0.0f, 100.0f));
                    break;
                }
            }
        }
    }
}

uint8_t ClevoFan::evaluate_target_duty() {
    uint8_t current_temp = _telemetry.temp;

    // Histerezis Kontrolü: Sıcaklık düşerken dalgalanmayı engelle
    if (_last_evaluated_temp != 0) {
        if (current_temp < _last_evaluated_temp && (_last_evaluated_temp - current_temp) < _hysteresis) {
            // Belirlenen histerezis eşiği kadar soğumadıysa mevcut PWM'i koru
            return _current_target_duty;
        }
    }

    _last_evaluated_temp = current_temp;

    // Sınırları sabitle (Clamp)
    uint8_t clamped_temp = std::clamp<uint8_t>(current_temp, TEMP_MIN, TEMP_MAX);
    size_t curve_idx = clamped_temp - TEMP_MIN;

    uint8_t target_pct = _curve[curve_idx];
    _current_target_duty = static_cast<uint8_t>((target_pct / 100.0) * 255);

    return _current_target_duty;
}