#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Column.h"
#include "Task.h"
#include "TaskManager.h"
#include "Board.h"
using namespace std;

// ===================================== Private Functions =====================================

void Kanban::Board::UpdateColumnValues(const sf::RenderWindow& window)
{
    auto targetSize = window.getSize();

    // colPerScreen+1 is the number of columns that fit in one screen, if drawn side-by-side
    // where the +1 column is the extra space that provides even padding between the columns
    colWidth_ =  targetSize.x / (colPerScreen + 1);
    colHeight_ = targetSize.y * 0.75f;
    colPaddingX_ = colWidth_ / (colPerScreen + 1);
    colPosY_ = (targetSize.y - colHeight_) / 2;
}

void Kanban::Board::UpdateRenderTexture(const sf::RenderWindow& window)
{
    const int colCount = columns_.size();
    auto targetSize = window.getSize();

    // calculating width of all columns and their padding, plus another column and padding for the addColumnButton
    unsigned int rendTextWidth = (colCount+2) * colPaddingX_ + colWidth_ * (colCount+1);
    rendTextWidth = max(rendTextWidth, targetSize.x);

    // don't recreate texture if there's been no change to dimensions
    if (rendTextWidth == renderTexture_.getSize().x)
        return;

    if (!renderTexture_.create(rendTextWidth, targetSize.y))
    {
        cerr << "could not create render texture in Board.Draw()" << endl;
    }
}

void Kanban::Board::UpdateBoardView(const sf::RenderWindow& window, const float deltaTime)
{
    float defaultCenterX = window.getSize().x/2.f;
    float maxDistX = (columns_.size() - colPerScreen + 1) * (colWidth_ + colPaddingX_);
    float maxCenterX = defaultCenterX + maxDistX;

    // move boardView, according to position of scroll bar, independent of frame rate
    float targetPosX = Utilities::LerpClamped(defaultCenterX, maxCenterX, scrollMoveDelta_);
    if (targetPosX != boardView.getCenter().x)
    {
        float originalCenterY = boardView.getCenter().y;
        float currPosX = boardView.getCenter().x;
        float velocity = BOARD_SCROLL_SPEED * deltaTime; // pixels per second

        // move towards target position
        boardView.setCenter(Utilities::LerpClamped(currPosX, targetPosX, velocity), originalCenterY);

        // clamp position if close enough
        if (std::abs(targetPosX - boardView.getCenter().x) < 0.0001f)
            boardView.setCenter(targetPosX, originalCenterY);
    }

    // clamp position of boardView so that it is within bounds of renderTexture
    // useful for adjusting view when columns are removed
    if (boardView.getCenter().x > maxCenterX)
        boardView.setCenter(maxCenterX, boardView.getCenter().y);
}

void Kanban::Board::UpdateScrollBar(const sf::RenderWindow& window, const float deltaTime)
{
    // only allow scrolling view when columnCount exceeds the columns drawn per screen
    // note: this includes the add column button
    if (columns_.size() >= colPerScreen)
    {
        float scrollBarWidthRatio = window.getSize().x / static_cast<float>(renderTexture_.getSize().x);
        scrollBar_.Enable();
        scrollMoveDelta_ = scrollBar_.Update(scrollBarWidthRatio, deltaTime);
    }
    else
    {
        scrollMoveDelta_ = 0;
        scrollBar_.Disable();
    }
}

void Kanban::Board::DrawColumns(sf::RenderWindow& window) {
    const int colCount = columns_.size();
    unsigned int rendTextWidth = (colCount+2) * colPaddingX_ + colWidth_ * (colCount+1);

    // drawing to render texture to allow for off-screen drawing
    renderTexture_.clear();

    // draw background
    sf::RectangleShape r(sf::Vector2f(renderTexture_.getSize().x, renderTexture_.getSize().y));
    r.setFillColor(sf::Color(160,160,160,255));
    renderTexture_.draw(r);

    // draw columns
    for (int i = 0; i < colCount; i++)
    {
        int xOffset = (i+1) * colPaddingX_ + colWidth_ * i;
        columns_[i]->Render(sf::Vector2f(xOffset, colPosY_), sf::Vector2f(colWidth_, colHeight_), renderTexture_);
    }

    // draw add column button at next column pos
    int addColumnPosX = rendTextWidth - colWidth_ - colPaddingX_;
    addColumnButton_.Draw(sf::Vector2f(addColumnPosX, colPosY_), sf::Vector2f(colWidth_, colHeight_), {}, renderTexture_);

    // draw render texture within coordinate space of boardView
    window.setView(boardView);
    renderTexture_.display();
    window.draw(sf::Sprite(renderTexture_.getTexture()));

    // reset window's view
    window.setView(window.getDefaultView());
}

// ====================================== Public Functions ======================================

Kanban::Board::Board(const sf::RenderWindow& target, TaskManager& taskManager, ReminderManager& reminderManager) :
    userInputMode(UserInputMode::Default), scrollBar_(target), boardView(target.getDefaultView()), activeColumn(nullptr)
{
    UpdateColumnValues(target);
    UpdateRenderTexture(target);
    windowPromptManager_ = new WindowPromptManager(target, *this, reminderManager);
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

void Kanban::Board::ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target)
{
    if (!scrollBarActive)
        return;

    scrollMoveDelta_ = scrollBar_.Move(pixelPos, target);
}

void Kanban::Board::ProcessKeyEvent(const sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::F)
    {
        windowPromptManager_->ShowReminderPrompt();
        return;
    }
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

// todo: add functionality to delete tasks that were removed from TaskManager
void Kanban::Board::Update(const sf::RenderWindow& target, const float deltaTime)
{
    UpdateColumnValues(target);
    UpdateRenderTexture(target);
    UpdateBoardView(target, deltaTime);
    UpdateScrollBar(target, deltaTime);

    // add new tasks
    auto allTasks = taskManager_->getAllTasks();
    for (Task& task : allTasks)
    {
        if (task.getId().has_value() && taskIds_.find(task.getId().value()) == taskIds_.end())
            taskIds_[task.getId().value()] = Available;
    }

    windowPromptManager_->UpdatePrompts();
}

bool Kanban::Board::CheckCollision(sf::Vector2i point, sf::RenderWindow& target)
{
    // reset active column
    if (activeColumn)
    {
        activeColumn = nullptr;
        userInputMode = UserInputMode::Default;
        userInputStr.clear();
    }

    scrollBarActive = scrollBar_.CheckCollision(point, target);
    if (scrollBarActive)
        return true;

    if (windowPromptManager_->CheckCollision(point, target))
        return true;

    auto mousePos = target.mapPixelToCoords(point, boardView);

    if (addColumnButton_.CheckCollision(mousePos))
    {
        cout << "adding column" << endl;
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

void Kanban::Board::Draw(sf::RenderWindow& window)
{
    DrawColumns(window);
    windowPromptManager_->Draw(window);
    scrollBar_.Draw(window);
}