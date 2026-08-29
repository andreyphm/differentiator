#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "output.h"
#include "macros.h"
#include "font.h"
#include "dump.h"
#include "parser.h"

#define OP_DESIGN           operators_array[NODE_OPERATION].design
#define OP_CODE             operators_array[NODE_OPERATION].code
#define OP_PRIORITY         operators_array[NODE_OPERATION].priority
#define PARENT_PRIORITY     operators_array[PARENT_OPERATION].priority

#define TEMP_DIRECTORY      "temp"
#define TEMP_LATEX_FILE     "temp/output.tex"
#define TEMP_PDF_FILE       "temp/output.pdf"

static const size_t FIRST_CONSOLE_CAPACITY        = 128;
static const size_t MAX_LATEX_EXPRESSION_LENGTH   = 80;

struct latex_output_state_t
{
    size_t line_length;
    size_t max_line_length;
};

static size_t latex_expression_length(const node_t* node, const variable_t* variables);
static size_t latex_first_line_length(const node_t* node, const variable_t* variables,
                                      const node_t* parent, bool line_break_allowed);
static const char* latex_operator_design(operator_code operation);
static void latex_binary_separator(const node_t* parent, const node_t* right,
                                   const variable_t* variables, bool line_break_allowed,
                                   FILE* output_file, latex_output_state_t* state);
static void tree_to_latex(const node_t* node, FILE* output_file, const variable_t* variables);
static void latex_output(const node_t* node, FILE* output_file, const variable_t* variables,
                         const node_t* parent, bool line_break_allowed,
                         latex_output_state_t* state);
static bool expression_to_tree(program_status_data program_status, variable_t** variables_ptr,
                               FILE* input_file, node_t** node_ptr);
static void from_file_to_tree(variable_t** variables, FILE* input_file,
                              char** buffer_ptr, char** original_ptr);
static bool from_console_to_tree(variable_t** variables, char** buffer_ptr, char** original_ptr);
static bool create_temp_directory(void);
static bool run_pdf_latex(void);
static bool copy_file(const char* source_file_name, const char* destination_file_name);
static bool console_parentheses_required(const node_t* node, const node_t* parent, bool is_right_child);
static void console_output(const node_t* node, const variable_t* variables, const node_t* parent,
                           bool is_right_child);

static void tree_to_latex(const node_t* node, FILE* output_file, const variable_t* variables)
{
    assert(node);
    assert(output_file);
    assert(variables);

    latex_output_state_t state = {0, MAX_LATEX_EXPRESSION_LENGTH};

    fprintf(output_file, "\\documentclass{article}\n");
    fprintf(output_file, "\\usepackage{amsmath}\n");
    fprintf(output_file, "\\begin{document}\n");
    fprintf(output_file, "\\[\n\\begin{aligned}\n& ");
    latex_output(node, output_file, variables, nullptr, true, &state);
    fprintf(output_file, "\n\\end{aligned}\n\\]\n");
    fprintf(output_file, "\\end{document}\n");

    fflush(output_file);
}

