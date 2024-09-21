/*
** EPITECH PROJECT, 2024
** sqliterepl
** File description:
** sqliterepl
*/

#ifndef SQLITEREPL_H_
#define SQLITEREPL_H_

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "execute.h"

typedef enum {
    META_SUCCESS,
    META_UNRECOGNIZED
} MetaResult;

/**
 * @brief Executes any meta command (.cmd) contained in the input buffer
 * 
 * @param b (IBuffer*)
 * @return MetaResult 
 */
MetaResult do_meta_command(IBuffer *b, Table *t);



#endif /* !SQLITEREPL_H_ */
