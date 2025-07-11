#include "Element.h"
#include <SFML/Graphics.hpp>
#include "../../include/TaskManager.h"
#include "../../include/Task.h"
#include "../Utilities/Utilities.h"
using namespace std;

namespace Kanban
{
    void Element::Draw(sf::Vector2f position, sf::Vector2f size, sf::Vector2f origin, sf::RenderTarget& target)
    {
        rect.setPosition(position);
        rect.setSize(size);
        rect.setFillColor(bgColor);
        rect.setOrigin(origin);
        target.draw(rect);

        DrawDetails(target, size, position);
    }

    bool Element::CheckCollision(sf::Vector2f point)
    {
        sf::FloatRect boundingBox = rect.getGlobalBounds();
        return boundingBox.contains(point);
    }

    void Element::Select()
    {
        selected = true;
        rect.setOutlineColor(sf::Color::Cyan);
        rect.setOutlineThickness(3);
    }

    void Element::Deselect()
    {
        selected = false;
        rect.setOutlineColor(bgColor);
        rect.setOutlineThickness(0);
    }
}
