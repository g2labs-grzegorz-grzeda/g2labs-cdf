/**
 * MIT License
 * Copyright (c) 2023 Grzegorz Grzęda
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
#ifndef CLI_H
#define CLI_H

// TODO: add doxygen comments

typedef void (*cli_print_t)(const char* format, ...);

typedef struct cli cli_t;

typedef int (*cli_command_t)(cli_t* cli, int argc, char** argv);

// TODO: add external parametrisation of input and argv buffer sizes
cli_t* cli_create(cli_print_t cli_print);

void cli_register(cli_t* cli, const char* name, const char* help, cli_command_t command);

void cli_process(cli_t* cli, char c);

#endif  // CLI_H