cmake_minimum_required(VERSION 3.16)
project(clevo_controller VERSION 1.0.0 LANGUAGES C)

# C99 standardını zorunlu kıl
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Paylaşımlı kütüphane (.so) için Position Independent Code (PIC)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Paylaşımlı kütüphane hedefi
add_library(clevo_controller SHARED
    src/clevo_controller.c
)

# Header dizinleri
target_include_directories(clevo_controller PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# GNUInstallDirs: lib, include gibi standart Linux dizin yollarını sağlar
include(GNUInstallDirs)

# 1. Kütüphaneyi kur (/usr/local/lib/libclevo_controller.so)
install(TARGETS clevo_controller
    EXPORT clevo_controller-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 2. Header dosyalarını kur (/usr/local/include/clevo_controller/*.h)
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/clevo_controller
    FILES_MATCHING PATTERN "*.h"
)

# 3. Diğer CMake projelerinin find_package(clevo_controller) yapabilmesi için config dışa aktarımı
install(EXPORT clevo_controller-targets
    FILE clevo_controller-config.cmake
    NAMESPACE clevo::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/clevo_controller
)