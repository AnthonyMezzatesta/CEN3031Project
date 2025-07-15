#pragma once
#include <SFML/Graphics.hpp>
#include "Task.h"
#include "Icon.h"
#include "GUIElement.h"
#include "../Board/Column.h"

namespace Kanban
{
    class Column;

    class TaskCard : public GUIElement
    {
        enum Icons { Overdue, Delete };
        Task task_;
        vector<Icon*> icons_;
        Column* column_;

        void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override;
    public:
        TaskCard(Column* column, Task& task);
        ~TaskCard();

        bool CheckCollision(sf::Vector2f point) override;
        std::optional<int> GetId() const { return task_.getId(); }
    };
}
