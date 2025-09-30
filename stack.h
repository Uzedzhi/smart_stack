#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define RED       "\e[1;91m"
#define WHITE     "\e[0;37m"
#define YELLOW    "\e[1;93m"
#define BYELLOW   "\e[0;33m"
#define BLACK     "\e[0;30m"
#define MAGENTA   "\e[0;35m"

#define BEGIN     do {
#define END       } while(0);

#define LEVEL_OF_CHECK 3

#if LEVEL_OF_CHECK > 0
#define sassert(condition, ERROR_CODE, ...) {                                                   \
                        if (!(condition)){                                                      \
                            fprintf(stderr, RED "-->ERROR:" WHITE " in file \"%s\"\n"           \
                                                "             line %d\n"                        \
                                                "EXPLANATION: %s\n",                            \
                                            __FILE_NAME__, __LINE__, err_strings[ERROR_CODE]);  \
                            char str[MAX_SIZE] = {};                                            \
                            sprintf(str, " " __VA_ARGS__);                                      \
                            if (strlen(str) != 1)                                               \
                                fprintf(stderr, "MORE INFO: " __VA_ARGS__ );                    \
                            else                                                                \
                                fprintf(stderr, "ADDITIONAL INFO CANT BE ACCESSED\n"            \
                                          BLACK "(RAZRABI POLENILISI NAPISAT)" WHITE);          \
                            putc('\n', stderr);                                                 \
                            exit(ERROR_CODE);                                                   \
                    }}
#else 
#define sassert(condition, ...) ((void)0)
#endif // sassert

#define push_error(ERROR_CODE, ...) \
    sassert(0, ERROR_CODE, ##__VA_ARGS__);

// main structs

enum stackErr_t {
    ERR_PTR_NULL = 1, ERR_STACK_NULL = 2, ERR_CAPACITY_INVALID = 4, ERR_SIZE_INVALID = 8, ERR_DIFFERENT_TYPE = 16, ERR_CANAREIKA_LEFT_CHANGE = 32, ERR_CANAREIKA_RIGHT_CHANGE = 64, ERR_OVERFLOW = 128, ERR_BUFFER_SIZE_INVALID = 256, ERR_HASH_CHANGED = 512, NO_ERROR = 1024
};

struct stack_t;
typedef const char * const string;

const size_t NUL_CANAREIKA = 0x0000000000000000;
const size_t CANAREIKA     = 0xB333DEDDALL + 0xC0CAC0LL;
const size_t MAX_CAPACITY = 0xffffff;
const size_t MAX_SIZE = 200;
const size_t MAX_PRINT_ROWS = 5;
const size_t POWER = 1007;
const size_t MOD = 0xEDADEDAEDA;
string err_strings[] = {"no error", "your ptr is null", "stack is null", "capacity is invalid number", "size is invalid number", "type of your argument is different from initialized", "something changed region to the left of an array", "something changed region to the right of an array", "some number overflew past limit", "buffer size should be more than 0!", "hash of your function unexpectedly changed, maybe you swapped or edited elements by yourself?"};
string format_string = "\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/";
string bad = "MEOW!!!";
const static size_t strlen_format_string = strlen(format_string);
const static size_t num_of_errors = sizeof(err_strings) / sizeof(err_strings[0]);

#define stackDtor(stack) \
    stackDtor_internal(stack);\
    stackDtor2_internal(&stack);

 

//error check
#if LEVEL_OF_CHECK > 0
#define STACK_ERR_CHECK(stack, is_pt, sizeof_value, ...) {                                    \
    int errors = stackErrcheck(stack, sizeof_value, is_pt); \
    if (errors != 0) {\
        stackDump(stack, sizeof_value, errors, __VA_ARGS__);      \
        stack->hash = get_stack_hash(*stack);\
        return errors;\
    }}
#else
#define STACK_ERR_CHECK(stack, is_pt, sizeof_value, ...) ;
#endif // stack_err_sheck



#if LEVEL_OF_CHECK > 1
#define print_canareika_bytes(stack, LEFT_canareika_location, RIGHT_canareika_location) \
    if (errors | (ERR_STACK_NULL == 0)) {\
        printf(BLACK "canareika left -> " BYELLOW);\
        print_bytes_left_canareika(stack, LEFT_canareika_location);\
        printf("\n                  ");\
        print_bytes_right_canareika(stack, RIGHT_canareika_location);\
        printf(BLACK " <- canareika right" WHITE);\
    }
#else
#define print_canareika_bytes(stack, LEFT_canareika_location, RIGHT_canareika_location) ;
#endif // print_canareika_bytes

