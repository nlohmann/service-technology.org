/*!
\file LinearAlgebra.cc
\author Andreas
\status new


*/
#include <cstdlib>
#include <cstring>

#include <iostream>
using std::cout;
using std::endl;

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "LinearAlgebra.h"
#include "Net.h"

/// computes the ggt of two unsigned integers
inline uint64_t ggt(uint64_t a, uint64_t b)
{
    while (true) {
        a %= b;
        if (!a) {
            return b;
        }
        b %= a;
        if (!b) {
            return a;
        }
    }
}

/// multiplies two unsigned intergers with respect to overflows
inline uint64_t safeMult(uint64_t a, uint64_t b)
{
    // overflow handling
    if (b > 0 && a > 18446744073709551615 / b) {
        assert(false);
    }
    return (a * b);
}

/// writes current row on cout
void Matrix::Row::printRow() const
{
    for (index_t i = 0; i < varCount; ++i) {
       cout << coefficients[i] << "*" << variables[i] << " ";
    }
    cout << endl;
}

/// frees memory of current row
Matrix::Row::~Row()
{
    free(variables);
    free(coefficients);
}

/// creates a new row based on Net.h types
Matrix::Row::Row(index_t length, const index_t* var, const mult_t* coef)
{
    varCount = length;
    next = NULL;
    
    // request memory for new row
    variables = (index_t*) malloc(length * SIZEOF_INDEX_T);
    // coefficients are stored as uint64_t
    coefficients = (uint64_t*) malloc(length * sizeof(uint64_t));
    
    // copy the given variables into new (own) memory
    memcpy(variables, var, length * SIZEOF_INDEX_T);
    // copy the given coefficients into new (own) memory
    // memcpy is no option because the data types are different
    for (index_t i = 0; i < length; ++i) {
        coefficients[i] = coef[i];
    }
}

/// creates a new row based on LinearAlgebra.h types
Matrix::Row::Row(index_t length, const index_t* var, const uint64_t* coef)
{
    varCount = length;
    next = NULL;
    
    // request memory for new row
    variables = (index_t*) malloc(length * SIZEOF_INDEX_T);
    // coefficients are stored as uint64_t
    coefficients = (uint64_t*) malloc(length * sizeof(uint64_t));
    
    // memcpy is used because given and new memory has the same types
    memcpy(variables, var, length * SIZEOF_INDEX_T);
    memcpy(coefficients, coef, length * sizeof(uint64_t));
}

