/*!
\author Andre
\file LinkedList.h
\status new

\brief A singly-linked list providing in-place embedding of lists.
*/


#pragma once
#include <assert.h>
#include <stdlib.h>

template <typename T>
class Cons
{
protected:
    bool discarded;
    T content;
public:
    Cons();
    Cons<T> *next;
    void deallocate();
    void discard();
    T getContent();
    bool isLast();
    Cons<T> *insert(T);
};

template <typename T>
Cons<T>::Cons()
{
    discarded = false;
    next = NULL;
}

template <typename T>
T Cons<T>::getContent()
{
    return content;
}

template <typename T>
void Cons<T>::discard()
{
    discarded = true;
}

template <typename T>
void Cons<T>::deallocate()
{
    if (!isLast())
    {
        next->deallocate();
        delete next;
    }
}

template <typename T>
Cons<T> *Cons<T>::insert(T t)
{
    if (discarded)
    {
        discarded = false;
        content = t;
        return this;
    }

    Cons<T> *cons = new Cons<T>();
    cons->content = t;
    cons->next = next;
    if (next != NULL && next->next == this)
    {
        next->next = cons;
    }
    next = cons;
    return cons;
}

template <typename T>
bool Cons<T>::isLast()
{
    return (next == NULL || (next->next != NULL && next->next == this));
}


/*!
\brief A singly-linked list which has cells in which entire lists can be embedded.

*/
template <typename T>
class LinkedList
{
private:
    Cons<T> *head; /// dummy element (which points to the first element)
    Cons<T> *last; /// dummy element (which points to the last element)
    Cons<T> *current; // points to the current object
public:
    LinkedList();
    LinkedList<T> copy();

    void deallocate();

    void append(LinkedList<T>);
    void push(T);
    void enqueue(T);

    void reset();
    void end();
    bool next();
    bool hasNext();
    bool isValid();

    Cons<T> *getCons();
    bool isEmpty();
    unsigned int size();
    T *toArray();
};

template <typename T>
LinkedList<T>::LinkedList()
{
    head = new Cons<T>();
    last = new Cons<T>();
    head->next = last;
    last->next = head;
    current = last;
}

template <typename T>
LinkedList<T> LinkedList<T>::copy()
{
    LinkedList<T> list = LinkedList<T>();
    for (reset(); isValid(); next())
    {
        list.enqueue(getCons()->getContent());
    }
    return list;
}

template <typename T>
void LinkedList<T>::deallocate()
{
    if (!isEmpty())
    {
        head->deallocate();
        head->next = last;
        last->next = head;
    }
}

template <typename T>
void LinkedList<T>::append(LinkedList<T> list)
{
    for (list.reset(); list.isValid(); list.next())
    {
        enqueue(list.getCons()->getContent());
    }
}

template <typename T>
void LinkedList<T>::push(T t)
{
    head->next = head->insert(t);
}

template <typename T>
void LinkedList<T>::enqueue(T t)
{
    last->next = last->next->insert(t);
}

template <typename T>
void LinkedList<T>::reset()
{
    current = head->next;
}

template <typename T>
void LinkedList<T>::end()
{
    current = last->next;
}

template <typename T>
bool LinkedList<T>::next()
{
    if (isValid())
    {
        current = current->next;
    }
    return isValid();
}

template <typename T>
bool LinkedList<T>::hasNext()
{
    assert(current);
    return (current->next->next != current);
}


template <typename T>
bool LinkedList<T>::isValid()
{
    assert(current);
    return (current != head && current != last);
}

template <typename T>
Cons<T> *LinkedList<T>::getCons()
{
    return (isValid() ? current : NULL);
}

template <typename T>
bool LinkedList<T>::isEmpty()
{
    return (last->next == head);
}

template <typename T>
unsigned int LinkedList<T>::size()
{
    unsigned int n = 0;
    for (reset(); isValid(); next(), n++);
    return n;
}


template <typename T>
T *LinkedList<T>::toArray()
{
    int i = 0;
    T *array = new T[size()];
    for (reset(); isValid(); next(), i++)
    {
        array[i] = getCons()->getContent();
    }
    return array;
}
