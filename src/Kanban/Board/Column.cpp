#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "TaskManager.h"
#include "Task.h"
#include "Column.h"
#include "Board.h"
#include "../GUIElement/GUIElement.h"
#include "../GUIElement/Icon.h"
#include "../GUIElement/TaskCard.h"
#include "../WindowPrompt/WindowPromptManager.h"
#include "../Utilities/EventSystem/Subject.h"

using namespace std;
using namespace EventSystem;

void Kanban::Column::ActionObserver::OnNotify(Observer::EventEnum event, Observer::ActionEnum& action)
{
    if (event == Observer::EventEnum::Action)
    {
        if (action == Observer::ActionEnum::Rename)
        {
            cout << "renaming column" << endl;
            column_->board_->SetActiveColumn(column_);
        }
        if (action == Observer::ActionEnum::Delete)
        {
            cout << "deleting column" << endl;
            column_->board_->RemoveColumn(*column_);
        }
    }
}

void Kanban::Column::TaskObserver::OnNotify(Observer::EventEnum event, Task& task)
{
    if (event == Observer::EventEnum::TransferTask)
    {
        column_->board_->SetTaskAsTaken(task);
        column_->tasks_.push_back(new Kanban::TaskCard(column_, task));
    }
}

Kanban::Column::Column(const string& name, WindowPromptManager& windowPromptManager, Kanban::Board& board) :
    name_(name), actionObserver_(this), taskObserver_(this)
{
    font_.loadFromFile(Utilities::fontPath);
    text_.setFont(font_);
    icons_.push_back(new Icon(Icon::Type::plus));
    icons_.push_back(new Icon(Icon::Type::dots));
    windowPromptManager_ = &windowPromptManager;
    board_ = &board;
    TaskSubject::AddObserver(windowPromptManager_->taskObserver_);
    selectedCard_ = nullptr;
}

Kanban::Column::~Column() {
    for (unsigned int i = 0; i < icons_.size(); i++)
        delete icons_[i];
    for (unsigned int i = 0; i < tasks_.size(); i++)
    {
        board_->ReturnTask(tasks_[i]->GetId());
        delete tasks_[i];
    }
}


void Kanban::Column::RemoveTaskCard(Kanban::TaskCard* card)
{
    for (auto iter = tasks_.begin(); iter != tasks_.end(); ++iter)
    {
        if ((*iter)->GetId() == card->GetId())
        {
            board_->ReturnTask(card->GetId());
            delete *iter;
            tasks_.erase(iter);
            return;
        }
    }
}

// bool Kanban::Column::AddTask(Task& task) {
//     for (unsigned int i = 0; i < tasks_.size(); i++)
//     {
//         if (tasks_[i]->getId() == task.getId())
//             return false;
//     }
//     tasks_.push_back(new Kanban::TaskCard(task));
//     return true;
// }

// bool Kanban::Column::RemoveTask(Kanban::TaskCard& task) {
//     for (auto iter = tasks_.begin(); iter != tasks_.end(); ++iter)
//     {
//         if ((*iter)->getId() == task.getId())
//         {
//             delete *iter;
//             tasks_.erase(iter);
//             return true;
//         }
//     }
//     return false;
// }

void Kanban::Column::SelectIcon(Icon::Type type) {
    switch (type)
    {
        case Icon::Type::plus:
            std::cout << "Plus icon clicked - showing AddTaskPrompt" << std::endl;
            windowPromptManager_->OnNotify(
                Observer::EventEnum::ShowPrompt,
                Observer::PromptEnum::AddTask,
                taskObserver_
            );
            std::cout << "AddTaskPrompt should now be visible" << std::endl;
            break;
        case Icon::Type::dots:
            std::cout << "Dots icon clicked - showing SettingsPrompt" << std::endl;
            windowPromptManager_->OnNotify(
                Observer::EventEnum::ShowPrompt,
                Observer::PromptEnum::Settings,
                actionObserver_
            );
            std::cout << "SettingsPrompt should now be visible" << std::endl;
            break;
    }
}

void Kanban::Column::SelectTaskCard(Kanban::TaskCard* card) {
    if (selectedCard_)
        selectedCard_->Deselect();

    selectedCard_ = card;
    card->Select();
    TaskSubject::Notify(Observer::EventEnum::ShowPrompt, card->GetTask());
}

bool Kanban::Column::CheckCollision(sf::Vector2f point) {
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
                SelectTaskCard(tasks_[i]);
                return true;
            }
        }
        if (selectedCard_)
        {
            selectedCard_->Deselect();
            selectedCard_ = nullptr;
        }
        return true;
    }
    if (selectedCard_)
    {
        selectedCard_->Deselect();
        selectedCard_ = nullptr;
    }
    return false;
}

void Kanban::Column::RenderIcons(sf::RenderTarget& target, sf::Vector2f basePos, int colWidth)
{
    const int iconCount = icons_.size();
    for (int i = 0; i < iconCount; i++)
    {
        const int iconWidth = icons_[i]->GetWidth();
        int bgXPadding = iconWidth / 2;
        int iconXPadding = iconWidth;

        int totalIconWidth = (bgXPadding * iconCount) + (iconXPadding * iconCount) + (iconWidth * iconCount);
        int x = basePos.x + colWidth - totalIconWidth;
        int y = basePos.y + iconWidth;

        int xOffset = (bgXPadding * (i+1)) + (iconXPadding * i) + (iconWidth * i);
        icons_[i]->Draw(x + xOffset, y, target);
    }
}

void Kanban::Column::Render(sf::Vector2f position, sf::Vector2f size, sf::RenderTarget& target, const int tasksPerColumn) {

    // draw background
    rect_.setSize(size);
    rect_.setPosition(position);
    target.draw(rect_);

    int iconHeight = Icon::GetDefaultWidth() * 2;
    int yHeader = iconHeight + iconHeight / 2.f;
    if (!tasks_.empty())
    {
        int taskCount = tasksPerColumn;
        float taskWidth = size.x * 0.75f;
        float xOffset = size.x / 8.0f;

        float taskHeight = (size.y - yHeader) / (taskCount + 1.0f);
        float yPadding = taskHeight / (taskCount);

        for (unsigned int i = 0; i < tasks_.size(); i++)
        {
            float yOffset = (yPadding * i) + (taskHeight * i) + yHeader;
            tasks_[i]->Draw(sf::Vector2f(position.x + xOffset, position.y + yOffset),
                                sf::Vector2f(taskWidth, taskHeight), {}, target);
        }
    }

    RenderIcons(target, position, size.x);

    // draw column title
    int titleWidth = size.x;
    int titleHeight = yHeader;
    sf::Vector2f textSize(titleWidth, titleHeight);
    Utilities::DrawText(target, text_, textSize, position, name_, titleHeight / 2);
}
