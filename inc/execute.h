/*
** EPITECH PROJECT, 2024
** sqliterepl
** File description:
** execute
*/

#ifndef EXECUTE_H_
#define EXECUTE_H_

#include "ibuffer.h"
#include "table.h"

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
    EXECUTE_NOT_IMPLEMENTED
} ExecuteResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    StatementType type;
    Row insert_row;
} Statement;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED,
    PREPARE_SYNTAX_ERROR,
    PREPARE_NEGATIVE_ID,
    PREPARE_STRING_TOO_LONG
} PrepareResult;

/**
 * @brief Check for syntax errors in the given statement
 * 
 * @param b (IBuffer *) Contains the statement raw text
 * @param s (Statement *) Statement to be prepared
 * @return PrepareResult 
 */
PrepareResult prepare_insert(IBuffer *b, Statement *s);

/**
 * @brief Checks if buffer contains a known statement
 * 
 * @param b (IBuffer*)
 * @param s (Statement*)
 * @return PrepareResult 
 */
PrepareResult prepare_statement(IBuffer *b, Statement *s);

/**
 * @brief Executes the insert statement on the given table
 * 
 * @param s (Statement*)
 * @param t (Table*)
 * @return ExecuteResult 
 */
ExecuteResult execute_insert(Statement *s, Table *t);

/**
 * @brief Executes the select statement on the given table 
 * 
 * @param s (Statement*)
 * @param t (Table*)
 * @return ExecuteResult 
 */
ExecuteResult execute_select(Statement *s, Table *t);

/**
 * @brief Launch statement execution
 * 
 * @param s (Statement*)
 * @param t (Table*)
 * @return ExecuteResult 
 */
ExecuteResult execute_statement(Statement *s, Table *t);


#endif /* !EXECUTE_H_ */
