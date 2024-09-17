#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

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
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    StatementType type;
} Statement;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED,
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
        return PREPARE_SUCCESS;
    }

    if (!strcmp(b->buffer, "select")) {
        s->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED;
}

void execute_statement(Statement *s) {
    switch (s->type) {
        case STATEMENT_INSERT:
            printf("This is where we would do the insert.\n");
            break;
        case STATEMENT_SELECT:
            printf("This is where we would do the select.\n");
            break;
    }
}

int main(int ac, char **av) {
    IBuffer *b = new_input_buffer();

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
        }

        execute_statement(&s);
        printf("Executed.\n");
    }
}