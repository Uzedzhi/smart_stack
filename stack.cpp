#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cstdint>

#include "stackvars.hpp"
#include "stack.hpp"
#include "MyLibs/sassert.hpp"
#include "MyLibs/helper_funcs.hpp"

char _stk_last_error[MAX_STR_SIZE] = {};

static int count = 1;
const static stack_var_t POISON = (stack_var_t) 0x2134256754321123;
const static size_t MAX_CAPACITY = 0xffffff;
const static size_t MAX_PRINT_ROWS = 5;

stack_t *StackCtor() {
    stack_t *stk = CALLOC_WITH_TYPE(1, stack_t);
    _STK_RETURN_ERR(stk, NULL, "не удалось создать структуру стека. Проверьте, хватает ли памяти");

    stk->capacity   = _STK_INITIAL_SIZE;
    stk->size       = 0;

    #if _STK_LEVEL_OF_CHECK >= _STK_CNRK_CHECK
        stk->raw    = (stack_var_t *) calloc(_STK_INITIAL_SIZE + 2, sizeof(stack_var_t));
        stk->stack  = stk->raw + 1;
    #else
        stk->raw    = (stack_var_t *) calloc(_STK_INITIAL_SIZE, sizeof(stack_var_t));
        stk->stack  = stk->raw;
    #endif

    _STK_RETURN_ERR(stk->raw, NULL, 
                    "не удалось созадть стек. Проверьте, хватает ли памяти");

    for (size_t i = 0; i < stk->capacity; i++) {
        stk->stack[i] = (stack_var_t) POISON;
    }

#if _STK_LEVEL_OF_CHECK >= _STK_CNRK_CHECK
    stk->raw[0]                 = (stack_var_t) CANAREIKA;
    stk->raw[stk->capacity + 1] = (stack_var_t) CANAREIKA;
#endif

#if _STK_LEVEL_OF_CHECK >= _STK_HASH_CHECK
    stk->hash = get_stack_hash(*stk);
#endif
    return stk;
}

stack_var_t get_top(stack_t *stack) {
    return stack->stack[stack->size - 1];
}

size_t get_size(stack_t *stack) {
    return stack->size;
}

bool check_if_overflow(size_t first, size_t second) {
    return second > SIZE_MAX - first;
}

void print_part_of_var_info(stack_t *stack, size_t start, size_t end, size_t is_occupied) {
    sassert(stack, _STK_ERR_PTR_NULL);

    char occupied_symbol = (is_occupied) ? '*' : ' ';
    for (size_t i = start; i < end; i++) {
        print_with_otstyp(otstyp, "    %c[%zu]", occupied_symbol, i);

        if (is_same(stack->stack[i], (stack_var_t) POISON)) {
            fprintf(stderr, " = " MAGENTA "POISON" RESET);
        } else {
        #ifdef _STK_STR_TYPE
            if (i < stack->size)
                fprintf(stderr, " = " STACK_VAR_SPEC, stack->stack[i]);
            else
                fprintf(stderr, " = " YELLOW "FREED_PTR" RESET);
        #else
            fprintf(stderr, " = " STACK_VAR_SPEC, stack->stack[i]);
        #endif
        }

        for (size_t j = 0; j < sizeof(stack_var_t); j++) {
            fprintf(stderr, " [%02X]", *((unsigned char *)(stack->stack + i) + j));
        }
        fprintf(stderr, " %p\n", stack->stack + i);
    }
}

void print_whole_var_info(stack_t *stack) {
    sassert(stack, _STK_ERR_PTR_NULL);

    print_part_of_var_info(stack, 0, stack->size, 1);
    if (stack->capacity - stack->size > MAX_PRINT_ROWS) {
        print_part_of_var_info(stack, stack->size, stack->size + MAX_PRINT_ROWS, 0);
        print_with_otstyp(otstyp + 3, "...\n");
        print_part_of_var_info(stack, stack->capacity - MAX_PRINT_ROWS, stack->capacity, 0);
    }
    else
        print_part_of_var_info(stack, stack->size, stack->capacity, 0);
}

