#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "WindowResizeHandler.h"
#include "../Board/Board.h"

namespace Kanban {
    class Board;
}

using std::cout;
using std::endl;

class WindowPrompt
{
public:
    enum Type { Default, AddTaskPrompt, SettingsPrompt, TaskDetailsPrompt, ReminderPrompt, WindowResizePrompt };

    WindowPrompt(WindowResizeHandler& windowResizeHandler) : windowResizeObserver_(view_) {
        windowResizeHandler.AddObserver(windowResizeObserver_);
    }
    virtual ~WindowPrompt() {}

    virtual void Update(const float deltaTime) = 0;
    virtual void Draw(sf::RenderTarget& target) = 0;
    virtual void ProcessLeftClickReleased() {}
    virtual void ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target) {}
    virtual bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) = 0;

    Type GetType() const { return type_; }
    bool IsVisible() const { return isVisible; }
    bool IsActive() const { return isActive; }
    virtual void SetActive(bool value) { isActive = value; }
protected:
    Type type_ = Default;
    bool isVisible = false;
    bool isActive = false;
    Kanban::Board* board_ = nullptr;
    sf::View view_;
    sf::FloatRect viewPortLeft_  = sf::FloatRect(0.0f, 0.0f, 0.5f, 0.5f);
    sf::FloatRect viewPortRight_ = sf::FloatRect(0.5f, 0.0f, 0.5f, 0.5f);
private:
    EventSystem::WindowResizeObserver windowResizeObserver_;
};