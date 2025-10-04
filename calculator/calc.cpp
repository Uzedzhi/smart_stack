#include "../stack/stack.h"
#include "calc.h"
#include <math.h>

static string all_flags = "ceo";
static string all_commands[] = {"PUSH", "POP", "ADD", "SUB", "MUL", "QROOT", "DIV", "OUT", "HLT", "DUMP"};
static string error_text[] = {"file you try to open does not exist", "flag is invalid", "command is incorrect"};
const static size_t num_of_errors = sizeof(error_text) / sizeof(string);
const static size_t num_of_flags = strlen(all_flags);
const static size_t num_of_commands = sizeof(all_commands) / sizeof(string);

enum flags {
    COMPILE, EXECUTE, WHERE
};
enum calcErr_t {
    ERR_FILE_DOES_NOT_EXIST, ERR_FLAG_INCORRECT, ERR_INCORRECT_COMMAND
};

struct line_format{
    calcInst_t num_of_command;
    stack_var_t value;
};
static calcErr_s error = {};

void do_push(stack_t *stack, stack_var_t value) {
    stackPush(stack, value);
}
void do_pop(stack_t *stack) {
    stack_var_t n = 0;
    stackPop(stack, &n);
}
void do_add(stack_t *stack) {
    stack_var_t a = 0, b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, a + b);
}
void do_sub(stack_t *stack) {
    stack_var_t a = 0, b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, a - b);
}
void do_div(stack_t *stack) {
    stack_var_t a = 0, b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, b / a);
}
void do_dump(stack_t *stack) {
    stackDump(stack, 0, "calc.cpp", "do_dump", 0);
}
void do_mul(stack_t *stack) {
    stack_var_t a = 0, b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, a * b);
}
void do_out(stack_t *stack) {
    stack_var_t a = 0;
    stackPop(stack, &a);
    stackPush(stack, a);
    printf("-----------------%lf--------------------\n", a);
}

void do_qroot(stack_t *stack) {
    stack_var_t a = 0;
    stack_var_t b = 0;
    stack_var_t c = 0;
    stackPop(stack, &c);
    stackPop(stack, &b);
    stackPop(stack, &a);
    double D = b * b - 4 * a * c;
    if (D > 0) {
        stackPush(stack, (-b + sqrt(D)) / (2 * a));
        stackPush(stack, (-b - sqrt(D)) / (2 * a));
    } else if (D == 0) {
        stackPush(stack, -b / (2 * a));
    } else {
        stackPush(stack, 0);
    }
}
void do_hlt(stack_t *stack) {
    stackDtor(stack);
}

#define add_error(error_code, str, ...) {\
    error.error_codes[error_code] = 1;\
    error.is_error = 1;\
    sprintf(error.message, "%s" str, error.message, ##__VA_ARGS__);\
}

bool is_file_exists(const char * file_name) {
    sassert(file_name, ERR_PTR_NULL);

    FILE * fp = fopen(file_name, "r");
    if (fp == NULL)
        add_error(ERR_FILE_DOES_NOT_EXIST, "file %s does not exist", file_name);
        return 1;
    fclose(fp);
    return 0;
}


calcErr_s check_for_error(int active_flags[], char * user_file_compile, char * user_file_execute, char * user_file_where) {
    sassert(active_flags, ERR_PTR_NULL);

    if (active_flags[COMPILE] == 1 && user_file_compile == NULL)
        add_error(ERR_FLAG_INCORRECT, "there isnt compile file specified");
    if (active_flags[EXECUTE] == 1 && user_file_execute == NULL)
        add_error(ERR_FLAG_INCORRECT, "there isnt execute file specified");
    if (active_flags[WHERE] == 1 && user_file_where == NULL)
        add_error(ERR_FLAG_INCORRECT, "there isnt destination file specified");

    bool is_any_flag = false;
    for (size_t i = 0; i < num_of_flags; i++) {
        if (active_flags[i] > 1)
            add_error(ERR_FLAG_INCORRECT, "repeating flags are not allowed");
        if (active_flags[i] == 1)
            is_any_flag = true;
    }

    if (is_any_flag == 0)
        add_error(ERR_FLAG_INCORRECT, "there isnt a single flag specified");
    return error;
}

calcErr_s check_for_flags(int argc, char *argv[], int active_flags[], char ** user_file_compile, char ** user_file_execute, char ** user_file_where) {
    sassert(argv, ERR_PTR_NULL);
    sassert(active_flags, ERR_PTR_NULL);

    int ind_cur_flag = 0;
    bool is_first_time = true;
    while (++ind_cur_flag < argc) {
        char * cur_flag = argv[ind_cur_flag];
        if (cur_flag[0] == '-') {
            for (size_t i = 1; cur_flag[i] != '\0'; i++) {
                for (size_t j = 0; j < num_of_flags; j++) {
                    if (cur_flag[i] == all_flags[j]) {
                        active_flags[j]++;
                    }
                }
            }
        } else {
            if (active_flags[COMPILE]      == 1 && *user_file_compile == NULL)
                *user_file_compile = cur_flag;
            else if (active_flags[WHERE]   == 1 && is_first_time) {
                *user_file_where = cur_flag;
                is_first_time = false;
            }
            else if (active_flags[EXECUTE] == 1 && *user_file_execute == NULL)
                *user_file_execute = cur_flag;
        }
    }
    if (active_flags[COMPILE] == 1 && active_flags[EXECUTE] == 1 && *user_file_compile != NULL)
        *user_file_execute = *user_file_where;
    check_for_error(active_flags, *user_file_compile, *user_file_execute, *user_file_where);
    return error;

}

