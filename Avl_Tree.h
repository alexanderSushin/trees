#include <iostream>
#include <algorithm>

class Avl{
public:
    int key;
    Avl *left;
    Avl *right;
    int depth;
};

int depth(Avl *tr) {
    if (tr == NULL)
        return 0;
    return tr->depth;
}

Avl* newNode(int key) {
    Avl* node = new Avl();
    node->key = key;
    node->left = nullptr;
    node->right = nullptr;
    node->depth = 1;
    return(node);
}

Avl *rightRotate(Avl *y) {
    Avl *x = y->left;
    Avl *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->depth = std::max(depth(y->left), depth(y->right)) + 1;
    x->depth = std::max(depth(x->left), depth(x->right)) + 1;

    return x;
}

// left rotate the sub tree rooted with x 
Avl *leftRotate(Avl *x) {
    Avl *y = x->right;
    Avl *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->depth = std::max(depth(x->left), depth(x->right)) + 1;
    y->depth = std::max(depth(y->left), depth(y->right)) + 1;

    return y;
}

// Get Balance factor of node N  
int getBalance(Avl *tr) {
    if (tr == nullptr) return 0;
    return depth(tr->left) - depth(tr->right);
}

Avl *insert(Avl *node, int key)  {
    if (node == nullptr) return(newNode(key));
    if (key < node->key) node->left = insert(node->left, key);
    else if (key > node->key) node->right = insert(node->right, key);

    node->depth = 1 + std::max(depth(node->left), depth(node->right));

    int balance = getBalance(node);

    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

Avl *minValueNode(Avl *node) {
    Avl *current = node;
    while (current->left != nullptr) current = current->left;
    return current;
}
Avl *erase(Avl *root, int key) {
    if (root == nullptr) return root;

    if (key < root->key) root->left = erase(root->left, key);

    else if (key > root->key ) root->right = erase(root->right, key);

    else {
        if ((root->left == nullptr) || (root->right == nullptr)) {
            Avl *temp = root->left ? root->left : root->right;
            if (temp == nullptr) {
                temp = root;
                root = NULL;
            }
            else *root = *temp;
            free(temp);
        } else {
            Avl *temp = minValueNode(root->right);
            root->key = temp->key;
            root->right = erase(root->right, temp->key);
        }
    }
    if (root == nullptr) return root;

    root->depth = 1 + std::max(depth(root->left), depth(root->right));

    int balance = getBalance(root);


    if (balance > 1 && getBalance(root->left) >= 0) return rightRotate(root);

    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0) return leftRotate(root);

    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    return root;
}
