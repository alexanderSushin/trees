#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
Font font;

class Button {
    Text title;
    int backlight_thickness = 0;

public:
    int backlight = false;
    RectangleShape field;

    void setTitle(std::string name, int size, Color color = { 0, 0, 0, 255 }) {
        title.setString(name);
        title.setCharacterSize(size);
        title.setFillColor(color);
    }

    void setTitlePosition(Vector2f pos) {
        title.setPosition(pos);
    }

    void setButtonPosition(Vector2f pos) {
        field.setPosition(pos);
    }

    void setButtonSize(Vector2f size, int size_thickness = 3) {
        field.setSize(size);
        backlight_thickness = size_thickness;
    }

    void setButtonColor(Color fill_color, Color outline_color = Color::Black) {
        field.setFillColor(fill_color);
        field.setOutlineColor(outline_color);
    }

    bool contains(const Vector2i &mouse) {
        bool check = (mouse.x >= field.getPosition().x &&
                      mouse.x <= (field.getPosition().x + field.getSize().x) &&
                      mouse.y >= field.getPosition().y &&
                      mouse.y <= (field.getPosition().y + field.getSize().y));
        if (backlight >= 0) {
            if (check ^ backlight) {
                backlight ^= 1;
            }
        }
        return check;
    }

    bool pressed(Vector2i& mouse, Event now) {
        return (contains(mouse) && now.type == Event::MouseButtonPressed && now.key.code == Mouse::Left);
    }

    void draw(RenderWindow& window) {
        if (backlight == 1 || backlight == -1) {
            field.setOutlineThickness(backlight_thickness);
        } else {
            field.setOutlineThickness(0);
        }
        window.draw(field);
        title.setFont(font);
        window.draw(title);
    }
};

class InputField {
    Button field;
    Text text;
    bool only_numbers = false;
    bool only_small = false;
    Clock clock;

public:
    bool chosen = false;
    std::string value;

    void setOnlyNumbers(bool type) {
        only_numbers = type;
    }

    void setOnlySmall(bool type) {
        only_small = type;
    }

    void setTitle(std::string name, int size, Color color = Color::White) {
        field.setTitle(name, size, color);
    }

    void setTitlePosition(Vector2f pos) {
        field.setTitlePosition(pos);
    }

    void setButtonPosition(Vector2f pos) {
        field.setButtonPosition(pos);
    }

    void setTextPosition(Vector2f pos) {
        text.setPosition(pos);
    }

    void setButtonSettings(Vector2f size, int outline_size, Color fill, Color outline = {0, 0, 0, 255}) {
        field.setButtonSize(size, outline_size);
        field.setButtonColor(fill, outline);
    }

    void setTextSettings(int size, Color color) {
        text.setCharacterSize(size);
        text.setFillColor(color);
    }

    void change(Event& now, Vector2i mouse) {
        if (chosen) {
            if (now.key.code == Keyboard::Backspace && value.size() != 0) {
                Time time = clock.getElapsedTime();
                if (time > milliseconds(130)) {
                    clock.restart();
                    value.pop_back();
                }
            }
            if (now.type == Event::TextEntered) {
                if (now.text.unicode < 128 && now.text.unicode != 8 ||
                    now.text.unicode >= 1025 && now.text.unicode <= 1105) {
                    if (only_numbers) {
                        if (now.text.unicode >= (wchar_t)'0' && now.text.unicode <= (wchar_t)'9') {
                            value += now.text.unicode;
                        }
                    } else if (only_small) {
                        if (now.text.unicode >= (wchar_t)'a' && now.text.unicode <= (wchar_t)'z') {
                            value += now.text.unicode;
                        }
                    } else {
                        value += now.text.unicode;
                    }
                }
            }
            text.setString(value);
            if (text.getPosition().x + text.getLocalBounds().width >= field.field.getPosition().x + field.field.getSize().x) {
                value.pop_back();
                text.setString(value);
            }
        }
        if (field.contains(mouse) && now.type == Event::MouseButtonPressed && now.key.code == Mouse::Left) {
            chosen = true;
            field.backlight = -1;
        } else if (!field.contains(mouse) &&
                   (now.key.code == Mouse::Left && now.type == Event::MouseButtonPressed)
                   || now.key.code == Keyboard::Enter) {
            chosen = false;
            field.backlight = -2;
        } else if (field.backlight != -1) {
            field.backlight = -2;
        }
    }

    void draw(RenderWindow& window) {
        field.draw(window);
        text.setFont(font);
        window.draw(text);
    }
};

class CircleButton {
    int backlight_thickness = 0;

public:
    Vector2f center;
    Text title;
    int backlight = false;
    CircleShape field;

    void setTitle(std::string name, int size, Color color = { 0, 0, 0, 255 }) {
        title.setString(name);
        title.setCharacterSize(size);
        title.setFillColor(color);
    }

    void setTitlePosition(Vector2f pos) {
        title.setPosition(pos);
    }

    void setButtonPosition(Vector2f pos) {
        center = pos;
        field.setPosition(pos);
    }

    Vector2f getCenter() {
        Vector2f res;
        res.x = field.getRadius() + field.getPosition().x;
        res.y = field.getRadius() + field.getPosition().y;
        return res;
    }

    void setButtonSize(float size, int size_thickness = 2) {
        field.setRadius(size);
        backlight_thickness = size_thickness;
    }

    void setButtonColor(Color fill_color, Color outline_color = Color::Black) {
        field.setFillColor(fill_color);
        field.setOutlineColor(outline_color);
    }

    bool contains(Vector2f mouse) {
        float dx = (mouse.x - center.x - field.getRadius()), dy = (mouse.y - center.y - field.getRadius());
        bool check = dx * dx + dy * dy <= field.getRadius() * field.getRadius();
        if (backlight >= 0) {
            if (check ^ backlight) {
                backlight ^= 1;
            }
        }
        return check;
    }


    bool pressed(Vector2f mouse, Event now) {
        return (contains(mouse) && now.type == Event::MouseButtonPressed && now.key.code == Mouse::Left);
    }

    void draw(RenderWindow& window) {
        if (backlight == 1 || backlight == -1) {
            field.setOutlineThickness(backlight_thickness);
        } else {
            field.setOutlineThickness(0);
        }
        window.draw(field);
        title.setFont(font);
        window.draw(title);
    }
};

class Line {

public:
    Vertex vertices[4];
    double thickness;

    Line(Vector2f first, Vector2f second, double _thickness = 2, Color color = Color::Black) {
        thickness = _thickness;
        Vector2f p = second - first; // направляющий вектор прямой
        Vector2f p1 = p / sqrt(p.x * p.x + p.y * p.y);
        Vector2f n = Vector2f(-p1.y, p1.x) * (float)(thickness / 2);
        vertices[0].position = first + n;
        vertices[1].position = second + n;
        vertices[2].position = second - n;
        vertices[3].position = first - n;
        vertices[0].color = color;
        vertices[1].color = color;
        vertices[2].color = color;
        vertices[3].color = color;
    }

    void draw(RenderWindow& window) {
        window.draw(vertices, 4, Quads);
    }
};
