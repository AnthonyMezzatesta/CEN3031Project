#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <unordered_map>
#include "Task.h"
#include "TaskManager.h"
#include "Utilities.h"
#include "../Utilities/EventSystem/Subject.h"
#include "../GUIElement/TaskOption.h"
#include "../Board/Board.h"

namespace Kanban {
    class Board;
}

using std::cout;
using std::endl;

class WindowPrompt
{
public:
    enum Type { Default, AddTaskPrompt, SettingsPrompt, TaskDetailsPrompt, ReminderPrompt };

    virtual ~WindowPrompt() {}

    virtual void Update() = 0;
    virtual void Draw(sf::RenderTarget& target) = 0;
    virtual bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) = 0;

    bool isActive = false;
    Type GetType() const { return type_; }
    bool IsVisible() const { return isVisible; }
    bool isVisible = false; // Moved to public for easier access
    bool IsActive() const { return isActive; }
    virtual void SetActive(bool value) { isActive = value; }
protected:
    // sf::Text text_;
    Type type_ = Default;
    Kanban::Board* board_ = nullptr;
    sf::View view_;
};