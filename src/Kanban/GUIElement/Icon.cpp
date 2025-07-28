#include <string>
#include <SFML/Graphics.hpp>
#include "Icon.h"
using namespace std;

Icon::Icon(Type type, sf::Color color, float scaleFactor): type(type), sideLenPixel(defaultSideLenPixel) {
    if (!texture_.loadFromFile(iconsPath))
        throw std::runtime_error("could not load icons sprite sheet");
    sprite_.setTexture(texture_);
    sprite_.setColor(color);
    sprite_.setTextureRect(sf::IntRect(type * sideLenPixel, 0, sideLenPixel, sideLenPixel));
    sideLenPixel *= scaleFactor;
    sprite_.scale(scaleFactor, scaleFactor);
}

void Icon::Draw(int x, int y, sf::RenderTarget& target) {
    sf::RectangleShape bgOutline(sf::Vector2f(sideLenPixel * 2, sideLenPixel * 2));
    bgOutline.setFillColor(sf::Color::Black);
    bgOutline.setPosition(x - sideLenPixel/2.f, y - sideLenPixel/2.f);

    sf::RectangleShape bgFill(bgOutline.getSize());
    bgFill.setFillColor(sf::Color(192, 192, 192, 255));
    bgFill.scale(0.8, 0.8);
    bgFill.setPosition(bgOutline.getPosition());
    bgFill.move(bgOutline.getSize().x * 0.1, bgOutline.getSize().y * 0.1);

    sprite_.setPosition(x, y);

    target.draw(bgOutline);
    target.draw(bgFill);
    target.draw(sprite_);
}

bool Icon::CheckCollision(sf::Vector2f point) const {
    float scale = sprite_.getScale().x;
    auto pos = sprite_.getPosition();
    pos -= sf::Vector2f(sideLenPixel/2.f * scale, (sideLenPixel/2.f * scale));
    sf::FloatRect bgBounds(pos.x, pos.y, sideLenPixel * 2 * scale, sideLenPixel * 2 * scale);
    return bgBounds.contains(point);
    // return sprite_.getGlobalBounds().contains(point);
}