#if LEVEL_OF_CHECK > 1
#define print_canareika(stack, errors, location, value) {\
    int index = 0;\
    if ((char *)location##_canareika_location - (char*)stack->stack < 0)\
        index = -1;\
    else index = (int) stack->capacity;\
    \
    if (errors | (ERR_CANAREIKA_##location##_CHANGE == 1)) {\
        print_with_otstyp(otstyp, "    *[%d] = %zu" RED " [" #location " CANAREiKA CHANGED], %s %p\n"\
                          WHITE, index, value, bad, location##_canareika_location);\
    } else {\
        print_with_otstyp(otstyp, "    *[%d] = %zu" YELLOW " [CANAREIKA] %p\n"\
                          WHITE, index, value, location##_canareika_location);\
    }}
#else
#define print_canareika(stack, errors, location, value) ;
#endif // print_canareika

#if LEVEL_OF_CHECK > 1
#define check_if_canareika_correct(stack)\
    size_t left_canareika;\
    memcpy(&left_canareika, stack->raw, sizeof(size_t));\
    size_t right_canareika;\
    memcpy(&right_canareika, get_stack_offset(stack, stack->capacity), sizeof(size_t));\
    if (left_canareika  != CANAREIKA)\
        errors |= ERR_CANAREIKA_LEFT_CHANGE;\
    if (right_canareika != CANAREIKA)\
        errors |= ERR_CANAREIKA_RIGHT_CHANGE;
#else
#define check_if_canareika_correct(stack) ;
#endif // check canareika correct

#if LEVEL_OF_CHECK > 1
#define init_canareika_values(stack) \
    size_t * LEFT_canareika_location  = (size_t *)stack->raw;\
    size_t * RIGHT_canareika_location = (size_t *) get_stack_offset(stack, stack->capacity);\
    size_t   LEFT_canareika_value;\
    size_t   RIGHT_canareika_value;\
    memcpy(&LEFT_canareika_value,  LEFT_canareika_location,  sizeof(size_t));\
    memcpy(&RIGHT_canareika_value, RIGHT_canareika_location, sizeof(size_t));
#else
#define init_canareika_values(stack) ;
#endif // init canareika values



#if LEVEL_OF_CHECK > 2
#define get_total_bytes(stack, first_expr, second_expr) \
    stack->total_bytes = first_expr + second_expr;\
    if (check_if_overflow(first_expr, second_expr))\
        push_error(ERR_OVERFLOW, "realloc didnt make it... overflow with %zu bytes", stack->total_bytes);
#else
#define get_total_bytes(stack, first_expr, second_expr) \
    stack->total_bytes = first_expr + second_expr;
#endif // get_total_bytes

#if LEVEL_OF_CHECK > 2
#define check_if_hash_correct(stack) {\
    if (get_stack_hash(*stack) != stack->hash)\
        sassert(0, ERR_HASH_CHANGED, "FATAL ERROR, cant progress");\
}
#else
#define check_if_hash_correct(stack) (void * )0
#endif // check_if_hash_correct



//initialization
#define init_stack(stk, TYPE, buffer_size)\
    sassert(buffer_size > 0, ERR_BUFFER_SIZE_INVALID);\
    stack_t *stk = create_stack(sizeof(TYPE), #stk, buffer_size, __FILE__, __func__, __LINE__);\

//centralize
#define print_with_otstyp(otstyp, str, ...) {\
    for (size_t k = 0; k < otstyp; k++)\
        putchar(' ');\
    printf(str, ##__VA_ARGS__);}

//tipo typename
#define print_var_num(stack) {\
    if (stack->var_size == 1)\
        printf("%-3hhd : ", *(char * )get_stack_offset(stack, i));\
    else if (stack->var_size == 2)\
        printf("%-3hd : ", *(short * )get_stack_offset(stack, i));\
    else if (stack->var_size == 4)\
        printf("%-3d : ", *(int * )get_stack_offset(stack, i));\
    else if (stack->var_size == 8)\
        printf("%-3lf : ", *(double * )get_stack_offset(stack, i));\
}

// main pop
#define stackPop(stack, value) \
    stackPop_internal(stack, sizeof(*value), value, __FILE__, __func__, __LINE__);

// main push
#define stackPush(stack, value) \
    stackPush_internal(stack, sizeof(*value), value, __FILE__, __func__, __LINE__);

// declarations
stack_t * create_stack(size_t var_bytes, string var_name, size_t buffer_size,
                       const char * file_name, const char * func_name, size_t line);
size_t min(size_t a, size_t b);
size_t get_stack_hash(stack_t stack);
size_t mod_pow(size_t base, size_t exp, size_t mod);
int stackErrcheck(stack_t *stack, size_t sizeof_value, bool is_pt);
int stackPush_internal(stack_t *stack, size_t sizeof_value, void * value,
                              const char * file_name, const char * func_name, size_t line);
int stackPop_internal(stack_t *stack, size_t sizeof_value,
                      void * value, const char * file_name, const char * func_name, size_t line);
stackErr_t stackDtor_internal(stack_t *stack);
stackErr_t stackDtor2_internal(stack_t **stack);
void * get_stack_offset(stack_t *stack, size_t i);
void print_all_reasons(int errors);
void print_part_of_var_info(stack_t *stack, size_t start, size_t end, size_t is_occupied);
void print_whole_var_info(stack_t *stack);
void print_bytes_left_canareika(stack_t *stack, size_t * LEFT_canareika_location);
void print_bytes_right_canareika(stack_t *stack, size_t * RIGHT_canareika_location);
void stackDump(stack_t *stack, size_t sizeof_value, int errors,
               const char * file_name, const char * func_name, size_t line);
void reallocate_stack(stack_t*, size_t);
bool check_if_overflow(size_t first, size_t second);
bool is_error_active(int errors, stackErr_t error);

#endif // stack.h