/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.

 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*
 * NOTE: This file should not be called SList.h because the GNU C++ Compiler
 * comes with a file of the same name.
 */

/*!
 * \file    SinglyLinkedList.h
 *
 * \brief   A singly linked list.
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H

#include <cstdlib> // NULL
#include <stdexcept>

// This file contains _template_ classes. That is why they are also implemented
// here.


// Declare SList forward, because SListIterator needs it.
template<typename T> class SList;


// Item of a SList. Contains the actual data and a pointer to the next item in
// the SList. The data is called 'element'.
template<typename T> class SListItem {
    private:
        /**
         * Holds the actual data of this SListItem.
         */
        T element_;

        /**
         * Points to the next SListItem in a SList.
         */
        SListItem<T>* next_;

    public:
        /**
         * Constructs a SListItem, that holds the given 'element' and points to no
         * next SListItem.
         */
        SListItem(const T& element) :
            element_(element), next_(NULL) {
        }

        /**
         * Returns the 'element' (data) of this SListItem.
         */
        T getElement() const {
            return element_;
        }

        /**
         * Returns the next SListItem after this SListItem in a SList.
         */
        SListItem* getNext() const {
            return next_;
        }

        /**
         * Sets the next SListItem after this SListItem in a SList.
         */
        void setNext(SListItem<T>* next) {
            next_ = next;
        }
};


/**
 * A SListIterator is used to traverse a SList. This iterator allows
 * modification of the underlying SList, e.g., removing an element.
 */
template<typename T> class SListIterator {
    private:
        /**
         * Points to the SList this SListIterator belongs.
         */
        SList<T>* list_;

        /**
         * Points to the SListItem that will be returned at the next call to
         * getNext().
         */
        SListItem<T>* next_;

        /**
         * Points to the SListItem before 'next_' in 'list_'. Is NULL if there is
         * none. Needed for remove().
         */
        SListItem<T>* preNext_;

        /**
         * Points to the SListItem before 'PreNext_' in 'list_'. Is NULL if there
         * is none. Needed for remove().
         */
        SListItem<T>* prePreNext_;

    public:

        /**
         * Constructs a SListIterator that belongs to 'list'. It points to the
         * first item of 'list'.
         */
        SListIterator(SList<T>* list) :
            list_(list), next_(list->first_), preNext_(NULL), prePreNext_(NULL) {
        }

        /**
         * Adds 'element' to the end of this SList. Complexity is O(1).
         */
        void add(const T& element) {
            // in case we are at the end of the list, but a new element is added
            // then we have to adjust the iterator's next element
            if (next_ == NULL) {
                next_ = list_->add(element);
            } else {
                list_->add(element);
            }
        }

        /**
         * Returns true iff there is a next element in the corresponding SList. If
         * there is, it can be obtained by a call to getNext().
         */
        bool hasNext() {
            return next_ != NULL;
        }

        /**
         * Returns the next element in the corresponding SList. May only be called
         * once after a call to hasNext() returned true.
         */
        T getNext() {
            T elementAtNext = next_->getElement();

            if (next_ == list_->first_) {
                // Case: The iterator points to the first element in the SList.
                // So update next_ and initialize preNext_.
                prePreNext_ = NULL;
                preNext_ = next_;
                next_ = next_->getNext();
            } else if (next_ == list_->first_->getNext()) {
                // Case: The iterator points to the second element in the SList.
                // So update next_ and preNext_, and initialize prePreNext_.
                prePreNext_ = list_->first_;
                preNext_ = next_;
                next_ = next_->getNext();
            } else {
                // Case: The iterator points at least to the third element. So
                // next_, preNext_ and prePreNext_ are already initialized.

                // Move prePreNext_ only if there was no call to remove() after the
                // last call to getNext().
                if (prePreNext_->getNext() != next_) {
                    prePreNext_ = prePreNext_->getNext();
                }

                // Move preNext_ and next_.
                preNext_ = next_;
                next_ = next_->getNext();
            }

            return elementAtNext;
        }

        /**
         * Removes the element that was last returned by a call to getNext() from
         * the corresponding SList. May only be called once after a call to
         * getNext(). Complexity is O(1).
         */
        void remove() {
            if (preNext_ == list_->first_) {
                // Case: The first element of the SList shall be removed.
                delete list_->first_;
                list_->first_ = next_;
                preNext_ = NULL;
                prePreNext_ = NULL;

                // Update the pointer to the last element of the SList if it is
                // empty now.
                if (list_->first_ == NULL) {
                    list_->last_ = NULL;
                }

            } else {
                // Case: The element to be removed is not the first element of the
                // SList.
                prePreNext_->setNext(next_);
                delete preNext_;

                // Update the pointer to the last element of the SList if the last
                // element was just removed.
                if (next_ == NULL) {
                    list_->last_ = prePreNext_;
                }
            }

            --list_->size_;
        }
};


/**
 * A SListConstIterator is used to traverse a SList. This iterator allows no
 * modification of the underlying SList.
 */