void print_bytes_right_canareika(stack_t *stack) {
    sassert(stack, _STK_ERR_PTR_NULL);

    for (size_t i = 0; i < 3 * sizeof(stack_var_t); i++) {
        if (i == 2 * sizeof(stack_var_t)) {
            fprintf(stderr, " | " BYELLOW);
        }
        fprintf(stderr, "[%02X]", *((unsigned char *)(stack->stack + stack->capacity - 2) + i));
    }
}

void print_bytes_left_canareika(stack_t *stack) {
    sassert(stack, _STK_ERR_PTR_NULL);

    for (size_t i = 0; i < 3 * sizeof(stack_var_t); i++) {
        if (i == sizeof(stack_var_t)) {
            fprintf(stderr, RESET " | ");
        }
        fprintf(stderr, "[%02X]", *((unsigned char *)stack->raw + i));
    }
}

int is_same(stack_var_t value1, stack_var_t value2) {
    #ifdef _STK_STR_TYPE
        return value1 == value2;
    #else
        #ifdef _STK_FLT_TYPE
            return fabs(value1 - value2) < FLT_ERR;
        #else
            return value1 == value2;
        #endif
    #endif
}

int stackDump_raw(stack_t *stack) {
    sassert(stack,     _STK_ERR_PTR_NULL);

    for (size_t i = 0; i < stack->capacity; i++) {
        stack_var_t value = stack->stack[i];
        printf(RESET);
        if (i == stack->size - 1)                   printf(GREEN);
        if (is_same(value, (stack_var_t) POISON))   printf("[PSN]\n");
        else                                        printf("[" STACK_VAR_SPEC "]\n", (stack_var_t) value);
    }
    printf(RESET "\n");
    return 0;
}

void stkPERROR() {
    if (_stk_last_error[0] != '\0')
        fprintf(stderr, RED "[ОШИБКА]: %s" RESET "\n", _stk_last_error);
}

StkErr stackErrcheck(stack_t *stack, bool is_pt) {
    _STK_RETURN_ERR(stack,          _STK_ERR_PTR_NULL,
                    "указатель на stack нулевой");
    _STK_RETURN_ERR(stack->stack,   _STK_ERR_PTR_NULL,
                    "указатель на stack->stack нулевой");
    _STK_RETURN_ERR(stack->raw,     _STK_ERR_PTR_NULL,
                    "указатель на stack->raw нулевой");
    _STK_RETURN_ERR(stack->capacity <= MAX_CAPACITY, _STK_ERR_CAPACITY_INVALID, 
                    "емкость стека больше максимально допустимой(%zu > %zu)", stack->capacity, MAX_CAPACITY);
    _STK_RETURN_ERR(stack->size <= stack->capacity, _STK_ERR_INVALID_SIZE, 
                    "размер стека больше емкости(%zu > %zu)", stack->size, stack->capacity);
    _STK_RETURN_ERR(!is_pt || stack->size != 0, _STK_ERR_INVALID_SIZE, 
                    "нельзя делать pop в пустом стеке");

#if _STK_LEVEL_OF_CHECK >= _STK_CNRK_CHECK
    StkErr res = check_if_canareika_correct(stack);
    if (!res)
        return res;
#endif

#if _STK_LEVEL_OF_CHECK >= _STK_HASH_CHECK
    size_t hash = get_stack_hash(stack);
    if (is_pt) {
        _STK_RETURN_ERR(stack->hash == hash, _STK_ERR_INVALID_HASH, 
                        "у стека изменился хеш с последней операции(%zu != %zu)."
                        "Было обнаружено когда попытался сделаться pop."
                        "Может вы изменили его программно между операциями?", stack->hash, hash);
    } else {
        _STK_RETURN_ERR(stack->hash == hash, _STK_ERR_INVALID_HASH, 
                        "у стека изменился хеш с последней операции(%zu != %zu)."
                        "Было обнаружено когда попытался сделаться push."
                        "Может вы изменили его программно между операциями?", stack->hash, hash); 
    }
#endif

    return _STK_OK;
}

StkErr check_if_canareika_correct(stack_t *stack) {
    sassert(stack, _STK_ERR_PTR_NULL);
    
    stack_var_t left_canareika  = stack->raw[0];
    stack_var_t right_canareika = stack->stack[stack->capacity];
    _STK_RETURN_ERR(is_same(left_canareika,  (stack_var_t) CANAREIKA), _STK_ERR_CANAREIKA_LEFT_CHANGE,
                    "левая канарейка изменилась");
    _STK_RETURN_ERR(is_same(right_canareika, (stack_var_t) CANAREIKA), _STK_ERR_CANAREIKA_RIGHT_CHANGE,
                    "правая канарейка изменилась");

    return _STK_OK;
}

