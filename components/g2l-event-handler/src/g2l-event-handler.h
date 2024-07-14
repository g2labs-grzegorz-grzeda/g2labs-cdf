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
#ifndef G2L_EVENT_HANDLER_H
#define G2L_EVENT_HANDLER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup g2l_event_handler Event Handler
 * @brief Simple and efficient C library for handling events
 * @{
 */

/**
 * @brief Event handler type
 * This is a structure that holds the actual event handling callback and some context.
 * It is used to store the event handler in the list of handlers.
 * @note This structure is not meant to be used directly by the user.
 */
typedef struct g2l_event_handler g2l_event_handler_t;

/**
 * @brief Event handler callback function type
 *
 * This is a callback pointer the the actual event handling code.
 * @param[in] handler pointer to the event handler
 * @param[in] id event ID
 * @param[in] context pointer to some context passed during registration
 * @param[in] payload pointer to some payload associated with the event ID
 * @param[in] size size of the payload
 */
typedef void (*g2l_event_handler_callback_t)(g2l_event_handler_t* handler,
                                             uint16_t id,
                                             void* context,
                                             void* payload,
                                             size_t size);

/**
 * @brief Create a new event handler
 * @return pointer to the newly created event handler
 * @return NULL if the event handler could not be created
 */
g2l_event_handler_t* g2l_event_handler_create(void);

/**
 * @brief Destroy the event handler
 * @param[in] handler pointer to the event handler to destroy
 */
void g2l_event_handler_destroy(g2l_event_handler_t* handler);

/**
 * @brief Register an event handler
 * @param[in] handler pointer to the event handler
 * @param[in] id event ID that we are registering for
 * @param[in] context pointer to some context for the event @ref handler
 * @param[in] callback pointer to the actual event handling callback
 * @return true if registration was successfull
 * @return false if handler was invalid or no more memory to store it
 */
bool g2l_event_handler_register(g2l_event_handler_t* handler,
                                uint16_t id,
                                void* context,
                                g2l_event_handler_callback_t callback);

/**
 * @brief Send event to the handler
 * @param[in] handler pointer to the event handler
 * @param[in] id event ID to be sent
 * @param[in] payload pointer to possible payload associated with the event ID
 * @return true if handler was executed for give event
 * @return false otherwise
 */
bool g2l_event_handler_send(g2l_event_handler_t* handler, uint16_t id, void* payload, size_t size);

/**
 * @}
 */

#endif  // G2L_EVENT_HANDLER_H