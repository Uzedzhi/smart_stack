#include <stdio.h>
#include "stack.hpp"

int main(void) {
#ifdef _STK_STR_TYPE
    stack_t *stack = StackCtor();

    char buf[256] = {};
    for (size_t i = 0; i < 100; i++) {
        snprintf(buf, 255, "push number %d", i);
        push(stack, buf);
        printf("pushed %s\n", buf);
    }

    stackDump(stack);

    char * ptr;
    for (size_t i = 0; i < 97; i++) {
        pop(stack, &ptr);
        printf("value: %s\n", ptr);
        free(ptr);
    }
    stackDump(stack);

    stackDtor(stack);
#else
#ifdef _STK_FLT_TYPE
    stack_t *stack = StackCtor();

    double value = 0;
    for (size_t i = 0; i < 100; i++) {
        push(stack, (double) i + 0.6);
        printf("pushed %lf\n", value);
    }

    stackDump(stack);

    for (size_t i = 0; i < 100; i++) {
        pop(stack, &value);
        printf("value: %lf\n", value);
    }
        pop(stack, &value);
        pop(stack, &value);
    stackDump(stack);

    stackDtor(stack);
#else
    stack_t *stack = StackCtor();

    int value = 0;
    for (size_t i = 0; i < 100; i++) {
        push(stack, i);
        printf("pushed %d\n", value);
    }

    stackDump(stack);

    for (size_t i = 0; i < 100; i++) {
        pop(stack, &value);
        printf("value: %d\n", value);
    }
    stackDump(stack);

    stackDtor(stack);
#endif
#endif
    return 0;
}