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

const char* HELP_COMMAND_NAME = "help";
const char* HELP_COMMAND_HELP = "Print available commands";

typedef struct cli {
    cli_print_t print;
    linked_list_t* commands;
    char* buffer;
    size_t buffer_end;
    size_t buffer_size;
    char** parameter_buffer;
    size_t parameter_buffer_size;
    int parameter_count;
    char enter_character;
    const char* omit_characters;
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
    cli->print("Available commands:\n");
    for (linked_list_iterator_t* it = linked_list_iterator_begin(cli->commands); it;
         it = linked_list_iterator_next(it)) {
        cli_entry_t* entry = linked_list_get(it);
        cli->print(" %*s - %s\n", max_cmd_length, entry->name, entry->help);
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

static int parse_arguments(cli_t* cli) {
    if (cli->buffer_end < 1) {
        return CLI_RETURN_ERROR_PARAMETER_BUFFER_EMPTY;
    }
    cli->parameter_count = 0;
    for (char* ptr = cli->buffer; *ptr || (ptr < (cli->buffer + cli->buffer_end));) {
        ptr = get_first_non_white_character(ptr);
        if (*ptr == '\0') {
            break;
        }
        if (cli->parameter_count >= cli->parameter_buffer_size) {
            return CLI_RETURN_ERROR_PARAMETER_COUNT_EXCEEDED;
        }
        cli->parameter_buffer[cli->parameter_count++] = ptr;
        ptr = get_first_white_character(ptr);
        if (*ptr == '\0') {
            break;
        }
        *(ptr++) = '\0';
    }
    cli->buffer_end = 0;
    return CLI_RETURN_CONTINUE;
}

static int execute_command(cli_t* cli) {
    if (cli->parameter_count < 1) {
        return CLI_RETURN_ERROR_COMMAND_NOT_FOUND;
    }
    for (linked_list_iterator_t* it = linked_list_iterator_begin(cli->commands); it;
         it = linked_list_iterator_next(it)) {
        cli_entry_t* entry = linked_list_get(it);
        if (strcmp(cli->parameter_buffer[0], entry->name) == 0) {
            return entry->command(cli, cli->parameter_count, cli->parameter_buffer);
        }
    }
    command_help(cli, 0, 0);
    return CLI_RETURN_ERROR_COMMAND_NOT_FOUND;
}

cli_t* cli_create(cli_config_t* config) {
    if (!config) {
        return NULL;
    }
    cli_t* cli = calloc(1, sizeof(cli_t));
    if (!cli) {
        return NULL;
    }
    cli->buffer_size =
        (config->max_input_buffer_size ? config->max_input_buffer_size : CLI_DEFAULT_MAX_INPUT_BUFFER_SIZE);
    cli->parameter_buffer_size =
        (config->max_parameter_count ? config->max_parameter_count : CLI_DEFAULT_MAX_PARAMETER_COUNT);
    cli->buffer = calloc(cli->buffer_size, sizeof(char));
    if (!cli->buffer) {
        free(cli);
        return NULL;
    }
    cli->parameter_buffer = calloc(cli->parameter_buffer_size, sizeof(char*));
    if (!cli->parameter_buffer) {
        free(cli->buffer);
        free(cli);
        return NULL;
    }
    cli->print = config->print;
    cli->enter_character = (config->enter_character ? config->enter_character : CLI_DEFAULT_ENTER_CHARACTER);
    cli->omit_characters = (config->omit_characters ? config->omit_characters : CLI_DEFAULT_OMIT_CHARACTERS);
    cli_register(cli, HELP_COMMAND_NAME, HELP_COMMAND_HELP, command_help);
    return cli;
}

void cli_destroy(cli_t* cli) {
    if (!cli) {
        return;
    }
    linked_list_destroy(cli->commands);
    free(cli->buffer);
    free(cli->parameter_buffer);
    free(cli);
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

int cli_process(cli_t* cli, char c) {
    if ((c == cli->enter_character) && (cli->buffer_end > 0)) {
        cli->buffer[cli->buffer_end] = '\0';
        int parse_result = parse_arguments(cli);
        if (parse_result != CLI_RETURN_CONTINUE) {
            return parse_result;
        }
        return execute_command(cli);
    } else {
        if (cli->buffer_end < cli->buffer_size) {
            cli->buffer[cli->buffer_end] = c;
            cli->buffer_end++;
            return CLI_RETURN_CONTINUE;
        }
        return CLI_RETURN_ERROR_PARAMETER_BUFFER_OVERFLOW;
    }
}