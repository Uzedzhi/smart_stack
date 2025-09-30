#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ======== Универсальная структура ======== */
typedef struct {
    void *data;        // массив элементов
    size_t top;        // индекс вершины
    size_t capacity;   // вместимость
    size_t elem_size;  // размер одного элемента
} Stack;

/* ======== Единые функции ======== */

void stack_init(Stack *s, size_t capacity, size_t elem_size) {
    s->data = malloc(elem_size * capacity);
    s->top = 0;
    s->capacity = capacity;
    s->elem_size = elem_size;
}

void stack_free(Stack *s) {
    free(s->data);
    s->data = NULL;
    s->top = 0;
    s->capacity = 0;
    s->elem_size = 0;
}

int stack_push(Stack *s, void *value) {
    memcpy((char*)s->data + s->top++ * s->elem_size, value, s->elem_size);
    return 1;
}

union un{
    int i;
    void * c;
};

    #define typename(x) _Generic((x),                                                 \
            _Bool: _Bool,                  unsigned char: unsigned char,          \
             char: char,                     signed char: signed char,            \
        short int: short int,         unsigned short int: unsigned short int,     \
              int: int,                     unsigned int: unsigned int,           \
         long int: long int,           unsigned long int: unsigned long int,      \
    long long int: long long int, unsigned long long int: unsigned long long int, \
            float: float,                         double: double,                 \
      long double: long double,\
          default: long double)

int stack_pop(Stack *s, void *out) {
    s->top--;
    memcpy(out, (char*)s->data + s->top * s->elem_size, s->elem_size);
    return 1;
}

#define STACK_INIT(stack, capacity, TYPE)\
    stack_init((stack), (capacity), sizeof(TYPE));

#define STACK_PUSH(stack, value) \
    typename(value) val = value;\
    stack_push(stack, (void *)val);

#define STACK_POP(stack, out) \
    stack_pop((stack), (out));