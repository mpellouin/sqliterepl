/*
** EPITECH PROJECT, 2024
** sqliterepl
** File description:
** table
*/

#include "table.h"


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

void print_row(Row *r) {
    printf("(%d, %s, %s)\n", r->id, r->username, r->email);
}

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