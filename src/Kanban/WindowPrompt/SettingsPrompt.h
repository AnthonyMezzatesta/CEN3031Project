#pragma once
#include <SFML/Graphics.hpp>
#include "Subject.h"
#include "WindowPrompt.h"
#include "../Board/Board.h"

class SettingsPrompt final : public WindowPrompt, public EventSystem::ActionSubject
{
    sf::RectangleShape bg;
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
    SettingsPrompt(const sf::RenderWindow& target);
    ~SettingsPrompt();

    // todo: add option to get tasks by filter
    void Update() override;
    void Deactivate();

    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override;
    void Draw(sf::RenderTarget& target) override;
};
