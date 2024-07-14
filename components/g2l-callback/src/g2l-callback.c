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
#include <stddef.h>
#include <stdlib.h>
#include "g2l-linked-list.h"

typedef struct callbacks_entry {
    g2l_callback_handler_t handler;
    void* context;
} g2l_callbacks_entry_t;

typedef struct g2l_callback {
    g2l_linked_list_t* list;
} g2l_callback_t;

g2l_callback_t* g2l_callback_create(void) {
    g2l_callback_t* callbacks = calloc(1, sizeof(g2l_callback_t));
    if (callbacks) {
        callbacks->list = g2l_linked_list_create();
    }
    return callbacks;
}

bool g2l_callback_register_handler(g2l_callback_t* callbacks, g2l_callback_handler_t handler, void* context) {
    if (!callbacks || !handler) {
        return false;
    }
    g2l_callbacks_entry_t* entry = calloc(1, sizeof(g2l_callbacks_entry_t));
    if (!entry) {
        return false;
    }
    entry->handler = handler;
    entry->context = context;
    g2l_linked_list_append(callbacks->list, entry);
    return true;
}

void g2l_callback_dispatch(g2l_callback_t* callbacks, void* payload) {
    if (!callbacks) {
        return;
    }
    for (g2l_linked_list_iterator_t* it = g2l_linked_list_iterator_begin(callbacks->list); it;
         it = g2l_linked_list_iterator_next(it)) {
        g2l_callbacks_entry_t* entry = g2l_linked_list_get(it);
        entry->handler(entry->context, payload);
    }
}
