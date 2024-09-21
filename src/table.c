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

Cursor *table_start(Table *t) {
    Cursor *c = (Cursor *)malloc(sizeof(Cursor));
    c->table = t;
    c->row_num = 0;
    c->is_end_of_table = t->num_rows == 0;
    return c;
}

Cursor *table_end(Table *t) {
    Cursor *c = (Cursor *)malloc(sizeof(Cursor));
    c->table = t;
    c->row_num = t->num_rows;
    c->is_end_of_table = true;
    return c;
}

void *get_page(Pager *p, __uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d > %d.\n", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (p->pages[page_num] == NULL) {
        void *page = malloc(PAGE_SIZE);
        __uint32_t num_pages = p->file_length / PAGE_SIZE;

        if (p->file_length % PAGE_SIZE) {
            num_pages += 1;
        }

        if (page_num <= num_pages) {
            lseek(p->fd, PAGE_SIZE * page_num, SEEK_SET);
            ssize_t bytes_read = read(p->fd, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error reading file: %d.\n", errno);
                exit(EXIT_FAILURE);
            }
        }

        p->pages[page_num] = page;
    }
    return p->pages[page_num];
}

void *get_cursor_value(Cursor *c) {
    __uint32_t page_num = c->row_num / ROWS_PER_PAGE;
    void *page = get_page(c->table->pager, page_num);

    __uint32_t row_offset = c->row_num % ROWS_PER_PAGE;
    __uint32_t byte_offset = row_offset * ROW_SIZE;

    return page + byte_offset;
}

void cursor_advance(Cursor *c) {
    c->row_num += 1;
    if (c->row_num >= c->table->num_rows) {
        c->is_end_of_table = true;
    }
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

Pager *pager_open(const char *filename) {
    int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);

    if (fd == -1) {
        printf("Unable to open file.\n");
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(fd, 0, SEEK_END);
    Pager *p = (Pager*)malloc(sizeof(Pager));
    p->fd = fd;
    p->file_length = (__uint32_t)file_length;
    
    for (__uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        p->pages[i] = NULL;
    }
    return p;
}

void pager_flush(Pager *p, __uint32_t page_num, __uint32_t size) {
    if (p->pages[page_num] == NULL) {
        printf("Error: Tried to flush null page.\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(p->fd, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        printf("Error seeking: %d.\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(p->fd, p->pages[page_num], size);
    if (bytes_written == -1) {
        printf("Error writing : %d.\n", errno);
        exit(EXIT_FAILURE);
    }
}

Table *db_open(const char *filename) {
    Pager *p = pager_open(filename);
    __uint32_t num_rows = p->file_length / ROW_SIZE;


    Table *t = (Table *)malloc(sizeof(Table));
    t->num_rows = num_rows;
    t->pager = p;
    return t;
}

void db_close(Table *t) {
    Pager *p = t->pager;
    __uint32_t full_num_pages = t->num_rows / ROWS_PER_PAGE;

    for (__uint32_t i = 0; i < full_num_pages; i++) {
        if (p->pages[i] == NULL) {
            continue;
        }
        pager_flush(p, i, PAGE_SIZE);
        free(p->pages[i]);
        p->pages[i] = NULL;
    }

    __uint32_t num_additional_rows = t->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
    __uint32_t page_num = full_num_pages;
    if (p->pages[page_num] != NULL) {
      pager_flush(p, page_num, num_additional_rows * ROW_SIZE);
      free(p->pages[page_num]);
      p->pages[page_num] = NULL;
    }
  }

    int close_state = close(p->fd);
    if (close_state == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    free(p);
    free(t);
}