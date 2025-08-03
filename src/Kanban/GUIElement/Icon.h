#pragma once
#include <string>
#include <Utilities.h>
#include <SFML/Graphics.hpp>

#include "Observer.h"
#include "WindowResizeHandler.h"

struct  Icon
{
    enum Type { plus, overdue, dots, minus, bell, bellDot };
    Icon(Type type, float scaleFactor = 1, sf::Color color = Utilities::icon1, sf::Color accent = Utilities::priorityHigh);

    void Update(int screenWidth);

    void Draw(int x, int y, sf::RenderTarget& target);
    bool CheckCollision(sf::Vector2f point) const;

    // static int GetDefaultWidth() { return defaultSideLenPixel; }
    // int GetWidth() const { return sideLenPixel; }
    int GetWidth() const { return sideLenPixel * 2; } // accounting for border
    Type GetType() const { return type; }
    void ToggleSecondLayer(bool value) { drawSecondLayer_ = value; }
protected:
    const static int defaultSideLenPixel = 16;
    int sideLenPixel;
    float defaultScaleRatio;
    bool drawSecondLayer_ = false;
    Type type;
    sf::Sprite layer1_;
    sf::Sprite layer2_;
    sf::Texture texture_;
    std::string iconsPath = "../../resources/Icons.png";

    void SetScale(float factor);
};
