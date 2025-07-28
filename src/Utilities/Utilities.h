#pragma once
#include <string>
#include <SFML/Graphics.hpp>
using std::string;

struct Utilities
{
    static string fontPath;
    static sf::Color fill0;
    static sf::Color fill1;
    static sf::Color fill2;
    static sf::Color textColor;

    static float Lerp(float a, float b, float t);
    static float InvLerp(float a, float b, float v);
    static float LerpClamped(float a, float b, float t);
    static float InvLerpClamped(float a, float b, float v);
    static float RemapClamped(float in1, float in2, float out1, float out2, float v);

    // centering text: https://stackoverflow.com/questions/14505571/centering-text-on-the-screen-with-sfml
    static void DrawText(sf::RenderTarget& target, sf::Text& text, sf::Vector2f size, sf::Vector2f basePos,
        const string& textStr, int characterPixelSize, sf::Color fillColor = sf::Color::Black);
};