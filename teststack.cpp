#include <stdio.h>

#include "stack.h"

int main() {
    //bool error1[] = {0, 0, 0, 0, 0, 0, 0, 0};

    {
    printf(MAGENTA "first test: 2 push 2 pop\n" WHITE);
    init_stack(stk, int, 5);

    int num = 10;
    puts("push 10");
    stackPush(stk, &num);
    puts("push 20");
    num = 20;
    stackPush(stk, &num);
    
    int out = 0;
    printf("pop 1: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    printf("pop 2: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    stackDtor(stk);

    puts(BYELLOW "test 1 done" WHITE);
    }
    {
    puts(MAGENTA "second test: 5 push, initial capacity 1" WHITE);
    init_stack(stk, int, 1);

    int num = 10;
    puts("push 10");
    stackPush(stk, &num);

    puts("push 20");
    num = 20;
    stackPush(stk, &num);

    puts("push 30");
    num = 30;
    stackPush(stk, &num);

    puts("push 40");
    num = 40;
    stackPush(stk, &num);

    puts("push 50");
    num = 50;
    stackPush(stk, &num);
    
    int out = 0;

    printf("pop 1: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    printf("pop 2: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    printf("pop 3: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    printf("pop 4: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    printf("pop 5: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    stackDtor(stk);

    puts(BYELLOW "test 2 done" WHITE);
    }
    {
    puts(MAGENTA "third test: 2 push 3 pop" WHITE);
    init_stack(stk, int, 5);

    int num = 10;
    puts("push 10");
    stackPush(stk, &num);

    puts("push 20");
    num = 20;
    stackPush(stk, &num);
    
    int out = 0;

    printf("pop 1: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    printf("pop 2: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    printf("pop 3: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    stackDtor(stk);

    puts(BYELLOW "test 3 done" WHITE);

    }
    {
    puts(MAGENTA "fourth test: 2 pop 2 push but NULLIFY stack" WHITE);
    init_stack(stk, int, 5);

    int num = 10;
    puts("push 10");
    stackPush(stk, &num);

    puts("push 20");
    num = 20;
    stackPush(stk, &num);

    stk = NULL;
    
    int out = 0;

    printf("pop 1: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    printf("pop 2: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    puts(BYELLOW "test 4 done" WHITE);
    }
    {
    puts(MAGENTA "fifth test: 2 pop 2 push but change smth in stack" WHITE);
    init_stack(stk, int, 5);

    int num = 10;
    puts("push 10");
    stackPush(stk, &num);

    *((char *) stk + 32) = 98;
    puts("push 20");
    num = 20;
    stackPush(stk, &num);

    stk = NULL;
    
    int out = 0;

    printf("pop 1: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    printf("pop 2: ");
    stackPop(stk, &out);
    printf("%d\n", out);

    puts(BYELLOW "test 5 done" WHITE);
    }
    return 0;
}
