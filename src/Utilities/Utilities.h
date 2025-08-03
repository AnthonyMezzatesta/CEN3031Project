#pragma once
#include <string>
#include <Task.h>
#include <SFML/Graphics.hpp>
using std::string;

struct Utilities
{
    static int windowStyle;
    static string windowTitle;
    static sf::Vector2i defaultWindowSize;

    static string fontPath;
    static sf::Color fill0;
    static sf::Color fill1;
    static sf::Color fill2;
    static sf::Color fill3;
    static sf::Color textColor;
    static sf::Color priorityLow;
    static sf::Color priorityMed;
    static sf::Color priorityHigh;
    static sf::Color icon1;

    static float Lerp(float a, float b, float t);
    static float InvLerp(float a, float b, float v);
    static float LerpClamped(float a, float b, float t);
    static float InvLerpClamped(float a, float b, float v);
    static float RemapClamped(float in1, float in2, float out1, float out2, float v);

    static sf::Color TaskPriorityToColor(Task::Priority priority);
    static string TaskPriorityToString(Task::Priority priority);
    static void DrawText(sf::RenderTarget& target, sf::Text& text, sf::Vector2f size, sf::Vector2f basePos,
        const string& textStr, int characterPixelSize, sf::Color fillColor = sf::Color::Black);
};