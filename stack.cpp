#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cstdint>

#include "stack.h"

static int count = 1;
static size_t * ptr_start = NULL;
static size_t * ptr_end = NULL;

struct stack_t {
    const char * func_name;
    const char * file_name;
    const char * var_name;
    size_t line;
    void * stack;
    void * raw;
    size_t total_bytes;
    size_t var_size;
    size_t size;
    size_t capacity;
    size_t hash;
};

// function bodies
size_t min(size_t a, size_t b) {
    return (a < b) ? a : b;
}

stack_t * create_stack(size_t var_bytes, string var_name, size_t buffer_size,
                       const char * file_name, const char * func_name, size_t line) {
    sassert(file_name, ERR_PTR_NULL);
    sassert(func_name, ERR_PTR_NULL);

    stack_t *stk = (stack_t *) calloc(1, sizeof(stack_t));
    sassert(stk != NULL, ERR_PTR_NULL);

    stk->func_name = func_name;
    stk->file_name = file_name;
    stk->var_name  = var_name;
    stk->line      = line;

    get_total_bytes(stk, buffer_size * var_bytes, 2 * sizeof(size_t));
    stk->stack = (void *)((char *)calloc(stk->total_bytes, sizeof(char)) + sizeof(size_t));
    sassert(stk != NULL, ERR_PTR_NULL);
    
    stk->var_size =  var_bytes;
    stk->capacity =  buffer_size;
    stk->size     =  0;

    stk->raw  = (void *)((char *) stk->stack - sizeof(size_t));
    memcpy(stk->raw, &CANAREIKA, sizeof(size_t));
    memcpy(get_stack_offset(stk, stk->capacity), &CANAREIKA, sizeof(size_t));
    stk->hash = get_stack_hash(*stk);
    ptr_start = (size_t *) stk;
    ptr_end   = (size_t *) stk + sizeof(stack_t);
    return stk;
}

bool check_if_overflow(size_t first, size_t second) {
    return first > 0 && second > SIZE_MAX - first;
}

void print_whole_var_info(stack_t *stack) {
    sassert(stack, ERR_PTR_NULL);

    size_t otstyp = strlen_format_string / 2;
    print_part_of_var_info(stack, 0, stack->size, 1);
    if (stack->capacity - stack->size > MAX_PRINT_ROWS) {                                                                                                                       \
        print_part_of_var_info(stack, stack->size, stack->size + MAX_PRINT_ROWS, 0);
        print_with_otstyp(otstyp + 20, "...\n");
    }
    else
        print_part_of_var_info(stack, stack->size, stack->capacity, 0);

}

void print_part_of_var_info(stack_t *stack, size_t start, size_t end, size_t is_occupied) {
    sassert(stack, ERR_PTR_NULL);

    size_t otstyp = strlen_format_string / 2;
    char occupied_symbol = (is_occupied) ? '*' : ' ';
    for (size_t i = start; i < end; i++) {
        print_with_otstyp(otstyp, "    %c[%zu] = ", occupied_symbol, i);
        print_var_num(stack);
        for (size_t j = 0; j < stack->var_size; j++) {
            printf("[%d] ", *(((char *)(get_stack_offset(stack, i))) + j));
        }
        printf("%p\n", (char *)get_stack_offset(stack, i));
    }     
}

void print_bytes_right_canareika(stack_t *stack, size_t * RIGHT_canareika_location) {
    sassert(stack,     ERR_PTR_NULL);
    sassert(RIGHT_canareika_location,     ERR_PTR_NULL);

    for (size_t i = 0; i < sizeof(size_t) + stack->var_size * 2; i++) {
        if (i == stack->var_size * 2) {
            printf(" | " BYELLOW);
        }
        printf("[%d]", *((char *)RIGHT_canareika_location - stack->var_size * 2 + i));
    }
}

