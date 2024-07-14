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
#include <stdlib.h>

typedef struct linked_list_iterator {
    void* data;
    struct linked_list_iterator* next;
    struct linked_list_iterator* prev;
} linked_list_iterator_t;

typedef struct linked_list {
    linked_list_iterator_t* head;
    linked_list_iterator_t* tail;
} linked_list_t;

linked_list_t* linked_list_create(void) {
    linked_list_t* list = calloc(1, sizeof(linked_list_t));
    if (list == NULL) {
        return NULL;
    }
    return list;
}

void linked_list_destroy(linked_list_t* list) {
    linked_list_iterator_t* iterator = linked_list_iterator_begin(list);
    while (iterator != NULL) {
        linked_list_iterator_t* next = linked_list_iterator_next(iterator);
        free(iterator);
        iterator = next;
    }
    free(list);
}

void linked_list_append(linked_list_t* list, void* data) {
    if (list == NULL) {
        return;
    }
    linked_list_iterator_t* iterator = calloc(1, sizeof(linked_list_iterator_t));
    if (iterator == NULL) {
        return;
    }
    iterator->data = data;
    iterator->next = NULL;
    iterator->prev = linked_list_iterator_end(list);
    if (list->tail != NULL) {
        list->tail->next = iterator;
    }
    list->tail = iterator;
    if (list->head == NULL) {
        list->head = iterator;
    }
}

linked_list_iterator_t* linked_list_iterator_begin(linked_list_t* list) {
    if (list == NULL) {
        return NULL;
    }
    return list->head;
}

linked_list_iterator_t* linked_list_iterator_end(linked_list_t* list) {
    if (list == NULL) {
        return NULL;
    }
    return list->tail;
}

linked_list_iterator_t* linked_list_iterator_next(linked_list_iterator_t* iterator) {
    if (iterator == NULL) {
        return NULL;
    }
    return iterator->next;
}

linked_list_iterator_t* linked_list_iterator_prev(linked_list_iterator_t* iterator) {
    if (iterator == NULL) {
        return NULL;
    }
    return iterator->prev;
}

void* linked_list_get(linked_list_iterator_t* iterator) {
    if (iterator == NULL) {
        return NULL;
    }
    return iterator->data;
}
