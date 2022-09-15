#include <iostream>
#include <map>
#include <algorithm>
#include <utility>
#include <utility>
#include <vector>
#include <random>

class SplayTree {
public:
    int key;
    SplayTree *left, *right, *parent;
    SplayTree(int tkey, SplayTree *l = nullptr, SplayTree *r = nullptr) {
        key = tkey;
        left =  l, right = r;
        parent = nullptr;
    }
};


void set_parent(SplayTree *&child, SplayTree *parent) {
    if (child != nullptr) {
        child->parent = parent;
    }
    return;
}

void keep_parent(SplayTree *&tr) {
    set_parent(tr->left, tr);
    set_parent(tr->right, tr);
    return;
}

SplayTree *zig(SplayTree *parent, SplayTree *tr) {
    SplayTree *gparent = parent->parent;
    if (gparent != nullptr) {
        if (gparent->left == parent) {
            gparent->left = tr;
        } else {
            gparent->right = tr;
        }
    }
    if (parent->left == tr) {
        SplayTree *tmp = tr->right;
        tr->right = parent;
        parent->left = tmp;
    }
    else {
        SplayTree *tmp = tr->left;
        tr->left = parent;
        parent->right = tmp;
    }
    keep_parent(tr);
    keep_parent(parent);
    set_parent(tr, gparent);
    return tr;
}

SplayTree *splay(SplayTree *tr) {
    if (tr->parent == nullptr) return tr;
    SplayTree *parent = tr->parent;
    SplayTree *gparent = parent->parent;
    if (gparent == nullptr) {
        tr = zig(parent, tr);
        return tr;
    }
    bool zigzig = (gparent->left == parent) == (parent->left == tr);
    if (zigzig) {
        parent = zig(gparent, parent);
        tr = zig(parent, tr);
    } else {
        tr = zig(parent, tr);
        tr = zig(gparent, tr);
    }
    return splay(tr);
}

SplayTree *find(SplayTree *tr, int key) {
    if (tr == nullptr) return nullptr;
    if (tr->key == key) {
        return splay(tr);
    }
    if (key < tr->key && tr->left != nullptr) {
        return find(tr->left, key);
    }
    if (key > tr->key && tr->right != nullptr) {
        return find(tr->right, key);
    }
    return splay(tr);
}

std::pair<SplayTree *, SplayTree *> split(SplayTree *tr, int key) {
    if (tr == nullptr) return {nullptr, nullptr};
    tr = find(tr, key);
    if (tr->key == key) {
        set_parent(tr->left, nullptr);
        set_parent(tr->right, nullptr);
        return {tr->left, tr->right};
    }
    if (tr->key < key) {
        SplayTree *right = tr->right;
        tr->right = nullptr;
        set_parent(right, nullptr);
        return {tr, right};
    } else {
        SplayTree *left = tr->left;
        tr->left = nullptr;
        set_parent(left, nullptr);
        return {left, tr};
    }
}

SplayTree *merge(SplayTree *l, SplayTree *r) {
    if (l == nullptr) return r;
    if (r == nullptr) return l;
    r = find(r, l->key);
    r->left = l;
    l->parent = r;
    return r;
}

SplayTree *erase(SplayTree *tr, int key) {
    tr = find(tr, key);
    SplayTree *l = tr->left, *r = tr->right;
    set_parent(l, nullptr);
    set_parent(r, nullptr);
    //delete tr;
    return merge(l, r);
}

SplayTree *insert(SplayTree *tr, int key) {
    auto [l, r] = split(tr, key);
    SplayTree *root = new SplayTree(key, l, r);
    tr = root;
    keep_parent(tr);
    return tr;
}
