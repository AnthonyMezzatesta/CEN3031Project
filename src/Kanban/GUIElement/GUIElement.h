#pragma once
#include <SFML/Graphics.hpp>
#include <Utilities.h>

namespace Kanban
{
    class GUIElement
    {
    protected:
        sf::Font font;
        sf::Text textObj;
        sf::Color bgColor;
        sf::RectangleShape rect;
        bool selected = false;
        virtual void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) = 0;
    public:
        GUIElement(sf::Color color = sf::Color::White);
        virtual ~GUIElement() {}
        virtual void Draw(sf::Vector2f position, sf::Vector2f size,
            sf::Vector2f origin, sf::RenderTarget& target);
        virtual bool CheckCollision(sf::Vector2f point);
        virtual void Select();
        virtual void Deselect();
    };
}