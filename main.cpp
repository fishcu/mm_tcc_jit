#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

extern "C" {
#ifdef BUILD_FOR_MM
#define TCC_CPU_VERSION = 7
#define TCC_TARGET_ARM
#define TCC_ARM_EABI
#define TCC_ARM_HARDFLOAT
#define TCC_ARM_VFP
#endif  // BUILD_FOR_MM
#include "libtcc.h"
}

void handle_tcc_error(void* opaque, const char* msg) {
    fprintf(reinterpret_cast<FILE*>(opaque), "%s\n", msg);
}

int main(int argc, char* argv[]) {
    TCCState* tcc = tcc_new();
    if (tcc == nullptr) {
        std::cerr << "Failed to create TCC instance" << std::endl;
        return 1;
    }

    tcc_set_error_func(tcc, stderr, handle_tcc_error);
    assert(tcc_get_error_func(tcc) == handle_tcc_error);
    assert(tcc_get_error_opaque(tcc) == stderr);

    // TCC is not installed in the system.
    for (int i = 1; i < argc; ++i) {
        char* a = argv[i];
        if (a[0] == '-') {
            if (a[1] == 'B')
                tcc_set_lib_path(tcc, a + 2);
            else if (a[1] == 'I')
                tcc_add_include_path(tcc, a + 2);
            else if (a[1] == 'L')
                tcc_add_library_path(tcc, a + 2);
        }
    }

    tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);

    const char* source =
#ifdef BUILD_FOR_MM
        "#define TCC_CPU_VERSION = 7"
        "#define TCC_TARGET_ARM"
        "#define TCC_ARM_EABI"
        "#define TCC_ARM_HARDFLOAT"
        "#define TCC_ARM_VFP"
#endif  // BUILD_FOR_MM
        "#include <math.h>"
        "float jit_sqrt(float x) {"
        "    return sqrt(x);"
        "}";

    if (tcc_compile_string(tcc, source) == -1) {
        tcc_delete(tcc);
        return 1;
    }

    if (tcc_relocate(tcc, TCC_RELOCATE_AUTO) == -1) {
        tcc_delete(tcc);
        return 1;
    }

    auto jit_sqrt = reinterpret_cast<float (*)(float)>(
        tcc_get_symbol(tcc, "jit_sqrt"));
    if (!jit_sqrt) {
        std::cerr << "Failed to retrieve function pointer" << std::endl;
        tcc_delete(tcc);
        return 1;
    }

    constexpr float x = 2.0f;
    printf("sqrt of %f is %f\n", x, jit_sqrt(x));

    tcc_delete(tcc);

    return 0;
}