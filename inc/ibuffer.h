/*
** EPITECH PROJECT, 2024
** sqliterepl
** File description:
** ibuffer
*/

#ifndef IBUFFER_H_
#define IBUFFER_H_

#include <stdlib.h>
#include <stdio.h>

typedef struct InputBuffer {
    char* buffer;
    size_t buff_length;
    size_t input_length;
} IBuffer;

/**
 * @brief Prints the cli prompt
 * 
 */
void print_prompt();

/**
 * @brief Reads the input from the user and feeds it to the IBuffer
 * 
 * @param b Ibuffer *
 */
void read_input(IBuffer *b);

/**
 * @brief Closes and frees the input buffer
 * 
 * @param b IBuffer *
 */
void close_input_buffer(IBuffer *b);

/**
 * @brief Creates a new input buffer
 * 
 * @return IBuffer* 
 */
IBuffer *new_input_buffer();


#endif /* !IBUFFER_H_ */