calcInst_t get_num_of_command(const char * command) {
    sassert(command, ERR_PTR_NULL);

    for (size_t i = 0; i < sizeof(all_commands) / sizeof(string); i++) {
        if (strcmp(command, all_commands[i]) == 0) {
            return (calcInst_t) i;
        }
    }
    return UNDEF;
}

size_t get_file_size(FILE * fp) {
    sassert(fp, ERR_PTR_NULL);

    fseek(fp, 0, SEEK_END);
    size_t file_size = (size_t) ftell(fp);
    rewind(fp);

    return file_size;
}

bool inline expects_arg(calcInst_t command) {
    if (command == PUSH) return true;
    return false;
}

bool is_in_array(const char * command, size_t size) {
    sassert(command, ERR_PTR_NULL);
    sassert(all_commands, ERR_PTR_NULL);

    for (size_t i = 0; i < size; i++) {
        if (strcmp(command, all_commands[i]) == 0)
            return true;
    }

    return false;
}

void nullify_anything_extra(char * buffer, size_t file_size, size_t actually_read) {
    sassert(buffer != NULL, ERR_PTR_NULL);

    while (actually_read < file_size) {
        buffer[++actually_read] = '\0';
    }
}

char * get_buffer_from_file(char * user_file_compile) {
    sassert(user_file_compile, ERR_PTR_NULL);

    FILE * fp = fopen(user_file_compile, "r");
    sassert(fp, ERR_PTR_NULL);

    size_t file_size = get_file_size(fp);
    char * compile_buffer = (char *) calloc(file_size + 1, sizeof(char));
    sassert(compile_buffer, ERR_PTR_NULL);

    size_t actually_read = fread(compile_buffer, sizeof(char), file_size, fp);
    nullify_anything_extra(compile_buffer, file_size, actually_read);

    return compile_buffer;
}

calcErr_s compile_file(char * user_file_compile, char * user_file_where) {
    sassert(user_file_compile, ERR_PTR_NULL);

    FILE * compiled_fp = fopen(user_file_where,  "wb");
    sassert(compiled_fp, ERR_PTR_NULL);

    char * compile_buffer = get_buffer_from_file(user_file_compile);
    char * token_buffer = strtok(compile_buffer, " \t\n\r");
    line_format cur_line = {};

    while (token_buffer != NULL) {
        cur_line.num_of_command = get_num_of_command(token_buffer);

        if (cur_line.num_of_command == UNDEF) {
            add_error(ERR_INCORRECT_COMMAND, "command is undefined");
            break;
        }
        if (expects_arg(cur_line.num_of_command)) {
            token_buffer = strtok(NULL, " \t\n\r");
            cur_line.value = atof(token_buffer);
        } else 
            cur_line.value = 0;
        
        fwrite(&cur_line, sizeof(cur_line), 1, compiled_fp);
        token_buffer = strtok(NULL, " \t\n\r");
    }
    free(compile_buffer);
    fclose(compiled_fp);
    return error;
}

void do_command(stack_t *stack, line_format cur_line) {
    sassert(stack, ERR_PTR_NULL);
    switch(cur_line.num_of_command) {
        case PUSH:
            do_push(stack, cur_line.value);
            break;
        case POP:
            do_pop(stack);
            break;
        case ADD:
            do_add(stack);
            break;
        case SUB:
            do_sub(stack);
            break;
        case DIV:
            do_div(stack);
            break;
        case MUL:
            do_mul(stack);
            break;
        case QROOT:
            do_qroot(stack);
            break;
        case OUT:
            do_out(stack);
            break;
        case DUMP:
            do_dump(stack);
            break;
        case UNDEF:
            push_error(ERR_INCORRECT_COMMAND);
        default:
            push_error(ERR_INCORRECT_COMMAND);
    }
}

void execute_file(char * user_file_execute) {
    sassert(user_file_execute, ERR_PTR_NULL);

    FILE * execute_file = fopen(user_file_execute, "r");
    sassert(execute_file, ERR_PTR_NULL);
    
    init_stack(stack, 50.0);
    line_format cur_line = {};

    while (fread(&cur_line, sizeof(line_format), 1, execute_file) != 0) {
        if (cur_line.num_of_command < 0 || cur_line.num_of_command >= num_of_commands) {
            add_error(ERR_INCORRECT_COMMAND, "command <%d> is incorrect", cur_line.num_of_command);
            break;
        }
        if (cur_line.num_of_command == HLT) {
            stackDtor(stack);
            break;
        }
        do_command(stack, cur_line);
    }
}

void print_errors(calcErr_s errors) {
    if (errors.is_error == 1) {
        printf("All errors:\n");
        printf("<%s>\n", errors.message);
    }
}

calcErr_s check_and_execute_flags(int argc, char *argv[]) {
    sassert(argv, ERR_PTR_NULL);

    int active_flags[num_of_flags] = {};
    char *user_file_compile = NULL;
    char *user_file_execute = NULL;
    char *user_file_where = "out.txt";
    check_for_flags(argc, argv, active_flags, &user_file_compile, &user_file_execute, &user_file_where);
    if (error.is_error == 1)
        return error;
    
    if (active_flags[COMPILE] == 1) {
        if (!is_file_exists(user_file_compile))
            return error;
        compile_file(user_file_compile, user_file_where);
    }

    if (active_flags[EXECUTE] == 1) {
        if (!is_file_exists(user_file_execute))
            return error;
        execute_file(user_file_execute);
    }

    return error;
}