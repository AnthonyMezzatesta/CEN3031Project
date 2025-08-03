#include <string>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "Icon.h"
using namespace std;

Icon::Icon(Type type, float scaleFactor, sf::Color color, sf::Color accent) : sideLenPixel(defaultSideLenPixel), defaultScaleRatio(scaleFactor), type(type)
{
    if (!texture_.loadFromFile(iconsPath))
        throw std::runtime_error("could not load icons sprite sheet");

    layer1_.setTexture(texture_);
    layer1_.setColor(color);
    layer1_.setTextureRect(sf::IntRect(type * defaultSideLenPixel, 0, defaultSideLenPixel, defaultSideLenPixel));
    layer1_.scale(defaultScaleRatio, defaultScaleRatio);

    if (type == Icon::bell)
    {
        layer2_.setTexture(texture_);
        layer2_.setColor(accent);
        layer2_.setTextureRect(sf::IntRect(Icon::bellDot * defaultSideLenPixel, 0, defaultSideLenPixel, defaultSideLenPixel));
        layer2_.scale(defaultScaleRatio, defaultScaleRatio);
    }

    sideLenPixel *= defaultScaleRatio;
}

void Icon::Update(int screenWidth) {
    // get ratio from default to projected scaleFactor
    float scaleFactor = screenWidth / static_cast<float>(Utilities::defaultWindowSize.x);
    scaleFactor *= defaultScaleRatio;
    SetScale(scaleFactor);
}

void Icon::Draw(int x, int y, sf::RenderTarget& target) {
    sf::RectangleShape bgOutline(sf::Vector2f(sideLenPixel * 2, sideLenPixel * 2));
    bgOutline.setFillColor(sf::Color::Black);
    bgOutline.setPosition(x, y);

    sf::RectangleShape bgFill(bgOutline.getSize());
    bgFill.setFillColor(Utilities::fill3);
    bgFill.scale(0.8, 0.8);
    bgFill.setPosition(bgOutline.getPosition());
    bgFill.move(bgOutline.getSize().x * 0.1, bgOutline.getSize().y * 0.1);

    x += sideLenPixel/2.f;
    y += sideLenPixel/2.f;

    layer1_.setPosition(x, y);

    target.draw(bgOutline);
    target.draw(bgFill);
    target.draw(layer1_);

    if (drawSecondLayer_)
    {
        layer2_.setPosition(x, y);
        target.draw(layer2_);
    }
}

bool Icon::CheckCollision(sf::Vector2f point) const {
    auto pos = layer1_.getPosition();
    pos -= sf::Vector2f(sideLenPixel/2.f, (sideLenPixel/2.f));
    sf::FloatRect bgBounds(pos.x, pos.y, sideLenPixel * 2, sideLenPixel * 2);
    return bgBounds.contains(point);
    // return sprite_.getGlobalBounds().contains(point);
}

// protected function
void Icon::SetScale(float factor) {
    int oldSideLen = sideLenPixel;
    sideLenPixel = std::roundf(defaultSideLenPixel * factor);
    float ratio = sideLenPixel / static_cast<float>(oldSideLen);

    layer1_.scale(ratio, ratio);
    layer2_.scale(ratio, ratio);
}
