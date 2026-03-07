#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "tokenization.h"
#include "font.h"

error_code tokenization(const char* buffer, variable_t* variables, list_t* const list)
{
    while (*buffer != '$')
    {
        skip_spaces(&buffer);

        if (try_digit(&buffer, list)    ||
            try_char_op(&buffer, list)  ||
            try_function(&buffer, list) ||
            try_bracket(&buffer, list)  ||
            try_variable(&buffer, list, variables))
        {
            continue;
        }

        // else
        // {
        //     printf(MAKE_BOLD_RED("SYNTAX_ERROR\n"));
        //     return SYNTAX_ERROR;
        // }
    }

    list_push_back(SPEC, (token_union){.spec_symbol = '$'}, list);

    token_t* new_head = list->head->next;
    free(list->head);
    list->head = new_head;

    list_dump(list, "list_dump.txt", "list_dump.png", variables);

    return NO_ERROR;
}

void skip_spaces(const char** string)
{
    while (isspace(**string))
        (*string)++;
}

bool try_digit(const char** buffer, list_t* const list)
{
    bool dot_already = false;
    double value = 0;

    if (isdigit(**buffer))
    {
        sscanf(*buffer, "%lf", &value);
        list_push_back(NUM, (token_union){.number = value}, list);

        while (isdigit(**buffer))
        {
            (*buffer)++;
            if (**buffer == '.' && !dot_already)
            {
                (*buffer)++;
                dot_already = true;
            }
        }

        return true;
    }

    return false;
}

bool try_char_op(const char** buffer, list_t* const list)
{
    for (size_t i = 0; i <= LAST_CHAR_OP_NUM; i++)
    {
        if (!strncmp(operators_array[i].design, *buffer, 1))
        {
            list_push_back(OP, (token_union){.op = operators_array[i].code}, list);
            (*buffer)++;

            return true;
        }
    }

    return false;
}

bool try_function(const char** buffer, list_t* const list)
{
    const char* start_of_buffer = *buffer;

    if (isalpha(**buffer))
    {
        (*buffer)++;

        while (isalpha(**buffer))
            (*buffer)++;

        for (size_t i = FIRST_FUNC_NUM; i <= LAST_FUNC_NUM; i++)
        {
            if (!strncmp(operators_array[i].design, start_of_buffer, (size_t) (*buffer - start_of_buffer)))
            {
                list_push_back(OP, (token_union){.op = operators_array[i].code}, list);
                return true;
            }
        }

        *buffer = start_of_buffer;
        return false;
    }

    return false;
}

bool try_bracket(const char** buffer, list_t* const list)
{
    if (**buffer == '(' || **buffer == ')')
    {
        list_push_back(SPEC, (token_union){.spec_symbol = **buffer}, list);
        (*buffer)++;
        return true;
    }

    return false;
}

bool try_variable(const char** buffer, list_t* const list, variable_t* variables)
{
    static int last_variable_num = 0;
    static bool no_variables = true;

    const char* start_of_buffer = *buffer;

    if (!isalpha(**buffer))
        return false;

    while (isalpha(**buffer))
        (*buffer)++;

    size_t name_length = (size_t) (*buffer - start_of_buffer);

    if (!no_variables)
    {
        for (int i = 0; i <= last_variable_num; i++)
        {
            if (variables[i].length == name_length && !strncmp(variables[i].name, start_of_buffer, name_length))
            {
                list_push_back(VAR, (token_union){.var_number = i}, list);
                return true;
            }
        }
    }

    if (no_variables)
    {
        list_push_back(VAR, (token_union){.var_number = 0}, list);
        no_variables = false;
    }

    else
    {
        last_variable_num++;
        list_push_back(VAR, (token_union){.var_number = last_variable_num}, list);
    }

    char* name_copy = strndup(start_of_buffer, name_length);

    variables[last_variable_num].name = name_copy;
    variables[last_variable_num].number = last_variable_num;
    variables[last_variable_num].length = name_length;

    return true;
}

token_t* list_push_back(const type_data type, token_union data, list_t* const list)
{
    assert(list);

    token_t* const token = create_token(type, data, list);

    token->prev = list->tail;
    list->tail->next = token;
    list->tail = token;

    return token;
}

