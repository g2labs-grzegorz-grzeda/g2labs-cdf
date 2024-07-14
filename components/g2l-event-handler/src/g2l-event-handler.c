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
#include "g2l-event-handler.h"
#include <stddef.h>
#include <stdlib.h>
#include "g2l-linked-list.h"

typedef struct event_handler_entry {
    uint16_t id;
    void* context;
    g2l_event_handler_callback_t callback;
} g2l_event_handler_entry_t;

typedef struct g2l_event_handler {
    g2l_linked_list_t* handlers;
} g2l_event_handler_t;

g2l_event_handler_t* g2l_event_handler_create(void) {
    g2l_event_handler_t* handler = calloc(1, sizeof(g2l_event_handler_t));
    if (!handler) {
        return NULL;
    }
    handler->handlers = g2l_linked_list_create();
    if (!handler->handlers) {
        free(handler);
        return NULL;
    }
    return handler;
}

void g2l_event_handler_destroy(g2l_event_handler_t* handler) {
    if (!handler) {
        return;
    }
    g2l_linked_list_destroy(handler->handlers);
    free(handler);
}

bool g2l_event_handler_register(g2l_event_handler_t* handler,
                                uint16_t id,
                                void* context,
                                g2l_event_handler_callback_t callback) {
    if (!handler || !callback) {
        return false;
    }
    g2l_event_handler_entry_t* entry = calloc(1, sizeof(g2l_event_handler_entry_t));
    if (!entry) {
        return false;
    }
    entry->id = id;
    entry->context = context;
    entry->callback = callback;
    g2l_linked_list_append(handler->handlers, entry);
    return true;
}

bool g2l_event_handler_send(g2l_event_handler_t* handler, uint16_t id, void* payload, size_t size) {
    if (!handler) {
        return false;
    }
    bool was_sent_at_least_to_one_handler = false;
    for (g2l_linked_list_iterator_t* it = g2l_linked_list_iterator_begin(handler->handlers); it;
         it = g2l_linked_list_iterator_next(it)) {
        g2l_event_handler_entry_t* entry = g2l_linked_list_get(it);
        if (entry->id == id) {
            entry->callback(handler, id, entry->context, payload, size);
            was_sent_at_least_to_one_handler = true;
        }
    }
    return was_sent_at_least_to_one_handler;
}
