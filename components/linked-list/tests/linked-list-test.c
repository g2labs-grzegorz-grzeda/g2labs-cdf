/*
 * MIT License
 *
 * Copyright (c) 2024 G2Labs Grzegorz Grzeda
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "linked-list.h"
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmocka.h"

static void test_create_linked_list(void** state) {
    (void)state;  // unused
    linked_list_t* list = linked_list_create();
    assert_ptr_not_equal(list, NULL);
    linked_list_destroy(list);
}

static void test_list_append(void** state) {
    (void)state;  // unused
    linked_list_t* list = linked_list_create();
    int element = 5;
    linked_list_append(list, &element);
    linked_list_iterator_t* it = linked_list_iterator_begin(list);
    assert_ptr_not_equal(it, NULL);

    int* data = linked_list_get(it);
    assert_int_equal(*data, element);
    it = linked_list_iterator_next(it);
    assert_ptr_equal(it, NULL);

    linked_list_destroy(list);
}

static void test_list_iterate(void** state) {
    (void)state;  // unused
    linked_list_t* list = linked_list_create();
    int elements[] = {1, 2, 3, 4, 5};
    for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
        linked_list_append(list, &elements[i]);
    }

    linked_list_iterator_t* it = linked_list_iterator_begin(list);
    assert_ptr_not_equal(it, NULL);

    for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
        int* data = linked_list_get(it);
        assert_int_equal(*data, elements[i]);
        it = linked_list_iterator_next(it);
        if (i == sizeof(elements) / sizeof(elements[0]) - 1) {
            assert_ptr_equal(it, NULL);
        } else {
            assert_ptr_not_equal(it, NULL);
        }
    }

    linked_list_destroy(list);
}

static void test_list_iterate_reverse(void** state) {
    (void)state;  // unused
    linked_list_t* list = linked_list_create();
    int elements[] = {1, 2, 3, 4, 5};
    for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
        linked_list_append(list, &elements[i]);
    }

    linked_list_iterator_t* it = linked_list_iterator_end(list);
    assert_ptr_not_equal(it, NULL);

    for (size_t i = sizeof(elements) / sizeof(elements[0]); i > 0; i--) {
        assert_ptr_not_equal(it, NULL);
        int* data = linked_list_get(it);
        assert_int_equal(*data, elements[i - 1]);
        it = linked_list_iterator_prev(it);
    }

    it = linked_list_iterator_prev(it);
    assert_ptr_equal(it, NULL);

    linked_list_destroy(list);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_linked_list),
        cmocka_unit_test(test_list_append),
        cmocka_unit_test(test_list_iterate),
        cmocka_unit_test(test_list_iterate_reverse),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
    return 0;
}