/// eleminates the first variable on the second row of the first variable
void Matrix::Row::apply(Matrix& matrix)
{
    // the variable to be eliminated should be the same
    assert(variables[0] == next->variables[0]);
    assert(this != next);
    
    // calculate correctur factors
    uint64_t ggtFactor = ggt(coefficients[0], next->coefficients[0]);
    const uint64_t firstRowFactor = next->coefficients[0] / ggtFactor;
    const uint64_t secondRowFactor = coefficients[0] / ggtFactor;
    
    // get some space for the new row (secondRow - firstRow)
    // at most |firstRow| + |secondRow| elements are neccessary
    // one less is also suitable
    index_t* newVar = (index_t*) calloc((varCount + next->varCount), SIZEOF_INDEX_T);
    uint64_t* newCoef = (uint64_t*) calloc((varCount + next->varCount), sizeof(uint64_t));
    index_t newSize = 0;
    
    // start with the second element, because the first one is to be ruled out
    index_t firstRow = 1;
    index_t secondRow = 1;
    
    // as long as there are some "common" variables left
    while ((firstRow < varCount) && (secondRow < next->varCount)) {
        // at least one element in both rows is left
        if (variables[firstRow] < next->variables[secondRow]) {
            // the one in the first row has the smaller index
            newVar[newSize] = variables[firstRow];
            // new coefficient is (-1) * rowOne * factorOne  
            newCoef[newSize] = safeMult(-firstRowFactor, coefficients[firstRow]);
        }
        else if (variables[firstRow] > next->variables[secondRow]) {
            // the one in the second row has the smaller index
            newVar[newSize] = next->variables[secondRow];
            // new coefficient is rowTwo * FactorTwo
            newCoef[newSize] = safeMult(secondRowFactor, next->coefficients[secondRow]);
        }
        else {
            // it's the same index, so calculate new coefficient
            newVar[newSize] = variables[firstRow];
            // new coefficient is (rowTwo * factorTwo) - (rowOne * factorOne)
            newCoef[newSize] = safeMult(secondRowFactor, next->coefficients[secondRow]);
            newCoef[newSize] -= safeMult(firstRowFactor, coefficients[firstRow]);
            // new coefficient may be 0
            if (newCoef[newSize] == 0) {
                // decrease newSize
                newSize--;
                // assumption: decreasing 0 will lead to maxInt but
                //              upcoming increase will result in 0 again            
            }
        }
        
        // goto next elements
        newSize++;
        firstRow++;
        secondRow++;
    }
    // all "common" elements are processed 
   
    while (firstRow < varCount) {
        // first row has more elements as second row
        newVar[newSize] = variables[firstRow];
        // new coefficient is (-1) * rowOne * factorOne  
        newCoef[newSize] = safeMult(-firstRowFactor, coefficients[firstRow]);
        
        // goto next elements
        newSize++;
        firstRow++;
    }
    
    while (secondRow < next->varCount) {
        // second row has more elements as first row
        newVar[newSize] = next->variables[secondRow];
        // new coefficient is rowTwo * FactorTwo
        newCoef[newSize] = safeMult(secondRowFactor, next->coefficients[secondRow]);
        
        // goto next elements
        newSize++;
        secondRow++;
    }
    
    // maybe the new row is empty
    if (newSize == 0) {
        // new row is empty
        // process with next row (of current first variable)
        next = next->next;
        // free memory of the new (empty) row
        free(newVar);
        free(newCoef);
        // exit method
        return;
    }
    
    // new row has been calculated
    // decrease coefficients
    // calculate ggt of new row
    ggtFactor = newCoef[0];
    for (index_t i = 1; i < newSize; ++i) {
        ggtFactor = ggt(ggtFactor, newCoef[i]);
    }
    // use new ggt for new row
    for (index_t i = 0; i < newSize; ++i) {
       newCoef[i] /= ggtFactor;
    }
    
    // create new row based on new arrays
    Row* newRow = new Row(newSize, newVar, newCoef);
    // free memory of the new row (data is already processed)
    free(newVar);
    free(newCoef);
    
    // delete second row and add new row
    // process with next row (of current first variable)
    Row* tmp = next;
    next = next->next;
    // delete current second row
    delete tmp;
    // decrease rowCounter
    --matrix.rowCount;
    // insert new row at its right position
    newRow->next = matrix.matrix[newRow->variables[0]];
    matrix.matrix[newRow->variables[0]] = newRow;
}

/// frees memory of current matrix
Matrix::~Matrix()
{
    for (index_t c = 0; c < colCount; ++c) {
        Row* curRow = matrix[c];
        while (curRow) {
            // save current row
            Row* toDelete = curRow;
            // set next row (successor of current row)
            curRow = curRow->next;
            // delete current row
            delete toDelete;
        }
    }
    // delete array for variables (and their rows)
    delete[] matrix;
}

/// writes current matrix on cout
void Matrix::printMatrix() const
{
    for (index_t c = 0; c < colCount; ++c) {
        Row* curRow = matrix[c];
        while (curRow) {
            curRow->printRow();
            curRow = curRow->next; 
        }
    }
}

/// creats a new matrix
/// size depicts the number of variables (=columns)
Matrix::Matrix(index_t size)
{
    matrix = new Row*[size];
    rowCount = 0;
    colCount = size;
    
    for (index_t i = 0; i < size; ++i) {
        matrix[i] = NULL;
    }
}

/// adds a row to the current matrix with Net.h types
void Matrix::addRow(index_t length, const index_t* var, const mult_t* coef)
{
    // if new row contains no variables, do nothing
    if (length == 0) return;
    
    // create new row based on given data
    Row* row = new Row(length, var, coef);

    // insert new row at right position
    row->next = matrix[row->variables[0]];
    matrix[row->variables[0]] = row;
    
    // increase rowCount
    ++rowCount;
}

/// reduces the current matrix to triangular form
void Matrix::reduce()
{
    // if there no rows, do nothing
    if (rowCount == 0) {
        return;
    }

    // for each variable i (=column)
    for (index_t i = 0; i < colCount; ++i) {
        // if there at least two rows with variable i as first variable 
        while (matrix[i] && matrix[i]->next) {
            // get rid of the second row
            matrix[i]->apply(*this);
        }
    }
}
