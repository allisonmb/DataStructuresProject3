#pragma once
#include "RedBlackTree.h"

template<typename KeyType, typename DataType>
class Map {

private:
    RedBlackTree<KeyType, DataType> tree;

public:

    //constructor
    explicit Map() : tree(-1) {
        //tree RedBlackTree<KeyType, DataType>(-1);
    }

    //Print all keys and data in the map
    void printMap() {
        tree.printTree();
    }

    //insert key and data into the map
    //if the key is already in the map, then give an error
    void insert(const KeyType& key, const DataType& data) {

        //If the search finds the key already in the map, then give an error
        //otherwise insert the key and data into the tree
        try {
            tree.search(key);
            cout << "Insertion failed: Key is already in map" << endl;
        }

        catch(char const* e) {
            tree.insert(key, data);
        }
    }


    DataType find(const KeyType& key) {
            return tree.search(key);
    }

};