#include <SFML/Graphics.hpp>
#include "Task.h"
#include "Utilities.h"
#include "Icon.h"
#include "../Board/Column.h"
#include "GUIElement.h"
#include "../Board/Column.h"
#include "TaskCard.h"
#include <optional>
using namespace std;

void Kanban::TaskCard::DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) {
    // draw task name
    Utilities::DrawText(target, textObj, size, basePos, task_.getName(), size.y / 4);

    int x = basePos.x + size.x - icons_[Delete]->GetWidth();
    int y = basePos.y;
    icons_[Delete]->Draw(x, y,target);

    if (task_.isOverdue())
    {
        x = basePos.x - icons_[Overdue]->GetWidth() / 4;
        y = basePos.y + size.y - icons_[Overdue]->GetWidth();
        icons_[Overdue]->Draw(x, y, target);
    }
}


Kanban::TaskCard::TaskCard(Column* column, Task& task):
    GUIElement(sf::Color(190, 190, 190, 255)), task_(task), icons_(2) {
    if (!font.loadFromFile(Utilities::fontPath))
        throw std::runtime_error("could not load font");
    textObj.setFont(font);

    column_ = column;
    icons_[Overdue] = new Icon(Icon::Type::overdue, sf::Color::Red, 0.5f);
    icons_[Delete] = new Icon(Icon::Type::minus, sf::Color(96,96,96,255), 0.5f);
}

Kanban::TaskCard::~TaskCard() {
    // Safely delete icons
    for (unsigned int i = 0; i < icons_.size(); i++) {
        if (icons_[i]) {
            delete icons_[i];
            icons_[i] = nullptr;
        }
    }
    icons_.clear();
}

bool Kanban::TaskCard::CheckCollision(sf::Vector2f point)
{
    // Check delete icon FIRST, before any other processing
    for (unsigned int i = 0; i < icons_.size(); i++)
    {
        if (i == Icons::Delete && icons_[i] && icons_[i]->CheckCollision(point))
        {

            // Store column pointer before we get deleted
            Column* col = column_;
            
            // Tell the column to remove this TaskCard
            col->RemoveTaskCard(this);
            
            // Return true to indicate the collision was handled
            return true;
        }
    }

    // Only check bounding box if we didn't delete the object
    return rect.getGlobalBounds().contains(point);
}

bool Kanban::TaskCard::ContainsPoint(const sf::Vector2f& point) const {
    return boundingBox_.getGlobalBounds().contains(point);
}