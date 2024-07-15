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
#ifndef MODBUS_H
#define MODBUS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct modbus modbus_t;

typedef void (*modbus_respond_cb_t)(const uint8_t* data, size_t len);

typedef bool (*modbus_register_cb_t)(uint16_t address, uint16_t* value);

modbus_t* modbus_create(uint8_t slave_address, modbus_respond_cb_t respond_cb, size_t max_stream_registers);

void modbus_destroy(modbus_t* modbus);

void modbus_register(modbus_t* modbus,
                     uint16_t address,
                     uint8_t range,
                     modbus_register_cb_t read_cb,
                     modbus_register_cb_t write_cb);

void modbus_process(modbus_t* modbus, const uint8_t* data, size_t len);

#endif  // MODBUS_H