void print_left_canareika(stack_t *stack) {
    print_with_otstyp(otstyp, "    *[-1] = " MAGENTA "CANARY " RESET);
    for (size_t i = 0; i < sizeof(stack_var_t); i++) {
        fprintf(stderr, "[%02X] ", *((unsigned char *) stack->raw + i));
    }
    if (!is_same(stack->raw[0], (stack_var_t) CANAREIKA))
        fprintf(stderr, "%p" RED    " [LEFT CANAREIKA CHANGED] %s" RESET "\n", stack->raw, bad);
    else
        fprintf(stderr, "%p" YELLOW " [LEFT CANAREIKA]"         RESET "\n", stack->raw);
}

void print_right_canareika(stack_t *stack) {
    print_with_otstyp(otstyp, "    *[160] = " MAGENTA "CANARY " RESET);
    for (size_t i = 0; i < sizeof(stack_var_t); i++) {
        fprintf(stderr, "[%02X] ", *((unsigned char *)(stack->stack + stack->capacity) + i));
    }
    if (!is_same(stack->stack[stack->capacity], (stack_var_t) CANAREIKA))
        fprintf(stderr, "%p" RED    " [RIGHT CANAREIKA CHANGED] %s" RESET "\n", stack->stack[stack->capacity], bad);
    else
        fprintf(stderr, "%p" YELLOW " [RIGHT CANAREIKA]"         RESET "\n", stack->stack[stack->capacity]);
}

void print_canareika_bytes(stack_t *stack) {
    fprintf(stderr, BLACK "canareika left -> " BYELLOW);
    print_bytes_left_canareika(stack);
    fprintf(stderr, "\n                  ");
    print_bytes_right_canareika(stack);
    fprintf(stderr, BLACK " <- canareika right" RESET);
}

int stackDump_internal(stack_t *stack, int is_end, const char *file_name, const char *func_name, size_t line) {
    sassert(stack,     _STK_ERR_PTR_NULL);
    sassert(file_name, _STK_ERR_PTR_NULL);
    sassert(func_name, _STK_ERR_PTR_NULL);

    size_t size     = stack->size;
    size_t capacity = stack->capacity;
    fprintf(stderr, RED "%sSTART DUMP (%d)%s" RESET "\n", format_string, count++, format_string);
    if (_stk_last_error[0] == '\0') {
    #ifdef DEBUG
        print_with_otstyp(otstyp, "Stack is dumped with no errors \n")
    #else
        print_with_otstyp(otstyp, "no information about errors. Recompile with -DDEBUG\n")
    #endif
    } else {
        print_with_otstyp(otstyp, "last error: ");
        stkPERROR();
    }

    print_with_otstyp(otstyp, "struct: \n");
    print_with_otstyp(otstyp, "\tstack->raw:      <%p>;\n", stack->raw);
    print_with_otstyp(otstyp, "\tstack->stack:    <%p>; (%zu) bytes\n", stack->stack, sizeof(stack_var_t) * capacity);
    print_with_otstyp(otstyp, "\tstack->size:     <%zu>; " RED "%s\n"   RESET, stack->size,     (size >= capacity)        ? bad : " ");
    print_with_otstyp(otstyp, "\tstack->capacity: <%zu>; " RED "%s\n\n" RESET, stack->capacity, (capacity > MAX_CAPACITY) ? bad : " ");
    #ifdef _STK_STR_TYPE
        string type = "string";
    #else
        #ifdef _STK_FLT_TYPE
            string type = "double";
        #else
            string type = "integer";
        #endif
    #endif
    print_with_otstyp(otstyp, "type           | <%s>\n", type);        
    print_with_otstyp(otstyp, "dump where     | <%s; %s:%zu>\n", file_name, func_name, line);
    print_with_otstyp(otstyp, "size           | <%zu> " RED "%s\n"   RESET, stack->size,     (size >= capacity)        ? bad : "");
    print_with_otstyp(otstyp, "capacity       | <%zu> " RED "%s\n\n" RESET, stack->capacity, (capacity > MAX_CAPACITY) ? bad : "");

    BEGIN
    if (!stack->stack || !stack->raw) {
        print_with_otstyp(otstyp, "Next dump is not possible, because stack->stack"
                                  "or stack->raw pointer is null");
        break;
    }
    print_with_otstyp(otstyp, "stack dump:  {\n");
    
#if _STK_LEVEL_OF_CHECK >= _STK_CNRK_CHECK
    print_left_canareika(stack);
#endif
    if (size >= capacity) {
        print_part_of_var_info(stack, 0, fmin(stack->capacity, MAX_PRINT_ROWS), 0);
        print_with_otstyp(3 * otstyp, "...\n");
    } else
        print_whole_var_info(stack);
    
#if _STK_LEVEL_OF_CHECK >= _STK_CNRK_CHECK
    print_right_canareika(stack);
#endif

    print_with_otstyp(otstyp, "   };\n");
    print_with_otstyp(otstyp, "}\n");

#if _STK_LEVEL_OF_CHECK >= _STK_CNRK_CHECK
    print_canareika_bytes(stack);
#endif
    END

    if (is_end)
        printf(RED "\n%sENDING DUMP%s\n\n" RESET, format_string, format_string);
    return 0;
}

