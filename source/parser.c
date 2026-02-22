#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "parser.h"
#include "operations_macros.h"

node_t* GetG(char** current_character_ptr)
{
    node_t* value = GetE(current_character_ptr);
    return value;
}

node_t* GetE(char** current_character_ptr)
{
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
    node_t* value = GetS(current_character_ptr);
    while (**current_character_ptr == '*' || **current_character_ptr == '/')
    {
        int op = **current_character_ptr;
        (*current_character_ptr)++;

        node_t* value_2 = GetS(current_character_ptr);

        if (op == '*')
            value = MUL_(value, value_2);
        else
            value = DIV_(value, value_2);
    }

    return value;
}

node_t* GetP(char** current_character_ptr)
{
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

    if (is_func_name(variable_str))
        return GetF(current_character_ptr, variable_str);

    return VAR_(variable_str);
}

node_t* GetS(char** current_character_ptr)
{
    node_t* value = GetP(current_character_ptr);
    while (**current_character_ptr == '^')
    {
        (*current_character_ptr)++;

        node_t* value_2 = GetP(current_character_ptr);

        value = POW_(value, value_2);
    }

    return value;
}

node_t* GetF(char** current_character_ptr, const char* string)
{
    for (size_t op_enum = FIRST_FUNC_NUM; op_enum <= LAST_FUNC_NUM; op_enum++)
    {
        if (!strncmp(string, operators_array[op_enum].design, operators_array[op_enum].strlen))
        {
            if (**current_character_ptr == '(')
            {
                (*current_character_ptr)++;
                node_t* value = GetE(current_character_ptr);
                (*current_character_ptr)++;

                switch(op_enum)
                {
                    case LN:  return LN_(value);
                    case COS: return COS_(value);
                    case SIN: return SIN_(value);
                    case EXP: return EXP_(value);
                    default: return nullptr;
                }
            }
        }
    }
    return nullptr;
}

bool is_func_name(const char* string)
{
    for (size_t i = FIRST_FUNC_NUM; i <= LAST_FUNC_NUM; i++)
    {
        if (!strncmp(string, operators_array[i].design, operators_array[i].strlen))
            return true;
    }
    return false;
}
