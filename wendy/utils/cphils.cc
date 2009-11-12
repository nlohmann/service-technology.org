#include <cstdlib>
#include <cstdio>
#include <cstdlib>

int main(int argc, char **argv) {
    if (argc != 3 or !atoi(argv[1]) or !atoi(argv[2])) {
        fprintf(stderr, "error: call %s n k\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    unsigned int n = atoi(argv[1]);
    unsigned int k = atoi(argv[2]);
    
    printf("{ n=%d, k=%d }\n\n", n, k);
    
    printf("PLACE\n");
    printf("INTERNAL\n");
    for (unsigned int i = 0; i < n; ++i) {
        printf("  phil%d_id, phil%d_i1, phil%d_i2, phil%d_ea, ", i, i, i, i);
        if (i % 2 == 0) {
            printf("phil%d_i0,", i);
        }
        printf("\n");
    }
    printf("  ");
    for (unsigned int i = 0; i < n-1; ++i) {
        printf("fork_%d, ", i);
    }
    printf("fork_%d;\n", n-1);
    printf("SYNCHRONOUS\n");
    for (unsigned int i = 0; i < n-1; ++i) {
        printf("  tl_%d, in_%d, tr_%d, rn_%d,\n", i, i, i, i);
    }
    printf("  tl_%d, in_%d, tr_%d, rn_%d;\n", n-1, n-1, n-1, n-1);


    printf("\n");
    
    printf("INITIALMARKING\n  ");
    for (unsigned int i = 0; i < n; ++i) {
        printf("phil%d_id, ", i);
    }
    printf("\n  ");
    for (unsigned int i = 0; i < n-1; ++i) {
        printf("fork_%d, ", i);
    }
    printf("fork_%d;\n\n", n-1);

    printf("FINALMARKING\n  ");
    for (unsigned int i = 0; i < n; ++i) {
        printf("phil%d_id, ", i);
    }
    printf("\n  ");
    for (unsigned int i = 0; i < n-1; ++i) {
        printf("fork_%d, ", i);
    }
    printf("fork_%d;\n\n", n-1);
    
    for (unsigned int i = 0; i < n; ++i) {
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
        printf("CONSUME phil%d_i2:%d, fork_%d;\n", i, k, (i+1)%n);
        printf("PRODUCE phil%d_ea;\n", i);
        printf("SYNCHRONIZE tr_%d;\n\n", i);

        printf("TRANSITION phil%d_return\n", i);
        printf("CONSUME phil%d_ea;\n", i);
        printf("PRODUCE phil%d_id, fork_%d, fork_%d;\n", i, i, (i+1)%n);
        printf("SYNCHRONIZE rn_%d;\n\n", i);
    }
    
    return EXIT_SUCCESS;
}
