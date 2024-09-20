/*
** EPITECH PROJECT, 2024
** sqliterepl
** File description:
** execute
*/

#include "sqliterepl.h"

PrepareResult prepare_insert(IBuffer *b, Statement *s) {
    strtok(b->buffer, " ");
    char *id = strtok(NULL, " ");
    char *username = strtok(NULL, " ");
    char *email = strtok(NULL, " ");

    if (id == NULL || username == NULL || email == NULL) {
        return PREPARE_SYNTAX_ERROR;
    }

    int id_num = atoi(id);
    if (id_num < 0) {
        return PREPARE_NEGATIVE_ID;
    }

    if (strlen(username) >= USERNAME_SIZE || strlen(email) >= EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    s->insert_row.id = id_num;
    strcpy(s->insert_row.email, email);
    strcpy(s->insert_row.username, username);
    s->type = STATEMENT_INSERT;
    return PREPARE_SUCCESS;
}

PrepareResult prepare_statement(IBuffer *b, Statement *s) {
    if (!strncmp(b->buffer, "insert", 6)) {
        return prepare_insert(b, s);
    }

    if (!strcmp(b->buffer, "select")) {
        s->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED;
}

ExecuteResult execute_insert(Statement *s, Table *t) {
    if (t->num_rows + 1 >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }
    void *slot = get_row_slot(t, t->num_rows);
    serialize_row(&(s->insert_row), slot);
    t->num_rows++;

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement *s, Table *t) {
    Row row;
    // temp
    s->type = STATEMENT_SELECT;
    for (__uint32_t i = 0; i < t->num_rows; i++) {
        deserialize_row(&row, get_row_slot(t, i));
        print_row(&row);
    }

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement *s, Table *t) {
    switch (s->type) {
        case STATEMENT_INSERT:
            return execute_insert(s, t);
        case STATEMENT_SELECT:
            return execute_select(s, t);
    }
    return EXECUTE_NOT_IMPLEMENTED;
}