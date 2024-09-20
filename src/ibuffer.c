/*
** EPITECH PROJECT, 2024
** sqliterepl
** File description:
** ibuffer
*/

#include "ibuffer.h"

void print_prompt() {
    printf("db > ");
}

void read_input(IBuffer *b) {
    __ssize_t nread = getline(&(b->buffer), &(b->buff_length), stdin);

    if (nread <= 0) {
        printf("Error reading input.\n");
        exit(EXIT_FAILURE);
    }

    b->buffer[nread - 1] = '\0';
    b->input_length = nread - 1;
}

void close_input_buffer(IBuffer *b) {
    free(b->buffer);
    free(b);
}

IBuffer *new_input_buffer() {
    IBuffer *buff = (IBuffer *)malloc(sizeof(IBuffer));
    
    buff->buff_length = 0;
    buff->input_length = 0;
    buff->buffer = NULL;

    return buff;
}