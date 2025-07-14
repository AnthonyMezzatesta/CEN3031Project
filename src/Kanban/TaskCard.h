#pragma once
#include <SFML/Graphics.hpp>
#include "../../include/TaskManager.h"
#include "../../include/Task.h"
#include "../Utilities/Utilities.h"
#include "Element.h"
#include "Task.h"

namespace Kanban
{
    class TaskCard : public Element
    {
        Task task_;
        Icon overdueIcon_;

        void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override
        {
            // draw task name
            Utilities::DrawText(target, textObj, size, basePos, task_.getName(), 16/*size.y * 0.15*/);

            if (task_.isOverdue())
            {
                int x = basePos.x + size.x - Icon::GetWidth();
                int y = basePos.y + size.y - Icon::GetWidth();
                // overdueIcon_.SetScale(2);
                overdueIcon_.Draw(x, y, target);
            }
        }
    public:
        TaskCard(Task& task) : Element(sf::Color(190, 190, 190, 255)),
            task_(task), overdueIcon_(Icon::Type::overdue, sf::Color::Red)
        {
            if (!font.loadFromFile(Utilities::fontPath))
                throw std::runtime_error("could not load font");
            textObj.setFont(font);
        }
        std::optional<int> getId() { return task_.getId(); }

    };
}