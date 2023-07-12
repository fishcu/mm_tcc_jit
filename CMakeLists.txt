cmake_minimum_required(VERSION 3.12)
set(PROJECT_NAME "mm_tcc_jit")
project(${PROJECT_NAME})

option(BUILD_FOR_MM "Enable build for Miyoo Mini" OFF)

if (BUILD_FOR_MM)
    message(STATUS "Building for MM, cross compile var is $ENV{CROSS_COMPILE}") 
    set(CMAKE_C_COMPILER "$ENV{CROSS_COMPILE}gcc")
    set(CMAKE_CXX_COMPILER "$ENV{CROSS_COMPILE}g++")
endif()

set(TINYCC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tinycc)
set(TINYCC_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tinycc)
add_library(tinycc INTERFACE)
target_link_libraries(tinycc INTERFACE ${TINYCC_LIB_DIR}/libtcc.a ${TINYCC_LIB_DIR}/libtcc1.a)
target_include_directories(tinycc INTERFACE ${TINYCC_SOURCE_DIR})
target_link_libraries(tinycc INTERFACE pthread dl)

add_executable(${PROJECT_NAME}
    "main.c"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    # CXX_STANDARD 17
    # CXX_STANDARD_REQUIRED YES
    POSITION_INDEPENDENT_CODE ON
)
target_link_libraries(${PROJECT_NAME} PRIVATE tinycc)
if (BUILD_FOR_MM)
    target_compile_options(${PROJECT_NAME}
        PRIVATE
        "-Wall"
        "-pedantic"
        "-O2"
        "-marm" "-march=armv7ve+simd" "-mtune=cortex-a7" "-mfpu=neon-vfpv4" "-mfloat-abi=hard"
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        BUILD_FOR_MM
    )
else()
    target_compile_options(${PROJECT_NAME}
        PRIVATE
        "-Wall"
        "-pedantic"
        "-O2"
        "-march=native"
    )
endif()
