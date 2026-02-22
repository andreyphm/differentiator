#include <string.h>

#include "parser.h"
#include "operations_macros.h"

node_t* GetG(char** current_character_ptr)
{
    printf("GetG\n");
    node_t* value = GetE(current_character_ptr);
    return value;
}

node_t* GetE(char** current_character_ptr)
{
    printf("GetE\n");
    node_t* value = GetT(current_character_ptr);
    while (**current_character_ptr == '+' || **current_character_ptr == '-')
    {
        int op = **current_character_ptr;
        (*current_character_ptr)++;

        node_t* value_2 = GetT(current_character_ptr);

        if (op == '+')
            value = ADD_(value, value_2);
        else
            value = SUB_(value, value_2);
    }

    return value;
}

node_t* GetT(char** current_character_ptr)
{
    printf("GetT\n");
    node_t* value = GetP(current_character_ptr);
    while (**current_character_ptr == '*' || **current_character_ptr == '/')
    {
        int op = **current_character_ptr;
        (*current_character_ptr)++;

        node_t* value_2 = GetP(current_character_ptr);

        if (op == '*')
            value = MUL_(value, value_2);
        else
            value = DIV_(value, value_2);
    }

    return value;
}

node_t* GetP(char** current_character_ptr)
{
    printf("GetP\n");
    if (**current_character_ptr == '(')
    {
        (*current_character_ptr)++;
        node_t* value = GetE(current_character_ptr);
        (*current_character_ptr)++;
        return value;
    }

    else if ('0' <= **current_character_ptr && **current_character_ptr <= '9')
        return GetN(current_character_ptr);

    else if (('a' <= **current_character_ptr && **current_character_ptr <= 'z') || **current_character_ptr == '_')
        return GetV(current_character_ptr);

    else return nullptr;
}

node_t* GetN(char** current_character_ptr)
{
    printf("GetN\n");
    int value = 0;
    while ('0' <= **current_character_ptr && **current_character_ptr <= '9')
    {
        value = value * 10 + (**current_character_ptr - '0');
        (*current_character_ptr)++;
    }

    return NUM_(value);
}

node_t* GetV(char** current_character_ptr)
{
    printf("GetV\n");
    char buffer[20] = {};
    size_t i = 1;
    buffer[0] = **current_character_ptr;
    (*current_character_ptr)++;

    while (('a' <= **current_character_ptr && **current_character_ptr <= 'z')
            || ('0' <= **current_character_ptr && **current_character_ptr <= '9')
            || **current_character_ptr == '_')
    {
        buffer[i] = **current_character_ptr;
        (*current_character_ptr)++;
        i++;
    }

    buffer[i+1] = '\0';
    const char* variable_str = buffer;

    return VAR_(variable_str);
}
