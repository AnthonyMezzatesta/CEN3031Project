#pragma once
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "TaskManager.h"
#include "Utilities.h"
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
            Utilities::DrawText(target, textObj, size, basePos, task.getName(), size.y / 4, Utilities::textColor);
        }
    public:
        TaskOption(Task& task) : GUIElement(Utilities::fill0), task(task)
        {
            if (!font.loadFromFile(Utilities::fontPath))
                throw std::runtime_error("could not load font");
            textObj.setFont(font);
        }

        Task& GetTask() { return task; }
        std::optional<int> getId() { return task.getId(); }

    };
}