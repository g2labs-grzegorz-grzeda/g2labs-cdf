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
#ifndef G2L_LINKED_LIST_H
#define G2L_LINKED_LIST_H

#include <stddef.h>

/**
 * @defgroup g2l-linked-list G2L Linked list
 *
 * @brief A doubly linked list.
 * @{
 */

typedef struct g2l_linked_list g2l_linked_list_t;
typedef struct g2l_linked_list_iterator g2l_linked_list_iterator_t;

/**
 * @brief Create a new linked list.
 * @return A pointer to the newly created linked list.
 * @return NULL if the linked list could not be created.
 * @note The linked list must be destroyed with g2l_linked_list_destroy() when it is
 * no longer needed.
 */
g2l_linked_list_t* g2l_linked_list_create(void);

/**
 * @brief Destroy a linked list.
 * @param list A pointer to the linked list to destroy.
 * @note This function will free all memory allocated for the linked list.
 */
void g2l_linked_list_destroy(g2l_linked_list_t* list);

/**
 * @brief Append data to the end of a linked list.
 * @param list A pointer to the linked list.
 * @param data A pointer to the data to append.
 */
void g2l_linked_list_append(g2l_linked_list_t* list, void* data);

/**
 * @brief Get an iterator to the beginning of a linked list.
 * @param list A pointer to the linked list.
 * @return A pointer to the iterator.
 * @return NULL if the iterator could not be created.
 */
g2l_linked_list_iterator_t* g2l_linked_list_iterator_begin(g2l_linked_list_t* list);

/**
 * @brief Get an iterator to the end of a linked list.
 * @param list A pointer to the linked list.
 * @return A pointer to the iterator.
 * @return NULL if the iterator could not be created.
 */
g2l_linked_list_iterator_t* g2l_linked_list_iterator_end(g2l_linked_list_t* list);

/**
 * @brief Get next iterator in a linked list.
 * @param iterator A pointer to the iterator.
 * @return A pointer to the next iterator.
 * @return NULL if there are no more iterators.
 */
g2l_linked_list_iterator_t* g2l_linked_list_iterator_next(g2l_linked_list_iterator_t* iterator);

/**
 * @brief Get previous iterator in a linked list.
 * @param iterator A pointer to the iterator.
 * @return A pointer to the previous iterator.
 * @return NULL if there are no more iterators.
 */
g2l_linked_list_iterator_t* g2l_linked_list_iterator_prev(g2l_linked_list_iterator_t* iterator);

/**
 * @brief Get the next element in a linked list.
 * @param iterator A pointer to the iterator.
 * @return A pointer to the next element.
 * @return NULL if there are no more elements.
 */
void* g2l_linked_list_get(g2l_linked_list_iterator_t* iterator);

/**
 * @}
 */

#endif  // G2L_LINKED_LIST_H
