/*!
\file LinearAlgebra.h
\author Andreas
\status approved 11.04.2012
*/

#pragma once
#include <Core/Dimensions.h>
#include <Net/Net.h>

#ifndef INDEX_T_MAX
#define INDEX_T_MAX 0xFFFFFFFF
#endif

/// computes the ggt of two unsigned integers
inline int64_t ggt(int64_t a, int64_t b)
{
    assert(b != 0);
    while (true)
    {
        a %= b;
        if (!a)
        {
            return b;
        }
        b %= a;
        if (!b)
        {
            return a;
        }
    }
}

/// multiplies two unsigned intergers with respect to overflows
inline int64_t safeMult(int64_t a, int64_t b)
{
    //overflow handling
    ///\todo overflow handling
    ///\todo Add a URL where this magic numbers come from
    if (b > 0 && a > 9223372036854775807 / b)
    {
        assert(false);
    }

    return (a * b);
}

/*!
A matrix is used to store variables (=columns) and their equations (=rows).
Using places (transitions) as variables and their effect to the net as rows,
the reduce() method can find the significant places (transitions).
*/
class Matrix
{
public:
    /// A row is used to store a row.
    class Row
    {
        friend class Matrix;
        friend void Net::setProgressMeasure();

    public:
        /// Generate and initialize a row based on Net.h types
        Row(index_t, const index_t*, const int64_t*, index_t = 0);
        /// Delete a row
        ~Row();

        /// Eleminate current first variable of a row in its successor row (same first variable)
        void apply(Matrix &, index_t rowToChange = INDEX_T_MAX);

        /// Write row to cout
        void DEBUG__printRow() const;

        /// Checks whether the variables are ordered properly
        bool DEBUG__checkRow() const;

    private:
        /// Number of variables in current row with non zero coefficients
        index_t varCount;
        /// Array of variable indizes in current row with non zero coefficients
        index_t* variables;
        /// Array of non zero coefficients in current row (same order as variables)
        int64_t* coefficients;

        /// Reference number of current row
        index_t reference;

        /// Pointer to successor row with same first variable (NULL if none present)
        Row* next;
    };
private:
    /// Array of rows for number of varibales (columns)
    Row** matrix;
    /// Number of stored rows in current matrix
    index_t rowCount;
    /// Number of stored columns in current matrix
    index_t colCount;
    /// Number of non empty columns
    index_t significantColCount;

    bool DEBUG__checkReduced() const;

public:
    /// Generate and initialize a matrix
    explicit Matrix(index_t);
    /// Delete a matrix
    ~Matrix();

    /// Add a new row to the matrix
    void addRow(index_t, const index_t*, const int64_t*, index_t = 0);

    /// Delete the successor of a row in the matrix
    void deleteRow(Row*);

    /// Generate the triangular form of the matrix (original one gets lost)
    void reduce();

    /// Approach the diagonal form of the matrix (with a remainder in linear dependent columns)
    void diagonalise();

    /// Returns true iff a column with given index is significant
    bool isSignificant(index_t) const;

    /// Returns reference number of first row with given index
    index_t getReference(index_t) const;

    /// Returns row of the first row with given index
    Row* getRow(index_t) const;

    /// Returns number of rows
    index_t getRowCount() const;

    /// Returns the number of significant (= not empty) columns
    index_t getSignificantColCount() const;

    /// Write matrix to cout
    void DEBUG__printMatrix() const;
};