void print_bytes_left_canareika(stack_t *stack, size_t * LEFT_canareika_location) {
    sassert(stack,     ERR_PTR_NULL);
    sassert(LEFT_canareika_location,     ERR_PTR_NULL);

    for (size_t i = 0; i < sizeof(size_t) + stack->var_size * 2; i++) {
        if (i == sizeof(size_t)) {
            printf(WHITE  " | ");
        }
        printf("[%d]", *((char *)LEFT_canareika_location + i));
    }
}

bool is_error_active(int errors, stackErr_t error) {
    return (errors & error) == error;
}

void stackDump(stack_t *stack, size_t sizeof_value, int errors,
               const char * file_name, const char * func_name, size_t line) {
    sassert(stack,     ERR_PTR_NULL);
    sassert(file_name, ERR_PTR_NULL);
    sassert(func_name, ERR_PTR_NULL);
    sassert(is_error_active(errors, ERR_PTR_NULL) == 0, ERR_PTR_NULL);
        
    size_t otstyp = strlen_format_string / 2;

    printf(RED "%sSTART DUMP%s" WHITE " (count=%d)\n", format_string, format_string, count++);
    print_with_otstyp(otstyp, "reasons : "); print_all_reasons(errors);
    print_with_otstyp(otstyp, "var stack      | <%s>\n",                stack->var_name);
    print_with_otstyp(otstyp, "HEX stack      | <%p>\n",                stack->stack);
    print_with_otstyp(otstyp, "your type bytes| <%zu> ",                sizeof_value); 
    (is_error_active(errors, ERR_DIFFERENT_TYPE)) ? printf(RED "<-needed %zu bytes %s\n" WHITE, stack->var_size, bad) : printf("\n");
    print_with_otstyp(otstyp, "init where     | <%s; %s:%zu>\n",        stack->file_name, stack->func_name, stack->line);
    print_with_otstyp(otstyp, "dump where     | <%s; %s:%zu>\n",        file_name, func_name, line);
    print_with_otstyp(otstyp, "var size       | <%zu> " RED "%s\n" WHITE, stack->size, (is_error_active(errors, ERR_SIZE_INVALID)) ? bad : "\n");
    print_with_otstyp(otstyp, "var capacity   | <%zu> " RED "%s\n" WHITE, stack->capacity, (is_error_active(errors, ERR_CAPACITY_INVALID)) ? bad : "\n");
    print_with_otstyp(otstyp, "var (%d bytes) values {\n", (int)(stack->var_size * stack->capacity));

    BEGIN
    if (is_error_active(errors, ERR_STACK_NULL))
        break;

    init_canareika_values(stack);
    print_canareika(stack, errors, LEFT, LEFT_canareika_value);
    if (is_error_active(errors, ERR_SIZE_INVALID)) {
        print_part_of_var_info(stack, 0, min(stack->capacity, MAX_PRINT_ROWS), 0);
        print_with_otstyp(otstyp + 24, "...\n");
    } else
        print_whole_var_info(stack);
    print_canareika(stack, errors, RIGHT, RIGHT_canareika_value);

    print_with_otstyp(otstyp, "   };\n");
    print_with_otstyp(otstyp, "}\n");

    print_canareika_bytes(stack, LEFT_canareika_location, RIGHT_canareika_location);
    END

    printf(RED "\n%sENDING DUMP%s\n\n" WHITE, format_string, format_string);
}

void reallocate_stack(stack_t *stack, size_t new_capacity){
    sassert(stack != NULL, ERR_PTR_NULL);

    get_total_bytes(stack, (stack->capacity * stack->var_size) * 2, 2 * sizeof(size_t));

    memcpy(get_stack_offset(stack, stack->capacity), &NUL_CANAREIKA, sizeof(size_t));
    void * temp_stack = realloc(stack->raw, stack->total_bytes);
    sassert(temp_stack != NULL, ERR_PTR_NULL);

    stack->stack = (void*)((char *)temp_stack + sizeof(size_t));
    stack->raw = temp_stack;
    stack->capacity = new_capacity;

    memcpy(get_stack_offset(stack, stack->capacity), &CANAREIKA, sizeof(size_t));
}