static void latex_output(const node_t* node, FILE* output_file, const variable_t* variables,
                         const node_t* parent, bool line_break_allowed,
                         latex_output_state_t* state)
{
    assert(output_file);
    assert(variables);
    assert(state);

    if (!node) return;
    assert(node->value);

    switch (NODE_TYPE)
    {
        case NUM:
        {
            if (NODE_NUMBER > 0 || is_close_to_zero(NODE_NUMBER))
                fprintf(output_file, "%lg", NODE_NUMBER);
            else
                fprintf(output_file, "\\left(%lg\\right)", NODE_NUMBER);

            state->line_length += latex_expression_length(node, variables);
            break;
        }

        case VAR:
            fprintf(output_file, "%s", variables[NODE_VAR_NUMBER].name);
            state->line_length += variables[NODE_VAR_NUMBER].length;
            break;

        case OP:
        {
            switch(OP_CODE)
            {
                case ADD:
                case SUB:
                case MUL:
                {
                    bool parentheses = parent && (OP_PRIORITY > PARENT_PRIORITY);
                    if (parentheses)
                    {
                        fprintf(output_file, "\\bigl(");
                        state->line_length++;
                    }

                    latex_output(node->left, output_file, variables, node,
                                 line_break_allowed, state);
                    latex_binary_separator(node, node->right, variables,
                                           line_break_allowed, output_file, state);
                    latex_output(node->right, output_file, variables, node,
                                 line_break_allowed, state);

                    if (parentheses)
                    {
                        fprintf(output_file, "\\bigr)");
                        state->line_length++;
                    }
                    break;
                }

                case DIV:
                {
                    bool parentheses = parent && (OP_PRIORITY > PARENT_PRIORITY);
                    latex_output_state_t nested_state = {0, state->max_line_length};
                    if (parentheses) fprintf(output_file, "\\left(");
                    fprintf(output_file, "\\frac{");
                    latex_output(node->left, output_file, variables, node, false, &nested_state);
                    fprintf(output_file, "}{");
                    nested_state.line_length = 0;
                    latex_output(node->right, output_file, variables, node, false, &nested_state);
                    fprintf(output_file, "}");
                    if (parentheses) fprintf(output_file, "\\right)");
                    state->line_length += latex_expression_length(node, variables) +
                                          (parentheses ? 2 : 0);
                    break;
                }

                case POW:
                {
                    latex_output_state_t nested_state = {0, state->max_line_length};
                    latex_output(node->left, output_file, variables, node, false, &nested_state);
                    fprintf(output_file, "^{");
                    nested_state.line_length = 0;
                    latex_output(node->right, output_file, variables, node, false, &nested_state);
                    fprintf(output_file, "}");
                    state->line_length += latex_expression_length(node, variables);
                    break;
                }

                case LN:
                case COS:
                case SIN:
                case EXP:
                {
                    latex_output_state_t nested_state = {0, state->max_line_length};
                    fprintf(output_file, "\\%s\\left(", OP_DESIGN);
                    latex_output(node->left, output_file, variables, node, false, &nested_state);
                    fprintf(output_file, "\\right)");
                    state->line_length += latex_expression_length(node, variables);
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case SPEC:
        default:
            break;
    }
}

static bool create_temp_directory(void)
{
    if (mkdir(TEMP_DIRECTORY, 0755) == 0 || errno == EEXIST)
        return true;

    printf(MAKE_BOLD_RED("Can't create the temporary directory.\n"));
    return false;
}

static bool run_pdf_latex(void)
{
    pid_t process_id = fork();
    if (process_id == -1)
    {
        printf(MAKE_BOLD_RED("Failed to start pdflatex.\n"));
        return false;
    }

    if (process_id == 0)
    {
        int null_output = open("/dev/null", O_WRONLY);
        if (null_output != -1)
        {
            dup2(null_output, STDOUT_FILENO);
            dup2(null_output, STDERR_FILENO);
            close(null_output);
        }
        execlp("pdflatex", "pdflatex", "-interaction=nonstopmode", "-halt-on-error",
               "-output-directory=temp", TEMP_LATEX_FILE, (char*) nullptr);
        _exit(127);
    }

    int process_status = 0;
    if (waitpid(process_id, &process_status, 0) == -1)
        return false;
    return WIFEXITED(process_status) && WEXITSTATUS(process_status) == 0;
}

static bool copy_file(const char* source_file_name, const char* destination_file_name)
{
    assert(source_file_name);
    assert(destination_file_name);

    FILE* source_file = fopen(source_file_name, "rb");
    if (!source_file) return false;

    FILE* destination_file = fopen(destination_file_name, "wb");
    if (!destination_file)
    {
        fclose(source_file);
        return false;
    }

    char buffer[4096] = {};
    bool copy_succeeded = true;
    for (size_t bytes_read = fread(buffer, 1, sizeof(buffer), source_file);
         bytes_read > 0;
         bytes_read = fread(buffer, 1, sizeof(buffer), source_file))
    {
        if (fwrite(buffer, 1, bytes_read, destination_file) != bytes_read)
        {
            copy_succeeded = false;
            break;
        }
    }

    fclose(source_file);
    fclose(destination_file);
    if (!copy_succeeded) remove(destination_file_name);

    return copy_succeeded;
}

bool tree_to_pdf_file(const node_t* node, const variable_t* variables, const char* output_file_name)
{
    assert(node);
    assert(variables);
    assert(output_file_name);

    if (!create_temp_directory())
        return false;

    FILE* latex_file = fopen(TEMP_LATEX_FILE, "w");
    if (!latex_file)
    {
        printf(MAKE_BOLD_RED("Can't create %s.\n"), TEMP_LATEX_FILE);
        return false;
    }

    tree_to_latex(node, latex_file, variables);
    fclose(latex_file);
    remove(TEMP_PDF_FILE);

    if (!run_pdf_latex())
    {
        printf(MAKE_BOLD_RED("Failed to compile the PDF. Check temp/output.log.\n"));
        return false;
    }

    if (!copy_file(TEMP_PDF_FILE, output_file_name))
    {
        printf(MAKE_BOLD_RED("Can't save the PDF to: %s\n"), output_file_name);
        return false;
    }

    printf(MAKE_BOLD_GREEN("The result is saved to %s.\n"), output_file_name);
    return true;
}

bool make_dif_tree(program_status_data program_status, variable_t** variables_ptr,
                   FILE* input_file, node_t** node_ptr)
{
    assert(program_status == FROM_FILE_TO_TREE || program_status == FROM_CONSOLE_TO_TREE);
    assert(variables_ptr);
    assert(node_ptr);
    assert(program_status != FROM_FILE_TO_TREE || input_file);

    if (!expression_to_tree(program_status, variables_ptr, input_file, node_ptr))
        return false;

    node_t* differentiated_node = differentiate(*node_ptr);
    for (bool simplifications = true; simplifications;)
    {
        simplifications = false;
        differentiated_node = simplify_tree(differentiated_node, &simplifications);
    }

    destroy_node(*node_ptr);
    *node_ptr = differentiated_node;
    tree_dump(*node_ptr, *variables_ptr);

    // printf(MAKE_BOLD_GREEN("Successfully differentiated.\n"));
    return true;
}

static bool expression_to_tree(program_status_data program_status, variable_t** variables_ptr,
                               FILE* input_file, node_t** node_ptr)
{
    assert(program_status == FROM_FILE_TO_TREE || program_status == FROM_CONSOLE_TO_TREE);
    assert(variables_ptr);
    assert(node_ptr);
    assert(program_status != FROM_FILE_TO_TREE || input_file);

    variable_t* variables = nullptr;
    char* buffer = nullptr;
    char* original_ptr = nullptr;
    if (*variables_ptr) variables_destroy(variables_ptr);
    if (*node_ptr)
    {
        destroy_node(*node_ptr);
        *node_ptr = nullptr;
    }

    if (program_status == FROM_FILE_TO_TREE)
        from_file_to_tree(&variables, input_file, &buffer, &original_ptr);
    else if (!from_console_to_tree(&variables, &buffer, &original_ptr))
        return false;
                    
    list_t list = {nullptr, nullptr, nullptr};
    error_code error = tokenization(buffer, variables, &list);
    if (error)
    {
        free(original_ptr);
        list_destroy(&list);
        *variables_ptr = variables;
        printf(MAKE_BOLD_RED("The expression contains an invalid token.\n"));
        return false;
    }

    token_t* current = list.head;
    *node_ptr = GetG(&current);

    free(original_ptr);
    list_destroy(&list);
    *variables_ptr = variables;
    if (!*node_ptr)
    {
        printf(MAKE_BOLD_RED("The expression has invalid syntax.\n"));
        return false;
    }

    return true;
}

static void from_file_to_tree(variable_t** variables, FILE* input_file,
                              char** buffer_ptr, char** original_ptr)
{
    assert(variables);
    assert(input_file);
    assert(buffer_ptr);
    assert(original_ptr);

    *variables = (variable_t*) calloc(MAX_NUMBER_OF_VARS, sizeof(variable_t));
    
    rewind(input_file);
    *buffer_ptr = read_file_to_buffer(input_file);
    size_t buffer_len = strlen(*buffer_ptr);
    (*buffer_ptr)[buffer_len] = '$';
    *original_ptr = *buffer_ptr;
}

static bool from_console_to_tree(variable_t** variables, char** buffer_ptr, char** original_ptr)
{
    assert(variables);
    assert(buffer_ptr);
    assert(original_ptr);

    *variables = (variable_t*) calloc(MAX_NUMBER_OF_VARS, sizeof(variable_t));

    size_t capacity = FIRST_CONSOLE_CAPACITY;
    *buffer_ptr = (char*) calloc(capacity, sizeof(char));
    if (!*buffer_ptr || !fgets(*buffer_ptr, (int) capacity, stdin))
    {
        free(*variables);
        free(*buffer_ptr);
        *variables = nullptr;
        *buffer_ptr = nullptr;
        printf(MAKE_BOLD_RED("Failed to read from console\n"));
        return false;
    }

    size_t length = strlen(*buffer_ptr);
    while (length > 0 && (*buffer_ptr)[length - 1] != '\n' && !feof(stdin))
    {
        capacity *= 2;
        char* resized_buffer = (char*) realloc(*buffer_ptr, capacity);
        if (!resized_buffer)
        {
            free(*variables);
            free(*buffer_ptr);
            *variables = nullptr;
            *buffer_ptr = nullptr;
            return false;
        }
        *buffer_ptr = resized_buffer;
        if (!fgets(*buffer_ptr + length, (int) (capacity - length), stdin))
            break;
        length = strlen(*buffer_ptr);
    }

    if (length > 0 && (*buffer_ptr)[length - 1] == '\n')
        (*buffer_ptr)[length - 1] = '$';
    else
    {
        char* resized_buffer = (char*) realloc(*buffer_ptr, length + 2);
        if (!resized_buffer)
        {
            free(*variables);
            free(*buffer_ptr);
            *variables = nullptr;
            *buffer_ptr = nullptr;
            return false;
        }
        *buffer_ptr = resized_buffer;
        (*buffer_ptr)[length] = '$';
        (*buffer_ptr)[length + 1] = '\0';
    }

    *original_ptr = *buffer_ptr;
    return true;
}

static size_t latex_expression_length(const node_t* node, const variable_t* variables)
{
    if (!node) return 0;

    assert(node->value);
    assert(variables);

    switch (node->value->type)
    {
        case NUM:
        {
            char number[64] = {};
            int length = snprintf(number, sizeof(number), "%lg", node->value->data_t.number);
            size_t result = length > 0 ? (size_t) length : 0;
            if (node->value->data_t.number < 0 && !is_close_to_zero(node->value->data_t.number))
                result += 2;
            return result;
        }

        case VAR:
            return variables[node->value->data_t.var_number].length;

        case OP:
        {
            operator_code operation = node->value->data_t.op;
            size_t left_length = latex_expression_length(node->left, variables);

            if (operators_array[operation].is_one_arg)
                return operators_array[operation].strlen + left_length + 2;

            size_t right_length = latex_expression_length(node->right, variables);
            if (operation == DIV)
                return (left_length > right_length ? left_length : right_length) + 2;

            if (operation == POW)
                return left_length + right_length + 1;

            return left_length + right_length + 3;
        }

        case SPEC:
        default:
            return 0;
    }
}

static size_t latex_first_line_length(const node_t* node, const variable_t* variables,
                                      const node_t* parent, bool line_break_allowed)
{
    assert(node);
    assert(node->value);
    assert(variables);
    if (parent) assert(parent->value);

    if (!line_break_allowed || node->value->type != OP)
        return latex_expression_length(node, variables);

    operator_code operation = node->value->data_t.op;
    if (operation != ADD && operation != SUB && operation != MUL)
        return latex_expression_length(node, variables);

    bool parentheses = parent && parent->value->type == OP &&
                       operators_array[operation].priority >
                       operators_array[parent->value->data_t.op].priority;
    size_t result = latex_first_line_length(node->left, variables, node, true);
    return result + (parentheses ? 1 : 0);
}

static const char* latex_operator_design(operator_code operation)
{
    switch (operation)
    {
        case ADD: return "+";
        case SUB: return "-";
        case MUL: return "\\cdot";
        case DIV:
        case POW:
        case LN:
        case COS:
        case SIN:
        case EXP:
        default:  return "";
    }
}

static void latex_binary_separator(const node_t* parent, const node_t* right,
                                   const variable_t* variables, bool line_break_allowed,
                                   FILE* output_file, latex_output_state_t* state)
{
    assert(parent);
    assert(parent->value);
    assert(parent->value->type == OP);
    assert(right);
    assert(variables);
    assert(output_file);
    assert(state);

    operator_code operation = parent->value->data_t.op;
    const char* design = latex_operator_design(operation);
    size_t right_length = latex_first_line_length(right, variables, parent,
                                                  line_break_allowed);
    if (line_break_allowed &&
        state->line_length + right_length + 3 > state->max_line_length)
    {
        fprintf(output_file, " %s{} \\\\\n& {}%s ", design, design);
        state->line_length = 2;
        return;
    }

    fprintf(output_file, " %s ", design);
    state->line_length += 3;
}

static bool console_parentheses_required(const node_t* node, const node_t* parent, bool is_right_child)
{
    assert(node);
    assert(node->value);
    if (parent) assert(parent->value);

    if (!parent || node->value->type != OP || parent->value->type != OP)
        return false;

    priority_t node_priority = operators_array[node->value->data_t.op].priority;
    priority_t parent_priority = operators_array[parent->value->data_t.op].priority;
    if (node_priority > parent_priority) return true;
    if (node_priority < parent_priority) return false;

    operator_code parent_operation = parent->value->data_t.op;
    operator_code node_operation = node->value->data_t.op;
    return is_right_child && (parent_operation == SUB || parent_operation == DIV ||
                              parent_operation == POW ||
                              (parent_operation == MUL && node_operation == DIV));
}

static void console_output(const node_t* node, const variable_t* variables, const node_t* parent,
                           bool is_right_child)
{
    assert(variables);

    if (!node) return;
    assert(node->value);

    switch (node->value->type)
    {
        case NUM:
            printf("%lg", node->value->data_t.number);
            break;

        case VAR:
            printf("%s", variables[node->value->data_t.var_number].name);
            break;

        case OP:
        {
            operator_code operation = node->value->data_t.op;
            bool parentheses = console_parentheses_required(node, parent, is_right_child);
            if (parentheses) printf("(");

            if (operators_array[operation].is_one_arg)
            {
                printf("%s(", operators_array[operation].design);
                console_output(node->left, variables, node, false);
                printf(")");
            }
            else
            {
                console_output(node->left, variables, node, false);
                printf(" %s ", operators_array[operation].design);
                console_output(node->right, variables, node, true);
            }

            if (parentheses) printf(")");
            break;
        }

        case SPEC:
        default:
            break;
    }
}

void tree_to_console(const node_t* node, const variable_t* variables)
{
    assert(node);
    assert(variables);

    printf(MAKE_BOLD("Result:\n"));
    console_output(node, variables, nullptr, false);
    printf("\n");
}

void program_complete(variable_t** variables_ptr, node_t** node_ptr, FILE* input_file)
{
    assert(variables_ptr);
    assert(node_ptr);

    if (*variables_ptr) variables_destroy(variables_ptr);
    destroy_node(*node_ptr);
    if (input_file) fclose(input_file);
}
