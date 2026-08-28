#ifndef TOKENIZATION_H
#define TOKENIZATION_H

#include "differentiator.h"

const int MAX_NUMBER_OF_VARS = 20;

union token_union
{
    double number;
    int var_number;
    operator_code op;
    char spec_symbol;
};

struct token_t
{
    type_data type;
    token_union data_t;
    token_t* prev;
    token_t* next;
};

struct list_t
{
    token_t* head;
    token_t* current;
    token_t* tail;
};

error_code tokenization(const char* buffer, variable_t* variables, list_t* const list);
void list_destroy(list_t* list);

#endif //TOKENIZATION_H
