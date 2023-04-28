#pragma once
#include <iostream>

using namespace std;

//Code for RedBlackTree class is mostly from Data Structures and Algorithm Analysis in C++ 4th edition
//Mark Allen Weiss pg 571-575
//Code for the rotateWithLeftChild and rotateWithRightChild is based on the logic explained on page 156
//from the same book

template <typename KeyType, typename DataType>
class RedBlackTree {

private:
    struct RedBlackNode {
        KeyType key;
        DataType data;
        RedBlackNode* left;
        RedBlackNode* right;
        int color;

        //constructors for a red black node
        RedBlackNode(const KeyType& theKey = KeyType{}, const DataType& theData = DataType{},
                     RedBlackNode* lt = nullptr, RedBlackNode* rt = nullptr, int c = BLACK)
                     : key{theKey}, data{theData}, left{lt}, right{rt}, color{c} {}

        RedBlackNode(KeyType&& theKey, DataType&& theData, RedBlackNode* lt = nullptr,
                     RedBlackNode* rt = nullptr, int c = BLACK)
                     : key{theKey}, data{theData}, left{lt}, right{rt}, color{c} {}
    };

    //pointer to the head of the tree
    RedBlackNode* header;
    RedBlackNode* nullNode;

    //used to help insert nodes
    RedBlackNode* current;
    RedBlackNode* parent;
    RedBlackNode* grand;
    RedBlackNode* great;

    //function to copy a red black node - used in copy constructor
    RedBlackNode* clone(RedBlackNode* t) const {
        //Check if the left is the same as t, if true then return nullNode
        if(t == t->left) {
            return nullNode;
        }

        else {
            return new RedBlackNode{t->key, t->data, clone(t->left), clone(t->right), t->color};
        }
    }

    //functions needed for insertion

    //this function handles rotations and color flips so the tree stays a red black tree
    //item is the item being inserted into the tree
    void handleReorient(const KeyType& item) {
        //perform color flip
        current->color = RED;
        current->left->color = BLACK;
        current->right->color = BLACK;

        //If the parent's color is red, a rotation must occur
        if(parent->color == RED) {
            grand->color = RED;

            //If this condition is true, then a double rotation needs to occur
            if(item < grand->key != item < parent->key) {
                parent = rotate(item, grand);
            }

            current = rotate(item, great);
            current->color = BLACK;
        }

        //Make the root black
        header->right->color = BLACK;
    }

    //Rotate the tree to balance it
    //Returns the root of the rotated subtree
    RedBlackNode* rotate(const KeyType& item, RedBlackNode* theParent) {
        if(item < theParent->key) {
            item < theParent->left->key ?
            //Left left rotation
            rotateWithLeftChild(theParent->left) :
            //left right rotation
            rotateWithRightChild(theParent->right);
            return theParent->left;
        }

        else {
            item < theParent->right->key ?
            //Right left rotation
            rotateWithLeftChild(theParent->right) :
            //right right rotation
            rotateWithRightChild(theParent->right);
            return theParent->right;
        }
    }

    void rotateWithLeftChild(RedBlackNode*& k2) {

        RedBlackNode* k1 = k2->left;
        k2->left = k1->right;
        k1->right = k2;
        k2 = k1;
    }
    void rotateWithRightChild(RedBlackNode*& k1) {

        RedBlackNode* k2 = k1->right;
        k1->right = k2->left;
        k2->left = k1;
        k1 = k2;
    }

    //Print tree
    void printTree(RedBlackNode* t) const {
        if(t != t->left) {
            printTree(t->left);
            cout << t->key << ", " << t->data << endl;
            printTree(t->right);
        }
    }

public:
    //Constructor
    explicit RedBlackTree(const KeyType& negInf) {
        nullNode = new RedBlackNode;
        nullNode->left = nullNode->right = nullNode;

        header = new RedBlackNode{negInf};
        header->left = header->right = nullNode;
    }

    //Copy constructors
    RedBlackTree(const RedBlackTree& rhs) {
        nullNode = new RedBlackNode;
        nullNode->left = nullNode->right = nullNode;

        header = new RedBlackNode{rhs.header->key};
        header->left = nullNode;
        header->right = clone(rhs.header->right);
    }

    RedBlackTree(RedBlackTree&& rhs) {
        nullNode = new RedBlackNode;
        nullNode->left = nullNode->right = nullNode;

        header = new RedBlackNode{rhs.header->key};
        header->left = nullNode;
        header->right = clone(rhs.header->right);
    }

    //Assignment operators
    RedBlackTree & operator=(const RedBlackTree& rhs) {
        nullNode = new RedBlackNode;
        nullNode->left = nullNode->right = nullNode;

        header = new RedBlackNode{rhs.header->key};
        header->left = nullNode;
        header->right = clone(rhs.header->right);
    }

    RedBlackTree & operator=(RedBlackTree&& rhs) {
        nullNode = new RedBlackNode;
        nullNode->left = nullNode->right = nullNode;

        header = new RedBlackNode{rhs.header->key};
        header->left = nullNode;
        header->right = clone(rhs.header->right);
    }

    //Destructor
    //Perform a postorder traversal and delete all nodes in the tree
    ~RedBlackTree() {
        deletePostorder(header);
    }

    //used by the destructor to delete the tree
    //need separate function for recursion
    void deletePostorder(RedBlackNode* node) {
        if (node != nullNode) {
            deletePostorder(node->left);
            deletePostorder(node->right);
            node->left = nullptr;
            node->right = nullptr;
            delete node;
        }
    }

    //print tree
    void printTree() const {
        if(header->right == nullNode) {
            cout << "Empty tree" << endl;
        }

        else {
            printTree(header->right);
        }
    }

    //insert node into the tree
    void insert(const KeyType& newKey, const DataType& newData) {
        current = parent = grand = header;
        nullNode->key = newKey;

        while(current->key != newKey) {
            great = grand;
            grand = parent;
            parent = current;

            current = newKey < current->key ? current->left : current->right;

            //Check if there are two red children and if there are fix it
            if (current->left->color == RED && current->right->color == RED) {
                handleReorient(newKey);
            }
        }

        //Insertion fails if already present
        if(current != nullNode) {
            return;
        }

        current = new RedBlackNode{newKey, newData, nullNode, nullNode};

        //attach to parent
        if(newKey < parent->key) {
            parent->left = current;
        }

        else {
            parent->right = current;
        }

        handleReorient(newKey);
    }

    //search function to find a key in the tree
    //have two so can call the function without passing in a node and still have recursion
    DataType search(const KeyType& key) {

        RedBlackNode* resultNode = recursiveSearch(key, header->right);

            if (resultNode == nullNode) {
                throw "e";
            }

            else {
                return resultNode->data;
            }


    }

    RedBlackNode* recursiveSearch(const KeyType& key, RedBlackNode*& node) {

        if(node == nullNode) {
            return nullNode;
        }

        else if(key == node->key) {
            return node;
        }

        else if(key < node->key) {
            return recursiveSearch(key, node->left);
        }

        else {
            return recursiveSearch(key, node->right);
        }

    }

    RedBlackNode getNullNode() {
        return nullNode;
    }

    //Give numerical values to RED and BLACK to be assigned to color in the RedBlackNode struct
    enum {RED, BLACK};

};