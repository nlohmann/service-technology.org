#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* a type for indices */
typedef int index_t;

/* a type for values */
typedef long double value_t;


/* print the matrix */
void printM(value_t** M, index_t ROWS, index_t COLS) {
    index_t i, j;

    for (i = 0; i < ROWS; ++i) {
        printf("[");
        for (j = 0; j < COLS; ++j) {
            printf("%10.0Lf  ", M[i][j]);
        }
        printf("]\n");
    }
}


/* calculate a 2x2 subdeterminant */
#define det(M, i, j) (M[0][0] * M[i][j]) - (M[i][0] * M[0][j])


/* manipulate matrix to ensure that a(1,1) is not zero */
inline void prep(value_t** M, index_t* ROWS, index_t* COLS) {
    index_t i, j;

    /* check if we're done */
    if (M[0][0] != 0 || *ROWS == 1 || *COLS == 1) {
        return;
    }

    /* find row with nonzero first entry */
    i = 0;
    while (++i < *ROWS && M[i][0] == 0);

    /* check if we found a row */
    if (i == *ROWS) {
        /* copy last column to front */
        printf("move last column to front\n");
        #pragma omp parallel for private(i)
        for (i = 0; i < *ROWS; ++i) {
            M[i][0] = M[i][*ROWS-1];
        }
        /* decrease column count */
        --(*COLS);

        /* recursive call */
        prep(M, ROWS, COLS);
    } else {
        /* swap this row to front */
        printf("swap row %d to front\n", i);
        #pragma omp parallel for private(j)
        for (j = 0; j < *COLS; ++j) {
            if (M[0][j] != M[i][j]) {
                value_t temp = M[0][j];
                M[0][j] = M[i][j];
                M[i][j] = temp;
            }
        }
    }
}


/* calculate the rank of matrix M */
index_t rank(value_t** M, index_t ROWS, index_t COLS) {
    index_t i, j;
    static value_t** N1;
    static value_t** N2;
    value_t** this;
    value_t** next;
    static index_t iteration = 0;

    if (iteration == 0) {
        /* initialize helper matrices on first iteration */
        N1 = (value_t**)malloc(ROWS * sizeof(value_t*));
        N2 = (value_t**)malloc(ROWS * sizeof(value_t*));
        for (i = 0; i < ROWS; ++i) {
            N1[i] = (value_t*)malloc(COLS * sizeof(value_t));
            N2[i] = (value_t*)malloc(COLS * sizeof(value_t));
        }

        /* copy input matrix (should be memcpy) */
        #pragma omp parallel for private(i,j)
        for (i = 0; i < ROWS; ++i) {
            for (j = 0; j < COLS; ++j) {
                N1[i][j] = M[i][j];
            }
        }
    }

    /* set helper matrices */
    if (iteration % 2 == 0) {
        this = N1;
        next = N2;
    } else {
        this = N2;
        next = N1;
    }

    /* special case: vector => rank is only 0 for null vector */
    if (ROWS == 1 || COLS == 1) {
        for (i = 0; i < ROWS; ++i) {
            for (j = 0; j < COLS; ++j) {
                if (this[i][j] != 0) {
                    return 1;
                }
            }
        }
        return 0;
    } else {
        /* process matrix to ensure a(1,1) != 0 */
        prep(this, &ROWS, &COLS);

        /* reduce problem to subdeterminant calculation */
        #pragma omp parallel for private(i)
        for (i = 0; i < ROWS - 1; ++i) {
            for (j = 0; j < COLS - 1; ++j) {
                next[i][j] = det(this, i + 1, j + 1);
            }
        }

        /* recursive call */
        ++iteration;
        return (1 + rank(M, ROWS - 1, COLS - 1));
    }
}


int main() {
    value_t** M;
    index_t i, j;
    index_t ROWS = 500;
    index_t COLS = 500;

    srand(time(NULL));
    M = (value_t**)malloc(ROWS * sizeof(value_t*));
    #pragma omp parallel for private(i)
    for (i = 0; i < ROWS; ++i) {
        M[i] = (value_t*)malloc(COLS * sizeof(value_t));
        for (j = 0; j < COLS; ++j) {
            M[i][j] = rand() % 4 + 1 - 2;
        }
    }

    printf("rank = %d\n", rank(M, ROWS, COLS));
    return 0;
}