template<typename T> class SListConstIterator {
    private:
        /**
         * Points to the SListItem that will be returned at the next call to
         * getNext().
         */
        SListItem<T>* next_;

    public:

        /**
         * Constructs a SListConstIterator that points to 'first'.
         */
        SListConstIterator(SListItem<T>* first) :
            next_(first) {
        }

        /**
         * Returns true iff there is a next element in the corresponding SList. If
         * there is, it can be obtained by a call to getNext().
         */
        bool hasNext() const {
            return next_ != NULL;
        }

        /**
         * Returns the next element in the corresponding SList. May only be called
         * once after a call to hasNext() returned true.
         */
        T getNext() {
            T elementAtNext = next_->getElement();
            next_ = next_->getNext();
            return elementAtNext;
        }
};


/**
 * A singly linked list.
 */
template<typename T> class SList {

        /**
         * Allow SListIterator to see the private parts of SList. This alleviates the
         * implemantion of SListIterator->remove() enormously.
         */
        friend class SListIterator<T>;

    private:

        /**
         * Points to the first element of this SList. Is NULL if there is none.
         */
        SListItem<T>* first_;

        /**
         * Points to the last element of this SList. Is NULL if there is none.
         */
        SListItem<T>* last_;

        /**
         * Stores the size of this SList, i.e., its number of elements.
         */
        unsigned int size_;

    public:

        /**
         * The iterator type needed to traverse a SList.
         */
        typedef SListIterator<T>* Iterator;

        /**
         * The const iterator type needed to traverse a SList.
         */
        typedef SListConstIterator<T>* ConstIterator;

        /**
         * Constructs an empty SList.
         */
        SList() :
            first_(NULL), last_(NULL), size_(0) {
        }

        /**
         * Destroys an empty SList. Elements are not destroyed. Only the SListItems
         * used to store the elements are destroyed.
         */
        ~SList() {
            clear();
        }

        /**
         * Adds 'element' to the end of this SList. Complexity is O(1).
         */
        SListItem<T>* add(const T& element) {
            SListItem<T>* item = new SListItem<T>(element);

            if (size_ == 0) {
                // Case: This SList is empty.
                first_ = last_ = item;
                size_ = 1;
            } else {
                // Case: This SList is not empty.
                last_->setNext(item);
                last_ = item;
                ++size_;
            }

            return item;
        }

        /**
         * Clears this SList. Aftwards its size equals 0. Complexity is O(n).
         */
        void clear() {
            SListItem<T>* current = first_;
            while (current != NULL) {
                SListItem<T>* oldCurrent = current;
                current = current->getNext();
                delete oldCurrent;
            }

            first_ = NULL;
            last_ = NULL;
            size_ = 0;
        }

        /**
         * Removes all elements that are equal to 'elementToDelete' from this
         * SList. Complexity is O(n).
         */
        void remove(const T& elementToDelete) {
            Iterator iter = getIterator();
            while (iter->hasNext()) {
                T element = iter->getNext();
                if (element == elementToDelete) {
                    iter->remove();
                }
            }
            delete iter;
        }

        /**
         * Returns an iterator for this SList. This iterator can be used to
         * traverse this SList from begin to end. You can use this iterator to
         * modify the SList, e.g., remove an element. You have to 'delete' the
         * iterator after usage. A typical loop to traverse an SList looks as
         * follows:
         *
         * <pre>
         * SList<int> list;
         * SList<int>::Iterator iter = list.getIterator();
         * while (iter->hasNext) {
         *     int element = iter->getNext();
         * }
         * delete iter;
         * </pre>
         *
         */
        Iterator getIterator() {
            return new SListIterator<T>(this);
        }

        /**
         * Returns a const iterator for this SList. This iterator can be used to
         * traverse this SList from start to end. This iterator allows no
         * modification of the underlying SList. You have to 'delete' the iterator
         * after usage. A typical loop to traverse an SList looks as follows:
         *
         * <pre>
         * SList<int> list;
         * SList<int>::ConstIterator iter = list.getConstIterator();
         * while (iter->hasNext) {
         *     int element = iter->getNext();
         * }
         * delete iter;
         * </pre>
         *
         */
        ConstIterator getConstIterator() const {
            return new SListConstIterator<T>(first_);
        }

        /**
         * Returns the size of this SList, i.e., the number of its elements.
         */
        unsigned int size() const {
            return size_;
        }

        /**
         * Returns true iff this SList is empty.
         */
        bool isEmpty() const {
            return size_ == 0;
        }

        /**
         * Returns the element at the given 'index'. The first element has
         * index 0.
         * @throw invalid_argument is the given 'index' is out of bounds.
         */
        T get(unsigned int index) {
            if (index >= size()) {
                throw std::invalid_argument("index is out of bounds.");
            }

            unsigned int current = 0;
            ConstIterator iter = getConstIterator();
            while (iter->hasNext()) {
                T result = iter->getNext();
                if (current == index) {
                    delete iter;
                    return result;
                }
                ++current;
            }
            delete iter;

            // This line should never be reached. We cannot return something
            // here to silence compiler warning about exit from non-void
            // function. So we throw an exception here.
            throw std::invalid_argument("index is out of bounds.");
        }
};

#endif
