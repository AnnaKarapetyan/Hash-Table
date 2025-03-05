#pragma once 

#include "Node.h"
#include <assert.h>
#include <iostream>

template<typename V, typename K>
class HashTable
{
private:
    int TableSize; // Hash table size
    Node<V, K>** m_table; // Array of pointers to Node type
    int count_of_elements; // Number of elements in the table
    float load_factor; // Load factor metric

public:
    HashTable(int size = 100); // Parameterized constructor for table size
    ~HashTable();
    HashTable(const HashTable& copy); // Copy constructor

    int HashFunction(K key); // Hash function for index generation
    void Add(V value, K key); // To add an element
    void Remove(K key); // To remove an element
    void clear(); // To clear the table

    V operator[] (K key); // Allows access to value by key
    HashTable& operator= (const HashTable& copy); // Assignment operator

    void print_table() const; // Function to print the table
    bool empty() const; // Check if the table is empty
    int NumberOfItemsInKey(const K& key); // Number of collisions for a given key

private:
    void resize(); // Function to resize the table (newSize = oldSize * 2 + 1)
};

template<typename V, typename K>
HashTable<V, K>::HashTable(int size)
    : TableSize(size)
    , m_table(new Node<V, K>* [TableSize]) // Memory allocation for the table
    , count_of_elements(0) // Initial count of elements is 0
    , load_factor(0) // Load factor is 0 when the table is created
{
    for (int i = 0; i < TableSize; ++i)
    {
        m_table[i] = nullptr;
    }
    // Initially, assign nullptr to all pointers in the array
}

template<typename V, typename K>
HashTable<V, K>::~HashTable()
{
    clear(); // Clears the table but doesn't free memory for the pointers in the array
    delete[] m_table; // Delete the array of table pointers
    m_table = nullptr; // Set the table pointer to nullptr
}

template<typename V, typename K>
HashTable<V, K>::HashTable(const HashTable& copy)
{
    TableSize = copy.TableSize;
    m_table = new Node<V, K>* [TableSize]; // Allocate memory for the table

    for (int i = 0; i < TableSize; ++i)
    {
        m_table[i] = nullptr; // Assign nullptr to the table pointers
    }

    for (int i = 0; i < copy.TableSize; ++i) // Iterate over the old table
    {
        Node<V, K>* t = copy.m_table[i]; // Get each table pointer
        while (t) // Traverse the linked list if there is one
        {
            Add(t->value, t->key); // Add all old data to the new table
            t = t->next;
        }
    }
}

template<typename V, typename K>
HashTable<V, K>& HashTable<V, K>::operator= (const HashTable& copy) // Perform the same steps as the copy constructor, but delete the current table content first
{
    clear();
    TableSize = copy.TableSize;
    m_table = new Node<V, K>* [TableSize];

    for (int i = 0; i < TableSize; ++i)
    {
        m_table[i] = nullptr;
    }

    for (int i = 0; i < copy.TableSize; ++i)
    {
        Node<V, K>* t = copy.m_table[i];
        while (t)
        {
            Add(t->value, t->key);
            t = t->next;
        }
    }
    return *this; // Return a reference to the current object for multiple assignments
}

template<typename V, typename K>
void HashTable<V, K>::clear()
{
    for (int i = 0; i < TableSize; i++) {
        Node<V, K>* tmp = m_table[i]; // Temporary variable to traverse and delete the linked list
        while (tmp != nullptr) {
            Node<V, K>* next = tmp->next;
            delete tmp; // Keep the next node and delete the current one
            tmp = next; // Move to the next node
        }
        m_table[i] = nullptr; // Set the table pointer to nullptr
    }
    count_of_elements = 0;
    load_factor = 0;
}

template<typename V, typename K>
int HashTable<V, K>::HashFunction(K key) // Assumes the key is a text type, hash function can be replaced with a more appropriate one if necessary
{
    int size = key.size();
    int s = 0;

    for (int i = 0; i < size; ++i)
    {
        s += (i + TableSize) * int(key[i]);
    }
    // Traverse through the key and sum up the ASCII values of its characters, multiplied by a chosen pattern (i + TableSize) to ensure better dispersion in the table and avoid collisions
    return s % TableSize; // Return the sum modulo the table size to get a valid index
}

