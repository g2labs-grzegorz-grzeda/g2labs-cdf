/**
 * MIT License
 * Copyright (c) 2024 Grzegorz Grzęda
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "g2l-callback.h"
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "cmocka.h"

static void test_callback_handler(void* context, void* payload) {
    function_called();
    check_expected_ptr(context);
    check_expected(payload);
}

static void test_another_callback_handler(void* context, void* payload) {
    function_called();
    check_expected_ptr(context);
    check_expected(payload);
}

static void test_create_callbacks(void** state) {
    (void)state;  // unused
    g2l_callback_t* cbs = g2l_callback_create();
    assert_ptr_not_equal(cbs, NULL);
}

static void test_register_invalid_callback(void** state) {
    (void)state;  // unused
    assert_false(g2l_callback_register_handler(NULL, NULL, NULL));
    g2l_callback_t* cbs = g2l_callback_create();
    assert_false(g2l_callback_register_handler(cbs, NULL, NULL));
    g2l_callback_dispatch(NULL, NULL);
}

static void test_register_callbacks(void** state) {
    (void)state;  // unused
    g2l_callback_t* cbs = g2l_callback_create();

    uint8_t some_context;

    assert_true(g2l_callback_register_handler(cbs, test_callback_handler, NULL));
    assert_true(g2l_callback_register_handler(cbs, test_callback_handler, &some_context));
    assert_true(g2l_callback_register_handler(cbs, test_another_callback_handler, &some_context));

    expect_function_call(test_callback_handler);
    expect_value(test_callback_handler, context, NULL);
    expect_value(test_callback_handler, payload, NULL);
    expect_function_call(test_callback_handler);
    expect_value(test_callback_handler, context, &some_context);
    expect_value(test_callback_handler, payload, NULL);
    expect_function_call(test_another_callback_handler);
    expect_value(test_another_callback_handler, context, &some_context);
    expect_value(test_another_callback_handler, payload, NULL);
    g2l_callback_dispatch(cbs, NULL);

    uint8_t some_payload;
    expect_function_call(test_callback_handler);
    expect_value(test_callback_handler, context, NULL);
    expect_value(test_callback_handler, payload, &some_payload);
    expect_function_call(test_callback_handler);
    expect_value(test_callback_handler, context, &some_context);
    expect_value(test_callback_handler, payload, &some_payload);
    expect_function_call(test_another_callback_handler);
    expect_value(test_another_callback_handler, context, &some_context);
    expect_value(test_another_callback_handler, payload, &some_payload);
    g2l_callback_dispatch(cbs, &some_payload);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_callbacks),
        cmocka_unit_test(test_register_invalid_callback),
        cmocka_unit_test(test_register_callbacks),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
