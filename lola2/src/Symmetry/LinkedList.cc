/*!
\author Andre
\file LinkedList.cc
\status new

\brief
*/

#include <assert.h>
#include <iostream>

#include <Symmetry/LinkedList.h>


#ifdef MAIN_LINKED_LIST

template <typename T>
bool isSorted(LinkedList<T> list)
{
    if (list.isEmpty())
    {
        return true;
    }

    bool result = true;
    list.reset();
    T element = list.getCons()->getContent();
    for (list.next(); list.isValid(); list.next())
    {
        result &= list.getCons()->getContent() >= element;
        element = list.getCons()->getContent();
    }

    return result;
}

int main(int argc, char *argv[])
{
    LinkedList<int> l1 = LinkedList<int>();
    l1.enqueue(1);
    l1.enqueue(4);
    assert(isSorted(l1));

    LinkedList<int> l2 = LinkedList<int>();
    l2.enqueue(6);
    l2.push(5);
    l2.enqueue(7);
    assert(isSorted(l2));

    l1.append(l2);
    assert(isSorted(l1));
    int num = 0;
    for (l1.reset(); l1.isValid(); num++, l1.next());
    assert(num == 5);

    l1.reset();
    l1.getCons()->discard();
    l1.getCons()->insert(1)->insert(2)->insert(3);
    for (int i = 1; l1.isValid(); i++, l1.next())
    {
        assert(i == l1.getCons()->getContent());
    }

    LinkedList<int> l3 = l2.copy();
    l3.append(l1);
    for (l2.reset(), l3.reset(); l2.isValid() && l3.isValid(); l2.next(), l3.next());
    assert(!l2.isValid() && l3.isValid());

    return 0;
}
#endif
