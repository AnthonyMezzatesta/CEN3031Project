#pragma once
#include <string>
#include <SFML/Graphics.hpp>

class Icon
{
public:
    enum Type { plus, overdue, dots, minus };
    Icon(Type type, sf::Color color = sf::Color(128, 128, 128, 255), float scaleFactor = 1);

    void Draw(int x, int y, sf::RenderTarget& target);
    bool CheckCollision(sf::Vector2f point) const;

    static int GetDefaultWidth() { return defaultSideLenPixel; }
    int GetWidth() const { return sideLenPixel; }
    Type GetType() const { return type; }
private:
    const static int defaultSideLenPixel = 16;
    int sideLenPixel;
    Type type;
    sf::Sprite sprite_;
    sf::Texture texture_;
    std::string iconsPath = "../../resources/Icons.png";
};
