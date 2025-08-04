#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "WindowResizeHandler.h"

using std::cout;
using std::endl;

class WindowPrompt
{
public:
    enum Type { Default, AddTaskPrompt, SettingsPrompt, TaskDetailsPrompt, ReminderPrompt, WindowResizePrompt };

    WindowPrompt(WindowResizeHandler& windowResizeHandler) : windowResizeObserver_(view_) {
        windowResizeHandler.AddObserver(windowResizeObserver_);
        font_.loadFromFile(Utilities::fontPath);
        text_.setFont(font_);
    }
    virtual ~WindowPrompt() {}

    virtual void Update(const float deltaTime) = 0;
    virtual void Draw(sf::RenderTarget& target) = 0;
    virtual void ProcessLeftClickReleased() {}
    virtual void ProcessMouseMove(sf::Vector2i pixelPos, sf::RenderWindow& target) {}
    virtual void ReadUserInput(char c) {}
    virtual void ProcessKeyEvent(const sf::Keyboard::Key key) {}
    virtual bool CheckCollision(sf::RenderWindow& target, sf::Vector2i point) = 0;

    Type GetType() const { return type_; }
    bool IsVisible() const { return isVisible; }
    bool IsActive() const { return isActive; }
    virtual void Deactivate() {
        isActive = false;
        isVisible = false;
    }
    virtual void Activate() { isActive = true; }
protected:
    sf::Font font_;
    sf::Text text_;
    Type type_ = Default;
    bool isVisible = false;
    bool isActive = false;
    sf::View view_;
    sf::FloatRect viewPortLeft_  = sf::FloatRect(0.0f, 0.0f, 0.5f, 0.5f);
    sf::FloatRect viewPortRight_ = sf::FloatRect(0.5f, 0.0f, 0.5f, 0.5f);
private:
    EventSystem::WindowResizeObserver windowResizeObserver_;
};