#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Column.h"
#include "Task.h"
#include "TaskManager.h"
#include "Board.h"
using namespace std;

void Kanban::Board::DrawColumns(sf::RenderWindow& window) {
    const int colCount = columns_.size();
    auto targetSize = window.getSize();

    int colWidth =  targetSize.x / (colPerScreen + 1);
    int colHeight = targetSize.y * 0.75f;
    int xPadding = colWidth / (colPerScreen + 1);
    int yOffset = (targetSize.y - colHeight) / 2;

    // drawing to render texture to allow for off-screen drawing
    sf::RenderTexture renderTexture;
    unsigned int rendTextWidth = (colCount+2) * xPadding + colWidth * (colCount+1);
    if (!renderTexture.create(max(rendTextWidth, targetSize.x), targetSize.y))
    {
        cerr << "could not create render texture in Board.Draw()" << endl;
        return;
    }

    // draw background
    sf::RectangleShape r(sf::Vector2f(renderTexture.getSize().x, renderTexture.getSize().y));
    r.setFillColor(sf::Color(160,160,160,255));
    renderTexture.draw(r);

    // draw columns
    for (int i = 0; i < colCount; i++)
    {
        int xOffset = (i+1) * xPadding + colWidth * i;
        columns_[i]->Render(sf::Vector2f(xOffset, yOffset), sf::Vector2f(colWidth, colHeight), renderTexture);
    }

    // draw add column button at next column pos
    int addColumnPosX = rendTextWidth - colWidth - xPadding;
    addColumnButton_.Draw(sf::Vector2f(addColumnPosX, yOffset), sf::Vector2f(colWidth, colHeight), {}, renderTexture);

    // draw render texture within coordinate space of boardView
    window.setView(boardView);
    renderTexture.display();
    window.draw(sf::Sprite(renderTexture.getTexture()));

    // reset window's view
    window.setView(window.getDefaultView());
}

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

Kanban::Board::Board(const sf::RenderWindow& target, TaskManager& taskManager, ReminderManager& reminderManager) :
    userInputMode(UserInputMode::Default), boardView(target.getDefaultView()), activeColumn(nullptr)
{
    windowPromptManager_ = new WindowPromptManager(target, taskManager, *this, reminderManager);
    taskManager_ = &taskManager;
    // boardView.setViewport(sf::FloatRect(0.1f, 0.15f, 0.75f, 0.75f));
}

Kanban::Board::~Board() {
    delete windowPromptManager_;
    for (unsigned int i = 0; i < columns_.size(); i++)
        delete columns_[i];
}

void Kanban::Board::AddColumn(const string& name = "title")
{
    columns_.push_back(new Column(name, *windowPromptManager_, *this));
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
    userInputStr = column->GetName(); // Initialize with current name
    std::cout << "Column active for renaming: " << userInputStr << std::endl;
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

    RefreshTaskCards();

    windowPromptManager_->UpdatePrompts();
}

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

void Kanban::Board::ProcessKeyEvent(const sf::Keyboard::Key key)
{
    windowPromptManager_->ProcessKeyEvent(key);
}

void Kanban::Board::ReadUserInput(char c)
{
    windowPromptManager_->ReadUserInput(c);

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

void Kanban::Board::Draw(sf::RenderWindow& window)
{
    DrawColumns(window);
    windowPromptManager_->Draw(window);
}

bool Kanban::Board::CheckCollision(sf::Vector2i point, sf::RenderWindow& target)
{
    auto mousePos = target.mapPixelToCoords(point, boardView);

    if (windowPromptManager_->CheckCollision(point, target))
        return true;

    // reset active column
    if (activeColumn)
    {
        activeColumn = nullptr;
        userInputMode = UserInputMode::Default;
        userInputStr.clear();
    }

    if (addColumnButton_.CheckCollision(mousePos))
    {
        cout << "button pressed" << endl;
        AddColumn();
        return true;
    }

    for (unsigned int i = 0; i < columns_.size(); i++)
    {
        if (columns_[i]->CheckCollision(mousePos))
            return true;
    }

    return false;
}

// std::optional<Task> Kanban::Board::GetTaskAtPosition(sf::Vector2i pixelPos, sf::RenderWindow& window) {
//     // Convert pixel position to board view coordinates
//     sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, boardView);
//
//     // Loop through all columns and their task cards
//     for (auto* column : columns_) {
//         auto taskCards = column->GetTaskOptions();
//         for (auto* taskCard : taskCards) {
//             if (taskCard && taskCard->ContainsPoint(worldPos)) {
//                 return taskCard->GetTask();
//             }
//         }
//     }
//     return std::nullopt;
// }

// Added to fix segmentation fault (core dumped) crash when deleting an edited task
void Kanban::Board::RefreshTaskCards() {
    // Get all updated tasks from database
    auto allTasks = taskManager_->getAllTasks();

    // Update existing TaskCards with fresh data from database
    for (auto* column : columns_) {
        auto taskCards = column->GetTaskOptions();

        // Create a copy of the vector to iterate over, in case cards get deleted during refresh
        std::vector<Kanban::TaskCard*> cardsCopy = taskCards;

        for (auto* taskCard : cardsCopy) {
            if (taskCard && taskCard->GetId().has_value()) {
                int taskId = taskCard->GetId().value();
                bool foundInDatabase = false;

                // Find the updated task in the database
                for (const auto& updatedTask : allTasks) {
                    if (updatedTask.getId().has_value() &&
                        updatedTask.getId().value() == taskId) {
                        // Update the TaskCard's task with fresh data
                        taskCard->UpdateTask(updatedTask);
                        foundInDatabase = true;
                        break;
                    }
                }

                // If task was deleted from database, remove the TaskCard
                if (!foundInDatabase) {
                    column->RemoveTaskCard(taskCard);
                }
            }
        }
    }
}
