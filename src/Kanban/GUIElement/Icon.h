#pragma once
#include <string>
#include <SFML/Graphics.hpp>

class Icon
{
public:
    enum Type { plus, overdue, dots };
    Icon(Type type, sf::Color color = sf::Color(128, 128, 128, 255), int scaleFactor = 1);

    void SetScale(int scale);
    void Draw(int x, int y, sf::RenderTarget& target);
    bool CheckCollision(sf::Vector2f point) const;

    static int GetWidth() { return sideLenPixel; }
    Type GetType() const { return type; }
private:
    const static int sideLenPixel = 16;
    Type type;
    sf::Sprite sprite_;
    sf::Texture texture_;
    std::string iconsPath = "../../resources/Icons.png";
};
