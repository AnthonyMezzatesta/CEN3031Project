#pragma once
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "TaskManager.h"
#include "Task.h"
#include "GUIElement.h"

namespace Kanban
{
    class TaskOption : public GUIElement
    {
        Task task;

        void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override
        {
            // draw task name
            Utilities::DrawText(target, textObj, size, basePos, task.getName(), 24/*size.y * 0.15*/);
        }
    public:
        TaskOption(Task& task) : GUIElement(sf::Color(190, 190, 190, 255)), task(task)
        {
            if (!font.loadFromFile(Utilities::fontPath))
                throw std::runtime_error("could not load font");
            textObj.setFont(font);
        }
        Task GetTask() { return task; }
        std::optional<int> getId() { return task.getId(); }

    };
}