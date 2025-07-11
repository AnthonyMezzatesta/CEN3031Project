#pragma once
#include <string>
#include <SFML/Graphics.hpp>

class Icon
{
public:
    enum Types { plus = 0 };
    Icon(Types type, int scaleFactor = 1);

    void ScaleBy(int scaleFactor);
    void Draw(int x, int y, sf::RenderTarget& target);
    bool CheckCollision(sf::Vector2f point) const;

    Types GetType() const { return type; }
private:
    const static int sideLenPixel = 16;
    Types type;
    sf::Sprite sprite_;
    sf::Texture texture_;
    std::string iconsPath = "../../resources/Icons.png";
};
