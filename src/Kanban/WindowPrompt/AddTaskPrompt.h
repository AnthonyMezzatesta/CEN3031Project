#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "Subject.h"
#include "WindowPrompt.h"
#include "../GUIElement/TaskOption.h"
#include "../Board/Board.h"
#include "../GUIElement/ScrollableTexture.h"

class AddTaskPrompt final : public WindowPrompt, public EventSystem::TaskSubject
{
    sf::Font font_;
    sf::Text text_;
    sf::RectangleShape bg;
    sf::Color bgColor = sf::Color(128, 128, 128, 255);
    std::unordered_map<int, Kanban::TaskOption*> taskElements_;
    ScrollableTexture scrollTexture_;
    const static int tasksPerScreen_ = 5;

    sf::Transform transformDynamic_;

    void UpdateValues();
    void UpdateScrollTexture(const float deltaTime);
public:
    AddTaskPrompt(const sf::RenderWindow& target, Kanban::Board& board);
    ~AddTaskPrompt();

    void ClearTaskElements();

    void Update(const float deltaTime) override;
    void Deactivate();

    void ProcessLeftClickReleased() override;
    void ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target) override;
    bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) override;
    void Draw(sf::RenderTarget& target) override;
};
