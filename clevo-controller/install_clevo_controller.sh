# 1. Derleme klasörünü oluştur ve yapılandır
cmake -B ./build -DCMAKE_BUILD_TYPE=Release

echo "[*] clevo_controller.so delremesi yapılıyor"

# 2. Kütüphaneyi derle
cmake --build ./build

echo "[*] clevo_controller.so sisteme kuruluyor (/usr/local altına)"

# 3. Sisteme kur (/usr/local altına)
sudo cmake --install ./build

echo "[*] dinamik kütüphane önbelleği temizleniyor"

# 4. Dinamik kütüphane önbelleğini tazele
sudo ldconfig

echo "[+] clevo_controller.so kurulumu tamamlandı"