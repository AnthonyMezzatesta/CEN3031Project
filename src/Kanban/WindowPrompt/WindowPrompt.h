#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
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

struct WindowPrompt
{
    enum Type { Default, AddTaskPrompt, SettingsPrompt, TaskDetailsPrompt };

    virtual ~WindowPrompt() {}

    virtual void Update() = 0;
    virtual void Draw(sf::RenderTarget& target) = 0;
    virtual bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) = 0;

    Type GetType() const { return type_; }
    bool IsVisible() const { return isVisible; }
    bool IsActive() const { return isActive; }
    virtual void SetActive(bool value) { isActive = value; }
protected:
    // sf::Text text_;
    Type type_ = Default;
    bool isVisible = false;
    bool isActive = false;
    Kanban::Board* board_ = nullptr;
    sf::View view_;
};
struct DummyPrompt : public WindowPrompt
{
    DummyPrompt() { type_ = Default; }
    void Update() override {}
    void Draw(sf::RenderTarget& target) override {}
    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override { return false; }
};