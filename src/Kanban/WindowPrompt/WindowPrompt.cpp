#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include "Task.h"
#include "TaskManager.h"
#include "Utilities.h"
#include "Subject.h"
#include "../GUIElement/TaskOption.h"
#include "../Board/Board.h"
#include "WindowPrompt.h"

AddTaskWindowPrompt::AddTaskWindowPrompt(const sf::RenderWindow& target, Kanban::Board& board) {
    type_ = AddTaskPrompt;
    board_ = &board;
    view_ = target.getDefaultView();
    view_.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 0.5f));
    bg.setFillColor(bgColor);

    sf::Font font;
    if (!font.loadFromFile(Utilities::fontPath))
        throw std::runtime_error("could not load font");
    text_.setFont(font);
}

AddTaskWindowPrompt::~AddTaskWindowPrompt() {
    ClearTaskElements();
}

void AddTaskWindowPrompt::ClearTaskElements() {
    for (auto& kvp : taskElements_)
        delete kvp.second;
    taskElements_.clear();
}

void AddTaskWindowPrompt::Update() {
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

void AddTaskWindowPrompt::Deactivate() {
    isActive = false;
    isVisible = false;
    RemoveAllObservers();
}

bool AddTaskWindowPrompt::CheckCollision(sf::RenderWindow& target, sf::Vector2i point) {
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

void AddTaskWindowPrompt::Draw(sf::RenderTarget& target) {
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

const char* SettingsWindowPrompt::OptionEnumToString(OptionEnum value) {
    switch (value)
    {
        case Rename:
            return "Rename";
        case Delete:
            return "Delete";
        default:
            return "\0";
    }
}

void SettingsWindowPrompt::SettingsOption::DrawDetails(sf::RenderTarget& target, sf::Vector2f size,
    sf::Vector2f basePos) {
    // draw name
    Utilities::DrawText(target, textObj, size, basePos, name_, 24/*size.y * 0.15*/);
}

SettingsWindowPrompt::SettingsOption::SettingsOption(OptionEnum type): GUIElement(sf::Color(190, 190, 190, 255)), type_(type), name_(OptionEnumToString(type_)) {
    if (!font.loadFromFile(Utilities::fontPath))
        throw std::runtime_error("could not load font");
    textObj.setFont(font);
}

SettingsWindowPrompt::OptionEnum SettingsWindowPrompt::SettingsOption::GetType() { return type_; }

SettingsWindowPrompt::SettingsWindowPrompt(const sf::RenderWindow& target, Kanban::Board& board) {
    type_ = SettingsPrompt;
    board_ = &board;
    view_ = target.getDefaultView();
    view_.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 0.5f));
    bg.setFillColor(bgColor);

    sf::Font font;
    if (!font.loadFromFile(Utilities::fontPath))
        throw std::runtime_error("could not load font");
    text_.setFont(font);

    options_[0] = new SettingsOption(Rename);
    options_[1] = new SettingsOption(Delete);
}

SettingsWindowPrompt::~SettingsWindowPrompt() {
    for (auto* o : options_)
        delete o;
}

void SettingsWindowPrompt::Update() {
    if (isActive) isVisible = true;
}

void SettingsWindowPrompt::Deactivate() {
    isActive = false;
    isVisible = false;
    RemoveAllObservers();
}

bool SettingsWindowPrompt::CheckCollision(sf::RenderWindow& target, sf::Vector2i point) {
    auto mousePos = target.mapPixelToCoords(point, view_);

    // exit early if not visible
    if (!isVisible || !bg.getGlobalBounds().contains(mousePos))
    {
        // if point was outside of bounds...
        if (isActive && isVisible)
            Deactivate();

        return false;
    }

    int size = std::size(options_);
    for (int i = 0; i < size; i++)
    {
        if (options_[i]->CheckCollision(mousePos))
        {
            if (options_[i]->GetType() == OptionEnum::Rename)
            {
                Notify(EventSystem::Observer::EventEnum::Action, EventSystem::Observer::ActionEnum::Rename);
            }
            else if (options_[i]->GetType() == OptionEnum::Delete)
            {
                Notify(EventSystem::Observer::EventEnum::Action, EventSystem::Observer::ActionEnum::Delete);
            }
            Deactivate();
        }
    }

    return true;
}

void SettingsWindowPrompt::Draw(sf::RenderTarget& target) {
    if (!isVisible)
        return;

    target.setView(view_);

    bg.setSize(sf::Vector2f(target.getSize().x, target.getSize().y));
    target.draw(bg);

    auto size = target.getSize();
    int optionCount = std::size(options_);
    float optionWidth = size.x * 0.75f;
    float xOffset = size.x / 8.0f;
    float optionHeight = size.y / (optionCount + 1.0f);
    float yPadding = optionHeight / (optionCount + 1.0f);
    for (int i = 0; i < optionCount; i++)
    {
        float yOffset = (i+1) * yPadding + optionHeight * i;
        options_[i]->Draw(sf::Vector2f(xOffset, yOffset),
                          sf::Vector2f(optionWidth, optionHeight), {}, target);
    }

    target.setView(target.getDefaultView());
}