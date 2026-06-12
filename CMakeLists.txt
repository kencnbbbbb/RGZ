cmake_minimum_required(VERSION 3.15)
project(RasGrafRab VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найти OpenSSL
find_package(OpenSSL REQUIRED)

# Подключаем папку include
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/plugins)

# --- Плагины ---
# Rabin
add_library(rabin SHARED
    plugins/rabin/Rabin.cpp
    plugins/rabin/rabin_plugin.cpp
    plugins/mess/mess.cpp  # Rabin использует mess.h для isPrime, modPow и т.д.
)
target_link_libraries(rabin PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Mess-Omura
add_library(mess SHARED
    plugins/mess/mess.cpp
    plugins/mess/mess_plugin.cpp
)
target_link_libraries(mess PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# A1Ya33
add_library(a1ya33 SHARED
    plugins/a1ya33/A1Ya33.cpp
    plugins/a1ya33/a1ya33_plugin.cpp
)

# Shamir
add_library(shamir SHARED
    plugins/shamir/Shamir.cpp
    plugins/shamir/shamir_plugin.cpp
)

# RSA
add_library(rsa SHARED
    plugins/rsa/rsa_crypto.cpp
    plugins/rsa/rsa_plugin.cpp
    plugins/utils.cpp
)
target_link_libraries(rsa PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# ChaCha20
add_library(chacha20 SHARED
    plugins/chacha20/chacha20_crypto.cpp
    plugins/chacha20/chacha20_plugin.cpp
    plugins/utils.cpp
)
target_link_libraries(chacha20 PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# --- Исполняемый файл ---
add_executable(RasGrafRab
    app/main.cpp
    app/PluginLoader.cpp
    app/ConsoleUI.cpp
)
target_link_libraries(RasGrafRab PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Для Linux: libdl для dlopen
if(UNIX AND NOT APPLE)
    target_link_libraries(RasGrafRab PRIVATE dl)
endif()

# Копировать плагины в папку plugins/ рядом с исполняемым файлом после сборки
add_custom_command(TARGET RasGrafRab POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:RasGrafRab>/plugins"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:rabin>" "$<TARGET_FILE_DIR:RasGrafRab>/plugins/"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:mess>" "$<TARGET_FILE_DIR:RasGrafRab>/plugins/"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:a1ya33>" "$<TARGET_FILE_DIR:RasGrafRab>/plugins/"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:shamir>" "$<TARGET_FILE_DIR:RasGrafRab>/plugins/"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:rsa>" "$<TARGET_FILE_DIR:RasGrafRab>/plugins/"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:chacha20>" "$<TARGET_FILE_DIR:RasGrafRab>/plugins/"
)

# Для Windows: копировать DLL OpenSSL в папку с exe (чтобы не добавлять в PATH)
if(WIN32)
    add_custom_command(TARGET RasGrafRab POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:OpenSSL::SSL>"
        "$<TARGET_FILE:OpenSSL::Crypto>"
        "$<TARGET_FILE_DIR:RasGrafRab>"
    )
endif()
