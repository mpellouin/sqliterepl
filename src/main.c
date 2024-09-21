/*
** EPITECH PROJECT, 2024
** sqliterepl
** File description:
** main
*/

#include "sqliterepl.h"

MetaResult do_meta_command(IBuffer *b, Table *t) {
    if (!strcmp(b->buffer, ".exit")) {
        db_close(t);
        close_input_buffer(b);
        exit(EXIT_SUCCESS);
    } else {
        return META_UNRECOGNIZED;
    }
}

int main(int ac, char **av) {
    if (ac < 2) {
        printf("A database file must be provided.\n");
        return EXIT_FAILURE;
    }
    char *db_filepath = av[1];
    IBuffer *b = new_input_buffer();
    Table *t = db_open(db_filepath);

    while (true) {
        print_prompt();
        read_input(b);
        if (b->buffer[0] == '.') {
            switch(do_meta_command(b, t)) {
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
            case(PREPARE_STRING_TOO_LONG):
                printf("Given string is too long.\n");
                continue;
            case(PREPARE_NEGATIVE_ID):
                printf("Error: ID must be positive.\n");
                continue;
        }

        switch(execute_statement(&s, t)) {
            case(EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case(EXECUTE_TABLE_FULL):
                printf("Error: Table full.\n");
                break;
            case(EXECUTE_NOT_IMPLEMENTED):
                printf("Error: Not implemented.\n");
        }
    }
}