token_t* create_token(const type_data type, token_union data, list_t* const list)
{
    assert(list);

    token_t* const token = (token_t*) calloc(1, sizeof(token_t));

    token->type = type;

    switch(type)
    {
        case OP:
            token->data_t.op = data.op;
            break;

        case VAR:
            token->data_t.var_number = data.var_number;
            break;

        case NUM:
            token->data_t.number = data.number;
            break;

        case SPEC:
            token->data_t.spec_symbol = data.spec_symbol;

        default:
            break;
    }

    token->prev = nullptr;
    token->next = nullptr;

    return token;
}

void list_dump(list_t* const list, const char* const txt_file_name, const char* const png_file_name, const variable_t* const variables)
{
    assert(list);
    assert(txt_file_name);
    assert(png_file_name);

    FILE* txt_file = fopen(txt_file_name, "w");
    fprintf(txt_file, "digraph structs\n{\nrankdir = LR;\ngraph[bgcolor=\"#e0e0e9ff\"];\n");

    int node_number = 1;
    for (list->current = list->head; list->current != list->tail; node_number++)
    {
        fprintf(txt_file, "node_%d [style=filled, penwidth = 3, fillcolor=\"#b7e5f3ff\","
               "color = \"#3f6969ff\", shape=record, label= \" ", node_number);
        switch(list->current->type)
        {
            case OP:
                fprintf(txt_file, "TYPE = OP | OP_CODE = %s | ", operators_array[list->current->data_t.op].name);
                break;
            case VAR:
                fprintf(txt_file, "TYPE = VAR | VAR_NUM = %d (%s) | ", list->current->data_t.var_number, variables[list->current->data_t.var_number].name);
                break;
            case NUM:
                fprintf(txt_file, "TYPE = NUM | VALUE = %lg | ", list->current->data_t.number);
                break;
            case SPEC:
                fprintf(txt_file, "TYPE = SPEC | VALUE = %c | ", list->current->data_t.spec_symbol);
            default:
                break;
        }
        fprintf(txt_file, "ADDRESS = %p |\n", list->current);
        fprintf(txt_file, "{next = %p | prev = %p}\" ];\n", list->current->next, list->current->prev);

        list->current = list->current->next;
    }

    fprintf(txt_file, "node_%d [style=filled, penwidth = 3, fillcolor=\"#b7e5f3ff\","
               "color = \"#3f6969ff\", shape=record, label= \" ", node_number);
    switch(list->current->type)
    {
        case OP:
            fprintf(txt_file, "TYPE = OP | OP_CODE = %s | ", operators_array[list->current->data_t.op].name);
            break;
        case VAR:
            fprintf(txt_file, "TYPE = VAR | VAR_NUM = %d (%s) | ", list->current->data_t.var_number, variables[list->current->data_t.var_number].name);
            break;
        case NUM:
            fprintf(txt_file, "TYPE = NUM | VALUE = %lg | ", list->current->data_t.number);
            break;
        case SPEC:
            fprintf(txt_file, "TYPE = SPEC | VALUE = %c | ", list->current->data_t.spec_symbol);
        default:
            break;
    }
    fprintf(txt_file, "ADDRESS = %p |\n", list->current);
    fprintf(txt_file, "{next = %p | prev = %p}\" ];\n", list->current->next, list->current->prev);

    int list_capacity = node_number;
    fprintf(txt_file, "{\nedge[color = \"#149b5aff\", weight = 1000];\n");
    for (node_number = 1; node_number < list_capacity; node_number++)
    {
        fprintf(txt_file, "node_%d -> node_%d\n", node_number, node_number + 1);
    }

    fprintf(txt_file, "}\n");
    fprintf(txt_file, "}");
    fclose(txt_file);

    char command[1000];
    sprintf(command, "dot %s -T png -o %s", txt_file_name, png_file_name);

    system(command);
}

void list_destroy(list_t* list)
{
    token_t* current = list->head;

    while (current)
    {
        token_t* next = current->next;
        free(current);
        current = next;
    }
}
