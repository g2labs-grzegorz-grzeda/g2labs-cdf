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
#include "cli.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked-list.h"

#define COMMANDS_INPUT_BUFFER_SIZE (200)
#define COMMANDS_PARAMETER_MAX_COUNT (10)

const char* HELP_COMMAND_NAME = "help";
const char* HELP_COMMAND_HELP = "Print available commands";

typedef struct cli {
    cli_print_t cli_print;
    linked_list_t* commands;
    char* buffer;
    size_t buffer_end;
    char** argv_buffer;
    int argc;
} cli_t;

typedef struct cli_entry {
    const char* name;
    const char* help;
    cli_command_t command;
} cli_entry_t;

static int command_help(cli_t* cli, int argc, char** argv) {
    (void)argc;
    (void)argv;
    size_t max_cmd_length = 0;
    for (linked_list_iterator_t* it = linked_list_iterator_begin(cli->commands); it;
         it = linked_list_iterator_next(it)) {
        cli_entry_t* entry = linked_list_get(it);
        size_t size = strlen(entry->name);
        if (size > max_cmd_length) {
            max_cmd_length = size;
        }
    }
    cli->cli_print("Available commands:\n");
    for (linked_list_iterator_t* it = linked_list_iterator_begin(cli->commands); it;
         it = linked_list_iterator_next(it)) {
        cli_entry_t* entry = linked_list_get(it);
        cli->cli_print(" %*s - %s\n", max_cmd_length, entry->name, entry->help);
    }
    return 0;
}

static char* get_first_non_white_character(char* input) {
    while (*input) {
        if (isgraph(*input) != 0) {
            break;
        }
        input++;
    }
    return input;
}

static char* get_first_white_character(char* input) {
    while (*input) {
        if (isgraph(*input) == 0) {
            break;
        }
        input++;
    }
    return input;
}

static void parse_arguments(cli_t* cli) {
    if (cli->buffer_end < 1) {
        return;
    }
    cli->argc = 0;
    for (char* ptr = cli->buffer; *ptr || (ptr < (cli->buffer + cli->buffer_end));) {
        ptr = get_first_non_white_character(ptr);
        if (*ptr == '\0') {
            break;
        }
        cli->argv_buffer[cli->argc++] = ptr;
        ptr = get_first_white_character(ptr);
        if (*ptr == '\0') {
            break;
        }
        *(ptr++) = '\0';
    }
    cli->buffer_end = 0;
}

static int execute_command(cli_t* cli) {
    if (cli->argc < 1) {
        return -2;
    }
    for (linked_list_iterator_t* it = linked_list_iterator_begin(cli->commands); it;
         it = linked_list_iterator_next(it)) {
        cli_entry_t* entry = linked_list_get(it);
        if (strcmp(cli->argv_buffer[0], entry->name) == 0) {
            return entry->command(cli, cli->argc, cli->argv_buffer);
        }
    }
    command_help(cli, 0, 0);
    return -1;
}

cli_t* cli_create(cli_print_t cli_print) {
    cli_t* cli = calloc(1, sizeof(cli_t));
    if (!cli) {
        return NULL;
    }
    cli->buffer = calloc(COMMANDS_INPUT_BUFFER_SIZE, sizeof(char));
    if (!cli->buffer) {
        free(cli);
        return NULL;
    }
    cli->argv_buffer = calloc(COMMANDS_PARAMETER_MAX_COUNT, sizeof(char*));
    if (!cli->argv_buffer) {
        free(cli->buffer);
        free(cli);
        return NULL;
    }
    cli->cli_print = cli_print;
    cli_register(cli, HELP_COMMAND_NAME, HELP_COMMAND_HELP, command_help);
    return cli;
}

void cli_register(cli_t* cli, const char* name, const char* help, cli_command_t command) {
    if (!cli || !command || !name) {
        return;
    }
    cli_entry_t* entry = calloc(1, sizeof(cli_entry_t));
    if (!entry) {
        return;
    }
    entry->name = name;
    entry->help = help;
    entry->command = command;
    linked_list_append(cli->commands, entry);
}

void cli_process(cli_t* cli, char c) {
    if ((c == '\n') || (c == '\r')) {
        cli->buffer[cli->buffer_end] = '\0';
        parse_arguments(cli);
        execute_command(cli);
    } else {
        cli->buffer[cli->buffer_end] = c;
        cli->buffer_end++;
    }
}