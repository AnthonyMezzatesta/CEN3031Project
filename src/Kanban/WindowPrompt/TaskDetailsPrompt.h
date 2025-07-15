#pragma once
#include <SFML/Graphics.hpp>
#include "../Utilities/EventSystem/Subject.h"
#include "WindowPrompt.h"
#include "../Board/Board.h"

class TaskDetailsPrompt final : public WindowPrompt, public EventSystem::TaskSubject
{
    // enum OptionEnum { Name, Description };
    // static const char* OptionEnumToString(OptionEnum value);
    //
    // class SettingsOption : public Kanban::GUIElement
    // {
    //     OptionEnum type_;
    //     string name_;
    //     void DrawDetails(sf::RenderTarget& target, sf::Vector2f size, sf::Vector2f basePos) override;
    //
    // public:
    //     SettingsOption(OptionEnum type);
    //     OptionEnum GetType();
    // };
    // SettingsOption* options_[2];

    sf::RectangleShape bg_;
    sf::Color bgColor_ = sf::Color(128, 128, 128, 255);
    Task task_;
public:
    TaskDetailsPrompt(const sf::RenderWindow& target, Kanban::Board& board);
    ~TaskDetailsPrompt() {}

    void SetTask(const Task& task) { task_ = task; }
    void Update() override;
    void Deactivate();

    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override;
    void Draw(sf::RenderTarget& target) override;
};