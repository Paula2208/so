#include <stdio.h>
#include <stdlib.h>

double leibniz(long start, long end) {
    double sum = 0.0;
    for (long k = start; k < end; k++) {
        sum += ((k % 2 == 0) ? 1.0 : -1.0) / (2 * k + 1);
    }
    return sum;
}

int main() {
    long N = 1e9;

    double pi = leibniz(0, N);

    printf("PI ≈ %.15f\n", 4 * pi);
    return 0;
}
