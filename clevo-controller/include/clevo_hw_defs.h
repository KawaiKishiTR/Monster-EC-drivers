#pragma once

#include <cstdint>
#include <sys/ioctl.h>

// Sistemdeki mevcut makroları sıfırlayıp kendi tanımlarımızı koruyoruz
#undef _IOC_WRITE
#undef _IOC_READ
#undef _IOC
#undef _IOR
#undef _IOW

// Linux x86_64 IOCTL Makroları
#define _IOC_WRITE 1U
#define _IOC_READ  2U
#define _IOC(dir, type, nr, size) \
    (((dir) << 30) | ((size) << 16) | (((type) & 0xFF) << 8) | ((nr) & 0xFF))

#define _IOR(type, nr, size) _IOC(_IOC_READ,  (type), (nr), (size))
#define _IOW(type, nr, size) _IOC(_IOC_WRITE, (type), (nr), (size))

constexpr uint32_t IOCTL_MAGIC   = 0xEC;
constexpr uint32_t MAGIC_READ_CL  = IOCTL_MAGIC + 1; // 0xED
constexpr uint32_t MAGIC_WRITE_CL = IOCTL_MAGIC + 2; // 0xEE
constexpr uint32_t PTR_SIZE       = 8; // sizeof(int32_t*) on 64-bit

constexpr unsigned long R_CL_FANINFO1 = _IOR(MAGIC_READ_CL, 0x10, PTR_SIZE);
constexpr unsigned long R_CL_FANINFO2 = _IOR(MAGIC_READ_CL, 0x11, PTR_SIZE);
constexpr unsigned long W_CL_FANSPEED = _IOW(MAGIC_WRITE_CL, 0x10, PTR_SIZE);
constexpr unsigned long W_CL_FANAUTO  = _IOW(MAGIC_WRITE_CL, 0x11, PTR_SIZE);

constexpr const char* DEV_PATH = "/dev/tuxedo_io";
constexpr const char* LED_BRIGHTNESS_PATH = "/sys/class/leds/rgb:kbd_backlight/brightness";
constexpr const char* LED_COLOR_PATH      = "/sys/class/leds/rgb:kbd_backlight/multi_intensity";