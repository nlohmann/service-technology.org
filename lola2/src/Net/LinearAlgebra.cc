/*!
\file LinearAlgebra.cc
\author Andreas
\status approved 11.04.2012

\todo finalize overflow handling
*/

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <Net/LinearAlgebra.h>

using namespace std;

// LCOV_EXCL_START
#include <iostream>
using std::cout;
using std::endl;
/// writes current row on cout
void Matrix::Row::DEBUG__printRow() const
{
    for (index_t i = 0; i < varCount; ++i)
    {
        cout << coefficients[i] << "*" << variables[i] << " ";
        //cout << coefficients[i] << "*" << Net::Name[PL][variables[i]] << " ";
    }
    cout << endl;
    /*
    for (index_t i = 0; i < varCount; ++i) {
       //cout << coefficients[i] << "*" << variables[i] << " ";
       cout << coefficients[i] << "*" << Net::Name[PL][variables[i]] << " ";
    }
    cout << endl;
    */
}
/// writes current matrix on cout
void Matrix::DEBUG__printMatrix() const
{
    for (index_t c = 0; c < colCount; ++c)
    {
        cout << "place " << c << endl;
        Row* curRow = matrix[c];
        while (curRow != NULL)
        {
            curRow->DEBUG__printRow();
            curRow = curRow->next;
        }
    }
}
// LCOV_EXCL_STOP

