sudo modprobe -a sparse-keymap led_class_multicolor

sudo insmod tuxedo-drivers/src/tuxedo_compatibility_check/tuxedo_compatibility_check.ko
sudo insmod tuxedo-drivers/src/tuxedo_keyboard.ko
sudo insmod tuxedo-drivers/src/clevo_acpi.ko
sudo insmod tuxedo-drivers/src/clevo_wmi.ko
sudo insmod tuxedo-drivers/src/tuxedo_io/tuxedo_io.ko