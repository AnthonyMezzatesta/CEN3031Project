#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Icon.h"
#include "TaskCard.h"
#include "Element.h"
#include "../../include/TaskManager.h"
#include "../../include/Task.h"
#include "Column.h"
using namespace std;

Column::Column(const string& name, const float width, const float height,
    WindowPromptManager* windowPromptManager):
    name_(name), size_(width, height), rect_(size_)
{
    // if (!renderTexture_.create(size_.x, size_.y))
    // {
    //     throw runtime_error("could not create column texture");
    // }
    icons_.push_back(new Icon(Icon::Types::plus));

    this->windowPromptManager = windowPromptManager;
}

Column::~Column() {
    for (unsigned int i = 0; i < icons_.size(); i++)
        delete icons_[i];
    for (unsigned int i = 0; i < tasks_.size(); i++)
        delete tasks_[i];
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
            windowPromptManager->ShowPrompt(WindowPrompt::AddTask);
            break;
        default:
            break;
    }
}

void Column::SelectTask(Kanban::TaskCard* task) {
    task->Select();
}

bool Column::CheckCollision(sf::Vector2f point) {
    // auto pos = rect_.getPosition();
    // sf::FloatRect f(pos.x, pos.y, rect_.getLocalBounds().width, rect_.getLocalBounds().height);
    // sf::FloatRect boundingBox = sprite_.getGlobalBounds();
    
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
    // assuming all icon sprites are of same scale
    // int width = iconCount * 16 + (iconCount + 1) * 8;
    int height = 32;
    // sf::RectangleShape iconBG(sf::Vector2f(width, height));
    // iconBG.setPosition(basePos);
    // iconBG.setFillColor(sf::Color(192, 192, 192, 255));
    // target.draw(iconBG);

    int x = basePos.x;
    int y = basePos.y + height/4;
    int bgXPadding = 8;
    int iconXPadding = 16;
    for (int i = 0; i < iconCount; i++)
    {
        int xOffset = ((i+1) * bgXPadding) + (iconXPadding * i) + (16 * i);
        icons_[i]->Draw(x + xOffset, y, target);
    }
}

void Column::Render(sf::Vector2f position, sf::RenderTarget& target) {

    // draw background
    rect_.setSize(size_);
    // renderTexture_.draw(rect);
    rect_.setPosition(position);
    target.draw(rect_);

    if (!tasks_.empty())
    {
        int taskCount = tasks_.size();
        float taskWidth = size_.x * 0.75f;
        float xOffset = size_.x / 8.0f;
        float taskHeight = size_.y / (taskCount + 1.0f);
        float yPadding = taskHeight / (taskCount + 1.0f);
        for (int i = 0; i < taskCount; i++)
        {
            float yOffset = (i+1) * yPadding + taskHeight * i;
            tasks_[i]->Draw(sf::Vector2f(position.x + xOffset, position.y + yOffset),
                                sf::Vector2f(taskWidth, taskHeight), {}, target);
        }
    }

    RenderIcons(target, position);

    // sprite_.setTexture(renderTexture_.getTexture());
    // sprite_.setPosition(position);
    // target.draw(sprite_);
}