template<typename V, typename K>
void HashTable<V, K>::Add(V v, K k)
{
    int ind = HashFunction(k); // Get the index in the table where we need to store the new element
    if (m_table[ind] == nullptr) // If nothing is at this index, assign the pointer to a new node and create it with the passed data
    {
        m_table[ind] = new Node<V, K>(v, k);
        ++count_of_elements;
    }
    else
    {
        // If the index is not empty (i.e., there's a linked list), traverse to the end and add the new element in the same way
        Node<V, K>* tmp = m_table[ind];
        Node<V, K>* tmp_prev = nullptr; // Temporary variable to hold the previous node
        while (tmp)
        {
            if (tmp->key == k) // If the key already exists, replace the old value with the new one
            {
                tmp->value = v;
                return;
            }
            tmp_prev = tmp;
            tmp = tmp->next;
        }
        // When tmp becomes nullptr, we've reached the end of the list
        tmp_prev->next = new Node<V, K>(v, k); // Link the new node to the previous one
        ++count_of_elements; // Increase the number of elements in the table
    }
    load_factor = double(count_of_elements) / double(TableSize);
    if (load_factor > 0.7)
    {
        resize();
    }
}

template<typename V, typename K>
void HashTable<V, K>::Remove(K k)
{
    int ind = HashFunction(k);
    if (m_table[ind] == nullptr)
    {
        return; // Empty case
    }
    Node<V, K>* tmp = m_table[ind];
    Node<V, K>* tmp_prev = nullptr;
    if (tmp->key == k) // If the first node has the required element, delete it
    {
        m_table[ind] = tmp->next;
        delete tmp;

        --count_of_elements;
        load_factor = double(count_of_elements) / double(TableSize); // Update the load factor
        return;
    }

    while (tmp->next) // Traverse until we find the element or reach the end
    {
        tmp_prev = tmp;
        tmp = tmp->next;
        if (tmp->key == k) // If found, delete it
        {
            Node<V, K>* t = tmp;
            tmp_prev->next = tmp->next; // Point the previous node to the next of the deleted one
            delete t;

            --count_of_elements;
            load_factor = double(count_of_elements) / double(TableSize); // Update the load factor
            return;
        }
    }
    std::cout << "Not found" << std::endl; // If the element was not found
}

template<typename V, typename K>
int HashTable<V, K>::NumberOfItemsInKey(const K& key)
{
    int c = 1; // Start with 1
    int ind = HashFunction(key); // Find the required index
    if (m_table[ind] == nullptr)
    {
        return 0; // If the index is empty, return 0
    }
    Node<V, K>* tmp = m_table[ind];
    while (tmp->next)
    {
        tmp = tmp->next;
        c += 1; // Increment the count for each new node
    }
    return c;
}

template<typename V, typename K>
V HashTable<V, K>::operator[] (K key)
{
    int ind = HashFunction(key);
    if (m_table[ind]) // If the index is not empty
    {
        Node<V, K>* tmp = m_table[ind]; // Start traversing the list to find the key
        while (tmp)
        {
            if (tmp->key == key)
            {
                return tmp->value; // Return the value for the found key
            }
            tmp = tmp->next;
        }
    }
    std::cout << "Not found" << std::endl;
    assert(0 > 1); // If not found, terminate the program with a message
}

template<typename V, typename K>
void HashTable<V, K>::print_table() const // Traverse all table pointers and linked lists to print them
{
    for (int i = 0; i < TableSize; ++i)
    {
        if (m_table[i] == nullptr)
        {
            continue;
        }
        Node<V, K>* tmp = m_table[i];
        std::cout << "[" << tmp->key << ":" << tmp->value << "]" << ' ';
        while (tmp->next)
        {
            tmp = tmp->next;
            std::cout << "[" << tmp->key << ":" << tmp->value << "]" << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

template<typename V, typename K>
void HashTable<V, K>::resize()
{
    std::cout << "Resizing" << std::endl;
    int tmp_size = TableSize * 2 + 1; // Define the new size as 2 times the old size plus 1
    HashTable<V, K> tmp(tmp_size); // Create a new hash table with the new size
    for (int i = 0; i < TableSize; ++i) // Iterate over the old table
    {
        Node<V, K>* t = m_table[i];
        while (t) // If there is a linked list, traverse it
        {
            tmp.Add(t->value, t->key); // Add all old data to the new table
            t = t->next;
        }
    }

    *this = tmp;
}

template<typename V, typename K>
bool HashTable<V, K>::empty()const
{
    return count_of_elements == 0;
}
