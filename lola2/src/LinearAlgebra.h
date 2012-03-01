/*!
\file LinearAlgebra.h
\author Andreas
\status new


*/

#pragma once

#include "Dimensions.h"

/*!
A matrix is used to store variables (=columns) and their equations (=rows).
Using places (transitions) as variables and their effect to the net as rows, the reduce() method can find the significant places (transitions).
*/
class Matrix
{
    public: 
        /// Generate and initialize a matrix
        explicit Matrix(index_t);
        /// Delete a matrix
        ~Matrix();

        /// Add a new row to the matrix
        void addRow(index_t, const index_t*, const mult_t*);
        
        /// Generate the triangular form of the matrix (original one gets lost)
        void reduce();

        /// Write matrix to cout (for debug purposes)
        void printMatrix() const;

    private:
        /*!
        A row is used to store a row.
        */
        class Row
        {
            public:
                /// Generate and initialize a row based on Net.h types
                Row(index_t, const index_t*, const mult_t*);
                /// Generate and initialize a row based on LinearAlgebra.h types
                Row(index_t, const index_t*, const uint64_t*);
                /// Delete a row
                ~Row();
                
                /// Eleminate current first variable of a row in its successor row (same first variable)
                void apply(Matrix&);
            
                /// Write row to cout (for debug purposes)
                void printRow() const;
            
                /// Number of variables in current row with non zero coefficients
                index_t varCount;
                /// Array of variable indizes in current row with non zero coefficients
                index_t* variables;
                /// Array of non zero coefficients in current row (same order as variables)
                uint64_t* coefficients;
                
                /// Pointer to successor row with same first variable (NULL if none present)
                Row* next;
        };
    
        /// Array of rows for number of varibales (columns)
        Row** matrix;
        /// Number of stored rows in current matrix
        index_t rowCount;
        /// Number of stored columns in current matrix
        index_t colCount;
};
