#ifndef TOKENIZATION_H
#define TOKENIZATION_H

#include "differentiator.h"

const int MAX_NUMBER_OF_VARS = 20;

typedef union
{
    double number;
    int var_number;
    operator_code op;
    char spec_symbol;
} token_union;

typedef struct token_t
{
    type_data type;
    token_union data_t;
    token_t* prev;
    token_t* next;
} token_t;

typedef struct list_t
{
    token_t* head;
    token_t* current;
    token_t* tail;
} list_t;

typedef struct variable_t
{
    int number;
    const char* name;
    size_t length;
} variable_t;

error_code tokenization(const char* buffer, variable_t* variables);
void skip_spaces(const char** string);

bool try_digit(const char** buffer, list_t* const list);
bool try_char_op(const char** buffer, list_t* const list);
bool try_function(const char** buffer, list_t* const list);
bool try_bracket(const char** buffer, list_t* const list);
bool try_variable(const char** buffer, list_t* const list, variable_t* variables);

token_t* list_push_back(const type_data type, token_union data, list_t* const list);
token_t* create_token(const type_data type, token_union data, list_t* const list);
void list_dump(list_t* const list, const char* const txt_file_name, const char* const png_file_name, const variable_t* const variables);
void list_destroy(list_t* list, variable_t* variables);

#endif //TOKENIZATION_H
