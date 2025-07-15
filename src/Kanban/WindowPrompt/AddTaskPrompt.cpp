#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include "Task.h"
#include "Utilities.h"
#include "Subject.h"
#include "../GUIElement/TaskOption.h"
#include "../Board/Board.h"
#include "AddTaskPrompt.h"

AddTaskPrompt::AddTaskPrompt(const sf::RenderWindow& target, Kanban::Board& board) {
    type_ = WindowPrompt::Type::AddTaskPrompt;
    board_ = &board;
    view_ = target.getDefaultView();
    view_.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 0.5f));
    bg.setFillColor(bgColor);

    // sf::Font font;
    // if (!font.loadFromFile(Utilities::fontPath))
    //     throw std::runtime_error("could not load font");
    // text_.setFont(font);
}

AddTaskPrompt::~AddTaskPrompt() {
    ClearTaskElements();
}

void AddTaskPrompt::ClearTaskElements() {
    for (auto& kvp : taskElements_)
        delete kvp.second;
    taskElements_.clear();
}

// todo: add option to get tasks by filter
void AddTaskPrompt::Update() {
    if (isActive) isVisible = true;

    // update window prompt with currently available tasks
    if (!isActive)
    {
        ClearTaskElements();
        auto allTasks = board_->GetAvailableTasks();
        for (Task& task : allTasks)
        {
            if (task.getId().has_value())
                taskElements_[task.getId().value()] = new Kanban::TaskOption(task);
        }
    }
}

void AddTaskPrompt::Deactivate() {
    isActive = false;
    isVisible = false;
    RemoveAllObservers();
}

bool AddTaskPrompt::CheckCollision(sf::RenderWindow& target, sf::Vector2i point) {
    auto mousePos = target.mapPixelToCoords(point, view_);

    // exit early if not visible
    if (!isVisible || !bg.getGlobalBounds().contains(mousePos))
    {
        // if point was outside of bounds...
        if (isActive && isVisible)
            Deactivate();

        return false;
    }

    for (auto iter = taskElements_.begin(); iter != taskElements_.end(); ++iter)
    {
        if (iter->second->CheckCollision(mousePos))
        {
            // send task to observers
            Notify(EventSystem::Observer::TransferTask, iter->second->GetTask());
            taskElements_.erase(iter);
            delete iter->second;
            return true;
        }
    }

    return true;
}

void AddTaskPrompt::Draw(sf::RenderTarget& target) {
    if (!isVisible)
        return;

    target.setView(view_);

    bg.setSize(sf::Vector2f(target.getSize().x, target.getSize().y));
    target.draw(bg);

    if (taskElements_.empty())
        return;

    auto size = target.getSize();
    int taskCount = taskElements_.size();
    float taskWidth = size.x * 0.75f;
    float xOffset = size.x / 8.0f;
    float taskHeight = size.y / (taskCount + 1.0f);
    float yPadding = taskHeight / (taskCount + 1.0f);
    int i = 0;
    for (auto iter = taskElements_.begin(); iter != taskElements_.end(); ++iter)
    {
        float yOffset = (i+1) * yPadding + taskHeight * i;
        iter->second->Draw(sf::Vector2f(xOffset, yOffset),
                           sf::Vector2f(taskWidth, taskHeight), {}, target);
        i++;
    }

    target.setView(target.getDefaultView());
}
