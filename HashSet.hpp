// HashSet.hpp
//
// ICS 46 Winter 2019
// Project #3: Set the Controls for the Heart of the Sun
//
// A HashSet is an implementation of a Set that is a separately-chained
// hash table, implemented as a dynamically-allocated array of linked
// lists.  At any given time, the HashSet has a "size" indicating
// how many elements are stored within it, along with a "capacity"
// indicating the size of the array.
//
// As elements are added to the HashSet and the proportion of the HashSet's
// size to its capacity exceeds 0.8 (i.e., there are more than 80% as many
// elements as there are array cells), the HashSet should be resized so
// that it is twice as large as it was before.
//
// You are not permitted to use the containers in the C++ Standard Library
// (such as std::set, std::map, or std::vector) to store the information
// in your data structure.  Instead, you'll need to use a dynamically-
// allocated array and your own linked list implemenation; the linked list
// doesn't have to be its own class, though you can do that, if you'd like.

#ifndef HASHSET_HPP
#define HASHSET_HPP

#include <functional>
#include "Set.hpp"

using namespace std;


template <typename ElementType>
class HashSet : public Set<ElementType>
{
    public:
        // The default capacity of the HashSet before anything has been
        // added to it.
        static constexpr unsigned int DEFAULT_CAPACITY = 10;

        // A HashFunction is a function that takes a reference to a const
        // ElementType and returns an unsigned int.
        using HashFunction = function<unsigned int(const ElementType&)>;

        // Initializes a HashSet to be empty, so that it will use the given
        // hash function whenever it needs to hash an element.
        explicit HashSet(HashFunction hashFunction);

        // Cleans up the HashSet so that it leaks no memory.
        virtual ~HashSet() noexcept;

        // Initializes a new HashSet to be a copy of an existing one.
        HashSet(const HashSet& s);

        // Initializes a new HashSet whose contents are moved from an
        // expiring one.
        HashSet(HashSet&& s) noexcept;

        // Assigns an existing HashSet into another.
        HashSet& operator=(const HashSet& s);

        // Assigns an expiring HashSet into another.
        HashSet& operator=(HashSet&& s) noexcept;

        // isImplemented() should be modified to return true if you've
        // decided to implement a HashSet, false otherwise.
        virtual bool isImplemented() const noexcept override;


        // add() adds an element to the set.  If the element is already in the set,
        // this function has no effect.  This function triggers a resizing of the
        // array when the ratio of size to capacity would exceed 0.8.  In the case
        // where the array is resized, this function runs in linear time (with
        // respect to the number of elements, assuming a good hash function);
        // otherwise, it runs in constant time (again, assuming a good hash
        // function).
        virtual void add(const ElementType& element) override;


        // contains() returns true if the given element is already in the set,
        // false otherwise.  This function runs in constant time (with respect
        // to the number of elements, assuming a good hash function).
        virtual bool contains(const ElementType& element) const override;


        // size() returns the number of elements in the set.
        virtual unsigned int size() const noexcept override;


        // elementsAtIndex() returns the number of elements that hashed to a
        // particular index in the array.  If the index is out of the boundaries
        // of the array, this function returns 0.
        unsigned int elementsAtIndex(unsigned int index) const;


        // isElementAtIndex() returns true if the given element hashed to a
        // particular index in the array, false otherwise.  If the index is
        // out of the boundaries of the array, this functions returns 0.
        bool isElementAtIndex(const ElementType& element, unsigned int index) const;

        void addHelper(const ElementType& e);

        void freeAnyLinkedLists();

    private:
        HashFunction hashFunction; // maps string words into array indexes
        unsigned int currentSize = 0; // current # of elements
        unsigned int currentCapacity = DEFAULT_CAPACITY;
        struct Node // linked list nodes
        {
            ElementType element;
            Node* next = nullptr;
            Node () {
                element = NULL;
                next = nullptr;
            }
            Node(ElementType e) {
                element = e;
            }
        };
        Node ** arr; // pointer to a list of node pointers
};



namespace impl_
{
    template <typename ElementType>
    unsigned int HashSet__undefinedHashFunction(const ElementType& element)
    {
        return 0;
    }
}


template <typename ElementType>
HashSet<ElementType>::HashSet(HashFunction hashFunction)
    : hashFunction{hashFunction}
{
    //cout << "inside default constructor" << endl;
    arr = new Node * [DEFAULT_CAPACITY]();
    currentSize = 0;
    currentCapacity = DEFAULT_CAPACITY;
    //cout << "end of constructor" << endl;
}

template <typename ElementType>
void HashSet<ElementType>::freeAnyLinkedLists() {
    /* Deletes any allocated nodes in linked lists */
    if (currentSize > 0) {
        for (int i = 0; i < currentCapacity; i++) {
            Node * current = arr[i];
            Node * next;
            while (current != nullptr) {
                next = current->next;
                delete current;
                current = next;
            }
        }
        delete arr;
    }
}


template <typename ElementType>
HashSet<ElementType>::~HashSet() noexcept
{
    freeAnyLinkedLists();
}

