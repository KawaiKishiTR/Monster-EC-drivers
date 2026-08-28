#!/usr/bin/env python3
import argparse
import array
import fcntl
import os
import sys

# Linux x86_64 IOCTL Hesaplama
_IOC_WRITE = 1
_IOC_READ = 2


def _IOC(dir_, type_, nr, size):
    return (
        (dir_ << 30)
        | (ord(type_) if isinstance(type_, str) else type_) << 8
        | (nr << 0)
        | (size << 16)
    )


def _IOR(type_, nr, size):
    return _IOC(_IOC_READ, type_, nr, size)


def _IOW(type_, nr, size):
    return _IOC(_IOC_WRITE, type_, nr, size)


IOCTL_MAGIC = 0xEC
MAGIC_READ_CL = IOCTL_MAGIC + 1  # 0xED
MAGIC_WRITE_CL = IOCTL_MAGIC + 2  # 0xEE
PTR_SIZE = 8

R_CL_FANINFO1 = _IOR(MAGIC_READ_CL, 0x10, PTR_SIZE)  # CPU
R_CL_FANINFO2 = _IOR(MAGIC_READ_CL, 0x11, PTR_SIZE)  # GPU
W_CL_FANSPEED = _IOW(MAGIC_WRITE_CL, 0x10, PTR_SIZE)
W_CL_FANAUTO = _IOW(MAGIC_WRITE_CL, 0x11, PTR_SIZE)

DEV_PATH = "/dev/tuxedo_io"


def parse_fan_data(raw: int):
    duty_raw = raw & 0xFF
    temp = (raw >> 8) & 0xFF
    tach = (raw >> 16) & 0xFFFF

    rpm = int(2156220 / tach) if (tach > 0 and tach != 0xFFFF) else 0
    duty_pct = int((duty_raw / 255.0) * 100) if duty_raw <= 255 else 0
    return temp, duty_pct, duty_raw, rpm, tach


def get_fan_info(fd, cmd):
    buf = array.array("i", [0])
    fcntl.ioctl(fd, cmd, buf, True)
    raw = buf[0] & 0xFFFFFFFF
    return parse_fan_data(raw)


def main():
    parser = argparse.ArgumentParser(
        description="Clevo/Monster Fan Kontrol Aracı"
    )
    parser.add_argument(
        "--status", action="store_true", help="Telemetri durumunu göster"
    )
    parser.add_argument(
        "--cpu",
        type=int,
        metavar="0-100",
        help="CPU Fan hızını yüzde olarak ayarla",
    )
    parser.add_argument(
        "--gpu",
        type=int,
        metavar="0-100",
        help="GPU Fan hızını yüzde olarak ayarla",
    )
    parser.add_argument(
        "--all",
        type=int,
        metavar="0-100",
        help="Tüm fanları aynı yüzdeye ayarla",
    )
    parser.add_argument(
        "--auto", action="store_true", help="Fanları BIOS otomatik moduna devret"
    )

    args = parser.parse_args()

    if not os.path.exists(DEV_PATH):
        print(f"Hata: {DEV_PATH} bulunamadı.", file=sys.stderr)
        sys.exit(1)

    try:
        fd = os.open(DEV_PATH, os.O_RDWR)
    except PermissionError:
        print("Hata: Root yetkisi gerekli (sudo).", file=sys.stderr)
        sys.exit(1)

    try:
        if args.auto:
            for fan_id in [0, 1, 0xFF]:
                try:
                    fcntl.ioctl(fd, W_CL_FANAUTO, array.array("i", [fan_id]))
                except Exception:
                    pass
            print("[+] Tüm fanlar otomatik (BIOS) moduna alındı.")

        elif (
            args.all is not None or args.cpu is not None or args.gpu is not None
        ):
            t1, d1, raw_d1, _, _ = get_fan_info(fd, R_CL_FANINFO1)
            t2, d2, raw_d2, _, _ = get_fan_info(fd, R_CL_FANINFO2)

            f1_val, f2_val, f3_val = raw_d1, raw_d2, 0

            if args.all is not None:
                val = int((max(0, min(100, args.all)) / 100.0) * 255)
                f1_val = f2_val = f3_val = val
            else:
                if args.cpu is not None:
                    f1_val = int((max(0, min(100, args.cpu)) / 100.0) * 255)
                if args.gpu is not None:
                    f2_val = int((max(0, min(100, args.gpu)) / 100.0) * 255)

            payload = (
                (f1_val & 0xFF)
                | ((f2_val & 0xFF) << 8)
                | ((f3_val & 0xFF) << 16)
            )
            fcntl.ioctl(fd, W_CL_FANSPEED, array.array("i", [payload]))
            print(f"[+] Fan hızları uygulandı -> CPU PWM: {f1_val} | GPU PWM: {f2_val}")

        else:
            print("--- Clevo Donanım Telemetrisi ---")
            for name, cmd in [
                ("Fan 1 (CPU)", R_CL_FANINFO1),
                ("Fan 2 (GPU)", R_CL_FANINFO2),
            ]:
                try:
                    temp, duty_pct, duty_raw, rpm, tach = get_fan_info(fd, cmd)
                    print(
                        f"[{name}] Sıcaklık: {temp}°C | PWM: %{duty_pct} ({duty_raw}/255) | Devir: ~{rpm} RPM"
                    )
                except Exception as e:
                    print(f"[{name}] Hata: {e}")

    finally:
        os.close(fd)


if __name__ == "__main__":
    main()