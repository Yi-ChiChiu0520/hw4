#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    int getHeight(AVLNode<Key, Value> *n);
    int getBalance(AVLNode<Key, Value> *n);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value> *a);
    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value> *a);
    AVLNode<Key, Value>* rotateLeftThenRight(AVLNode<Key, Value> *n);
    AVLNode<Key, Value>* rotateRightThenLeft(AVLNode<Key, Value> *n);
    void rebalance(AVLNode<Key, Value> *n);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item) {
    // Insert as in BST
    Node<Key, Value>* temp = this->internalFind(new_item.first);
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(temp);
    if (!node) { // If node doesn't exist, perform normal BST insert
        BinarySearchTree<Key, Value>::insert(new_item);
        node = static_cast<AVLNode<Key, Value>*>(this->internalFind(new_item.first));
    } else { // If node exists, update the value
        node->setValue(new_item.second);
    }

    // Start rebalancing from the inserted/updated node
    if (node) {
        rebalance(node);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (!node) {
        return; // Node not found, nothing to remove
    }

    // If the node to be removed has two children
    if (node->getLeft() && node->getRight()) {
        Node<Key, Value>* pred = this->predecessor(node);
        this->nodeSwap(node, static_cast<AVLNode<Key, Value>*>(pred));
    }

    // Perform the actual removal as in BST
    BinarySearchTree<Key, Value>::remove(key);

    // Rebalance from the parent of the removed node
    rebalance(static_cast<AVLNode<Key, Value>*>(node->getParent()));
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
int AVLTree<Key, Value>::getHeight(AVLNode<Key, Value> *n)
{
    if (n == nullptr) {
        return -1;
    }
    return 1 + std::max(getHeight(n->getLeft()), getHeight(n->getRight()));
}

template<class Key, class Value>
int AVLTree<Key, Value>::getBalance(AVLNode<Key, Value> *n)
{
    if (n == nullptr) {
        return 0;
    }
    return getHeight(n->getRight()) - getHeight(n->getLeft());
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value> *a)
{
    AVLNode<Key, Value> *b = a->getLeft();
    a->setLeft(b->getRight());
    if (a->getLeft() != nullptr) {
        a->getLeft()->setParent(a);
    }
    b->setParent(a->getParent());
    if (a->getParent() != nullptr) {
        if (a->getParent()->getLeft() == a) {
            a->getParent()->setLeft(b);
        } else {
            a->getParent()->setRight(b);
        }
    } else {
        this->root_ = b;
    }
    b->setRight(a);
    a->setParent(b);
    rebalance(a);
    return b;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value> *a)
{
    AVLNode<Key, Value> *b = a->getRight();
    a->setRight(b->getLeft());
    if (a->getRight() != nullptr) {
        a->getRight()->setParent(a);
    }
    b->setParent(a->getParent());
    if (a->getParent() != nullptr) {
        if (a->getParent()->getLeft() == a) {
            a->getParent()->setLeft(b);
        } else {
            a->getParent()->setRight(b);
        }
    } else {
        this->root_ = b;
    }
    b->setLeft(a);
    a->setParent(b);
    rebalance(a);
    return b;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeftThenRight(AVLNode<Key, Value> *n)
{
    n->setLeft(rotateLeft(n->getLeft()));
    return rotateRight(n);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRightThenLeft(AVLNode<Key, Value> *n)
{
    n->setRight(rotateRight(n->getRight()));
    return rotateLeft(n);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node) {
    while (node != nullptr) {
        int balance = getBalance(node);
        if (balance > 1) {
            if (getBalance(node->getRight()) < 0) {
                rotateRightThenLeft(node);
            } else {
                rotateLeft(node);
            }
        } else if (balance < -1) {
            if (getBalance(node->getLeft()) > 0) {
                rotateLeftThenRight(node);
            } else {
                rotateRight(node);
            }
        }
        node = node->getParent(); // Move up the tree
    }
}



#endif