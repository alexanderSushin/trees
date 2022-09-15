#include "Splay_Tree.h"
#include "Avl_Tree.h"
#include "Treap.h"
#include "Input.h"
#include <SFML/Graphics.hpp>
#include <array>
using namespace sf;

int last = 0;

Avl *tr1;
Treap *tr2;
SplayTree *tr4;

float radius = 60;
float dy = 150;
float dx = 100;

template<class T> int size(T *tr) {
    if (tr == nullptr) return 0;
    int value = 0;
    value = std::max(value, size(tr->left) + 1);
    value = std::max(value, size(tr->right) + 1);
    return value;
}

template<class T> int calc(T *tr) {
    if (tr == nullptr) return 0;
    int value = 1;
    value += calc(tr->left);
    value += calc(tr->right);
    return value;
}

template<class T> std::pair<int, float> redo1(T *tr, std::vector<CircleButton> &buttons,
                                             std::vector<std::array<int, 2>> &gr, float x, float y) {
    if (tr == nullptr) return {-1, x};
    std::array<int, 2> adj;
    auto left = redo1(tr->left, buttons, gr, x, y + dy);
    adj[0] = left.first;
    x = left.second + dx;
    auto right = redo1(tr->right, buttons, gr, x, y + dy);
    adj[1] = right.first;
    gr.push_back(adj);
    CircleButton cur;
    cur.setButtonPosition({x, y});
    cur.setButtonColor(Color::White);
    cur.setButtonSize(radius);
    cur.setTitle(std::to_string(tr->key), 3 / 4.0 * radius);
    cur.setTitlePosition({x + 10, y + 25});
    buttons.push_back(cur);
    return {buttons.size() - 1, right.second};
};


template<class T> int redo2(T *tr, std::vector<CircleButton> &buttons,
                           std::vector<std::array<int, 2>> &gr, float lhs, float rhs, float y) {
    if (tr == nullptr) return -1;
    std::array<int, 2> adj;
    float mid = (lhs + rhs) / 2;
    adj[0] = redo2(tr->left, buttons, gr, lhs, mid, y + dy);
    adj[1] = redo2(tr->right, buttons, gr, mid, rhs, y + dy);
    gr.push_back(adj);
    CircleButton cur;
    cur.setButtonPosition({mid, y});
    cur.setButtonColor(Color::White);
    cur.setButtonSize(radius);
    cur.setTitle(std::to_string(tr->key), 3 / 4.0 * radius);
    cur.setTitlePosition({mid + 10, y + 25});
    buttons.push_back(cur);
    return buttons.size() - 1;
};

template<class T> int redo(T *tr, std::vector<CircleButton> &buttons,
                            std::vector<std::array<int, 2>> &gr) {
    int h = size(tr);
    int num = calc(tr);
    if (pow(2, h) <= 3 * num) {
        dy = 150 * h;
        redo2(tr, buttons, gr, 0, pow(2, h) * 2 * radius, 10);
    } else {
        dy = 150;
        redo1(tr, buttons, gr, 0, 10);
    }
};


void renderTree(int id) {
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow window(VideoMode(2000, 1500), "visualisator", Style::Close | Style::Titlebar, settings);
    Vector2f deltaPos = {0, 0};
    Vector2f oldPos;
    bool moving = false;
    float zoom = 1.f;
    View view = window.getDefaultView();
    view.setSize(window.getDefaultView().getSize());
    view.zoom(zoom);
    window.setView(view);
    std::vector<CircleButton> buttons;
    std::vector<std::array<int, 2>> gr;
    int now_size;
    if (id == 1) {
        redo(tr1, buttons, gr);
    } else if (id == 2) {
        redo(tr2, buttons, gr);
    } else if (id == 4) {
        redo(tr4, buttons, gr);
    }
    //zoom = 1.0 / now_size;
    while (window.isOpen()) {
        window.clear(Color(200, 200, 200));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(window);
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            } else if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == 0) {
                    moving = true;
                    oldPos = window.mapPixelToCoords(Vector2i(event.mouseButton.x, event.mouseButton.y));
                }
            } else if (event.type == Event::MouseButtonReleased) {
                if (event.mouseButton.button == 0) {
                    moving = false;
                }
            } else if (event.type == Event::MouseWheelMoved) {
                if (moving) break;
                if (event.mouseWheel.delta <= -1)
                    zoom = zoom * 1.05f;
                else if (event.mouseWheel.delta >= 1)
                    zoom = zoom * 0.95f;
                view.setSize(window.getDefaultView().getSize());
                view.zoom(zoom);
                window.setView(view);
            } else if (event.type == Event::MouseMoved) {
                if (!moving) break;
                Vector2f newPos = window.mapPixelToCoords(Vector2i(event.mouseMove.x, event.mouseMove.y));
                deltaPos = oldPos - newPos;
                view.setCenter(view.getCenter() + deltaPos);
                window.setView(view);
                oldPos = window.mapPixelToCoords(Vector2i(event.mouseMove.x, event.mouseMove.y));
            }
            bool was = 0;
            for (int i = 0; i < buttons.size(); ++i) {
                Vector2f to = window.mapPixelToCoords(mouse_position);
                to.x += view.getViewport().left;
                to.y += view.getViewport().top;
                if (buttons[i].pressed(to, event)) {
                    was = 1;
                    std::string value = buttons[i].title.getString();
                    tr1 = erase(tr1, std::stoi(value));
                    tr2 = erase(tr2, std::stoi(value));
                    tr4 = erase(tr4, std::stoi(value));
                }
            }
            if (was) {
                buttons.clear();
                gr.clear();
                if (id == 1) {
                    redo(tr1, buttons, gr);
                } else if (id == 2) {
                    redo(tr2, buttons, gr);
                } else if (id == 4) {
                    redo(tr4, buttons, gr);
                }
            }
        }
        //std::cout << zoom << '\n';
        for (int i = 0; i < gr.size(); ++i) {
            if (gr[i][0] != -1) {
                Line left(buttons[i].getCenter(), buttons[gr[i][0]].getCenter(), 3 * pow(zoom, 3 / 3.0));
                left.draw(window);
            }
            if (gr[i][1] != -1) {
                Line right(buttons[i].getCenter(), buttons[gr[i][1]].getCenter(), 3 * pow(zoom, 3 / 3.0));
                right.draw(window);
            }
        }
        for (int i = 0; i < buttons.size(); ++i) {
            buttons[i].draw(window);
        }
        window.display();
        view = window.getView();
    }
};