// TO TEST
template <typename ElementType>
HashSet<ElementType>::HashSet(const HashSet& s)
    : hashFunction{impl_::HashSet__undefinedHashFunction<ElementType>},
    currentSize{s.currentSize},
    currentCapacity{s.currentCapacity},
    arr{s.arr}
{
    currentCapacity = s.currentCapacity;
    currentSize = s.currentSize;
    arr = new Node * [s.currentCapacity];
    *arr = *s.arr;
}

template <typename ElementType>
HashSet<ElementType>::HashSet(HashSet&& s) noexcept
    : hashFunction{impl_::HashSet__undefinedHashFunction<ElementType>},
    currentSize{0},
    currentCapacity{0},
    arr{nullptr}
{
    //cout << "inside move constructor" << endl;
    swap(arr, s.arr);
    swap(currentSize, s.currentSize);
    swap(currentCapacity, s.currentCapacity);
}

template <typename ElementType>
HashSet<ElementType>& HashSet<ElementType>::operator=(const HashSet& s)
{
    if (this != &s) {
        currentCapacity = s.currentCapacity;
        currentSize = s.currentSize;
        freeAnyLinkedLists();
        arr = new Node * [s.currentCapacity];
        *arr = *s.arr;
    }

    return *this;
}

template <typename ElementType>
HashSet<ElementType>& HashSet<ElementType>::operator=(HashSet&& s) noexcept
{
    //cout << "inside move assignment operator" << endl;
    swap(arr, s.arr);
    swap(currentSize, s.currentSize);
    swap(currentCapacity, s.currentCapacity);
    return *this;
}


template <typename ElementType>
bool HashSet<ElementType>::isImplemented() const noexcept
{
    return true;
}

template <typename ElementType>
void HashSet<ElementType>::addHelper(const ElementType& element)
{
    unsigned int index = (hashFunction(element)) % currentCapacity;
    //cout << "adding word: " << element << " to index " << index << endl;
    if (arr[index] == nullptr || arr[index] == NULL) {
        //cout << "index was null, adding first element" << endl;
        arr[index] = new Node(element);
    } else {
        //cout << "index was not a nullptr (had " << arr[index]->element << " , appending element " << element << " to bucket.." << endl;
        Node * p = arr[index];
        while (p->next != nullptr)
            p = p->next;
        p->next = new Node(element);
        //cout << "after adding, next is: " << arr[index]->next->element << endl;
    }
}

template <typename ElementType>
void HashSet<ElementType>::add(const ElementType& element)
{
    addHelper(element);

    currentSize++;

    /* Check if current size exceeds 80% of current capacity */
    if (((float)currentSize / (float)currentCapacity) > 0.8) {

        unsigned int oldCapacity = currentCapacity;
        unsigned int newCapacity = currentCapacity * 2;

        Node ** arrCopy = arr;

        arr = new Node * [newCapacity];

        currentCapacity *= 2;

        for (int i = 0; i < oldCapacity; i++) {
            if (arrCopy[i] != nullptr) {
                if (arrCopy[i]->next == nullptr) {
                    ElementType value = arrCopy[i]->element;
                    addHelper(value);
                } else {
                    for (Node * p = arrCopy[i]; p != nullptr; p = p->next) {
                        if (p != nullptr) {
                            addHelper(p->element);
                        }
                    }
                }
            }
        }

        for (int i = 0; i < oldCapacity; i++)
            if (arrCopy[i] != nullptr) {
                for (Node * p = arrCopy[i]; p != nullptr; p = p->next) {
                    if (p != nullptr) {
                        delete p;
                    }
                }
            }

        delete [] arrCopy;

    }
}


template <typename ElementType>
bool HashSet<ElementType>::contains(const ElementType& element) const
{
    unsigned int index = this->hashFunction(element) % this->currentCapacity;

    //cout << "looking for word '" << element << "' at index " << index << endl;

    if (arr[index] == nullptr) {
        /* Bucket at this index is empty */
        return false;
    } else {
        Node * p = arr[index];
        while (p != nullptr) {
            if (p->element == element)
                return true;
            else
                p = p->next;
        }
        /* Element not found in designated bucket */
        return false;
    }
}


template <typename ElementType>
unsigned int HashSet<ElementType>::size() const noexcept
{
    return currentSize;
}

template <typename ElementType>
unsigned int HashSet<ElementType>::elementsAtIndex(unsigned int index) const
{
    /* Out of bounds check */
    if (index < 0 || index >= currentCapacity)
        return 0;
    
    /* Count number of elements in bucket at index */
    int count = 0;
    Node * lookingAt = arr[index];
    while (lookingAt != nullptr) {
        count++;
        /* Advance bucket element ptr */
        lookingAt = lookingAt->next;
    }
    return count;
}


template <typename ElementType>
bool HashSet<ElementType>::isElementAtIndex(const ElementType& element, unsigned int index) const
{
    /* Out of bounds check */
    if (index < 0 || index >= currentCapacity)
        return false;

    /* Check each element in linked list bucket for match */
    Node * lookingAt = arr[index];
    while (lookingAt != nullptr) {
        if (lookingAt->element == element) {
            /* Found matching element in bucket */
            return true;
        }
        /* Advance bucket element ptr */
        lookingAt = lookingAt->next;
    }
    return false;
}

#endif // HASHSET_HPP