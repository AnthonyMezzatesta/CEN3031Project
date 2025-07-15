#pragma once
#include <string>
#include <SFML/Graphics.hpp>
using std::string;

struct Utilities
{
    static string fontPath;

    // centering text: https://stackoverflow.com/questions/14505571/centering-text-on-the-screen-with-sfml
    static void DrawText(sf::RenderTarget& target, sf::Text& text, sf::Vector2f size, sf::Vector2f basePos, const string& textStr, int characterPixelSize, sf::Color fillColor = sf::Color::Black)
    {
        text.setString(textStr);
        text.setFillColor(fillColor);
        text.setCharacterSize(characterPixelSize); // in pixels, not points
        text.setPosition(basePos);

        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.f, textRect.top + textRect.height/2.f);

        float xOffset = size.x / 2;
        float yOffset = size.y / 2;
        text.move(xOffset, yOffset);
        target.draw(text);
    }
};