void print_all_reasons(int errors) {
    for (size_t i = 0; i < num_of_errors; i++) {
        if ((errors & 1) == 1)
            printf("<%s> ", err_strings[i + 1]);
        errors>>=1;
    }
    putchar('\n');
}

void * get_stack_offset(stack_t *stack, size_t i) {
    sassert(stack, ERR_PTR_NULL);

    return (char*)stack->stack + i * stack->var_size;
}

int stackPush_internal(stack_t *stack, size_t sizeof_value,
                       void * value, const char * file_name, const char * func_name, size_t line) {
    sassert(stack,     ERR_PTR_NULL);
    sassert(value,     ERR_PTR_NULL);
    sassert(file_name, ERR_PTR_NULL);
    sassert(func_name, ERR_PTR_NULL);
    STACK_ERR_CHECK(stack, 0, sizeof_value, file_name, func_name, line);

    if (stack->size >= stack->capacity) {
        reallocate_stack(stack, stack->capacity * 2);
    }
    memcpy(get_stack_offset(stack, stack->size++), value, stack->var_size);
    stack->hash = get_stack_hash(*stack);

    STACK_ERR_CHECK(stack, 0, sizeof_value, file_name, func_name, line);

    return NO_ERROR;
}

int stackPop_internal(stack_t *stack, size_t sizeof_value,
                      void * value, const char * file_name, const char * func_name, size_t line) {
    sassert(stack,     ERR_PTR_NULL);
    sassert(value,     ERR_PTR_NULL);
    sassert(file_name, ERR_PTR_NULL);
    sassert(func_name, ERR_PTR_NULL);
    STACK_ERR_CHECK(stack, 1, sizeof_value, file_name, func_name, line);

    memcpy(value, get_stack_offset(stack, --stack->size), stack->var_size);
    stack->hash = get_stack_hash(*stack);

    STACK_ERR_CHECK(stack, 0, sizeof_value, file_name, func_name, line);
    return NO_ERROR;
}
size_t mod_pow(size_t base, size_t exp, size_t mod) {
    size_t result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

size_t get_stack_hash(stack_t stack) {
    size_t hash = 0;
    for (size_t i = 0; i < sizeof(stack_t) - sizeof(size_t); i++) {
        hash += mod_pow((size_t)*((char *) &stack + i) + i, POWER, MOD);
    }
    return hash;
}

stackErr_t stackDtor_internal(stack_t *stack) {
    sassert(stack, ERR_PTR_NULL);
    sassert((size_t *)stack >= ptr_start && (size_t *)stack <= ptr_end, ERR_PTR_NULL);
    sassert(get_stack_hash(*stack) == stack->hash, ERR_HASH_CHANGED);

    free(stack->raw);
    stack->raw = NULL;
    stack->stack = NULL;

    return NO_ERROR;
}

stackErr_t stackDtor2_internal(stack_t **stack) {
    free(*stack);
    *stack = NULL;
    return NO_ERROR;
}

int stackErrcheck(stack_t *stack, size_t sizeof_value, bool is_pt) {
    int errors = 0;
    BEGIN
    if (stack->stack == NULL || stack->raw == NULL) {
        errors |= ERR_PTR_NULL;
        break;
    }
    if (sizeof_value != stack->var_size)
        errors |= ERR_DIFFERENT_TYPE;

    check_if_hash_correct(stack);

    check_if_canareika_correct(stack);
    if (stack->capacity > MAX_CAPACITY)
        errors |= ERR_CAPACITY_INVALID;
    if (stack->size > stack->capacity)
        errors |= ERR_SIZE_INVALID;
    if (is_pt == 1 && stack->size - 1 > stack->capacity) {
        errors |= ERR_SIZE_INVALID;
        stack->size--;
    }
    END
    return errors;
}