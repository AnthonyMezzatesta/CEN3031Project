#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "Subject.h"
#include "WindowPrompt.h"
#include "../GUIElement/TaskOption.h"
#include "../Board/Board.h"

class AddTaskPrompt final : public WindowPrompt, public EventSystem::TaskSubject
{
    sf::RectangleShape bg;
    sf::Color bgColor = sf::Color(128, 128, 128, 255);
    std::unordered_map<int, Kanban::TaskOption*> taskElements_;
public:
    AddTaskPrompt(const sf::RenderWindow& target, Kanban::Board& board);
    ~AddTaskPrompt();

    void ClearTaskElements();

    void Update() override;
    void Deactivate();

    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override;
    void Draw(sf::RenderTarget& target) override;
};