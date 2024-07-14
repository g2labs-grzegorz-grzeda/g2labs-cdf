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
#include "g2l-linked-list.h"
#include <stdlib.h>

typedef struct g2l_linked_list_iterator {
    void* data;
    struct g2l_linked_list_iterator* next;
    struct g2l_linked_list_iterator* prev;
} g2l_linked_list_iterator_t;

typedef struct g2l_linked_list {
    g2l_linked_list_iterator_t* head;
    g2l_linked_list_iterator_t* tail;
} g2l_linked_list_t;

g2l_linked_list_t* g2l_linked_list_create(void) {
    g2l_linked_list_t* list = calloc(1, sizeof(g2l_linked_list_t));
    if (list == NULL) {
        return NULL;
    }
    return list;
}

void g2l_linked_list_destroy(g2l_linked_list_t* list) {
    g2l_linked_list_iterator_t* iterator = g2l_linked_list_iterator_begin(list);
    while (iterator != NULL) {
        g2l_linked_list_iterator_t* next = g2l_linked_list_iterator_next(iterator);
        free(iterator);
        iterator = next;
    }
    free(list);
}

void g2l_linked_list_append(g2l_linked_list_t* list, void* data) {
    if (list == NULL) {
        return;
    }
    g2l_linked_list_iterator_t* iterator = calloc(1, sizeof(g2l_linked_list_iterator_t));
    if (iterator == NULL) {
        return;
    }
    iterator->data = data;
    iterator->next = NULL;
    iterator->prev = g2l_linked_list_iterator_end(list);
    if (list->tail != NULL) {
        list->tail->next = iterator;
    }
    list->tail = iterator;
    if (list->head == NULL) {
        list->head = iterator;
    }
}

g2l_linked_list_iterator_t* g2l_linked_list_iterator_begin(g2l_linked_list_t* list) {
    if (list == NULL) {
        return NULL;
    }
    return list->head;
}

g2l_linked_list_iterator_t* g2l_linked_list_iterator_end(g2l_linked_list_t* list) {
    if (list == NULL) {
        return NULL;
    }
    return list->tail;
}

g2l_linked_list_iterator_t* g2l_linked_list_iterator_next(g2l_linked_list_iterator_t* iterator) {
    if (iterator == NULL) {
        return NULL;
    }
    return iterator->next;
}

g2l_linked_list_iterator_t* g2l_linked_list_iterator_prev(g2l_linked_list_iterator_t* iterator) {
    if (iterator == NULL) {
        return NULL;
    }
    return iterator->prev;
}

void* g2l_linked_list_get(g2l_linked_list_iterator_t* iterator) {
    if (iterator == NULL) {
        return NULL;
    }
    return iterator->data;
}
