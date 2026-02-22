#include "parser.h"
#include "operations_macros.h"

node_t* GetG(char* current_character_ptr)
{
    node_t* value = GetE(current_character_ptr);
    return value;
}

node_t* GetE(char* current_character_ptr)
{
    node_t* value = GetT(current_character_ptr);
    while (*current_character_ptr == '+' || *current_character_ptr == '-')
    {
        int op = *current_character_ptr;
        current_character_ptr++;

        node_t* value_2 = GetT(current_character_ptr);

        if (op == '+')
            value = ADD_(value, value_2);
        else
            value = SUB_(value, value_2);
    }

    return value;
}

node_t* GetT(char* current_character_ptr)
{
    node_t* value = GetP(current_character_ptr);
    while (*current_character_ptr == '*' || *current_character_ptr == '/')
    {
        int op = *current_character_ptr;
        current_character_ptr++;

        node_t* value_2 = GetP(current_character_ptr);

        if (op == '*')
            value = MUL_(value, value_2);
        else
            value = DIV_(value, value_2);
    }

    return value;
}

node_t* GetP(char* current_character_ptr)
{
    if (*current_character_ptr == '(')
    {
        current_character_ptr++;
        node_t* value = GetE(current_character_ptr);
        current_character_ptr++;
        return value;
    }

    else
        return GetN(current_character_ptr);
}

node_t* GetN(char* current_character_ptr)
{
    int value = 0;
    while ('0' <= *current_character_ptr && *current_character_ptr <= '9')
    {
        value = value * 10 + (*current_character_ptr - '0');
        current_character_ptr++;
    }

    return NUM_(value);
}
