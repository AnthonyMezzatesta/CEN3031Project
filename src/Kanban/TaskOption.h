#pragma once
#include <SFML/Graphics.hpp>
#include "../Utilities/Utilities.h"
#include "TaskManager.h"
#include "Task.h"
#include "Element.h"

namespace Kanban
{
    class TaskOption : public Element
    {
        Task task;

        // centering text: https://stackoverflow.com/questions/14505571/centering-text-on-the-screen-with-sfml
        void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override
        {
            textObj.setString(task.getName());
            textObj.setFillColor(sf::Color::Black);
            textObj.setPosition(basePos);

            textObj.setCharacterSize(size.y * 0.1); // in pixels, not points
            sf::FloatRect textRect = textObj.getLocalBounds();
            textObj.setOrigin(textRect.left + textRect.width/2.f, textRect.top + textRect.height/2.f);

            float xOffset = size.x / 2;
            float yOffset = size.y / 2;
            textObj.move(xOffset, yOffset);
            target.draw(textObj);
        }
    public:
        TaskOption(Task& task) : Element(sf::Color(190, 190, 190, 255)), task(task)
        {
            if (!font.loadFromFile(Utilities::fontPath))
                throw std::runtime_error("could not load font");
            textObj.setFont(font);
        }
        Task GetTask() { return task; }
        std::optional<int> getId() { return task.getId(); }

    };
}