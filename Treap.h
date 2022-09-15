#include <iostream>
#include <map>
#include <algorithm>
#include <utility>
#include <utility>
#include <vector>
#include <random>

const int SEED = 123131;
const int INF = 1e9;
//std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
std::mt19937 rnd(SEED);

class Treap {
public:
    int key, priority;
    Treap *left;
    Treap *right;
    Treap(int tkey) {
        key = tkey;
        priority = rnd() % INF;
        left = right = nullptr;
    }
};

std::pair<Treap *, Treap *> split(Treap *tree, int key) {
    if (tree == nullptr) return {nullptr, nullptr};
    if (tree->key > key) {
        std::pair<Treap *,Treap *> add = split(tree->left, key);
        tree->left = add.second;
        return {add.first, tree};
    }
    std::pair<Treap *, Treap *> add = split(tree->right, key);
    tree->right = add.first;
    return {tree, add.second};
}

Treap *merge(Treap *tl, Treap *tr) {
    if (tl == nullptr) return tr;
    if (tr == nullptr) return tl;
    if (tl->priority > tr->priority) {
        tl->right = merge(tl->right, tr);
        return tl;
    }
    tr->left = merge(tl, tr->left);
    return tr;
}

bool search(Treap *tree, int key) {
    if (tree == nullptr) return false;
    if (tree->key == key) {
        return true;
    }
    if (tree->key > key) {
        return search(tree->left, key);
    }
    return search(tree->right, key);
}

Treap *erase(Treap *root, int key) {
    std::pair<Treap*, Treap*> split1 = split(root, key);
    std::pair<Treap*, Treap*> split2 = split(split1.first, key - 1);
    //delete split2.second;
    return merge(split2.first, split1.second);
}

Treap *insert(Treap *root, int key) {
    if (search(root, key)) return root;
    Treap *tree = new Treap(key);
    std::pair<Treap *, Treap *> split1 = split(root, key);
    return merge(merge(split1.first, tree), split1.second);
}
