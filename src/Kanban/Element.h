#pragma once
#include <SFML/Graphics.hpp>
#include "../../include/TaskManager.h"
#include "../../include/Task.h"
#include "../Utilities/Utilities.h"
#include "Task.h"

namespace Kanban
{
    class Element
    {
    protected:
        sf::Font font;
        sf::Text textObj;
        sf::Color bgColor;
        sf::RectangleShape rect;
        bool selected = false;
        virtual void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) {}
    public:
        Element(sf::Color color = sf::Color::White) : bgColor(color) {}
        virtual ~Element() {}
        void Draw(sf::Vector2f position, sf::Vector2f size,
            sf::Vector2f origin, sf::RenderTarget& target);
        bool CheckCollision(sf::Vector2f point);
        virtual void Select();
        virtual void Deselect();
    };
}