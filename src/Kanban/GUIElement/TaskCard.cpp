#include <SFML/Graphics.hpp>
#include "Task.h"
#include "Utilities.h"
#include "Icon.h"
#include "../Board/Column.h"
#include "GUIElement.h"
#include "../Board/Column.h"
#include "TaskCard.h"
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

Kanban::TaskCard::TaskCard(Column* column, Task& task): GUIElement(sf::Color(190, 190, 190, 255)),
                                                       task_(task), icons_(2) {
    if (!font.loadFromFile(Utilities::fontPath))
        throw std::runtime_error("could not load font");
    textObj.setFont(font);

    column_ = column;
    icons_[Overdue] = new Icon(Icon::Type::overdue, sf::Color::Red, 0.5f);
    icons_[Delete] = new Icon(Icon::Type::minus, sf::Color(96,96,96,255), 0.5f);
}

Kanban::TaskCard::~TaskCard() {
    for (int i = 0; i < icons_.size(); i++)
        delete icons_[i];
}

bool Kanban::TaskCard::CheckCollision(sf::Vector2f point)
{
    for (int i = 0; i < std::size(icons_); i++)
    {
        if (i == Icons::Delete && icons_[i]->CheckCollision(point))
        {
            column_->RemoveTaskCard(this);
            return false;
        }
    }

    return rect.getGlobalBounds().contains(point);
}