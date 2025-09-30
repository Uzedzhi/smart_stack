#include <stdio.h>
#include <limits.h>
#include <math.h>

bool check_if_overflow(size_t first, size_t second) {
    printf("%zu %zu\n", first, second);
    return first > 0 && second > SIZE_MAX - first;
}

size_t mod_pow(size_t base, size_t exp) {
    size_t result = 1;
    while (exp > 0) {
        result = (size_t) result * base; 
        exp--;
    }
    return result;
}

int main(void) {
    printf("%zu\n", mod_pow(24, 25));
    return 0;
}