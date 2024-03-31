#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool checkEqualPaths(Node* root, int depth, int& leafDepth); 

bool equalPaths(Node * root)
{
    // Add your code below
  int leafDepth = -1; // Used to store the depth of the first encountered leaf node
  return checkEqualPaths(root, 0, leafDepth);
}

bool checkEqualPaths(Node* root, int depth, int& leafDepth) {
    if (root == nullptr) return true; // Base case for empty tree or reaching beyond leaf nodes

    // If it's a leaf node
    if (root->left == nullptr && root->right == nullptr) {
        if (leafDepth == -1) {
            leafDepth = depth; // Set the depth for the first leaf node
            return true;
        }
        return depth == leafDepth; // Compare depths of subsequent leaf nodes
    }

    // Recursively check for left and right subtrees
    return checkEqualPaths(root->left, depth + 1, leafDepth) &&
           checkEqualPaths(root->right, depth + 1, leafDepth);
}
