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
#include "modbus.h"
#include <stdlib.h>
#include "linked-list.h"

#define MODBUS_FUNCTION_READ_HOLDING_REGISTERS (0x03)
#define MODBUS_FUNCTION_WRITE_MULTIPLE_REGISTERS (0x10)

#define MODBUS_ERROR_CODE_FUNCTION_MASK (0x80)
#define MODBUS_ERROR_CODE_ILLEGAL_FUNCTION (0x01)
#define MODBUS_ERROR_CODE_ILLEGAL_DATA_ADDRESS (0x02)

#define MODBUS_CRC_INITIAL_VALUE (0xFFFF)
#define MODBUS_CRC_POLYNOMIAL (0xA001)

typedef struct modbus_register {
    uint16_t address;
    uint16_t range;
    modbus_register_cb_t read_cb;
    modbus_register_cb_t write_cb;
} modbus_register_t;

typedef struct modbus {
    uint8_t slave_address;
    modbus_respond_cb_t respond_cb;
    linked_list_t* registers;
    size_t max_stream_registers;
    uint16_t* stream_registers;
    uint8_t* response_frame;
} modbus_t;

static uint16_t modbus_calculate_crc16(const uint8_t* buffer, uint16_t buffer_length) {
    uint16_t crc = MODBUS_CRC_INITIAL_VALUE;  // Initial CRC value
    for (uint16_t pos = 0; pos < buffer_length; pos++) {
        crc ^= (uint16_t)buffer[pos];  // XOR byte into least sig. byte of crc

        for (int i = 8; i != 0; i--) {         // Loop over each bit
            if ((crc & 0x0001) != 0) {         // If the LSB is set
                crc >>= 1;                     // Shift right
                crc ^= MODBUS_CRC_POLYNOMIAL;  // XOR with the polynomial
            } else {                           // Else LSB is not set
                crc >>= 1;                     // Just shift right
            }
        }
    }
    return crc;
}

static void send_modbus_error(modbus_t* modbus, uint8_t function, uint8_t error_code) {
    uint8_t response[] = {modbus->slave_address, function | MODBUS_ERROR_CODE_FUNCTION_MASK, error_code, 0, 0};
    uint16_t crc = modbus_calculate_crc16(response, sizeof(response) - 2);
    response[sizeof(response) - 2] = crc & 0xff;
    response[sizeof(response) - 1] = crc >> 8;
    modbus->respond_cb(response, sizeof(response));
}

static void send_modbus_response(modbus_t* modbus, uint8_t function, uint16_t* data, size_t count) {
    modbus->response_frame[0] = modbus->slave_address;
    modbus->response_frame[1] = function;
    modbus->response_frame[2] = count * 2;
    for (size_t i = 0; i < count; i++) {
        modbus->response_frame[3 + i * 2] = data[i] >> 8;
        modbus->response_frame[4 + i * 2] = data[i] & 0xff;
    }
    uint16_t crc = modbus_calculate_crc16(modbus->response_frame, count * 2 + 3);
    modbus->response_frame[count * 2 + 3] = crc & 0xff;
    modbus->response_frame[count * 2 + 4] = crc >> 8;
    modbus->respond_cb(modbus->response_frame, count * 2 + 5);
}

static void send_write_acknowledge(modbus_t* modbus, uint8_t function_code, uint16_t address, uint16_t count) {
    uint8_t response[] = {
        modbus->slave_address, function_code, address >> 8, address & 0xff, count >> 8, count & 0xFF, 0, 0};
    uint16_t crc = modbus_calculate_crc16(response, sizeof(response) - 2);
    response[sizeof(response) - 2] = crc & 0xff;
    response[sizeof(response) - 1] = crc >> 8;
    modbus->respond_cb(response, sizeof(response));
}

static void process_modbus_read_holdings_registers(modbus_t* modbus, uint16_t address, size_t count) {
    for (linked_list_iterator_t* it = linked_list_iterator_begin(modbus->registers); it;
         it = linked_list_iterator_next(it)) {
        modbus_register_t* reg = linked_list_get(it);
        if ((address >= reg->address) && (address < reg->address + reg->range) &&
            (address + count <= reg->address + reg->range) && (reg->read_cb != NULL)) {
            for (size_t i = 0; i < count; i++) {
                uint16_t value;
                if (reg->read_cb(address + i, &value)) {
                    modbus->stream_registers[i] = value;
                } else {
                    break;
                }
            }
            send_modbus_response(modbus, MODBUS_FUNCTION_READ_HOLDING_REGISTERS, modbus->stream_registers, count);
            return;
        }
    }
    send_modbus_error(modbus, MODBUS_FUNCTION_READ_HOLDING_REGISTERS, MODBUS_ERROR_CODE_ILLEGAL_DATA_ADDRESS);
}

