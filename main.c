#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 256
#define TABLE_MAX_PAGES 100

typedef struct row {
    __uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct table {
    __uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;

typedef struct InputBuffer {
    char* buffer;
    size_t buff_length;
    size_t input_length;
} IBuffer;

typedef enum {
    META_SUCCESS,
    META_UNRECOGNIZED
} MetaResult;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
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
    PREPARE_SYNTAX_ERROR
} PrepareResult;

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

MetaResult do_meta_command(IBuffer *b) {
    if (!strcmp(b->buffer, ".exit")) {
        close_input_buffer(b);
        exit(EXIT_SUCCESS);
    } else {
        return META_UNRECOGNIZED;
    }
}

PrepareResult prepare_statement(IBuffer *b, Statement *s) {
    if (!strncmp(b->buffer, "insert", 6)) {
        s->type = STATEMENT_INSERT;
        int args_assigned = sscanf(b->buffer, "insert %d %s %s", &(s->insert_row.id), &(s->insert_row.username), &(s->insert_row.email));
        if (args_assigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }

    if (!strcmp(b->buffer, "select")) {
        s->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED;
}

void print_row(Row *r) {
    printf("(%d, %s, %s)\n", r->id, r->username, r->email);
}

const __uint32_t ID_SIZE = size_of_attribute(Row, id);
const __uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const __uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const __uint32_t ID_OFFSET = 0;
const __uint32_t USERNAME_OFFSET = ID_SIZE + ID_OFFSET;
const __uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const __uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const __uint32_t PAGE_SIZE = 4096;
const __uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const __uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

void *get_row_slot(Table *t, __uint32_t row_num) {
    __uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = t->pages[page_num];

    if (page == NULL) {
        page = t->pages[page_num] = malloc(PAGE_SIZE);
    }
    __uint32_t row_offset = row_num % ROWS_PER_PAGE;
    __uint32_t byte_offset = row_offset * ROW_SIZE;

    return page + byte_offset;
}

void serialize_row(Row *s, void *d) {
    memcpy(d + ID_OFFSET, &(s->id), ID_SIZE);
    memcpy(d + USERNAME_OFFSET, &(s->username), USERNAME_SIZE);
    memcpy(d + EMAIL_OFFSET, &(s->email), EMAIL_SIZE);
}

void deserialize_row(Row *d, void* s) {
    memcpy(&(d->id), s + ID_OFFSET, ID_SIZE);
    memcpy(&(d->username), s + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(d->email), s + EMAIL_OFFSET, EMAIL_SIZE);
}


ExecuteResult execute_insert(Statement *s, Table *t) {
    if (t->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }
    void *slot = get_row_slot(t, t->num_rows);
    serialize_row(&(s->insert_row), slot);
    t->num_rows++;

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement *s, Table *t) {
    Row row;
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
}

Table *new_table() {
    Table *t = (Table *)malloc(sizeof(Table));
    t->num_rows = 0;

    for (__uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        t->pages[i] = NULL;
    }
    return t;
}

void free_table(Table *t) {
    for (__uint32_t i = 0; t->pages[i]; i++) {
        free(t->pages[i]);
    }
    free(t);
}

int main(int ac, char **av) {
    IBuffer *b = new_input_buffer();
    Table *t = new_table();

    while (true) {
        print_prompt();
        read_input(b);
        if (b->buffer[0] == '.') {
            switch(do_meta_command(b)) {
                case META_SUCCESS:
                    continue;
                case META_UNRECOGNIZED:
                    printf("Unrecognized command '%s'.\n", b->buffer);
                    continue;
            }
        }

        Statement s;
        switch(prepare_statement(b, &s)) {
            case(PREPARE_SUCCESS):
                break;
            case(PREPARE_UNRECOGNIZED):
                printf("Unrecognized keyword at the start of '%s'\n", b->buffer);
                continue;
            case(PREPARE_SYNTAX_ERROR):
                printf("Syntax Error. Could not parse statement.\n");
                continue;
        }

        switch(execute_statement(&s, t)) {
            case(EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case(EXECUTE_TABLE_FULL):
                printf("Error: Table full.\n");
                break;
        }
    }
}