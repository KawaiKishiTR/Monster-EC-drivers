#!/usr/bin/env bash
set -e

echo "[*] Derleme yapılıyor..."
cd tuxedo-drivers
make clean
make -j"$(nproc)"

echo "[*] Modüller sisteme kuruluyor..."
KVER=$(uname -r)
DEST_DIR="/lib/modules/${KVER}/extra"

sudo mkdir -p "${DEST_DIR}"

# Üretilen .ko dosyalarını doğrudan modül dizinine aktarma
sudo find . -name "*.ko" -exec cp {} "${DEST_DIR}/" \;

# Modül bağımlılık eşleşmesini (depmod) yenileme
sudo depmod -a

echo "[*] Modüller belleğe alınıyor..."
sudo modprobe -a sparse-keymap led_class_multicolor
sudo modprobe tuxedo_compatibility_check || true
sudo modprobe tuxedo_keyboard || true
sudo modprobe clevo_acpi || true
sudo modprobe clevo_wmi || true
sudo modprobe tuxedo_io || true
cd ..
echo "[+] Modüller başarıyla yüklendi."

echo "[*] clevo_controller.so kütüphanesi yeniden derlenip kuruluyor"
cd clevo-controller
exec ./install_clevo_controller.sh
cd ..
echo "[+] clevo_controller.so kurulumu başarılı..."