static void process_modbus_write_holding_registers(modbus_t* modbus,
                                                   uint16_t address,
                                                   size_t count,
                                                   const uint8_t* payload) {
    for (linked_list_iterator_t* it = linked_list_iterator_begin(modbus->registers); it;
         it = linked_list_iterator_next(it)) {
        modbus_register_t* reg = linked_list_get(it);
        if ((address >= reg->address) && (address < reg->address + reg->range) &&
            (address + count <= reg->address + reg->range) && (reg->write_cb != NULL)) {
            for (size_t i = 0; i < count; i++) {
                uint16_t value = (payload[i * 2] << 8) | payload[i * 2 + 1];
                payload += 2;
                if (!reg->write_cb(address + i, &value)) {
                    break;
                }
            }
            send_write_acknowledge(modbus, MODBUS_FUNCTION_WRITE_MULTIPLE_REGISTERS, address, count);
            return;
        }
    }
    send_modbus_error(modbus, MODBUS_FUNCTION_WRITE_MULTIPLE_REGISTERS, MODBUS_ERROR_CODE_ILLEGAL_DATA_ADDRESS);
}

modbus_t* modbus_create(uint8_t slave_address, modbus_respond_cb_t respond_cb, size_t max_stream_registers) {
    modbus_t* modbus = calloc(1, sizeof(modbus_t));
    if (!modbus) {
        return NULL;
    }
    modbus->registers = linked_list_create();
    if (!modbus->registers) {
        free(modbus);
        return NULL;
    }
    modbus->stream_registers = calloc(max_stream_registers, sizeof(modbus->stream_registers[0]));
    if (!modbus->stream_registers) {
        linked_list_destroy(modbus->registers);
        free(modbus);
        return NULL;
    }
    modbus->response_frame = calloc(max_stream_registers * 2 + 7, sizeof(modbus->response_frame[0]));
    if (!modbus->response_frame) {
        free(modbus->stream_registers);
        linked_list_destroy(modbus->registers);
        free(modbus);
        return NULL;
    }
    modbus->max_stream_registers = max_stream_registers;
    modbus->slave_address = slave_address;
    modbus->respond_cb = respond_cb;
    return modbus;
}

void modbus_destroy(modbus_t* modbus) {
    if (!modbus) {
        return;
    }
    free(modbus->response_frame);
    free(modbus->stream_registers);
    linked_list_destroy(modbus->registers);
    free(modbus);
}

void modbus_register(modbus_t* modbus,
                     uint16_t address,
                     uint8_t range,
                     modbus_register_cb_t read_cb,
                     modbus_register_cb_t write_cb) {
    if (!modbus || range == 0 || (read_cb == NULL && write_cb == NULL)) {
        return;
    }
    modbus_register_t* reg = calloc(1, sizeof(modbus_register_t));
    if (!reg) {
        return;
    }
    reg->address = address;
    reg->range = range;
    reg->read_cb = read_cb;
    reg->write_cb = write_cb;
    linked_list_append(modbus->registers, reg);
}

void modbus_process(modbus_t* modbus, const uint8_t* modbus_frame, size_t frame_length) {
    if (!modbus || !modbus_frame || frame_length < 7) {
        return;
    }
    if (modbus_frame[0] != modbus->slave_address) {
        return;
    }
    if (modbus_calculate_crc16(modbus_frame, frame_length) != 0) {
        return;
    }
    uint8_t function_code = modbus_frame[1];
    if (function_code != MODBUS_FUNCTION_READ_HOLDING_REGISTERS &&
        function_code != MODBUS_FUNCTION_WRITE_MULTIPLE_REGISTERS) {
        send_modbus_error(modbus, function_code, MODBUS_ERROR_CODE_ILLEGAL_FUNCTION);
        return;
    }
    uint16_t address = (modbus_frame[2] << 8) | modbus_frame[3];
    uint16_t count = (modbus_frame[4] << 8) | modbus_frame[5];
    if (count > modbus->max_stream_registers) {
        send_modbus_error(modbus, MODBUS_FUNCTION_READ_HOLDING_REGISTERS, MODBUS_ERROR_CODE_ILLEGAL_DATA_ADDRESS);
        return;
    }
    if (function_code == MODBUS_FUNCTION_READ_HOLDING_REGISTERS) {
        process_modbus_read_holdings_registers(modbus, address, count);
        return;
    }
    if (function_code == MODBUS_FUNCTION_WRITE_MULTIPLE_REGISTERS) {
        process_modbus_write_holding_registers(modbus, address, count, modbus_frame + 7);
        return;
    }
}