/*
 * All Rights Reserved
 *
 * Copyright (c) 2024 Grzegorz Grzęda
 *
 * THE CONTENTS OF THIS PROJECT ARE PROPRIETARY AND CONFIDENTIAL.
 * UNAUTHORIZED COPYING, TRANSFERRING OR REPRODUCTION OF THE CONTENTS OF THIS
 * PROJECT, VIA ANY MEDIUM IS STRICTLY PROHIBITED.
 *
 * The receipt or possession of the source code and/or any parts thereof does
 * not convey or imply any right to use them for any purpose other than the
 * purpose for which they were provided to you.
 *
 * The software is provided "AS IS", without warranty of any kind, express or
 * implied, including but not limited to the warranties of merchantability,
 * fitness for a particular purpose and non infringement. In no event shall the
 * authors or copyright holders be liable for any claim, damages or other
 * liability, whether in an action of contract, tort or otherwise, arising from,
 * out of or in connection with the software or the use or other dealings in the
 * software.
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the software.
 */
#include "event-handler.h"
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmocka.h"

static void test_create_event_handler(void** state) {
    (void)state;  // unused
    event_handler_t* handler = event_handler_create();
    assert_non_null(handler);
    event_handler_destroy(handler);
}

static void my_handler_function_1(event_handler_t* handler, uint16_t id, void* context, void* payload, size_t size) {
    function_called();
    check_expected(handler);
    check_expected(id);
    check_expected(context);
    check_expected(payload);
    check_expected(size);
}

static void my_handler_function_2(event_handler_t* handler, uint16_t id, void* context, void* payload, size_t size) {
    function_called();
    check_expected(handler);
    check_expected(id);
    check_expected(context);
    check_expected(payload);
    check_expected(size);
}

static void test_register_one_handler(void** state) {
    (void)state;  // unused
    event_handler_t* handler = event_handler_create();
    assert_non_null(handler);

    char random_context[20];
    bool result = event_handler_register(handler, 1, random_context, my_handler_function_1);
    assert_int_equal(result, true);

    char random_data[10];

    expect_function_call(my_handler_function_1);
    expect_value(my_handler_function_1, handler, handler);
    expect_value(my_handler_function_1, id, 1);
    expect_value(my_handler_function_1, context, random_context);
    expect_value(my_handler_function_1, payload, random_data);
    expect_value(my_handler_function_1, size, sizeof(random_data));
    event_handler_send(handler, 1, random_data, sizeof(random_data));

    event_handler_destroy(handler);
}

static void test_register_one_handler_multiple_contexts(void** state) {
    (void)state;  // unused
    event_handler_t* handler = event_handler_create();
    assert_non_null(handler);

    char random_context[20];
    bool result = event_handler_register(handler, 1, random_context, my_handler_function_1);
    assert_int_equal(result, true);

    char random_context2[20];
    bool result2 = event_handler_register(handler, 2, random_context2, my_handler_function_1);
    assert_int_equal(result2, true);

    char random_data[10];

    expect_function_call(my_handler_function_1);
    expect_value(my_handler_function_1, handler, handler);
    expect_value(my_handler_function_1, id, 1);
    expect_value(my_handler_function_1, context, random_context);
    expect_value(my_handler_function_1, payload, random_data);
    expect_value(my_handler_function_1, size, sizeof(random_data));
    event_handler_send(handler, 1, random_data, sizeof(random_data));

    expect_function_call(my_handler_function_1);
    expect_value(my_handler_function_1, handler, handler);
    expect_value(my_handler_function_1, id, 2);
    expect_value(my_handler_function_1, context, random_context2);
    expect_value(my_handler_function_1, payload, random_data);
    expect_value(my_handler_function_1, size, sizeof(random_data));
    event_handler_send(handler, 2, random_data, sizeof(random_data));

    event_handler_destroy(handler);
}

static void test_register_multiple_handlers(void** state) {
    (void)state;  // unused
    event_handler_t* handler = event_handler_create();
    assert_non_null(handler);

    char random_context[20];
    bool result = event_handler_register(handler, 1, random_context, my_handler_function_1);
    assert_int_equal(result, true);

    char random_context2[20];
    bool result2 = event_handler_register(handler, 2, random_context2, my_handler_function_2);
    assert_int_equal(result2, true);

    char random_data_1[10];
    char random_data_2[10];

    expect_function_call(my_handler_function_1);
    expect_value(my_handler_function_1, handler, handler);
    expect_value(my_handler_function_1, id, 1);
    expect_value(my_handler_function_1, context, random_context);
    expect_value(my_handler_function_1, payload, random_data_1);
    expect_value(my_handler_function_1, size, sizeof(random_data_1));
    event_handler_send(handler, 1, random_data_1, sizeof(random_data_1));

    expect_function_call(my_handler_function_2);
    expect_value(my_handler_function_2, handler, handler);
    expect_value(my_handler_function_2, id, 2);
    expect_value(my_handler_function_2, context, random_context2);
    expect_value(my_handler_function_2, payload, random_data_2);
    expect_value(my_handler_function_2, size, sizeof(random_data_2));
    event_handler_send(handler, 2, random_data_2, sizeof(random_data_2));

    event_handler_destroy(handler);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_event_handler),
        cmocka_unit_test(test_register_one_handler),
        cmocka_unit_test(test_register_one_handler_multiple_contexts),
        cmocka_unit_test(test_register_multiple_handlers),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
