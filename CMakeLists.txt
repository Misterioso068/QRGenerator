cmake_minimum_required(VERSION 3.16)
project(qrgen C)

set(CMAKE_C_STANDARD 11)

# Find SDL3
find_package(SDL3 REQUIRED)

# Source files
set(SOURCES
    src/main.c
    src/logger.c
    src/input_handler.c
    src/galois_field.c
    src/poly_math.c
    src/rs_encoding.c
    src/qr_generator.c
    src/gui.c
)

add_executable(qrgen ${SOURCES})

target_include_directories(qrgen PRIVATE include)

target_link_libraries(qrgen PRIVATE SDL3::SDL3)

# Automatically copy runtime DLLs beside the executable on Windows
if (WIN32)
    add_custom_command(TARGET qrgen POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:qrgen>
            $<TARGET_FILE_DIR:qrgen>
        COMMAND_EXPAND_LISTS
    )
endif()