void renderMany() {
    RenderWindow input(VideoMode(500, 300), "add_many", Style::Close | Style::Titlebar);
    InputField left;
    left.setButtonPosition({50, 100});
    left.setButtonSettings({180, 100}, 3, Color::White);
    left.setTextSettings(70, Color::Black);
    left.setTextPosition({50 + 10, 100});
    left.setOnlyNumbers(1);
    Button save;
    save.setButtonColor(Color::White);
    save.setButtonSize({150, 100});
    save.setButtonPosition({300, 100});
    save.setTitlePosition({332, 100});
    save.setTitle("go", 75);
    while (input.isOpen()) {
        input.clear(Color(200, 200, 200));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(input);
        while (input.pollEvent(event)) {
            if (event.type == Event::Closed) {
                input.close();
            }
            left.change(event, mouse_position);
            if (save.pressed(mouse_position, event)) {
                input.close();
                int num = last;
                if (left.value != "") {
                    num += std::stoi(left.value);
                }
                std::vector<int> pos;
                for (; last < num; ++last) {
                    pos.push_back(last);
                }
                std::shuffle(pos.begin(), pos.end(), rnd);
                while (pos.size()) {
                    int value = pos.back();
                    //std::cout << value << '\n';
                    tr1 = insert(tr1, value);
                    tr2 = insert(tr2, value);
                    tr4 = insert(tr4, value);
                    pos.pop_back();
                }
            }
        }
        left.draw(input);
        save.draw(input);
        input.display();
    }
}


int main() {
    font.loadFromFile("/Users/alexsus/Desktop/polynomials/arial.ttf");
    Button Avl, Treap, Splay, RedBlack;
    Avl.setButtonPosition({100, 100});
    Treap.setButtonPosition({500, 100});
    RedBlack.setButtonPosition({900, 100});
    Splay.setButtonPosition({1300, 100});
    Avl.setButtonSize({300, 100}, 3);
    Treap.setButtonSize({300, 100}, 3);
    RedBlack.setButtonSize({300, 100}, 3);
    Splay.setButtonSize({300, 100}, 3);
    Avl.setButtonColor(Color::White);
    Treap.setButtonColor(Color::White);
    RedBlack.setButtonColor(Color::White);
    Splay.setButtonColor(Color::White);
    Avl.setTitle("Avl", 75);
    Treap.setTitle("Treap", 75);
    RedBlack.setTitle("RB", 75);
    Splay.setTitle("Splay", 75);
    Avl.setTitlePosition({100 + 90, 100});
    Treap.setTitlePosition({500 + 45, 100});
    RedBlack.setTitlePosition({900 + 95, 100});
    Splay.setTitlePosition({1300 + 50, 100});

    Button Add;
    Add.setButtonColor(Color::White);
    Add.setButtonSize({300, 100});
    Add.setButtonPosition({500, 300});
    Add.setTitle("Add", 75);
    Add.setTitlePosition({500 + 80, 300});

    Button AddMany;
    AddMany.setButtonColor(Color::White);
    AddMany.setButtonSize({300, 100});
    AddMany.setButtonPosition({900, 300});
    AddMany.setTitle("Many", 75);
    AddMany.setTitlePosition({900 + 58, 300});

    RenderWindow window(VideoMode(1700, 500), "trees", Style::Close | Style::Titlebar);
    while (window.isOpen()) {
        window.clear(Color(200, 200, 200));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(window);
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            } else if (Add.pressed(mouse_position, event)) {
                tr1 = insert(tr1, last);
                tr2 = insert(tr2, last);
                tr4 = insert(tr4, last);
                ++last;
            } else if (AddMany.pressed(mouse_position, event)) {
                renderMany();
            } else if (Avl.pressed(mouse_position, event)) {
                renderTree(1);
            } else if (Treap.pressed(mouse_position, event)) {
                renderTree(2);
            } else if (RedBlack.pressed(mouse_position, event)) {

            } else if (Splay.pressed(mouse_position, event)) {
                renderTree(4);
            }
        }
        Avl.draw(window);
        Treap.draw(window);
        RedBlack.draw(window);
        Splay.draw(window);
        Add.draw(window);
        AddMany.draw(window);
        window.display();
        //std::cout << last << '\n';
    }
    return 0;
}
