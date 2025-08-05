#pragma once
#include <SFML/Graphics.hpp>

struct GUIState
{
    enum StateEnum { Default, Board, TaskCreation, UserLogin };
    StateEnum GetStateType() const { return state_; }

    virtual void ProcessLeftClickReleased() = 0;
    virtual void ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target) = 0;
    virtual void ReadUserInput(char c) = 0;
    virtual void Update(const sf::RenderWindow& window, const float deltaTime) = 0;
    virtual bool CheckCollision(sf::Vector2i point, sf::RenderWindow& target) = 0;
    virtual void Draw(sf::RenderWindow& window) = 0;
    virtual ~GUIState() {}
protected:
    const StateEnum state_;
    GUIState(StateEnum state) : state_(state) {}
};