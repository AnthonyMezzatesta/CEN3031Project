#pragma once
#include <string>
#include <Utilities.h>
#include <SFML/Graphics.hpp>

class Icon
{
public:
    enum Type { plus, overdue, dots, minus, bell, bellDot };
    Icon(Type type, sf::Color color = Utilities::icon1, sf::Color accent = Utilities::priorityHigh, float scaleFactor = 1);

    void Draw(int x, int y, sf::RenderTarget& target);
    bool CheckCollision(sf::Vector2f point) const;

    static int GetDefaultWidth() { return defaultSideLenPixel; }
    int GetWidth() const { return sideLenPixel; }
    Type GetType() const { return type; }
    void ToggleSecondLayer(bool value) { drawSecondLayer_ = value; }
protected:
    const static int defaultSideLenPixel = 16;
    int sideLenPixel;
    bool drawSecondLayer_ = false;
    Type type;
    sf::Sprite layer1_;
    sf::Sprite layer2_;
    sf::Texture texture_;
    std::string iconsPath = "../../resources/Icons.png";
};
