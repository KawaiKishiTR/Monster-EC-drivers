#include "clevo_hw_defs.h"
#include "clevo_controller.h"
#include "clevo_fan.hpp"
#include <thread>
#include <chrono>

int main() {
    clevo_init();

    // CPU ve GPU Fan kontrolcüleri
    ClevoFan cpu_fan(R_CL_FANINFO1, /*hysteresis=*/2);
    ClevoFan gpu_fan(R_CL_FANINFO2, /*hysteresis=*/3);

    // Özel Curve Tanımlama (Örn: GPU daha agresif soğusun)
    gpu_fan.set_linear_curve({
        {40, 25},
        {60, 60},
        {75, 90},
        {80, 100}
    });

    // Kontrol Döngüsü (Control Loop - Arka plan servisi / Daemon)
     while (true) {
         cpu_fan.update_telemetry(clevo_get_fd());
         gpu_fan.update_telemetry(clevo_get_fd());
    
         uint8_t cpu_duty = cpu_fan.evaluate_target_duty();
         uint8_t gpu_duty = gpu_fan.evaluate_target_duty();
    
         int32_t payload = (cpu_duty & 0xFF) | ((gpu_duty & 0xFF) << 8);
         ioctl(clevo_get_fd(), W_CL_FANSPEED, &payload);
    
         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
     }

    clevo_close();
    return 0;
}