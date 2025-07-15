#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Column.h"
#include "Task.h"
#include "TaskManager.h"
#include "Board.h"
using namespace std;

Kanban::Board::Board(const sf::RenderTarget& target, TaskManager& taskManager) :
    userInputMode(UserInputMode::Default), boardView(target.getDefaultView()), activeColumn(nullptr)
{
    taskManager_ = &taskManager;
    // boardView.setViewport(sf::FloatRect(0.1f, 0.15f, 0.75f, 0.75f));
}

Kanban::Board::~Board() {
    for (unsigned int i = 0; i < columns_.size(); i++)
        delete columns_[i];
}

bool Kanban::Board::AddColumn(const string& name, const sf::RenderTarget& target,
    WindowPromptManager& windowPromptManager)
{
    for (unsigned int i = 0; i < columns_.size(); i++)
    {
        if (columns_[i]->GetName() == name)
        {
            cout << "column name already taken" << endl;
            return false;
        }
    }
    auto targetSize = target.getSize();
    int width =  targetSize.x / (colPerScreen + 1);
    int height = targetSize.y * 0.75f;
    columns_.push_back(new Column(name, width, height, windowPromptManager, *this));
    return true;
}

void Kanban::Board::RemoveColumn(Column& column)
{
    for (auto iter = columns_.begin(); iter != columns_.end(); ++iter)
    {
        if (*iter == &column)
        {
            delete *iter;
            columns_.erase(iter);
            return;
        }
    }
}

void Kanban::Board::SetActiveColumn(Column* column)
{
    activeColumn = column;
    userInputMode = UserInputMode::ColumnName;
}

// bool Kanban::Board::AddTaskToColumn(string colName, Task& task) {
//     for (unsigned int i = 0; i < columns.size(); i++)
//     {
//         if (columns[i]->GetName() == colName)
//             return columns[i]->AddTask(task);
//     }
//     return false;
// }

void Kanban::Board::MoveView(sf::Keyboard::Key key, const float deltaTime) {
    if (key == sf::Keyboard::Left)
    {
        boardView.move(-200 * deltaTime, 0.f);
    }
    else if (key == sf::Keyboard::Right)
    {
        boardView.move(200 * deltaTime, 0.f);
    }
}

// todo: add functionality to delete tasks that were removed from TaskManager
void Kanban::Board::Update()
{
    // add new tasks
    auto allTasks = taskManager_->getAllTasks();
    for (Task& task : allTasks)
    {
        if (task.getId().has_value() && taskIds_.find(task.getId().value()) == taskIds_.end())
            taskIds_[task.getId().value()] = Available;
    }
}
// Task Kanban::Board::GetTask(int id)
// {
//     auto kvp = taskIds_.find(id);
//     if (kvp == taskIds_.end() || kvp->second == Taken)
//         return {};
//
//     return taskManager_->getTask(id).value_or(Task());
// }
void Kanban::Board::SetTaskAsTaken(Task& task)
{
    if (task.getId().has_value())
    {
        int id = task.getId().value();
        auto kvp = taskIds_.find(id);
        if (kvp != taskIds_.end() && kvp->second == Available)
            taskIds_[id] = Taken;
    }
}

void Kanban::Board::ReturnTask(std::optional<int> id)
{
    if (id.has_value() && taskIds_.find(id.value()) != taskIds_.end())
        taskIds_[id.value()] = Available;
}

// void Kanban::Board::ReturnTasks(vector<Task>& tasks)
// {
//     for (auto& task : tasks)
//         taskIds_[task.getId().value_or(-1)] = Available;
// }
vector<Task> Kanban::Board::GetAvailableTasks() const
{
    vector<Task> tasks;
    for (auto& kvp : taskIds_)
    {
        if (kvp.second == Available)
        {
            auto id = taskManager_->getTask(kvp.first);
            if (id.has_value())
                tasks.push_back(id.value());
        }
    }

    return tasks;
}

void Kanban::Board::ReadUserInput(char c)
{
    if (userInputMode == UserInputMode::Default)
        return;

    // std::cout << "ASCII character typed: " << c << std::endl;

    if (c == '\t')
        return;

    if (c == '\b')
        userInputStr.pop_back();
    else if (c == '\n')
    {
        if (activeColumn && userInputMode == UserInputMode::ColumnName)
        {
            activeColumn = nullptr;
            userInputStr.clear();
            userInputMode = UserInputMode::Default;
            return;
        }
    }
    else
        userInputStr.push_back(c);

    if (activeColumn && userInputMode == UserInputMode::ColumnName)
        activeColumn->SetName(userInputStr);
}

void Kanban::Board::DrawBoard(sf::RenderWindow& window)
{
    // columns don't resize based on number, they 'tile'

    window.setView(boardView);

    sf::RectangleShape r(sf::Vector2f(window.getSize().x, window.getSize().y));
    r.setFillColor(sf::Color(160,160,160,255));
    window.draw(r);

    const int colCount = columns_.size();
    auto targetSize = window.getSize();
    int width =  targetSize.x / (colPerScreen + 1);
    int height = targetSize.y * 0.75f;
    int xPadding = width / (colPerScreen + 1);
    int yOffset = (targetSize.y - height) / 2;
    for (int i = 0; i < colCount; i++)
    {
        int xOffset = (i+1) * xPadding + width * i;
        columns_[i]->Render(sf::Vector2f(xOffset, yOffset), window);
    }

    window.setView(window.getDefaultView());
}

bool Kanban::Board::CheckCollision(sf::Vector2i point, sf::RenderWindow& target)
{
    auto mousePos = target.mapPixelToCoords(point, boardView);

    // reset active column
    if (activeColumn)
    {
        activeColumn = nullptr;
        userInputMode = UserInputMode::Default;
        userInputStr.clear();
    }

    for (unsigned int i = 0; i < columns_.size(); i++)
    {
        if (columns_[i]->CheckCollision(mousePos))
            return true;
    }
    return false;
}