/// computes the ggt of two unsigned integers
inline int64_t ggt(int64_t a, int64_t b)
{
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

/// frees memory of current row
Matrix::Row::~Row()
{
    free(variables);
    free(coefficients);
}

/// creates a new row based on LinearAlgebra.h types
Matrix::Row::Row(index_t length, const index_t* var, const int64_t* coef)
{
    varCount = length;
    next = NULL;

    // request memory for new row
    variables = (index_t*) malloc(length * SIZEOF_INDEX_T);
    // coefficients are stored as int64_t
    coefficients = (int64_t*) malloc(length * sizeof(int64_t));

    // memcpy is used because given and new memory has the same types
    memcpy(variables, var, length * SIZEOF_INDEX_T);
    memcpy(coefficients, coef, length * sizeof(int64_t));
}

/// eleminates the first variable on the second row of the first variable
void Matrix::Row::apply(Matrix &matrix)
{
    // the variable to be eliminated should be the same
    assert(variables[0] == next->variables[0]);
    assert(this != next);

    // calculate correctur factors
    int64_t ggtFactor = ggt(coefficients[0], next->coefficients[0]);
    const int64_t firstRowFactor = next->coefficients[0] / ggtFactor;
    const int64_t secondRowFactor = coefficients[0] / ggtFactor;

    // get some space for the new row (secondRow - firstRow)
    // at most |firstRow| + |secondRow| elements are neccessary
    // one less is also suitable
    index_t* newVar = (index_t*) calloc((varCount + next->varCount), SIZEOF_INDEX_T);
    int64_t* newCoef = (int64_t*) calloc((varCount + next->varCount), sizeof(int64_t));
    index_t newSize = 0;

    // start with the second element, because the first one is to be ruled out
    index_t firstRow = 1;
    index_t secondRow = 1;

    // as long as there are some "common" variables left
    while ((firstRow < varCount) && (secondRow < next->varCount))
    {
        // at least one element in both rows is left
        if (variables[firstRow] < next->variables[secondRow])
        {
            // the one in the first row has the smaller index
            newVar[newSize] = variables[firstRow];
            // new coefficient is (-1) * rowOne * factorOne
            newCoef[newSize] = safeMult(-firstRowFactor, coefficients[firstRow]);
            // goto next element
            firstRow++;
        }
        else if (variables[firstRow] > next->variables[secondRow])
        {
            // the one in the second row has the smaller index
            newVar[newSize] = next->variables[secondRow];
            // new coefficient is rowTwo * FactorTwo
            newCoef[newSize] = safeMult(secondRowFactor, next->coefficients[secondRow]);
            // goto next element
            secondRow++;
        }
        else
        {
            // it's the same index, so calculate new coefficient
            newVar[newSize] = variables[firstRow];
            // new coefficient is (rowTwo * factorTwo) - (rowOne * factorOne)
            newCoef[newSize] = safeMult(secondRowFactor, next->coefficients[secondRow]);
            newCoef[newSize] -= safeMult(firstRowFactor, coefficients[firstRow]);
            // new coefficient may be 0
            if (newCoef[newSize] == 0)
            {
                // decrease newSize
                newSize--;
                // assumption: decreasing 0 will lead to maxInt but
                //              upcoming increase will result in 0 again
            }
            // goto next elements
            firstRow++;
            secondRow++;
        }
        // increase newSize
        newSize++;
    }
    // all "common" elements are processed

    while (firstRow < varCount)
    {
        // first row has more elements as second row
        newVar[newSize] = variables[firstRow];
        // new coefficient is (-1) * rowOne * factorOne
        newCoef[newSize] = safeMult(-firstRowFactor, coefficients[firstRow]);

        // goto next elements
        newSize++;
        firstRow++;
    }

    while (secondRow < next->varCount)
    {
        // second row has more elements as first row
        newVar[newSize] = next->variables[secondRow];
        // new coefficient is rowTwo * FactorTwo
        newCoef[newSize] = safeMult(secondRowFactor, next->coefficients[secondRow]);

        // goto next elements
        newSize++;
        secondRow++;
    }

    // new row has been calculated
    // decrease coefficients
    // calculate ggt of new row

    ggtFactor = newCoef[0];
    for (index_t i = 1; i < newSize; ++i)
    {
        ggtFactor = ggt(ggtFactor, newCoef[i]);
    }
    // use new ggt for new row
    for (index_t i = 0; i < newSize; ++i)
    {
        newCoef[i] /= ggtFactor;
    }

    // delete second row
    matrix.deleteRow(this);

    // create new row based on new arrays
    if (newSize != 0)
    {
        matrix.addRow(newSize, newVar, newCoef);
    }
    // free memory of the new row (data is already processed)
    free(newVar);
    free(newCoef);
}

/// frees memory of current matrix
Matrix::~Matrix()
{
    for (index_t c = 0; c < colCount; ++c)
    {
        Row* curRow = matrix[c];
        while (curRow != NULL)
        {
            // save current row
            Row* toDelete = curRow;
            // set next row (successor of current row)
            curRow = curRow->next;
            // delete current row
            delete toDelete;
            --rowCount;
        }
        matrix[c] = NULL;
    }
    // delete array for variables (and their rows)
    delete[] matrix;
}

/// creates a new matrix
/// size depicts the number of variables (=columns)
Matrix::Matrix(index_t size) : rowCount(0), colCount(size), significantColCount(0)
{
    matrix = new Row*[size];

    for (index_t i = 0; i < size; ++i)
    {
        matrix[i] = NULL;
    }
}

/// adds a row to the current matrix
void Matrix::addRow(index_t length, const index_t* var, const int64_t* coef)
{
    // if new row contains no variables, do nothing
    if (length == 0)
    {
        return;
    }

    // create new row based on given data
    Row* row = new Row(length, var, coef);

    // insert new row at right position
    row->next = matrix[row->variables[0]];
    matrix[row->variables[0]] = row;

    // increase rowCount
    ++rowCount;
}

/// deletes the successor ot the given row in the current matrix
void Matrix::deleteRow(Row* row)
{
    // if row or its successor is NULL, do nothing
    if (row == NULL or row->next == NULL)
    {
        return;
    }

    // set successor to successors sucessor
    Row* tmp = row->next;
    row->next = row->next->next;

    // delete successor
    delete tmp;

    // decrease rowCount
    --rowCount;
}


/// reduces the current matrix to triangular form
void Matrix::reduce()
{
    // if there no rows, do nothing
    if (rowCount == 0)
    {
        return;
    }

    // for each variable i (=column)
    for (index_t i = 0; i < colCount; ++i)
    {
        if (matrix[i] != NULL)
        {
            significantColCount++;
        }
        // if there at least two rows with variable i as first variable
        while ((matrix[i] != NULL) && (matrix[i]->next != NULL))
        {
            // get rid of the second row
            matrix[i]->apply(*this);
        }
    }
}

/// Returns true iff place with given index is significant
bool Matrix::isSignificant(index_t place) const
{
    assert(place < colCount);
    return (matrix[place] != NULL);
}

/// Returns the number of significant (= not empty) columns
index_t Matrix::getSignificantColCount() const
{
    assert(significantColCount <= colCount);
    return significantColCount;
}
