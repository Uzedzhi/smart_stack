#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    stk->file_name =  file_name;
    stk->var_name =  var_name;
    stk->line =  line;

    stk->total_bytes = buffer_size * var_bytes + 2 * sizeof(size_t);
    if (check_if_overflow(buffer_size * var_bytes, 2 * sizeof(size_t)))
        push_error(ERR_OVERFLOW, "realloc didnt make it... overflow with %zu bytes", stk->total_bytes);
    stk->stack = (void *)((char *)calloc(stk->total_bytes, sizeof(char)) + sizeof(size_t));
    sassert(stk != NULL, ERR_PTR_NULL);
    
    stk->var_size =  var_bytes;
    stk->capacity =  buffer_size;
    stk->size     =  0;

    stk->raw = (void *)((char *) stk->stack - sizeof(size_t));
    memcpy(stk->raw, &CANAREIKA, sizeof(size_t));
    memcpy(get_stack_offset(stk, stk->capacity), &CANAREIKA, sizeof(size_t));
    stk->hash = get_stack_hash(*stk);
    ptr_start = (size_t *) stk;
    ptr_end = (size_t *) stk + sizeof(stack_t);
    return stk;
}

bool check_if_overflow(size_t first, size_t second) {
    size_t ov = first + second;
    if (ov - first != second)
        return 1;
    return 0;
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

void stackDump(stack_t *stack, size_t sizeof_value, bool errors[],
               const char * file_name, const char * func_name, size_t line) {
    sassert(stack,     ERR_PTR_NULL);
    sassert(errors,    ERR_PTR_NULL);
    sassert(file_name, ERR_PTR_NULL);
    sassert(func_name, ERR_PTR_NULL);
    sassert(errors[ERR_PTR_NULL] != 1, ERR_PTR_NULL);
        

    size_t otstyp = strlen_format_string / 2;
    
    printf(RED "%sSTART DUMP%s" WHITE " (count=%d)\n", format_string, format_string, count++);
    print_with_otstyp(otstyp, "reasons : "); print_all_reasons(errors);
    print_with_otstyp(otstyp, "var stack      | <%s>\n",                stack->var_name);
    print_with_otstyp(otstyp, "HEX stack      | <%p>\n",                stack->stack);
    print_with_otstyp(otstyp, "your type bytes| <%zu> ",                sizeof_value); 
    (errors[ERR_DIFFERENT_TYPE] == 1) ? printf(RED "<-needed %zu bytes %s\n" WHITE, stack->var_size, bad) : printf("\n");
    print_with_otstyp(otstyp, "init where     | <%s; %s:%zu>\n",        stack->file_name, stack->func_name, stack->line);
    print_with_otstyp(otstyp, "dump where     | <%s; %s:%zu>\n",        file_name, func_name, line);
    print_with_otstyp(otstyp, "var size       | <%zu> " RED "%s\n" WHITE, stack->size, (errors[ERR_SIZE_INVALID] == 1) ? bad : "\n");
    print_with_otstyp(otstyp, "var capacity   | <%zu> " RED "%s\n" WHITE, stack->capacity, (errors[ERR_CAPACITY_INVALID] == 1) ? bad : "\n");
    print_with_otstyp(otstyp, "var (%d bytes) values {\n", (int)(stack->var_size * stack->capacity));

    BEGIN
    if (errors[ERR_STACK_NULL] == 1)
        break;

    size_t * LEFT_canareika_location  = (size_t *)stack->raw;
    size_t * RIGHT_canareika_location = (size_t *) get_stack_offset(stack, stack->capacity);
    size_t   LEFT_canareika_value;
    size_t   RIGHT_canareika_value;
    memcpy(&LEFT_canareika_value,  LEFT_canareika_location,  sizeof(size_t));
    memcpy(&RIGHT_canareika_value, RIGHT_canareika_location, sizeof(size_t));

    print_canareika(stack, errors, LEFT, LEFT_canareika_value);

    if (errors[ERR_SIZE_INVALID] == 1) {
        print_part_of_var_info(stack, 0, min(stack->capacity, MAX_PRINT_ROWS), 0);
        print_with_otstyp(otstyp + 24, "...\n");
    } else
        print_whole_var_info(stack);
    
    print_canareika(stack, errors, RIGHT, RIGHT_canareika_value);

    print_with_otstyp(otstyp, "   };\n");
    print_with_otstyp(otstyp, "}\n");

    if (errors[ERR_STACK_NULL] == 0) {
        printf(BLACK "canareika left -> " BYELLOW);
        print_bytes_left_canareika(stack, LEFT_canareika_location);
        printf("\n                  ");
        print_bytes_right_canareika(stack, RIGHT_canareika_location);
        printf(BLACK " <- canareika right" WHITE);
    }
    END

    printf(RED "\n%sENDING DUMP%s\n\n" WHITE, format_string, format_string);
}

void reallocate_stack(stack_t *stack, size_t new_capacity){
    sassert(stack != NULL, ERR_PTR_NULL);

    stack->total_bytes = (stack->capacity * stack->var_size) * 2 + 2 * sizeof(size_t);
    if (check_if_overflow((stack->capacity * stack->var_size), (stack->capacity * stack->var_size) + 2 * sizeof(size_t)))
        push_error(ERR_OVERFLOW, "realloc didnt make it... overflow with %zu bytes", stack->total_bytes);

    memcpy(get_stack_offset(stack, stack->capacity), &NUL_CANAREIKA, sizeof(size_t));
    void * temp_stack = realloc(stack->raw, stack->total_bytes);
    sassert(temp_stack != NULL, ERR_PTR_NULL);

    stack->stack = (void*)((char *)temp_stack + sizeof(size_t));
    stack->raw = temp_stack;
    stack->capacity = new_capacity;

    memcpy((char *)get_stack_offset(stack, stack->capacity), &CANAREIKA, sizeof(size_t));
}

void print_all_reasons(bool errors[]) {
    sassert(errors, ERR_PTR_NULL);

    for (size_t i = 0; i < num_of_errors; i++) {
        if (errors[i] == 1) 
            printf("<%s> ", err_strings[i]);
    }
    putchar('\n');
}

void * get_stack_offset(stack_t *stack, size_t i) {
    sassert(stack, ERR_PTR_NULL);

    return (char*)stack->stack + i * stack->var_size;
}

stackErr_t stackPush_internal(stack_t *stack, size_t sizeof_value,
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

stackErr_t stackPop_internal(stack_t *stack, size_t sizeof_value,
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

void stackErrcheck(stack_t *stack, size_t sizeof_value, bool is_pt, bool errors[]) {
    sassert(stack, ERR_PTR_NULL);
    sassert(errors, ERR_PTR_NULL);

    BEGIN
    if (stack->stack == NULL || stack->raw == NULL) {
        errors[ERR_STACK_NULL] = 1;
        break;
    }
    if (sizeof_value != stack->var_size)
        errors[ERR_DIFFERENT_TYPE] = 1;

    if (get_stack_hash(*stack) != stack->hash)
        sassert(0, ERR_HASH_CHANGED, "FATAL ERROR, cant progress");

    size_t left_canareika;
    memcpy(&left_canareika, stack->raw, sizeof(size_t));
    size_t right_canareika;
    memcpy(&right_canareika, get_stack_offset(stack, stack->capacity), sizeof(size_t));
    

    if (left_canareika  != CANAREIKA)
        errors[ERR_CANAREIKA_LEFT_CHANGE]  = 1;
    if (right_canareika != CANAREIKA)
        errors[ERR_CANAREIKA_RIGHT_CHANGE] = 1;
    if (stack->capacity > MAX_CAPACITY)
        errors[ERR_CAPACITY_INVALID] = 1;
    if (stack->size > stack->capacity)
        errors[ERR_SIZE_INVALID] = 1;
    if (is_pt == 1 && stack->size - 1 > stack->capacity) {
        errors[ERR_SIZE_INVALID] = 1;
        stack->size--;
    }
    END
}