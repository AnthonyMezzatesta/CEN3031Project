#include <SFML/Graphics.hpp>
#include "Task.h"
#include "Utilities.h"
#include "Icon.h"
#include "../Board/Column.h"
#include "TaskCard.h"
using namespace std;

void Kanban::TaskCard::DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) {
    // draw task name
    Utilities::DrawText(target, textObj, size, basePos, task_.getName(), size.y / 4, Utilities::textColor);

    Icon* deleteIcon = icons_[Delete];
    float x = basePos.x + size.x - deleteIcon->GetWidth() * 0.625f;
    float y = basePos.y - deleteIcon->GetWidth() * 0.325f;
    deleteIcon->Draw(x, y,target);

    if (task_.isOverdue())
    {
        x = basePos.x;
        y = basePos.y + size.y - icons_[Overdue]->GetWidth();
        icons_[Overdue]->Draw(x, y, target);
    }
}

Kanban::TaskCard::TaskCard(Column* column, const Task& task, sf::Color fillColor):
    GUIElement(fillColor), task_(task), icons_(2) {
    column_ = column;
    icons_[Overdue] = new Icon(Icon::Type::overdue, 0.5f, Utilities::priorityHigh);
    icons_[Delete] = new Icon(Icon::Type::minus, 0.5f, Utilities::icon1);
}

Kanban::TaskCard::~TaskCard() {
    for (unsigned int i = 0; i < icons_.size(); i++)
        delete icons_[i];
}

void Kanban::TaskCard::UpdateIcons(int screenWidth) {
    for (auto* icon : icons_)
        icon->Update(screenWidth);
}

bool Kanban::TaskCard::CheckCollision(sf::Vector2f point)
{
    for (unsigned int i = 0; i < std::size(icons_); i++)
    {
        if (i == Icons::Delete && icons_[i]->CheckCollision(point))
        {
            column_->RemoveTaskCard(this);
            return false;
        }
    }

    return rect.getGlobalBounds().contains(point);
}
