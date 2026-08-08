#ifndef STACK_H
#define STACK_H

#include <stdlib.h>

typedef const char * const string;
typedef struct stack_t stack_t;

// ---------------stack element type------------------
#ifdef _STK_STR_TYPE
    typedef char *    stack_var_t;
    #define STACK_VAR_SPEC "%.255s"
#else 
    #ifdef _STK_FLT_TYPE
        typedef double      stack_var_t;
        #define STACK_VAR_SPEC "%.2f"
    #else
        typedef int         stack_var_t;
        #define STACK_VAR_SPEC "%-3d"
    #endif
#endif
// ---------------------------------------------------

// ----------------------Errors-----------------------

    enum StkErr {
        _STK_OK                             =  0,
        _STK_ERR_BUFFER_SIZE_INVALID        = -1,
        _STK_ERR_MALLOC_FAIL                = -2,
        _STK_ERR_PTR_NULL                   = -3,
        _STK_ERR_REALLOC_FAIL               = -4,
        _STK_ERR_INVALID_SIZE               = -5,
        _STK_ERR_CAPACITY_INVALID           = -6,
        _STK_ERR_INVALID_HASH               = -7,
        _STK_ERR_CANAREIKA_LEFT_CHANGE      = -8,
        _STK_ERR_CANAREIKA_RIGHT_CHANGE     = -9,
    };

// ---------------------------------------------------

#define stackDump(stack) \
    stackDump_internal(stack, 1, __FILE__, __func__, __LINE__);

stack_t *StackCtor();
void stackDtor(stack_t *stack);
stack_var_t get_top(stack_t *stack);
size_t get_size(stack_t *stack);
StkErr push(stack_t *stack, stack_var_t  value);
StkErr pop(stack_t *stack,  stack_var_t *value);
int stackDump_raw(stack_t *stack);
int stackDump_internal(stack_t *stack, int is_end, const char *file_name, const char *func_name, size_t line);
void stkPERROR();

#endif // stack.hpp