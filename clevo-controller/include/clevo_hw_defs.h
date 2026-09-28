#ifndef CLEVO_HW_DEFS_H
#define CLEVO_HW_DEFS_H

#include <stdint.h>
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

#define IOCTL_MAGIC   0xEC
#define MAGIC_READ_CL  (IOCTL_MAGIC + 1) // 0xED
#define MAGIC_WRITE_CL (IOCTL_MAGIC + 2) // 0xEE
#define PTR_SIZE       8 // 64-bit mimaride sizeof(int32_t*)

#define R_CL_FANINFO1 _IOR(MAGIC_READ_CL, 0x10, PTR_SIZE)
#define R_CL_FANINFO2 _IOR(MAGIC_READ_CL, 0x11, PTR_SIZE)
#define W_CL_FANSPEED _IOW(MAGIC_WRITE_CL, 0x10, PTR_SIZE)
#define W_CL_FANAUTO  _IOW(MAGIC_WRITE_CL, 0x11, PTR_SIZE)

#define DEV_PATH            "/dev/tuxedo_io"
#define LED_BRIGHTNESS_PATH "/sys/class/leds/rgb:kbd_backlight/brightness"
#define LED_COLOR_PATH      "/sys/class/leds/rgb:kbd_backlight/multi_intensity"

#endif // CLEVO_HW_DEFS_H