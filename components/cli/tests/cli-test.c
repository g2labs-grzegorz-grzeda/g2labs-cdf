#include "cli.h"
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "cmocka.h"

static void null_test(void** state) {
    (void)state;  // unused
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}