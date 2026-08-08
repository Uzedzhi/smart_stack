#ifndef STACKVARS_H
#define STACKVARS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "stack.hpp"


const size_t _STK_INITIAL_SIZE  = 10;
const size_t MAX_STR_SIZE       = 256;
const size_t otstyp             = 2;
string bad                      = "<- MEOWWWW!!!";
string format_string            = "\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/";

// --------------------ErrHandle----------------------
#ifdef DEBUG
#define _STK_RETURN_ERR(condition, code, str, ...)\
    if (!(condition)) {\
        snprintf(_stk_last_error, MAX_STR_SIZE - 1, str, ##__VA_ARGS__);\
        return code;\
    }
#else
#define _STK_RETURN_ERR(condition, code, str, ...)\
    if (!(condition))\
        return code;
#endif
// ---------------------------------------------------

// -----------------LEVEL_OF_CHECK--------------------
#define _STK_NO_CHECK        0
#define _STK_MIN_CHECK       1
#define _STK_CNRK_CHECK      2
#define _STK_HASH_CHECK      3

#ifndef _STK_LEVEL_OF_CHECK
#define _STK_LEVEL_OF_CHECK _STK_CNRK_CHECK
#endif
// ---------------------------------------------------

// --------------------main struct---------------------
typedef struct stack_t{
    stack_var_t *stack;
    stack_var_t *raw;
    size_t size;
    size_t capacity;

#if _STK_LEVEL_OF_CHECK >= _STK_HASH_CHECK
    size_t hash;
#endif
} stack_t;
// ---------------------------------------------------

const stack_var_t CANAREIKA = (stack_var_t) (0xB333DED + 0xC0CAL);
const double FLT_ERR = 1e-6;

#if _STK_LEVEL_OF_CHECK == _STK_NO_CHECK
#define STACK_ERR_CHECK(stacj, is_put, ...) 
#else
#define STACK_ERR_CHECK(stack, is_put, ...) {            \
        StkErr err = stackErrcheck(stack, is_put);       \
        if (err != _STK_OK)                              \
            return err;                                  \
    }
#endif

// centralize
#define print_with_otstyp(otstyp, str, ...) {   \
        for (size_t k = 0; k < otstyp; k++)     \
            putc('\t', stderr);                 \
        fprintf(stderr, str, ##__VA_ARGS__);   \
    }

int is_same(stack_var_t value1, stack_var_t value2);
StkErr check_if_canareika_correct(stack_t *stack);
unsigned int get_stack_hash(stack_t *stack);

#endif