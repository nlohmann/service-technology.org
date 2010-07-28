#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    unsigned int i, n, k;

    if (argc == 2 && !strcmp(argv[1], "--version")) {
        printf("Dining philosophers generator\n");
        printf("  part of %s\n\n", PACKAGE_STRING);
        return EXIT_SUCCESS;
    }

    if (argc == 2 && !strcmp(argv[1], "--help")) {
        printf("call %s n k\n", argv[0]);
        printf("  n : number of philosophers\n");
        printf("  k : number of internal steps\n");
        return EXIT_SUCCESS;
    }

    if (argc != 3 || !atoi(argv[1]) || !atoi(argv[2])) {
        fprintf(stderr, "error: call %s n k\n", argv[0]);
        fprintf(stderr, "  n : number of philosophers\n");
        fprintf(stderr, "  k : number of internal steps\n");
        return EXIT_FAILURE;
    }

    n = atoi(argv[1]);
    k = atoi(argv[2]);

    printf("{ n=%d, k=%d }\n\n", n, k);

    printf("PLACE\n");
    printf("INTERNAL\n");
    for (i = 0; i < n; ++i) {
        printf("  phil%d_id, phil%d_i1, phil%d_i2, phil%d_ea, ", i, i, i, i);
        if (i % 2 == 0) {
            printf("phil%d_i0,", i);
        }
        printf("\n");
    }
    printf("  ");
    for (i = 0; i < n - 1; ++i) {
        printf("fork_%d, ", i);
    }
    printf("fork_%d;\n", n - 1);
    printf("SYNCHRONOUS\n");
    for (i = 0; i < n - 1; ++i) {
        printf("  tl_%d, in_%d, tr_%d, rn_%d,\n", i, i, i, i);
    }
    printf("  tl_%d, in_%d, tr_%d, rn_%d;\n", n - 1, n - 1, n - 1, n - 1);


    printf("\n");

    printf("INITIALMARKING\n  ");
    for (i = 0; i < n; ++i) {
        printf("phil%d_id, ", i);
    }
    printf("\n  ");
    for (i = 0; i < n - 1; ++i) {
        printf("fork_%d, ", i);
    }
    printf("fork_%d;\n\n", n - 1);

    printf("FINALMARKING\n  ");
    for (i = 0; i < n; ++i) {
        printf("phil%d_id, ", i);
    }
    printf("\n  ");
    for (i = 0; i < n - 1; ++i) {
        printf("fork_%d, ", i);
    }
    printf("fork_%d;\n\n", n - 1);

    for (i = 0; i < n; ++i) {
        printf("{ philosopher #%d }\n", i);
        if (i % 2 != 0) {
            printf("TRANSITION phil%d_takeleft\n", i);
            printf("CONSUME phil%d_id, fork_%d;\n", i, i);
            printf("PRODUCE phil%d_i1:%d;\n", i, k);
            printf("SYNCHRONIZE tl_%d;\n\n", i);
        } else {
            printf("TRANSITION phil%d_takeleft_silent\n", i);
            printf("CONSUME phil%d_id, fork_%d;\n", i, i);
            printf("PRODUCE phil%d_i0;\n\n", i);

            printf("TRANSITION phil%d_takeleft\n", i);
            printf("CONSUME phil%d_i0;\n", i);
            printf("PRODUCE phil%d_i1:%d;\n", i, k);
            printf("SYNCHRONIZE tl_%d;\n\n", i);
        }

        printf("TRANSITION phil%d_internal\n", i);
        printf("CONSUME phil%d_i1;\n", i);
        printf("PRODUCE phil%d_i2;\n", i);
        printf("SYNCHRONIZE in_%d;\n\n", i);

        printf("TRANSITION phil%d_takeright\n", i);
        printf("CONSUME phil%d_i2:%d, fork_%d;\n", i, k, (i + 1) % n);
        printf("PRODUCE phil%d_ea;\n", i);
        printf("SYNCHRONIZE tr_%d;\n\n", i);

        printf("TRANSITION phil%d_return\n", i);
        printf("CONSUME phil%d_ea;\n", i);
        printf("PRODUCE phil%d_id, fork_%d, fork_%d;\n", i, i, (i + 1) % n);
        printf("SYNCHRONIZE rn_%d;\n\n", i);
    }

    return EXIT_SUCCESS;
}
