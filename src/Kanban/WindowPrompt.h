#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../Utilities/EventSystem/Subject.h"
#include "../Utilities/Utilities.h"
#include "TaskOption.h"
#include "TaskManager.h"
#include "Task.h"

using std::cout;
using std::endl;

// todo: figure out how to inherit subject
class WindowPrompt
{
public:
    enum Type { Default, AddTaskPrompt, SettingsPrompt};

    virtual ~WindowPrompt() {}

    virtual void Update() = 0;
    virtual void Draw(sf::RenderTarget& target) = 0;
    virtual bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) = 0;

    Type GetType() const { return type_; }
    bool IsVisible() const { return isVisible; }
    bool IsActive() const { return isActive; }
    virtual void SetActive(bool value) { isActive = value; }
protected:
    // struct TaskEvent : public EventSystem::TaskSubject
    // {
    //     struct DummyPrompt;
    //     class AddTaskWindowPrompt;
    //
    //     friend WindowPrompt;
    //     friend DummyPrompt;
    //     friend AddTaskWindowPrompt;
    // };
    // TaskEvent taskEvent;
    sf::Text text_;
    Type type_ = Default;
    bool isVisible = false;
    bool isActive = false;
    TaskManager* taskManager_ = nullptr;
    sf::View view_;
};
struct DummyPrompt : public WindowPrompt
{
    DummyPrompt() { type_ = Default; }
    void Update() override {}
    void Draw(sf::RenderTarget& target) override {}
    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override { return false; }
};
class AddTaskWindowPrompt final : public WindowPrompt, public EventSystem::TaskSubject
{
    const int defaultKey = -1;
    sf::RectangleShape bg;
    sf::Color bgColor = sf::Color(128, 128, 128, 255);
    std::unordered_map<int, Kanban::TaskOption*> taskElements_;
public:
    AddTaskWindowPrompt(const sf::RenderWindow& target, TaskManager& taskManager, const Type type = AddTaskPrompt)
    {
        type_ = type;
        taskManager_ = &taskManager;
        view_ = target.getDefaultView();
        view_.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 0.5f));
        bg.setFillColor(bgColor);

        sf::Font font;
        if (!font.loadFromFile(Utilities::fontPath))
            throw std::runtime_error("could not load font");
        text_.setFont(font);
    }
    ~AddTaskWindowPrompt()
    {
        ClearTaskElements();
    }

    void ClearTaskElements()
    {
        for (auto& kvp : taskElements_)
            delete kvp.second;
        taskElements_.clear();
    }

    // todo: add option to get tasks by filter
    void Update() override
    {
        if (isActive) isVisible = true;

        // send tasksToDeliver_ to observers
        if (!tasksToDeliver_.empty())
            Notify(EventSystem::Observer::AddTask);

        // update window prompt with current tasks
        if (taskElements_.empty())
        {
            auto allTasks = taskManager_->getAllTasks();
            for (Task& task : allTasks)
                taskElements_[task.getId().value_or(defaultKey)] = new Kanban::TaskOption(task);
        }
    }

    void Deactivate()
    {
        isActive = false;
        isVisible = false;
        RemoveAllObservers();
    }

    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override
    {
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
                // move task to be sent to observers
                tasksToDeliver_.push_back(iter->second->GetTask());
                taskElements_.erase(iter);
                delete iter->second;
                return true;
            }
        }

        return true;
    }

    void Draw(sf::RenderTarget& target) override
    {
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
};

class SettingsWindowPrompt final : public WindowPrompt, public EventSystem::ActionSubject
{
    const int defaultKey = -1;
    sf::RectangleShape bg;
    sf::Color bgColor = sf::Color(128, 128, 128, 255);
    // std::unordered_map<int, Kanban::TaskOption*> taskElements_;
public:
    SettingsWindowPrompt(const sf::RenderWindow& target, TaskManager& taskManager, const Type type = SettingsPrompt)
    {
        type_ = type;
        taskManager_ = &taskManager;
        view_ = target.getDefaultView();
        view_.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 0.5f));
        bg.setFillColor(bgColor);

        sf::Font font;
        if (!font.loadFromFile(Utilities::fontPath))
            throw std::runtime_error("could not load font");
        text_.setFont(font);
    }
    // ~SettingsWindowPrompt()
    // {
    //     ClearTaskElements();
    // }

    // void ClearTaskElements()
    // {
    //     for (auto& kvp : taskElements_)
    //         delete kvp.second;
    //     taskElements_.clear();
    // }

    // todo: add option to get tasks by filter
    void Update() override
    {
        if (isActive) isVisible = true;

        Notify(EventSystem::Observer::Action, EventSystem::ActionObserver::Rename);

        // update window prompt with current tasks
        // if (taskElements_.empty())
        // {
        //     auto allTasks = taskManager_->getAllTasks();
        //     for (Task& task : allTasks)
        //         taskElements_[task.getId().value_or(defaultKey)] = new Kanban::TaskOption(task);
        // }
    }

    void Deactivate()
    {
        isActive = false;
        isVisible = false;
        RemoveAllObservers();
    }

    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override
    {
        auto mousePos = target.mapPixelToCoords(point, view_);

        // exit early if not visible
        if (!isVisible || !bg.getGlobalBounds().contains(mousePos))
        {
            // if point was outside of bounds...
            if (isActive && isVisible)
                Deactivate();

            return false;
        }

        // for (auto iter = taskElements_.begin(); iter != taskElements_.end(); ++iter)
        // {
        //     if (iter->second->CheckCollision(mousePos))
        //     {
        //         // move task to be sent to observers
        //         tasksToDeliver_.push_back(iter->second->GetTask());
        //         taskElements_.erase(iter);
        //         delete iter->second;
        //         return true;
        //     }
        // }

        return true;
    }

    void Draw(sf::RenderTarget& target) override
    {
        if (!isVisible)
            return;

        target.setView(view_);

        bg.setSize(sf::Vector2f(target.getSize().x, target.getSize().y));
        target.draw(bg);

        // if (taskElements_.empty())
        //     return;
        //
        // auto size = target.getSize();
        // int taskCount = taskElements_.size();
        // float taskWidth = size.x * 0.75f;
        // float xOffset = size.x / 8.0f;
        // float taskHeight = size.y / (taskCount + 1.0f);
        // float yPadding = taskHeight / (taskCount + 1.0f);
        // int i = 0;
        // for (auto iter = taskElements_.begin(); iter != taskElements_.end(); ++iter)
        // {
        //     float yOffset = (i+1) * yPadding + taskHeight * i;
        //     iter->second->Draw(sf::Vector2f(xOffset, yOffset),
        //                         sf::Vector2f(taskWidth, taskHeight), {}, target);
        //     i++;
        // }

        target.setView(target.getDefaultView());
    }
};