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
#ifndef CALLBACK_H
#define CALLBACK_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup callback Callback
 * @brief Simple and efficient C library for handling callback
 * @{
 */

/**
 * @brief Callback handler type
 *
 * This is a callback pointer.
 * @param[in] context pointer to some context passed during registration
 * @param[in] payload pointer to some payload associated with the event ID
 */
typedef void (*callback_handler_t)(void* context, void* payload);

/**
 * @brief Callback structure type
 *
 * This is a declaration of the callback structure type. Use only by pointer.
 */
typedef struct callback callback_t;

/**
 * @brief Create the callback
 */
callback_t* callback_create(void);

/**
 * @brief Register an new callback handler
 *
 * @note This function is not thread safe! Consider registering all handlers in
 * the initialization part of your project.
 *
 * @param[in] callback pointer to the callback object
 * @param[in] handler pointer to the actual callback handler
 * @param[in] context pointer to some context for the callback handler
 * @return true if registration was successfull
 * @return false if handler was invalid or no more memory to store it
 */
bool callback_register_handler(callback_t* callback, callback_handler_t handler, void* context);

/**
 * @brief Dispatch callback
 *
 * It calls each handler registered.
 * @note This code is platform-agnostic. If this function was called from e.g.
 * - ISR
 * - another thread
 * It is the handlers job to cross thread/interrupt barriers correctly (e.g. through queues, semaphores etc.).
 * The handlers are being called in the `callback_dispatch` caller context.
 *
 * @param[in] callback pointer to the callback object
 * @param[in] payload pointer to possible payload associated with the event ID
 */
void callback_dispatch(callback_t* callback, void* payload);

/**
 * @}
 */

#endif  // CALLBACK_H
