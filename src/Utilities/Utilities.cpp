#include <SFML/Graphics.hpp>
#include <string>
#include "Utilities.h"

// redeclaring static variable
std::string Utilities::fontPath = "../../resources/aovel-sans-rounded-font/AovelSansRounded-rdDL.ttf";

float Utilities::Lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}

float Utilities::InvLerp(float a, float b, float v) {
    return (v - a) / (b - a);
}

float Utilities::LerpClamped(float a, float b, float t) {
    t = std::clamp(t, 0.f, 1.f);
    return (1 - t) * a + t * b;
}

float Utilities::InvLerpClamped(float a, float b, float v) {
    v = std::clamp(v, a, b);
    return (v - a) / (b - a);
}

float Utilities::RemapClamped(float in1, float in2, float out1, float out2, float v) {
    float t = InvLerpClamped(in1, in2, v);
    return LerpClamped(out1, out2, t);
}

void Utilities::DrawText(sf::RenderTarget& target, sf::Text& text, sf::Vector2f size, sf::Vector2f basePos,
    const string& textStr, int characterPixelSize, sf::Color fillColor) {
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
