#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Icon.h"
#include "TaskCard.h"
#include "Element.h"
#include "TaskManager.h"
#include "Task.h"
#include "Column.h"
#include "WindowPromptManager.h"

using namespace std;

Column::Column(const string& name, const float width, const float height,
    WindowPromptManager& windowPromptManager) :
    name_(name), size_(width, height), rect_(size_)
{
    font_.loadFromFile(Utilities::fontPath);
    text_.setFont(font_);
    icons_.push_back(new Icon(Icon::Types::plus));
    icons_.push_back(new Icon(Icon::Types::dots));
    AddObserver(&windowPromptManager);
}

Column::~Column() {
    for (unsigned int i = 0; i < icons_.size(); i++)
        delete icons_[i];
    for (unsigned int i = 0; i < tasks_.size(); i++)
        delete tasks_[i];
}

void Column::OnNotify(Event event, vector<Task>& tasks)
{
    if (event == Event::AddTask)
    {
        const int prevSize = tasks_.size();
        tasks_.resize(prevSize + tasks.size());
        for (unsigned int i = 0; i < tasks.size(); i++)
            tasks_[i + prevSize] = new Kanban::TaskCard(tasks[i]);
    }
}

bool Column::AddTask(Task& task) {
    for (unsigned int i = 0; i < tasks_.size(); i++)
    {
        if (tasks_[i]->getId() == task.getId())
            return false;
    }
    tasks_.push_back(new Kanban::TaskCard(task));
    return true;
}

bool Column::RemoveTask(Kanban::TaskCard& task) {
    for (auto iter = tasks_.begin(); iter != tasks_.end(); ++iter)
    {
        if ((*iter)->getId() == task.getId())
        {
            delete *iter;
            tasks_.erase(iter);
            return true;
        }
    }
    return false;
}

void Column::SelectIcon(Icon::Types type) {
    switch (type)
    {
        case Icon::Types::plus:
            cout << "plus icon selected" << endl;
            Notify(EventSystem::Observer::ShowPrompt,
                   EventSystem::ColumnPromptObserver::AddTask,
                   this);
            break;
        default:
            break;
    }
}

void Column::SelectTask(Kanban::TaskCard* task) {
    task->Select();
}

bool Column::CheckCollision(sf::Vector2f point) {
    if (rect_.getGlobalBounds().contains(point))
    {
        for (unsigned int i = 0; i < icons_.size(); i++)
        {
            if (icons_[i]->CheckCollision(point))
            {
                SelectIcon(icons_[i]->GetType());
                return true;
            }
        }
        for (unsigned int i = 0; i < tasks_.size(); i++)
        {
            if (tasks_[i]->CheckCollision(point))
            {
                SelectTask(tasks_[i]);
                return true;
            }
        }
        return true;
    }
    return false;
}

void Column::RenderIcons(sf::RenderTarget& target, sf::Vector2f basePos) {
    int iconCount = icons_.size();

    const int iconWidth = Icon::GetWidth();
    int bgXPadding = iconWidth / 2;
    int iconXPadding = iconWidth;

    int totalIconWidth = (bgXPadding * iconCount) + (iconXPadding * iconCount) + (iconWidth * iconCount);
    int x = basePos.x + size_.x - totalIconWidth;
    int y = basePos.y + iconWidth;

    for (int i = 0; i < iconCount; i++)
    {
        int xOffset = (bgXPadding * (i+1)) + (iconXPadding * i) + (iconWidth * i);
        icons_[i]->Draw(x + xOffset, y, target);
    }
}

void Column::Render(sf::Vector2f position, sf::RenderTarget& target, const int tasksPerColumn) {

    // draw background
    rect_.setSize(size_);
    rect_.setPosition(position);
    target.draw(rect_);

    int iconHeight = Icon::GetWidth() * 2;
    int yheader = iconHeight + iconHeight / 2.f;
    if (!tasks_.empty())
    {
        int taskCount = tasksPerColumn;
        float taskWidth = size_.x * 0.75f;
        float xOffset = size_.x / 8.0f;

        float taskHeight = (size_.y - yheader) / (taskCount + 1.0f);
        float yPadding = taskHeight / (taskCount);

        for (int i = 0; i < tasks_.size(); i++)
        {
            float yOffset = (yPadding * i) + (taskHeight * i) + yheader;
            tasks_[i]->Draw(sf::Vector2f(position.x + xOffset, position.y + yOffset),
                                sf::Vector2f(taskWidth, taskHeight), {}, target);
        }
    }

    RenderIcons(target, position);

    // draw column title
    int titleWidth = size_.x;
    int titleHeight = yheader;
    sf::Vector2f textSize(titleWidth, titleHeight);
    Utilities::DrawText(target, text_, textSize, position, name_, 24);
}
