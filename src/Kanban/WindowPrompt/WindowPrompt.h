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
class AddTaskWindowPrompt final : public WindowPrompt, public EventSystem::TaskSubject
{
    const int defaultKey = -1;
    sf::RectangleShape bg;
    sf::Color bgColor = sf::Color(128, 128, 128, 255);
    std::unordered_map<int, Kanban::TaskOption*> taskElements_;
public:
    AddTaskWindowPrompt(const sf::RenderWindow& target, Kanban::Board& board);

    ~AddTaskWindowPrompt();

    void ClearTaskElements();

    // todo: add option to get tasks by filter
    void Update() override;

    void Deactivate();

    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override;

    void Draw(sf::RenderTarget& target) override;
};

class SettingsWindowPrompt final : public WindowPrompt, public EventSystem::ActionSubject
{
    const int defaultKey = -1;
    sf::RectangleShape bg;
    sf::Color bgColor = sf::Color(128, 128, 128, 255);
    enum OptionEnum { Rename, Delete };

    static const char* OptionEnumToString(OptionEnum value);

    class SettingsOption : public Kanban::GUIElement
    {
        OptionEnum type_;
        string name_;
        void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override;

    public:
        SettingsOption(OptionEnum type);
        OptionEnum GetType();
    };
    SettingsOption* options_[2];

public:
    SettingsWindowPrompt(const sf::RenderWindow& target, Kanban::Board& board);

    ~SettingsWindowPrompt();

    // todo: add option to get tasks by filter
    void Update() override;

    void Deactivate();

    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override;

    void Draw(sf::RenderTarget& target) override;
};
