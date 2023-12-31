#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtcc.h"

void handle_tcc_error(void *opaque, const char *msg) {
    fprintf((FILE *)opaque, "%s\n", msg);
}

int main(int argc, char *argv[]) {
    TCCState *tcc = tcc_new();
    if (!tcc) {
        printf("Failed to create TCC instance!\n");
        return 1;
    }

    tcc_set_error_func(tcc, stderr, handle_tcc_error);
    assert(tcc_get_error_func(tcc) == handle_tcc_error);
    assert(tcc_get_error_opaque(tcc) == stderr);

    // TCC is not installed in the system.
    for (int i = 1; i < argc; ++i) {
        char *a = argv[i];
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

    const char *source =
        "#include <math.h>\n"
        "float jit_sqrt(float x) {\n"
        "    return sqrt(x);\n"
        "}";

    if (tcc_compile_string(tcc, source) != 0) {
        printf("Failed to compile!\n");
        tcc_delete(tcc);
        return 1;
    }

    if (tcc_add_library(tcc, "m") != 0) {
        printf("Failed to add library!\n");
        tcc_delete(tcc);
        return 1;
    }

    if (tcc_relocate(tcc, TCC_RELOCATE_AUTO) != 0) {
        printf("Failed to relocate!\n");
        tcc_delete(tcc);
        return 1;
    }

    float (*func)(float);
    func = tcc_get_symbol(tcc, "jit_sqrt");
    if (!func) {
        printf("Failed to retrieve function pointer!\n");
        tcc_delete(tcc);
        return 1;
    }

    const float x = 2.0f;
    const float y = func(x);
    printf("sqrt of %f is %f\n", x, y);

    tcc_delete(tcc);

    return 0;
}
