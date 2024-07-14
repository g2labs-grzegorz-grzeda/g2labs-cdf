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

#include <stddef.h>

/**
 * @defgroup event_handler Event Handler
 * @brief Simple and efficient C library for handling events
 * @{
 */
#define CLI_DEFAULT_MAX_INPUT_BUFFER_SIZE 200 /**< Default maximum size of input buffer */
#define CLI_DEFAULT_MAX_PARAMETER_COUNT 10    /**< Default maximum number of parameters */
#define CLI_DEFAULT_ENTER_CHARACTER '\r'      /**< Default enter character */
#define CLI_DEFAULT_OMIT_CHARACTERS "\t\n"    /**< Default omit characters */

#define CLI_RETURN_ERROR_COMMAND_NOT_FOUND -1         /**< Command not found */
#define CLI_RETURN_ERROR_PARAMETER_COUNT_EXCEEDED -2  /**< Exceeded maximum number of parameters */
#define CLI_RETURN_ERROR_PARAMETER_BUFFER_OVERFLOW -3 /**< Exceeded maximum size of parameter buffer */
#define CLI_RETURN_ERROR_PARAMETER_BUFFER_EMPTY -4    /**< Parameter buffer is empty */
#define CLI_RETURN_CONTINUE -5                        /**< Continue processing */

typedef void (*cli_print_t)(const char* format, ...); /**< Print function pointer */

typedef struct cli cli_t;

typedef struct cli_config {
    cli_print_t print;            /**< Print function */
    size_t max_input_buffer_size; /**< Maximum input buffer size */
    size_t max_parameter_count;
    char enter_character;
    const char* omit_characters;
} cli_config_t; /**< CLI configuration structure definition */

/**
 * @brief CLI Command handler type
 * @param[in] cli pointer to CLI instance
 * @param[in] argc number of passed parameters
 * @param[in] argv parameter buffer
 * @return integer with command status
 */
typedef int (*cli_command_t)(cli_t* cli, int argc, char** argv);

/**
 * @brief Create CLI instance based of configuration data
 *
 * @param[in] config pointer to configuration structure
 * @return pointer to CLI instance
 * @return NULL in case of errors
 */
cli_t* cli_create(cli_config_t* config);

/**
 * @brief Destroy a CLI instance
 * @param[in] cli pointer to the CLI instance
 */
void cli_destroy(cli_t* cli);

/**
 * @brief Register a new command
 * @param[in] cli pointer to CLI instance
 * @param[in] name C-Str with command name to display with help command
 * @param[in] help C-Str with help text to display with help command
 * @param[in] command pointer to the command handler
 */
void cli_register(cli_t* cli, const char* name, const char* help, cli_command_t command);

/**
 * @brief Process the CLI with new input character
 * @param[in] cli pointer to CLI instance
 * @param[in] c new input character
 * @return return code from CLI in case of error
 * @return return code from executed command
 */
int cli_process(cli_t* cli, char c);

/**
 * @}
 */

#endif  // CLI_H