StkErr reallocate_stack(stack_t *stack, double multiplier) {
    sassert(stack, _STK_ERR_PTR_NULL);

    size_t old_capacity  = stack->capacity;

#if _STK_LEVEL_OF_CHECK >= _STK_CNRK_CHECK
    size_t new_capacity  = old_capacity * 2 + 2;
#else
    size_t new_capacity  = old_capacity * 2;
#endif

    stack->capacity     *= 2;
    if (reallocate_array((void **) &(stack->raw), old_capacity, new_capacity * sizeof(stack_var_t)) == NULL)
        return _STK_ERR_REALLOC_FAIL;

    #if _STK_LEVEL_OF_CHECK >= _STK_CNRK_CHECK
        stack->stack = stack->raw + 1;
        stack->stack[stack->capacity] = (stack_var_t) CANAREIKA;
    #else
        stack->stack = stack->raw;
    #endif

    for (size_t i = old_capacity; i < stack->capacity; i++) {
        stack->stack[i] = (stack_var_t) POISON;
    }

    return _STK_OK;
}

StkErr push(stack_t *stack, stack_var_t value) {
    STACK_ERR_CHECK(stack, 0);

    if (stack->size >= stack->capacity)
        _STK_RETURN_ERR(reallocate_stack(stack, 2) == _STK_OK, _STK_ERR_REALLOC_FAIL, 
                        "не удалось увеличить размер стека до %zu элементов", stack->capacity);

#ifdef _STK_STR_TYPE
    stack->stack[stack->size++] = strdup(value);
#else
    stack->stack[stack->size++] = value;
#endif

#if _STK_LEVEL_OF_CHECK >= _STK_HASH_CHECK
    stack->hash = get_stack_hash(stack);
#endif

    return _STK_OK;
}

StkErr pop(stack_t *stack, stack_var_t *value) {
    STACK_ERR_CHECK(stack, 1);

    stack->size--;
    size_t size = stack->size;
    if (value)
        *value = stack->stack[size];

#if _STK_LEVEL_OF_CHECK >= _STK_MIN_CHECK
    stack->stack[size] = (stack_var_t) POISON;
#endif

#if _STK_LEVEL_OF_CHECK >= _STK_HASH_CHECK
    stack->hash = get_stack_hash(stack);
#endif
    return _STK_OK;
}

unsigned int get_stack_hash(stack_t *stack) {
    unsigned int hash = 0;
    for (unsigned int i = 0; i < sizeof(stack_t) - sizeof(unsigned int); i++) {
        hash = hash * 65559 + (unsigned int) (*((char *) stack + i));
    }
    return hash;
}

void stackDtor(stack_t *stack) {
    if (stack != NULL) {
    #ifdef _STK_STR_TYPE
        for (size_t i = 0; i < stack->size; i++) {
            SMART_FREE(stack->stack[i]);
        }
    #endif
        SMART_FREE(stack->raw);
        free(stack);
    }
}