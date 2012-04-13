/*!
\file LinearAlgebra.h
\author Andreas
\status approved 11.04.2012
*/

#pragma once
#include "Core/Dimensions.h"


/*!
A matrix is used to store variables (=columns) and their equations (=rows).
Using places (transitions) as variables and their effect to the net as rows,
the reduce() method can find the significant places (transitions).
*/
class Matrix
{
    private:
        /// A row is used to store a row.
        class Row
        {
            public:
                /// Generate and initialize a row based on Net.h types
                Row(index_t, const index_t*, const int64_t*);
                /// Delete a row
                ~Row();

                /// Eleminate current first variable of a row in its successor row (same first variable)
                void apply(Matrix &);

                /// Write row to cout (for debug purposes)
                void printRow() const;

                /// Number of variables in current row with non zero coefficients
                index_t varCount;
                /// Array of variable indizes in current row with non zero coefficients
                index_t* variables;
                /// Array of non zero coefficients in current row (same order as variables)
                int64_t* coefficients;

                /// Pointer to successor row with same first variable (NULL if none present)
                Row* next;
        };

        /// Array of rows for number of varibales (columns)
        Row** matrix;
        /// Number of stored rows in current matrix
        index_t rowCount;
        /// Number of stored columns in current matrix
        index_t colCount;

    public:
        /// Generate and initialize a matrix
        explicit Matrix(index_t);
        /// Delete a matrix
        ~Matrix();

        /// Add a new row to the matrix
        void addRow(index_t, const index_t*, const int64_t*);

        /// Delete the successor of a row in the matrix
        void deleteRow(Row*);

        /// Generate the triangular form of the matrix (original one gets lost)
        void reduce();

        /// Write matrix to cout (for debug purposes)
        void printMatrix() const;

        /// Returns true iff place with given index is significant
        bool isSignificant(index_t) const;
};
