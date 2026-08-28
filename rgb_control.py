#!/usr/bin/env python3
import argparse
from pathlib import Path
import sys

LED_PATH = Path("/sys/class/leds/rgb:kbd_backlight")


def set_backlight(r: int, g: int, b: int, brightness: int = 255):
    if not LED_PATH.exists():
        print(f"Hata: {LED_PATH} bulunamadı. Sürücü yüklü mü?", file=sys.stderr)
        sys.exit(1)

    try:
        (LED_PATH / "brightness").write_text(f"{brightness}\n")
        (LED_PATH / "multi_intensity").write_text(f"{r} {g} {b}\n")
        print(f"[+] RGB ayarlandı: ({r}, {g}, {b}) | Parlaklık: {brightness}")
    except PermissionError:
        print("Hata: Root yetkisi gerekli (sudo).", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Clevo Klavye RGB Yöneticisi")
    parser.add_argument("r", type=int, help="Kırmızı (0-255)")
    parser.add_argument("g", type=int, help="Yeşil (0-255)")
    parser.add_argument("b", type=int, help="Mavi (0-255)")
    parser.add_argument(
        "-br", "--brightness", type=int, default=255, help="Parlaklık (0-255)"
    )

    args = parser.parse_args()
    set_backlight(args.r, args.g, args.b, args.brightness)