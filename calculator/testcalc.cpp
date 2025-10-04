#include <stdio.h>

#include "../stack/stack.h"
#include "calc.h"

int main(int argc, char *argv[]) {
    if (argc > 1) {
        calcErr_s error = check_and_execute_flags(argc, argv);
        if(error.is_error == 1) {
            print_errors(error);
            return 1;
        }
    }
    return 0;
}