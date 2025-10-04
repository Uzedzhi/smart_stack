#ifndef CALC_H
#define CALC_H

#include "../stack/stack.h"

const size_t MAX_SIZE = 200;
enum calcInst_t {
    PUSH, POP, ADD, SUB, MUL, QROOT, DIV, OUT, HLT, DUMP, UNDEF
};
struct line_format;
struct calcErr_s{
    char message[MAX_SIZE];
    bool error_codes[MAX_SIZE];
    bool is_error;
};

void do_push(stack_t *stack, stack_var_t value);
void do_pop(stack_t *stack);
void do_add(stack_t *stack);
void do_sub(stack_t *stack);
void do_div(stack_t *stack);
void do_mul(stack_t *stack);
void do_out(stack_t *stack);
void do_hlt(stack_t *stack);
void do_command(stack_t *stack, line_format cur_line);
void execute_file(char * user_file_execute);
void nullify_anything_extra(char * buffer, size_t file_size, size_t actually_read);
void print_errors(calcErr_s error);
bool is_in_array(const char * command, size_t size);
bool is_file_exists(const char * file_name);
bool inline expects_arg(calcInst_t command);
char * get_buffer_from_file(char * user_file_compile);
calcErr_s check_for_error(int active_flags[], char * user_file_compile, char * user_file_execute, char * user_file_where);
calcErr_s check_and_execute_flags(int argc, char *argv[]);
calcErr_s compile_file(char * user_file_compile, char * user_file_where);
calcErr_s check_for_flags(int argc, char *argv[], int active_flags[], char ** user_file_compile, char ** user_file_execute, char ** user_file_where);
calcInst_t get_num_of_command(const char * command);
size_t get_file_size(FILE * fp);

#endif // CALC_H