/*
** EPITECH PROJECT, 2024
** sqliterepl
** File description:
** table
*/

#ifndef TABLE_H_
#define TABLE_H_

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 256
#define TABLE_MAX_PAGES 100

typedef struct row {
    __uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

typedef struct table {
    __uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;

extern const __uint32_t ID_SIZE;
extern const __uint32_t USERNAME_SIZE;
extern const __uint32_t EMAIL_SIZE;
extern const __uint32_t ID_OFFSET;
extern const __uint32_t USERNAME_OFFSET;
extern const __uint32_t EMAIL_OFFSET;
extern const __uint32_t ROW_SIZE;
extern const __uint32_t PAGE_SIZE;
extern const __uint32_t ROWS_PER_PAGE;
extern const __uint32_t TABLE_MAX_ROWS;

/**
 * @brief Prints the content of a Row
 * 
 * @param r Row *
 */
void print_row(Row *r);

/**
 * @brief Returns the adress of the requested tow in the table
 * 
 * @param t (Table*)
 * @param row_num (__uint32_t) Row number in the table
 * @return (void*)
 */
void *get_row_slot(Table *t, __uint32_t row_num);

/**
 * @brief Serializes a Row in a given destination
 * 
 * @param s (Row*) Row to serialize
 * @param d (void*) Adress to serialize to
 */
void serialize_row(Row *s, void *d);


/**
 * @brief Deserializes a row from a given address
 * 
 * @param d (Row*)
 * @param s (void*) address containing the serialized data
 */
void deserialize_row(Row *d, void* s);


/**
 * @brief Creates a new Table
 * 
 * @return (Table*) 
 */
Table *new_table();


/**
 * @brief Frees a Table
 * 
 * @param t (Table*)
 */
void free_table(Table *t);

#endif /* !TABLE_H_ */
