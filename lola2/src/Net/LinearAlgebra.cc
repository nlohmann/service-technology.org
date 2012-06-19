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
/// writes current row on cout
void Matrix::Row::DEBUG__printRow() const
{
    for (index_t i = 0; i < varCount; ++i)
    {
        std::cout << coefficients[i] << "*" << variables[i] << " ";
        //std::cout << coefficients[i] << "*" << Net::Name[PL][variables[i]] << " ";
    }
    std::cout << "[" << reference << "]";
    std::cout << std::endl;
    /*
    for (index_t i = 0; i < varCount; ++i) {
       //std::cout << coefficients[i] << "*" << variables[i] << " ";
       std::cout << coefficients[i] << "*" << Net::Name[PL][variables[i]] << " ";
    }
    std::cout << std::endl;
    */
}
/// writes current matrix on cout
void Matrix::DEBUG__printMatrix() const
{
    for (index_t c = 0; c < colCount; ++c)
    {
        std::cout << "column " << c << endl;
        Row* curRow = matrix[c];
        while (curRow != NULL)
        {
            curRow->DEBUG__printRow();
            curRow = curRow->next;
        }
    }
}
// LCOV_EXCL_STOP

/// checks whether the reduced matrix has at most one row per column
bool Matrix::DEBUG__checkReduced() const
{
    // for each variable i (=column)
    for (index_t i = 0; i < colCount; ++i)
    {
        if (matrix[i] != NULL)
        {
            assert(matrix[i]->next == NULL);
        }
    }
    return true;
}

/// checks whether the variables in a row a ordered properly
bool Matrix::Row::DEBUG__checkRow() const
{
    // for each variable i
    index_t leftElem = 0;
    for (index_t i = 0; i < varCount; ++i)
    {
        assert(variables[i] >= leftElem);
        if (i > 0)
        {
            assert(variables[i] != leftElem);
        }
        leftElem = variables[i];
    }
    return true;
}

/// frees memory of current row
Matrix::Row::~Row()
{
    free(variables);
    free(coefficients);
}

/// creates a new row based on LinearAlgebra.h types
Matrix::Row::Row(index_t length, const index_t* var, const int64_t* coef, index_t ref) : varCount(length), next(NULL), reference(ref)
{
    // request memory for new row
    variables = (index_t*) malloc(length * SIZEOF_INDEX_T);
    // coefficients are stored as int64_t
    coefficients = (int64_t*) malloc(length * SIZEOF_INT64_T);

    // memcpy is used because given and new memory has the same types
    memcpy(variables, var, length * SIZEOF_INDEX_T);
    memcpy(coefficients, coef, length * SIZEOF_INT64_T);

    assert(DEBUG__checkRow());
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
    int64_t* newCoef = (int64_t*) calloc((varCount + next->varCount), SIZEOF_INT64_T);
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

    // save current reference of second row
    const index_t curReference = this->next->reference;
    // delete second row
    matrix.deleteRow(this);

    // create new row based on new arrays
    if (newSize != 0)
    {
        assert(newVar[0] > variables[0]);
        matrix.addRow(newSize, newVar, newCoef, curReference);
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
void Matrix::addRow(index_t length, const index_t* var, const int64_t* coef, index_t ref)
{
    // if new row contains no variables, do nothing
    if (length == 0)
    {
        return;
    }

    // create new row based on given data
    Row* row = new Row(length, var, coef, ref);

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
    assert(DEBUG__checkReduced());
}

/// Returns number of rows
index_t Matrix::getRowCount() const
{
    return rowCount;
}

/// Returns true iff a column with given index is significant
bool Matrix::isSignificant(index_t column) const
{
    assert(column < colCount);
    return (matrix[column] != NULL);
}

/// Returns reference number of first row with given
index_t Matrix::getReference(index_t column) const
{
    assert(column < colCount);
    return (matrix[column]->reference);
}

/// Returns row of the first row with given index
Matrix::Row* Matrix::getRow(index_t column) const
{
    assert(column < colCount);
    return matrix[column];
}

/// Returns the number of significant (= not empty) columns
index_t Matrix::getSignificantColCount() const
{
    assert(significantColCount <= colCount);
    return significantColCount;
}
