#include "GUIElement.h"
#include <SFML/Graphics.hpp>
using namespace std;

namespace Kanban
{
    void GUIElement::Draw(sf::Vector2f position, sf::Vector2f size, sf::Vector2f origin, sf::RenderTarget& target)
    {
        rect.setPosition(position);
        rect.setSize(size);
        rect.setFillColor(bgColor);
        rect.setOrigin(origin);
        target.draw(rect);

        DrawDetails(target, size, position);
    }

    bool GUIElement::CheckCollision(sf::Vector2f point)
    {
        return rect.getGlobalBounds().contains(point);
    }

    void GUIElement::Select()
    {
        // selected = true;
        // rect.setOutlineColor(sf::Color::Cyan);
        // rect.setOutlineThickness(3);
    }

    void GUIElement::Deselect()
    {
        selected = false;
        rect.setOutlineColor(bgColor);
        rect.setOutlineThickness(0);
    }
}
